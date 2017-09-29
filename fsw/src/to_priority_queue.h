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
* 3. Neither the name Windhover Labs nor the names of its 
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

#ifndef TO_PRIORITY_QUEUE_H
#define TO_PRIORITY_QUEUE_H

/************************************************************************
** Pragmas
*************************************************************************/

/************************************************************************
** Includes
*************************************************************************/

#include "cfe.h"

/************************************************************************
** Local Defines
*************************************************************************/

typedef struct TO_ChannelData_t TO_ChannelData_t;

/************************************************************************
** Local Structure Definitions
*************************************************************************/

/**
** \brief Priority queue state.  Used to determine if a table entry is used or not.
*/
typedef enum
{
	TO_PQUEUE_UNUSED = 0,
	TO_PQUEUE_ENA = 1,
} TO_PriorityQueueState_t;

/**
** \brief Priority queue type.  'FIFO' queues are normal FIFO queues, but 'SINGLE' queues have the message flow removed after a message is pushed onto the queue.
*/
typedef enum
{
	TO_PRIORITY_QUEUE_TYPE_FIFO = 0,
	TO_PRIORITY_QUEUE_TYPE_SINGLE,
} TO_PriorityQueueType_t;


/** \brief Definition for a single priority queue entry. */
typedef struct
{
	TO_PriorityQueueState_t State;
	uint16					MsgLimit;
	TO_PriorityQueueType_t  QType;
} TO_PriorityQueue_t;

typedef struct
{
	uint16					DroppedMsgCnt;
    uint16					QueuedMsgCnt;
    uint16					CurrentlyQueuedCnt;
    uint16					HighwaterMark;
    uint32					OSALQueueID;
} TO_PriorityQueueMetrics_t;



/************************************************************************
** External Global Variables
*************************************************************************/



/************************************************************************/
/** \brief Resets all metrics.
**
**  \par Description
**       This function is called at when the Reset command is received.
**       It resets all metrics, which include the Dropped Message Counts,
**       and Queued Message Counts.
**
*************************************************************************/
void TO_PriorityQueue_ResetCountsAll(TO_ChannelData_t* channel);



/************************************************************************/
/** \brief Cleanup all PRIORITY QUEUES.
**
**  \par Description
**       This function is call at application termination and empties
**       the queue, deallocating each message its popped off the queue.
**
*************************************************************************/
void TO_PriorityQueue_CleanupAll(TO_ChannelData_t* channel);



/************************************************************************/
/** \brief Buildup all priority queues.
**
**  \par Description
**       This function is called at when the application has loaded a new
**       configuration table.
**
**  \returns
**  0 if no error occurred.  On error, an OSAL error is returned
**  indicating what error occured.
**  \endreturns
**
*************************************************************************/
int32 TO_PriorityQueue_BuildupAll(TO_ChannelData_t* channel);



/************************************************************************/
/** \brief Queue message.
**
**  \par Description
**       This function is called by the Classifier to queue a message
**       into a priority queue.
**
**  \param [in]   MsgPtr        A #CFE_SB_Msg_t pointer that
**                              references the software bus message

**
**  \param [in]   PQueue        A #TO_PriorityQueue_t pointer to
**                              the priority queue object.
**
**  \returns
**  0 if successful.  OSAL error if unsuccessful.
**  \endreturns
**
*************************************************************************/
int32 TO_PriorityQueue_QueueMsg(TO_ChannelData_t *channel, CFE_SB_MsgPtr_t MsgPtr, uint32 PQueueIndex);



/************************************************************************/
/** \brief Teardown all priority queues.
**
**  \par Description
**       This function is called at when the application is about to load
**       a new configuration table.  This will flush the queues and
**       deallocate the messages contained in the queues.
**
**  \returns
**  0 if successful.  OSAL error if unsuccessful.
**  \endreturns
**
*************************************************************************/
int32 TO_PriorityQueue_TeardownAll(TO_ChannelData_t* channel);



/************************************************************************/
/** \brief Query a priority queue.
**
**  \par Description
**       This function is called when a ground command is received to
**       query a message flow.  An event is raised with the following
**       text format:
**       "PQI=<PQueueIndex> CI=<ChannelID> S=<State> ML=<MsgLimit> QT=<QueueType> D=<DroppedMsgCnt> Q=<QueuedMsgCnt> CQ=<CurrentlyQueuedCnt> HWM=<HighwaterMark>"
**
**  \param [in]   PQueueIdx     Index of the priority queue.
**
**  \returns
**  TRUE if priority queue is found.  FALSE if not found.
**  \endreturns
**
*************************************************************************/
boolean TO_PriorityQueue_Query(uint16 ChannelIdx, uint16 PQueueIdx);



/************************************************************************/
/** \brief Checks whether a priority index is valid or not.
**
**  \par Description
**       This function is used internally to determine if an index is
**       to a valid priority queue.
**
**  \param [in]   PQueueIdx     Index of the priority queue.
**
**  \returns
**  TRUE if priority queue is valid.  FALSE if not.
**  \endreturns
**
*************************************************************************/
boolean TO_PriorityQueue_IsValid(TO_ChannelData_t* channel, uint32 PQueueIdx);


#endif
