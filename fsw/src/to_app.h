    
#ifndef TO_APP_H
#define TO_APP_H

/************************************************************************
** Pragmas
*************************************************************************/

/************************************************************************
** Includes
*************************************************************************/
#include "cfe.h"

#include "to_platform_cfg.h"
#include "to_mission_cfg.h"
#include "to_private_ids.h"
#include "to_private_types.h"
#include "to_perfids.h"
#include "to_msgids.h"
#include "to_msg.h"
#include "to_events.h"
#include "to_config_utils.h"
#include "to_tbldefs.h"
#include "to_message_flow.h"
#include "to_classifier.h"
#include "to_priority_queue.h"
#include "to_scheduler.h"
#include "to_output_queue.h"
#include "to_custom.h"
#include "to_channel.h"

#ifdef __cplusplus
extern "C" {
#endif

/************************************************************************
** Local Defines
*************************************************************************/


/************************************************************************
** Local Structure Definitions
*************************************************************************/
/**
**  \brief TO Operational Data Structure
*/
typedef struct
{
    /* Copy of the last transmitted message. */
    /* TODO:  Add Doxygen markup */
    uint32 BufferOut[CFE_SB_MAX_SB_MSG_SIZE/4];
    uint16 OutMessageSize;

    /** \brief CFE Event Table */
    CFE_EVS_BinFilter_t  EventTbl[TO_EVT_CNT];

    /**\brief Scheduling Pipe ID */
    CFE_SB_PipeId_t  SchPipeId;

    /** \brief Command Pipe ID */
    CFE_SB_PipeId_t  CmdPipeId;

    /* Task-related */

    /** \brief Task Run Status */
    uint32  uiRunStatus;

    /* Inputs/Outputs */

    /** \brief Input Data from I/O or other apps */
    TO_InData_t   InData;

    /** \brief Output Data published at the end of cycle */
    TO_OutData_t  OutData;

    /** \brief Housekeeping Telemetry for downlink */
    TO_HkTlm_t  HkTlm;

    /** \brief Memory pool buffer for queued messages. */
    uint8		MemPoolBuffer [TO_NUM_BYTES_IN_MEM_POOL];/**< \brief HK mempool buffer */

    TO_ChannelData_t ChannelData[TO_MAX_CHANNELS];

    uint32 MutexID;

    /* TODO:  Add declarations for additional private data here */
} TO_AppData_t;

/************************************************************************
** External Global Variables
*************************************************************************/
extern TO_AppData_t TO_AppData;

/************************************************************************
** Global Variables
*************************************************************************/

/************************************************************************
** Local Variables
*************************************************************************/

/************************************************************************
** Local Function Prototypes
*************************************************************************/

/************************************************************************/
/** \brief CFS Telemetry Output Task (TO) application entry point
**
**  \par Description
**       CFS Telemetry Output Task application entry point.  This function
**       performs app initialization, then waits for the cFE ES Startup
**       Sync, then executes the RPR main processing loop.
**
**  \par Assumptions, External Events, and Notes:
**       If there is an unrecoverable failure during initialization the
**       main loop is never executed and the application will exit.
**
*************************************************************************/
void  TO_AppMain(void);

/************************************************************************/
/** \brief Initialize the CFS Telemetry Output (TO) application
**
**  \par Description
**       Telemetry Output application initialization routine. This
**       function performs all the required startup steps to
**       initialize (or restore from CDS) TO data structures and get
**       the application registered with the cFE services so it can
**       begin to receive command messages and send events.
**
**  \par Assumptions, External Events, and Notes:
**       None
**
**  \returns
**  \retcode #CFE_SUCCESS  \retdesc \copydoc CFE_SUCCESS    \endcode
**  \retstmt Return codes from #CFE_ES_RegisterApp          \endcode
**  \retstmt Return codes from #TO_InitEvent               \endcode
**  \retstmt Return codes from #TO_InitPipe                \endcode
**  \retstmt Return codes from #TO_InitData                \endcode
**  \retstmt Return codes from #TO_InitConfigTbl           \endcode
**  \retstmt Return codes from #TO_InitCdsTbl              \endcode
**  \retstmt Return codes from #OS_TaskInstallDeleteHandler \endcode
**  \endreturns
**
*************************************************************************/
int32  TO_InitApp(void);

/************************************************************************/
/** \brief Initialize Event Services and Event tables
**
**  \par Description
**       This function performs the steps required to setup
**       cFE Event Services for use by the TO application.
**
**  \par Assumptions, External Events, and Notes:
**       None
**
**  \returns
**  \retcode #CFE_SUCCESS  \retdesc \copydoc CFE_SUCCESS \endcode
**  \retstmt Return codes from #CFE_EVS_Register  \endcode
**  \endreturns
**
*************************************************************************/
int32  TO_InitEvent(void);

/************************************************************************/
/** \brief Initialize global variables used by TO application
**
**  \par Description
**       This function performs the steps required to initialize
**       the TO application data.
**
**  \par Assumptions, External Events, and Notes:
**       None
**
**  \returns
**  \retcode #CFE_SUCCESS  \retdesc \copydoc CFE_SUCCESS \endcode
**  \retstmt Return codes from #CFE_EVS_Register  \endcode
**  \endreturns
**
*************************************************************************/
void  TO_InitData(void);

/************************************************************************/
/** \brief Initialize message pipes
**
**  \par Description
**       This function performs the steps required to setup
**       initialize the cFE Software Bus message pipes and subscribe to
**       messages for the TO application.
**
**  \par Assumptions, External Events, and Notes:
**       None
**
**  \returns
**  \retcode #CFE_SUCCESS  \retdesc \copydoc CFE_SUCCESS \endcode
**  \retstmt Return codes from #CFE_SB_CreatePipe        \endcode
**  \retstmt Return codes from #CFE_SB_SubscribeEx       \endcode
**  \retstmt Return codes from #CFE_SB_Subscribe         \endcode
**  \endreturns
**
*************************************************************************/
int32  TO_InitPipe(void);

/************************************************************************/
/** \brief Telemetry Output Task (TO) cleanup prior to exit
**
**  \par Description
**       This function handles any necessary cleanup prior
**       to application exit.
**
**  \par Assumptions, External Events, and Notes:
**       None
**
*************************************************************************/
void  TO_CleanupCallback(void);

/************************************************************************/
/** \brief Receive and process messages
**
**  \par Description
**       This function receives and processes messages
**       for the TO application
**
**  \par Assumptions, External Events, and Notes:
**       None
**
**  \param [in]   iBlocking    A #CFE_SB_PEND_FOREVER, #CFE_SB_POLL or
**                             millisecond timeout
**
**  \returns
**  \retcode #CFE_SUCCESS  \retdesc \copydoc CFE_SUCCESS \endcode
**  \retstmt Return codes from #CFE_SB_RcvMsg            \endcode
**  \endreturns
**
*************************************************************************/
int32  TO_RcvMsg(int32 iBlocking);


/************************************************************************/
/** \brief Telemetry Output Task incoming data processing
**
**  \par Description
**       This function processes incoming data subscribed
**       by TO application
**
**  \par Assumptions, External Events, and Notes:
**       None
**
*************************************************************************/
void  TO_ProcessNewData(void);

/************************************************************************/
/** \brief Telemetry Output Task incoming command processing
**
**  \par Description
**       This function processes incoming commands subscribed
**       by TO application
**
**  \par Assumptions, External Events, and Notes:
**       None
**
*************************************************************************/
void  TO_ProcessNewCmds(void);

/************************************************************************/
/** \brief Telemetry Output Task application commands
**
**  \par Description
**       This function processes command messages
**       specific to the TO application
**
**  \par Assumptions, External Events, and Notes:
**       None
**
**  \param [in]   MsgPtr       A #CFE_SB_Msg_t pointer that
**                             references the software bus message
**
*************************************************************************/
void  TO_ProcessNewAppCmds(CFE_SB_Msg_t* MsgPtr);


/************************************************************************/
/** \brief Sends TO housekeeping message
**
**  \par Description
**       This function sends the housekeeping message
**
**  \par Assumptions, External Events, and Notes:
**       None
**
*************************************************************************/
void  TO_ReportHousekeeping(void);

/************************************************************************/
/** \brief Sends TO output data
**
**  \par Description
**       This function publishes the TO application output data.
**
**  \par Assumptions, External Events, and Notes:
**       None
**
*************************************************************************/
void  TO_SendOutData(void);

/************************************************************************/
/** \brief Verify Command Length
**
**  \par Description
**       This function verifies the command message length.
**
**  \par Assumptions, External Events, and Notes:
**       None
**
**  \param [in]   MsgPtr        A #CFE_SB_Msg_t pointer that
**                              references the software bus message
**  \param [in]   usExpectedLen The expected length of the message
**
**  \returns
**  TRUE if the message length matches expectations, FALSE if it does not.
**  \endreturns
**
*************************************************************************/
boolean  TO_VerifyCmdLength(CFE_SB_Msg_t* MsgPtr, uint16 usExpectedLen);

boolean  TO_SendDiag(uint16 ChannelIdx);

#ifdef __cplusplus
}
#endif 

#endif /* TO_APP_H */

/************************/
/*  End of File Comment */
/************************/
