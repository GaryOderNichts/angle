#!/usr/bin/python3
#
# Copyright 2019 The ANGLE Project Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.
#
# gen_gl_enum_utils.py:
#   Generates GLenum value to string mapping for ANGLE
#   NOTE: don't run this script directly. Run scripts/run_code_generation.py.

import sys
import os

import registry_xml

template_gl_enums_header = """// GENERATED FILE - DO NOT EDIT.
// Generated by {script_name} using data from {data_source_name}.
//
// Copyright 2019 The ANGLE Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// gl_enum_utils_autogen.h:
//   mapping of GLenum value to string.

# ifndef COMMON_GL_ENUM_UTILS_AUTOGEN_H_
# define COMMON_GL_ENUM_UTILS_AUTOGEN_H_

namespace gl
{{
enum class GLESEnum
{{
    {gles_enum_groups}
}};

enum class BigGLEnum
{{
    {gl_enum_groups}
}};
}}  // namespace gl

# endif  // COMMON_GL_ENUM_UTILS_AUTOGEN_H_
"""

template_gl_enums_source = """// GENERATED FILE - DO NOT EDIT.
// Generated by {script_name} using data from {data_source_name}.
//
// Copyright 2019 The ANGLE Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// gl_enum_utils_autogen.cpp:
//   mapping of GLenum value to string.

#include "common/gl_enum_utils_autogen.h"

#include "common/debug.h"
#include "common/gl_enum_utils.h"

#include <algorithm>
#include <cstring>

namespace gl
{{
namespace
{{
const char *UnknownEnumToString(unsigned int value)
{{
    constexpr size_t kBufferSize = 64;
#ifdef __WIIU__
    static char sBuffer[kBufferSize];
#else
    static thread_local char sBuffer[kBufferSize];
#endif
    snprintf(sBuffer, kBufferSize, "0x%04X", value);
    return sBuffer;
}}
}}  // anonymous namespace

const char *GLenumToString(GLESEnum enumGroup, unsigned int value)
{{
    switch (enumGroup)
    {{
        {gles_enums_value_to_string_table}
        default:
            return UnknownEnumToString(value);
    }}
}}

const char *GLenumToString(BigGLEnum enumGroup, unsigned int value)
{{
    switch (enumGroup)
    {{
        {gl_enums_value_to_string_table}
        default:
            return UnknownEnumToString(value);
    }}
}}

namespace
{{
using StringEnumEntry = std::pair<const char*, unsigned int>;
static StringEnumEntry g_stringEnumTable[] = {{
    {string_to_enum_table}
}};

const size_t g_numStringEnums = std::size(g_stringEnumTable);
}}  // anonymous namespace

unsigned int StringToGLenum(const char *str)
{{
    auto it = std::lower_bound(
        &g_stringEnumTable[0], &g_stringEnumTable[g_numStringEnums], str,
        [](const StringEnumEntry& a, const char* b) {{ return strcmp(a.first, b) < 0; }});

    if (strcmp(it->first, str) == 0)
    {{
        return it->second;
    }}

    UNREACHABLE();
    return 0;
}}
}}  // namespace gl

"""

template_enum_group_case = """case {api_enum}::{group_name}: {{
    switch (value) {{
        {inner_group_cases}
        default:
            return UnknownEnumToString(value);
    }}
}}
"""

template_enum_value_to_string_case = """case {value}: return {name};"""
exclude_enum_groups = {'SpecialNumbers'}

# Special enum groups that don't have any enum values.
# Add groups here if you get missing group compile errors.
empty_enum_groups = ['SemaphoreParameterName', 'ShaderBinaryFormat']


def dump_value_to_string_mapping(enum_groups, api_enum):
    exporting_groups = list()
    for group_name, inner_mapping in enum_groups.items():
        # Convert to pairs and strip out-of-range values.
        string_value_pairs = list(
            filter(lambda x: x[1] >= 0 and x[1] <= 0xFFFFFFFFF, inner_mapping.items()))
        if not string_value_pairs:
            continue

        # sort according values
        string_value_pairs.sort(key=lambda x: (x[1], len(x[0]), x[0]))

        # remove all duplicate values from the pairs list
        # some value may have more than one GLenum mapped to them, such as:
        #     GL_DRAW_FRAMEBUFFER_BINDING and GL_FRAMEBUFFER_BINDING
        #     GL_BLEND_EQUATION_RGB and GL_BLEND_EQUATION
        # it is safe to output either one of them, for simplity here just
        # choose the shorter one which comes first in the sorted list
        exporting_string_value_pairs = list()
        for index, pair in enumerate(string_value_pairs):
            if index == 0 or pair[1] != string_value_pairs[index - 1][1]:
                exporting_string_value_pairs.append(pair)

        inner_code_block = "\n".join([
            template_enum_value_to_string_case.format(
                value='0x%X' % value,
                name='"%s"' % name,
            ) for name, value in exporting_string_value_pairs
        ])

        exporting_groups.append((group_name, inner_code_block))

    return "\n".join([
        template_enum_group_case.format(
            api_enum=api_enum,
            group_name=group_name,
            inner_group_cases=inner_code_block,
        ) for group_name, inner_code_block in sorted(exporting_groups, key=lambda x: x[0])
    ])


def dump_string_to_value_mapping(enums_and_values):

    def f(value):
        if value < 0:
            return str(value)
        if value < 0xFFFF:
            return "0x%04X" % value
        if value <= 0xFFFFFFFF:
            return "0x%X" % value
        else:
            return "0xFFFFFFFF"

    return '\n'.join('{"%s", %s},' % (k, f(v)) for k, v in sorted(enums_and_values))


