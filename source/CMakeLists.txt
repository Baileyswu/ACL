cmake_minimum_required (VERSION 3.8)

set (nfa_SRC "nfa.cpp")

add_library (nfa_static STATIC ${nfa_SRC})