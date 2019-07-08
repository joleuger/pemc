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
#boost_dep = dependency('boost', modules : ['thread', 'system', 'timer'])
boost_dep = dependency('boost', modules : ['system', 'timer', 'align'], version : '>=1.6.8')

microsoft_gsl_dep = dependency('microsoft_gsl', fallback : ['microsoft-gsl', 'microsoft_gsl_dep'])

fmt_dep = dependency('fmt', fallback : ['fmt', 'fmt_dep'])

spdlog_include = include_directories('subprojects/spdlog-1.3.1/include',)
spdlog_dep = declare_dependency(include_directories : spdlog_include, compile_args:'-DSPDLOG_FMT_EXTERNAL', dependencies : [fmt_dep])


# libpemc_include = include_directories('src')
libpemc_include = include_directories('.',
  'subprojects/ThrowAssert'
)

libpemc =  static_library('pemc',
  'pemc/basic/label.cc',
  'pemc/basic/probability.cc',
  'pemc/basic/raw_memory.cc',
  'pemc/formula/formula.cc',
  'pemc/formula/adapted_formula.cc',
  'pemc/formula/unary_formula.cc',
  'pemc/formula/binary_formula.cc',
  'pemc/formula/bounded_unary_formula.cc',
  'pemc/formula/bounded_binary_formula.cc',
  'pemc/formula/formula_to_string_visitor.cc',
  'pemc/formula/generate_label_based_formula_evaluator.cc',
  'pemc/formula/formula_utils.cc',
  'pemc/generic_traverser/generic_traverser.cc',
  'pemc/generic_traverser/path_tracker.cc',
  'pemc/generic_traverser/state_storage.cc',
  'pemc/lcmdp/lcmdp.cc',
  'pemc/lcmdp/lcmdp_model_checker.cc',
  'pemc/lcmdp/lcmdp_to_gv.cc',
  'pemc/lmc/lmc.cc',
  'pemc/lmc/lmc_model_checker.cc',
  'pemc/lmc/lmc_to_gv.cc',
  'pemc/lmc_traverser/add_transitions_to_lmc_modifier.cc',
  'pemc/lmc_traverser/lmc_choice_resolver.cc',
  'pemc/executable_model/abstract_model.cc',
  'pemc/executable_model/model_executor.cc',
  'pemc/executable_model/temporary_state_storage.cc',
  'pemc/pemc.cc',
  include_directories : libpemc_include,
  dependencies: [microsoft_gsl_dep, boost_dep, fmt_dep, spdlog_dep])

libpemc_dep = declare_dependency(link_with: libpemc,
  include_directories: libpemc_include,
  dependencies: [microsoft_gsl_dep, boost_dep, fmt_dep, spdlog_dep] )


libpemcCpp_include = include_directories('language')

libpemcCpp =  static_library('pemcCpp',
  'language/pemc_cpp/cpp_formula.cc',
  'language/pemc_cpp/cpp_model.cc',
  'language/pemc_cpp/generate_slow_cpp_formula_evaluator.cc',
  include_directories : [ libpemc_include, libpemcCpp_include ],
  dependencies: [microsoft_gsl_dep] )

libpemcCpp_dep = declare_dependency(
  link_with: [ libpemc, libpemcCpp ],
  include_directories: [ libpemc_include, libpemcCpp_include ],
  dependencies: [microsoft_gsl_dep, boost_dep])


# TEST
gtest_main_dep = dependency('googletest', fallback : ['googletest', 'gtest_main_dep'])

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
  'tests/lmcTraverser/addTransitionsToLmcModifier.cc',
  'tests/lmcTraverser/lmcChoiceResolver.cc',
  'tests/executableModel/modelExecutor.cc',
  'tests/simpleExecutableModel/generateSlowSimpleFormulaEvaluator.cc',
  'tests/simpleExecutableModel/simpleFormula.cc',
  'tests/simpleExecutableModel/simpleModel.cc',
  'tests/simpleExecutableModel/slowSimpleFormulaEvaluator.cc',
  'tests/pemc.cc'
]

libpemcCpptests = [
  'language/pemc_cpp_tests/test.cc'
]

if meson.get_cross_property('build-tests', 'yes') == 'yes'
   pemcExe = executable('testpemc', libpemctests,
      dependencies : [gtest_main_dep, libpemc_dep])
   test('gtest test', pemcExe)

   pemcCppExe = executable('testpemcCpp', libpemcCpptests,
      dependencies : [gtest_main_dep, libpemcCpp_dep])
   test('gtest test', pemcCppExe)
endif


if meson.get_cross_property('build-examples', 'yes') == 'yes'
   executable('example_dice', 'language/pemc_cpp_examples/dice.cc', dependencies : [libpemcCpp_dep])
endif
