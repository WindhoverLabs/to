#include "to_app.h"
#include "to_custom_stubs.h"
#include "to_custom.h"
#include "to_events.h"


extern TO_AppData_t TO_AppData;

TO_Custom_Returns_t TO_Custom_Test_Returns = {0};


int32 TO_Custom_Init(void)
{
    return TO_Custom_Test_Returns.TO_Custom_Init_Return;
}


int32 TO_OutputChannel_CustomBuildup(uint32 index)
{
    return TO_Custom_Test_Returns.TO_Custom_Buildup_Return;
}


int32 TO_OutputChannel_CustomTeardown(uint32 index)
{
    return TO_Custom_Test_Returns.TO_Custom_Teardown_Return;
}


void TO_OutputChannel_CustomCleanupAll(void)
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


uint8 TO_OutputChannel_Status(uint32 index)
{
    return TO_Custom_Test_Returns.TO_Custom_Status_Return;
}
