# Scenario for wait Linux loaded
# wait string "Yet Another Linux"
import os
from scenario_item import ScenarioItem
from logger import Logger, log_debug, log_info, log_warning, log_error

def kernel_started_cb(p, result):
        log_debug('Kernel started')

scenario_linux_loaded = [
        ScenarioItem(None, False, 'Yet Another Linux', True, kernel_started_cb, 20),
]

class linux_loaded:
    def __init__(self):
        pass

    def get_scenario(self):
        return scenario_linux_loaded
