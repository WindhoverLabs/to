#include "to_app.h"
#include <string.h>



void TO_PriorityQueue_ResetCountsAll(void)
{
	uint32 i = 0;

	for(i=0; i < TO_MAX_PRIORITY_QUEUES; ++i)
	{
		TO_AppData.Config.PriorityQueue[i].DroppedMsgCnt = 0;
		TO_AppData.Config.PriorityQueue[i].QueuedMsgCnt = 0;
		TO_AppData.Config.PriorityQueue[i].HighwaterMark = 0;
	}
}



int32 TO_PriorityQueue_BuildupAll(void)
{
	uint32 i = 0;
	int32 iStatus = CFE_SUCCESS;

    /*
     * Create the priority queues.
     */
    for(i=0; i < TO_MAX_PRIORITY_QUEUES; ++i)
    {
    	if(TO_AppData.Config.PriorityQueue[i].State != TO_PQUEUE_UNUSED)
    	{
			char QueueName[OS_MAX_API_NAME];
			snprintf(QueueName, OS_MAX_API_NAME, "TO_PQ_%u", (unsigned int)i);
			iStatus = OS_QueueCreate(
					&TO_AppData.Config.PriorityQueue[i].OSALQueueID,
					QueueName,
					TO_AppData.Config.OutputChannel[i].MsgLimit,
					sizeof(CFE_SB_Msg_t*),
					0);
			if (iStatus != OS_SUCCESS)
			{
				 (void) CFE_EVS_SendEvent(TO_CONFIG_TABLE_ERR_EID, CFE_EVS_ERROR,
										  "Failed to create priority queue. (%i)",
										  (unsigned int)iStatus);
				 goto end_of_function;

			}
    	}
    }

    end_of_function:
    	return iStatus;
}


int32 TO_PriorityQueue_TeardownAll(void)
{
	uint32 i = 0;
	int32 iStatus = 0;

	for(i=0; i < TO_MAX_PRIORITY_QUEUES; ++i)
	{
		if(TO_AppData.Config.PriorityQueue[i].State != TO_PQUEUE_UNUSED)
		{
			if(TO_AppData.Config.PriorityQueue[i].OSALQueueID !=0)
			{
				CFE_SB_MsgPtr_t  msgPtr = 0;
				uint32 msgSize = 0;
				while(iStatus == OS_SUCCESS)
				{
					iStatus =  OS_QueueGet(
							TO_AppData.Config.PriorityQueue[i].OSALQueueID,
							&msgPtr, sizeof(msgPtr), &msgSize, OS_CHECK);
					if(iStatus == OS_SUCCESS)
					{
						iStatus = CFE_ES_PutPoolBuf(TO_AppData.HkTlm.MemPoolHandle, (uint32*)msgPtr);
						if(iStatus != OS_SUCCESS)
						{
							(void) CFE_EVS_SendEvent(TO_CONFIG_TABLE_ERR_EID, CFE_EVS_ERROR,
									"Failed to return message back to memory pool on tbl load. (%i)",
									(unsigned int)iStatus);
							goto end_of_function;
						}
					}
				}
				if(iStatus != OS_QUEUE_EMPTY)
				{
					(void) CFE_EVS_SendEvent(TO_CONFIG_TABLE_ERR_EID, CFE_EVS_ERROR,
							"Message flow failed to pop all messages from pqueue %u. (%i)",
							(unsigned int)i,
							(unsigned int)iStatus);
					goto end_of_function;
				}
				else
				{
					/* Queue is empty.  Delete the queue. */
					iStatus = OS_QueueDelete(
							TO_AppData.Config.PriorityQueue[i].OSALQueueID);
					if(iStatus != OS_SUCCESS)
					{
						(void) CFE_EVS_SendEvent(TO_CONFIG_TABLE_ERR_EID, CFE_EVS_ERROR,
								"Failed to delete priority queue %u. (%i)",
								(unsigned int)i,
								(unsigned int)iStatus);
						goto end_of_function;
					}
				}
			}
		}
	}

end_of_function:
	return iStatus;
}



void TO_PriorityQueue_CleanupAll(void )
{

}



int32 TO_PriorityQueue_QueueMsg(CFE_SB_MsgPtr_t MsgPtr, TO_TlmPriorityQueue_t* PQueue)
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
		iStatus = CFE_ES_PutPoolBuf(TO_AppData.HkTlm.MemPoolHandle, (uint32*)msgCopy);
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



boolean TO_PriorityQueue_Query(uint16 PQueueIdx)
{
	boolean rc = FALSE;

	if(PQueueIdx >= TO_MAX_PRIORITY_QUEUES)
	{
		(void) CFE_EVS_SendEvent(TO_PQUEUE_INFO_EID, CFE_EVS_ERROR,
		                      "PQueueIdx %u out of bounds", PQueueIdx);
	}
	else
	{
		TO_TlmPriorityQueue_t *pqueue = (TO_TlmPriorityQueue_t*)&TO_AppData.Config.PriorityQueue[PQueueIdx];
		(void) CFE_EVS_SendEvent(TO_PQUEUE_INFO_EID, CFE_EVS_INFORMATION,
							  "PQI=%u S=%u OCI=%u ML=%u QT=%u D=%u Q=%u CQ=%u HWM=%u",
							  PQueueIdx,
							  pqueue->ChannelID,
							  pqueue->State,
							  pqueue->MsgLimit,
							  pqueue->QType,
							  pqueue->DroppedMsgCnt,
							  pqueue->QueuedMsgCnt,
							  pqueue->CurrentlyQueuedCnt,
							  pqueue->HighwaterMark);
		rc = TRUE;
	}

	return rc;
}
