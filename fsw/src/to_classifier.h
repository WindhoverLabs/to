#include "cfe.h"
#include "to_tbldefs.h"


/* TODO:  Add Doxygen markup. */
void TO_RunClassifier(void);
TO_TlmMessageFlow_t* TO_GetMessageFlowObject(CFE_SB_MsgId_t MsgID);
TO_TlmPriorityQueue_t* TO_GetPQueueForMessageFlow(TO_TlmMessageFlow_t *MsgFlow);
int32 TO_QueueMessageInPqueue(CFE_SB_MsgPtr_t MsgPtr, TO_TlmPriorityQueue_t* PQueue);
