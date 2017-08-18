
#include "to_test_utils.h"

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

#include <time.h>

/*
 * Config table for testing
 */
#define TO_PQUEUE_GROUND_SINGLE_PASS_IDX		0
#define TO_PQUEUE_GROUND_HIGH_OPS_RSRVD_IDX		1
#define TO_PQUEUE_GROUND_HIGH_IDX				2
#define TO_PQUEUE_GROUND_MEDIUM_IDX				3
#define TO_PQUEUE_GROUND_LOW_IDX				4
#define TO_PQUEUE_ONBOARD_SINGLE_PASS_IDX		5
#define TO_PQUEUE_ONBOARD_HIGH_OPS_RSRVD_IDX	6
#define TO_PQUEUE_ONBOARD_HIGH_IDX				7
#define TO_PQUEUE_ONBOARD_MEDIUM_IDX			8
#define TO_PQUEUE_ONBOARD_LOW_IDX				9

#define TO_CHANNEL_GROUND						0
#define TO_CHANNEL_ONBOARD						1

TO_ConfigTbl_t TO_EmptyConfigTable =
{
	/* Table ID */
	1,
	{
		/* Message Flows */
		{0, 0, 0, 0, 0, 0}
	},{
		/* Priority Queues */
		/* TO_PQUEUE_GROUND_SINGLE_PASS_IDX */
		{TO_PQUEUE_ENA, TO_CHANNEL_GROUND, 10, TO_PRIORITY_QUEUE_TYPE_SINGLE, 0, 0},
		/* TO_PQUEUE_GROUND_HIGH_OPS_RSRVD_IDX */
		{TO_PQUEUE_ENA, TO_CHANNEL_GROUND, 10, TO_PRIORITY_QUEUE_TYPE_FIFO, 0, 0},
		/* TO_PQUEUE_GROUND_HIGH_IDX */
		{TO_PQUEUE_ENA, TO_CHANNEL_GROUND, 10, TO_PRIORITY_QUEUE_TYPE_FIFO, 0, 0},
		/* TO_PQUEUE_GROUND_MEDIUM_IDX */
		{TO_PQUEUE_ENA, TO_CHANNEL_GROUND, 10, TO_PRIORITY_QUEUE_TYPE_FIFO, 0, 0},
		/* TO_PQUEUE_GROUND_LOW_IDX */
		{TO_PQUEUE_ENA, TO_CHANNEL_GROUND, 10, TO_PRIORITY_QUEUE_TYPE_FIFO, 0, 0},
		/* TO_PQUEUE_ONBOARD_SINGLE_PASS_IDX */
		{TO_PQUEUE_ENA, TO_CHANNEL_ONBOARD, 10, TO_PRIORITY_QUEUE_TYPE_SINGLE, 0, 0},
		/* TO_PQUEUE_GROUND_HIGH_OPS_RSRVD_IDX */
		{TO_PQUEUE_ENA, TO_CHANNEL_ONBOARD, 10, TO_PRIORITY_QUEUE_TYPE_FIFO, 0, 0},
		/* TO_PQUEUE_GROUND_HIGH_IDX */
		{TO_PQUEUE_ENA, TO_CHANNEL_ONBOARD, 10, TO_PRIORITY_QUEUE_TYPE_FIFO, 0, 0},
		/* TO_PQUEUE_GROUND_MEDIUM_IDX */
		{TO_PQUEUE_ENA, TO_CHANNEL_ONBOARD, 10, TO_PRIORITY_QUEUE_TYPE_FIFO, 0, 0},
		/* TO_PQUEUE_GROUND_LOW_IDX */
		{TO_PQUEUE_ENA, TO_CHANNEL_ONBOARD, 10, TO_PRIORITY_QUEUE_TYPE_FIFO, 0, 0},
	},
	{
		/* Output Channels */
		/* TO_CHANNEL_GROUND */
		{10, 0, 0, 0},
		/* TO_CHANNEL_ONBOARD */
		{10, 0, 0, 0}
	}
};



