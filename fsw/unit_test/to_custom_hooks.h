#ifndef TO_CUSTOM_HOOKS_H
#define TO_CUSTOM_HOOKS_H

#ifdef __cplusplus
extern "C" {
#endif

#include "cfe.h"

typedef struct
{
    boolean TO_Custom_Init_Use_Hook;
} TO_Custom_Hooks_t;

extern TO_Custom_Hooks_t TO_Custom_Test_Hooks;

int32 TO_Custom_InitHook(void);

#ifdef __cplusplus
}
#endif

#endif /* TO_CUSTOM_HOOKS_H */
