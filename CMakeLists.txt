cmake_minimum_required (VERSION 3.8)
set (CMAKE_CXX_STANDARD 11)
project (ACL)

include_directories (${CMAKE_CURRENT_SOURCE_DIR})

set (nfa_SRC "nfa.cpp")

add_library (nfa_static STATIC ${nfa_SRC})

add_executable (main "test.cpp")

target_link_libraries (main nfa_static)
