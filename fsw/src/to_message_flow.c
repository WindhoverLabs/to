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
* 3. Neither the name Windhover Labs, L.L.C. nor the names of its 
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

#include "to_app.h"
#include "to_message_flow.h"



/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Build up all the message flows                                  */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
int32 TO_MessageFlow_Buildup(TO_ChannelData_t* channel)
{
	uint32 i = 0;
	int32 iStatus = CFE_SUCCESS;

    /*
     * Create message flows by subscribing to messages.
     */
	for(i = 0; i < TO_MAX_MESSAGE_FLOWS; ++i)
	{
		if(channel->ConfigTblPtr->MessageFlow[i].MsgId != 0)
		{
			/* Subscribe to message. */
			iStatus = CFE_SB_SubscribeEx(channel->ConfigTblPtr->MessageFlow[i].MsgId, channel->DataPipeId,
										 CFE_SB_Default_Qos, channel->ConfigTblPtr->MessageFlow[i].MsgLimit);
			if (iStatus != CFE_SUCCESS)
			{
				(void) CFE_EVS_SendEvent(TO_CONFIG_TABLE_ERR_EID, CFE_EVS_ERROR,
						"Message flow failed to subscribe to (0x%08X). (%i)",
						channel->ConfigTblPtr->MessageFlow[i].MsgId,
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
int32 TO_MessageFlow_TeardownAll(TO_ChannelData_t* channel)
{
	uint32 i = 0;
	int32 iStatus = CFE_SUCCESS;

	for(i = 0; i < TO_MAX_MESSAGE_FLOWS; ++i)
	{
		if(channel->ConfigTblPtr != 0)
		{
			if(channel->ConfigTblPtr->MessageFlow[i].MsgId != 0)
			{
				/* Unsubscribe from message. */
				iStatus =  CFE_SB_Unsubscribe(
						channel->ConfigTblPtr->MessageFlow[i].MsgId,
						channel->DataPipeId);
				if (iStatus != CFE_SUCCESS)
				{
					(void) CFE_EVS_SendEvent(TO_CONFIG_TABLE_ERR_EID, CFE_EVS_ERROR,
							"Message flow failed to unsubscribe from 0x%04x. (%i)",
							channel->ConfigTblPtr->MessageFlow[i].MsgId,
							(unsigned int)iStatus);
					goto end_of_function;
				}
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
void TO_MessageFlow_CleanupAll(TO_ChannelData_t* channel)
{
	TO_MessageFlow_TeardownAll(channel);
}



/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Reset all runtime metrics.                                      */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void TO_MessageFlow_ResetCountsAll(TO_ChannelData_t* channel)
{
	uint32 i = 0;

	for(i = 0; i < TO_MAX_MESSAGE_FLOWS; ++i)
	{
		channel->DumpTbl.MessageFlow[i].DroppedMsgCnt = 0;
		channel->DumpTbl.MessageFlow[i].QueuedMsgCnt = 0;
	}
}



/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Get message flow object                                */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
TO_MessageFlow_t* TO_MessageFlow_GetObject(TO_ChannelData_t* channel, CFE_SB_MsgId_t MsgID, uint32 *Index)
{
	TO_MessageFlow_t *outMsgFlow = 0;
	uint32 i = 0;

	for(i = 0; i < TO_MAX_MESSAGE_FLOWS; ++i)
	{
		if(channel->ConfigTblPtr->MessageFlow[i].MsgId == MsgID)
		{
			outMsgFlow = &channel->ConfigTblPtr->MessageFlow[i];
			/* Increment Cursor so the next call will skip the object we just
			 * found.
			 */
			if(Index != 0)
			{
				*Index = i;
			}
			++i;
			break;
		}
	}


	return outMsgFlow;
}



/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Get the priority queue for a specific message flow              */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
TO_PriorityQueue_t* TO_MessageFlow_GetPQueue(TO_ChannelData_t* channel, TO_MessageFlow_t *MsgFlow, uint32 *Index)
{
	uint32 i = 0;
	uint32 idx = 0;
	TO_PriorityQueue_t* outPQueue = 0;

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

	*Index = idx;
	outPQueue = &channel->ConfigTblPtr->PriorityQueue[idx];

end_of_function:
    return outPQueue;
}



/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Add a message flow                                              */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
boolean TO_MessageFlow_Add(uint16 ChannelIdx, CFE_SB_MsgId_t MsgID, uint16 MsgLimit, uint16 PQueueIdx)
{
	uint32 i = 0;
	TO_MessageFlow_t *msgFlow = 0;
    boolean added = FALSE;
	int32 iStatus = CFE_SUCCESS;

    TO_ChannelData_t *channel;

    /* First, check if the channel index is valid. */
    if(ChannelIdx >= TO_MAX_CHANNELS)
    {
    	(void) CFE_EVS_SendEvent(TO_CMD_ADD_MSG_FLOW_EID, CFE_EVS_ERROR,
    		"Invalid channel index.");
    	goto end_of_function;
    }
    channel = &TO_AppData.ChannelData[ChannelIdx];

	TO_Channel_LockByRef(channel);

    /* Next, see if the channel is open. */
    if(channel->State != TO_CHANNEL_OPENED)
    {
    	(void) CFE_EVS_SendEvent(TO_CMD_ADD_MSG_FLOW_EID, CFE_EVS_ERROR,
    		"Channel not open.");
    	TO_Channel_UnlockByRef(channel);
    	goto end_of_function;
    }

    /* Next, see if there is a flow with this message ID. */
    msgFlow = TO_MessageFlow_GetObject(channel, MsgID, 0);

    if(msgFlow != 0)
    {
    	/* A message flow already exists.  Reject the request. */
		(void) CFE_EVS_SendEvent(TO_CMD_ADD_MSG_FLOW_EID, CFE_EVS_ERROR,
	                      "Message flow is already defined.");
    	TO_Channel_UnlockByRef(channel);
		goto end_of_function;
    }

    /* Now that we know a message flow definition doesn't already exist, find
     * an unused entry so we can add one.  Unused entries have the MsgID set
     * to 0.
     */
	for(i = 0; i < TO_MAX_MESSAGE_FLOWS; ++i)
	{
		if(channel->ConfigTblPtr->MessageFlow[i].MsgId == 0)
		{

			/* We found an unused entry.  Set the message flow definition
			 * here.  First, make sure the requested priority queue is valid.
			 */
			if(TO_PriorityQueue_IsValid(channel, PQueueIdx) == FALSE)
			{
				/* This is an invalid priority queue. */
				(void) CFE_EVS_SendEvent(TO_CMD_ADD_MSG_FLOW_EID, CFE_EVS_ERROR,
						"Priority Queue Index %u is invalid.",
						PQueueIdx);
		    	TO_Channel_UnlockByRef(channel);
				goto end_of_function;
			}

			/* Now subscribe to the message to ensure the message ID is
			 * valid.
			 */
			iStatus = CFE_SB_SubscribeEx(MsgID, channel->DataPipeId,
										 CFE_SB_Default_Qos, MsgLimit);
			if (iStatus != CFE_SUCCESS)
			{
				(void) CFE_EVS_SendEvent(TO_CMD_ADD_MSG_FLOW_EID, CFE_EVS_ERROR,
						"Message flow failed to subscribe to (0x%08X). (%i)",
						MsgID,
						(unsigned int)iStatus);
		    	TO_Channel_UnlockByRef(channel);
				goto end_of_function;
			}

			/* Now that the message was successfully subscribed to, set the
			 * message flow definition.
			 */
			channel->ConfigTblPtr->MessageFlow[i].MsgId = MsgID;
			channel->ConfigTblPtr->MessageFlow[i].MsgLimit = MsgLimit;
			channel->ConfigTblPtr->MessageFlow[i].PQueueID = PQueueIdx;
			channel->DumpTbl.MessageFlow[i].DroppedMsgCnt = 0;
			channel->DumpTbl.MessageFlow[i].QueuedMsgCnt = 0;

			CFE_TBL_Modified(channel->ConfigTblHdl);
			CFE_TBL_Modified(channel->DumpTblHdl);

			added = TRUE;

			break;
		}
	}

	TO_Channel_UnlockByRef(channel);

end_of_function:
	return added;
}



/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Remove a message flow                                           */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
boolean TO_MessageFlow_Remove(uint16 ChannelIdx, CFE_SB_MsgId_t MsgID)
{
	uint32 msgFlowIndex = 0;
	TO_MessageFlow_t *msgFlow = 0;
    boolean removed = FALSE;
	int32 iStatus = CFE_SUCCESS;

    TO_ChannelData_t *channel;

    /* First, check if the channel index is valid. */
    if(ChannelIdx >= TO_MAX_CHANNELS)
    {
    	(void) CFE_EVS_SendEvent(TO_CMD_REMOVE_MSG_FLOW_EID, CFE_EVS_ERROR,
    		"Invalid channel index.");
    	goto end_of_function;
    }
    channel = &TO_AppData.ChannelData[ChannelIdx];

	TO_Channel_LockByRef(channel);

    /* Next, see if the channel is open. */
    if(channel->State != TO_CHANNEL_OPENED)
    {
    	(void) CFE_EVS_SendEvent(TO_CMD_REMOVE_MSG_FLOW_EID, CFE_EVS_ERROR,
    		"Channel not open.");
    	TO_Channel_UnlockByRef(channel);
    	goto end_of_function;
    }

    /* Next, see if there is a flow with this message ID */
    msgFlow = TO_MessageFlow_GetObject(channel, MsgID, &msgFlowIndex);
    if(msgFlow == 0)
    {
    	/* A message flow does not exist.  Reject the request. */
		(void) CFE_EVS_SendEvent(TO_CMD_REMOVE_MSG_FLOW_EID, CFE_EVS_ERROR,
	                      "Message flow is not defined.");
    	TO_Channel_UnlockByRef(channel);
		goto end_of_function;
    }

    /* Now that we have the message flow object, unsubscribe from it and clear
     * the entries. */
	iStatus =  CFE_SB_Unsubscribe(
			msgFlow->MsgId,
			channel->DataPipeId);
	if (iStatus != CFE_SUCCESS)
	{
		(void) CFE_EVS_SendEvent(TO_CMD_REMOVE_MSG_FLOW_EID, CFE_EVS_ERROR,
				"Message flow failed to unsubscribe from 0x%08X. (%i)",
				msgFlow->MsgId,
				(unsigned int)iStatus);
    	TO_Channel_UnlockByRef(channel);
		goto end_of_function;
	}

	/* Now just clear the entries. */
	channel->ConfigTblPtr->MessageFlow[msgFlowIndex].MsgId = 0;
	channel->ConfigTblPtr->MessageFlow[msgFlowIndex].MsgLimit = 0;
	channel->ConfigTblPtr->MessageFlow[msgFlowIndex].PQueueID = 0;
	channel->DumpTbl.MessageFlow[msgFlowIndex].DroppedMsgCnt = 0;
	channel->DumpTbl.MessageFlow[msgFlowIndex].QueuedMsgCnt = 0;

	TO_Channel_UnlockByRef(channel);

	CFE_TBL_Modified(channel->ConfigTblHdl);
	CFE_TBL_Modified(channel->DumpTblHdl);

    removed = TRUE;

end_of_function:
	return removed;
}



/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Query a message flow                                            */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
boolean TO_MessageFlow_Query(uint16 ChannelIdx, CFE_SB_MsgId_t MsgID)
{
	uint32 i = 0;
	TO_MessageFlow_t *outMsgFlow = 0;
    boolean found = FALSE;

    TO_ChannelData_t *channel;

    /* First, check if the channel index is valid. */
    if(ChannelIdx >= TO_MAX_CHANNELS)
    {
    	(void) CFE_EVS_SendEvent(TO_CMD_ADD_MSG_FLOW_EID, CFE_EVS_ERROR,
    		"Invalid channel index.");
    	goto end_of_function;
    }
    channel = &TO_AppData.ChannelData[ChannelIdx];

	TO_Channel_LockByRef(channel);

    /* Next, see if the channel is open. */
    if(channel->State != TO_CHANNEL_OPENED)
    {
    	(void) CFE_EVS_SendEvent(TO_CMD_ADD_MSG_FLOW_EID, CFE_EVS_ERROR,
    		"Channel not open.");
    	TO_Channel_UnlockByRef(channel);
    	goto end_of_function;
    }

	for(i = 0; i < TO_MAX_MESSAGE_FLOWS; ++i)
	{
		if(channel->ConfigTblPtr->MessageFlow[i].MsgId == MsgID)
		{
			(void) CFE_EVS_SendEvent(TO_MSG_FLOW_INFO_EID, CFE_EVS_INFORMATION,
							  "MID=0x%04x ML=%u PQI=%u D=%u Q=%u",
							  channel->ConfigTblPtr->MessageFlow[i].MsgId,
							  channel->ConfigTblPtr->MessageFlow[i].MsgLimit,
							  channel->ConfigTblPtr->MessageFlow[i].PQueueID,
							  channel->DumpTbl.MessageFlow[i].DroppedMsgCnt,
							  channel->DumpTbl.MessageFlow[i].QueuedMsgCnt);
			found = TRUE;
			break;
		}
	}

	TO_Channel_UnlockByRef(channel);

	if(found != TRUE)
	{
		(void) CFE_EVS_SendEvent(TO_MSG_FLOW_INFO_EID, CFE_EVS_ERROR,
		                      "MsgID=0x%04x not found", MsgID);
	}

end_of_function:

	return found;
}

