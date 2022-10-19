cmake_minimum_required(VERSION 2.8)
project(alotz)

include (cmake/utils.cmake)

set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_CXX_FLAGS "$ENV{CXXFLAGS} -rdynamic -O0 -ggdb -std=c++11 -Wall -Wno-deprecated -Werror -Wno-unused-function -Wno-builtin-macro-redefined")

include_directories(.)
include_directories(/usr/include)
include_directories(/usr/local/include)
link_directories(/usr/lib64)
link_directories(/usr/local/lib64)

set(LIB_SRC
    src/log.cc
    src/util.cc
    src/config.cc
    src/thread.cc
    src/fiber.cc
    src/scheduler.cc
    src/iomanager.cc
    src/timer.cc
    src/address.cc
    src/fd_manager.cc
    src/hook.cc
    src/bytearray.cc
    src/socket.cc
)

add_library(alotz SHARED ${LIB_SRC})
force_redefine_file_macro_for_sources(alotz) #__FILE__

find_library(YAMLCPP yaml-cpp)
find_library(PTHREAD pthread)
#set(LIBS ${LIBS} alotz)
#set(LIBS ${LIBS} dl)
#set(LIBS ${LIBS} ${YAMLCPP})
#set(LIBS ${LIBS} ${PTHREAD})

set(LIBS
    alotz
    dl
    yaml-cpp
    pthread
)

message("***", ${LIBS})


add_executable(test_util test/test_util.cc)
add_dependencies(test_util alotz)
force_redefine_file_macro_for_sources(test_util) #__FILE__
target_link_libraries(test_util ${LIBS})

add_executable(test_config test/test_config.cc)
add_dependencies(test_config alotz)
force_redefine_file_macro_for_sources(test_config) #__FILE__
target_link_libraries(test_config ${LIBS})

add_executable(test_thread test/test_thread.cc)
add_dependencies(test_thread alotz)
force_redefine_file_macro_for_sources(test_thread) #__FILE__
target_link_libraries(test_thread ${LIBS})

add_executable(test_fiber test/test_fiber.cc)
add_dependencies(test_fiber alotz)
force_redefine_file_macro_for_sources(test_fiber) #__FILE__
target_link_libraries(test_fiber ${LIBS})

add_executable(test_scheduler test/test_scheduler.cc)
add_dependencies(test_scheduler alotz)
force_redefine_file_macro_for_sources(test_scheduler) #__FILE__
target_link_libraries(test_scheduler ${LIBS})

add_executable(test_iomanager test/test_iomanager.cc)
add_dependencies(test_iomanager alotz)
force_redefine_file_macro_for_sources(test_iomanager) #__FILE__
target_link_libraries(test_iomanager ${LIBS})

add_executable(test_address test/test_address.cc)
add_dependencies(test_address alotz)
force_redefine_file_macro_for_sources(test_address) #__FILE__
target_link_libraries(test_address ${LIBS})

add_executable(test_hook test/test_hook.cc)
add_dependencies(test_hook alotz)
force_redefine_file_macro_for_sources(test_hook) #__FILE__
target_link_libraries(test_hook ${LIBS})

add_executable(test_bytearray test/test_bytearray.cc)
add_dependencies(test_bytearray alotz)
force_redefine_file_macro_for_sources(test_bytearray) #__FILE__
target_link_libraries(test_bytearray ${LIBS})

add_executable(test_socket test/test_socket.cc)
add_dependencies(test_socket alotz)
force_redefine_file_macro_for_sources(test_socket) #__FILE__
target_link_libraries(test_socket ${LIBS})

SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR/bin})
SET(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR/lib})