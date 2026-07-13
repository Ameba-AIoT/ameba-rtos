import logging


class ProgressLogHandler(logging.Handler):
    def __init__(self, callback):
        super().__init__()
        self._cb = callback

    def emit(self, record):
        msg = record.getMessage()
        if "Programming progress:" in msg:
            try:
                pct = int(msg.split("Programming progress:")[1].strip().rstrip("%"))
                self._cb(pct)
            except (IndexError, ValueError):
                pass
