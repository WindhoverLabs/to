#include "to_scheduler.h"
#include "to_app.h"







/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Run the Scheduler algorithm                                     */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void TO_RunScheduler()
{
	uint32 i = 0;

	for(i=0; i < TO_MAX_PRIORITY_QUEUES; ++i)
	{
		int32 iStatus = 0;
		TO_TlmPriorityQueue_t *pqueue = &TO_AppData.Config.PriorityQueue[i];
		if(pqueue->State != TO_PQUEUE_UNUSED)
		{
			if(pqueue->OSALQueueID !=0)
			{
				TO_TlmOutputChannelQueue_t *channel = &TO_AppData.Config.OutputChannel[pqueue->ChannelID];
				CFE_SB_MsgPtr_t  msgPtr = 0;
				uint32 msgSize = 0;
				while((iStatus == OS_SUCCESS) && (channel->QueuedCount < channel->MsgLimit))
				{
					iStatus =  OS_QueueGet(
							TO_AppData.Config.PriorityQueue[i].OSALQueueID,
							&msgPtr, sizeof(msgPtr), &msgSize, OS_CHECK);
					if(iStatus == OS_SUCCESS)
					{
						iStatus == TO_QueueMessageInOutChannel(msgPtr, channel);
		            	if(iStatus == CFE_SUCCESS)
		            	{
		            		channel->QueuedCount++;
		            	}
					}
				}
			}
		}
	}
}




int32 TO_QueueMessageInOutChannel(CFE_SB_MsgPtr_t MsgPtr, TO_TlmOutputChannelQueue_t* OutChannel)
{
	int32 iStatus = 0;

	if(OutChannel == 0)
	{
		iStatus = -2;
		goto end_of_function;
	}

    /* Queue the pointer to the message. */
    iStatus = OS_QueuePut(OutChannel->OSALQueueID, &MsgPtr, sizeof(MsgPtr), 0);
    if(iStatus == OS_QUEUE_FULL)
    {
    	/* This is ok.  Just let the caller no the queue is full.  But,
    	 * deallocate the memory allocated first since we don't need it
    	 * anymore.
    	 */
		iStatus = CFE_ES_PutPoolBuf(TO_AppData.HkTlm.MemPoolHandle, (uint32 *)MsgPtr);
	    if(iStatus < 0)
    	{
        	(void) CFE_EVS_SendEvent(TO_GET_POOL_ERR_EID, CFE_EVS_ERROR,
                             "PutPoolBuf: error=%i",
                             (int)iStatus);
        	goto end_of_function;
    	}
    }
    else if(iStatus != CFE_SUCCESS)
    {
        (void) CFE_EVS_SendEvent(TO_GET_POOL_ERR_EID, CFE_EVS_ERROR,
                          "OS_QueuePut failed: size=%u error=%i",
						  sizeof(&MsgPtr), (int)iStatus);
        goto end_of_function;
    }

end_of_function:
    return iStatus;
}
