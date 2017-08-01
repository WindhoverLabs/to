
add_airliner_app_def(to
    FILE TO
    SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/../src/to_app.c
        ${CMAKE_CURRENT_SOURCE_DIR}/../src/to_cds_utils.c
        ${CMAKE_CURRENT_SOURCE_DIR}/../src/to_config_utils.c
        ${CMAKE_CURRENT_SOURCE_DIR}/../src/to_classifier.c
        ${CMAKE_CURRENT_SOURCE_DIR}/../src/to_scheduler.c
   
    INCLUDES
        ${CMAKE_CURRENT_SOURCE_DIR}/../src/
)



