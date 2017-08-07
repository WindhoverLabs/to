#include "to_app.h"
#include <string.h>



/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Clear all dynamics metrics.                                     */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
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



/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Buildup all priority queues after a reconfiguration.            */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
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
					TO_AppData.Config.PriorityQueue[i].MsgLimit,
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



/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Teardown all priority queues after a reconfiguration.           */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
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
				void *buffer;
				uint32 bufferSize = 0;
				while(iStatus == OS_SUCCESS)
				{
					iStatus =  OS_QueueGet(
							TO_AppData.Config.PriorityQueue[i].OSALQueueID,
							&buffer, sizeof(buffer), &bufferSize, OS_CHECK);
					if(iStatus == OS_SUCCESS)
					{
						iStatus = CFE_ES_PutPoolBuf(TO_AppData.HkTlm.MemPoolHandle, (uint32*)buffer);
						if(iStatus < 0)
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



/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Cleanup all priority queues for termination.                    */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void TO_PriorityQueue_CleanupAll(void )
{
	TO_PriorityQueue_TeardownAll();
}



/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Push a message on a priority queue.                             */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
int32 TO_PriorityQueue_QueueMsg(CFE_SB_MsgPtr_t MsgPtr, TO_TlmPriorityQueue_t* PQueue)
{
	int32 iStatus = 0;
	void *copyBuffer;
	uint32 bufferSize = 0;

	if(PQueue == 0)
	{
		iStatus = -2;
		goto end_of_function;
	}

	bufferSize = CFE_SB_GetTotalMsgLength(MsgPtr);

	/* Allocate a chunk of memory from the memory pool to store the message
	 * copy.
	 */
    iStatus = CFE_ES_GetPoolBuf ((uint32 **) &copyBuffer,
    		TO_AppData.HkTlm.MemPoolHandle, bufferSize);
    if(iStatus < bufferSize)
    {
        (void) CFE_EVS_SendEvent(TO_GET_POOL_ERR_EID, CFE_EVS_ERROR,
                          "GetPoolBuf failed: size=%lu error=%i",
						  bufferSize, (int)iStatus);
        goto end_of_function;
    }

    /* Copy the message into the newly allocated memory. */
    memcpy(copyBuffer, MsgPtr, bufferSize);

    /* Queue the pointer to the message copy. */
    iStatus = OS_QueuePut(PQueue->OSALQueueID, &copyBuffer, sizeof(copyBuffer), 0);
    if(iStatus == OS_QUEUE_FULL)
    {
    	/* This is ok.  Just let the caller no the queue is full.  But,
    	 * deallocate the memory allocated first since we don't need it
    	 * anymore.
    	 */
		iStatus = CFE_ES_PutPoolBuf(TO_AppData.HkTlm.MemPoolHandle, (uint32*)copyBuffer);
	    if(iStatus < 0)
    	{
        	(void) CFE_EVS_SendEvent(TO_GET_POOL_ERR_EID, CFE_EVS_ERROR,
                             "PutPoolBuf: error=%i",
                             (int)iStatus);
        	goto end_of_function;
    	}
	    iStatus = -1;
    }
    else if(iStatus != CFE_SUCCESS)
    {
        (void) CFE_EVS_SendEvent(TO_GET_POOL_ERR_EID, CFE_EVS_ERROR,
                          "OS_QueuePut failed: size=%u error=%i",
						  sizeof(copyBuffer), (int)iStatus);
        goto end_of_function;
    }

end_of_function:
    return iStatus;
}



/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Query a priority queue.                                         */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
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
							  "PQI=%u CI=%u S=%u ML=%u QT=%u D=%u Q=%u CQ=%u HWM=%u",
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



/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Is this index a valid priority queue?                           */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
boolean TO_PriorityQueue_IsValid(uint32 PQueueIdx)
{
	boolean isValid = TRUE;

	if(PQueueIdx >= TO_MAX_PRIORITY_QUEUES)
	{
		isValid = FALSE;
	}
	else if(TO_AppData.Config.PriorityQueue[PQueueIdx].State != TO_PQUEUE_ENA)
	{
		isValid = FALSE;
	}

	return isValid;
}
