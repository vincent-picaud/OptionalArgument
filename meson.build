project('OptionalArgument', 'cpp',
	version : '0.0.1',
	default_options : ['cpp_std=c++17'])

add_project_arguments(['-pedantic'], language : 'cpp')

gtest_proj = subproject('gtest')
gtest_dep = gtest_proj.get_variable('gtest_main_dep')

subdir('src')
subdir('test')
subdir('examples')

