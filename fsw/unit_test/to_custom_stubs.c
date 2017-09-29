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
* 3. Neither the name Windhover Labs nor the names of its 
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

#include "to_app.h"
#include "to_custom_hooks.h"
#include "to_custom_stubs.h"
#include "to_custom.h"
#include "to_events.h"


extern TO_AppData_t TO_AppData;

TO_Custom_Returns_t TO_Custom_Test_Returns = {0};


int32 TO_Custom_Init(void)
{
    if(TO_Custom_Test_Hooks.TO_Custom_Init_Use_Hook == TRUE)
    {
        return TO_Custom_InitHook();
    }
    else
    {
    return TO_Custom_Test_Returns.TO_Custom_Init_Return;
    }
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
