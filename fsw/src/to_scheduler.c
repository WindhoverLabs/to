
#include "to_app.h"
#include "to_scheduler.h"
#include "to_output_channel.h"







/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Run the Scheduler algorithm                                     */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void TO_Scheduler_Run()
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
				while((iStatus == OS_SUCCESS) && (channel->CurrentlyQueuedCnt < channel->MsgLimit))
				{
					iStatus =  OS_QueueGet(
							TO_AppData.Config.PriorityQueue[i].OSALQueueID,
							&msgPtr, sizeof(msgPtr), &msgSize, OS_CHECK);
					if(iStatus == OS_SUCCESS)
					{
						TO_AppData.Config.PriorityQueue[i].CurrentlyQueuedCnt--;
						iStatus == TO_OutputChannel_QueueMsg(msgPtr, channel);
		            	if(iStatus == CFE_SUCCESS)
		            	{
		            		channel->CurrentlyQueuedCnt++;
		                	if(channel->HighwaterMark < channel->CurrentlyQueuedCnt)
		                	{
		                		channel->HighwaterMark++;
		                	}
		            	}
					}
				}
			}
		}
	}
}
