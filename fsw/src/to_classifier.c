#include "to_classifier.h"
#include "to_app.h"

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Run the Classifier algorithm                                    */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void TO_RunClassifier()
{
    int32 iStatus = CFE_SUCCESS;
    CFE_SB_MsgPtr_t   DataMsgPtr=NULL;
    uint32          i = 0;

    /* Process telemetry messages till the pipe is empty, or until we hit the
     * maximum number of messages we want to process in this frame. */
    for(i = 0; i < TO_MAX_MSGS_OUT_PER_FRAME; ++i)
    {
        iStatus = CFE_SB_RcvMsg(&DataMsgPtr, TO_AppData.DataPipeId, CFE_SB_POLL);
        if (iStatus == CFE_SUCCESS)
        {
        	TO_TlmPriorityQueue_t *pqueue = 0;
        	TO_TlmMessageFlow_t *msgFlow = 0;

        	CFE_SB_MsgId_t DataMsgID = CFE_SB_GetMsgId(DataMsgPtr);

        	msgFlow = TO_GetMessageFlowObject(DataMsgID);

        	if(msgFlow == 0)
        	{
        		/* No message flow was found.  This message must have
        		 * just been removed from the message flow table.  Just
        		 * unsubscribe from this message. */
        		CFE_SB_Unsubscribe(DataMsgID, TO_AppData.DataPipeId);
                CFE_EVS_SendEvent(TO_MSG_UNSUB_DBG_EID, CFE_EVS_DEBUG,
                                  "Unsubscribed from 0x%04x", (unsigned int)DataMsgID);
                continue;
        	}

        	pqueue = TO_GetPQueueForMessageFlow(msgFlow);
        	if(pqueue != 0)
        	{
            	/* Queue the message. */
            	iStatus = TO_QueueMessageInPqueue(DataMsgPtr, pqueue);
            	if(iStatus == CFE_SUCCESS)
            	{
            		/* The message was queued.  Increment counters. */
                	msgFlow->TotalQueuedMsgCnt++;
                	msgFlow->CurrentQueuedMsgCnt++;
            	}
            	else
            	{
        			/* Queue is full.  Increment counters and drop the message. */
        			msgFlow->DroppedMsgCnt++;
                	pqueue->DroppedMsgCnt++;
        			CFE_EVS_SendEvent(TO_MSG_DROP_FROM_FLOW_DBG_EID, CFE_EVS_DEBUG,
                                  "Unsubscribed from 0x%04x", (unsigned int)DataMsgID);
        			continue;
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






TO_TlmMessageFlow_t* TO_GetMessageFlowObject(CFE_SB_MsgId_t MsgID)
{
	uint32 i = 0;
	TO_TlmMessageFlow_t *outMsgFlow = 0;

	for(i = 0; i < TO_MAX_MESSAGE_FLOWS; ++i)
	{
		if(TO_AppData.Config.MessageFlow[i].MsgId == MsgID)
		{
			outMsgFlow = &TO_AppData.Config.MessageFlow[i];
			break;
		}
	}

	return outMsgFlow;
}



TO_TlmPriorityQueue_t* TO_GetPQueueForMessageFlow(TO_TlmMessageFlow_t *MsgFlow)
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



int32 TO_QueueMessageInPqueue(CFE_SB_MsgPtr_t MsgPtr, TO_TlmPriorityQueue_t* PQueue)
{
	int32 iStatus = 0;
	CFE_SB_MsgPtr_t  msgCopy;
	uint16 msgSize = 0;

	if(PQueue == 0)
	{
		iStatus = -2;
		goto end_of_function;
	}

	msgSize = CFE_SB_GetTotalMsgLength(MsgPtr);

	/* Allocate a chunk of memory from the memory pool to store the message
	 * copy.
	 */
    iStatus = CFE_ES_GetPoolBuf ((uint32 **) &msgCopy,
    		TO_AppData.HkTlm.MemPoolHandle, msgSize);
    if(iStatus < msgSize)
    {
        (void) CFE_EVS_SendEvent(TO_GET_POOL_ERR_EID, CFE_EVS_ERROR,
                          "GetPoolBuf failed: size=%u error=%i",
                          msgSize, (int)iStatus);
        exit(1);
        goto end_of_function;
    }

    /* Copy the message into the newly allocated memory. */
    memcpy(msgCopy, MsgPtr, msgSize);

    /* Queue the pointer to the message copy. */
    iStatus = OS_QueuePut(PQueue->OSALQueueID, &msgCopy, sizeof(msgCopy), 0);
    if(iStatus == OS_QUEUE_FULL)
    {
    	/* This is ok.  Just let the caller no the queue is full.  But,
    	 * deallocate the memory allocated first since we don't need it
    	 * anymore.
    	 */
		iStatus = CFE_ES_PutPoolBuf(TO_AppData.HkTlm.MemPoolHandle, msgCopy);
	    if(iStatus < msgSize)
    	{
        	(void) CFE_EVS_SendEvent(TO_GET_POOL_ERR_EID, CFE_EVS_ERROR,
                             "PutPoolBuf: error=%i",
                             (int)iStatus);
        	goto end_of_function;
    	}
	    iStatus = CFE_SUCCESS;
    }
    else if(iStatus != CFE_SUCCESS)
    {
        (void) CFE_EVS_SendEvent(TO_GET_POOL_ERR_EID, CFE_EVS_ERROR,
                          "OS_QueuePut failed: size=%u error=%i",
						  sizeof(&msgCopy), (int)iStatus);
        goto end_of_function;
    }

end_of_function:
    return iStatus;
}
