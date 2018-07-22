project('pemc', 'cpp',
  default_options : ['cpp_std=c++14'])

# libpemc_include = include_directories('src')
libpemc_include = include_directories('.')

libpemc =  static_library('pemc',
  'pemc/lcmdp/lcmdp.cc',
  'pemc/lmc/lmc.cc',
  'pemc/lmc/lmcModelChecker.cc',
  'pemc/lmc/lmcToGv.cc',
  'pemc/lcmdp/lcmdp.cc',
  'pemc/lcmdp/lcmdpModelChecker.cc',
  'pemc/lcmdp/lcmdpToGv.cc',
  include_directories : libpemc_include)
libpemc_dep = declare_dependency(link_with: libpemc, include_directories: libpemc_include)



# TEST
gtest_include = include_directories('googletest/googletest/include')

gtest_build_include= include_directories('googletest/googletest','googletest/googletest/include')
gtest_dep = declare_dependency(sources : ['googletest/googletest/src/gtest-all.cc','googletest/googletest/src/gtest_main.cc'], include_directories : gtest_build_include)

tests = [
  'tests/test.cc',
  'tests/lmc/lmc.cc',
  'tests/lmc/lmcModelChecker.cc',
  'tests/lcmdp/lcmdp.cc',
  'tests/lcmdp/lcmdpModelChecker.cc'
]

if meson.get_cross_property('build-tests', 'yes') == 'yes'
   e = executable('testprog', tests,
      dependencies : [gtest_dep, libpemc_dep])
   test('gtest test', e)
endif
