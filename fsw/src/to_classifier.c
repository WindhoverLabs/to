#include "to_app.h"
#include "to_classifier.h"
#include "to_message_flow.h"
#include "to_priority_queue.h"



/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Run the Classifier algorithm                                    */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void TO_Classifier_Run(TO_ChannelData_t* channel)
{
    int32 iStatus = CFE_SUCCESS;
    CFE_SB_MsgPtr_t   DataMsgPtr=NULL;
    uint32          i = 0;

    /* Process telemetry messages till the pipe is empty, or until we hit the
     * maximum number of messages we want to process in this frame. */
    for(i = 0; i < TO_MAX_MSGS_OUT_PER_FRAME; ++i)
    {
        iStatus = CFE_SB_RcvMsg(&DataMsgPtr, channel->DataPipeId, CFE_SB_POLL);
        if (iStatus == CFE_SUCCESS)
        {
        	TO_PriorityQueue_t *pqueue = 0;
        	TO_MessageFlow_t *msgFlow = 0;
        	int32 msgFlowIndex = 0;
        	int32 pQueueIndex = 0;

        	CFE_SB_MsgId_t DataMsgID = CFE_SB_GetMsgId(DataMsgPtr);

        	/* Get the first Message Flow object.  If this returns null, the
        	 * Message ID is not in the table at all so we shouldn't have
        	 * received this message.  Raise an event.
        	 */
    		msgFlow = TO_MessageFlow_GetObject(channel, DataMsgID, &msgFlowIndex);
			if(msgFlow == 0)
			{
				(void) CFE_EVS_SendEvent(TO_MSGID_ERR_EID, CFE_EVS_ERROR,
								  "Recvd invalid SCH msgId (0x%04X)", (unsigned short)DataMsgID);
			}

			/* Get the Priority Queue assigned to this Message Flow. */
			pqueue = TO_MessageFlow_GetPQueue(channel, msgFlow, &pQueueIndex);
			if(pqueue != 0)
			{
				/* Queue the message. */
				iStatus = TO_PriorityQueue_QueueMsg(channel, DataMsgPtr, pQueueIndex);
				if(iStatus == CFE_SUCCESS)
				{
					/* The message was queued.  Increment counters. */
					channel->DumpTbl.MessageFlow[msgFlowIndex].QueuedMsgCnt++;
					channel->DumpTbl.PriorityQueue[pQueueIndex].QueuedMsgCnt++;
					channel->DumpTbl.PriorityQueue[pQueueIndex].CurrentlyQueuedCnt++;
					if(channel->DumpTbl.PriorityQueue[pQueueIndex].HighwaterMark <
							channel->DumpTbl.PriorityQueue[pQueueIndex].CurrentlyQueuedCnt)
					{
						channel->DumpTbl.PriorityQueue[pQueueIndex].HighwaterMark++;
					}
				}
				else
				{
					/* Queue is full.  Increment counters and drop the message. */
					channel->DumpTbl.MessageFlow[msgFlowIndex].DroppedMsgCnt++;
					channel->DumpTbl.PriorityQueue[pQueueIndex].DroppedMsgCnt++;
					CFE_EVS_SendEvent(TO_MSG_DROP_FROM_FLOW_DBG_EID, CFE_EVS_DEBUG,
								  "PQ full.  Dropped message 0x%04x", (unsigned int)DataMsgID);
				}
			}
        }
        else if (iStatus == CFE_SB_NO_MESSAGE)
        {
            break;
        }
        else
        {
            CFE_EVS_SendEvent(TO_PIPE_ERR_EID, CFE_EVS_ERROR,
                              "TO: data pipe read error (0x%08X)", (int)iStatus);
            break;
        }
    }
}
