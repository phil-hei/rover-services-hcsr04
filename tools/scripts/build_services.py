#!/usr/bin/env python
import os
import shutil
import json
import logging
import subprocess

script_path = os.path.dirname(os.path.realpath(__file__))
root_path = os.path.dirname(os.path.dirname(script_path))

services_path = os.path.join(root_path, 'service')

mapping = {'info': logging.INFO,
           'warning': logging.WARNING,
           'debug': logging.DEBUG,
           'error': logging.ERROR,
           }


def getopts(argv):
    opts = {}  # Empty dictionary to store key-value pairs.
    while argv:  # While there are arguments left to parse...
        if argv[0][0] is '-':  # Found a "-name value" pair.
            if len(argv) > 1:
                if argv[1][0] != '-':
                    opts[argv[0]] = argv[1]
                else:
                    opts[argv[0]] = True
            elif len(argv) == 1:
                opts[argv[0]] = True

        # Reduce the argument list by copying it starting from index 1.
        argv = argv[1:]
    return opts


def create_dir(path):
    if not os.path.exists(path):
        os.makedirs(path)
        logging.info("Creating - " + path)


def build_service(path, target=None, clean=False, install=False):
    build_path = os.path.join(path, 'build')

    if clean and os.path.exists(build_path):
        logging.info("Cleaning " + build_path)
        shutil.rmtree(build_path)

    subprocess.call(["conf.d/autobuild/agl/autobuild", "package"], cwd=path)

    proc = subprocess.Popen(["ls"],
                            stdout=subprocess.PIPE,
                            cwd=build_path)
    proc2 = subprocess.Popen(["grep", "wgt"],
                             stdin=proc.stdout,
                             stdout=subprocess.PIPE,
                             cwd=build_path)
    wgt_file = proc2.stdout.readline().split()[0]

    proc.kill()
    proc2.kill()

    if target:
        logging.info("Deploying " + wgt_file)
        subprocess.call(["scp", wgt_file, "root@" + target + ":."],
                        cwd=build_path)
    if install:
        logging.info("Installing " + wgt_file)
        install_script = os.path.join(script_path, "install_wgt.sh")
        bash_cmd = install_script + " root@" + target + " " + wgt_file
        subprocess.check_output(bash_cmd, shell=True)


def check_filter(filter, name):
    if filter:
        if filter not in name:
            return False

    return True


if __name__ == '__main__':
    from sys import argv
    myargs = getopts(argv)

    target = None
    clean = False
    install = False
    filter = None

    if '-l' in myargs:
        logging.basicConfig(level=mapping[myargs['-l']])
    else:
        logging.basicConfig(level=logging.WARNING)

    if '-d' in myargs:
        target = myargs['-d']

    if '-c' in myargs:
        clean = True

    if '-i' in myargs:
        if '-d' in myargs:
            install = True
        else:
            logging.warning("No target specified")

    if '-f' in myargs:
        filter = myargs['-f']

    logging.info("Provided params - " + json.dumps(myargs,
                                                   sort_keys=True,
                                                   indent=4))

    for filename in os.listdir(services_path):
        if not check_filter(filter, filename):
            continue

        logging.info("Building Service - " + filename)
        full_path = os.path.join(services_path, filename)

        build_service(full_path, target, clean, install)

    if check_filter(filter, "demo"):
        full_path = os.path.join(root_path, "demo")
        build_service(full_path, target, clean, install)

    if check_filter(filter, "menu"):
        full_path = os.path.join(root_path, "menu")
        build_service(full_path, target, clean, install)
