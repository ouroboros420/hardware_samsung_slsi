#!/usr/bin/env python3

# Copyright 2021 Samsung Electronics

import json
import os
import re
import shutil

from utils import *
from argparse import ArgumentParser

# Lets user run script from anywhere
ROOT_DIR = os.path.abspath(os.path.join(os.path.dirname(os.path.abspath(__file__)), '..'))

def remove_json_files():
        interface_headers = ["private_handle.json", "metadata_gralloc.json", "metadata_gpu.json"]
        for header in interface_headers:
                file_for_parsing = os.path.join(ROOT_DIR, "src", "interface", header)
                os.remove(file_for_parsing)

def generate_interface_json_files(file_path):
        output_bin_dir = os.path.join(ROOT_DIR, "external", "json_generator")
        interface_headers = ["private_handle.h", "metadata_gralloc.h", "metadata_gpu.h"]
        for header in interface_headers:
                file_for_parsing = os.path.join(ROOT_DIR, "src", "interface", header)
                interface_json_gen_cmd = os.path.join(output_bin_dir, "interface_json_generator") + " " +\
                                                        file_for_parsing + " " +file_path
                run_command(interface_json_gen_cmd)

def parse_struct(struct_definitions, struct_name, member_info):
        if not member_info:
                member_info.append(struct_name)

        parsed_struct_info = []
        for each_member in struct_definitions[struct_name]:
                words_in_type = each_member['Type'].split()
                if len(words_in_type) == 1:
                        if each_member['Type'] in struct_definitions:
                                member_info_arg = member_info.copy()
                                member_info_arg.append(each_member['Name'])
                                parsed_member_struct_info = parse_struct(struct_definitions,
                                                                each_member['Type'], member_info_arg)
                                parsed_struct_info.extend(parsed_member_struct_info)
                        else:
                                parsed_member_info = [each_member['SetterEnabled']]
                                parsed_member_info.append(each_member['Type'])
                                parsed_member_info.extend(member_info)
                                parsed_member_info.append(each_member['Name'])
                                parsed_struct_info.append(parsed_member_info)

                elif len(words_in_type) > 1:
                        array_size = ''
                        array_size = array_size.join(re.findall(r"\[(\d+)\]", each_member['Type']))
                        if array_size:
                                if words_in_type[0] == 'struct':
                                        member_info_arg = member_info.copy()
                                        member_info_arg.extend([each_member['Name'], "is_array", str(array_size)])
                                        parsed_member_struct_info = parse_struct(struct_definitions,
                                                                        words_in_type[1], member_info_arg)
                                        parsed_struct_info.extend(parsed_member_struct_info)
                                elif words_in_type[0] in struct_definitions:
                                        member_info_arg = member_info.copy()
                                        member_info_arg.extend([each_member['Name'], "is_array", str(array_size)])
                                        parsed_member_struct_info = parse_struct(struct_definitions,
                                                                        words_in_type[0], member_info_arg)
                                        parsed_struct_info.extend(parsed_member_struct_info)
                                elif words_in_type[0] != "char":
                                        parsed_member_info = [each_member['SetterEnabled']]
                                        parsed_member_info.append(words_in_type[0])
                                        parsed_member_info.extend(member_info)
                                        parsed_member_info.extend([each_member['Name'], "is_array", str(array_size)])
                                        parsed_struct_info.append(parsed_member_info)

                        elif words_in_type[0] == 'struct':
                                member_info_arg = member_info.copy()
                                member_info_arg.append(each_member['Name'])
                                parsed_member_struct_info = parse_struct(struct_definitions,
                                                                words_in_type[1], member_info_arg)
                                parsed_struct_info.extend(parsed_member_struct_info)

        return parsed_struct_info

def scan_json_files(path, json_files):
        struct_def = {}
        enum_def = {}

        for file in json_files:
#                with open(os.path.join(ROOT_DIR, "out", "linux", file)) as f:
                with open(os.path.join(path, file)) as f:
                        data = json.load(f)

                for each_structure in data['Structs']:
                        if each_structure['Name']:
                                struct_def[each_structure['Name']] = each_structure['Members']

                for each_enum in data['Enums']:
                        if each_enum['Name'] == 'SGR_ALLOC_LAYOUT':
                                enum_def['SGR_ALLOC_LAYOUT'] = each_enum['Members']

        return struct_def, enum_def

