
#ifndef TO_MSG_H
#define TO_MSG_H

/************************************************************************
** Pragmas
*************************************************************************/

/************************************************************************
** Includes
*************************************************************************/
#include "cfe.h"

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
    CFE_SB_MsgId_t MsgID;
    uint16 MsgLimit;
    uint16 PQueueIdx;
} TO_AddMessageFlowCmd_t;

typedef struct
{
    uint8  ucCmdHeader[CFE_SB_CMD_HDR_SIZE];
    CFE_SB_MsgId_t MsgID;
    uint16 PQueueIdx;
} TO_RemoveMessageFlowCmd_t;

typedef struct
{
    uint8  ucCmdHeader[CFE_SB_CMD_HDR_SIZE];
    CFE_SB_MsgId_t MsgID;
    uint16 PQueueIdx;
} TO_QueryMessageFlowCmd_t;

typedef struct
{
    uint8  ucCmdHeader[CFE_SB_CMD_HDR_SIZE];
    uint16 PQueueIndex;
} TO_QueryPriorityQueueCmd_t;

typedef struct
{
    uint8  ucCmdHeader[CFE_SB_CMD_HDR_SIZE];
    uint16 OutputChannelIndex;
} TO_QueryOutputChannelCmd_t;

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
    uint8              usTotalMsgDropped;

    /** \totlmmnemonic \TO_NOSERFUNCCNT
        \brief Count of messages with no serialize functions. */
    uint8              usNoSerFuncCnt;

    /** \totlmmnemonic \TO_MEMPOOLHNDL
        \brief Memory pool handle for queued messages. */
    uint32             MemPoolHandle;
  
    /* TODO:  Add declarations for additional housekeeping data here */

} TO_HkTlm_t;


#ifdef __cplusplus
}
#endif

#endif /* TO_MSG_H */

/************************/
/*  End of File Comment */
/************************/
