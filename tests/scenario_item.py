from logger import Logger, log_debug, log_info, log_warning, log_error

#zboot_template = 'zboot {} {} {} {}\n'
#scenario_templates = {
#        zboot_template : [0, 0, 0, 0]
#};

class ScenarioItem:
        # input_template - python format-style string that is going to be sent to
        #                  stdin of a child process: zboot {:08x} {:08x}
        # read_until_prompt - continue reading stdout of a child process until prompt is
        #                     detected.
        # read_until_token  - continue reading stdout of a child process until specific
        #                     string is is detected. Difference is that prompt is shown
        #                     at the start of the line and token can be anywhere on the
        #                     line
        # capture           - capture all lines in list of strings
        # cb                - after scenario is complete run this callback
        # timeout

        def __init__(self, input_template, read_until_prompt, read_until_token, capture, cb, timeout=60):
                self.__input_template = input_template
                self.__read_until_prompt=read_until_prompt
                self.__read_until_token = read_until_token
                self.__capture = capture
                self.__cb = cb
                self.__timeout = timeout

        def run(self, p):
                debug_log_prompt = 'no_prompt'
                debug_log_token = 'no_token'
                if self.__read_until_prompt:
                        debug_log_prompt = p.get_prompt()

                log_debug('{} prompt:{},token:{} {}'.format(
                        '[[ScenarioItem::Run]] ',
                        debug_log_prompt,
                        debug_log_token,
                        ' [[ScenarioItem::RunEnd]]'))

                result = None
                if self.__input_template:
                        input_string = self.__input_template
#                        if input_string in scenario_templates.keys():
#                                args = scenario_templates[input_string]
#                                input_string = input_string.format(*args)
                        p.write(input_string)

                if self.__read_until_prompt and self.__read_until_token:
                        result = p.read_until_prompt_or_token(self.__read_until_token,
                                 self.__capture, self.__timeout)

                elif self.__read_until_prompt:
                        result = p.read_until_prompt(self.__capture, self.__timeout)

                elif self.__read_until_token:
                        result = p.read_until_token(self.__read_until_token, self.__capture, self.__timeout)

                if self.__cb:
                        self.__cb(p, result)

                return result

