# fake Scenario for finish
# should be at the end
import os
from scenario_item import ScenarioItem

scenario_done = [
        ScenarioItem(None, False, None, True, None, 1),
]

class done:
    def __init__(self):
        pass

    def get_scenario(self):
        return scenario_done
