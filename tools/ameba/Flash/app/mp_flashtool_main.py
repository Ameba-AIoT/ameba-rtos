import os
import re
import sys
import time

from version_info import gui_version
from copy import deepcopy

from PySide6.QtCore import Qt, QTimer
from PySide6.QtGui import QFont, QGuiApplication, QIcon
from PySide6.QtWidgets import (
    QCheckBox, QComboBox, QFileDialog, QFrame, QGridLayout,
    QGroupBox, QHBoxLayout, QHeaderView, QLabel, QLineEdit, QMainWindow,
    QMenu, QMessageBox, QScrollArea, QStatusBar,
    QTableWidget, QTableWidgetItem, QTabWidget, QVBoxLayout, QWidget, QPushButton,
)

from base.json_utils import JsonUtils
from base.device_profile import RtkDeviceProfile
from base.rt_settings import RtSettings
from app.port_progress_widget import PortProgressWidget
from app.download_worker import DownloadWorker


class MPFlashToolMain(QMainWindow):

    # Startup-height model (see _compute_window_height):
    #   height = LIST_CHROME_HEIGHT + visible_ports * PORT_ROW_STRIDE
    PORT_ROW_STRIDE = 22       # px per port row (row height + inter-row spacing)
    LIST_CHROME_HEIGHT = 320   # px for tabs/config/header/buttons/status bar/margins
    DEFAULT_VISIBLE_PORTS = 20
    MIN_WINDOW_HEIGHT = 540    # floor: keep a normal-looking GUI for small port counts

    def __init__(self):
        super().__init__()

        self.tool_title = "Ameba MP Flash Tool"
        self.tool_title_cn = "Ameba 量产烧录工具"
        self._device_label = ""

        self.app_version = gui_version
        self.setWindowTitle(f"{self.tool_title} v{self.app_version}")
        # Height is driven by the GuiVisiblePorts setting so the Download tab's
        # port list can show that many rows without a scroll bar.
        self.setGeometry(200, 100, 720, self._compute_window_height())

        basedir = os.path.dirname(os.path.abspath(__file__))
        icon_path = os.path.join(basedir, "icon/mp_flashtool.ico")
        if os.path.exists(icon_path):
            self.setWindowIcon(QIcon(icon_path))

        self.create_menu_bar()

        main_widget = QWidget()
        self.setCentralWidget(main_widget)
        self.setup_main_content(main_widget)

        self.status_bar = QStatusBar()
        image_path = self.image_line_edit.text().strip()
        if image_path:
            self.status_bar.showMessage(image_path)
        else:
            self.status_bar.showMessage("No image selected")
        self.setStatusBar(self.status_bar)

        self._saved_ports = None
        self._active_workers = 0
        self._port_rows = []       # list of (device_str, QCheckBox, QLabel, PortProgressWidget)
        self._total_timer = None   # QTimer for overall download elapsed time
        self._total_start = 0.0
        self._port_timers = {}     # {row_index: QTimer}
        self._port_starts = {}     # {row_index: start_time}
        self._log_file = None
        self.setup_connections()
        self.load_settings()
        self.scan_and_populate_devices(filter_ports=self._saved_ports)
        self._init_log_file()

    def create_menu_bar(self):
        pass

    def setup_main_content(self, parent_widget):
        # Main QTabWidget — Layout + Download + Configuration
        self.main_tab_widget = QTabWidget()

        # ── Layout Tab (device profile & image layout) ────────────────────────
        profile_tab = QWidget()
        self._build_profile_selection_tab(profile_tab)
        self.main_tab_widget.addTab(profile_tab, "Layout")

        # ── Download Tab (flash download settings) ────────────────────────────
        common_tab = QWidget()
        self._build_common_tab(common_tab)
        self.main_tab_widget.addTab(common_tab, "Download")

        # ── Configuration Tab (Post Process & FlashProtectionProcess) ─────────
        options_tab = QWidget()
        self._build_options_tab(options_tab)
        self.main_tab_widget.addTab(options_tab, "Configuration")

        # Default to Download tab, but disable its content until a profile is loaded.
        self.main_tab_widget.setCurrentIndex(1)
        self._download_container.setEnabled(False)

        parent_layout = QVBoxLayout(parent_widget)
        parent_layout.addWidget(self.main_tab_widget)

    def _build_profile_selection_tab(self, tab_widget):
        """Build the Profile tab — label, line edit, Browse button, and image layout table."""
        layout = QVBoxLayout(tab_widget)
        layout.setContentsMargins(20, 25, 20, 10)
        layout.setSpacing(12)

        self.profile_line_edit = QLineEdit()
        self.profile_line_edit.setPlaceholderText("Select a device profile file (.rdev)...")
        self.profile_browse_button = QPushButton("Browse")

        row_layout = QHBoxLayout()
        self._lbl_profile = QLabel("Profile:")
        row_layout.addWidget(self._lbl_profile)
        row_layout.addWidget(self.profile_line_edit, 1)
        row_layout.addWidget(self.profile_browse_button)
        layout.addLayout(row_layout)

        layout.addSpacing(10)

        # ── Image Layout Table ─────────────────────────────────────────
        self._lbl_image_layout = QLabel("Layout:")
        layout.addWidget(self._lbl_image_layout)
        self.profile_image_table = QTableWidget()
        self.profile_image_table.setColumnCount(6)
        self.profile_image_table.setHorizontalHeaderLabels(
            ["", "Image Name", "Start Addr (Hex)", "End Addr (Hex)", "Memory Type", "Full Erase"]
        )
        self.profile_image_table.horizontalHeader().setSectionResizeMode(
            0, QHeaderView.ResizeMode.ResizeToContents
        )
        self.profile_image_table.horizontalHeader().setSectionResizeMode(
            1, QHeaderView.ResizeMode.Stretch
        )
        self.profile_image_table.horizontalHeader().setSectionResizeMode(
            2, QHeaderView.ResizeMode.Stretch
        )
        self.profile_image_table.horizontalHeader().setSectionResizeMode(
            3, QHeaderView.ResizeMode.Stretch
        )
        self.profile_image_table.horizontalHeader().setSectionResizeMode(
            4, QHeaderView.ResizeMode.Stretch
        )
        self.profile_image_table.horizontalHeader().setSectionResizeMode(
            5, QHeaderView.ResizeMode.ResizeToContents
        )
        self.profile_image_table.verticalHeader().setVisible(False)
        self.profile_image_table.verticalHeader().setDefaultSectionSize(28)
        self.profile_image_table.setEditTriggers(QTableWidget.EditTrigger.NoEditTriggers)
        self.profile_image_table.horizontalHeader().setStretchLastSection(False)
        layout.addWidget(self.profile_image_table, 1)

        layout.addStretch()

    def _build_common_tab(self, tab_widget):
        """Build the Download tab — config grid on left, checkboxes on right."""
        self._download_container = QGroupBox()
        self._download_container.setTitle("")
        main_layout = QVBoxLayout(self._download_container)
        main_layout.setContentsMargins(0, 0, 0, 0)

        top_section_layout = QHBoxLayout()
        top_section_layout.addLayout(self.create_config_layout(), 2)
        top_section_layout.addWidget(self.create_download_options_widget(), 1, Qt.AlignmentFlag.AlignTop)
        main_layout.addLayout(top_section_layout)
        main_layout.addSpacing(25)

        main_layout.addLayout(self.create_device_list_layout())

        # Bottom buttons
        bottom_layout = QHBoxLayout()
        self.scan_button = QPushButton("Scan Devices")
        self.download_button = QPushButton("Download")

        left_btn = QHBoxLayout()
        left_btn.addStretch()
        left_btn.addWidget(self.scan_button)
        left_btn.addStretch()

        right_btn = QHBoxLayout()
        right_btn.addStretch()
        right_btn.addWidget(self.download_button)
        right_btn.addStretch()

        bottom_layout.addLayout(left_btn)
        bottom_layout.addLayout(right_btn)
        main_layout.addLayout(bottom_layout)

        tab_layout = QVBoxLayout(tab_widget)
        tab_layout.addWidget(self._download_container)

    def create_config_layout(self):
        v_layout = QVBoxLayout()
        grid = QGridLayout()
        grid.setContentsMargins(0, 5, 0, 0)
        grid.setSpacing(8)

        baud_rates = [
            "115200", "128000", "153600", "230400", "380400",
            "460800", "500000", "921600", "1000000", "1382400",
            "1444400", "1500000", "2000000", "3000000",
        ]
        self.baudrate_combo = QComboBox()
        self.baudrate_combo.addItems(baud_rates)
        self.baudrate_combo.setCurrentText("1500000")
        self.baudrate_combo.setMaxVisibleItems(len(baud_rates))

        self._lbl_baudrate = QLabel("Baudrate:")
        grid.addWidget(self._lbl_baudrate, 0, 0, Qt.AlignmentFlag.AlignLeft)
        grid.addWidget(self.baudrate_combo, 0, 1)

        self.image_line_edit = QLineEdit()
        self.browse_button = QPushButton("Browse")
        self._lbl_image_dir = QLabel("Image/Dir:")
        grid.addWidget(self._lbl_image_dir, 1, 0, Qt.AlignmentFlag.AlignLeft)
        grid.addWidget(self.image_line_edit, 1, 1)
        grid.addWidget(self.browse_button, 1, 2)

        self.addr_hex_edit = QLineEdit("0x08000000")
        self._lbl_addr = QLabel("Addr (Hex):")
        grid.addWidget(self._lbl_addr, 2, 0, Qt.AlignmentFlag.AlignLeft)
        grid.addWidget(self.addr_hex_edit, 2, 1)

        self.len_hex_edit = QLineEdit("0x0")
        self._lbl_len = QLabel("Len (Hex):")
        grid.addWidget(self._lbl_len, 3, 0, Qt.AlignmentFlag.AlignLeft)
        grid.addWidget(self.len_hex_edit, 3, 1)

        v_layout.addLayout(grid)
        return v_layout

    def create_download_options_widget(self):
        """Build the right-side options widget for the Download tab."""
        widget = QWidget()
        layout = QVBoxLayout(widget)
        layout.setContentsMargins(25, 5, 0, 0)
        layout.setSpacing(12)
        self.chip_erase_checkbox = QCheckBox("Chip Erase")
        layout.addWidget(self.chip_erase_checkbox)
        self.usb_download_checkbox = QCheckBox("USB Download")
        layout.addWidget(self.usb_download_checkbox)
        self.verify_checkbox = QCheckBox("Verification")
        self.verify_checkbox.setVisible(False)
        layout.addWidget(self.verify_checkbox)
        layout.addStretch()
        return widget

    def _build_options_tab(self, tab_widget):
        """Build the Options tab — left column with settings, right column with language toggle."""
        tab_layout = QVBoxLayout(tab_widget)
        tab_layout.setContentsMargins(20, 25, 20, 10)
        tab_layout.setSpacing(12)

        top_section = QHBoxLayout()

        # ── Left column: checkboxes + combos ────────────────────────────
        left_layout = QVBoxLayout()
        left_layout.setSpacing(12)

        self.factory_mode_checkbox = QCheckBox("Factory Mode")
        left_layout.addWidget(self.factory_mode_checkbox)

        self.auto_program_spic_addr_mode_4byte_checkbox = QCheckBox("Auto program 4-byte address mode")
        self.auto_program_spic_addr_mode_4byte_checkbox.setChecked(True)
        left_layout.addWidget(self.auto_program_spic_addr_mode_4byte_checkbox)

        self.auto_switch_to_download_mode_with_dtr_rts_checkbox = QCheckBox("Auto enter download mode with DTR/RTS")
        self.auto_switch_to_download_mode_with_dtr_rts_checkbox.setChecked(True)
        left_layout.addWidget(self.auto_switch_to_download_mode_with_dtr_rts_checkbox)

        self.auto_reset_device_with_dtr_rts_checkbox = QCheckBox("Auto reset device with DTR/RTS")
        self.auto_reset_device_with_dtr_rts_checkbox.setChecked(True)
        left_layout.addWidget(self.auto_reset_device_with_dtr_rts_checkbox)

        # left_layout.addSpacing(8)

        # ── Save Log + Log Level ───────────────────────────────────────────
        self.save_log_checkbox = QCheckBox("Save Log")
        self.save_log_checkbox.setChecked(True)
        left_layout.addWidget(self.save_log_checkbox)

        self._lbl_log_level = QLabel("Log Level:")
        left_layout.addWidget(self._lbl_log_level)
        self.log_level_combo = QComboBox()
        self._log_level_items = [
            ("Fatal", "fatal"),
            ("Error", "error"),
            ("Warning", "warning"),
            ("Info", "info"),
            ("Debug", "debug"),
        ]
        for text, data in self._log_level_items:
            self.log_level_combo.addItem(text, userData=data)
        self.log_level_combo.setCurrentIndex(
            self.log_level_combo.findData("info")
        )
        self.log_level_combo.setMaximumWidth(280)
        left_layout.addWidget(self.log_level_combo)

        self._lbl_post_process = QLabel("Post Process:")
        left_layout.addWidget(self._lbl_post_process)
        self.post_process_combo = QComboBox()
        self._post_process_items = [
            ("None", "NONE"),
            ("Indicate download result", "INDICATION"),
            ("Reset device", "RESET"),
            ("Boot", "BOOT"),
        ]
        for text, data in self._post_process_items:
            self.post_process_combo.addItem(text, userData=data)
        self.post_process_combo.setCurrentIndex(
            self.post_process_combo.findData("RESET")
        )
        self.post_process_combo.setMaximumWidth(280)
        left_layout.addWidget(self.post_process_combo)

        self._lbl_flash_protection = QLabel("Flash Protection Process:")
        left_layout.addWidget(self._lbl_flash_protection)
        self.flash_protection_combo = QComboBox()
        self._flash_protection_items = [
            ("Try program", RtSettings.FLASH_PROTECTION_PROCESS_TRY),
            ("Unlock and program", RtSettings.FLASH_PROTECTION_PROCESS_UNLOCK),
            ("Abort", RtSettings.FLASH_PROTECTION_PROCESS_ABORT),
        ]
        for text, data in self._flash_protection_items:
            self.flash_protection_combo.addItem(text, userData=data)
        self.flash_protection_combo.setCurrentIndex(
            self.flash_protection_combo.findData(RtSettings.FLASH_PROTECTION_PROCESS_UNLOCK)
        )
        self.flash_protection_combo.setMaximumWidth(280)
        left_layout.addWidget(self.flash_protection_combo)

        top_section.addLayout(left_layout)
        top_section.addStretch()

        # ── Right column: language toggle (top-aligned) ─────────────────
        right_layout = QVBoxLayout()
        right_layout.setContentsMargins(40, 0, 0, 0)
        self.lang_checkbox = QCheckBox("中文")
        self.lang_checkbox.setChecked(False)
        right_layout.addWidget(self.lang_checkbox)
        right_layout.addStretch()

        top_section.addLayout(right_layout)

        tab_layout.addLayout(top_section)
        tab_layout.addStretch()

    def create_device_list_layout(self):
        root_layout = QVBoxLayout()

        # Header row: "Check: All None" on left, "Progress" label on right
        header_layout = QHBoxLayout()
        self._lbl_check = QLabel("Check :")
        header_layout.addWidget(self._lbl_check)
        self.all_label = QLabel("<a href='#'>All</a>")
        self.none_label = QLabel("<a href='#'>None</a>")
        header_layout.addWidget(self.all_label)
        header_layout.addWidget(self.none_label)
        header_layout.addStretch()
        self.total_time_label = QLabel("Total: --:--")
        header_layout.addWidget(self.total_time_label)
        header_layout.addSpacing(20)
        root_layout.addLayout(header_layout)

        # Scroll area containing one row-widget per port.
        # Each row: [QCheckBox (stretch 1)] [timer fixed 42px] [PortProgressWidget fixed 200px]
        self._port_rows_container = QWidget()
        self._port_rows_layout = QVBoxLayout(self._port_rows_container)
        self._port_rows_layout.setSpacing(3)
        self._port_rows_layout.setContentsMargins(4, 4, 4, 4)
        self._port_rows_layout.addStretch()

        scroll = QScrollArea()
        scroll.setWidgetResizable(True)
        scroll.setWidget(self._port_rows_container)
        scroll.setFrameShape(QFrame.Shape.StyledPanel)
        scroll.setFocusPolicy(Qt.FocusPolicy.NoFocus)

        root_layout.addWidget(scroll)
        return root_layout

    def setup_connections(self):
        self.scan_button.clicked.connect(lambda: self.scan_and_populate_devices())
        self.download_button.clicked.connect(lambda: self.start_download())
        self.browse_button.clicked.connect(self.browse_image_file)
        self.profile_browse_button.clicked.connect(self.open_profile_file)
        self.factory_mode_checkbox.stateChanged.connect(
            lambda state: self._apply_factory_mode(state == Qt.CheckState.Checked.value)
        )
        self.save_log_checkbox.stateChanged.connect(
            lambda state: self._on_save_log_changed(state == Qt.CheckState.Checked.value)
        )
        self.lang_checkbox.stateChanged.connect(
            lambda state: self._toggle_language(state == Qt.CheckState.Checked.value)
        )
        self.all_label.linkActivated.connect(self.check_all_devices)
        self.none_label.linkActivated.connect(self.uncheck_all_devices)

    # ── Port list population ───────────────────────────────────────────────

    def scan_and_populate_devices(self, filter_ports=None):
        """扫描串口设备并填充列表。

        filter_ports: 若提供，则只显示其中在当前扫描结果内的端口（用于启动时恢复）。
                      为 None 时不过滤，显示全部扫描结果。
        """
        try:
            import serial.tools.list_ports
            all_ports = serial.tools.list_ports.comports()
        except ImportError:
            self.status_bar.showMessage("Error: pyserial not installed.")
            return

        RTK_USB_VID = 0x0BDA  # 与 download_handler.py 保持一致

        use_usb = self.usb_download_checkbox.isChecked()
        if use_usb:
            available_ports = [p for p in all_ports if p.vid == RTK_USB_VID]
            port_type = "USB"
        else:
            available_ports = [p for p in all_ports if p.vid != RTK_USB_VID]
            port_type = "UART"

        if filter_ports is not None:
            allowed = set(filter_ports)
            available_ports = [p for p in available_ports if p.device in allowed]

        # Clear existing rows (preserve the trailing stretch item)
        while self._port_rows_layout.count() > 1:
            item = self._port_rows_layout.takeAt(0)
            if item.widget():
                item.widget().deleteLater()
        self._port_rows = []

        for port_info in available_ports:
            desc = port_info.description or ""
            # Strip trailing " (COMxx)" that duplicates the device name.
            desc = re.sub(r'\s*\(COM\d+\)\s*$', '', desc, flags=re.IGNORECASE).strip()
            label = (
                f"{port_info.device}  ({desc})"
                if desc and desc.lower() != "n/a"
                else port_info.device
            )

            row_widget = QWidget()
            # Smaller font so 20 rows fit in the default window height without scrolling.
            row_font = QFont(row_widget.font())
            row_font.setPointSizeF(8)
            row_widget.setFont(row_font)
            row_layout = QHBoxLayout(row_widget)
            row_layout.setContentsMargins(4, 0, 4, 0)

            checkbox = QCheckBox(label)
            checkbox.setFont(row_font)
            timer_label = QLabel("--:--")
            timer_label.setFont(row_font)
            timer_label.setFixedWidth(42)
            timer_label.setAlignment(Qt.AlignmentFlag.AlignCenter)
            progress = PortProgressWidget()
            progress.setFont(row_font)
            progress.setFixedWidth(200)
            progress.setFixedHeight(16)

            row_layout.addWidget(checkbox, 1)
            row_layout.addWidget(timer_label)
            row_layout.addWidget(progress)

            # Insert before the trailing stretch
            self._port_rows_layout.insertWidget(
                self._port_rows_layout.count() - 1, row_widget
            )
            self._port_rows.append((port_info.device, checkbox, timer_label, progress))

        self.status_bar.showMessage(
            self.image_line_edit.text().strip() or "No image selected"
        )

    def check_all_devices(self, _=None):
        for _, cb, _, _ in self._port_rows:
            cb.setChecked(True)

    def uncheck_all_devices(self, _=None):
        for _, cb, _, _ in self._port_rows:
            cb.setChecked(False)

    # ── Download ───────────────────────────────────────────────────────────

    def start_download(self):
        if not getattr(self, "profile_info", None):
            QMessageBox.warning(self, "Missing Profile", "Please load a device profile first.")
            return

        image_path = self.image_line_edit.text().strip()
        if not image_path or (not os.path.isfile(image_path) and not os.path.isdir(image_path)):
            QMessageBox.warning(self, "Missing Image", "Please select a valid image file or directory.")
            return

        try:
            start_address = int(self.addr_hex_edit.text(), 16)
        except ValueError:
            QMessageBox.warning(self, "Invalid Address", "Start address is not a valid hex value.")
            return

        checked_rows = [
            (i, device)
            for i, (device, cb, _, _) in enumerate(self._port_rows)
            if cb.isChecked()
        ]

        if not checked_rows:
            QMessageBox.warning(self, "No Device", "Please select at least one device.")
            return

        # Build RtSettings from file then override with GUI selections
        settings = self._build_settings()

        baudrate = int(self.baudrate_combo.currentText())
        chip_erase = self.chip_erase_checkbox.isChecked()

        self.download_button.setEnabled(False)
        self.scan_button.setEnabled(False)
        self._active_workers = len(checked_rows)

        # Start total elapsed timer
        self._total_start = time.monotonic()
        self._total_timer = QTimer(self)
        self._total_timer.timeout.connect(self._update_total_timer)
        self._total_timer.start(500)
        prefix = "Total: " if not self.lang_checkbox.isChecked() else "总计："
        self.total_time_label.setText(f"{prefix}00:00")

        # Stop any leftover per-port timers from a previous run
        for t in self._port_timers.values():
            t.stop()
        self._port_timers.clear()
        self._port_starts.clear()

        for row, port in checked_rows:
            _, _, timer_lbl, prog = self._port_rows[row]
            timer_lbl.setText("00:00")
            prog.reset()

            # Per-port elapsed timer
            self._port_starts[row] = time.monotonic()
            pt = QTimer(self)
            pt.timeout.connect(lambda r=row: self._update_port_timer(r))
            pt.start(500)
            self._port_timers[row] = pt

            worker = DownloadWorker(
                port=port,
                baudrate=baudrate,
                profile_info=self.profile_info,
                settings=deepcopy(settings),
                image_path=image_path,
                start_address=start_address,
                chip_erase=chip_erase,
                log_file=self._log_file,
                log_level=settings.log_level,
                parent=self,
            )
            worker.progress_updated.connect(
                lambda pct, r=row: self._on_download_progress(r, pct)
            )
            worker.finished.connect(
                lambda ok, msg, r=row: self._on_download_finished(r, ok, msg)
            )
            worker.start()

    def _load_rt_settings(self) -> RtSettings:
        """Load Settings.json into an RtSettings object (empty defaults if file absent)."""
        path = self._settings_path()
        try:
            if os.path.exists(path):
                dt = JsonUtils.load_from_file(path, need_decrypt=False)
                return RtSettings(**(dt or {}))
        except Exception:
            pass
        return RtSettings(**{})

    def _build_settings(self) -> RtSettings:
        """Build an RtSettings for a download run: load from file, override session fields."""
        settings = self._load_rt_settings()

        settings.post_process = self.post_process_combo.currentData() or "NONE"

        settings.flash_protection_process = self.flash_protection_combo.currentData() or RtSettings.FLASH_PROTECTION_PROCESS_UNLOCK

        settings.log_level = self.log_level_combo.currentData() or "info"
        settings.save_log = self.save_log_checkbox.isChecked()
        settings.auto_program_spic_addr_mode_4byte = int(self.auto_program_spic_addr_mode_4byte_checkbox.isChecked())
        settings.auto_switch_to_download_mode_with_dtr_rts = int(self.auto_switch_to_download_mode_with_dtr_rts_checkbox.isChecked())
        settings.auto_reset_device_with_dtr_rts = int(self.auto_reset_device_with_dtr_rts_checkbox.isChecked())

        return settings

    def _on_download_progress(self, row: int, pct: int):
        if 0 <= row < len(self._port_rows):
            self._port_rows[row][3].set_progress(pct)

    def _on_download_finished(self, row: int, success: bool, msg: str):
        if 0 <= row < len(self._port_rows):
            self._port_rows[row][3].set_done(success, msg)
            # Stop per-port timer; freeze label at final elapsed time
            if row in self._port_timers:
                self._port_timers[row].stop()
                del self._port_timers[row]
            self._update_port_timer(row)  # one last tick to show final value

        self._active_workers -= 1
        if self._active_workers <= 0:
            self._active_workers = 0
            if self._total_timer:
                self._total_timer.stop()
                self._total_timer = None
            self._update_total_timer()  # one last tick
            self.download_button.setEnabled(True)
            self.scan_button.setEnabled(True)
            self.status_bar.showMessage("Download finished.")

    def _update_total_timer(self):
        elapsed = int(time.monotonic() - self._total_start)
        m, s = divmod(elapsed, 60)
        prefix = "Total: " if not self.lang_checkbox.isChecked() else "总计："
        self.total_time_label.setText(f"{prefix}{m:02d}:{s:02d}")

    def _update_port_timer(self, row: int):
        if row not in self._port_starts:
            return
        elapsed = int(time.monotonic() - self._port_starts[row])
        m, s = divmod(elapsed, 60)
        if 0 <= row < len(self._port_rows):
            self._port_rows[row][2].setText(f"{m:02d}:{s:02d}")

    # ── File / profile helpers ─────────────────────────────────────────────

    def browse_image_file(self):
        menu = QMenu(self)
        file_action = menu.addAction("Select Image File (.bin)...")
        dir_action  = menu.addAction("Select Image Directory...")
        action = menu.exec(
            self.browse_button.mapToGlobal(self.browse_button.rect().bottomLeft())
        )

        if action == file_action:
            file_path, _ = QFileDialog.getOpenFileName(
                self, "Select Image File", "", "Binary Files (*.bin);;All Files (*)"
            )
            if file_path:
                self.image_line_edit.setText(file_path)
                self.len_hex_edit.setText(f"0x{os.path.getsize(file_path):X}")

        elif action == dir_action:
            dir_path = QFileDialog.getExistingDirectory(
                self, "Select Image Directory", ""
            )
            if dir_path:
                self.image_line_edit.setText(dir_path)
                self.len_hex_edit.setText("0x0")

    def open_profile_file(self):
        file_path, _ = QFileDialog.getOpenFileName(
            self, "Select Device Profile", "", "Device Profile (*.rdev);;All Files (*)"
        )
        if not file_path:
            return
        error = self._apply_profile_from_path(file_path)
        if error:
            QMessageBox.critical(self, "Load Profile Failed", error)
            return
        # Clear image-related fields — they may not be compatible with the new profile.
        self.image_line_edit.clear()
        self.len_hex_edit.setText("0x0")

    @staticmethod
    def _memory_type_str(memory_type):
        mapping = {
            0: "RAM",
            1: "NOR",
            2: "NAND",
            3: "VENDOR",
            4: "HYBRID",
        }
        return mapping.get(memory_type, str(memory_type))

    def _update_window_title(self):
        """Set the window title based on current language and device label."""
        cn_mode = self.lang_checkbox.isChecked()
        title = self.tool_title_cn if cn_mode else self.tool_title
        if self._device_label:
            self.setWindowTitle(f"{title} v{self.app_version} ({self._device_label})")
        else:
            self.setWindowTitle(f"{title} v{self.app_version}")

    def _toggle_language(self, cn_mode):
        """Toggle all UI text between Chinese and English."""
        # ── Tab titles ────────────────────────────────────────────────────
        tab_titles = {
            0: ("布局", "Layout"),
            1: ("下载", "Download"),
            2: ("配置", "Configuration"),
        }
        for idx, (cn, en) in tab_titles.items():
            self.main_tab_widget.setTabText(idx, cn if cn_mode else en)

        # ── Window title ────────────────────────────────────────────────────
        self._update_window_title()

        # ── Layout tab ────────────────────────────────────────────────────
        self._lbl_profile.setText("配置文件：" if cn_mode else "Profile:")
        self._lbl_image_layout.setText("布局：" if cn_mode else "Layout:")
        self.profile_browse_button.setText("浏览" if cn_mode else "Browse")
        self.profile_line_edit.setPlaceholderText(
            "选择设备配置文件 (.rdev)..." if cn_mode else "Select a device profile file (.rdev)..."
        )

        table_headers = [
            ("", ""),
            ("固件名", "Image Name"),
            ("起始地址 (Hex)", "Start Addr (Hex)"),
            ("结束地址 (Hex)", "End Addr (Hex)"),
            ("存储类型", "Memory Type"),
            ("全片擦除", "Full Erase"),
        ]
        for col, (cn, en) in enumerate(table_headers):
            item = self.profile_image_table.horizontalHeaderItem(col)
            if item:
                item.setText(cn if cn_mode else en)

        # ── Download tab ──────────────────────────────────────────────────
        self._lbl_baudrate.setText(" 波特率：" if cn_mode else " Baudrate:")
        self._lbl_image_dir.setText(" 固件/目录：" if cn_mode else " Image/Dir:")
        self.browse_button.setText("浏览" if cn_mode else "Browse")
        self._lbl_addr.setText(" 地址 (Hex)：" if cn_mode else " Addr (Hex):")
        self._lbl_len.setText(" 长度 (Hex)：" if cn_mode else " Len (Hex):")

        self.chip_erase_checkbox.setText("全片擦除" if cn_mode else "Chip Erase")
        self.usb_download_checkbox.setText("USB 下载" if cn_mode else "USB Download")
        self.scan_button.setText("扫描设备" if cn_mode else "Scan Devices")
        self.download_button.setText("下载" if cn_mode else "Download")

        self._lbl_check.setText("选择：" if cn_mode else "Check :")
        self.all_label.setText("<a href='#'>全选</a>" if cn_mode else "<a href='#'>All</a>")
        self.none_label.setText("<a href='#'>取消</a>" if cn_mode else "<a href='#'>None</a>")
        self.total_time_label.setText("总计：--:--" if cn_mode else "Total: --:--")

        # ── Configuration tab ─────────────────────────────────────────────
        self.factory_mode_checkbox.setText("工厂模式" if cn_mode else "Factory Mode")
        self.auto_program_spic_addr_mode_4byte_checkbox.setText(
            "自动编程 4 字节地址模式" if cn_mode else "Auto program 4-byte address mode"
        )
        self.auto_switch_to_download_mode_with_dtr_rts_checkbox.setText(
            "使用 DTR/RTS 自动进入下载模式" if cn_mode else "Auto enter download mode with DTR/RTS"
        )
        self.auto_reset_device_with_dtr_rts_checkbox.setText(
            "使用 DTR/RTS 自动复位设备" if cn_mode else "Auto reset device with DTR/RTS"
        )
        self.save_log_checkbox.setText("保存日志" if cn_mode else "Save Log")

        self._lbl_log_level.setText("日志级别：" if cn_mode else "Log Level:")
        self._lbl_post_process.setText("后处理：" if cn_mode else "Post Process:")
        self._lbl_flash_protection.setText("Flash 写保护处理：" if cn_mode else "Flash Protection Process:")

        # ── Combo items ───────────────────────────────────────────────────
        self._update_combo_items(self.log_level_combo, cn_mode, [
            ("致命", "Fatal", "fatal"),
            ("错误", "Error", "error"),
            ("警告", "Warning", "warning"),
            ("信息", "Info", "info"),
            ("调试", "Debug", "debug"),
        ])
        self._update_combo_items(self.post_process_combo, cn_mode, [
            ("无", "None", "NONE"),
            ("指示下载结果", "Indicate download result", "INDICATION"),
            ("复位设备", "Reset device", "RESET"),
            ("启动", "Boot", "BOOT"),
        ])
        self._update_combo_items(self.flash_protection_combo, cn_mode, [
            ("尝试烧录", "Try program", RtSettings.FLASH_PROTECTION_PROCESS_TRY),
            ("解锁并烧录", "Unlock and program", RtSettings.FLASH_PROTECTION_PROCESS_UNLOCK),
            ("中止", "Abort", RtSettings.FLASH_PROTECTION_PROCESS_ABORT),
        ])

    @staticmethod
    def _update_combo_items(combo, cn_mode, triples):
        """Update combo box items to Chinese or English, preserving userData selection.

        triples: list of (cn_text, en_text, user_data)
        """
        current_data = combo.currentData()
        combo.blockSignals(True)
        combo.clear()
        for cn, en, data in triples:
            combo.addItem(cn if cn_mode else en, userData=data)
        # Restore current selection by userData
        if current_data is not None:
            idx = combo.findData(current_data)
            if idx >= 0:
                combo.setCurrentIndex(idx)
        combo.blockSignals(False)

    def _apply_profile_from_path(self, file_path):
        try:
            profile_json = JsonUtils.load_from_file(file_path)
            if profile_json is None:
                return "Failed to load profile file"
            self.profile_info = RtkDeviceProfile(**profile_json)
        except Exception as e:
            return str(e)
        self.current_profile_path = file_path
        self.profile_line_edit.setText(file_path)

        # ── Populate image layout table ────────────────────────────────
        self.profile_image_table.setRowCount(len(self.profile_info.images))
        for i, image in enumerate(self.profile_info.images):
            # Mandatory checkbox (centered)
            cb = QCheckBox()
            cb.setChecked(image.mandatory)
            cb.setEnabled(False)
            cb_wrapper = QWidget()
            cb_layout = QHBoxLayout(cb_wrapper)
            cb_layout.addWidget(cb, 0, Qt.AlignmentFlag.AlignCenter)
            cb_layout.setContentsMargins(0, 0, 0, 0)
            self.profile_image_table.setCellWidget(i, 0, cb_wrapper)

            self.profile_image_table.setItem(i, 1, QTableWidgetItem(image.image_name))
            self.profile_image_table.setItem(
                i, 2, QTableWidgetItem(f"0x{image.start_address:08X}")
            )
            self.profile_image_table.setItem(
                i, 3, QTableWidgetItem(f"0x{image.end_address:08X}")
            )
            self.profile_image_table.setItem(
                i, 4, QTableWidgetItem(self._memory_type_str(image.memory_type))
            )
            # Full Erase checkbox (centered)
            fe_cb = QCheckBox()
            fe_cb.setChecked(image.full_erase)
            fe_cb.setEnabled(False)
            fe_wrapper = QWidget()
            fe_layout = QHBoxLayout(fe_wrapper)
            fe_layout.addWidget(fe_cb, 0, Qt.AlignmentFlag.AlignCenter)
            fe_layout.setContentsMargins(0, 0, 0, 0)
            self.profile_image_table.setCellWidget(i, 5, fe_wrapper)
        support_usb = self.profile_info.support_usb_download
        self.usb_download_checkbox.setEnabled(support_usb)
        if not support_usb:
            self.usb_download_checkbox.setChecked(False)
        filename_stem = os.path.splitext(os.path.basename(file_path))[0]
        self._device_label = filename_stem.split("_")[0]
        self._update_window_title()
        self.status_bar.showMessage(f"Profile: {file_path}")
        self._download_container.setEnabled(True)
        return None

    # ── Settings persistence ───────────────────────────────────────────────

    def _init_log_file(self):
        if not self.save_log_checkbox.isChecked():
            self._log_file = None
            return
        if getattr(sys, 'frozen', False):
            base = os.path.dirname(sys.executable)
        else:
            base = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
        log_dir = os.path.join(base, "log")
        os.makedirs(log_dir, exist_ok=True)
        ts = time.strftime("%Y_%m_%d_%H_%M_%S")
        self._log_file = os.path.join(log_dir, f"{ts}.log")

    def _on_save_log_changed(self, checked):
        self.log_level_combo.setEnabled(checked)
        self._init_log_file()

    def _compute_window_height(self):
        """Startup window height from the GuiVisiblePorts setting.

        height = LIST_CHROME_HEIGHT + visible_ports * PORT_ROW_STRIDE, then:
          - floored at MIN_WINDOW_HEIGHT so a small count still shows a normal GUI,
          - capped at the available screen height so a huge count just maxes out.
        """
        visible = self.DEFAULT_VISIBLE_PORTS
        try:
            visible = max(1, int(self._load_rt_settings().gui_visible_ports))
        except Exception:
            pass

        height = self.LIST_CHROME_HEIGHT + visible * self.PORT_ROW_STRIDE

        # Lower clamp: don't shrink below a normal window.
        height = max(height, self.MIN_WINDOW_HEIGHT)

        # Upper clamp: never exceed the physical screen.
        screen = QGuiApplication.primaryScreen()
        if screen is not None:
            avail = screen.availableGeometry().height()
            height = min(height, avail - 40)
        return height

    def _settings_path(self):
        if getattr(sys, 'frozen', False):
            base = os.path.dirname(sys.executable)
        else:
            base = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
        return os.path.join(base, "Settings.json")

    def save_settings(self):
        # Load existing settings so RT fields written by the CLI are preserved.
        settings = self._load_rt_settings()

        # Update GUI fields on the RtSettings object
        settings.profile = getattr(self, "current_profile_path", "")
        settings.port = [device for device, _, _, _ in self._port_rows]
        settings.baudrate = int(self.baudrate_combo.currentText())
        image_path = self.image_line_edit.text()
        if os.path.isdir(image_path):
            settings.image_dir = image_path
            settings.image = ""
        else:
            settings.image = image_path
            settings.image_dir = ""
        try:
            settings.address = int(self.addr_hex_edit.text(), 16)
        except ValueError:
            pass
        try:
            settings.length = int(self.len_hex_edit.text(), 16)
        except ValueError:
            pass
        settings.factory_mode = self.factory_mode_checkbox.isChecked()
        settings.chip_erase = self.chip_erase_checkbox.isChecked()
        settings.usb_download = self.usb_download_checkbox.isChecked()

        settings.post_process = self.post_process_combo.currentData() or "NONE"

        settings.flash_protection_process = self.flash_protection_combo.currentData() or RtSettings.FLASH_PROTECTION_PROCESS_UNLOCK

        settings.log_level = self.log_level_combo.currentData() or "info"
        settings.save_log = self.save_log_checkbox.isChecked()
        settings.auto_program_spic_addr_mode_4byte = int(self.auto_program_spic_addr_mode_4byte_checkbox.isChecked())
        settings.auto_switch_to_download_mode_with_dtr_rts = int(self.auto_switch_to_download_mode_with_dtr_rts_checkbox.isChecked())
        settings.auto_reset_device_with_dtr_rts = int(self.auto_reset_device_with_dtr_rts_checkbox.isChecked())
        settings.gui_language_zh = self.lang_checkbox.isChecked()

        try:
            JsonUtils.save_to_file(self._settings_path(), settings.to_dict(), need_encrypt=False)
        except Exception:
            pass

    def load_settings(self):
        if not os.path.exists(self._settings_path()):
            return
        try:
            settings = self._load_rt_settings()
        except Exception:
            return

        profile_ok = bool(settings.profile and os.path.exists(settings.profile))
        if profile_ok:
            self._apply_profile_from_path(settings.profile)

        try:
            self.baudrate_combo.setCurrentText(str(int(settings.baudrate)))
        except (ValueError, TypeError):
            pass

        if profile_ok:
            if settings.image_dir:
                self.image_line_edit.setText(settings.image_dir)
                self.len_hex_edit.setText("0x0")
            else:
                self.image_line_edit.setText(settings.image)
                self.len_hex_edit.setText(f"0x{int(settings.length):X}")
            self.addr_hex_edit.setText(f"0x{int(settings.address):08X}")
        else:
            self.image_line_edit.clear()
            self.addr_hex_edit.setText("0x08000000")
            self.len_hex_edit.setText("0x0")

        self._saved_ports = settings.port if isinstance(settings.port, list) else None

        factory_mode = bool(settings.factory_mode)
        self.factory_mode_checkbox.setChecked(factory_mode)
        self._apply_factory_mode(factory_mode)
        self.chip_erase_checkbox.setChecked(bool(settings.chip_erase))
        self.usb_download_checkbox.setChecked(bool(settings.usb_download))

        idx = self.post_process_combo.findData(settings.post_process)
        if idx >= 0:
            self.post_process_combo.setCurrentIndex(idx)

        idx = self.flash_protection_combo.findData(settings.flash_protection_process)
        if idx >= 0:
            self.flash_protection_combo.setCurrentIndex(idx)

        idx = self.log_level_combo.findData(settings.log_level)
        if idx >= 0:
            self.log_level_combo.setCurrentIndex(idx)
        self.save_log_checkbox.setChecked(bool(settings.save_log))
        self.auto_program_spic_addr_mode_4byte_checkbox.setChecked(
            bool(settings.auto_program_spic_addr_mode_4byte)
        )
        self.auto_switch_to_download_mode_with_dtr_rts_checkbox.setChecked(
            bool(settings.auto_switch_to_download_mode_with_dtr_rts)
        )
        self.auto_reset_device_with_dtr_rts_checkbox.setChecked(
            bool(settings.auto_reset_device_with_dtr_rts)
        )

        cn_mode = bool(settings.gui_language_zh)
        self.lang_checkbox.setChecked(cn_mode)
        self._toggle_language(cn_mode)

    def _apply_factory_mode(self, enabled):
        configurable = not enabled
        self.profile_line_edit.setEnabled(configurable)
        self.profile_browse_button.setEnabled(configurable)
        self.post_process_combo.setEnabled(configurable)
        self.flash_protection_combo.setEnabled(configurable)
        self.log_level_combo.setEnabled(configurable)
        self.save_log_checkbox.setEnabled(configurable)
        self.auto_program_spic_addr_mode_4byte_checkbox.setEnabled(configurable)
        self.auto_switch_to_download_mode_with_dtr_rts_checkbox.setEnabled(configurable)
        self.auto_reset_device_with_dtr_rts_checkbox.setEnabled(configurable)
        self.baudrate_combo.setEnabled(configurable)
        self.addr_hex_edit.setEnabled(configurable)
        self.len_hex_edit.setEnabled(configurable)
        self.factory_mode_checkbox.setEnabled(configurable)
        self.chip_erase_checkbox.setEnabled(configurable)
        self.verify_checkbox.setEnabled(configurable)
        if enabled:
            self.usb_download_checkbox.setEnabled(False)
        else:
            support_usb = getattr(
                getattr(self, "profile_info", None), "support_usb_download", False
            )
            self.usb_download_checkbox.setEnabled(support_usb)

    def closeEvent(self, event):
        self.save_settings()
        event.accept()
