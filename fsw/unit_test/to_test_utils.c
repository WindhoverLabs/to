
#include "to_test_utils.h"
#include "to_custom_stubs.h"
#include "to_custom_hooks.h"
#include "to_platform_cfg.h"
#include "ut_cfe_evs_hooks.h"
#include "ut_cfe_time_stubs.h"
#include "ut_cfe_psp_memutils_stubs.h"
#include "ut_cfe_tbl_stubs.h"
#include "ut_cfe_tbl_hooks.h"
#include "ut_cfe_fs_stubs.h"
#include "ut_cfe_time_stubs.h"
#include "ut_osapi_stubs.h"
#include "ut_osfileapi_stubs.h"
#include "ut_cfe_sb_stubs.h"
#include "ut_cfe_es_stubs.h"
#include "ut_cfe_evs_stubs.h"
#include "to_table_names.h"

#include <time.h>


/*
 * Config table for testing
 */
#define TO_PQUEUE_SINGLE_PASS_IDX       0
#define TO_PQUEUE_HIGH_OPS_RSRVD_IDX    1
#define TO_PQUEUE_HIGH_IDX              2
#define TO_PQUEUE_MEDIUM_IDX            3
#define TO_PQUEUE_LOW_IDX               4


TO_ChannelTbl_t TO_EmptyConfigTable =
{
    /* Table ID */
    1,
    {
        /* Message Flows */
        {0, 0, 0}
    },{
        /* Priority Queues */
        {TO_PQUEUE_ENA, 10, TO_PRIORITY_QUEUE_TYPE_SINGLE},
        {TO_PQUEUE_ENA, 10, TO_PRIORITY_QUEUE_TYPE_FIFO},
        {TO_PQUEUE_ENA, 10, TO_PRIORITY_QUEUE_TYPE_FIFO},
        {TO_PQUEUE_ENA, 10, TO_PRIORITY_QUEUE_TYPE_FIFO},
        {TO_PQUEUE_ENA, 10, TO_PRIORITY_QUEUE_TYPE_FIFO}
    }
};


TO_ChannelTbl_t TO_FullConfigTable1 =
{
    /* Table ID */
    1,
    {
        /* Message Flows */
        {CFE_ES_HK_TLM_MID,         1,  TO_PQUEUE_HIGH_IDX},
        {CFE_EVS_HK_TLM_MID,        1,  TO_PQUEUE_HIGH_IDX},
        {CFE_SB_HK_TLM_MID,         1,  TO_PQUEUE_HIGH_IDX},
        {CFE_TBL_HK_TLM_MID,        1,  TO_PQUEUE_MEDIUM_IDX},
        {CFE_TIME_HK_TLM_MID,       1,  TO_PQUEUE_MEDIUM_IDX},
        {CFE_TIME_DIAG_TLM_MID,     1,  TO_PQUEUE_MEDIUM_IDX},
        {CFE_EVS_EVENT_MSG_MID,     32, TO_PQUEUE_LOW_IDX},
        {CFE_SB_STATS_TLM_MID,      1,  TO_PQUEUE_LOW_IDX},
        {CFE_ES_APP_TLM_MID,        1,  TO_PQUEUE_LOW_IDX}
    },{
        /* Priority Queues */
        {TO_PQUEUE_ENA, 10, TO_PRIORITY_QUEUE_TYPE_SINGLE},
        {TO_PQUEUE_ENA, 10, TO_PRIORITY_QUEUE_TYPE_FIFO},
        {TO_PQUEUE_ENA, 2, TO_PRIORITY_QUEUE_TYPE_FIFO},
        {TO_PQUEUE_ENA, 3, TO_PRIORITY_QUEUE_TYPE_FIFO},
        {TO_PQUEUE_ENA, 4, TO_PRIORITY_QUEUE_TYPE_FIFO}
    }
};


TO_ChannelTbl_t TO_FullConfigTable2 =
{
	/* Table ID */
	1,
	{
		/* Message Flows */
		{CFE_ES_HK_TLM_MID,			1,	TO_PQUEUE_HIGH_IDX},
		{CFE_EVS_HK_TLM_MID,		1,	TO_PQUEUE_HIGH_IDX},
		{CFE_SB_HK_TLM_MID,			1,	TO_PQUEUE_HIGH_IDX},
		{CFE_TBL_HK_TLM_MID,		1,	TO_PQUEUE_MEDIUM_IDX},
		{CFE_TIME_HK_TLM_MID,		1,	TO_PQUEUE_MEDIUM_IDX},
		{CFE_TIME_DIAG_TLM_MID,		1,	TO_PQUEUE_MEDIUM_IDX},
		{CFE_EVS_EVENT_MSG_MID,		32, TO_PQUEUE_LOW_IDX},
		{CFE_SB_STATS_TLM_MID,		1,	TO_PQUEUE_LOW_IDX},
		{CFE_ES_APP_TLM_MID,		1,	TO_PQUEUE_LOW_IDX}
	},{
		/* Priority Queues */
		{TO_PQUEUE_ENA, 10, TO_PRIORITY_QUEUE_TYPE_SINGLE},
		{TO_PQUEUE_ENA, 10, TO_PRIORITY_QUEUE_TYPE_FIFO},
		{TO_PQUEUE_ENA, 4, TO_PRIORITY_QUEUE_TYPE_FIFO},
		{TO_PQUEUE_ENA, 3, TO_PRIORITY_QUEUE_TYPE_FIFO},
		{TO_PQUEUE_ENA, 2, TO_PRIORITY_QUEUE_TYPE_FIFO}
	}
};


