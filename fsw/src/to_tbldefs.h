/****************************************************************************
*
*   Copyright (c) 2017 Windhover Labs, L.L.C. All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions
* are met:
*
* 1. Redistributions of source code must retain the above copyright
*    notice, this list of conditions and the following disclaimer.
* 2. Redistributions in binary form must reproduce the above copyright
*    notice, this list of conditions and the following disclaimer in
*    the documentation and/or other materials provided with the
*    distribution.
* 3. Neither the name Windhover Labs nor the names of its 
*    contributors may be used to endorse or promote products derived 
*    from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
* "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
* LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
* FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
* COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
* INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
* BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
* OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
* AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
* LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
* ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
* POSSIBILITY OF SUCH DAMAGE.
*
*****************************************************************************/

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


typedef enum
{
	TO_OUTPUT_TYPE_BINARY	= 0,
	TO_OUTPUT_TYPE_PROTOBUF	= 1
} TO_ChannelType_t;

/* TODO:  Add doxygen markup. */
/* Definition for config table entry */
typedef struct
{
	uint16					TableID;
	TO_ChannelType_t		ChannelType;
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