def main(header_output_path, source_output_path):
    xml = registry_xml.RegistryXML('gl.xml', 'gl_angle_ext.xml')

    # Compute a list of all GLES enums.
    gles_enums = set()
    bigl_enums = set()
    for feature in xml.root.findall('feature'):
        for require in feature.findall('require'):
            assert 'api' not in require.attrib
            if 'gles' in feature.attrib['api']:
                for enum in require.findall('enum'):
                    gles_enums.add(enum.attrib['name'])
            if feature.attrib['api'] == 'gl':
                for enum in require.findall('enum'):
                    bigl_enums.add(enum.attrib['name'])

    for extensions in xml.root.findall('extensions'):
        for extension in extensions.findall('extension'):
            if extension.attrib['name'] in registry_xml.supported_extensions:
                for require in extension.findall('require'):
                    ext_apis = extension.attrib['supported'].split('|')
                    if ('api' not in require.attrib or 'gles' in require.attrib['api']) and (
                            'gles' in extension.attrib['supported']):
                        for enum in require.findall('enum'):
                            gles_enums.add(enum.attrib['name'])
                    if ('api' not in require.attrib or
                            feature.attrib['api'] == 'gl') and ('gl' in ext_apis):
                        for enum in require.findall('enum'):
                            bigl_enums.add(enum.attrib['name'])

    # Build a map from GLenum name to its value
    gl_enum_groups = dict()
    gles_enum_groups = dict()

    # Add all enums to default groups
    gl_default_enums = dict()
    gles_default_enums = dict()
    gl_enum_groups[registry_xml.default_enum_group_name] = gl_default_enums
    gles_enum_groups[registry_xml.default_enum_group_name] = gles_default_enums
    enums_and_values = []

    for enums_node in xml.root.findall('enums'):
        for enum in enums_node.findall('enum'):
            enum_name = enum.attrib['name']
            enum_value = int(enum.attrib['value'], base=16)
            enums_and_values.append((enum_name, enum_value))

            if enum_name in gles_enums:
                gles_default_enums[enum_name] = enum_value
            if enum_name in bigl_enums:
                gl_default_enums[enum_name] = enum_value

            if 'group' in enum.attrib:
                for enum_group in enum.attrib['group'].split(','):
                    if enum_group in exclude_enum_groups:
                        continue
                    if enum_name in gles_enums:
                        if enum_group not in gles_enum_groups:
                            gles_enum_groups[enum_group] = dict()
                        gles_enum_groups[enum_group][enum_name] = enum_value
                    if enum_name in bigl_enums:
                        if enum_group not in gl_enum_groups:
                            gl_enum_groups[enum_group] = dict()
                        gl_enum_groups[enum_group][enum_name] = enum_value

    for empty_group in empty_enum_groups:
        assert not empty_group in gles_enum_groups or not empty_group in gl_enum_groups, 'Remove %s from the empty groups list, it has enums now.' % empty_group
        if empty_group not in gles_enum_groups:
            gles_enum_groups[empty_group] = dict()
        if empty_group not in gl_enum_groups:
            gl_enum_groups[empty_group] = dict()

    # Write GLenum groups into the header file.
    header_content = template_gl_enums_header.format(
        script_name=os.path.basename(sys.argv[0]),
        data_source_name="gl.xml and gl_angle_ext.xml",
        gles_enum_groups=',\n'.join(sorted(gles_enum_groups.keys())),
        gl_enum_groups=',\n'.join(sorted(gl_enum_groups.keys())))

    header_output_path = registry_xml.script_relative(header_output_path)
    with open(header_output_path, 'w') as f:
        f.write(header_content)

    # Write mapping to source file
    gles_enums_value_to_string_table = dump_value_to_string_mapping(gles_enum_groups, 'GLESEnum')
    gl_enums_value_to_string_table = dump_value_to_string_mapping(gl_enum_groups, 'BigGLEnum')
    string_to_enum_table = dump_string_to_value_mapping(enums_and_values)
    source_content = template_gl_enums_source.format(
        script_name=os.path.basename(sys.argv[0]),
        data_source_name="gl.xml and gl_angle_ext.xml",
        gles_enums_value_to_string_table=gles_enums_value_to_string_table,
        gl_enums_value_to_string_table=gl_enums_value_to_string_table,
        string_to_enum_table=string_to_enum_table,
    )

    source_output_path = registry_xml.script_relative(source_output_path)
    with open(source_output_path, 'w') as f:
        f.write(source_content)

    return 0


if __name__ == '__main__':
    inputs = [
        '../third_party/OpenGL-Registry/src/xml/gl.xml',
        'gl_angle_ext.xml',
        'registry_xml.py',
    ]

    gl_enum_utils_autogen_base_path = '../src/common/gl_enum_utils_autogen'
    outputs = [
        gl_enum_utils_autogen_base_path + '.h',
        gl_enum_utils_autogen_base_path + '.cpp',
    ]

    if len(sys.argv) > 1:
        if sys.argv[1] == 'inputs':
            print(','.join(inputs))
        elif sys.argv[1] == 'outputs':
            print(','.join(outputs))
    else:
        sys.exit(
            main(
                registry_xml.script_relative(outputs[0]),
                registry_xml.script_relative(outputs[1])))
