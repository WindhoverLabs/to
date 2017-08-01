
#ifndef TO_TBLDEFS_H
#define TO_TBLDEFS_H

/************************************************************************
** Pragmas
*************************************************************************/

/************************************************************************
** Includes
*************************************************************************/
#include "cfe.h"
#include "to_mission_cfg.h"
#include "to_platform_cfg.h"

#ifdef __cplusplus
extern "C" {
#endif

/************************************************************************
** Local Defines
*************************************************************************/
/**
 * \brief Defines the number of entries in the table
 */
#define TO_CONFIG_TABLE_MAX_ENTRIES  (1)

/**
 * \brief Defines the table identification name used for table registration.
 */
#define TO_CONFIG_TABLENAME          ("CONFIG_TBL")

/**
 * \brief Defines the table file name used for table registration.
 */
#define TO_CDS_TABLENAME  ("to_CdsTbl")

typedef enum
{
	TO_PQUEUE_UNUSED = 0,
	TO_PQUEUE_ENA = 1,
} TO_PriorityQueueState_t;

typedef enum
{
	TO_OUT_CHANNEL_UNUSED = 0,
	TO_OUT_CHANNEL_ENA = 1,
} TO_OutputChannelState_t;

typedef enum
{
	TO_PRIORITY_QUEUE_TYPE_FIFO = 0,
	TO_PRIORITY_QUEUE_TYPE_SINGLE,
} TO_PriorityQueueType_t;

/************************************************************************
** Local Structure Declarations
*************************************************************************/

/** \brief Definition for a single telemetry message flow entry. */
typedef struct
{
    CFE_SB_MsgId_t  MsgId;
    uint16          MsgLimit;
    uint16			PQueueID;
    uint16          MinSize;
    uint16			DroppedMsgCnt;
    uint16			TotalQueuedMsgCnt;
    uint16			CurrentQueuedMsgCnt;
    uint32          TxCount;
} TO_TlmMessageFlow_t;

/** \brief Definition for a single priority queue entry. */
typedef struct
{
	TO_PriorityQueueState_t State;
	uint16					ChannelID;
	uint16					MsgLimit;
	TO_PriorityQueueType_t  QType;
	uint16					DroppedMsgCnt;
    uint32					OSALQueueID;
} TO_TlmPriorityQueue_t;

/** \brief Definition for a single priority queue entry. */
typedef struct
{
	TO_OutputChannelState_t State;
	uint16					MsgLimit;
	uint16					QueuedCount;
	uint16					DroppedMsgCnt;
    uint32					OSALQueueID;
} TO_TlmOutputChannelQueue_t;


/* TODO:  Add doxygen markup. */
/* Definition for config table entry */
typedef struct
{
	uint8	TableID;

	TO_TlmMessageFlow_t	       MessageFlow[TO_MAX_MESSAGE_FLOWS];
	TO_TlmPriorityQueue_t      PriorityQueue[TO_MAX_PRIORITY_QUEUES];
	TO_TlmOutputChannelQueue_t OutputChannel[TO_MAX_OUTPUT_CHANNELS];
} TO_ConfigTbl_t;

/** \brief Definition for Critical Data Storage (CDS) table entry */
typedef struct
{
    int32  iParam;

    /* TODO:  Add type declaration for CDS data here. */
} TO_CdsTbl_t;

/************************************************************************
** External Global Variables
*************************************************************************/

/************************************************************************
** Global Variables
*************************************************************************/

/************************************************************************
** Local Variables
*************************************************************************/

/************************************************************************
** Local Function Prototypes
*************************************************************************/


#ifdef __cplusplus
}
#endif

#endif /* TO_TBLDEFS_H */

/************************/
/*  End of File Comment */
/************************/
