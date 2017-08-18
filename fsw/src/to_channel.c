#include "to_app.h"
#include "to_channel.h"



/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Run the Classifier algorithm                                    */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void TO_Channel_ProcessAll()
{
    uint32 i = 0;

    for(i = 0; i < TO_MAX_CHANNELS; ++i)
    {
        TO_Channel_ProcessChannel(i);
    }
}


void TO_Channel_Process(uint32 idx)
{
    if(idx < TO_MAX_CHANNELS)
    {
    	TO_Classifier_Run();
    	TO_Scheduler_Run();
    }
}

