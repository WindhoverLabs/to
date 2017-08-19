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
    char TableName[CFE_TBL_MAX_NAME_LENGTH];

    /** \brief Config Table File Name */
    char TableFileName[OS_MAX_PATH_LEN];

    /** \brief Config Table Handle */
    CFE_TBL_Handle_t  ConfigTblHdl;

    /** \brief Config Table Pointer */
    TO_ChannelTbl_t*  ConfigTblPtr;
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
int32 TO_Channel_OpenChannel(uint32 index, char *TableName, char *TableFileName);


#endif
