
add_airliner_app_def(to
    FILE TO
    SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/../src/to_app.c
        ${CMAKE_CURRENT_SOURCE_DIR}/../src/to_app.h
        ${CMAKE_CURRENT_SOURCE_DIR}/../src/to_channel.c
        ${CMAKE_CURRENT_SOURCE_DIR}/../src/to_channel.h
        ${CMAKE_CURRENT_SOURCE_DIR}/../src/to_classifier.c
        ${CMAKE_CURRENT_SOURCE_DIR}/../src/to_classifier.h
        ${CMAKE_CURRENT_SOURCE_DIR}/../src/to_config_utils.c
        ${CMAKE_CURRENT_SOURCE_DIR}/../src/to_config_utils.h
        ${CMAKE_CURRENT_SOURCE_DIR}/../src/to_custom.h
        ${CMAKE_CURRENT_SOURCE_DIR}/../src/to_events.h
        ${CMAKE_CURRENT_SOURCE_DIR}/../src/to_message_flow.c
        ${CMAKE_CURRENT_SOURCE_DIR}/../src/to_message_flow.h
        ${CMAKE_CURRENT_SOURCE_DIR}/../src/to_msg.h
        ${CMAKE_CURRENT_SOURCE_DIR}/../src/to_output_queue.c
        ${CMAKE_CURRENT_SOURCE_DIR}/../src/to_output_queue.h
        ${CMAKE_CURRENT_SOURCE_DIR}/../src/to_priority_queue.c
        ${CMAKE_CURRENT_SOURCE_DIR}/../src/to_priority_queue.h
        ${CMAKE_CURRENT_SOURCE_DIR}/../src/to_private_ids.h
        ${CMAKE_CURRENT_SOURCE_DIR}/../src/to_private_types.h
        ${CMAKE_CURRENT_SOURCE_DIR}/../src/to_scheduler.c
        ${CMAKE_CURRENT_SOURCE_DIR}/../src/to_scheduler.h
        ${CMAKE_CURRENT_SOURCE_DIR}/../src/to_tbldefs.h
        ${CMAKE_CURRENT_SOURCE_DIR}/../src/to_version.h
        
    INCLUDES
        ${CMAKE_CURRENT_SOURCE_DIR}/../src/
        
    USER_DOCS   ${CMAKE_CURRENT_SOURCE_DIR}/user_doxy.in   
    DESIGN_DOCS   ${CMAKE_CURRENT_SOURCE_DIR}/detail_doxy.in      
)


add_airliner_app_unit_test(to-ut
    UTASSERT 
    SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/../unit_test/to_app_test.c
        ${CMAKE_CURRENT_SOURCE_DIR}/../unit_test/to_app_test.h
        ${CMAKE_CURRENT_SOURCE_DIR}/../unit_test/to_cmds_test.c
        ${CMAKE_CURRENT_SOURCE_DIR}/../unit_test/to_cmds_test.h
        ${CMAKE_CURRENT_SOURCE_DIR}/../unit_test/to_config_tbl_test.c
        ${CMAKE_CURRENT_SOURCE_DIR}/../unit_test/to_config_tbl_test.h
        ${CMAKE_CURRENT_SOURCE_DIR}/../unit_test/to_test_utils.c
        ${CMAKE_CURRENT_SOURCE_DIR}/../unit_test/to_test_utils.h
        ${CMAKE_CURRENT_SOURCE_DIR}/../unit_test/to_testrunner.c
        ${CMAKE_CURRENT_SOURCE_DIR}/../unit_test/to_custom_stubs.c
        ${CMAKE_CURRENT_SOURCE_DIR}/../unit_test/ut_osapi_hooks.c
        ${CMAKE_CURRENT_SOURCE_DIR}/../unit_test/ut_osapi_hooks.h
        ${CMAKE_CURRENT_SOURCE_DIR}/../unit_test/ut_cfe_es_hooks_ex.c
        ${CMAKE_CURRENT_SOURCE_DIR}/../unit_test/ut_cfe_es_hooks_ex.h
        ${CMAKE_CURRENT_SOURCE_DIR}/../src/to_app.c
        ${CMAKE_CURRENT_SOURCE_DIR}/../src/to_app.h
        ${CMAKE_CURRENT_SOURCE_DIR}/../src/to_channel.c
        ${CMAKE_CURRENT_SOURCE_DIR}/../src/to_channel.h
        ${CMAKE_CURRENT_SOURCE_DIR}/../src/to_classifier.c
        ${CMAKE_CURRENT_SOURCE_DIR}/../src/to_classifier.h
        ${CMAKE_CURRENT_SOURCE_DIR}/../src/to_message_flow.c
        ${CMAKE_CURRENT_SOURCE_DIR}/../src/to_message_flow.h
        ${CMAKE_CURRENT_SOURCE_DIR}/../src/to_output_queue.c
        ${CMAKE_CURRENT_SOURCE_DIR}/../src/to_output_queue.h
        ${CMAKE_CURRENT_SOURCE_DIR}/../src/to_priority_queue.c
        ${CMAKE_CURRENT_SOURCE_DIR}/../src/to_priority_queue.h
        ${CMAKE_CURRENT_SOURCE_DIR}/../src/to_scheduler.c
        ${CMAKE_CURRENT_SOURCE_DIR}/../src/to_scheduler.h
        ${CMAKE_CURRENT_SOURCE_DIR}/../src/to_config_utils.c
        ${CMAKE_CURRENT_SOURCE_DIR}/../src/to_config_utils.h
        
    INCLUDES
        ${CMAKE_CURRENT_SOURCE_DIR}/../src/
        ${CMAKE_CURRENT_SOURCE_DIR}/../unit_test/
)