def get_function_name_and_args(member_info):
        function_name = member_info[2]
        function_args = []

        for i in range(3, len(member_info)):
                if member_info[i] == "is_array":
                        arg_str = member_info[i-1] + "_index"
                        function_args.extend([arg_str, member_info[i+1]])
                elif member_info[i].isnumeric():
                        continue
                else:
                        function_name += ("_" + member_info[i])

        return function_name, function_args

def get_member_name(member_info):
        member_name = member_info[3]

        if len(member_info) > 4:
                for i in range(4, len(member_info)):
                        if member_info[i] == "is_array":
                                member_name += ("[" + member_info[i-1] + "_index]")
                        elif member_info[i].isnumeric():
                                continue
                        else:
                                member_name += ("." + member_info[i])
        return member_name

def write_function_args(f, function_args):
        for arg in function_args:
                if not arg.isnumeric():
                        f.write(", int " + arg)

def write_function_args_check(f, function_args, is_getter):
        for i in range(len(function_args)):
                if function_args[i].isnumeric():
                        continue
                else:
                        f.write("if (" + function_args[i] + " >= " + function_args[i+1] + ") {\n                ")
                        f.write('ALOGE("Array access out of bounds. ' + function_args[i] +\
                                ' = %d. Array size is ' + function_args[i+1] + '", ' +\
                                function_args[i] + ');\n                ')
                        if is_getter == True:
                                f.write("return 0;\n        }\n\n        ")
                        else:
                                f.write("return;\n        }\n\n        ")

def write_function_declarations(f, struct_info):
        for member in struct_info:
                func_name, func_args = get_function_name_and_args(member)

                f.write(member[1] + " sgr_intf_get_" + func_name + "(const struct native_handle *handle")
                write_function_args(f, func_args)
                f.write(");\n")

                if member[0] == True:
                        f.write("void sgr_intf_set_" + func_name + "(struct native_handle *handle")
                        write_function_args(f, func_args)
                        f.write(", const " + member[1] + " set_val);\n\n")
                else:
                        f.write("\n")

def write_enums(f, enum_def):
        for enum in enum_def:
                f.write("enum SGR_INTF_" + enum + " {\n")
                for member in enum_def[enum]:
                        f.write("        SGR_INTF_" + member["Name"] + " = " + str(member["Value"]) + ",\n")
                f.write("};\n\n")

def generate_cpp_interface_header(path, parsed_struct_info, enum_def):
        f = open(os.path.join(path, "g_private_handle_interface.h"), "w")
        f.write("///\n/// @file g_private_handle_interface.h\n/// @copyright 2021-2022 Samsung Electronics\n///\n\n")
        f.write("/// THIS IS AN AUTOGENERATED FILE ///\n")
        f.write("///   DO NOT EDIT THIS FILE       ///\n\n")
        f.write("#ifndef PRIVATE_HANDLE_INTERFACE_H_\n")
        f.write("#define PRIVATE_HANDLE_INTERFACE_H_\n\n")
        f.write("#include <stdint.h>\n\n")
        f.write("struct native_handle;\n\n")

        write_enums(f, enum_def)

        for each_struct_info in parsed_struct_info:
                write_function_declarations(f, parsed_struct_info[each_struct_info])

        f.write("#endif        // PRIVATE_HANDLE_INTERFACE_H_\n")
        f.close()

def write_private_handle_functions(f, struct_info):
        for member in struct_info:
                func_name, func_args = get_function_name_and_args(member)

                f.write("\n" + member[1] + " sgr_intf_get_" + func_name + "(const struct native_handle *handle")
                write_function_args(f, func_args)
                f.write(")\n{\n        ")
                write_function_args_check(f, func_args, True)
                f.write("const struct private_handle_t *priv_hnd = (const struct private_handle_t *)handle;\n\n        ")
                f.write("return priv_hnd->" + get_member_name(member) + ";\n}\n")

                if member[0] == True:
                        f.write("\nvoid sgr_intf_set_" + func_name + "(struct native_handle *handle")
                        write_function_args(f, func_args)
                        f.write(", const " + member[1] + " set_val)\n{\n        ")
                        write_function_args_check(f, func_args, False)
                        f.write("struct private_handle_t *priv_hnd = (struct private_handle_t *)handle;\n\n        ")
                        f.write("priv_hnd->" + get_member_name(member) + " = set_val;\n}\n")

