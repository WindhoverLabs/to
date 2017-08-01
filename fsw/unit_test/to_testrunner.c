
#include "uttest.h"

#include "to_app_test.h"
#include "to_cmds_test.h"
#include "to_config_tbl_test.h"
#include "to_cds_tbl_test.h"

int main(void)
{   
    TO_App_Test_AddTestCases();
    TO_Cmds_Test_AddTestCases();
    TO_Config_Tbl_Test_AddTestCases();
    TO_CDS_Tbl_Test_AddTestCases();

    return(UtTest_Run());

}

