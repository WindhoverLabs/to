#ifndef TO_CUSTOM_H
#define TO_CUSTOM_H

/************************************************************************
** Pragmas
*************************************************************************/

/************************************************************************
** Includes
*************************************************************************/

#include "cfe.h"

/************************************************************************
** Local Defines
*************************************************************************/

/************************************************************************
** Local Structure Definitions
*************************************************************************/

/************************************************************************
** External Global Variables
*************************************************************************/


/************************************************************************/
/** \brief Custom function to initialize all output channels.
**
**  \par Description
**       This function is called once at initialization and allows the
**       custom layer to provide specific functionality to initialize
**       internal objects.
**
**  \par Assumptions, External Events, and Notes:
**       This function must be defined, but not all custom
**       layers will do anything in this function.
**
*************************************************************************/
int32 TO_OutputChannel_CustomInitAll(void);



/************************************************************************/
/** \brief Custom function to build up all output channels.
**
**  \par Description
**       This function is called after a new configuration table has
**       been loaded, allowing the custom layer to do whatever it needs
**       to do after a reconfiguration, if anything.
**
**  \par Assumptions, External Events, and Notes:
**       This function must be defined, but not all custom
**       layers will do anything in this function.
**
*************************************************************************/
int32 TO_OutputChannel_CustomBuildupAll(void);



/************************************************************************/
/** \brief Custom function in to tear down all output channels.
**
**  \par Description
**       This function is called in preparation for loading a new
**       configuration table, allowing the custom layer to do whatever it
**       needs with the current configuration before reconfiguration,
**       if anything.
**
**  \par Assumptions, External Events, and Notes:
**       This function must be defined, but not all custom
**       layers will do anything in this function.
**
*************************************************************************/
int32 TO_OutputChannel_CustomTeardownAll(void);



/************************************************************************/
/** \brief Custom function to cleanup all output channels before
**         shutdown.
**
**  \par Description
**       This function is called in preparation for application shutdown.
**       This allows the custom layer to do whatever it needs to do, if
**       anything.
**
**  \par Assumptions, External Events, and Notes:
**       This function must be defined, but not all custom
**       layers will do anything in this function.
**
*************************************************************************/
void  TO_OutputChannel_CustomCleanupAll(void);



/************************************************************************/
/** \brief Custom function to process new commands.
**
**  \par Description
**       This function is called when the sealed layer receives a command
**       it doesn't know about.  This allows the custom layer to receive
**       commands without requiring the sealed layer to know about it.
**
**  \par Assumptions, External Events, and Notes:
**       This function must be defined, though not all custom layers will
**       have commands to process.  This function must at least raise an
**       event when an unknown command is received.
**
**  \param [in]   MsgPtr        A #CFE_SB_Msg_t pointer that
**                              references the software bus message
**
*************************************************************************/
void TO_OutputChannel_ProcessNewCustomCmds(CFE_SB_Msg_t* MsgPtr);

#endif
