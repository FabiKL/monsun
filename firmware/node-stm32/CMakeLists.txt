cmake_minimum_required(VERSION 3.16)
project(monsun_node_stm32 C ASM)

set(MONSUN_PORT "" CACHE STRING "Port to build: g431rb, l152re")

add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/../libs/monsun_link monsun_link)

if(MONSUN_PORT STREQUAL "")
    message(WARNING "No MONSUN_PORT selected; ports are not implemented yet (see README.md).")
else()
    message(FATAL_ERROR "Port '${MONSUN_PORT}' is not implemented yet.")
endif()
