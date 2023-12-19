cmake_minimum_required(VERSION 3.17)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(TinyXPath2
  VERSION 1.3.1
  LANGUAGES CXX
  )

# set(CMAKE_BUILD_TYPE "Debug")

set(TINYXPATH2_HEADERS action_store.h byte_stream.h htmlutil.h lex_token.h lex_util.h node_set.h tinyxpath_conf.h tokenlist.h xml_util.h xpath_expression.h xpath_processor.h xpath_stack.h xpath_static.h xpath_stream.h xpath_syntax.h)

add_library(tinyxpath2 STATIC htmlutil.cpp action_store.cpp lex_util.cpp node_set.cpp tokenlist.cpp xml_util.cpp xpath_expression.cpp xpath_processor.cpp xpath_stack.cpp xpath_stream.cpp xpath_syntax.cpp xpath_static.cpp ${TINYXPATH2_HEADERS})

set_target_properties(tinyxpath2 PROPERTIES PUBLIC_HEADER "action_store.h;byte_stream.h;htmlutil.h;lex_token.h;lex_util.h;node_set.h;tinyxpath_conf.h;tokenlist.h;xml_util.h;xpath_expression.h;xpath_processor.h;xpath_stack.h;xpath_static.h;xpath_stream.h;xpath_syntax.h")

install(TARGETS tinyxpath2
  PUBLIC_HEADER DESTINATION include
  ARCHIVE DESTINATION lib/${LIBRARY_SUBDIR})

if (CMAKE_BUILD_TYPE EQUAL "Debug")
  add_executable(test_tinyxpath2 main.cpp htmlutil.cpp action_store.cpp lex_util.cpp node_set.cpp tokenlist.cpp xml_util.cpp xpath_expression.cpp xpath_processor.cpp xpath_stack.cpp xpath_stream.cpp xpath_syntax.cpp xpath_static.cpp ${TINYXPATH2_HEADERS})
endif()