/*
 * Function Definitions
 */

void TO_Test_Setup_NoConfig(void)
{
    /* initialize test environment to default state for every test */

    CFE_PSP_MemSet(&TO_AppData, 0x00, sizeof(TO_AppData_t));
    CFE_PSP_MemSet(&TO_Custom_Test_Returns, 0x00, sizeof(TO_Custom_Test_Returns));
    CFE_PSP_MemSet(&TO_Custom_Test_Hooks, 0x00, sizeof(TO_Custom_Test_Hooks));

    Ut_CFE_EVS_Reset();
    Ut_CFE_FS_Reset();
    Ut_CFE_TIME_Reset();
    Ut_CFE_TBL_Reset();
    Ut_CFE_SB_Reset();
    Ut_CFE_ES_Reset();
    Ut_OSAPI_Reset();
    Ut_OSFILEAPI_Reset();
    
    /* Clear queues, semaphores, etc and call init */
    Ut_OS_API_Clear();
}


void TO_Test_Setup_EmptyConfig(void)
{
    /* initialize test environment to default state for every test */

    CFE_PSP_MemSet(&TO_AppData, 0x00, sizeof(TO_AppData_t));
    CFE_PSP_MemSet(&TO_Custom_Test_Returns, 0x00, sizeof(TO_Custom_Test_Returns));
    CFE_PSP_MemSet(&TO_Custom_Test_Hooks, 0x00, sizeof(TO_Custom_Test_Hooks));

    Ut_CFE_EVS_Reset();
    Ut_CFE_FS_Reset();
    Ut_CFE_TIME_Reset();
    Ut_CFE_TBL_Reset();
    Ut_CFE_SB_Reset();
    Ut_CFE_ES_Reset();
    Ut_OSAPI_Reset();
    Ut_OSFILEAPI_Reset();
    
    /* Clear queues, semaphores, etc and call init */
    Ut_OS_API_Clear();

    Ut_CFE_TBL_AddTable(TO_CONFIG_TABLE_FILENAME, (void *) &TO_EmptyConfigTable);
}


void TO_Test_Setup_FullConfig1(void)
{
    /* initialize test environment to default state for every test */

    CFE_PSP_MemSet(&TO_AppData, 0x00, sizeof(TO_AppData_t));
    CFE_PSP_MemSet(&TO_Custom_Test_Returns, 0x00, sizeof(TO_Custom_Test_Returns));
    CFE_PSP_MemSet(&TO_Custom_Test_Hooks, 0x00, sizeof(TO_Custom_Test_Hooks));
    
    Ut_CFE_EVS_Reset();
    Ut_CFE_FS_Reset();
    Ut_CFE_TIME_Reset();
    Ut_CFE_TBL_Reset();
    Ut_CFE_SB_Reset();
    Ut_CFE_ES_Reset();
    Ut_OSAPI_Reset();
    Ut_OSFILEAPI_Reset();
    
    /* Clear queues, semaphores, etc and call init */
    Ut_OS_API_Clear();

    Ut_CFE_TBL_AddTable(TO_CONFIG_TABLE_FILENAME, (void *) &TO_FullConfigTable1);
}


void TO_Test_Setup_FullConfig2(void)
{
    /* initialize test environment to default state for every test */

    CFE_PSP_MemSet(&TO_AppData, 0x00, sizeof(TO_AppData_t));
    CFE_PSP_MemSet(&TO_Custom_Test_Returns, 0x00, sizeof(TO_Custom_Test_Returns));
    CFE_PSP_MemSet(&TO_Custom_Test_Hooks, 0x00, sizeof(TO_Custom_Test_Hooks));

    Ut_CFE_EVS_Reset();
    Ut_CFE_FS_Reset();
    Ut_CFE_TIME_Reset();
    Ut_CFE_TBL_Reset();
    Ut_CFE_SB_Reset();
    Ut_CFE_ES_Reset();
    Ut_OSAPI_Reset();
    Ut_OSFILEAPI_Reset();
    
    /* Clear queues, semaphores, etc and call init */
    Ut_OS_API_Clear();

    Ut_CFE_TBL_AddTable(TO_CONFIG_TABLE_FILENAME, (void *) &TO_FullConfigTable2);
}


void TO_Test_TearDown(void) 
{
    CFE_PSP_MemSet(&TO_AppData, 0x00, sizeof(TO_AppData_t));
}
