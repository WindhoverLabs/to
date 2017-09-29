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
* 3. Neither the name Windhover Labs, L.L.C. nor the names of its 
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

/************************************************************************
** Pragmas
*************************************************************************/

/************************************************************************
** Includes
*************************************************************************/
#include "to_config_utils.h"
#include "to_channel.h"
#include "to_output_queue.h"
#include <string.h>

/************************************************************************
** Local Defines
*************************************************************************/

/************************************************************************
** Local Structure Declarations
*************************************************************************/

/************************************************************************
** External Global Variables
*************************************************************************/
extern TO_AppData_t  TO_AppData;

/************************************************************************
** Global Variables
*************************************************************************/

/************************************************************************
** Local Variables
*************************************************************************/

/************************************************************************
** Function Prototypes
*************************************************************************/

/************************************************************************
** Function Definitions
*************************************************************************/


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Initialize Config Table                                         */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int32 TO_InitConfigTbl(TO_ChannelData_t *channel)
{
    int32  iStatus=0;

    /* Register Config table */
    iStatus = CFE_TBL_Register(&channel->ConfigTblHdl,
    						   channel->ConfigTableName,
                               sizeof(TO_ChannelTbl_t),
                               CFE_TBL_OPT_DEFAULT,
                               TO_ValidateConfigTbl);
    if (iStatus != CFE_SUCCESS)
    {
        /* Note, a critical table could return another nominal code.  If this table is
         * made critical this logic would have to change. */
        (void) CFE_EVS_SendEvent(TO_INIT_ERR_EID, CFE_EVS_ERROR,
                                 "Failed to register Config table (0x%08X)",
                                 (unsigned int)iStatus);
        goto TO_InitConfigTbl_Exit_Tag;
    }

    /* Register Dump table */
    iStatus = CFE_TBL_Register(&channel->DumpTblHdl,
    						   channel->DumpTableName,
                               sizeof(TO_ChannelDumpTbl_t),
							   CFE_TBL_OPT_USR_DEF_ADDR,
                               0);
    if (iStatus != CFE_SUCCESS)
    {
        /* Note, a critical table could return another nominal code.  If this table is
         * made critical this logic would have to change. */
        (void) CFE_EVS_SendEvent(TO_INIT_ERR_EID, CFE_EVS_ERROR,
                                 "Failed to register Config table (0x%08X)",
                                 (unsigned int)iStatus);
        goto TO_InitConfigTbl_Exit_Tag;
    }

    /* Load Config table file */
    iStatus = CFE_TBL_Load(channel->ConfigTblHdl,
                           CFE_TBL_SRC_FILE,
						   channel->ConfigTableFileName);
    if (iStatus != CFE_SUCCESS)
    {
        /* Note, CFE_SUCCESS is for a successful full table load.  If a partial table
           load is desired then this logic would have to change. */
        (void) CFE_EVS_SendEvent(TO_INIT_ERR_EID, CFE_EVS_ERROR,
                                 "Failed to load Config Table (0x%08X)",
                                 (unsigned int)iStatus);
        goto TO_InitConfigTbl_Exit_Tag;
    }

    /* Load Dump table */
    iStatus = CFE_TBL_Load(channel->DumpTblHdl,
    		               CFE_TBL_SRC_ADDRESS,
						   &channel->DumpTbl);
    if (iStatus != CFE_SUCCESS)
    {
        /* Note, CFE_SUCCESS is for a successful full table load.  If a partial table
           load is desired then this logic would have to change. */
        (void) CFE_EVS_SendEvent(TO_INIT_ERR_EID, CFE_EVS_ERROR,
                                 "Failed to load Dump Table (0x%08X)",
                                 (unsigned int)iStatus);
        goto TO_InitConfigTbl_Exit_Tag;
    }

    TO_AcquireAllTables();

TO_InitConfigTbl_Exit_Tag:
    return (iStatus);
}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Validate Config Table                                           */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int32 TO_ValidateConfigTbl(void* ConfigTblPtr)
{
    int32  iStatus=0;
    TO_ChannelTbl_t* TO_ConfigTblPtr = (TO_ChannelTbl_t*)(ConfigTblPtr);

    if (ConfigTblPtr == NULL)
    {
        iStatus = -1;
        goto TO_ValidateConfigTbl_Exit_Tag;
    }

    /* TODO:  Add code to validate new data values here.
    **
    ** Examples:
    ** if (TO_ConfigTblPtr->iParam <= 16) {
    **   (void) CFE_EVS_SendEvent(TO_CONFIG_TABLE_INF_EID, CFE_EVS_ERROR,
     *                         "Invalid value for Config parameter sParam (%d)",
    **                         TO_ConfigTblPtr->iParam);
    ** }
    **/

TO_ValidateConfigTbl_Exit_Tag:
    return (iStatus);
}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Process New Config Table                                        */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int32 TO_ProcessNewConfigTbl(TO_ChannelData_t* channel)
{
	uint32 i = 0;
	int32 iStatus = 0;

	/* Build up the new configuration. */
	iStatus = TO_MessageFlow_Buildup(channel);
	if(iStatus != 0)
	{
		goto end_of_function;
	}
	iStatus = TO_PriorityQueue_BuildupAll(channel);
	if(iStatus != 0)
	{
		goto end_of_function;
	}

end_of_function:
    return iStatus;
}



