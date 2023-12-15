cmake_minimum_required(VERSION 3.17)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(TinyXPath2
  VERSION 1.3.1
  LANGUAGES CXX
  )

# set(CMAKE_BUILD_TYPE "Debug")

include_directories(tinyxml2)

add_library(tinyxpath2 STATIC htmlutil.cpp tinyxml2/tinyxml2.cpp action_store.cpp lex_util.cpp node_set.cpp tokenlist.cpp xml_util.cpp xpath_expression.cpp xpath_processor.cpp xpath_stack.cpp xpath_stream.cpp xpath_syntax.cpp xpath_static.cpp action_store.h byte_stream.h htmlutil.h lex_token.h lex_util.h node_set.h tinyxpath_conf.h tokenlist.h xml_util.h xpath_expression.h xpath_processor.h xpath_stack.h xpath_stream.h xpath_syntax.h)

set_target_properties(tinyxpath2 PROPERTIES PUBLIC_HEADER "tinyxml2/tinyxml2.h;xpath_static.h")

install(TARGETS tinyxpath2
  PUBLIC_HEADER DESTINATION include
  ARCHIVE DESTINATION lib/${LIBRARY_SUBDIR})

add_executable(test_tinyxpath2 main.cpp htmlutil.cpp tinyxml2/tinyxml2.cpp action_store.cpp lex_util.cpp node_set.cpp tokenlist.cpp xml_util.cpp xpath_expression.cpp xpath_processor.cpp xpath_stack.cpp xpath_stream.cpp xpath_syntax.cpp xpath_static.cpp action_store.h byte_stream.h htmlutil.h lex_token.h lex_util.h node_set.h tinyxpath_conf.h tokenlist.h xml_util.h xpath_expression.h xpath_processor.h xpath_stack.h xpath_stream.h xpath_syntax.h)
