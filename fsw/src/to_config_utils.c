
/************************************************************************
** Pragmas
*************************************************************************/

/************************************************************************
** Includes
*************************************************************************/
#include "to_config_utils.h"
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

int32 TO_InitConfigTbl()
{
    int32  iStatus=0;

    /* Register Config table */
    iStatus = CFE_TBL_Register(&TO_AppData.ConfigTblHdl,
                               TO_CONFIG_TABLENAME,
                               (sizeof(TO_ConfigTbl_t) * TO_CONFIG_TABLE_MAX_ENTRIES),
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

    /* Load Config table file */
    iStatus = CFE_TBL_Load(TO_AppData.ConfigTblHdl,
                           CFE_TBL_SRC_FILE,
                           TO_CONFIG_TABLE_FILENAME);
    if (iStatus != CFE_SUCCESS)
    {
        /* Note, CFE_SUCCESS is for a successful full table load.  If a partial table
           load is desired then this logic would have to change. */
        (void) CFE_EVS_SendEvent(TO_INIT_ERR_EID, CFE_EVS_ERROR,
                                 "Failed to load Config Table (0x%08X)",
                                 (unsigned int)iStatus);
        goto TO_InitConfigTbl_Exit_Tag;
    }

    iStatus = TO_AcquireConfigPointers();

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
    TO_ConfigTbl_t* TO_ConfigTblPtr = (TO_ConfigTbl_t*)(ConfigTblPtr);

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

int32 TO_ProcessNewConfigTbl()
{
	uint32 i = 0;
	int32 iStatus = 0;

    /* We need to release all the resources and return the system back to its
     * initialization state. First, clear all the message flows by unsubscribing from all
	 * messages.
	 */
	for(i = 0; i < TO_MAX_MESSAGE_FLOWS; ++i)
	{
		if(TO_AppData.Config.MessageFlow[i].MsgId != 0)
		{
			/* Unsubscribe from message. */
			iStatus =  CFE_SB_Unsubscribe(
					TO_AppData.Config.MessageFlow[i].MsgId,
					TO_AppData.DataPipeId);
			if (iStatus != CFE_SUCCESS)
			{
				(void) CFE_EVS_SendEvent(TO_CONFIG_TABLE_ERR_EID, CFE_EVS_ERROR,
						"Message flow failed to unsubscribe from 0x%08X. (%i)",
						TO_AppData.Config.MessageFlow[i].MsgId,
						(unsigned int)iStatus);
				goto end_of_function;
			}
		}
	}

	/* Next, pop all the messages off each priority queue so we can return
	 * each message memory allocation back to the memory pool, then delete
	 * the queue.
	 */
	for(i=0; i < TO_MAX_PRIORITY_QUEUES; ++i)
	{
		if(TO_AppData.Config.PriorityQueue[i].State != TO_PQUEUE_UNUSED)
		{
			if(TO_AppData.Config.PriorityQueue[i].OSALQueueID !=0)
			{
				CFE_SB_MsgPtr_t  msgPtr = 0;
				uint32 msgSize = 0;
				while(iStatus == OS_SUCCESS)
				{
					iStatus =  OS_QueueGet(
							TO_AppData.Config.PriorityQueue[i].OSALQueueID,
							&msgPtr, sizeof(msgPtr), &msgSize, OS_CHECK);
					if(iStatus == OS_SUCCESS)
					{
						iStatus = CFE_ES_PutPoolBuf(TO_AppData.HkTlm.MemPoolHandle, (uint32*)msgPtr);
						if(iStatus != OS_SUCCESS)
						{
							(void) CFE_EVS_SendEvent(TO_CONFIG_TABLE_ERR_EID, CFE_EVS_ERROR,
									"Failed to return message back to memory pool on tbl load. (%i)",
									(unsigned int)iStatus);
							goto end_of_function;
						}
					}
				}
				if(iStatus != OS_QUEUE_EMPTY)
				{
					(void) CFE_EVS_SendEvent(TO_CONFIG_TABLE_ERR_EID, CFE_EVS_ERROR,
							"Message flow failed to pop all messages from pqueue %u. (%i)",
							(unsigned int)i,
							(unsigned int)iStatus);
					goto end_of_function;
				}
				else
				{
					/* Queue is empty.  Delete the queue. */
					iStatus = OS_QueueDelete(
							TO_AppData.Config.PriorityQueue[i].OSALQueueID);
					if(iStatus != OS_SUCCESS)
					{
						(void) CFE_EVS_SendEvent(TO_CONFIG_TABLE_ERR_EID, CFE_EVS_ERROR,
								"Failed to delete priority queue %u. (%i)",
								(unsigned int)i,
								(unsigned int)iStatus);
						goto end_of_function;
					}
				}
			}
		}
	}

	/* Pop all the messages off each channel queue so we can return
	 * each message memory allocation back to the memory pool, then delete
	 * the queue.
	 */
	for(i=0; i < TO_MAX_OUTPUT_CHANNELS; ++i)
	{
		if(TO_AppData.Config.OutputChannel[i].State != TO_PQUEUE_UNUSED)
		{
			if(TO_AppData.Config.OutputChannel[i].OSALQueueID !=0)
			{
				CFE_SB_MsgPtr_t  msgPtr = 0;
				uint32 msgSize = 0;
				while(iStatus == OS_SUCCESS)
				{
					iStatus =  OS_QueueGet(
							TO_AppData.Config.OutputChannel[i].OSALQueueID,
							&msgPtr, sizeof(msgPtr), &msgSize, OS_CHECK);
					if(iStatus == OS_SUCCESS)
					{
						iStatus = CFE_ES_PutPoolBuf(TO_AppData.HkTlm.MemPoolHandle, (uint32*)msgPtr);
						if(iStatus != OS_SUCCESS)
						{
							(void) CFE_EVS_SendEvent(TO_CONFIG_TABLE_ERR_EID, CFE_EVS_ERROR,
									"Failed to return message back to memory pool on tbl load. (%i)",
									(unsigned int)iStatus);
							goto end_of_function;
						}
					}
				}
				if(iStatus != OS_QUEUE_EMPTY)
				{
					(void) CFE_EVS_SendEvent(TO_CONFIG_TABLE_ERR_EID, CFE_EVS_ERROR,
							"Message flow failed to pop all messages from queue %u. (%i)",
							(unsigned int)i,
							(unsigned int)iStatus);
					goto end_of_function;
				}
				else
				{
					/* Queue is empty.  Delete the queue. */
					iStatus = OS_QueueDelete(
							TO_AppData.Config.OutputChannel[i].OSALQueueID);
					if(iStatus != OS_SUCCESS)
					{
						(void) CFE_EVS_SendEvent(TO_CONFIG_TABLE_ERR_EID, CFE_EVS_ERROR,
								"Failed to delete priority queue %u. (%i)",
								(unsigned int)i,
								(unsigned int)iStatus);
						goto end_of_function;
					}
				}
			}
		}
	}

	/* Now copy the new table in so we can release the table pointer back to
	 * the system.
	 */
	memcpy(&TO_AppData.Config, TO_AppData.ConfigTblPtr, sizeof(TO_AppData.Config));

    /*
     * Now that we have the new table, create all the new resources we need
     * starting with queues for the channels and priority queues.
     */
    for(i=0; i < TO_MAX_OUTPUT_CHANNELS; ++i)
    {
    	if(TO_AppData.Config.OutputChannel[i].State != TO_OUT_CHANNEL_UNUSED)
    	{
			char QueueName[OS_MAX_API_NAME];
			snprintf(QueueName, OS_MAX_API_NAME, "TO_CH_%u", (unsigned int)i);
			iStatus = OS_QueueCreate(
					&TO_AppData.Config.OutputChannel[i].OSALQueueID,
					QueueName,
					TO_AppData.Config.OutputChannel[i].MsgLimit,
					sizeof(CFE_SB_Msg_t*),
					0);
			if (iStatus != OS_SUCCESS)
			{
				(void) CFE_EVS_SendEvent(TO_CONFIG_TABLE_ERR_EID, CFE_EVS_ERROR,
										 "Failed to create output channel queue. (%i)",
										 (unsigned int)iStatus);
				goto end_of_function;

			}
    	}
    }

    /*
     * Create the priority queues.
     */
    for(i=0; i < TO_MAX_PRIORITY_QUEUES; ++i)
    {
    	if(TO_AppData.Config.PriorityQueue[i].State != TO_PQUEUE_UNUSED)
    	{
			char QueueName[OS_MAX_API_NAME];
			snprintf(QueueName, OS_MAX_API_NAME, "TO_PQ_%u", (unsigned int)i);
			iStatus = OS_QueueCreate(
					&TO_AppData.Config.PriorityQueue[i].OSALQueueID,
					QueueName,
					TO_AppData.Config.OutputChannel[i].MsgLimit,
					sizeof(CFE_SB_Msg_t*),
					0);
			if (iStatus != OS_SUCCESS)
			{
				 (void) CFE_EVS_SendEvent(TO_CONFIG_TABLE_ERR_EID, CFE_EVS_ERROR,
										  "Failed to create priority queue. (%i)",
										  (unsigned int)iStatus);
				 goto end_of_function;

			}
    	}
    }

    /*
     * Create message flows by subscribing to messages.
     */
	for(i = 0; i < TO_MAX_MESSAGE_FLOWS; ++i)
	{
		if(TO_AppData.Config.MessageFlow[i].MsgId != 0)
		{
			/* Subscribe to message. */
			iStatus = CFE_SB_SubscribeEx(TO_AppData.Config.MessageFlow[i].MsgId, TO_AppData.DataPipeId,
										 CFE_SB_Default_Qos, TO_AppData.Config.MessageFlow[i].MsgLimit);
			if (iStatus != CFE_SUCCESS)
			{
				(void) CFE_EVS_SendEvent(TO_CONFIG_TABLE_ERR_EID, CFE_EVS_ERROR,
						"Message flow failed to subscribe to (0x%08X). (%i)",
						TO_AppData.Config.MessageFlow[i].MsgId,
						(unsigned int)iStatus);
				goto end_of_function;
			}
		}
	}

end_of_function:
    return iStatus;

}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Acquire Conifg Pointers                                         */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
int32 TO_AcquireConfigPointers(void)
{
    int32 iStatus = CFE_SUCCESS;
    uint32 i = 0;

    /*
    ** Release the table
    */
    /* TODO: This return value can indicate success, error, or that the info has been
     * updated.  We ignore this return value in favor of checking CFE_TBL_Manage(), but
     * be sure this is the behavior you want. */
    (void) CFE_TBL_ReleaseAddress(TO_AppData.ConfigTblHdl);

    /*
    ** Manage the table
    */
    iStatus = CFE_TBL_Manage(TO_AppData.ConfigTblHdl);
    if ((iStatus != CFE_SUCCESS) && (iStatus != CFE_TBL_INFO_UPDATED))
    {
        (void) CFE_EVS_SendEvent(TO_CONFIG_TABLE_ERR_EID, CFE_EVS_ERROR,
                                 "Failed to manage Config table (0x%08X)",
                                 (unsigned int)iStatus);
        goto TO_AcquireConfigPointers_Exit_Tag;
    }

    /*
    ** Get a pointer to the table
    */
    iStatus = CFE_TBL_GetAddress((void*)&TO_AppData.ConfigTblPtr,
                                 TO_AppData.ConfigTblHdl);
    if (iStatus == CFE_TBL_INFO_UPDATED)
    {
        TO_ProcessNewConfigTbl();
        iStatus = CFE_SUCCESS;
    }
    else if(iStatus != CFE_SUCCESS)
    {
    	TO_AppData.ConfigTblPtr = 0;
        (void) CFE_EVS_SendEvent(TO_CONFIG_TABLE_ERR_EID, CFE_EVS_ERROR,
                                 "Failed to get Config table's address (0x%08X)",
                                 (unsigned int)iStatus);
    }

TO_AcquireConfigPointers_Exit_Tag:
    return (iStatus);

} /* End of TO_AcquirePointers */

/************************/
/*  End of File Comment */
/************************/
    
