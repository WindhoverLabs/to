#ifndef TO_OUTPUT_CHANNEL_H
#define TO_OUTPUT_CHANNEL_H

#include "cfe.h"

typedef enum
{
	TO_OUT_CHANNEL_UNUSED = 0,
	TO_OUT_CHANNEL_ENA = 1,
} TO_OutputChannelState_t;


/** \brief Definition for a single priority queue entry. */
typedef struct
{
	TO_OutputChannelState_t State;
	uint16					MsgLimit;
	uint16					SentCount;
	uint16					CurrentlyQueuedCnt;
    uint16					HighwaterMark;
    uint32					OSALQueueID;
} TO_TlmOutputChannelQueue_t;

int32 TO_OutputChannel_Push(uint32 ChannelID, const char* Buffer, uint32 Size);
int32 TO_OutputChannel_InitAll(void);
void  TO_OutputChannel_CleanupAll(void);
void  TO_OutputChannel_ProcessNewCustomCmds(CFE_SB_Msg_t* MsgPtr);
void  TO_OutputChannel_ResetCountsAll(void);
int32 TO_OutputChannel_TeardownAll(void);
int32 TO_OutputChannel_BuildupAll(void);
int32 TO_OutputChannel_QueueMsg(CFE_SB_MsgPtr_t MsgPtr, TO_TlmOutputChannelQueue_t* OutChannel);
boolean TO_OutputChannel_Query(uint16 OutputChannelIdx);

#endif
