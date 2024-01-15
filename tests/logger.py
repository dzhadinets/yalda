import os
import logging


#log = Logger()

class Logger:
    def __new__(cls):
        if not hasattr(cls, 'instance'):
            cls.instance = super(Logger, cls).__new__(cls)
        return cls.instance

    def __init__(self, file=""):
        FORMAT = ' * LISTNER:[%(levelname)s] %(message)s'

        logging.basicConfig(format=FORMAT, level=logging.INFO)

        self.__log = logging.getLogger(__name__)

#        if file:
#            fhandler = logging.FileHandler(f file+".log", mode='w')
#            formatter = logging.Formatter(FORMAT)
#            fhandler.setFormatter(formatter)
#            self.__log.addHandler(fhandler)



    def debug(self, string):
        self.__log.debug(string)

    def info(self, string):
        self.__log.info(string)

    def warning(self, string):
        self.__log.warning(string)

    def error(self, string):
        self.__log.error(string)

def log_debug(string):
    Logger().debug(string)

def log_info(string):
    Logger().info(string)

def log_warning(string):
    Logger().warning(string)

def log_error(string):
    Logger().error(string)
