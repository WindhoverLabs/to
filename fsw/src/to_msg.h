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

#ifndef TO_MSG_H
#define TO_MSG_H

/************************************************************************
** Pragmas
*************************************************************************/

/************************************************************************
** Includes
*************************************************************************/
#include "cfe.h"
#include "to_channel.h"
#include "to_priority_queue.h"

#ifdef __cplusplus
extern "C" {
#endif

/************************************************************************
** Local Defines
*************************************************************************/

/************************************************************************
** TO Command Codes
*************************************************************************/

/** \tocmd Noop 
**  
**  \par Description
**       Implements the Noop command that demonstrates the TO task is alive
**
**  \tocmdmnemonic \TO_NOOP
**
**  \par Command Structure
**       #TO_NoArgCmd_t
**
**  \par Command Verification
**       Successful execution of this command may be verified with
**       the following telemetry:
**       - \b \c \TO_CMDACPTCNT - command counter will increment
**       - The #TO_CMD_INF_EID informational event message will be 
**         generated when the command is received
** 
**  \par Error Conditions
**       This command may fail for the following reason(s):
**       - Command packet length not as expected
** 
**  \par Evidence of failure may be found in the following telemetry: 
**       - \b \c \TO_CMDRJCTCNT - command error counter will increment
**       - Error specific event message #TO_MSGID_ERR_EID
**
**  \par Criticality
**       None
**
**  \sa #TO_RESET_CC
*/
#define TO_NOOP_CC                 (0)

/** \tocmd Reset Counters
**  
**  \par Description
**       Resets the to housekeeping counters
**
**  \tocmdmnemonic \TO_TLMRST
**
**  \par Command Structure
**       #TO_NoArgCmd_t
**
**  \par Command Verification
**       Successful execution of this command may be verified with
**       the following telemetry:
**       - \b \c \TO_CMDACTPCNT       - command counter will be cleared
**       - \b \c \TO_CMDRJCTCNT       - command error counter will be cleared
**       - The #TO_CMD_INF_EID debug event message will be 
**         generated when the command is executed
** 
**  \par Error Conditions
**       This command may fail for the following reason(s):
**       - Command packet length not as expected
** 
**  \par Evidence of failure may be found in the following telemetry: 
**       - \b \c \TO_CMDRJCTCNT - command error counter will increment
**       - Error specific event message #TO_MSGID_ERR_EID
**
**  \par Criticality
**       None
**
**  \sa #TO_NOOP_CC
*/
#define TO_RESET_CC                (1)


#define TO_ADD_MESSAGE_FLOW_CC     (2)
#define TO_REMOVE_MESSAGE_FLOW_CC  (3)
#define TO_QUERY_MESSAGE_FLOW_CC   (4)
#define TO_QUERY_PRIORITY_QUEUE_CC (5)
#define TO_QUERY_OUTPUT_CHANNEL_CC (6)
#define TO_SEND_DIAG_CC            (7)


/************************************************************************
** Local Structure Declarations
*************************************************************************/

/** 
**  \brief No Arguments Command
**  For command details see #TO_NOOP_CC, #TO_RESET_CC
**  Also see #TO_SEND_HK_MID
*/
typedef struct
{
    uint8  ucCmdHeader[CFE_SB_CMD_HDR_SIZE];
} TO_NoArgCmd_t;

typedef struct
{
    uint8  ucCmdHeader[CFE_SB_CMD_HDR_SIZE];
    uint16 ChannelIdx;
    CFE_SB_MsgId_t MsgID;
    uint16 MsgLimit;
    uint16 PQueueIdx;
} TO_AddMessageFlowCmd_t;

typedef struct
{
    uint8  ucCmdHeader[CFE_SB_CMD_HDR_SIZE];
    uint16 ChannelIdx;
    CFE_SB_MsgId_t MsgID;
} TO_RemoveMessageFlowCmd_t;

typedef struct
{
    uint8  ucCmdHeader[CFE_SB_CMD_HDR_SIZE];
    uint16 ChannelIdx;
    CFE_SB_MsgId_t MsgID;
} TO_QueryMessageFlowCmd_t;

typedef struct
{
    uint8  ucCmdHeader[CFE_SB_CMD_HDR_SIZE];
    uint16 ChannelIdx;
    uint16 PQueueIndex;
} TO_QueryPriorityQueueCmd_t;

typedef struct
{
    uint8  ucCmdHeader[CFE_SB_CMD_HDR_SIZE];
    uint16 ChannelIdx;
} TO_QueryOutputChannelCmd_t;

typedef struct
{
    uint8  ucCmdHeader[CFE_SB_CMD_HDR_SIZE];
    uint16 ChannelIdx;
} TO_SendDiagCmd_t;

/** 
**  \brief TODO Elaborate this struct
**  Boilerplate example of application-specific incoming data
*/
typedef struct
{
    uint8   TlmHeader[CFE_SB_TLM_HDR_SIZE];
    uint32  counter;

    /* TODO:  Add input data to this application here, such as raw data read from I/O
    **        devices.
    **        Option: for data that is already defined by another app, include
    **        that app's message header above.
    */

} TO_InData_t;

/** 
**  \brief TODO Elaborate this struct
**  Boilerplate example of application-specific outgoing data
*/
typedef struct
{
    uint8   ucTlmHeader[CFE_SB_TLM_HDR_SIZE];
    uint32  uiCounter;
} TO_OutData_t;

/** 
**  \brief TO application housekeeping data
*/
typedef struct
{
    /** \brief cFE SB Tlm Msg Hdr */
    uint8              TlmHeader[CFE_SB_TLM_HDR_SIZE];

    /** \totlmmnemonic \TO_CMDACPTCNT
        \brief Count of accepted commands */
    uint8              usCmdCnt;   

    /** \totlmmnemonic \TO_CMDRJCTCNT
        \brief Count of failed commands */
    uint8              usCmdErrCnt; 

    /** \totlmmnemonic \TO_TTLMSGDROP
        \brief Count of all messages dropped */
    uint16             usTotalMsgDropped;

    /** \totlmmnemonic \TO_MEMPOOLHNDL
        \brief Memory pool handle for queued messages. */
    uint32             MemPoolHandle;

    uint32 			   MemInUse;
    uint32 			   PeakMemInUse;
    uint32             MaxMem;
    uint16             QueuedInOutputChannel[TO_MAX_CHANNELS];
  
    /* TODO:  Add declarations for additional housekeeping data here */

} TO_HkTlm_t;


typedef struct
{
    CFE_SB_MsgId_t  MsgId;
    uint16          MsgLimit;
    uint16			PQueueID;
    uint16			DroppedMsgCnt;
    uint16			QueuedMsgCnt;
} TO_MessageFlowDiagTlm_t;


typedef struct
{
    TO_PriorityQueueState_t State;
	uint16 				    MsgLimit;
	TO_PriorityQueueType_t  QType;
	uint16					DroppedMsgCnt;
	uint16					QueuedMsgCnt;
	uint16					CurrentlyQueuedCnt;
	uint16					HighwaterMark;
} TO_PriorityDiagTlm_t;


typedef struct
{
	uint16					SentCount;
	uint16					CurrentlyQueuedCnt;
    uint16					HighwaterMark;
} TO_OutputQueueDiagTlm_t;


typedef struct
{
    uint8                   ucTlmHeader[CFE_SB_TLM_HDR_SIZE];
	uint32				    Index;
	TO_ChannelState_t       State;
    char  	                ConfigTableName[CFE_TBL_MAX_NAME_LENGTH];
    char  	                ConfigTableFileName[OS_MAX_PATH_LEN];
    char  	                DumpTableName[OS_MAX_API_NAME];
    char  	                ChannelName[OS_MAX_API_NAME];
	uint8 	                TableID;
	TO_MessageFlowDiagTlm_t MessageFlow[TO_MAX_MESSAGE_FLOWS];
	TO_PriorityDiagTlm_t    PQueue[TO_MAX_PRIORITY_QUEUES];
	TO_OutputQueueDiagTlm_t OQueue;
} TO_ChannelDiagTlm_t;



#ifdef __cplusplus
}
#endif

#endif /* TO_MSG_H */

/************************/
/*  End of File Comment */
/************************/
