
#ifndef TO_CUSTOM_H
#define TO_CUSTOM_H

#include "cfe.h"

int32 TO_InitCustom(void);
int32 TO_SendToChannel(uint32 ChannelID, const char* Buffer, uint32 Size);
void TO_CleanupCustom(void);
void TO_ProcessNewCustomCmds(CFE_SB_Msg_t* MsgPtr);

#endif
