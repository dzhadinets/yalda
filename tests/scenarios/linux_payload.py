# Scenario for execute some standart commands in Linux cmdline
import os
from scenario_item import ScenarioItem

scenario_payload = [
        ScenarioItem(None, True, None, True, None, 1), # fake for start promt
        ScenarioItem('cat /proc/cmdline\n', True, None, False, None, 5),
        ScenarioItem('ls -la\n', True, None, False, None, 5),
        ScenarioItem('cat /proc/cpuinfo\n', True, None, False, None, 5),
]

class linux_payload:
    def __init__(self):
        pass

    def get_scenario(self):
        return scenario_payload
