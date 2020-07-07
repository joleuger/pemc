project('pemc', 'c', 'cpp',
        default_options : ['c_std=c11', 'cpp_std=c++17'])

# if get_option('buildtype')=='release'
if not get_option('buildtype').startswith('debug')
  message('Deactivating Ensures and Expects of GSL')
  add_global_arguments('-DGSL_UNENFORCED_ON_CONTRACT_VIOLATION', language : 'cpp')
endif

if get_option('buildtype').startswith('debug')
  add_project_arguments('-DDEBUG', language : 'cpp')
endif

# external dependencies
#boost_dep = dependency('boost', modules : ['thread', 'system', 'timer'])
boost_dep = dependency('boost', modules : ['system', 'timer'], version : '>=1.6.8')

microsoft_gsl_dep = dependency('microsoft_gsl', fallback : ['microsoft-gsl', 'microsoft_gsl_dep'])

fmt_dep = dependency('fmt', fallback : ['fmt', 'fmt_dep'])

spdlog_include = include_directories('subprojects/spdlog-1.3.1/include',)
spdlog_dep = declare_dependency(include_directories : spdlog_include, compile_args:'-DSPDLOG_FMT_EXTERNAL', dependencies : [fmt_dep])


# platform specific dependencies
if target_machine.system() == 'windows'
  message('Target machine is Windows')
  # Idea from https://github.com/radareorg/radare2/blob/master/meson.build
  cc = meson.get_compiler('cpp')
  bcrypt_dep = cc.find_library('bcrypt')
  #ucrt_dep = cc.find_library('ucrt')
  platform_specific_dep = declare_dependency(
    link_with: [],
    dependencies: [bcrypt_dep],
    include_directories: []
  )
else
  message('Target machine is not Windows.')
  platform_specific_dep = declare_dependency(
    link_with: [],
    dependencies: [],
    include_directories: []
  )
endif


# libpemc_include = include_directories('src')
libpemc_include = include_directories('.')

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
  dependencies: [microsoft_gsl_dep, boost_dep, fmt_dep, spdlog_dep, platform_specific_dep])
  

libpemc_dep = declare_dependency(link_with: libpemc,
  include_directories: libpemc_include,
  dependencies: [microsoft_gsl_dep, boost_dep, fmt_dep, spdlog_dep] )

# If lib_pemc gets build as a shared library, then define PEMC_DLL of
# the file pemc/basic/dll_defines.h.
# TODO: Switch libpemc from shared_library to library and set global flag depending on
# the option default_library on https://mesonbuild.com/Builtin-options.html, which decides
# whether library means shared_library or static_library.
#add_project_arguments('-DPEMC_DLL', language : 'c')
#add_project_arguments('-DPEMC_DLL', language : 'cpp')


libpemc_shared = shared_library('pemc_shared',
    'pemc/c_api/c_api.cc',
  include_directories : libpemc_include,
  dependencies: [libpemc_dep, microsoft_gsl_dep, boost_dep, fmt_dep, spdlog_dep, platform_specific_dep],
  c_args : ['-DPEMC_DLL', '-DPEMC_DLL_EXPORTS'],
  cpp_args : ['-DPEMC_DLL', '-DPEMC_DLL_EXPORTS']
)

libpemc_shared_dep = declare_dependency(link_with: libpemc_shared,
  include_directories: libpemc_include,
  dependencies: [microsoft_gsl_dep, boost_dep, fmt_dep, spdlog_dep] )

libpemcCpp_include = include_directories('language')

libpemcCpp =  static_library('pemcCpp',
  'language/pemc_cpp/cpp_formula.cc',
  'language/pemc_cpp/cpp_model.cc',
  'language/pemc_cpp/generate_slow_cpp_formula_evaluator.cc',
  include_directories : [ libpemc_include, libpemcCpp_include ],
  dependencies: [libpemc_dep, microsoft_gsl_dep] )

libpemcCpp_dep = declare_dependency(
  link_with: [ libpemc, libpemcCpp ],
  include_directories: [ libpemc_include, libpemcCpp_include ],
  dependencies: [microsoft_gsl_dep, boost_dep])

# Build Python modules
pymod=import('python')
py_installation = pymod.find_installation('python3', required:false, disabler:true)

if py_installation.found()
  message('Python found')
  python_dep = py_installation.dependency()

  libpypemc_include = include_directories('language')

  # Python module does not need to be linked against the Library, because it opens the library during runtime
  # as a plugin. But: Dependencies must be installed with the module.
  libpypemc =  py_installation.extension_module('pypemc',
    'language/pemc_c/pemc_c.c',
    'language/pemc_python/pemcmodule.c',
    include_directories : [ libpemc_include, libpypemc_include ],
    dependencies: [python_dep, platform_specific_dep] )

endif


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
   executable('example_dice', 'language/pemc_cpp_examples/dice.cc', dependencies : [libpemc_dep, libpemcCpp_dep])
endif
