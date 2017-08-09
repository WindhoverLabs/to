#include "to_app.h"
#include "to_message_flow.h"



/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Build up all the message flows                                  */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
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



/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Teardown all the message flows                                  */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
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



/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Cleanup the message flows before shutdown.                      */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void TO_MessageFlow_CleanupAll(void)
{
	TO_MessageFlow_TeardownAll();
}



/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Reset all runtime metrics.                                      */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void TO_MessageFlow_ResetCountsAll(void)
{
	uint32 i = 0;

	for(i = 0; i < TO_MAX_MESSAGE_FLOWS; ++i)
	{
		TO_AppData.Config.MessageFlow[i].DroppedMsgCnt = 0;
		TO_AppData.Config.MessageFlow[i].QueuedMsgCnt = 0;
	}
}



/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Get the next message flow object                                */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
TO_TlmMessageFlow_t* TO_MessageFlow_GetNextObject(CFE_SB_MsgId_t MsgID, uint32 *Cursor)
{
	TO_TlmMessageFlow_t *outMsgFlow = 0;

	if(Cursor == 0)
	{
		outMsgFlow = 0;
	}
	else if(MsgID == 0)
	{
		outMsgFlow = 0;
		*Cursor = -1;
	}
	else
	{
		for(; *Cursor < TO_MAX_MESSAGE_FLOWS; ++(*Cursor))
		{
			if(TO_AppData.Config.MessageFlow[*Cursor].MsgId == MsgID)
			{
				outMsgFlow = &TO_AppData.Config.MessageFlow[*Cursor];
				/* Increment Cursor so the next call will skip the object we just
				 * found.
				 */
				++(*Cursor);
				break;
			}
		}

		/* This must be the end of the array.  Return a -1 to let the caller
		 * know there are no more objects to retrieve.
		 */
		if(outMsgFlow == 0)
		{
			*Cursor = -1;
		}
	}

	return outMsgFlow;
}



/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Get a specific message flow object                              */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
TO_TlmMessageFlow_t* TO_MessageFlow_GetObject(CFE_SB_MsgId_t MsgID, uint16 PQueueIdx)
{
	TO_TlmMessageFlow_t *outMsgFlow = 0;
	uint32 i = 0;

	if(MsgID == 0)
	{
		outMsgFlow = 0;
	}
	else
	{
		for(i; i < TO_MAX_MESSAGE_FLOWS; ++i)
		{
			if(TO_AppData.Config.MessageFlow[i].MsgId == MsgID)
			{
				if(TO_AppData.Config.MessageFlow[i].PQueueID == PQueueIdx)
				{
					outMsgFlow = &TO_AppData.Config.MessageFlow[i];
					break;
				}
			}
		}
	}

	return outMsgFlow;
}



/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Get the priority queue for a specific message flow              */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
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



