/****************************************************************************
*
*   Copyright (c) 2017 Windhover Labs, L.L.C. All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions
* are met:
*
* 1. Redistributions of source code must retain the above copyright
*    notice, this list of conditions and the following disclaimer.
* 2. Redistributions in binary form must reproduce the above copyright
*    notice, this list of conditions and the following disclaimer in
*    the documentation and/or other materials provided with the
*    distribution.
* 3. Neither the name Windhover Labs nor the names of its 
*    contributors may be used to endorse or promote products derived 
*    from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
* "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
* LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
* FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
* COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
* INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
* BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
* OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
* AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
* LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
* ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
* POSSIBILITY OF SUCH DAMAGE.
*
*****************************************************************************/

/************************************************************************
** Pragmas
*************************************************************************/

/************************************************************************
** Includes
*************************************************************************/
#include <string.h>

#include "cfe.h"

#include "to_app.h"
#include "to_msg.h"
#include "to_version.h"
#include "to_platform_cfg.h"
#include "to_message_flow.h"
#include "to_classifier.h"
#include "to_priority_queue.h"
#include "to_scheduler.h"
#include "to_output_queue.h"
#include "to_custom.h"
#include "to_channel.h"
#include "pb_lib.h"

/************************************************************************
** Local Defines
*************************************************************************/

/************************************************************************
** Local Structure Declarations
*************************************************************************/

/************************************************************************
** External Global Variables
*************************************************************************/

/************************************************************************
** Global Variables
*************************************************************************/
TO_AppData_t  TO_AppData;

/************************************************************************
** Local Variables
*************************************************************************/
uint32  TO_MemPoolDefSize[TO_MAX_MEMPOOL_BLK_SIZES] =
{
    TO_MAX_BLOCK_SIZE,
	TO_MEM_BLOCK_SIZE_07,
    TO_MEM_BLOCK_SIZE_06,
	TO_MEM_BLOCK_SIZE_05,
	TO_MEM_BLOCK_SIZE_04,
	TO_MEM_BLOCK_SIZE_03,
	TO_MEM_BLOCK_SIZE_02,
	TO_MEM_BLOCK_SIZE_01
};

