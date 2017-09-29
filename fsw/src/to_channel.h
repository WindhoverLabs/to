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

#ifndef TO_CHANNEL_H
#define TO_CHANNEL_H

/************************************************************************
** Pragmas
*************************************************************************/

/************************************************************************
** Includes
*************************************************************************/

#include "cfe.h"
#include "to_tbldefs.h"

typedef struct TO_ChannelData_t TO_ChannelData_t;

typedef enum
{
	TO_CHANNEL_CLOSED = 0,
	TO_CHANNEL_OPENED = 1,
	TO_CHANNEL_RECONFIGURING = 2,
} TO_ChannelState_t;

struct TO_ChannelData_t
{
	TO_ChannelState_t State;

    /* Config table-related */

    /** \brief Config Table Name */
    char ConfigTableName[CFE_TBL_MAX_NAME_LENGTH];

    /** \brief Config Table File Name */
    char ConfigTableFileName[OS_MAX_PATH_LEN];

    /** \brief Dump Table Name */
    char DumpTableName[OS_MAX_PATH_LEN];

    char ChannelName[OS_MAX_API_NAME];

    /** \brief Config Table Handle */
    CFE_TBL_Handle_t  ConfigTblHdl;

    /** \brief Config Table Pointer */
    TO_ChannelTbl_t*  ConfigTblPtr;

    /** \brief Dump Table Handle */
    CFE_TBL_Handle_t  DumpTblHdl;

    /** \brief Dump Table Pointer */
    TO_ChannelDumpTbl_t  DumpTbl;

    /** \brief Data Pipe ID */
    CFE_SB_PipeId_t  DataPipeId;

    uint32 MutexID;

	TO_OutputQueue_t OutputQueue;
};


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
/** \brief Process all the channels.
**
**  \par Description
**       TODO
**
**  \par Assumptions, External Events, and Notes:
**       TODO
**
*************************************************************************/
int32 TO_Channel_OpenChannel(uint32 index, char *ChannelName,
		char *ConfigTableName, char *ConfigTableFileName,
		char *DumpTableName);

int32 TO_Channel_ProcessTelemetryAll(void);
int32 TO_Channel_ProcessTelemetry(TO_ChannelData_t *channel);

int32 TO_Channel_ResetCountsAll(void);
int32 TO_Channel_ResetCounts(TO_ChannelData_t *channel);

void  TO_Channel_LockByIndex(uint32 index);
void  TO_Channel_UnlockByIndex(uint32 index);
void  TO_Channel_LockByRef(TO_ChannelData_t *channel);
void  TO_Channel_UnlockByRef(TO_ChannelData_t *channel);

int32 TO_Channel_InitAll(void);
int32 TO_Channel_Init(uint32 index);
void TO_Channel_CleanupAll(void);
void TO_Channel_Cleanup(uint32 index);

uint8 TO_Channel_State(uint32 index);


#endif
