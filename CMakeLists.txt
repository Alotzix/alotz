cmake_minimum_required(VERSION 2.8)
project(alotz)

set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_CXX_FLAGS "$ENV{CXXFLAGS} -rdynamic -O0 -ggdb -std=c++11 -Wall -Wno-deprecated -Werror -Wno-unused-function")

include_directories(.)
include_directories(/usr/include)
include_directories(/usr/local/include)
link_directories(/usr/lib64)
link_directories(/usr/local/lib)

find_library(YAMLCPP yaml-cpp)
message("***", ${YAMLCPP})

set(LIB_SRC
    src/log.cc
    src/util.cc
    src/config.cc
)

add_library(alotz SHARED ${LIB_SRC})

add_executable(test test/test.cc)
add_dependencies(test alotz)
target_link_libraries(test alotz ${YAMLCPP})

add_executable(test_config test/test_config.cc)
add_dependencies(test_config alotz)
target_link_libraries(test_config alotz ${YAMLCPP})

SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR/bin})
SET(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR/lib})