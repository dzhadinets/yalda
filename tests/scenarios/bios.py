# Scenario for loading BIOS
# wait string "Booting from ROM"
import os
from scenario_item import ScenarioItem

scenario_seabios = [
        ScenarioItem(None, False, 'Booting from ROM', False, None, 10),
]

class bios:
    def __init__(self):
        pass

    def get_scenario(self):
        return scenario_seabios
