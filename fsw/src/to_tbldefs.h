
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

/**
 * \brief Defines the table identification name used for table registration.
 */
#define TO_CONFIG_TABLENAME          ("CONFIG_TBL")

/**
 * \brief Defines the table file name used for table registration.
 */
#define TO_CDS_TABLENAME  ("to_CdsTbl")


/************************************************************************
** Local Structure Declarations
*************************************************************************/


/* TODO:  Add doxygen markup. */
/* Definition for config table entry */
typedef struct
{
	uint8	TableID;

	TO_TlmMessageFlow_t	       MessageFlow[TO_MAX_MESSAGE_FLOWS];
	TO_TlmPriorityQueue_t      PriorityQueue[TO_MAX_PRIORITY_QUEUES];
	TO_TlmOutputQueue_t        OutputChannel;
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
