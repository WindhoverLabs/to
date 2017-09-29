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
	return 0; //TO_OutputQueue_CustomInit();
}



/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Cleanup all output channels before shutdown.  This really does  */
/* nothing flow down to the custom function.                       */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void TO_OutputQueue_Cleanup(TO_ChannelData_t* channel)
{
}



/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Buildup all output channels after a reconfiguration             */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
int32 TO_OutputQueue_Buildup(TO_ChannelData_t* channel)
{
	int32 iStatus = 0;

	/*
	 * Now that we have the new table, create all the new resources we need
	 * starting with queues for the channels and priority queues.
	 */
	char QueueName[OS_MAX_API_NAME];
	snprintf(QueueName, OS_MAX_API_NAME, "TO_%s_OUT", channel->ChannelName);
	iStatus = OS_QueueCreate(
			&channel->OutputQueue.OSALQueueID,
			QueueName,
			TO_OUTPUT_QUEUE_DEPTH,
			sizeof(CFE_SB_Msg_t*),
			0);
	if (iStatus != OS_SUCCESS)
	{
		(void) CFE_EVS_SendEvent(TO_CONFIG_TABLE_ERR_EID, CFE_EVS_ERROR,
								 "Failed to create '%s' output channel queue. err=%i",
								 channel->ChannelName, (unsigned int)iStatus);
		goto end_of_function;
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
int32 TO_OutputQueue_Teardown(TO_ChannelData_t* channel)
{
	int32 iStatus = 0;

	//TO_OutputQueue_CustomTeardown();

	if(channel->OutputQueue.OSALQueueID !=0)
	{
		void *buffer;
		uint32 bufferSize = 0;
		while(iStatus == OS_SUCCESS)
		{
			iStatus =  OS_QueueGet(
					channel->OutputQueue.OSALQueueID,
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
            	OS_MutSemTake(TO_AppData.MutexID);
			    TO_AppData.HkTlm.MemInUse -= iStatus;
            	OS_MutSemGive(TO_AppData.MutexID);
			}
		}
		if(iStatus != OS_QUEUE_EMPTY)
		{
			(void) CFE_EVS_SendEvent(TO_CONFIG_TABLE_ERR_EID, CFE_EVS_ERROR,
					"Failed to pop all messages from channel '%s' output queue. (%i)",
					channel->ChannelName,
					(unsigned int)iStatus);
			goto end_of_function;
		}
	}

end_of_function:
	return iStatus;
}



/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Reset all dynamic metrics.                                      */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void TO_OutputQueue_ResetCounts(TO_ChannelData_t* channel)
{
	channel->OutputQueue.SentCount = 0;
	channel->OutputQueue.HighwaterMark = 0;
}



/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Push a message on the output channel queue.                     */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
int32 TO_OutputQueue_QueueMsg(TO_ChannelData_t* channel, CFE_SB_MsgPtr_t MsgPtr)
{
	int32 iStatus = 0;

    iStatus = OS_QueuePut(channel->OutputQueue.OSALQueueID, &MsgPtr, sizeof(MsgPtr), 0);
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
	    TO_AppData.HkTlm.MemInUse -= iStatus;
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
boolean TO_OutputChannel_Query(uint16 ChannelIdx)
{
	boolean rc = FALSE;

    TO_ChannelData_t *channel;

    /* First, check if the channel index is valid. */
    if(ChannelIdx >= TO_MAX_CHANNELS)
    {
    	(void) CFE_EVS_SendEvent(TO_OUT_CH_INFO_EID, CFE_EVS_ERROR,
    		"Invalid channel index.");
    	goto end_of_function;
    }
    channel = &TO_AppData.ChannelData[ChannelIdx];

    /* Next, see if the channel is open. */
    if(channel->State != TO_CHANNEL_OPENED)
    {
    	(void) CFE_EVS_SendEvent(TO_OUT_CH_INFO_EID, CFE_EVS_ERROR,
    		"Channel not open.");
    	goto end_of_function;
    }

	(void) CFE_EVS_SendEvent(TO_OUT_CH_INFO_EID, CFE_EVS_INFORMATION,
							  "S=%u ML=%u SC=%u CQC=%u HWM=%u",
							  channel->State,
							  TO_OUTPUT_QUEUE_DEPTH,
							  channel->OutputQueue.SentCount,
							  channel->OutputQueue.CurrentlyQueuedCnt,
							  channel->OutputQueue.HighwaterMark);
	rc = TRUE;

end_of_function:

	return rc;

}
