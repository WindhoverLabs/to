
add_airliner_app_def(to
    FILE TO
    SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/../src/to_app.c
        ${CMAKE_CURRENT_SOURCE_DIR}/../src/to_config_utils.c
        ${CMAKE_CURRENT_SOURCE_DIR}/../src/to_message_flow.c
        ${CMAKE_CURRENT_SOURCE_DIR}/../src/to_classifier.c
        ${CMAKE_CURRENT_SOURCE_DIR}/../src/to_priority_queue.c
        ${CMAKE_CURRENT_SOURCE_DIR}/../src/to_scheduler.c
        ${CMAKE_CURRENT_SOURCE_DIR}/../src/to_output_channel.c
   
    INCLUDES
        ${CMAKE_CURRENT_SOURCE_DIR}/../src/

    USER_DOCS   ${CMAKE_CURRENT_SOURCE_DIR}/user_doxy.in
)



add_airliner_app_unit_test(TO-UT
    UTF 
    SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/../unit_test/to_app_test
        ${CMAKE_CURRENT_SOURCE_DIR}/../unit_test/to_cmds_test.c
        ${CMAKE_CURRENT_SOURCE_DIR}/../unit_test/to_config_tbl_test.c
        ${CMAKE_CURRENT_SOURCE_DIR}/../unit_test/to_test_utils.c
        ${CMAKE_CURRENT_SOURCE_DIR}/../unit_test/to_testrunner.c
        ${CMAKE_CURRENT_SOURCE_DIR}/../src/to_app.c
        ${CMAKE_CURRENT_SOURCE_DIR}/../src/to_classifier.c
        ${CMAKE_CURRENT_SOURCE_DIR}/../src/to_message_flow.c
        ${CMAKE_CURRENT_SOURCE_DIR}/../src/to_output_channel.c
        ${CMAKE_CURRENT_SOURCE_DIR}/../src/to_priority_queue.c
        ${CMAKE_CURRENT_SOURCE_DIR}/../src/to_scheduler.c
        ${CMAKE_CURRENT_SOURCE_DIR}/../src/to_config_utils.c
        
    INCLUDES
        ${CMAKE_CURRENT_SOURCE_DIR}/../src/
        ${CMAKE_CURRENT_SOURCE_DIR}/../unit_test/
)