def write_metadata_gralloc_functions(f, struct_info):
        for member in struct_info:
                func_name, func_args = get_function_name_and_args(member)

                f.write("\n" + member[1] + " sgr_intf_get_" + func_name + "(const struct native_handle *handle")
                write_function_args(f, func_args)
                f.write(")\n{\n        ")
                write_function_args_check(f, func_args, True)
                f.write("const struct sgr_metadata *metadata = sgr_get_metadata(handle);\n\n        ")
                f.write("return metadata->" + get_member_name(member) + ";\n}\n")

                if member[0] == True:
                        f.write("\nvoid sgr_intf_set_" + func_name + "(struct native_handle *handle")
                        write_function_args(f, func_args)
                        f.write(", const " + member[1] + " set_val)\n{\n        ")
                        write_function_args_check(f, func_args, False)
                        f.write("struct sgr_metadata *metadata = sgr_get_metadata(handle);\n\n        ")
                        f.write("metadata->" + get_member_name(member) + " = set_val;\n}\n")

def write_metadata_gpu_functions(f, struct_info):
        for member in struct_info:
                func_name, func_args = get_function_name_and_args(member)

                f.write("\n" + member[1] + " sgr_intf_get_" + func_name + "(const struct native_handle *handle")
                write_function_args(f, func_args)
                f.write(")\n{\n        ")
                write_function_args_check(f, func_args, True)
                f.write("const struct sgr_metadata_gpu *metadata_gpu = (const sgr_metadata_gpu *)sgr_get_metadata_gpu(handle);\n\n        ")
                f.write("return metadata_gpu->" + get_member_name(member) + ";\n}\n")

                if member[0] == True:
                        f.write("\nvoid sgr_intf_set_" + func_name + "(struct native_handle *handle")
                        write_function_args(f, func_args)
                        f.write(", const " + member[1] + " set_val)\n{\n        ")
                        write_function_args_check(f, func_args, False)
                        f.write("struct sgr_metadata_gpu *metadata_gpu = (struct sgr_metadata_gpu *)sgr_get_metadata_gpu(handle);\n\n        ")
                        f.write("metadata_gpu->" + get_member_name(member) + " = set_val;\n}\n")

def generate_cpp_interface_source(path, parsed_struct_info):
#        f = open(os.path.join(path, "src", "interface", "g_private_handle_interface.cpp"), "w")
        f = open(os.path.join(path, "g_private_handle_interface.cpp"), "w")
        f.write("///\n/// @file g_private_handle_interface.cpp\n/// @copyright 2021-2022 Samsung Electronics\n///\n\n")
        f.write("/// THIS IS AN AUTOGENERATED FILE ///\n")
        f.write("///   DO NOT EDIT THIS FILE       ///\n\n")
        f.write("#include <stdint.h>\n\n")
        f.write("#include <cutils/log.h>\n")
        f.write("#include <cutils/native_handle.h>\n\n")
        f.write('#include "metadata_gpu.h"\n')
        f.write('#include "metadata_gralloc.h"\n')
        f.write('#include "private_handle.h"\n\n')
        f.write('#include "g_private_handle_interface.h"\n')

        write_private_handle_functions(f, parsed_struct_info["private_handle_t"])
        write_metadata_gralloc_functions(f, parsed_struct_info["sgr_metadata"])
        write_metadata_gpu_functions(f, parsed_struct_info["sgr_metadata_gpu"])

        f.close()

def generate_cpp_interface_files(file_path, target):
        interface_json_files = ["private_handle.json", "metadata_gralloc.json", "metadata_gpu.json"]
        struct_definitions, enum_definitions = scan_json_files(file_path,interface_json_files)

        parsed_struct_info = {}
        parsed_struct_info["private_handle_t"] = parse_struct(struct_definitions, "private_handle_t", [])
        parsed_struct_info["sgr_metadata"]     = parse_struct(struct_definitions, "sgr_metadata", [])
        parsed_struct_info["sgr_metadata_gpu"] = parse_struct(struct_definitions, "sgr_metadata_gpu", [])

        if target == "source":
            generate_cpp_interface_source(file_path, parsed_struct_info)
        else:
            generate_cpp_interface_header(file_path, parsed_struct_info, enum_definitions)

if __name__ == '__main__':
        # Arguments
        parser = ArgumentParser(usage="Create autogen file with version information")
        parser.add_argument("--file", help="Path of the file to be generated")
        parser.add_argument("--select", help="select built target (header/source)")
        options = parser.parse_args()

        file_path = "None"
        target = "None"

        file_path = "./src/interface"
        generate_interface_json_files(file_path)

        generate_cpp_interface_files(file_path, "source")
        generate_cpp_interface_files(file_path, "header")

        shutil.rmtree(os.path.join(ROOT_DIR, "scripts", "__pycache__"), ignore_errors=True)
        remove_json_files()

