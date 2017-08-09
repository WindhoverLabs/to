#ifndef TO_SCHEDULER_H
#define TO_SCHEDULER_H

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

/************************************************************************
** External Global Variables
*************************************************************************/


/************************************************************************/
/** \brief Runs the Classifier algorithm on incoming message flows.
**
**  \par Description
**       This runs fixed scheduler that services the priority queues in
**       the order they appear in the configuration table.  The Scheduler
**       will pop messages off the priority queue when the output channel
**       has room.  It will move on to the next priority queue when the
**       output channel is full or the current priority is empty.
**
**  \par Assumptions, External Events, and Notes:
**       This will raise error events as errors are encountered.
**
*************************************************************************/
void TO_Scheduler_Run(void);

#endif
