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
int32 TO_Custom_Init(void);



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
int32 TO_OutputChannel_CustomBuildup(uint32 index);



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
int32 TO_OutputChannel_CustomTeardown(uint32 index);



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



/************************************************************************/
/** \brief Custom function to return channel mode status.
**
**  \par Description
**       This function returns the status of a channel from the custom 
**       layer.
**
**  \par Assumptions, External Events, and Notes:
**       This function must be defined, though not all custom layers will
**       have commands to process.  This function must at least raise an
**       event when an unknown command is received.
**
**  \param [in]   index        The channel to return the status of
**
*************************************************************************/
uint8 TO_OutputChannel_Status(uint32 index);



#endif
