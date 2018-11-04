project('pemc', 'cpp',
  default_options : ['cpp_std=c++14'])

# if get_option('buildtype')=='release'
if not get_option('buildtype').startswith('debug')
  print('Deactivating Ensures and Expects of GSL')
  add_global_arguments('-DGSL_UNENFORCED_ON_CONTRACT_VIOLATION', language : 'cpp')
endif

if get_option('buildtype').startswith('debug')
  add_project_arguments('-DDEBUG', language : 'cpp')
endif

# external dependencies
#boostdep = dependency('boost', modules : ['thread', 'system', 'timer'])
boostdep = dependency('boost', modules : ['system', 'timer', 'align'], version : '>=1.6.8')

# libpemc_include = include_directories('src')
libpemc_include = include_directories('.',
  'external/GSL/include',
  'external/ThrowAssert',
#  'external/range/include'
)

libpemc =  static_library('pemc',
  'pemc/basic/label.cc',
  'pemc/basic/probability.cc',
  'pemc/basic/rawMemory.cc',
  'pemc/formula/formula.cc',
  'pemc/formula/adaptedFormula.cc',
  'pemc/formula/unaryFormula.cc',
  'pemc/formula/binaryFormula.cc',
  'pemc/formula/boundedUnaryFormula.cc',
  'pemc/formula/boundedBinaryFormula.cc',
  'pemc/formula/formulaToStringVisitor.cc',
  'pemc/formula/generateLabelBasedFormulaEvaluator.cc',
  'pemc/formula/formulaUtils.cc',
  'pemc/genericTraverser/genericTraverser.cc',
  'pemc/genericTraverser/pathTracker.cc',
  'pemc/genericTraverser/stateStorage.cc',
  'pemc/genericTraverser/temporaryStateStorage.cc',
  'pemc/lcmdp/lcmdp.cc',
  'pemc/lcmdp/lcmdpModelChecker.cc',
  'pemc/lcmdp/lcmdpToGv.cc',
  'pemc/lmc/lmc.cc',
  'pemc/lmc/lmcModelChecker.cc',
  'pemc/lmc/lmcToGv.cc',
  'pemc/lmcTraverser/addTransitionsToLmcModifier.cc',
  'pemc/lmcTraverser/lmcChoiceResolver.cc',
  'pemc/executableModel/abstractModel.cc',
  'pemc/executableModel/modelExecutor.cc',
  'pemc/pemc.cc',
  include_directories : libpemc_include)
libpemc_dep = declare_dependency(link_with: libpemc, include_directories: libpemc_include, dependencies: boostdep)


libpemcCpp_include = include_directories('language')

libpemcCpp =  static_library('pemcCpp',
  'language/pemcCpp/cppFormula.cc',
  'language/pemcCpp/generateSlowCppFormulaEvaluator.cc',
  include_directories : [ libpemc_include, libpemcCpp_include ] )

libpemcCpp_dep = declare_dependency(
  link_with: [ libpemc, libpemcCpp ],
  include_directories: [ libpemc_include, libpemcCpp_include ])


# TEST
gtest_include = include_directories('external/googletest/googletest/include')

gtest_build_include= include_directories('external/googletest/googletest','external/googletest/googletest/include')
gtest_dep = declare_dependency(sources : ['external/googletest/googletest/src/gtest-all.cc','external/googletest/googletest/src/gtest_main.cc'], include_directories : gtest_build_include)

libpemctests = [
  'tests/test.cc',
  'tests/formula/createUuids.cc',
  'tests/formula/formulaToString.cc',
  'tests/formula/labelBasedFormulaEvaluator.cc',
  'tests/genericTraverser/genericTraverser.cc',
  'tests/genericTraverser/pathTracker.cc',
  'tests/genericTraverser/stateStorage.cc',
  'tests/lmc/lmcExamples.cc',
  'tests/lmc/lmc.cc',
  'tests/lmc/lmcModelChecker.cc',
  'tests/lcmdp/lcmdp.cc',
  'tests/lcmdp/lcmdpModelChecker.cc',
  'tests/simpleExecutableModel/generateSlowSimpleFormulaEvaluator.cc',
  'tests/simpleExecutableModel/simpleFormula.cc'
]

libpemcCpptests = [
  'language/pemcCppTests/test.cc'
]

if meson.get_cross_property('build-tests', 'yes') == 'yes'
   pemcExe = executable('testpemc', libpemctests,
      dependencies : [gtest_dep, libpemc_dep])
   test('gtest test', pemcExe)

   pemcCppExe = executable('testpemcCpp', libpemcCpptests,
      dependencies : [gtest_dep, libpemcCpp_dep])
   test('gtest test', pemcCppExe)
endif
