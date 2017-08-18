#include "to_app.h"
#include "to_output_queue.h"
#include "to_custom.h"



/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Initialize all output channel.  This really does nothing        */
/* but flows down to the custom function.                          */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
int32 TO_OutputQueue_Init(void)
{
	return TO_OutputQueue_CustomInit();
}



/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Cleanup all output channels before shutdown.  This really does  */
/* nothing flow down to the custom function.                       */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void TO_OutputQueue_Cleanup(void)
{
	//TO_OutputQueue_Teardown();
	//TO_OutputQueue_CustomCleanup();
}



/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Buildup all output channels after a reconfiguration             */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
int32 TO_OutputQueue_Buildup(uint32 idx)
{
	int32 iStatus = 0;

	if(idx < TO_MAX_CHANNELS)
	{
		/*
		 * Now that we have the new table, create all the new resources we need
		 * starting with queues for the channels and priority queues.
		 */
		char QueueName[OS_MAX_API_NAME];
		snprintf(QueueName, OS_MAX_API_NAME, "TO_CH_%u", (unsigned int)idx);
		iStatus = OS_QueueCreate(
				&TO_AppData.Config.OutputChannel.OSALQueueID,
				QueueName,
				TO_AppData.Config.OutputChannel.MsgLimit,
				sizeof(CFE_SB_Msg_t*),
				0);
		if (iStatus != OS_SUCCESS)
		{
			(void) CFE_EVS_SendEvent(TO_CONFIG_TABLE_ERR_EID, CFE_EVS_ERROR,
									 "Failed to create output channel[%i] queue. err=%i",
									 idx, (unsigned int)iStatus);
			goto end_of_function;

		}

		TO_OutputQueue_CustomBuildup();
	}
	else
	{
		iStatus = -1;
	}

end_of_function:
	return iStatus;
}



/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Teardown all output channels in preparation for a               */
/* reconfiguration.                                                */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
int32 TO_OutputQueue_Teardown(uint32 idx)
{
	int32 iStatus = 0;

	TO_OutputQueue_CustomTeardown();

	if(idx < TO_MAX_OUTPUT_CHANNELS)
	{
		if(TO_AppData.Config.OutputChannel.OSALQueueID !=0)
		{
			void *buffer;
			uint32 bufferSize = 0;
			while(iStatus == OS_SUCCESS)
			{
				iStatus =  OS_QueueGet(
						TO_AppData.Config.OutputChannel[idx].OSALQueueID,
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
						"Message flow failed to pop all messages from queue %u. (%i)",
						(unsigned int)idx,
						(unsigned int)iStatus);
				goto end_of_function;
			}
			else
			{
				/* Queue is empty.  Delete the queue. */
				iStatus = OS_QueueDelete(
						TO_AppData.Config.OutputChannel[idx].OSALQueueID);
				if(iStatus != OS_SUCCESS)
				{
					(void) CFE_EVS_SendEvent(TO_CONFIG_TABLE_ERR_EID, CFE_EVS_ERROR,
							"Failed to delete priority queue %u. (%i)",
							(unsigned int)idx,
							(unsigned int)iStatus);
					goto end_of_function;
				}
			}
		}
	}
	else
	{
		iStatus = -1;
	}

end_of_function:
	return iStatus;
}



/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Reset all dynamic metrics.                                      */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void TO_OutputChannel_ResetCounts(uint32 idx)
{
	if(idx < TO_MAX_OUTPUT_CHANNELS)
	{
		TO_AppData.Config.OutputChannel[idx].SentCount = 0;
		TO_AppData.Config.OutputChannel[idx].HighwaterMark = 0;
	}
}



/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Push a message on the output channel queue.                     */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
int32 TO_OutputChannel_QueueMsg(CFE_SB_MsgPtr_t MsgPtr, TO_TlmOutputChannelQueue_t* OutChannel)
{
	int32 iStatus = 0;

	if(OutChannel == 0)
	{
		iStatus = -2;
		goto end_of_function;
	}

    iStatus = OS_QueuePut(OutChannel->OSALQueueID, &MsgPtr, sizeof(MsgPtr), 0);
    if(iStatus == OS_QUEUE_FULL)
    {
    	/* This is not supposed to happen since we only queue when the channel
    	 * is not full, but trap it anyway.  Let the caller no the queue is
    	 * full and deallocate the memory allocated first since we don't need
    	 * it anymore.
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
						  sizeof(MsgPtr), (int)iStatus);
        goto end_of_function;
    }

end_of_function:
    return iStatus;
}



/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Query an output channel.                                        */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
boolean TO_OutputChannel_Query(uint16 idx)
{
	boolean rc = FALSE;

	if(OutputChannelIdx >= TO_MAX_OUTPUT_CHANNELS)
	{
		(void) CFE_EVS_SendEvent(TO_OUT_CH_INFO_EID, CFE_EVS_ERROR,
		                      "Output Channel %u out of bounds", idx);
	}
	else
	{
		TO_TlmOutputChannelQueue_t *channel = (TO_TlmOutputChannelQueue_t*)&TO_AppData.Config.OutputChannel[idx];
		(void) CFE_EVS_SendEvent(TO_OUT_CH_INFO_EID, CFE_EVS_INFORMATION,
							  "OCI=%u S=%u ML=%u SC=%u CQC=%u HWM=%u",
							  OutputChannelIdx,
							  channel->State,
							  channel->MsgLimit,
							  channel->SentCount,
							  channel->CurrentlyQueuedCnt,
							  channel->HighwaterMark);
		rc = TRUE;
	}

	return rc;
}
