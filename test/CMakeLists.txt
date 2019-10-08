cmake_minimum_required (VERSION 3.8)

set(NFA_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../source)

include_directories (${CMAKE_CURRENT_SOURCE_DIR})
include_directories (${NFA_DIR})

file(GLOB NFA_SRC "${NFA_DIR}/*.cpp")

add_executable (testbin "test.cpp" ${NFA_SRC})

target_link_libraries (testbin nfa_static)