/************************************************************************
** Local Function Definitions
*************************************************************************/

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Initialize event tables                                         */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int32 TO_InitEvent()
{
    int32  iStatus=CFE_SUCCESS;
    int32  ind = 0;

    /* Initialize the event filter table.
     * Note: 0 is the CFE_EVS_NO_FILTER mask and event 0 is reserved (not used) */
    memset((void*)TO_AppData.EventTbl, 0x00, sizeof(TO_AppData.EventTbl));

    /* TODO: Choose the events you want to filter.  CFE_EVS_MAX_EVENT_FILTERS
     * limits the number of filters per app.  An explicit CFE_EVS_NO_FILTER 
     * (the default) has been provided as an example. */
    TO_AppData.EventTbl[  ind].EventID = TO_GET_POOL_ERR_EID;
    TO_AppData.EventTbl[ind++].Mask    = CFE_EVS_FIRST_4_STOP;

    TO_AppData.EventTbl[  ind].EventID = TO_INF_EID;
    TO_AppData.EventTbl[ind++].Mask    = CFE_EVS_NO_FILTER;

    TO_AppData.EventTbl[  ind].EventID = TO_CONFIG_TABLE_ERR_EID;
    TO_AppData.EventTbl[ind++].Mask    = CFE_EVS_NO_FILTER;

    TO_AppData.EventTbl[  ind].EventID = TO_PIPE_ERR_EID;
    TO_AppData.EventTbl[ind++].Mask    = CFE_EVS_NO_FILTER;

    TO_AppData.EventTbl[  ind].EventID = TO_MSGID_ERR_EID;
    TO_AppData.EventTbl[ind++].Mask    = CFE_EVS_NO_FILTER;

    TO_AppData.EventTbl[  ind].EventID = TO_MSGLEN_ERR_EID;
    TO_AppData.EventTbl[ind++].Mask    = CFE_EVS_NO_FILTER;

    TO_AppData.EventTbl[  ind].EventID = TO_MSGLEN_ERR_EID;
    TO_AppData.EventTbl[ind++].Mask    = CFE_EVS_NO_FILTER;

    /* Register the table with CFE */
    iStatus = CFE_EVS_Register(TO_AppData.EventTbl,
                               TO_EVT_CNT, CFE_EVS_BINARY_FILTER);
    if (iStatus != CFE_SUCCESS)
    {
        (void) CFE_ES_WriteToSysLog("TO - Failed to register with EVS (0x%08X)\n", (unsigned int)iStatus);
    }

    return (iStatus);
}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Initialize Message Pipes                                        */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int32 TO_InitPipe()
{
    int32  iStatus=CFE_SUCCESS;

    /* Init schedule pipe and subscribe to wakeup messages */
    iStatus = CFE_SB_CreatePipe(&TO_AppData.SchPipeId,
                                 TO_SCH_PIPE_DEPTH,
                                 TO_SCH_PIPE_NAME);
    if (iStatus == CFE_SUCCESS)
    {
        iStatus = CFE_SB_SubscribeEx(TO_SEND_TLM_MID, TO_AppData.SchPipeId, CFE_SB_Default_Qos, TO_SCH_PIPE_WAKEUP_RESERVED);

        if (iStatus != CFE_SUCCESS)
        {
            (void) CFE_EVS_SendEvent(TO_INIT_ERR_EID, CFE_EVS_ERROR,
                                     "Sch Pipe failed to subscribe to TO_SEND_TLM_MID. (0x%08X)",
                                     (unsigned int)iStatus);
            goto TO_InitPipe_Exit_Tag;
        }

        iStatus = CFE_SB_SubscribeEx(TO_SEND_HK_MID, TO_AppData.SchPipeId, CFE_SB_Default_Qos, TO_SCH_PIPE_SEND_HK_RESERVED);

        if (iStatus != CFE_SUCCESS)
        {
            (void) CFE_EVS_SendEvent(TO_INIT_ERR_EID, CFE_EVS_ERROR,
                                     "CMD Pipe failed to subscribe to TO_SEND_HK_MID. (0x%08X)",
                                     (unsigned int)iStatus);
            goto TO_InitPipe_Exit_Tag;
        }

    }
    else
    {
        (void) CFE_EVS_SendEvent(TO_INIT_ERR_EID, CFE_EVS_ERROR,
                                 "Failed to create SCH pipe (0x%08X)",
                                 (unsigned int)iStatus);
        goto TO_InitPipe_Exit_Tag;
    }

    /* Init command pipe and subscribe to command messages */
    iStatus = CFE_SB_CreatePipe(&TO_AppData.CmdPipeId,
                                 TO_CMD_PIPE_DEPTH,
                                 TO_CMD_PIPE_NAME);
    if (iStatus == CFE_SUCCESS)
    {
        /* Subscribe to command messages */
        iStatus = CFE_SB_Subscribe(TO_CMD_MID, TO_AppData.CmdPipeId);

        if (iStatus != CFE_SUCCESS)
        {
            (void) CFE_EVS_SendEvent(TO_INIT_ERR_EID, CFE_EVS_ERROR,
                                     "CMD Pipe failed to subscribe to TO_CMD_MID. (0x%08X)",
                                     (unsigned int)iStatus);
            goto TO_InitPipe_Exit_Tag;
        }
    }
    else
    {
        (void) CFE_EVS_SendEvent(TO_INIT_ERR_EID, CFE_EVS_ERROR,
                                 "Failed to create CMD pipe (0x%08X)",
                                 (unsigned int)iStatus);
        goto TO_InitPipe_Exit_Tag;
    }

TO_InitPipe_Exit_Tag:
    return (iStatus);
}
    

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Initialize Global Variables                                     */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void TO_InitData()
{
    /* Init input data */
    memset((void*)&TO_AppData.InData, 0x00, sizeof(TO_AppData.InData));

    /* Init data types message */
    //CFE_SB_InitMsg(&TO_AppData.OutData,
    //		TO_DATA_TYPE_MID, sizeof(TO_AppData.OutData), TRUE);

    /* Init housekeeping packet */
    CFE_SB_InitMsg(&TO_AppData.HkTlm,
                   TO_HK_TLM_MID, sizeof(TO_AppData.HkTlm), TRUE);

	OS_MutSemCreate(&TO_AppData.MutexID, "TO_APP", 0);

    TO_AppData.HkTlm.MaxMem = TO_NUM_BYTES_IN_MEM_POOL;
}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* TO initialization                                              */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int32 TO_InitApp()
{
    int32  iStatus   = CFE_SUCCESS;
    int8   hasEvents = 0;

    iStatus = TO_InitEvent();
    if (iStatus != CFE_SUCCESS)
    {
        (void) CFE_ES_WriteToSysLog("TO - Failed to init events (0x%08X)\n", (unsigned int)iStatus);
        goto TO_InitApp_Exit_Tag;
    }
    else
    {
        hasEvents = 1;
    }

    iStatus = TO_InitPipe();
    if (iStatus != CFE_SUCCESS)
    {
        (void) CFE_EVS_SendEvent(TO_INIT_ERR_EID, CFE_EVS_ERROR,
                                 "Failed to init pipes (0x%08X)",
                                 (unsigned int)iStatus);
        goto TO_InitApp_Exit_Tag;
    }

    TO_InitData();

    /* Initialize the memory pool for the priority queues and output channel
     * queues.
     */
    iStatus = CFE_ES_PoolCreateEx(&TO_AppData.HkTlm.MemPoolHandle,
    		      TO_AppData.MemPoolBuffer,
				  TO_NUM_BYTES_IN_MEM_POOL,
				  TO_MAX_MEMPOOL_BLK_SIZES,
                  &TO_MemPoolDefSize[0],
                  CFE_ES_USE_MUTEX);
    if (iStatus != CFE_SUCCESS)
    {
    	(void) CFE_EVS_SendEvent(TO_CR_POOL_ERR_EID, CFE_EVS_ERROR,
        		"Error creating memory pool (0x%08X)",(unsigned int)iStatus);
        goto TO_InitApp_Exit_Tag;
    }

    iStatus = TO_Channel_InitAll();
    if (iStatus != CFE_SUCCESS)
    {
    	(void) CFE_EVS_SendEvent(TO_INIT_ERR_EID, CFE_EVS_ERROR,
        		"Error initializing channels (0x%08X)",(unsigned int)iStatus);
        goto TO_InitApp_Exit_Tag;
    }

    iStatus = TO_Custom_Init();
    if (iStatus != CFE_SUCCESS)
    {
    	(void) CFE_EVS_SendEvent(TO_INIT_ERR_EID, CFE_EVS_ERROR,
            "Failed to init custom layer (0x%08X)",
            (unsigned int)iStatus);
        goto TO_InitApp_Exit_Tag;
    }

TO_InitApp_Exit_Tag:
    if (iStatus == CFE_SUCCESS)
    {
        (void) CFE_EVS_SendEvent(TO_INIT_INF_EID, CFE_EVS_INFORMATION,
                                 "Initialized.  Version %d.%d.%d.%d",
                                 TO_MAJOR_VERSION,
                                 TO_MINOR_VERSION,
                                 TO_REVISION,
                                 TO_MISSION_REV);
    }
    else
    {
        if (hasEvents == 1)
        {
            (void) CFE_EVS_SendEvent(TO_INIT_ERR_EID, CFE_EVS_ERROR, "Application failed to initialize");
        }
        else
        {
            (void) CFE_ES_WriteToSysLog("TO - Application failed to initialize\n");
        }
    }

    return (iStatus);
}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* TO Cleanup                                                     */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void TO_Cleanup()
{
    TO_Channel_CleanupAll();
    OS_MutSemDelete(TO_AppData.MutexID);
}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Receive and Process Messages                                    */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int32 TO_RcvMsg(int32 iBlocking)
{
    int32           iStatus=CFE_SUCCESS;
    CFE_SB_Msg_t*   MsgPtr=NULL;
    CFE_SB_MsgId_t  MsgId;

    /* Stop Performance Log entry */
//	TO_ChannelData_t *channel = &TO_AppData.ChannelData[0];
//	iStatus = OS_QueueDelete(
//			channel->OutputQueue.OSALQueueID);
//	if (iStatus != OS_SUCCESS)
//	{
//		(void) CFE_EVS_SendEvent(TO_CONFIG_TABLE_ERR_EID, CFE_EVS_ERROR,
//								 "Failed to delete '%s' output channel queue. err=%i",
//								 channel->ChannelName, (unsigned int)iStatus);
//	}
    CFE_ES_PerfLogExit(TO_MAIN_TASK_PERF_ID);

	TO_ReleaseAllTables();

    /* Wait for WakeUp messages from scheduler */
    iStatus = CFE_SB_RcvMsg(&MsgPtr, TO_AppData.SchPipeId, iBlocking);

//	channel = &TO_AppData.ChannelData[1];
//	iStatus = OS_QueueDelete(
//			channel->OutputQueue.OSALQueueID);
//	if (iStatus != OS_SUCCESS)
//	{
//		(void) CFE_EVS_SendEvent(TO_CONFIG_TABLE_ERR_EID, CFE_EVS_ERROR,
//								 "Failed to delete '%s' output channel queue. err=%i",
//								 channel->ChannelName, (unsigned int)iStatus);
//	}

    /* Start Performance Log entry */
    CFE_ES_PerfLogEntry(TO_MAIN_TASK_PERF_ID);

	TO_AcquireAllTables();

    if (iStatus == CFE_SUCCESS)
    {
        MsgId = CFE_SB_GetMsgId(MsgPtr);
        switch (MsgId)
        {
            case TO_SEND_TLM_MID:
                TO_ProcessNewCmds();
                TO_Channel_ProcessTelemetryAll();
                break;

            case TO_SEND_HK_MID:
                TO_ReportHousekeeping();
                break;

            default:
                (void) CFE_EVS_SendEvent(TO_MSGID_ERR_EID, CFE_EVS_ERROR,
                                  "Recvd invalid SCH msgId (0x%04X)", (unsigned short)MsgId);
        }
    }
    else if (iStatus == CFE_SB_NO_MESSAGE)
    {
        /* TODO: If there's no incoming message, you can do something here, or
         * nothing.  Note, this section is dead code only if the iBlocking arg
         * is CFE_SB_PEND_FOREVER. */
        iStatus = CFE_SUCCESS;
    }
    else if (iStatus == CFE_SB_TIME_OUT)
    {
        /* TODO: If there's no incoming message within a specified time (via the
         * iBlocking arg, you can do something here, or nothing.
         * Note, this section is dead code only if the iBlocking arg
         * is CFE_SB_PEND_FOREVER. */
        iStatus = CFE_SUCCESS;
    }
    else
    {
        /* TODO: This is an example of exiting on an error (either CFE_SB_BAD_ARGUMENT, or
         * CFE_SB_PIPE_RD_ERROR).
         */
        (void) CFE_EVS_SendEvent(TO_PIPE_ERR_EID, CFE_EVS_ERROR,
			  "SB pipe read error (0x%08X), app will exit", (unsigned int)iStatus);
        TO_AppData.uiRunStatus= CFE_ES_APP_ERROR;
    }

    return (iStatus);
}
    


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Process Incoming Commands                                       */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void TO_ProcessNewCmds()
{
    int iStatus = CFE_SUCCESS;
    CFE_SB_Msg_t*   CmdMsgPtr=NULL;
    CFE_SB_MsgId_t  CmdMsgId;

    /* Process command messages till the pipe is empty */
    while (1)
    {
        iStatus = CFE_SB_RcvMsg(&CmdMsgPtr, TO_AppData.CmdPipeId, CFE_SB_POLL);
        if(iStatus == CFE_SUCCESS)
        {
            CmdMsgId = CFE_SB_GetMsgId(CmdMsgPtr);
            switch (CmdMsgId)
            {
                case TO_CMD_MID:
                    TO_ProcessNewAppCmds(CmdMsgPtr);
                    break;

                /* TODO:  Add code to process other subscribed commands here
                **
                ** Example:
                **     case CFE_TIME_DATA_CMD_MID:
                **         TO_ProcessTimeDataCmd(CmdMsgPtr);
                **         break;
                */

                default:
                    /* Bump the command error counter for an unknown command.
                     * (This should only occur if it was subscribed to with this
                     *  pipe, but not handled in this switch-case.) */
                    TO_AppData.HkTlm.usCmdErrCnt++;
                    (void) CFE_EVS_SendEvent(TO_MSGID_ERR_EID, CFE_EVS_ERROR,
                                      "Recvd invalid CMD msgId (0x%04X)", (unsigned short)CmdMsgId);
                    break;
            }
        }
        else if (iStatus == CFE_SB_NO_MESSAGE)
        {
            break;
        }
        else
        {
            (void) CFE_EVS_SendEvent(TO_PIPE_ERR_EID, CFE_EVS_ERROR,
                  "CMD pipe read error (0x%08X)", (unsigned int)iStatus);
            TO_AppData.uiRunStatus = CFE_ES_APP_ERROR;
            break;
        }
    }
}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Process TO Commands                                            */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void TO_ProcessNewAppCmds(CFE_SB_Msg_t* MsgPtr)
{
    uint32  uiCmdCode=0;

    if (MsgPtr != NULL)
    {
        uiCmdCode = CFE_SB_GetCmdCode(MsgPtr);
        switch (uiCmdCode)
        {
            case TO_NOOP_CC:
            	if(TO_VerifyCmdLength(MsgPtr, sizeof(TO_NoArgCmd_t)))
            	{
                	OS_MutSemTake(TO_AppData.MutexID);
					TO_AppData.HkTlm.usCmdCnt++;
					(void) CFE_EVS_SendEvent(TO_CMD_NOOP_EID, CFE_EVS_INFORMATION,
									  "Executed NOOP cmd (%u), Version %d.%d.%d.%d",
									  (unsigned int)uiCmdCode,
									  TO_MAJOR_VERSION,
									  TO_MINOR_VERSION,
									  TO_REVISION,
									  TO_MISSION_REV);
                	OS_MutSemGive(TO_AppData.MutexID);
            	}
                break;

            case TO_RESET_CC:
            	if(TO_VerifyCmdLength(MsgPtr, sizeof(TO_NoArgCmd_t)))
            	{
                	OS_MutSemTake(TO_AppData.MutexID);
					TO_AppData.HkTlm.usCmdCnt = 0;
					TO_AppData.HkTlm.usCmdErrCnt = 0;
					TO_AppData.HkTlm.usTotalMsgDropped = 0;
					TO_AppData.HkTlm.PeakMemInUse = 0;
                	OS_MutSemGive(TO_AppData.MutexID);

					TO_Channel_ResetCountsAll();

					(void) CFE_EVS_SendEvent(TO_CMD_RESET_EID, CFE_EVS_INFORMATION,
									  "Executed RESET cmd (%u)", (unsigned int)uiCmdCode);
            	}
                break;

            case TO_ADD_MESSAGE_FLOW_CC:
            	if(TO_VerifyCmdLength(MsgPtr, sizeof(TO_AddMessageFlowCmd_t)))
            	{
            		TO_AddMessageFlowCmd_t *cmd = (TO_AddMessageFlowCmd_t*)MsgPtr;

            		if(TO_MessageFlow_Add(cmd->ChannelIdx, cmd->MsgID, cmd->MsgLimit, cmd->PQueueIdx) == FALSE)
            		{
                    	OS_MutSemTake(TO_AppData.MutexID);
            			TO_AppData.HkTlm.usCmdErrCnt++;
                    	OS_MutSemGive(TO_AppData.MutexID);
            		}
            		else
            		{
                    	OS_MutSemTake(TO_AppData.MutexID);
            			TO_AppData.HkTlm.usCmdCnt++;
                    	OS_MutSemGive(TO_AppData.MutexID);
    					(void) CFE_EVS_SendEvent(TO_CMD_ADD_MSG_FLOW_EID, CFE_EVS_INFORMATION,
    									  "Executed ADD_MESSAGE cmd (%u)", (unsigned int)uiCmdCode);
            		}
            	}
            	break;

            case TO_REMOVE_MESSAGE_FLOW_CC:
            	if(TO_VerifyCmdLength(MsgPtr, sizeof(TO_RemoveMessageFlowCmd_t)))
            	{
            		TO_RemoveMessageFlowCmd_t *cmd = (TO_RemoveMessageFlowCmd_t*)MsgPtr;

            		if(TO_MessageFlow_Remove(cmd->ChannelIdx, cmd->MsgID) == FALSE)
            		{
                    	OS_MutSemTake(TO_AppData.MutexID);
            			TO_AppData.HkTlm.usCmdErrCnt++;
                    	OS_MutSemGive(TO_AppData.MutexID);
            		}
            		else
            		{
                    	OS_MutSemTake(TO_AppData.MutexID);
            			TO_AppData.HkTlm.usCmdCnt++;
                    	OS_MutSemGive(TO_AppData.MutexID);
    					(void) CFE_EVS_SendEvent(TO_CMD_REMOVE_MSG_FLOW_EID, CFE_EVS_INFORMATION,
    									  "Executed ADD_REMOVE cmd (%u)", (unsigned int)uiCmdCode);
            		}
            	}
            	break;

            case TO_QUERY_MESSAGE_FLOW_CC:
            	if(TO_VerifyCmdLength(MsgPtr, sizeof(TO_QueryMessageFlowCmd_t)))
            	{
            		TO_QueryMessageFlowCmd_t *cmd = (TO_QueryMessageFlowCmd_t*)MsgPtr;

            		if(TO_MessageFlow_Query(cmd->ChannelIdx, cmd->MsgID) == FALSE)
            		{
                    	OS_MutSemTake(TO_AppData.MutexID);
            			TO_AppData.HkTlm.usCmdErrCnt++;
                    	OS_MutSemGive(TO_AppData.MutexID);
            		}
            		else
            		{
                    	OS_MutSemTake(TO_AppData.MutexID);
            			TO_AppData.HkTlm.usCmdCnt++;
                    	OS_MutSemGive(TO_AppData.MutexID);
            		}
            	}
            	break;

            case TO_QUERY_PRIORITY_QUEUE_CC:
            	if(TO_VerifyCmdLength(MsgPtr, sizeof(TO_QueryPriorityQueueCmd_t)))
            	{
            		TO_QueryPriorityQueueCmd_t *cmd = (TO_QueryPriorityQueueCmd_t*)MsgPtr;

            		if(TO_PriorityQueue_Query(cmd->ChannelIdx, cmd->PQueueIndex) == FALSE)
            		{
                    	OS_MutSemTake(TO_AppData.MutexID);
            			TO_AppData.HkTlm.usCmdErrCnt++;
                    	OS_MutSemGive(TO_AppData.MutexID);
            		}
            		else
            		{
                    	OS_MutSemTake(TO_AppData.MutexID);
            			TO_AppData.HkTlm.usCmdCnt++;
                    	OS_MutSemGive(TO_AppData.MutexID);
            		}
            	}
            	break;

            case TO_QUERY_OUTPUT_CHANNEL_CC:
            	if(TO_VerifyCmdLength(MsgPtr, sizeof(TO_QueryOutputChannelCmd_t)))
            	{
            		TO_QueryOutputChannelCmd_t *cmd = (TO_QueryOutputChannelCmd_t*)MsgPtr;

            		if(TO_OutputChannel_Query(cmd->ChannelIdx) == FALSE)
            		{
                    	OS_MutSemTake(TO_AppData.MutexID);
            			TO_AppData.HkTlm.usCmdErrCnt++;
                    	OS_MutSemGive(TO_AppData.MutexID);
            		}
            		else
            		{
                    	OS_MutSemTake(TO_AppData.MutexID);
            			TO_AppData.HkTlm.usCmdCnt++;
                    	OS_MutSemGive(TO_AppData.MutexID);
            		}
            	}
            	break;

            case TO_SEND_DIAG_CC:
            	if(TO_VerifyCmdLength(MsgPtr, sizeof(TO_SendDiagCmd_t)))
            	{
            		TO_SendDiagCmd_t *cmd = (TO_SendDiagCmd_t*)MsgPtr;

            		if(TO_SendDiag(cmd->ChannelIdx) == FALSE)
            		{
                    	OS_MutSemTake(TO_AppData.MutexID);
            			TO_AppData.HkTlm.usCmdErrCnt++;
                    	OS_MutSemGive(TO_AppData.MutexID);
            		}
            		else
            		{
                    	OS_MutSemTake(TO_AppData.MutexID);
            			TO_AppData.HkTlm.usCmdCnt++;
                    	OS_MutSemGive(TO_AppData.MutexID);
            		}
            	}
            	break;

            default:
            	TO_OutputChannel_ProcessNewCustomCmds(MsgPtr);
                break;
        }
    }
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Send TO Housekeeping                                           */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void TO_ReportHousekeeping()
{
	uint32 i = 0;
	int32 iStatus;

	for(i = 0; i < TO_MAX_CHANNELS; ++i)
	{
		TO_ChannelData_t *channel = &TO_AppData.ChannelData[i];
		TO_Channel_LockByRef(channel);
		TO_AppData.HkTlm.QueuedInOutputChannel[i] = channel->OutputQueue.CurrentlyQueuedCnt;
		TO_Channel_UnlockByRef(channel);
	}

    CFE_SB_TimeStampMsg((CFE_SB_Msg_t*)&TO_AppData.HkTlm);
	OS_MutSemTake(TO_AppData.MutexID);
    CFE_SB_SendMsg((CFE_SB_Msg_t*)&TO_AppData.HkTlm);
	OS_MutSemGive(TO_AppData.MutexID);
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Send TO Send Diagnostic Message                                 */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
boolean TO_SendDiag(uint16 ChannelIdx)
{
	uint32 iMessageFlow = 0;
	uint32 iPQueue = 0;
	boolean rc = FALSE;
	TO_ChannelDiagTlm_t diagMsg;

    TO_ChannelData_t *channel;

    /* First, check if the channel index is valid. */
    if(ChannelIdx >= TO_MAX_CHANNELS)
    {
    	(void) CFE_EVS_SendEvent(TO_CMD_SEND_DIAG_EID, CFE_EVS_ERROR,
    		"Invalid channel index.");
    	goto end_of_function;
    }
    channel = &TO_AppData.ChannelData[ChannelIdx];

    CFE_SB_InitMsg(&diagMsg, TO_DIAG_TLM_MID, sizeof(diagMsg), TRUE);

    diagMsg.Index = ChannelIdx;

    TO_Channel_LockByRef(channel);

	diagMsg.State = channel->State;
	strncpy(diagMsg.ConfigTableName, channel->ConfigTableName,
			sizeof(diagMsg.ConfigTableName));
	strncpy(diagMsg.ConfigTableFileName, channel->ConfigTableFileName,
			sizeof(diagMsg.ConfigTableFileName));
	strncpy(diagMsg.DumpTableName, channel->DumpTableName,
			sizeof(diagMsg.DumpTableName));
	strncpy(diagMsg.ChannelName, channel->ChannelName,
			sizeof(diagMsg.ChannelName));
	diagMsg.TableID = channel->ConfigTblPtr->TableID;

	for(iMessageFlow = 0; iMessageFlow < TO_MAX_MESSAGE_FLOWS; ++iMessageFlow)
	{
		diagMsg.MessageFlow[iMessageFlow].MsgId
			= channel->ConfigTblPtr->MessageFlow[iMessageFlow].MsgId;
		diagMsg.MessageFlow[iMessageFlow].MsgLimit
			= channel->ConfigTblPtr->MessageFlow[iMessageFlow].MsgLimit;
		diagMsg.MessageFlow[iMessageFlow].PQueueID
			= channel->ConfigTblPtr->MessageFlow[iMessageFlow].PQueueID;
		diagMsg.MessageFlow[iMessageFlow].DroppedMsgCnt
			= channel->DumpTbl.MessageFlow[iMessageFlow].DroppedMsgCnt;
		diagMsg.MessageFlow[iMessageFlow].QueuedMsgCnt
			= channel->DumpTbl.MessageFlow[iMessageFlow].QueuedMsgCnt;
	}

	for(iPQueue = 0; iPQueue < TO_MAX_PRIORITY_QUEUES; ++iPQueue)
	{
		diagMsg.PQueue[iPQueue].State
			= channel->ConfigTblPtr->PriorityQueue[iPQueue].State;
		diagMsg.PQueue[iPQueue].MsgLimit
			= channel->ConfigTblPtr->PriorityQueue[iPQueue].MsgLimit;
		diagMsg.PQueue[iPQueue].QType
			= channel->ConfigTblPtr->PriorityQueue[iPQueue].QType;
		diagMsg.PQueue[iPQueue].DroppedMsgCnt
			= channel->DumpTbl.PriorityQueue[iPQueue].DroppedMsgCnt;
		diagMsg.PQueue[iPQueue].QueuedMsgCnt
			= channel->DumpTbl.PriorityQueue[iPQueue].QueuedMsgCnt;
		diagMsg.PQueue[iPQueue].CurrentlyQueuedCnt
			= channel->DumpTbl.PriorityQueue[iPQueue].CurrentlyQueuedCnt;
		diagMsg.PQueue[iPQueue].HighwaterMark
			= channel->DumpTbl.PriorityQueue[iPQueue].HighwaterMark;
	}

	diagMsg.OQueue.SentCount
		= channel->OutputQueue.SentCount;
	diagMsg.OQueue.CurrentlyQueuedCnt
		= channel->OutputQueue.CurrentlyQueuedCnt;
	diagMsg.OQueue.HighwaterMark
		= channel->OutputQueue.HighwaterMark;

	TO_Channel_UnlockByRef(channel);

    CFE_SB_TimeStampMsg((CFE_SB_Msg_t*)&diagMsg);
    CFE_SB_SendMsg((CFE_SB_Msg_t*)&diagMsg);

    rc = TRUE;

end_of_function:
	return rc;

}


    
/*=====================================================================================
** Name: TO_VerifyCmdLength
**
** Purpose: To verify command length for a particular command message
**
** Arguments:
**    CFE_SB_Msg_t*  MsgPtr      - command message pointer
**    uint16         usExpLength - expected command length
**
** Returns:
**    boolean bResult - result of verification
**
** Routines Called:
**    TBD
**
** Called By:
**    TO_ProcessNewCmds
**
** Global Inputs/Reads:
**    None
**
** Global Outputs/Writes:
**    TBD
**
** Limitations, Assumptions, External Events, and Notes:
**    1. List assumptions that are made that apply to this function.
**    2. List the external source(s) and event(s) that can cause this function to execute.
**    3. List known limitations that apply to this function.
**    4. If there are no assumptions, external events, or notes then enter NONE.
**       Do not omit the section.
**
** Algorithm:
**    Psuedo-code or description of basic algorithm
**=====================================================================================*/
boolean TO_VerifyCmdLength(CFE_SB_Msg_t* MsgPtr,
                           uint16 usExpectedLen)
{
    boolean bResult  = TRUE;
    uint16  usMsgLen = 0;

    if (MsgPtr != NULL)
    {
        usMsgLen = CFE_SB_GetTotalMsgLength(MsgPtr);

        if (usExpectedLen != usMsgLen)
        {
            bResult = FALSE;
            CFE_SB_MsgId_t MsgId = CFE_SB_GetMsgId(MsgPtr);
            uint16 usCmdCode = CFE_SB_GetCmdCode(MsgPtr);

            (void) CFE_EVS_SendEvent(TO_MSGLEN_ERR_EID, CFE_EVS_ERROR,
                              "Rcvd invalid msgLen: msgId=0x%04X, cmdCode=%d, "
                              "msgLen=%d, expectedLen=%d",
                              MsgId, usCmdCode, usMsgLen, usExpectedLen);
            TO_AppData.HkTlm.usCmdErrCnt++;
        }
    }

    return (bResult);
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* TO application entry point and main process loop               */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void TO_AppMain()
{
    /* Clear the AppData structure.  Global data appears to retain its value when an application is
     * restarted.
     */
    memset(&TO_AppData, 0, sizeof(TO_AppData));

    /* Register the application with Executive Services */
    TO_AppData.uiRunStatus = CFE_ES_APP_RUN;

    int32 iStatus = CFE_ES_RegisterApp();
    if (iStatus != CFE_SUCCESS)
    {
        (void) CFE_ES_WriteToSysLog("TO - Failed to register the app (0x%08X)\n", (unsigned int)iStatus);
    }

    /* Start Performance Log entry */
    CFE_ES_PerfLogEntry(TO_MAIN_TASK_PERF_ID);

    /* Perform application initializations */
    if (iStatus == CFE_SUCCESS)
    {
        iStatus = TO_InitApp();
    }

    if (iStatus == CFE_SUCCESS)
    {
        /* Do not perform performance monitoring on startup sync */
        CFE_ES_PerfLogExit(TO_MAIN_TASK_PERF_ID);
        CFE_ES_WaitForStartupSync(TO_STARTUP_TIMEOUT_MSEC);
        CFE_ES_PerfLogEntry(TO_MAIN_TASK_PERF_ID);
    }
    else
    {
        TO_AppData.uiRunStatus = CFE_ES_APP_ERROR;
    }

    /* Application main loop */
    while (CFE_ES_RunLoop(&TO_AppData.uiRunStatus) == TRUE)
    {
        int32 iStatus = TO_RcvMsg(TO_SCH_PIPE_PEND_TIME);
        if (iStatus != CFE_SUCCESS)
        {
            /* TODO: Decide what to do for other return values in TO_RcvMsg(). */
        }
    }

    TO_Cleanup();

    /* Stop Performance Log entry */
    CFE_ES_PerfLogExit(TO_MAIN_TASK_PERF_ID);

    /* Exit the application */
    CFE_ES_ExitApp(TO_AppData.uiRunStatus);
}

uint32 TO_ProtobufTlmEncode(CFE_SB_MsgPtr_t MsgInPtr,
		char *BufferOut, uint32 Size)
{
	CFE_SB_MsgId_t  	msgId = 0;
	uint32  			msgSize = 0;
	uint16  			outSize = 0;
	uint32  			hdrSize = 0;
	uint32  			payloadSize = 0;
	PBLib_EncodeFuncPtr_t encodeFunc;

	/* Get required params */
	msgId = CFE_SB_GetMsgId(MsgInPtr);
	payloadSize = CFE_SB_GetUserDataLength(MsgInPtr); //change to user data length
	hdrSize = CFE_SB_MsgHdrSize(msgId);

	/* Get serialization funciton from PBL */
	encodeFunc = PBLIB_GetTlmSerializationFunc(msgId);
	if(encodeFunc == 0)
	{
		CFE_EVS_SendEvent (TO_NO_ENCODE_FUNC_EID, CFE_EVS_ERROR, "MsgId (0x%04X) has no serialization function",
									msgId);
		outSize = 0;
		goto TO_ProtobufEncode_Exit_Tag;
	}

	/* Call encode function */
	payloadSize = encodeFunc(MsgInPtr, &BufferOut[hdrSize], Size - hdrSize);
	outSize = hdrSize + payloadSize;

	/* Create new SB msg from serialized data */
	CFE_SB_InitMsg(BufferOut, msgId, outSize, FALSE);

	/* Update header info */
	CFE_SB_GenerateChecksum((CFE_SB_MsgPtr_t)BufferOut);

TO_ProtobufEncode_Exit_Tag:
	return outSize;
}


TO_ChannelType_t TO_GetChannelType(uint32 ChannelID)
{
	TO_AppData.ChannelData[ChannelID].ConfigTblPtr->ChannelType;
}


/************************/
/*  End of File Comment */
/************************/
