#include "to_app.h"
#include "to_custom.h"
#include "to_events.h"


extern TO_AppData_t TO_AppData;

int32 TO_OutputChannel_CustomInitAll(void)
{
	return 0;
}



int32 TO_OutputChannel_CustomBuildupAll(void)
{
	return 0;
}



int32 TO_OutputChannel_CustomTeardownAll(void)
{
	return 0;
}



void  TO_OutputChannel_CustomCleanupAll(void)
{

}



void TO_OutputChannel_ProcessNewCustomCmds(CFE_SB_Msg_t* MsgPtr)
{
    uint32  uiCmdCode=0;

    if (MsgPtr != NULL)
    {
    	uint16 inSize = CFE_SB_GetTotalMsgLength(MsgPtr);
        uiCmdCode = CFE_SB_GetCmdCode(MsgPtr);

        TO_AppData.HkTlm.usCmdErrCnt++;
        (void) CFE_EVS_SendEvent(TO_MSGID_ERR_EID, CFE_EVS_ERROR,
        		"Recvd invalid cmdId (%u)", (unsigned int)uiCmdCode);
    }
}
