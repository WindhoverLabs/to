#include "to_app.h"
#include "to_channel.h"
#include <string.h>

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Run the Classifier algorithm                                    */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
int32 TO_Channel_OpenChannel(uint32 index, char *ChannelName,
		char *ConfigTableName, char *ConfigTableFileName,
		char *DumpTableName)
{
	uint32 iStatus = -1;
	char pipeName[OS_MAX_PATH_LEN];
	TO_ChannelData_t *channel;

	if(index >= TO_MAX_CHANNELS)
	{
		iStatus == -1;
		goto end_of_function;
	}

	channel = &TO_AppData.ChannelData[index];

    if(channel->State == TO_CHANNEL_CLOSED)
    {
		sprintf(pipeName, "TO_%s", ChannelName);

		strncpy(channel->ChannelName,
				ChannelName, sizeof(channel->ChannelName));
		strncpy(channel->ConfigTableName,
				ConfigTableName, sizeof(channel->ConfigTableName));
		strncpy(channel->ConfigTableFileName,
				ConfigTableFileName, sizeof(channel->ConfigTableFileName));
		strncpy(channel->DumpTableName,
				DumpTableName, sizeof(channel->DumpTableName));

		iStatus = TO_OutputQueue_Buildup(channel);
		if(iStatus != 0)
		{
			goto end_of_function;
		}

		/* Init data pipe and subscribe to messages on the data pipe */
		iStatus = CFE_SB_CreatePipe(&channel->DataPipeId,
									 TO_DATA_PIPE_DEPTH,
									 pipeName);
		if (iStatus != CFE_SUCCESS)
		{
			(void) CFE_EVS_SendEvent(TO_INIT_ERR_EID, CFE_EVS_ERROR,
									 "Failed to create channel '%s' pipe (0x%08X)",
									 ChannelName,
									 (unsigned int)iStatus);
			goto end_of_function;
		}

		channel->State = TO_CHANNEL_OPENED;
		iStatus = TO_InitConfigTbl(channel);
		if (iStatus != CFE_SUCCESS)
		{
			CFE_EVS_SendEvent(TO_INIT_ERR_EID, CFE_EVS_ERROR,
							  "Failed to init config tables (0x%08X)",
							  (unsigned int)iStatus);
			channel->State = TO_CHANNEL_CLOSED;
			goto end_of_function;
		}
    }

end_of_function:
	return iStatus;
}


int32 TO_Channel_ProcessTelemetryAll(void)
{
    int32 iStatus = CFE_SUCCESS;
    uint32 i = 0;

    for(i = 0; i < TO_MAX_CHANNELS; ++i)
    {
	    TO_ChannelData_t *channel = &TO_AppData.ChannelData[i];
	    if(channel->State == TO_CHANNEL_OPENED)
	    {
	    	TO_Channel_ProcessTelemetry(channel);
	    }
    }
}


int32 TO_Channel_ProcessTelemetry(TO_ChannelData_t *channel)
{
	TO_Classifier_Run(channel);
	TO_Scheduler_Run(channel);
}



int32 TO_Channel_ResetCountsAll(void)
{
    int32 iStatus = CFE_SUCCESS;
    uint32 i = 0;

    for(i = 0; i < TO_MAX_CHANNELS; ++i)
    {
	    TO_ChannelData_t *channel = &TO_AppData.ChannelData[i];
	    if(channel->State == TO_CHANNEL_OPENED)
	    {
	    	TO_Channel_ResetCounts(channel);
	    }
    }
}



int32 TO_Channel_ResetCounts(TO_ChannelData_t *channel)
{
	TO_MessageFlow_ResetCountsAll(channel);
	TO_PriorityQueue_ResetCountsAll(channel);
	TO_OutputQueue_ResetCounts(channel);
}

