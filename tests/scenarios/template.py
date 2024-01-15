# Example Scenario  with callback function
import os
from scenario_item import ScenarioItem

TEMPLATE_PROMPT = '#'

def template_cb(p, result):
    print ('!!!!!!! Test example finished !!!!!!!')

#You may set special cmdline prompt if you need
def template_start_cb(p, result):
        p.set_prompt(TEMPLATE_PROMPT)

scenario_template = [
        ScenarioItem(None , False, None, False, template_start_cb, 20),
        ScenarioItem('cat /proc/cmdline\n', True, None, True, None, 5),
        ScenarioItem('ls -la\n', True, None, True, template_cb, 5),
]

# file name and class name must have identical names
# class must have minimum two function - '__init__' and 'get_scenario'
class template:
    def __init__(self):
        pass

    def get_scenario(self):
        return scenario_template
