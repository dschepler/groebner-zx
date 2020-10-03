project('groebner', 'cpp')
gmpxxdep = dependency('gmpxx')
groebner_lib = static_library('groebnerlib',
			      'buchberger.cpp',
			      'polynomial.cpp',
			      dependencies : gmpxxdep)
executable('groebner', 'main.cpp', link_with : groebner_lib, install : true)

gtestdep = dependency('gtest_main', required : false)
if gtestdep.found()
  testexe = executable('groebner_test',
		       'buchberger_test.cpp',
		       'polynomial_test.cpp',
		       link_with : groebner_lib,
		       dependencies : gtestdep)
  test('groebner_test', testexe)
endif
