cmake_minimum_required (VERSION 2.6)
include_directories("${PROJECT_SOURCE_DIR}/include")
set(SOURCES
    exp.c
    fileutils.c
    interpreter.c
    lexer.c
    main.c
    preprocessor.c
    tree.c
    )
set(HEADERS
	include/
	include/common.h
	include/exp.h
	include/fileutils.h
	include/interpreter.h
	include/lexer.h
	include/preprocessor.h
  include/tree.h
    )
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})
target_include_directories(${PROJECT_NAME} PUBLIC include)
install (TARGETS xlang DESTINATION bin)
