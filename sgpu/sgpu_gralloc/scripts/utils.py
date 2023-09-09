#!/usr/bin/python

# Copyright 2018-2020 Samsung Electronics

import os
import sys
import subprocess

from datetime import datetime


def gralloc_print(message):
    """ Used to flush after each print """
    print(message)
    sys.stdout.flush()


def print_verbose(verbose, message):
    if verbose == True:
        gralloc_print(message)


# EC Agent doesn't support color
if subprocess.check_output(["whoami"]).strip() != "ecagent":
    def print_red(message):
        """ Prints red text with no newline"""
        print("\033[91m{0}\033[0m".format(message), end='', flush=True)

    def print_blue(message):
        """ Prints blue text with no newline"""
        print("\033[94m{0}\033[0m".format(message), end='', flush=True)

    def print_green(message):
        """ Prints green text with no newline"""
        print("\033[92m{0}\033[0m".format(message), end='', flush=True)

else:
    def print_red(message):
        print(message, end='', flush=True)

    def print_blue(message):
        print(message, end='', flush=True)

    def print_green(message):
        print(message, end='', flush=True)


def run_command(cmd, print_output=False, output_filepath=""):
    """
    This handles to OS difference in the subprocess's call functions.
    It also will error out and print out the stdout on error
    """
    print_blue("\nRunning command: ")
    gralloc_print(cmd)

    try:
        use_shell = True
        if os.name == "nt":
            use_shell = False

        process = subprocess.Popen(cmd, stdout=subprocess.PIPE, stderr=subprocess.STDOUT, shell=use_shell, encoding="utf-8")

        if output_filepath != "":
            output_file = open(output_filepath, "w")

        output = []
        for line in process.stdout:
            if print_output:
                gralloc_print(line)
            else:
                output.append(line)
            if output_filepath != "":
                output_file.write("{0}\n".format(line))

        process.wait()
        process.stdout.close()

        if output_filepath != "":
            output_file.close()

        if process.returncode != 0:
            print_red("Command: {0}\n".format(str(cmd)))
            print_red("Error code: {0}\n".format(str(process.returncode)))
            if not print_output:
                print_red("Log:\n{0}\n".format("\n".join(output)))
            if output_filepath != "":
                print_red("*** Log File: {0} ***\n".format(output_filepath))
            sys.exit(process.returncode)

    except OSError as e:
        print_red("Error code {0} encountered while opening filepath  {1} :\n".format(str(e.errno), e.filename))
        print_red("{0}\n".format(e.strerror))
        sys.exit(e.errno)


def check_python_version(required_major, required_minor):
    """
    Checks to make sure the version of python being used is of the required version.
    Returns True if valid version is being used
    """
    python_version_major = sys.version_info[0]
    python_version_minor = sys.version_info[1]
    if python_version_major != required_major or python_version_minor < required_minor:
        python_error_string = """
            Attempted to execute with python version {0}.{1} but {2}.{3} or later is required
        """.format(python_version_major, python_version_minor, required_major, required_minor)
        print_red(python_error_string)
        return False
    else:
        return True