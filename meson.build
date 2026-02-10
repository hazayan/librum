project('Librum', 'cpp',
  version: '0.1',
  default_options: [
    'cpp_std=c++20',
    'warning_level=3',
    'buildtype=debugoptimized',
  ]
)

# Build options
build_tests = get_option('build_tests')
enable_coverage = get_option('enable_coverage')
use_sanitizers = get_option('use_sanitizers')

# Qt6 dependencies
qt6 = import('qt6')
qt6_dep = dependency('qt6',
  modules: ['Core', 'Quick', 'Widgets', 'Network', 'Gui', 'QuickControls2'],
  required: true
)

# Compiler flags
cpp_args = []
link_args = []

# Platform-specific flags
if host_machine.system() == 'windows'
  cpp_args += [
    '-Zc:rvalueCast',
    '-Zc:inline',
    '-Zc:strictStrings',
    '-Zc:throwingNew',
    '-permissive-',
    '-Zc:__cplusplus',
    '-Zc:externConstexpr',
    '-utf-8',
    '-w34100',
    '-w34189',
    '-w44996',
    '-w44456',
    '-w44457',
    '-w44458'
  ]
else
  cpp_args += [
    '-Wall',
    '-Wextra',
    '-Wshadow',
    '-Wnon-virtual-dtor',
    '-Wold-style-cast',
    '-Wcast-align',
    '-Wunused',
    '-Woverloaded-virtual',
    '-Wpedantic',
    '-Wmissing-include-dirs',
    '-Wnull-dereference',
    '-Wformat=2',
    '-Wcast-qual',
    '-Winit-self',
    '-Wswitch-enum',
    '-Wunreachable-code',
    '-Wredundant-decls',
  ]

  if meson.get_compiler('cpp').get_id() == 'gcc'
    cpp_args += [
      '-Wduplicated-cond',
      '-Wduplicated-branches',
      '-Wunsafe-loop-optimizations',
      '-Wlogical-op',
    ]
  endif
endif

# Coverage instrumentation
if enable_coverage
  cpp_args += [
    '-fprofile-arcs',
    '-ftest-coverage',
    '-fno-inline',
    '--coverage'
  ]
  link_args += [
    '-lgcov',
    '--coverage'
  ]
endif

# Sanitizers
if use_sanitizers
  sanitizer_args = [
    '-fsanitize=address,undefined',
    '-fsanitize=shift,integer-divide-by-zero',
    '-fsanitize=float-divide-by-zero,unreachable,vla-bound,null,return,leak',
    '-fsanitize=bounds,float-cast-overflow,enum'
  ]
  cpp_args += sanitizer_args
  link_args += sanitizer_args
endif

# System dependencies
# MuPDF - use system library
mupdf_dep = dependency('mupdf', required: false)

if not mupdf_dep.found()
  # Fallback to finding the library directly
  mupdf_lib = meson.get_compiler('cpp').find_library('mupdf', required: true)
  mupdfcpp_lib = meson.get_compiler('cpp').find_library('mupdfcpp', required: true)
  mupdf_dep = declare_dependency(
    dependencies: [mupdfcpp_lib, mupdf_lib],
    compile_args: ['-I/usr/include/mupdf', '-DNDEBUG']
  )
else
  # pkg-config found mupdf, but we still need mupdfcpp
  mupdfcpp_lib = meson.get_compiler('cpp').find_library('mupdfcpp', required: true)
  mupdf_dep = declare_dependency(
    dependencies: [mupdfcpp_lib, mupdf_dep],
    compile_args: ['-I/usr/include/mupdf', '-DNDEBUG']
  )
endif

# rapidfuzz-cpp - header-only library, check for system installation
rapidfuzz_dep = declare_dependency(
  compile_args: ['-I/usr/include']
)

# Boost.DI - header-only library
# Check for Boost.DI in standard include paths
di_dep = declare_dependency(
  compile_args: ['-I/usr/include', '-I/usr/local/include', '-I/usr/include/boost-di']
)

# googletest for tests - use system package
if build_tests
  gtest_dep = dependency('gtest', main: true, required: true)
else
  gtest_dep = dependency('', required: false)
endif

# Add subdirectories
subdir('src')

if build_tests
  subdir('tests')
endif

# Summary
message('''
Librum build configuration:
  Build tests: @0@
  Coverage: @1@
  Sanitizers: @2@
  Platform: @3@
  Build type: @4@
'''.format(
  build_tests,
  enable_coverage,
  use_sanitizers,
  host_machine.system(),
  get_option('buildtype')
))
