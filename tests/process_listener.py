#!/usr/bin/python3
import argparse
import os
from importlib.machinery import SourceFileLoader
from scenario_item import ScenarioItem
from console_proxy import ConsoleProcessProxy, TimeoutReached
from logger import Logger, log_debug, log_info, log_warning, log_error

DEFAULT_PROMPT = '/'

STATUS_OK = 0
STATUS_TIMEOUT = -1
STATUS_INTERRUPT = -2
STATUS_BAD_ARG = -3
STATUS_UNKNOWN = -127

scenario_null = [
        ScenarioItem(None, False, None, False, None, 1),
]

def main():

    parser = argparse.ArgumentParser()
    parser.add_argument("--run", type=str, default="")
    parser.add_argument("--scenario", type=str, default="")
    args = parser.parse_args()

    run_str = args.run
    scenario_str = args.scenario

    p = ConsoleProcessProxy([run_str], DEFAULT_PROMPT)

    if not run_str:
        log_error('CmdLine did not have run command')
        return STATUS_BAD_ARG

    if not scenario_str :
        log_error('CmdLine did not have scenarios')
        return STATUS_BAD_ARG

    scenario = scenario_null
    for sc_stc in scenario_str.split():
        if not os.path.isfile(sc_stc):
            log_warning('Scenarion not found in file: ' + sc_stc)
            continue
        cl_name = os.path.splitext(os.path.basename(sc_stc))[0]
        module = SourceFileLoader(cl_name, sc_stc).load_module()
        given_class = getattr(module, cl_name)
        instance = given_class()
        scenario += instance.get_scenario()

    exit_code = STATUS_OK
    try:
        for sc in scenario:
            sc.run(p)
        log_error('Execute successful')
    except TimeoutReached:
        log_error('Execute timeout')
        exit_code = STATUS_TIMEOUT
    except KeyboardInterrupt:
        log_error('The Program is terminated manually!')
        exit_code = STATUS_INTERRUPT
    except Exception as e:
        log_error('Unknown exception: ' + str(e))
        exit_code = STATUS_UNKNOWN

    p.terminate()
    log_debug('Program terminated\n')
    return exit_code

if __name__ == '__main__':
    exit(main())

