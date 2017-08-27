#ifndef TO_CUSTOM_STUBS_H
#define TO_CUSTOM_STUBS_H

#ifdef __cplusplus
extern "C" {
#endif


typedef struct
{
    int32 TO_Custom_Init_Return;
    int32 TO_Custom_Buildup_Return;
    int32 TO_Custom_Teardown_Return;
    uint8 TO_Custom_Status_Return;
} TO_Custom_Returns_t;

extern TO_Custom_Returns_t TO_Custom_Test_Returns;

#ifdef __cplusplus
}
#endif

#endif /* TO_CUSTOM_STUBS_H */
