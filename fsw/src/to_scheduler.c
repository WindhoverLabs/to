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
