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

#ifndef TO_MESSAGE_FLOW_H
#define TO_MESSAGE_FLOW_H

/************************************************************************
** Pragmas
*************************************************************************/

/************************************************************************
** Includes
*************************************************************************/

#include "cfe.h"
#include "to_priority_queue.h"

/************************************************************************
** Local Defines
*************************************************************************/

typedef struct TO_ChannelData_t TO_ChannelData_t;

/************************************************************************
** Local Structure Definitions
*************************************************************************/

/**
** \brief Definition for a single telemetry message flow entry.
*/
typedef struct
{
    CFE_SB_MsgId_t  MsgId;
    uint16          MsgLimit;
    uint16			PQueueID;
} TO_MessageFlow_t;


typedef struct
{
    uint16			DroppedMsgCnt;
    uint16			QueuedMsgCnt;
} TO_MessageFlowMetrics_t;

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
void TO_MessageFlow_ResetCountsAll(TO_ChannelData_t* channel);



/************************************************************************/
/** \brief Teardown all output channel queues.
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
int32 TO_MessageFlow_TeardownAll(TO_ChannelData_t* channel);



/************************************************************************/
/** \brief Buildup all message flows.
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
int32 TO_MessageFlow_Buildup(TO_ChannelData_t* channel);



/************************************************************************/
/** \brief Cleanup all message flows.
**
**  \par Description
**       This function is call at application termination and empties
**       the queue, deallocating each message its popped off the queue.
**
*************************************************************************/
void TO_MessageFlow_CleanupAll(TO_ChannelData_t* channel);



/************************************************************************/
/** \brief Get the next message flow object.
**
**  \par Description
**       This function is used by the classifier to get the next message
**       flow object with the specified message ID.
**
**  \param [in]   MsgPtr        A #CFE_SB_MsgId_t that references the
**                              software bus message.
**
**  \param [in/out] Cursor      Handle to the cursor.  Reset to 0 to
**                              start at the beginning.  Use the previous
**                              value for the next call to get the next
**                              object.
**
**  \returns
**  Pointer to the #TO_MessageFlow_t object when successful.  NULL
**  pointer when unsuccessful.
**  \endreturns
**
*************************************************************************/
TO_MessageFlow_t* TO_MessageFlow_GetObject(TO_ChannelData_t* channel, CFE_SB_MsgId_t MsgID, uint32 *Index);



/************************************************************************/
/** \brief Get priority queue object for the message flow.
**
**  \par Description
**       This function is used by the classifier to get the priority
**       queue object from the message flow object.
**
**  \param [in]   MsgPtr        A #TO_MessageFlow_t pointer that
**                              references the message flow.
**
**  \returns
**  Pointer to the #TO_PriorityQueue_t object when successful.  NULL
**  pointer when unsuccessful.
**  \endreturns
**
*************************************************************************/
TO_PriorityQueue_t* TO_MessageFlow_GetPQueue(TO_ChannelData_t* channel, TO_MessageFlow_t *MsgFlow, uint32 *Index);



/************************************************************************/
/** \brief Add a message flow.
**
**  \par Description
**       This function is called when a ground command is received to add
**       a message flow.  An event will be raised if this function fails.
**
**  \param [in]   MsgPtr        A #CFE_SB_MsgId_t that references the
**                              software bus message.
**
**  \param [in]   MsgLimit      The maximum messages the message flow
**                              queue will contain before overflowing.
**
**  \param [in]   PQueueIdx     Index of the priority queue.
**
**  \returns
**  TRUE if added.  FALSE if not.
**  \endreturns
**
*************************************************************************/
boolean TO_MessageFlow_Add(uint16 ChannelIdx, CFE_SB_MsgId_t MsgID, uint16 MsgLimit, uint16 PQueueIdx);



/************************************************************************/
/** \brief Remove a message flow.
**
**  \par Description
**       This function is called when a ground command is received to
**       remove a message flow.  An event will be raised if this function
**       fails.
**
**  \param [in]   MsgPtr        A #CFE_SB_MsgId_t that references the
**                              software bus message.
**
**  \param [in]   PQueueIdx     Index of the priority queue.
**
**  \returns
**  TRUE if message flow is found and removed.  FALSE if not.
**  \endreturns
**
*************************************************************************/
boolean TO_MessageFlow_Remove(uint16 ChannelIdx, CFE_SB_MsgId_t MsgID);



/************************************************************************/
/** \brief Query a message flow.
**
**  \par Description
**       This function is called when a ground command is received to
**       query a message flow.  An event is raised with the following
**       text format:
**       "MID=<MsgID> ML=<MsgLimit> PQI=<PriorityQueueIndex> D=<DroppedMsgCnt> Q=<QueuedMsgCnt>"
**
**  \param [in]   MsgPtr        A #CFE_SB_MsgId_t that references the
**                              software bus message.
**
**  \param [in]   PQueueIdx     Index of the priority queue.
**
**  \returns
**  TRUE if message flow is found.  FALSE if not found.
**  \endreturns
**
*************************************************************************/
boolean TO_MessageFlow_Query(uint16 ChannelIdx, CFE_SB_MsgId_t MsgID);

#endif
