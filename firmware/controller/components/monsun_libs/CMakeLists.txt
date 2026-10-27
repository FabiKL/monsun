idf_component_register(
    SRCS
        "${CMAKE_CURRENT_LIST_DIR}/../../../libs/monsun_engine/src/monsun_engine.c"
        "${CMAKE_CURRENT_LIST_DIR}/../../../libs/monsun_link/src/monsun_link.c"
    INCLUDE_DIRS
        "${CMAKE_CURRENT_LIST_DIR}/../../../libs/monsun_engine/include"
        "${CMAKE_CURRENT_LIST_DIR}/../../../libs/monsun_link/include"
)
