# Copyright © 2017-2018 Intel Corporation

# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:

# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.

# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.

prog_python = import('python').find_installation('python3')

g_version_h = custom_target(
  'g_version.h',
  output: 'g_version.h',
  command: [
    prog_python,
    meson.source_root() / 'sgpu' / 'gen_version.py',
    '--component', 'LIBDRM',
    '--file', meson.current_build_dir() / 'g_version.h'
  ]
)

datadir_sgpu = join_paths(get_option('prefix'), get_option('datadir'), 'libdrm')

cc = meson.get_compiler('c')

dep_ion = cc.find_library('ion', required : false)

libdrm_sgpu = library(
  'drm_sgpu',
  [
    files(
      'amdgpu_asic_id.c', 'amdgpu_bo.c', 'amdgpu_cs.c', 'amdgpu_device.c',
      'amdgpu_gpu_info.c', 'amdgpu_vamgr.c', 'amdgpu_vm.c', 'handle_table.c', 'sgpu_sysfs.c'
    ),
    config_file,
    g_version_h,
  ],
  c_args : [
    libdrm_c_args,
    '-DAMDGPU_ASIC_ID_TABLE="@0@"'.format(join_paths(datadir_sgpu, 'amdgpu.ids')),
  ],
  include_directories : [inc_root, inc_drm, inc_ion, inc_kern_ion],
  link_with : libdrm,
  dependencies : [dep_pthread_stubs, dep_atomic_ops, dep_ion],
  version : '1.0.0',
  install : true,
)

install_headers('amdgpu.h', subdir : 'libdrm')

pkg.generate(
  name : 'libdrm_sgpu',
  libraries : libdrm_sgpu,
  subdirs : ['.', 'libdrm'],
  version : meson.project_version(),
  requires_private : 'libdrm',
  description : 'Userspace interface to kernel DRM services for sgpu',
)

ext_libdrm_sgpu = declare_dependency(
  link_with : [libdrm, libdrm_sgpu],
  include_directories : [inc_drm, include_directories('.')],
)

test(
  'amdgpu-symbols-check',
  symbols_check,
  args : [
    '--lib', libdrm_sgpu,
    '--symbols-file', files('amdgpu-symbols.txt'),
    '--nm', prog_nm.path(),
  ],
)
