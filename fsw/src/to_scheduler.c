#include "to_app.h"
#include "to_scheduler.h"
#include "to_output_queue.h"



/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Run the Scheduler algorithm                                     */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void TO_Scheduler_Run(TO_ChannelData_t *channel)
{
	uint32 i = 0;

	for(i=0; i < TO_MAX_PRIORITY_QUEUES; ++i)
	{
		int32 iStatus = 0;
		TO_PriorityQueue_t *pqueue = &channel->ConfigTblPtr->PriorityQueue[i];
		if(pqueue->State != TO_PQUEUE_UNUSED)
		{
			if(channel->DumpTbl.PriorityQueue[i].OSALQueueID !=0)
			{
				TO_OutputQueue_t *oqueue = &channel->OutputQueue;
				void *buffer;
				uint32 bufferSize = 0;
				while((iStatus == OS_SUCCESS) && (oqueue->CurrentlyQueuedCnt < TO_OUTPUT_QUEUE_DEPTH))
				{
					iStatus =  OS_QueueGet(
							channel->DumpTbl.PriorityQueue[i].OSALQueueID,
							&buffer, sizeof(buffer), &bufferSize, OS_CHECK);
					if(iStatus == OS_SUCCESS)
					{
						channel->DumpTbl.PriorityQueue[i].CurrentlyQueuedCnt--;
						iStatus == TO_OutputQueue_QueueMsg(channel, buffer);
		            	if(iStatus == CFE_SUCCESS)
		            	{
		            		oqueue->CurrentlyQueuedCnt++;
		                	if(oqueue->HighwaterMark < oqueue->CurrentlyQueuedCnt)
		                	{
		                		oqueue->HighwaterMark++;
		                	}
		            	}

		    			CFE_TBL_Modified(channel->DumpTblHdl);
					}
				}
			}
		}
	}
}
