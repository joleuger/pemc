project('tutorial', 'cpp')

# gtest_dep = dependency('gtest', main : true, required : false)
gtest_include = include_directories('googletest/googletest/include')

gtest_build_include= include_directories('googletest/googletest','googletest/googletest/include')
gtest_dep = declare_dependency(sources : ['googletest/googletest/src/gtest-all.cc','googletest/googletest/src/gtest_main.cc'], include_directories : gtest_build_include)


if meson.get_cross_property('build-tests', 'yes') == 'yes'
   e = executable('testprog', 'src/test.cpp', dependencies : gtest_dep)
   test('gtest test', e)
endif
