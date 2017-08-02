#ifndef TO_CLASSIFIER_H
#define TO_CLASSIFIER_H

#include "cfe.h"
#include "to_message_flow.h"
#include "to_priority_queue.h"


/* TODO:  Add Doxygen markup. */
void TO_Classifier_Run(void);
TO_TlmMessageFlow_t* TO_Classifier_GetMsgFlowObject(CFE_SB_MsgId_t MsgID);
TO_TlmPriorityQueue_t* TO_Classifier_GetPQueueForMessageFlow(TO_TlmMessageFlow_t *MsgFlow);
void TO_Classifier_ResetCounts(void);

void TO_Classifier_Teardown(void);

#endif
