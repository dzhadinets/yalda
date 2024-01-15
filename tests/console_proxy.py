import subprocess
import time
import string
import select
import signal
import os
import psutil
from logger import Logger, log_debug, log_info, log_warning, log_error

class TimeoutReached(Exception):
        pass

def kill_child_processes(parent_pid, sig=signal.SIGTERM):
    try:
        parent = psutil.Process(parent_pid)
    except psutil.NoSuchProcess:
        return
    children = reversed(parent.children(recursive=True))
    for process in children:
        process.send_signal(sig)

def read_until_prompt_or_token(p, prompt, token, capture, timeout):
        log_debug('[[[ CAPTURING INPUT prompt="{}", token="{}", timeout={}sec]]]'.format(
                prompt,
                token,
                timeout))

        end_time = time.time() + timeout
        lines = []
        last_line = ''
        print_line = ''

        poll_obj = select.poll()
        poll_obj.register(p.stdout, select.POLLIN)

        while(True):
            poll_result = poll_obj.poll(0)
            if poll_result:
                l = p.stdout.read(1)
                c = l[0]

                if c not in string.printable:
                        if ord(c) == 0x1b:
                                # Escape
                                continue
                        log_info('\\x{:02x}-'.format(ord(c)))
                        continue
                else:
                    if c == '\n':
                        log_info(print_line)
                        print_line = ''
                    else:
                        print_line += c



                if c == '\r':
                        # This is for debug
                        log_debug('+')

                # U-boot console uses backspace to count-down 'Hit any key' timeout
                if ord(c) == 8:
                        # Backspace
                        last_line = last_line[:-1]
                elif c == '\n':
                        if capture:
                                lines.append(last_line)
                        last_line = ''
                        continue

                last_line += c

                if token and token in last_line:
                        log_info('[[[ REACHED TOKEN : "{}" ]]]'.format(token))
                        return lines

                if prompt and last_line == prompt:
                        log_info('[[[ REACHED PROMPT: "{}" ]]]'.format(prompt))
                        return lines

            if time.time() >= end_time:
                raise TimeoutReached


class ConsoleProcessProxy:
        def __init__(self, cmdlist, prompt):
            self.__p = subprocess.Popen(cmdlist,
                    text='utf-8',
                    bufsize=0,
                    stdin=subprocess.PIPE,
                    stdout=subprocess.PIPE,
                    stderr=subprocess.PIPE,
                    shell=True)
            self.__prompt = prompt

        def read_until_prompt_or_token(self, token, capture, timeout):
                return read_until_prompt_or_token(self.__p, self.__prompt, token, capture, timeout)

        def read_until_prompt(self, capture, timeout):
                return read_until_prompt_or_token(self.__p, self.__prompt, None, capture, timeout)

        def read_until_token(self, token, capture, timeout):
                return read_until_prompt_or_token(self.__p, None, token, capture, timeout)

        def set_prompt(self, prompt):
                self.__prompt = prompt

        def get_prompt(self):
                return self.__prompt

        def write(self, text):
                self.__p.stdin.write(text)

        def terminate(self):
                kill_child_processes(self.__p.pid)
                self.__p.terminate()
                self.__p.wait()
