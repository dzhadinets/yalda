# Scenario for loading U-Boot
# wait string "Booting from ROM", find boot partition and load from it
import os
from scenario_item import ScenarioItem

LINUX_PROMPT = '/'
UBOOT_PROMPT = '=>'

def kernel_start_cb(p, result):
        p.set_prompt(LINUX_PROMPT)

def uboot_load_cb(p, result):
        p.set_prompt(UBOOT_PROMPT)

zboot_template = 'zboot {} {} {} {}\n'
scenario_templates = {
        zboot_template : [0, 0, 0, 0]
};

def qfw_load_cb(p, result):
        for i in result:
                if 'loading kernel' in i:
                        tmp = i.split()
                        kernel_addr = tmp[4]
                        kernel_size = tmp[6]
                        initrd_addr = tmp[8]
                        initrd_size= tmp[10]
                        scenario_templates[zboot_template] = [kernel_addr, kernel_size, initrd_addr, initrd_size]
                        print(scenario_templates)

scenario_uboot = [
        ScenarioItem(None          , False, 'Hit any key', False, uboot_load_cb, 20),
        ScenarioItem('a'           , False , None, False, None, 5),
        ScenarioItem('printenv\n'  , True , None, True, None, 5),
        ScenarioItem('bootflow scan\n'  , False , None, True, None, 5),
        ScenarioItem('bootflow list\n'  , False , None, True, None, 5),
        ScenarioItem('bootflow select 0\n'  , False , None, True, None, 5),
        ScenarioItem('bootflow info\n'  , False , None, True, None, 5),
        ScenarioItem('bootflow boot\n'  , False , None, False, kernel_start_cb, 5),

#        ScenarioItem('qfw list\n'  , True , None         , True, qfw_load_cb),
#        ScenarioItem('qfw load\n'  , False , None         , True , kernel_start_cb),
#        ScenarioItem(zboot_template, False, None         , False, kernel_start_cb),
#        ScenarioItem('boot\n'      , False, None         , True, kernel_start_cb, 5),
#        ScenarioItem(None          , False , 'Kernel panic', False, None),
]


class uboot:
    def __init__(self):
        pass

    def get_scenario(self):
        return scenario_uboot
