#include "to_app.h"
#include "to_message_flow.h"



int32 TO_MessageFlow_BuildupAll(void)
{
	uint32 i = 0;
	int32 iStatus = CFE_SUCCESS;

    /*
     * Create message flows by subscribing to messages.
     */
	for(i = 0; i < TO_MAX_MESSAGE_FLOWS; ++i)
	{
		if(TO_AppData.Config.MessageFlow[i].MsgId != 0)
		{
			/* Subscribe to message. */
			iStatus = CFE_SB_SubscribeEx(TO_AppData.Config.MessageFlow[i].MsgId, TO_AppData.DataPipeId,
										 CFE_SB_Default_Qos, TO_AppData.Config.MessageFlow[i].MsgLimit);
			if (iStatus != CFE_SUCCESS)
			{
				(void) CFE_EVS_SendEvent(TO_CONFIG_TABLE_ERR_EID, CFE_EVS_ERROR,
						"Message flow failed to subscribe to (0x%08X). (%i)",
						TO_AppData.Config.MessageFlow[i].MsgId,
						(unsigned int)iStatus);
				goto end_of_function;
			}
		}
	}

end_of_function:
	return iStatus;
}



int32 TO_MessageFlow_TeardownAll(void)
{
	uint32 i = 0;
	int32 iStatus = CFE_SUCCESS;

	for(i = 0; i < TO_MAX_MESSAGE_FLOWS; ++i)
	{
		if(TO_AppData.Config.MessageFlow[i].MsgId != 0)
		{
			/* Unsubscribe from message. */
			iStatus =  CFE_SB_Unsubscribe(
					TO_AppData.Config.MessageFlow[i].MsgId,
					TO_AppData.DataPipeId);
			if (iStatus != CFE_SUCCESS)
			{
				(void) CFE_EVS_SendEvent(TO_CONFIG_TABLE_ERR_EID, CFE_EVS_ERROR,
						"Message flow failed to unsubscribe from 0x%08X. (%i)",
						TO_AppData.Config.MessageFlow[i].MsgId,
						(unsigned int)iStatus);
				goto end_of_function;
			}
		}
	}

end_of_function:
	return iStatus;
}



void TO_MessageFlow_CleanupAll(void)
{

}



void TO_MessageFlow_ResetCountsAll(void)
{
	uint32 i = 0;

	for(i = 0; i < TO_MAX_MESSAGE_FLOWS; ++i)
	{
		TO_AppData.Config.MessageFlow[i].DroppedMsgCnt = 0;
		TO_AppData.Config.MessageFlow[i].QueuedMsgCnt = 0;
	}
}



TO_TlmMessageFlow_t* TO_MessageFlow_GetObject(CFE_SB_MsgId_t MsgID)
{
	uint32 i = 0;
	TO_TlmMessageFlow_t *outMsgFlow = 0;

	if(MsgID == 0)
	{
		outMsgFlow = 0;
	}
	else
	{
		for(i = 0; i < TO_MAX_MESSAGE_FLOWS; ++i)
		{
			if(TO_AppData.Config.MessageFlow[i].MsgId == MsgID)
			{
				outMsgFlow = &TO_AppData.Config.MessageFlow[i];
				break;
			}
		}
	}

	return outMsgFlow;
}



TO_TlmPriorityQueue_t* TO_MessageFlow_GetPQueue(TO_TlmMessageFlow_t *MsgFlow)
{
	uint32 i = 0;
	uint32 idx = 0;
	TO_TlmPriorityQueue_t* outPQueue = 0;

	if(MsgFlow == 0)
	{
		outPQueue = 0;
		goto end_of_function;
	}

	idx = MsgFlow->PQueueID;
	if(idx >= TO_MAX_PRIORITY_QUEUES)
	{
		outPQueue = 0;
		goto end_of_function;
	}

	outPQueue = &TO_AppData.Config.PriorityQueue[idx];

end_of_function:
    return outPQueue;
}



boolean TO_MessageFlow_Query(CFE_SB_MsgId_t MsgID)
{
	uint32 i = 0;
	TO_TlmMessageFlow_t *outMsgFlow = 0;
    boolean found = FALSE;

	for(i = 0; i < TO_MAX_MESSAGE_FLOWS; ++i)
	{
		if(TO_AppData.Config.MessageFlow[i].MsgId == MsgID)
		{
			(void) CFE_EVS_SendEvent(TO_MSG_FLOW_INFO_EID, CFE_EVS_INFORMATION,
			                      "MID=0x%04x ML=%u MS=%u PQI=%u D=%u Q=%u",
								  TO_AppData.Config.MessageFlow[i].MsgId,
								  TO_AppData.Config.MessageFlow[i].MsgLimit,
								  TO_AppData.Config.MessageFlow[i].MinSize,
								  TO_AppData.Config.MessageFlow[i].PQueueID,
								  TO_AppData.Config.MessageFlow[i].DroppedMsgCnt,
								  TO_AppData.Config.MessageFlow[i].QueuedMsgCnt);
			found = TRUE;
		}
	}

	if(found != TRUE)
	{
		(void) CFE_EVS_SendEvent(TO_MSG_FLOW_INFO_EID, CFE_EVS_ERROR,
		                      "MsgID 0x%04x not found", MsgID);
	}

	return found;
}


CFE_SB_MsgId_t  MsgId;
uint16          MsgLimit;
uint16			PQueueID;
uint16          MinSize;
uint16			DroppedMsgCnt;
uint16			QueuedMsgCnt;
