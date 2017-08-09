#ifndef TO_OUTPUT_CHANNEL_H
#define TO_OUTPUT_CHANNEL_H

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
** \brief Output channel state.  Used to determine if a table entry is used or not.
*/
typedef enum
{
	TO_OUT_CHANNEL_UNUSED = 0,
	TO_OUT_CHANNEL_ENA = 1,
} TO_OutputChannelState_t;

/**
** \brief Definition for a single output channel queue entry.
*/
typedef struct
{
	TO_OutputChannelState_t State;
	uint16					MsgLimit;
	uint16					SentCount;
	uint16					CurrentlyQueuedCnt;
    uint16					HighwaterMark;
    uint32					OSALQueueID;
} TO_TlmOutputChannelQueue_t;

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
int32 TO_OutputChannel_Push(uint32 ChannelIdx, const char* Buffer, uint32 Size);



/************************************************************************/
/** \brief Initialize all output channels.
**
**  \par Description
**       This function is call at initialization to initialize internal
**       variables.
**
*************************************************************************/
int32 TO_OutputChannel_InitAll(void);



/************************************************************************/
/** \brief Cleanup all output channels.
**
**  \par Description
**       This function is call at application termination and empties
**       the queue, deallocating each message its popped off the queue.
**
*************************************************************************/
void  TO_OutputChannel_CleanupAll(void);



/************************************************************************/
/** \brief Resets all metrics.
**
**  \par Description
**       This function is called at when the Reset command is received.
**       It resets all metrics, which include the Sent Counts and the
**       Highwater Marks.
**
*************************************************************************/
void  TO_OutputChannel_ResetCountsAll(void);



/************************************************************************/
/** \brief Teardown all output channel queues.
**
**  \par Description
**       This function is called at when the application is about to load
**       a new configuration table.  This will flush the queues and
**       deallocate the messages contained in the queues.
**
*************************************************************************/
int32 TO_OutputChannel_TeardownAll(void);



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
int32 TO_OutputChannel_BuildupAll(void);



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
int32 TO_OutputChannel_QueueMsg(CFE_SB_MsgPtr_t MsgPtr, TO_TlmOutputChannelQueue_t* OutChannel);



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
boolean TO_OutputChannel_Query(uint16 OutputChannelIdx);

#endif
