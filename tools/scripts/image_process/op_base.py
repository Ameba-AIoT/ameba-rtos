from abc import ABC, abstractmethod
from ameba_enums import *
from context import Context
from utility import pascal_to_snake

class BaseMeta(type):
    def __new__(cls, name, bases, attrs):
        if 'cmd_promote' not in attrs:
            attrs['cmd_promote'] = pascal_to_snake(name)
        return super().__new__(cls, name, bases, attrs)

class OperationBase(metaclass=BaseMeta):
    def __init__(self, context:Context) -> None:
        self.logger = context.logger
        self.context = context

    def execute_all(self) -> Error:
        res = self.pre_process()
        if res:
            self.context.logger.fatal(f'operation pre_process failed: {self.cmd_promote}, {res}')
            return res

        res = self.process()
        if res:
            self.context.logger.fatal(f'operation process failed: {self.cmd_promote}, {res}')
            return res

        res = self.post_process()
        if res:
            self.context.logger.fatal('operation post_process failed: {self.cmd_promote}, {res}')
            return res
        return Error.success()

    @abstractmethod
    def pre_process(self) -> Error:...

    @abstractmethod
    def process(self) -> Error:...

    @abstractmethod
    def post_process(self) -> Error:...

    @staticmethod
    @abstractmethod
    def require_manifest_file(context:Context) -> bool:...

    @staticmethod
    @abstractmethod
    def require_layout_file(context:Context) -> bool:...

class OperationEmpty(OperationBase):
    cmd_help_msg = 'Internal using for create Context'

    def __init__(self, context:Context) -> None:
        super().__init__(context)

    @staticmethod
    def require_manifest_file(context:Context) -> bool:
        return True

    @staticmethod
    def require_layout_file(context:Context) -> bool:
        return True