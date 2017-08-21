#ifndef TO_OUTPUT_QUEUE_H
#define TO_OUTPUT_QUEUE_H

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

/************************************************************************
** Local Structure Definitions
*************************************************************************/

/**
** \brief Definition for a single output channel queue entry.
*/
typedef struct
{
	uint16					SentCount;
	uint16					CurrentlyQueuedCnt;
    uint16					HighwaterMark;
    uint32					OSALQueueID;
} TO_OutputQueue_t;

/************************************************************************
** External Global Variables
*************************************************************************/



/************************************************************************/
/** \brief Push a message onto the output channel queue.
**
**  \par Description
**       This function is called by the Scheduler to push a message onto
**       the output channel for downlink.
**
**  \param [in]   ChannelIdx    The index of the channel to push the
**                              message to.
**
**  \param [in]   Buffer        Buffer containing the message to push.
**
**  \param [in]   Size          Size of the message, in bytes, to push.
**
*************************************************************************/
int32 TO_OutputQueue_Push(TO_ChannelData_t* channel, const char* Buffer, uint32 Size);



/************************************************************************/
/** \brief Cleanup all output channels.
**
**  \par Description
**       This function is call at application termination and empties
**       the queue, deallocating each message its popped off the queue.
**
*************************************************************************/
void  TO_OutputQueue_CleanupAll(TO_ChannelData_t* channel);



/************************************************************************/
/** \brief Resets all metrics.
**
**  \par Description
**       This function is called at when the Reset command is received.
**       It resets all metrics, which include the Sent Counts and the
**       Highwater Marks.
**
*************************************************************************/
void  TO_OutputQueue_ResetCounts(TO_ChannelData_t* channel);



/************************************************************************/
/** \brief Teardown all output channel queues.
**
**  \par Description
**       This function is called at when the application is about to load
**       a new configuration table.  This will flush the queues and
**       deallocate the messages contained in the queues.
**
*************************************************************************/
int32 TO_OutputQueue_Teardown(TO_ChannelData_t* channel);



/************************************************************************/
/** \brief Buildup all output channel queues.
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
int32 TO_OutputQueue_Buildup(TO_ChannelData_t* channel);



/************************************************************************/
/** \brief Queue message.
**
**  \par Description
**       This function is called by the Scheduler to queue a message
**       for downlink.
**
**  \param [in]   MsgPtr        A #CFE_SB_Msg_t pointer that
**                              references the software bus message

**
**  \param [in]   OutChannel    A #TO_TlmOutputChannelQueue_t pointer to
**                              the output channel object.
**
**  \returns
**  0 if successful.  OSAL error if unsuccessful.
**  \endreturns
**
*************************************************************************/
int32 TO_OutputQueue_QueueMsg(TO_ChannelData_t* channel, CFE_SB_MsgPtr_t MsgPtr);



/************************************************************************/
/** \brief Query.
**
**  \par Description
**       This function is called when a ground command is received to
**       query an output channel.  This will raise an information event
**       containing text with Sent Count and Highwater Mark.
**
**  \param [in]   OutputChannelIdx  Index of the output channel to query.
**
**  \returns
**  TRUE if successful.  FALSE if an error occurred.
**  \endreturns
**
*************************************************************************/
boolean TO_OutputChannel_Query(uint16 ChannelIdx);

#endif
