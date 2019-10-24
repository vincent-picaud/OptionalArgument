test_array = [['optional_argument_test','optional_argument_exe','optional_argument.cpp']]

foreach test : test_array
  test(test.get(0),
       executable(test.get(1),
		  test.get(2),
		  dependencies
 		  : [ OptionalArgument_dep, gtest_dep ]))
endforeach
