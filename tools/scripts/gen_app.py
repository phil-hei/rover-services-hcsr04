#!/usr/bin/env python
from raml2agl import Raml2Agl as r2g
from jinja2 import Environment, FileSystemLoader
import os
import shutil
import json
import logging

script_path = os.path.dirname(os.path.realpath(__file__))
root_path = os.path.dirname(os.path.dirname(script_path))
models_path = os.path.join(root_path, 'models')
tools_path = os.path.join(root_path, 'tools')
app_templates_path = os.path.join(tools_path, 'app-templates')

app_path = os.path.join(root_path, 'app')
app_include_path = os.path.join(app_path, 'include')
app_src_path = os.path.join(app_path, 'src')
services_path = os.path.join(root_path, 'service')

# Load templates
path = os.path.dirname(os.path.realpath(__file__)) + "/templates"
env = Environment(
    loader=FileSystemLoader(path),
    trim_blocks=True,
    lstrip_blocks=True,
    keep_trailing_newline=False
)

generator = r2g.Raml2Agl()


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


def create_app_files(model_filename):
    model_path = os.path.join(models_path, model_filename)

    generator.set_input_model(model_path)
    generator.set_source_out_path(app_src_path)
    generator.set_headers_out_path(app_include_path)

    logging.info("Generating App Class from - " + model_filename)
    generator.generate_app()


def create_agl_build_system(model_filename):
    model_path = os.path.join(models_path, model_filename)
    info = {}
    info["short_name"] = os.path.splitext(model_filename)[0]
    service_path = os.path.join(services_path,
                                info["short_name"])

    # If exists then the build system is assumed to be set
    if os.path.exists(service_path):
        return

    # Create the service root dir
    logging.info("Creating AGL Service Build Files at - " + service_path)
    conf_path = os.path.join(service_path, 'conf.d')
    cmake_path = os.path.join(conf_path, 'cmake')

    create_dir(service_path)
    create_dir(cmake_path)

    # Create the config.cmake file
    tmpl = env.get_template("config.cmake")

    info["name"] = info["short_name"]
    cont = tmpl.render(info=info)

    fh = open(os.path.join(cmake_path, 'config.cmake'), "w")
    fh.write(cont)
    fh.close()

    # Deploy the CMakeLists.txt from templates
    cmakelists_file = os.path.join(app_templates_path,
                                   'samples.d',
                                   'CMakeLists.txt.sample')
    cmakelists_dst_file = os.path.join(service_path, 'CMakeLists.txt')
    shutil.copy2(cmakelists_file, cmakelists_dst_file)


if __name__ == '__main__':
    from sys import argv
    myargs = getopts(argv)

    if '-v' in myargs:
        logging.basicConfig(level=logging.INFO)

    logging.info("Provided params - " + json.dumps(myargs,
                                                   sort_keys=True,
                                                   indent=4))

    # Clean App directory
    if os.path.exists(app_path):
        shutil.rmtree(app_path)
        logging.info("Removing - " + app_path)

    create_dir(app_path)
    create_dir(app_src_path)
    create_dir(app_include_path)

    for filename in os.listdir(models_path):
        logging.info("Model File - " + filename)

        create_app_files(filename)
        create_agl_build_system(filename)
