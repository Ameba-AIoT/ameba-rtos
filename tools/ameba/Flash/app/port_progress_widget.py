from PySide6.QtCore import Qt
from PySide6.QtWidgets import QHBoxLayout, QProgressBar, QWidget


class PortProgressWidget(QWidget):
    def __init__(self, parent=None):
        super().__init__(parent)
        layout = QHBoxLayout(self)
        layout.setContentsMargins(4, 0, 4, 0)

        self.bar = QProgressBar()
        self.bar.setRange(0, 100)
        self.bar.setValue(0)
        self.bar.setFormat("Idle")
        self.bar.setTextVisible(True)
        self.bar.setAlignment(Qt.AlignmentFlag.AlignCenter)
        layout.addWidget(self.bar)

    def reset(self):
        self.bar.setValue(0)
        self.bar.setFormat("0%")
        self.bar.setStyleSheet(
            "QProgressBar::chunk { background-color: #2196F3; }"
        )

    def set_progress(self, pct: int):
        self.bar.setValue(pct)
        self.bar.setFormat(f"{pct}%")
        self.bar.setStyleSheet(
            "QProgressBar::chunk { background-color: #2196F3; }"
        )

    def set_done(self, success: bool, error_msg: str = ""):
        if success:
            self.bar.setValue(100)
            self.bar.setFormat("PASS")
            self.bar.setStyleSheet(
                "QProgressBar::chunk { background-color: #4CAF50; }"
            )
            self.bar.setToolTip("")
        else:
            self.bar.setValue(100)
            self.bar.setFormat("FAIL")
            self.bar.setStyleSheet(
                "QProgressBar::chunk { background-color: #F44336; }"
            )
            self.bar.setToolTip(error_msg)
