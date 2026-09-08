import concurrent.futures

from PySide6.QtCore import QThread, Signal

from base.golden_board_detector import query_golden_board


class DetectLayoutWorker(QThread):
    layout_detected = Signal(dict, dict)  # {port: slot_id}, {port: error}

    def __init__(self, ports, profile_info, settings, id_address, baudrate, parent=None):
        super().__init__(parent)
        self.ports = list(ports)
        self.profile_info = profile_info
        self.settings = settings
        self.id_address = int(id_address)
        self.baudrate = int(baudrate)

    def run(self):
        slot_map = {}
        errors = {}
        if not self.ports:
            self.layout_detected.emit(slot_map, errors)
            return

        max_workers = min(len(self.ports), 20)
        with concurrent.futures.ThreadPoolExecutor(max_workers=max_workers) as executor:
            futures = {
                executor.submit(
                    query_golden_board,
                    port,
                    self.profile_info,
                    self.settings,
                    self.id_address,
                    self.baudrate,
                ): port
                for port in self.ports
            }
            for future in concurrent.futures.as_completed(futures):
                port = futures[future]
                try:
                    slot_id, error = future.result()
                except Exception as err:
                    slot_id, error = None, str(err)
                if slot_id is None:
                    errors[port] = error or "unknown golden-board ID"
                else:
                    slot_map[port] = slot_id

        self.layout_detected.emit(slot_map, errors)
