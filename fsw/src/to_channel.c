#include "to_app.h"
#include "to_channel.h"

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Run the Classifier algorithm                                    */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
int32 TO_Channel_OpenChannel(uint32 index, char *TableName, char *TableFileName)
{
	uint32 iStatus = 0;

	strncpy(TO_AppData.ChannelData[index].TableName,
			TableName, sizeof(TO_AppData.ChannelData[index].TableName));
	strncpy(TO_AppData.ChannelData[index].TableFileName,
			TableFileName, sizeof(TO_AppData.ChannelData[index].TableFileName));

	TO_AppData.ChannelData[index].State = TO_CHANNEL_OPENED;

    iStatus = TO_InitConfigTbl(&TO_AppData.ChannelData[index]);
    if (iStatus != CFE_SUCCESS)
    {
        CFE_EVS_SendEvent(TO_INIT_ERR_EID, CFE_EVS_ERROR,
                          "Failed to init config tables (0x%08X)",
                          (unsigned int)iStatus);
    }

	return iStatus;
}
