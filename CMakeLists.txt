cmake_minimum_required(VERSION 3.17)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(TinyXPath2
  VERSION 1.3.1
  LANGUAGES CXX
  )

# set(CMAKE_MESSAGE_LOG_LEVEL "STATUS")
# set(CMAKE_BUILD_TYPE "Debug")

configure_file(tinyxpath2.pc.in tinyxpath2.pc @ONLY)

include_directories(tinyxml2)

set(TINYXPATH2_CODE htmlutil.cpp action_store.cpp lex_util.cpp node_set.cpp tokenlist.cpp xml_util.cpp xpath_expression.cpp xpath_processor.cpp xpath_stack.cpp xpath_stream.cpp xpath_syntax.cpp xpath_static.cpp)

add_library(tinyxpath2 STATIC ${TINYXPATH2_CODE})
target_include_directories(tinyxpath2 PRIVATE .)

set_target_properties(tinyxpath2 PROPERTIES PUBLIC_HEADER "action_store.h;byte_stream.h;htmlutil.h;lex_token.h;lex_util.h;node_set.h;tinyxpath_conf.h;tokenlist.h;xml_util.h;xpath_expression.h;xpath_processor.h;xpath_stack.h;xpath_static.h;xpath_stream.h;xpath_syntax.h")

install(TARGETS tinyxpath2
  PUBLIC_HEADER DESTINATION include
  ARCHIVE DESTINATION lib/${LIBRARY_SUBDIR})
install(FILES ${CMAKE_BINARY_DIR}/tinyxpath2.pc DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/pkgconfig)

message(WARNING "CMAKE_BUILD_TYPE: [${CMAKE_BUILD_TYPE}]")

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
  add_executable(test_tinyxpath2 main.cpp tinyxml2/tinyxml2.cpp ${TINYXPATH2_CODE})
endif()
