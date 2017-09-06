#include "ut_cfe_es_hooks.h"

int32 Ut_CFE_ES_GetPoolBuf(uint32 **BufPtr, CFE_ES_MemHandle_t HandlePtr, uint32 Size)
{
	if(BufPtr == 0)
	{
		return -1;
	}

	*BufPtr = malloc(Size);
    
    if(*BufPtr == 0)
	{
		return -1;
	}

	return Size;
}



int32 Ut_CFE_ES_PutPoolBuf(CFE_ES_MemHandle_t HandlePtr, uint32 *BufPtr)
{
	if(BufPtr == 0)
	{
		return -1;
	}

    free(BufPtr);

	return 0;
}