/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Add a message flow                                              */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
boolean TO_MessageFlow_Add(CFE_SB_MsgId_t MsgID, uint16 MsgLimit, uint16 PQueueIdx)
{
	uint32 i = 0;
	TO_TlmMessageFlow_t *msgFlow = 0;
    boolean added = FALSE;

    /* First, see if there is a flow with this message ID and priority queue. */
    msgFlow = TO_MessageFlow_GetObject(MsgID,  PQueueIdx);

    if(msgFlow != 0)
    {
    	/* A message flow with this route already exists.  Reject the request. */
		(void) CFE_EVS_SendEvent(TO_CMD_ADD_MSG_FLOW_EID, CFE_EVS_ERROR,
	                      "Message flow is already defined.");
		goto end_of_function;
    }

    /* Now that we know a message flow definition doesn't already exist, find
     * an unused entry so we can add one.  Unused entries have the MsgID set
     * to 0.
     */
	for(i = 0; i < TO_MAX_MESSAGE_FLOWS; ++i)
	{
		if(TO_AppData.Config.MessageFlow[i].MsgId == 0)
		{
			int32 iStatus = CFE_SUCCESS;

			/* We found an unused entry.  Set the message flow definition
			 * here.  First, make sure the requested priority queue is valid.
			 */
			if(TO_PriorityQueue_IsValid(PQueueIdx) == FALSE)
			{
				/* This is an invalid priority queue. */
				(void) CFE_EVS_SendEvent(TO_CMD_ADD_MSG_FLOW_EID, CFE_EVS_ERROR,
						"Priority Queue Index %u is invalid.",
						PQueueIdx);
				goto end_of_function;
			}

			/* Now subscribe to the message to ensure the message ID is
			 * valid.
			 */
			iStatus = CFE_SB_SubscribeEx(MsgID, TO_AppData.DataPipeId,
										 CFE_SB_Default_Qos, MsgLimit);
			if (iStatus != CFE_SUCCESS)
			{
				(void) CFE_EVS_SendEvent(TO_CMD_ADD_MSG_FLOW_EID, CFE_EVS_ERROR,
						"Message flow failed to subscribe to (0x%08X). (%i)",
						MsgID,
						(unsigned int)iStatus);
				goto end_of_function;
			}

			/* Now that the message was successfully subscribed to, set the
			 * message flow definition.
			 */
			TO_AppData.Config.MessageFlow[i].MsgId = MsgID,
			TO_AppData.Config.MessageFlow[i].MsgLimit = MsgLimit,
			TO_AppData.Config.MessageFlow[i].PQueueID = PQueueIdx,
			TO_AppData.Config.MessageFlow[i].DroppedMsgCnt = 0;
			TO_AppData.Config.MessageFlow[i].QueuedMsgCnt = 0;

			added = TRUE;
		}
	}

end_of_function:
	return added;
}



/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Remove a message flow                                           */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
boolean TO_MessageFlow_Remove(CFE_SB_MsgId_t MsgID, uint16 PQueueIdx)
{
	uint32 i = 0;
	TO_TlmMessageFlow_t *msgFlow = 0;
    boolean removed = FALSE;
    int32 iStatus = CFE_SUCCESS;

    /* First, see if there is a flow with this message ID and priority queue. */
    msgFlow = TO_MessageFlow_GetObject(MsgID, PQueueIdx);

    if(msgFlow == 0)
    {
    	/* A message flow with this route does not exist.  Reject the request. */
		(void) CFE_EVS_SendEvent(TO_CMD_REMOVE_MSG_FLOW_EID, CFE_EVS_ERROR,
	                      "Message flow is not defined.");
		goto end_of_function;
    }

    /* Now that we have the message flow object, unsubscribe from it and clear
     * the entries. */
	iStatus =  CFE_SB_Unsubscribe(
			msgFlow->MsgId,
			TO_AppData.DataPipeId);
	if (iStatus != CFE_SUCCESS)
	{
		(void) CFE_EVS_SendEvent(TO_CMD_REMOVE_MSG_FLOW_EID, CFE_EVS_ERROR,
				"Message flow failed to unsubscribe from 0x%08X. (%i)",
				msgFlow->MsgId,
				(unsigned int)iStatus);
		goto end_of_function;
	}

	/* Now just clear the entries. */
	msgFlow->MsgId = 0;
	msgFlow->MsgLimit = 0;
	msgFlow->PQueueID = 0;
	msgFlow->MinSize = 0;
	msgFlow->DroppedMsgCnt = 0;
	msgFlow->QueuedMsgCnt = 0;

    removed = TRUE;

end_of_function:
	return removed;
}



/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Query a message flow                                            */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
boolean TO_MessageFlow_Query(CFE_SB_MsgId_t MsgID, uint16 PQueueIdx)
{
	uint32 i = 0;
	TO_TlmMessageFlow_t *outMsgFlow = 0;
    boolean found = FALSE;

	for(i = 0; i < TO_MAX_MESSAGE_FLOWS; ++i)
	{
		if(TO_AppData.Config.MessageFlow[i].MsgId == MsgID)
		{
			if(TO_AppData.Config.MessageFlow[i].PQueueID == PQueueIdx)
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
	}

	if(found != TRUE)
	{
		(void) CFE_EVS_SendEvent(TO_MSG_FLOW_INFO_EID, CFE_EVS_ERROR,
		                      "MsgID=0x%04x PQueueIdx=%u not found", MsgID, PQueueIdx);
	}

	return found;
}

