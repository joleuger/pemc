project('pemc', 'cpp')


libpemc_include = include_directories('src')
libpemc =  static_library('pemc',
  'src/lcmdp/lcmdp.cc',
  'src/lmc/lmc.cc',
  include_directories : libpemc_include)
libpemc_dep = declare_dependency(link_with: libpemc, include_directories: libpemc_include)



# TEST
gtest_include = include_directories('googletest/googletest/include')

gtest_build_include= include_directories('googletest/googletest','googletest/googletest/include')
gtest_dep = declare_dependency(sources : ['googletest/googletest/src/gtest-all.cc','googletest/googletest/src/gtest_main.cc'], include_directories : gtest_build_include)


if meson.get_cross_property('build-tests', 'yes') == 'yes'
   e = executable('testprog', 'src/test.cc', dependencies : [gtest_dep, libpemc_dep])
   test('gtest test', e)
endif
