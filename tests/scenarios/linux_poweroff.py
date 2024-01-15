# Scenario for send command poweroff to Linux cmdline
import os
from scenario_item import ScenarioItem

scenario_poweroff = [
        ScenarioItem('poweroff -f', True, None, False, None, 15),
]

class linux_poweroff:
    def __init__(self):
        pass

    def get_scenario(self):
        return scenario_poweroff
