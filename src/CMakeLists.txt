cmake_minimum_required (VERSION 2.6)
include_directories("${PROJECT_SOURCE_DIR}/include")

if(MSVC)
set(HEADER
 include/win32/getopt.h
 )
message ("This is MSVC!!!") 
endif(MSVC)
set(SOURCES
    exp.c
    fileutils.c
    syntax_parser.c
    interpreter.c
    lexer.c
    main.c
    preprocessor.c
    tree.c
    generic_list.c
    debug.c
    )
set(HEADERS
	include/
	include/common.h
	include/exp.h
	include/fileutils.h
  include/syntax_parser.h
	include/interpreter.h
	include/lexer.h
	include/preprocessor.h
  include/tree.h
  include/generic_list.h
  include/config.h
  include/debug.h
  include/error.h
    )
  #if(MSVC)
  #set(HEADER
  # include/getopt.h
  # )
  #message ("This is MSVC!!!") 
  #endif(MSVC)
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})
target_include_directories(${PROJECT_NAME} PUBLIC include)
install (TARGETS xlang DESTINATION bin)
