
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
    TO_AppData.EventTbl[  ind].EventID = TO_RESERVED_EID;
    TO_AppData.EventTbl[ind++].Mask    = CFE_EVS_NO_FILTER;

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

    /* Init data pipe and subscribe to messages on the data pipe */
    iStatus = CFE_SB_CreatePipe(&TO_AppData.DataPipeId,
                                 TO_DATA_PIPE_DEPTH,
                                 TO_DATA_PIPE_NAME);
    if (iStatus != CFE_SUCCESS)
    {
        (void) CFE_EVS_SendEvent(TO_INIT_ERR_EID, CFE_EVS_ERROR,
                                 "Failed to create Data pipe (0x%08X)",
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

    iStatus = TO_OutputChannel_InitAll();
    if (iStatus != CFE_SUCCESS)
    {
    	(void) CFE_EVS_SendEvent(TO_INIT_ERR_EID, CFE_EVS_ERROR,
            "Failed to init Output Channels (0x%08X)",
            (unsigned int)iStatus);
        goto TO_InitApp_Exit_Tag;
    }

    iStatus = TO_InitConfigTbl();
    if (iStatus != CFE_SUCCESS)
    {
        (void) CFE_EVS_SendEvent(TO_INIT_ERR_EID, CFE_EVS_ERROR,
                                 "Failed to init config tables (0x%08X)",
                                 (unsigned int)iStatus);
        goto TO_InitApp_Exit_Tag;
    }

    TO_InitData();

    /* Initialize the memory pool for the priority queues and output channel
     * queues.
     */
    iStatus = CFE_ES_PoolCreate (&TO_AppData.HkTlm.MemPoolHandle,
                                  TO_AppData.MemPoolBuffer,
                                  sizeof (TO_AppData.MemPoolBuffer) );
    if (iStatus != CFE_SUCCESS)
    {
    	(void) CFE_EVS_SendEvent(TO_CR_POOL_ERR_EID, CFE_EVS_ERROR,
        		"Error creating memory pool (0x%08X)",(unsigned int)iStatus);
        goto TO_InitApp_Exit_Tag;
    }

    /* Register the cleanup callback */
    iStatus = OS_TaskInstallDeleteHandler(&TO_CleanupCallback);
    if (iStatus != CFE_SUCCESS)
    {
        (void) CFE_EVS_SendEvent(TO_INIT_ERR_EID, CFE_EVS_ERROR,
                                 "Failed to init register cleanup callback (0x%08X)",
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

void TO_CleanupCallback()
{
	TO_MessageFlow_TeardownAll();
	TO_PriorityQueue_TeardownAll();
	TO_OutputChannel_TeardownAll();
	TO_OutputChannel_CustomCleanupAll();
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
    CFE_ES_PerfLogExit(TO_MAIN_TASK_PERF_ID);

    /* Wait for WakeUp messages from scheduler */
    iStatus = CFE_SB_RcvMsg(&MsgPtr, TO_AppData.SchPipeId, iBlocking);

    /* Start Performance Log entry */
    CFE_ES_PerfLogEntry(TO_MAIN_TASK_PERF_ID);

    if (iStatus == CFE_SUCCESS)
    {
        MsgId = CFE_SB_GetMsgId(MsgPtr);
        switch (MsgId)
	{
            case TO_SEND_TLM_MID:
                TO_ProcessNewCmds();
                TO_ProcessTelemetry();
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
					TO_AppData.HkTlm.usCmdCnt++;
					(void) CFE_EVS_SendEvent(TO_CMD_NOOP_EID, CFE_EVS_INFORMATION,
									  "Executed NOOP cmd (%u), Version %d.%d.%d.%d",
									  (unsigned int)uiCmdCode,
									  TO_MAJOR_VERSION,
									  TO_MINOR_VERSION,
									  TO_REVISION,
									  TO_MISSION_REV);
            	}
                break;

            case TO_RESET_CC:
            	if(TO_VerifyCmdLength(MsgPtr, sizeof(TO_NoArgCmd_t)))
            	{
					TO_AppData.HkTlm.usCmdCnt = 0;
					TO_AppData.HkTlm.usCmdErrCnt = 0;
					TO_AppData.HkTlm.usTotalMsgDropped = 0;
					TO_AppData.HkTlm.usNoSerFuncCnt = 0;
					TO_MessageFlow_ResetCountsAll();
					TO_PriorityQueue_ResetCountsAll();
					TO_OutputChannel_ResetCountsAll();

					(void) CFE_EVS_SendEvent(TO_CMD_RESET_EID, CFE_EVS_INFORMATION,
									  "Executed RESET cmd (%u)", (unsigned int)uiCmdCode);
            	}
                break;

            case TO_ADD_MESSAGE_FLOW_CC:
            	if(TO_VerifyCmdLength(MsgPtr, sizeof(TO_AddMessageFlowCmd_t)))
            	{
            		TO_AddMessageFlowCmd_t *cmd = (TO_AddMessageFlowCmd_t*)MsgPtr;

            		if(TO_MessageFlow_Add(cmd->MsgID, cmd->MsgLimit, cmd->PQueueIdx) == FALSE)
            		{
            			TO_AppData.HkTlm.usCmdErrCnt++;
            		}
            		else
            		{
            			TO_AppData.HkTlm.usCmdCnt++;
    					(void) CFE_EVS_SendEvent(TO_CMD_ADD_MSG_FLOW_EID, CFE_EVS_INFORMATION,
    									  "Executed ADD_MESSAGE cmd (%u)", (unsigned int)uiCmdCode);
            		}
            	}
            	break;

            case TO_REMOVE_MESSAGE_FLOW_CC:
            	if(TO_VerifyCmdLength(MsgPtr, sizeof(TO_RemoveMessageFlowCmd_t)))
            	{
            		TO_RemoveMessageFlowCmd_t *cmd = (TO_RemoveMessageFlowCmd_t*)MsgPtr;

            		if(TO_MessageFlow_Remove(cmd->MsgID, cmd->PQueueIdx) == FALSE)
            		{
            			TO_AppData.HkTlm.usCmdErrCnt++;
            		}
            		else
            		{
            			TO_AppData.HkTlm.usCmdCnt++;
    					(void) CFE_EVS_SendEvent(TO_CMD_REMOVE_MSG_FLOW_EID, CFE_EVS_INFORMATION,
    									  "Executed ADD_REMOVE cmd (%u)", (unsigned int)uiCmdCode);
            		}
            	}
            	break;

            case TO_QUERY_MESSAGE_FLOW_CC:
            	if(TO_VerifyCmdLength(MsgPtr, sizeof(TO_QueryMessageFlowCmd_t)))
            	{
            		TO_QueryMessageFlowCmd_t *cmd = (TO_QueryMessageFlowCmd_t*)MsgPtr;

            		if(TO_MessageFlow_Query(cmd->MsgID, cmd->PQueueIdx) == FALSE)
            		{
            			TO_AppData.HkTlm.usCmdErrCnt++;
            		}
            		else
            		{
            			TO_AppData.HkTlm.usCmdCnt++;
            		}
            	}
            	break;

            case TO_QUERY_PRIORITY_QUEUE_CC:
            	if(TO_VerifyCmdLength(MsgPtr, sizeof(TO_QueryPriorityQueueCmd_t)))
            	{
            		TO_QueryPriorityQueueCmd_t *cmd = (TO_QueryPriorityQueueCmd_t*)MsgPtr;

            		if(TO_PriorityQueue_Query(cmd->PQueueIndex) == FALSE)
            		{
            			TO_AppData.HkTlm.usCmdErrCnt++;
            		}
            		else
            		{
            			TO_AppData.HkTlm.usCmdCnt++;
            		}
            	}
            	break;

            case TO_QUERY_OUTPUT_CHANNEL_CC:
            	if(TO_VerifyCmdLength(MsgPtr, sizeof(TO_QueryOutputChannelCmd_t)))
            	{
            		TO_QueryOutputChannelCmd_t *cmd = (TO_QueryOutputChannelCmd_t*)MsgPtr;

            		if(TO_OutputChannel_Query(cmd->OutputChannelIndex) == FALSE)
            		{
            			TO_AppData.HkTlm.usCmdErrCnt++;
            		}
            		else
            		{
            			TO_AppData.HkTlm.usCmdCnt++;
            		}
            	}
            	break;

            /* TODO:  Add code to process the rest of the TO commands here */

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
    /* TODO:  Add code to update housekeeping data, if needed, here.  */

    CFE_SB_TimeStampMsg((CFE_SB_Msg_t*)&TO_AppData.HkTlm);
    int32 iStatus = CFE_SB_SendMsg((CFE_SB_Msg_t*)&TO_AppData.HkTlm);
    if (iStatus != CFE_SUCCESS)
    {
        /* TODO: Decide what to do if the send message fails. */
    }
}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Process Telemetry                                               */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void TO_ProcessTelemetry()
{
	TO_Channel_ProcessAll();
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
                              "Rcvd invalid msgLen: msgId=0x%08X, cmdCode=%d, "
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

        iStatus = TO_AcquireConfigPointers();
        if(iStatus != CFE_SUCCESS)
        {
            /* We apparently tried to load a new table but failed.  Terminate the application. */
            TO_AppData.uiRunStatus = CFE_ES_APP_ERROR;
        }
    }

    /* Stop Performance Log entry */
    CFE_ES_PerfLogExit(TO_MAIN_TASK_PERF_ID);

    /* Exit the application */
    CFE_ES_ExitApp(TO_AppData.uiRunStatus);
}


/************************/
/*  End of File Comment */
/************************/
