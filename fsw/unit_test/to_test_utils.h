
#ifndef TO_TEST_UTILS_H
#define TO_TEST_UTILS_H

/*
 * Includes
 */

#include "to_app.h"

extern TO_AppData_t  TO_AppData;

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Function Definitions
 */

void TO_Test_Setup_EmptyConfig(void);
void TO_Test_Setup_FullConfig(void);
void TO_Test_Setup_InvalidConfig(void);
void TO_Test_TearDown(void);


#ifdef __cplusplus
}
#endif

#endif /* TO_TEST_UTILS_H */

