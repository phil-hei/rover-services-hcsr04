#!/usr/bin/env python
from raml2agl import Raml2Agl as r2g
from raml2agl import RamlParser as rp
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
app_src_path = os.path.join(app_path, 'src')
app_include_path = os.path.join(app_path, 'include', 'app')
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
parser = rp.RamlParser()

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


def copy_template(src, dst):
    create_dir(dst)

    for item in os.listdir(src):
        item_full = os.path.join(src, item)

        if os.path.isdir(item_full):
            item_dst_full = os.path.join(dst, item)
            copy_template(item_full, item_dst_full)
            continue

        splited = os.path.splitext(item)

        if '.in' == splited[1]:
            item_dst_full = os.path.join(dst, splited[0])
            shutil.copy2(item_full, item_dst_full)


def copy_autobuild_dir(dst):
    autobuild_dir = os.path.join(app_templates_path,
                                 'template.d',
                                 'autobuild')

    copy_template(autobuild_dir, dst)


def create_app_files(model_filename):
    model_path = os.path.join(models_path, model_filename)

    logging.info("Creating AGL Service Build Files at - " + app_path)

    create_dir(app_path)
    create_dir(app_src_path)
    create_dir(app_include_path)

    # Generate App sources
    generator.set_input_model(model_path)
    generator.set_source_out_path(app_src_path)
    generator.set_headers_out_path(app_include_path)

    logging.info("Generating App Class from - " + model_filename)
    generator.generate_app()


def create_app_build_system():
    logging.info("Creating AGL Application Build Files at - " + app_path)

    info = {}
    info["name"] = 'Rover Services API'
    info["short_name"] = 'rover-services-api'

    # Create the CMakeLists.txt file
    tmpl = env.get_template("app.CMakeLists.txt")

    cont = tmpl.render(info=info)

    fh = open(os.path.join(app_path, 'CMakeLists.txt'), "w")
    fh.write(cont)
    fh.close()


def create_agl_service_build_system(model_filename):
    model_path = os.path.join(models_path, model_filename)

    jmodel = parser.parse(model_path)

    info = {}
    info["short_name"] = jmodel["api_name"]
    service_path = os.path.join(services_path,
                                info["short_name"])

    create_dir(services_path)

    # Create the service root dir
    conf_path = os.path.join(service_path, 'conf.d')
    cmake_path = os.path.join(conf_path, 'cmake')
    binding_path = os.path.join(service_path, info["short_name"] + '-afb')
    src_dir = os.path.join(binding_path, 'src')
    include_dir = os.path.join(binding_path, 'include', 'service')
    wgt_path = os.path.join(conf_path, 'wgt')

    if os.path.exists(service_path):
        logging.warning("AGL Service Classes already existing. Skipping...")
        return

    logging.info("Creating AGL Service Build Files at - " + service_path)
    create_dir(service_path)
    create_dir(cmake_path)
    create_dir(src_dir)
    create_dir(include_dir)
    create_dir(wgt_path)

    # Create the config.cmake file
    tmpl = env.get_template("service.config.cmake")

    info["name"] = jmodel["title"]
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

    # Deploy the autobuild Scripts
    autobuild_dst_dir = os.path.join(conf_path, 'autobuild')
    copy_autobuild_dir(autobuild_dst_dir)

    # Deploy the widget config templat
    tmpl = env.get_template("service.config.xml.in")

    cont = tmpl.render(info=info)

    fh = open(os.path.join(wgt_path, "config.xml.in"), "w")
    fh.write(cont)
    fh.close()

    # Generate Sources
    generator.set_input_model(model_path)
    generator.set_source_out_path(src_dir)
    generator.set_headers_out_path(include_dir)

    generator.generate_service()
    generator.generate_service_class()

    # Create the CMakeLists.txt file
    tmpl = env.get_template("service.CMakeLists.txt")

    info["service_class"] = jmodel["service_class_name"]
    cont = tmpl.render(info=info)

    fh = open(os.path.join(binding_path, 'CMakeLists.txt'), "w")
    fh.write(cont)
    fh.close()


if __name__ == '__main__':
    from sys import argv
    myargs = getopts(argv)

    if '-l' in myargs:
        logging.basicConfig(level=mapping[myargs['-l']])
    else:
        logging.basicConfig(level=logging.WARNING)

    logging.info("Provided params - " + json.dumps(myargs,
                                                   sort_keys=True,
                                                   indent=4))

    # Clean App directory
    if os.path.exists(app_path):
        shutil.rmtree(app_path)
        logging.info("Removing - " + app_path)

    create_dir(app_path)

    create_app_build_system()

    for filename in os.listdir(models_path):
        logging.info("Model File - " + filename)

        create_app_files(filename)
        create_agl_service_build_system(filename)
