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
