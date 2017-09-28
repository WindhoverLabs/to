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
#include "to_message_flow.h"
#include "to_priority_queue.h"
#include "to_output_queue.h"

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


/************************************************************************
** Local Structure Declarations
*************************************************************************/


/* TODO:  Add doxygen markup. */
/* Definition for config table entry */
typedef struct
{
	uint16	TableID;

	TO_MessageFlow_t	    MessageFlow[TO_MAX_MESSAGE_FLOWS];
	TO_PriorityQueue_t      PriorityQueue[TO_MAX_PRIORITY_QUEUES];
} TO_ChannelTbl_t;

typedef struct
{
	uint16	TableID;

	TO_MessageFlowMetrics_t	  MessageFlow[TO_MAX_MESSAGE_FLOWS];
	TO_PriorityQueueMetrics_t PriorityQueue[TO_MAX_PRIORITY_QUEUES];
	TO_OutputQueue_t    	  OutputQueue;
} TO_ChannelDumpTbl_t;

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
