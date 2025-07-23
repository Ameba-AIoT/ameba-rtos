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

    @abstractmethod
    def pre_process(self) -> Error:...

    @abstractmethod
    def process(self) -> Error:...

    @abstractmethod
    def post_process(self) -> Error:...
