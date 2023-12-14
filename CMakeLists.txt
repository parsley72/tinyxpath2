cmake_minimum_required(VERSION 3.17)

project(TinyXPath
  VERSION 1.3.1
  LANGUAGES CXX
  )

add_library(tinyxpath STATIC htmlutil.cpp main.cpp tinystr.cpp tinyxml.cpp tinyxmlerror.cpp tinyxmlparser.cpp action_store.cpp lex_util.cpp node_set.cpp tokenlist.cpp xml_util.cpp xpath_expression.cpp xpath_processor.cpp xpath_stack.cpp xpath_stream.cpp xpath_syntax.cpp xpath_static.cpp action_store.h byte_stream.h htmlutil.h lex_token.h lex_util.h node_set.h tinyxpath_conf.h tokenlist.h xml_util.h xpath_expression.h xpath_processor.h xpath_stack.h xpath_stream.h xpath_syntax.h)

set_target_properties(tinyxpath PROPERTIES PUBLIC_HEADER "tinyxml.h;tinystr.h;xpath_static.h")

install(TARGETS tinyxpath
  PUBLIC_HEADER DESTINATION include
  ARCHIVE DESTINATION lib/${LIBRARY_SUBDIR})
