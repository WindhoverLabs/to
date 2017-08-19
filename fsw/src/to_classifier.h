#ifndef TO_CLASSIFIER_H
#define TO_CLASSIFIER_H

/************************************************************************
** Pragmas
*************************************************************************/

/************************************************************************
** Includes
*************************************************************************/

#include "cfe.h"
#include "to_message_flow.h"
#include "to_priority_queue.h"

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
**       This runs a priority based classification algorithm that pops
**       messages from incoming message flows, and copies the messages
**       to the priority queue specified in the configuration table.
**
**  \par Assumptions, External Events, and Notes:
**       This will raise error events as errors are encountered.
**
*************************************************************************/
void TO_Classifier_Run(TO_ChannelData_t* channel);

#endif