/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Release all tables                                              */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void TO_ReleaseAllTables()
{
    int32 iStatus = CFE_SUCCESS;
    uint32 i = 0;

    for(i = 0; i < TO_MAX_CHANNELS; ++i)
    {
    	if(TO_AppData.ChannelData[i].State == TO_CHANNEL_OPENED)
    	{
			CFE_TBL_ReleaseAddress(TO_AppData.ChannelData[i].ConfigTblHdl);
    	}
    }
}



/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Release all tables                                              */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void TO_AcquireAllTables()
{
    int32 iStatus = CFE_SUCCESS;
    uint32 i = 0;

    for(i = 0; i < TO_MAX_CHANNELS; ++i)
    {
		TO_Channel_LockByIndex(i);

    	if(TO_AppData.ChannelData[i].State == TO_CHANNEL_OPENED)
    	{
    		iStatus = CFE_TBL_GetStatus(TO_AppData.ChannelData[i].ConfigTblHdl);
			if ((iStatus & CFE_SEVERITY_BITMASK) == CFE_SEVERITY_ERROR)
			{
				(void) CFE_EVS_SendEvent(TO_CONFIG_TABLE_ERR_EID, CFE_EVS_ERROR,
										 "Failed to get config table status (0x%08X)",
										 (unsigned int)iStatus);
			}
			else if(iStatus == CFE_TBL_INFO_UPDATE_PENDING)
			{
				TO_MessageFlow_TeardownAll(&TO_AppData.ChannelData[i]);
				TO_PriorityQueue_TeardownAll(&TO_AppData.ChannelData[i]);
			}

			/*
			** Manage the tables
			*/
			CFE_TBL_Manage(TO_AppData.ChannelData[i].DumpTblHdl);
			iStatus = CFE_TBL_Manage(TO_AppData.ChannelData[i].ConfigTblHdl);
			if ((iStatus != CFE_SUCCESS) && (iStatus != CFE_TBL_INFO_UPDATED))
			{
				CFE_EVS_SendEvent(TO_CONFIG_TABLE_ERR_EID, CFE_EVS_ERROR,
										 "Failed to manage Config table (0x%08X)",
										 (unsigned int)iStatus);
			}
			else
			{
				/*
				** Get a pointer to the table
				*/
				iStatus = CFE_TBL_GetAddress(
						(void*)&TO_AppData.ChannelData[i].ConfigTblPtr,
						TO_AppData.ChannelData[i].ConfigTblHdl);
				if (iStatus == CFE_TBL_INFO_UPDATED)
				{
					TO_ProcessNewConfigTbl(&TO_AppData.ChannelData[i]);
					iStatus = CFE_SUCCESS;
				}
				else if(iStatus != CFE_SUCCESS)
				{
					TO_AppData.ChannelData[i].ConfigTblPtr = 0;
					CFE_EVS_SendEvent(TO_CONFIG_TABLE_ERR_EID, CFE_EVS_ERROR,
											 "Failed to get Config table's address (0x%08X)",
											 (unsigned int)iStatus);
				}
			}
    	}

		TO_Channel_UnlockByIndex(i);
    }
}



/************************/
/*  End of File Comment */
/************************/
    