TO_ConfigTbl_t TO_FullConfigTable =
{
	/* Table ID */
	1,
	{
		/* Message Flows */
		/* Ground Queues */
		{CFE_ES_HK_TLM_MID,			1,	TO_PQUEUE_GROUND_HIGH_IDX, 0, 0, 0},
		{CFE_EVS_HK_TLM_MID,		1,	TO_PQUEUE_GROUND_HIGH_IDX, 0, 0, 0},
		{CFE_SB_HK_TLM_MID,			1,	TO_PQUEUE_GROUND_HIGH_IDX, 0, 0, 0},
		{CFE_TBL_HK_TLM_MID,		1,	TO_PQUEUE_GROUND_MEDIUM_IDX, 0, 0, 0},
		{CFE_TIME_HK_TLM_MID,		1,	TO_PQUEUE_GROUND_MEDIUM_IDX, 0, 0, 0},
		{CFE_TIME_DIAG_TLM_MID,		1,	TO_PQUEUE_GROUND_MEDIUM_IDX, 0, 0, 0},
		{CFE_EVS_EVENT_MSG_MID,		32, TO_PQUEUE_GROUND_LOW_IDX, 0, 0, 0},
		{CFE_SB_STATS_TLM_MID,		1,	TO_PQUEUE_GROUND_LOW_IDX, 0, 0, 0},
		{CFE_ES_APP_TLM_MID,		1,	TO_PQUEUE_GROUND_LOW_IDX, 0, 0, 0},

		/* Onboard Queues */
		{CFE_TBL_REG_TLM_MID,		1,	TO_PQUEUE_ONBOARD_HIGH_IDX, 0, 0, 0},
		{CFE_SB_ONESUB_TLM_MID,		1,	TO_PQUEUE_ONBOARD_HIGH_IDX, 0, 0, 0},
		{CFE_ES_SHELL_TLM_MID,		32, TO_PQUEUE_ONBOARD_HIGH_IDX, 0, 0, 0},
		{CFE_ES_MEMSTATS_TLM_MID,	1,	TO_PQUEUE_ONBOARD_MEDIUM_IDX, 0, 0, 0},
		{CF_HK_TLM_MID,				1,	TO_PQUEUE_ONBOARD_MEDIUM_IDX, 0, 0, 0},
		{CF_TRANS_TLM_MID,			1,	TO_PQUEUE_ONBOARD_MEDIUM_IDX, 0, 0, 0},
		{CF_CONFIG_TLM_MID,			1,	TO_PQUEUE_ONBOARD_LOW_IDX, 0, 0, 0},
		{CF_SPACE_TO_GND_PDU_MID,	32,	TO_PQUEUE_ONBOARD_LOW_IDX, 0, 0, 0},
		{CS_HK_TLM_MID,				1,	TO_PQUEUE_ONBOARD_LOW_IDX, 0, 0, 0},
	},{
		/* Priority Queues */
		/* TO_PQUEUE_GROUND_SINGLE_PASS_IDX */
		{TO_PQUEUE_ENA, TO_CHANNEL_GROUND, 10, TO_PRIORITY_QUEUE_TYPE_SINGLE, 0, 0},
		/* TO_PQUEUE_GROUND_HIGH_OPS_RSRVD_IDX */
		{TO_PQUEUE_ENA, TO_CHANNEL_GROUND, 10, TO_PRIORITY_QUEUE_TYPE_FIFO, 0, 0},
		/* TO_PQUEUE_GROUND_HIGH_IDX */
		{TO_PQUEUE_ENA, TO_CHANNEL_GROUND, 2, TO_PRIORITY_QUEUE_TYPE_FIFO, 0, 0},
		/* TO_PQUEUE_GROUND_MEDIUM_IDX */
		{TO_PQUEUE_ENA, TO_CHANNEL_GROUND, 3, TO_PRIORITY_QUEUE_TYPE_FIFO, 0, 0},
		/* TO_PQUEUE_GROUND_LOW_IDX */
		{TO_PQUEUE_ENA, TO_CHANNEL_GROUND, 4, TO_PRIORITY_QUEUE_TYPE_FIFO, 0, 0},
		/* TO_PQUEUE_ONBOARD_SINGLE_PASS_IDX */
		{TO_PQUEUE_ENA, TO_CHANNEL_ONBOARD, 10, TO_PRIORITY_QUEUE_TYPE_SINGLE, 0, 0},
		/* TO_PQUEUE_GROUND_HIGH_OPS_RSRVD_IDX */
		{TO_PQUEUE_ENA, TO_CHANNEL_ONBOARD, 10, TO_PRIORITY_QUEUE_TYPE_FIFO, 0, 0},
		/* TO_PQUEUE_GROUND_HIGH_IDX */
		{TO_PQUEUE_ENA, TO_CHANNEL_ONBOARD, 4, TO_PRIORITY_QUEUE_TYPE_FIFO, 0, 0},
		/* TO_PQUEUE_GROUND_MEDIUM_IDX */
		{TO_PQUEUE_ENA, TO_CHANNEL_ONBOARD, 3, TO_PRIORITY_QUEUE_TYPE_FIFO, 0, 0},
		/* TO_PQUEUE_GROUND_LOW_IDX */
		{TO_PQUEUE_ENA, TO_CHANNEL_ONBOARD, 2, TO_PRIORITY_QUEUE_TYPE_FIFO, 0, 0},
	},
	{
		/* Output Channels */
		/* TO_CHANNEL_GROUND */
		{10, 0, 0, 0},
		/* TO_CHANNEL_ONBOARD */
		{10, 0, 0, 0}
	}
};

/*
 * Function Definitions
 */

void TO_Test_Setup_EmptyConfig(void)
{
    /* initialize test environment to default state for every test */

    CFE_PSP_MemSet(&TO_AppData, 0x00, sizeof(TO_AppData_t));

    Ut_CFE_EVS_Reset();
    Ut_CFE_FS_Reset();
    Ut_CFE_TIME_Reset();
    Ut_CFE_TBL_Reset();
    Ut_CFE_SB_Reset();
    Ut_CFE_ES_Reset();
    Ut_OSAPI_Reset();
    Ut_OSFILEAPI_Reset();

    Ut_CFE_TBL_AddTable(TO_CONFIG_TABLE_FILENAME, (void *) &TO_EmptyConfigTable);
}

void TO_Test_Setup_FullConfig(void)
{
    /* initialize test environment to default state for every test */

    CFE_PSP_MemSet(&TO_AppData, 0x00, sizeof(TO_AppData_t));

    Ut_CFE_EVS_Reset();
    Ut_CFE_FS_Reset();
    Ut_CFE_TIME_Reset();
    Ut_CFE_TBL_Reset();
    Ut_CFE_SB_Reset();
    Ut_CFE_ES_Reset();
    Ut_OSAPI_Reset();
    Ut_OSFILEAPI_Reset();

    Ut_CFE_TBL_AddTable(TO_CONFIG_TABLE_FILENAME, (void *) &TO_FullConfigTable);
}

void TO_Test_TearDown(void) {
    CFE_PSP_MemSet(&TO_AppData, 0x00, sizeof(TO_AppData_t));
}
