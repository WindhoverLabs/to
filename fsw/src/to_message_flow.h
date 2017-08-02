#ifndef TO_MESSAGE_FLOW_H
#define TO_MESSAGE_FLOW_H

#include "cfe.h"
#include "to_priority_queue.h"

/** \brief Definition for a single telemetry message flow entry. */
typedef struct
{
    CFE_SB_MsgId_t  MsgId;
    uint16          MsgLimit;
    uint16			PQueueID;
    uint16          MinSize;
    uint16			DroppedMsgCnt;
    uint16			QueuedMsgCnt;
} TO_TlmMessageFlow_t;


void TO_MessageFlow_ResetCountsAll(void);
int32 TO_MessageFlow_TeardownAll(void);
int32 TO_MessageFlow_BuildupAll(void);
void TO_MessageFlow_CleanupAll(void);
TO_TlmMessageFlow_t* TO_MessageFlow_GetObject(CFE_SB_MsgId_t MsgID);
TO_TlmPriorityQueue_t* TO_MessageFlow_GetPQueue(TO_TlmMessageFlow_t *MsgFlow);
boolean TO_MessageFlow_Query(CFE_SB_MsgId_t MsgID);

#endif
