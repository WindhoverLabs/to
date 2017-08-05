#include "to_app.h"
#include "to_output_channel.h"
#include "to_custom.h"



int32 TO_OutputChannel_InitAll(void)
{
	return TO_OutputChannel_CustomInitAll();
}



void TO_OutputChannel_CleanupAll(void)
{
	TO_OutputChannel_CustomCleanupAll();
}



int32 TO_OutputChannel_BuildupAll(void)
{
	uint32 i = 0;
	int32 iStatus = 0;

   /*
	 * Now that we have the new table, create all the new resources we need
	 * starting with queues for the channels and priority queues.
	 */
	for(i=0; i < TO_MAX_OUTPUT_CHANNELS; ++i)
	{
		if(TO_AppData.Config.OutputChannel[i].State != TO_OUT_CHANNEL_UNUSED)
		{
			char QueueName[OS_MAX_API_NAME];
			snprintf(QueueName, OS_MAX_API_NAME, "TO_CH_%u", (unsigned int)i);
			iStatus = OS_QueueCreate(
					&TO_AppData.Config.OutputChannel[i].OSALQueueID,
					QueueName,
					TO_AppData.Config.OutputChannel[i].MsgLimit,
					sizeof(CFE_SB_Msg_t*),
					0);
			if (iStatus != OS_SUCCESS)
			{
				(void) CFE_EVS_SendEvent(TO_CONFIG_TABLE_ERR_EID, CFE_EVS_ERROR,
										 "Failed to create output channel queue. (%i)",
										 (unsigned int)iStatus);
				goto end_of_function;

			}
		}
	}

    TO_OutputChannel_CustomBuildupAll();

end_of_function:
	return iStatus;
}



int32 TO_OutputChannel_TeardownAll(void)
{
	uint32 i = 0;
	int32 iStatus = 0;

	TO_OutputChannel_CustomTeardownAll();

	for(i=0; i < TO_MAX_OUTPUT_CHANNELS; ++i)
	{
		if(TO_AppData.Config.OutputChannel[i].State != TO_PQUEUE_UNUSED)
		{
			if(TO_AppData.Config.OutputChannel[i].OSALQueueID !=0)
			{
				void *buffer;
				uint32 bufferSize = 0;
				while(iStatus == OS_SUCCESS)
				{
					iStatus =  OS_QueueGet(
							TO_AppData.Config.OutputChannel[i].OSALQueueID,
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
							(unsigned int)i,
							(unsigned int)iStatus);
					goto end_of_function;
				}
				else
				{
					/* Queue is empty.  Delete the queue. */
					iStatus = OS_QueueDelete(
							TO_AppData.Config.OutputChannel[i].OSALQueueID);
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



void TO_OutputChannel_ResetCountsAll(void)
{
	uint32 i = 0;

	for(i=0; i < TO_MAX_OUTPUT_CHANNELS; ++i)
	{
		TO_AppData.Config.OutputChannel[i].SentCount = 0;
		TO_AppData.Config.OutputChannel[i].HighwaterMark = 0;
	}
}



int32 TO_OutputChannel_QueueMsg(CFE_SB_MsgPtr_t MsgPtr, TO_TlmOutputChannelQueue_t* OutChannel)
{
	int32 iStatus = 0;

	if(OutChannel == 0)
	{
		iStatus = -2;
		goto end_of_function;
	}

    //uint16 *buf = (uint16*)MsgPtr;
    ///* Queue the pointer to the message. */
	//OS_printf("TO_OutputChannel_QueueMsg(0x%08lx)  (%04x %04x %04x)\n",  MsgPtr, buf[0], buf[1], buf[2]);
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
						  sizeof(MsgPtr), (int)iStatus);
        goto end_of_function;
    }

end_of_function:
    return iStatus;
}



boolean TO_OutputChannel_Query(uint16 OutputChannelIdx)
{
	boolean rc = FALSE;

	if(OutputChannelIdx >= TO_MAX_OUTPUT_CHANNELS)
	{
		(void) CFE_EVS_SendEvent(TO_OUT_CH_INFO_EID, CFE_EVS_ERROR,
		                      "Output Channel %u out of bounds", OutputChannelIdx);
	}
	else
	{
		TO_TlmOutputChannelQueue_t *channel = (TO_TlmOutputChannelQueue_t*)&TO_AppData.Config.OutputChannel[OutputChannelIdx];
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
