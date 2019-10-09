cmake_minimum_required (VERSION 3.8)

set(NFA_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../source)

include_directories (${CMAKE_CURRENT_SOURCE_DIR})
include_directories (${NFA_DIR})
include_directories (${PTHREAD_DIR})

file(GLOB NFA_SRC "${NFA_DIR}/nfa.cpp")
file(GLOB XSocket_SRC "${NFA_DIR}/XSocket.cpp")

add_executable (cs-clientbin "client.cpp" ${XSocket_SRC})
add_executable (cs-serverbin "server.cpp" ${NFA_SRC} ${XSocket_SRC})

target_link_libraries (cs-clientbin nfa_static)
target_link_libraries (cs-serverbin nfa_static)
