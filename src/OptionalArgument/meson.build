OptionalArgument_headers = ['hello.hpp']
OptionalArgument_sources = ['hello.cpp']

OptionalArgument_lib = library('OptionalArgument',
				include_directories : inc,
				install : true,
				sources: [OptionalArgument_headers,OptionalArgument_sources])

OptionalArgument_dep = declare_dependency(include_directories : inc,
					   link_with : OptionalArgument_lib)

install_headers(OptionalArgument_headers,
		 subdir : 'OptionalArgument')

