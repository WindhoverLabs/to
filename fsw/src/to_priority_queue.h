#ifndef TO_PRIORITY_QUEUE_H
#define TO_PRIORITY_QUEUE_H

#include "cfe.h"

typedef enum
{
	TO_PQUEUE_UNUSED = 0,
	TO_PQUEUE_ENA = 1,
} TO_PriorityQueueState_t;

typedef enum
{
	TO_PRIORITY_QUEUE_TYPE_FIFO = 0,
	TO_PRIORITY_QUEUE_TYPE_SINGLE,
} TO_PriorityQueueType_t;


/** \brief Definition for a single priority queue entry. */
typedef struct
{
	TO_PriorityQueueState_t State;
	uint16					ChannelID;
	uint16					MsgLimit;
	TO_PriorityQueueType_t  QType;
	uint16					DroppedMsgCnt;
    uint16					QueuedMsgCnt;
    uint16					CurrentlyQueuedCnt;
    uint16					HighwaterMark;
    uint32					OSALQueueID;
} TO_TlmPriorityQueue_t;

void TO_PriorityQueue_ResetCountsAll(void);
void TO_PriorityQueue_CleanupAll(void );
int32 TO_PriorityQueue_BuildupAll(void);
int32 TO_PriorityQueue_QueueMsg(CFE_SB_MsgPtr_t MsgPtr, TO_TlmPriorityQueue_t* PQueue);
int32 TO_PriorityQueue_TeardownAll(void);
boolean TO_PriorityQueue_Query(uint16 PQueueIdx);


#endif
