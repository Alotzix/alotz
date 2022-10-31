cmake_minimum_required(VERSION 2.8)
project(alotz)

include (cmake/utils.cmake)

set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_CXX_FLAGS "$ENV{CXXFLAGS} -rdynamic -O3 -ggdb -std=c++11 -Wall -Wno-deprecated -Werror -Wno-unused-function -Wno-builtin-macro-redefined")

include_directories(.)
include_directories(/usr/include)
include_directories(/usr/local/include)
link_directories(/usr/lib64)
link_directories(/usr/local/lib64)

find_package(Boost REQUIRED)
if (BOOST_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
endif()

set(LIB_SRC
    src/address.cc
    src/bytearray.cc
    src/config.cc
    src/fd_manager.cc
    src/fiber.cc
    src/http/http.cc
    src/http/http_connection.cc
    src/http/http_parser.cc
    src/http/http_session.cc
    src/http/http_server.cc
    src/http/servlet.cc
    src/hook.cc
    src/iomanager.cc
    src/log.cc
    src/mutex.cc
    src/scheduler.cc
    src/socket.cc
    src/socket_stream.cc
    src/stream.cc
    src/tcp_server.cc
    src/timer.cc
    src/thread.cc
    src/util.cc    
)

ragelmaker(src/http/http11_parser.rl LIB_SRC ${CMAKE_CURRENT_SOURCE_DIR}/src/http)
ragelmaker(src/http/httpclient_parser.rl LIB_SRC ${CMAKE_CURRENT_SOURCE_DIR}/src/http)
ragelmaker(src/uri.rl LIB_SRC ${CMAKE_CURRENT_SOURCE_DIR}/src)

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

alotz_add_executable(test_config "test/test_config.cc" alotz "${LIBS}")
alotz_add_executable(test_thread "test/test_thread.cc" alotz "${LIBS}")
alotz_add_executable(test_util "test/test_util.cc" alotz "${LIBS}")
alotz_add_executable(test_fiber "test/test_fiber.cc" alotz "${LIBS}")
alotz_add_executable(test_scheduler "test/test_scheduler.cc" alotz "${LIBS}")
alotz_add_executable(test_iomanager "test/test_iomanager.cc" alotz "${LIBS}")
alotz_add_executable(test_hook "test/test_hook.cc" alotz "${LIBS}")
alotz_add_executable(test_address "test/test_address.cc" alotz "${LIBS}")
alotz_add_executable(test_socket "test/test_socket.cc" alotz "${LIBS}")
alotz_add_executable(test_bytearray "test/test_bytearray.cc" alotz "${LIBS}")
alotz_add_executable(test_http "test/test_http.cc" alotz "${LIBS}")
alotz_add_executable(test_http_parser "test/test_http_parser.cc" alotz "${LIBS}")
alotz_add_executable(test_tcp_server "test/test_tcp_server.cc" alotz "${LIBS}")
alotz_add_executable(echo_server "examples/echo_server.cc" alotz "${LIBS}")
alotz_add_executable(test_http_server "test/test_http_server.cc" alotz "${LIBS}")
alotz_add_executable(test_http_connection "test/test_http_connection.cc" alotz "${LIBS}")
alotz_add_executable(test_uri "test/test_uri.cc" alotz "${LIBS}")

SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR/bin})
SET(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR/lib})