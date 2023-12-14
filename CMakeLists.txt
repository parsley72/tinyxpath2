cmake_minimum_required(VERSION 3.17)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(TinyXPath
  VERSION 1.3.1
  LANGUAGES CXX
  )

include_directories(tinyxml)

add_library(tinyxpath STATIC htmlutil.cpp tinyxml/tinystr.cpp tinyxml/tinyxml.cpp tinyxml/tinyxmlerror.cpp tinyxml/tinyxmlparser.cpp action_store.cpp lex_util.cpp node_set.cpp tokenlist.cpp xml_util.cpp xpath_expression.cpp xpath_processor.cpp xpath_stack.cpp xpath_stream.cpp xpath_syntax.cpp xpath_static.cpp action_store.h byte_stream.h htmlutil.h lex_token.h lex_util.h node_set.h tinyxpath_conf.h tokenlist.h xml_util.h xpath_expression.h xpath_processor.h xpath_stack.h xpath_stream.h xpath_syntax.h)

set_target_properties(tinyxpath PROPERTIES PUBLIC_HEADER "tinyxml/tinyxml.h;tinystr.h;xpath_static.h")

install(TARGETS tinyxpath
  PUBLIC_HEADER DESTINATION include
  ARCHIVE DESTINATION lib/${LIBRARY_SUBDIR})

add_executable(test_tinyxpath main.cpp htmlutil.cpp tinyxml/tinystr.cpp tinyxml/tinyxml.cpp tinyxml/tinyxmlerror.cpp tinyxml/tinyxmlparser.cpp action_store.cpp lex_util.cpp node_set.cpp tokenlist.cpp xml_util.cpp xpath_expression.cpp xpath_processor.cpp xpath_stack.cpp xpath_stream.cpp xpath_syntax.cpp xpath_static.cpp action_store.h byte_stream.h htmlutil.h lex_token.h lex_util.h node_set.h tinyxpath_conf.h tokenlist.h xml_util.h xpath_expression.h xpath_processor.h xpath_stack.h xpath_stream.h xpath_syntax.h)
