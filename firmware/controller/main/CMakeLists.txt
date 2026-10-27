idf_component_register(
    SRCS "main.c"
    INCLUDE_DIRS "." "../boards"
    REQUIRES driver esp_timer monsun_libs monsun_registry
)
