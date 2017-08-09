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
    uint16          MinSize;
    uint16			DroppedMsgCnt;
    uint16			QueuedMsgCnt;
} TO_TlmMessageFlow_t;

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
void TO_MessageFlow_ResetCountsAll(void);



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
int32 TO_MessageFlow_TeardownAll(void);



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
int32 TO_MessageFlow_BuildupAll(void);



/************************************************************************/
/** \brief Cleanup all message flows.
**
**  \par Description
**       This function is call at application termination and empties
**       the queue, deallocating each message its popped off the queue.
**
*************************************************************************/
void TO_MessageFlow_CleanupAll(void);



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
**  Pointer to the #TO_TlmMessageFlow_t object when successful.  NULL
**  pointer when unsuccessful.
**  \endreturns
**
*************************************************************************/
TO_TlmMessageFlow_t* TO_MessageFlow_GetNextObject(CFE_SB_MsgId_t MsgID, uint32 *Cursor);



/************************************************************************/
/** \brief Get the specified message flow object.
**
**  \par Description
**       This function is used by the classifier to get a specific
**       message flow object with the specified message ID.
**
**  \param [in]   MsgPtr        A #CFE_SB_MsgId_t that references the
**                              software bus message.
**
**  \param [in]   PQueueIdx     The index of the priority queue.
**
**  \returns
**  Pointer to the #TO_TlmMessageFlow_t object when successful.  NULL
**  pointer when unsuccessful.
**  \endreturns
**
*************************************************************************/
TO_TlmMessageFlow_t* TO_MessageFlow_GetObject(CFE_SB_MsgId_t MsgID, uint16 PQueueIdx);



/************************************************************************/
/** \brief Get priority queue object for the message flow.
**
**  \par Description
**       This function is used by the classifier to get the priority
**       queue object from the message flow object.
**
**  \param [in]   MsgPtr        A #TO_TlmMessageFlow_t pointer that
**                              references the message flow.
**
**  \returns
**  Pointer to the #TO_TlmPriorityQueue_t object when successful.  NULL
**  pointer when unsuccessful.
**  \endreturns
**
*************************************************************************/
TO_TlmPriorityQueue_t* TO_MessageFlow_GetPQueue(TO_TlmMessageFlow_t *MsgFlow);



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
boolean TO_MessageFlow_Add(CFE_SB_MsgId_t MsgID, uint16 MsgLimit, uint16 PQueueIdx);



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
boolean TO_MessageFlow_Remove(CFE_SB_MsgId_t MsgID, uint16 PQueueIdx);



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
boolean TO_MessageFlow_Query(CFE_SB_MsgId_t MsgID, uint16 PQueueIdx);

#endif
