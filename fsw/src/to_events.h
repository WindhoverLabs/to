#ifndef TO_EVENTS_H
#define TO_EVENTS_H

#ifdef __cplusplus
extern "C" {
#endif

/************************************************************************
** Local Defines
*************************************************************************/


/* Event IDs
 * Conventions: _EID is the event identifier.  _EVT_CNT is the total number of
 * events and should always be last.  Events can be added before _EVT_CNT.
 * For long-term maintenance, consider not removing an event but replacing it
 * with an unused, reserved, enum to preserve the IDs later in the list. */
typedef enum {

/** \brief <tt> Value of zero is reserved, and should not be used. </tt> */
    TO_RESERVED_EID = 0,  /* Do not use this event ID */

/** \brief <tt> 'TO - ' </tt>
**  \event <tt> 'TO - ' </tt>
**  
**  \par Type: INFORMATION
**
**  \par Cause:
**
**  TODO fill this in
**
*/
    TO_INF_EID,

/** \brief <tt> 'TO Initialized. Version \%d.\%d.\%d' </tt>
**  \event <tt> 'TO Initialized. Version \%d.\%d.\%d' </tt>
**  
**  \par Type: INFORMATION
**
**  \par Cause:
**
**  This event message is issued when the CFS TO Task has
**  completed initialization.
**
**  The first \c %d field contains the Application's Major Version Number
**  The second \c %d field contains the Application's Minor Version Number
**  The third \c %d field contains the Application's Revision Number
*/
    TO_INIT_INF_EID,

/** \brief <tt> 'TO - ' </tt>
**  \event <tt> 'TO - ' </tt>
**  
**  \par Type: INFORMATION
**
**  \par Cause:
**
**  This message is generated when a table is initialized.
**
*/
    TO_CONFIG_TABLE_INF_EID,

/** \brief <tt> 'TO - ' </tt>
**  \event <tt> 'TO - ' </tt>
**  
**  \par Type: INFORMATION
**
**  \par Cause:
**
**  This event message is issued when TBD
**
*/
    TO_CDS_INF_EID,

/** \brief <tt> 'TO - Recvd $x cmd (%us)' </tt>
**  \event <tt> 'TO - Recvd $x cmd (%us)' </tt>
**  
**  \par Type: INFORMATION
**
**  \par Cause:
**
**  This event message is issued when the CFS TO Task has
**  received and processed a command.
**
*/
    TO_CMD_INF_EID,

/** \brief <tt> 'TO - ' </tt>
**  \event <tt> 'TO - ' </tt>
**  
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is issued when TBD
**
*/
    TO_ERR_EID,

/** \brief <tt> 'TO - ' </tt>
**  \event <tt> 'TO - ' </tt>
**  
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is issued when the CFS TO Task has
**  had an error in initialization.
**
*/
    TO_INIT_ERR_EID,

/** \brief <tt> 'TO - ' </tt>
**  \event <tt> 'TO - ' </tt>
**  
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is issued when the CFS TO Task has
**  had an error with the configuration table.
**
*/
    TO_CONFIG_TABLE_ERR_EID,

/** \brief <tt> 'TO - ' </tt>
**  \event <tt> 'TO - ' </tt>
**  
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is issued when the CFS TO Task has
**  had an error in CDS.
**
*/
    TO_CDS_ERR_EID,

/** \brief <tt> 'TO - $commandError' </tt>
**  \event <tt> 'TO - $commandError' </tt>
**  
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is issued when the CFS TO Task has
**  had an error processing a command.
**
*/
    TO_CMD_ERR_EID,

/** \brief <tt> 'TO: SB pipe read error (0x%08X), app will exit' </tt>
**  \event <tt> 'TO: SB pipe read error (0x%08X), app will exit' </tt>
**  
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is issued when the CFS TO Task has
**  had an error reading from a pipe.
**
*/
    TO_PIPE_ERR_EID,

/** \brief <tt> 'TO - Recvd invalid $type msgId (0x%04x)' </tt>
**  \event <tt> 'TO - Recvd invalid $type msgId (0x%04x)' </tt>
**  
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is issued when the CFS TO Task has
**  received an invalid message ID.
**
*/
    TO_MSGID_ERR_EID,

/** \brief <tt> 'TO - Rcvd invalid msgLen: msgId=0x%08X, cmdCode=%d, msgLen=%d, expectedLen=%d" </tt>
**  \event <tt> 'TO - Rcvd invalid msgLen: msgId=0x%08X, cmdCode=%d, msgLen=%d, expectedLen=%d" </tt>
**  
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is issued when the CFS TO Task has
**  received a message with a bad length.
**
*/
    TO_MSGLEN_ERR_EID,

    /* TODO:  Add Doxygen markup. */
    TO_MSG_WRITE_ERR_EID,
	TO_MSG_UNSUB_DBG_EID,
	TO_MSG_DROP_FROM_FLOW_DBG_EID,
	TO_CR_POOL_ERR_EID,
	TO_GET_POOL_ERR_EID,
	TO_TLM_LISTEN_ERR_EID,
	TO_MSG_FLOW_INFO_EID,
	TO_PQUEUE_INFO_EID,
	TO_OUT_CH_INFO_EID,
	TO_CMD_NOOP_EID,
	TO_CMD_RESET_EID,
	TO_CMD_ADD_MSG_FLOW_EID,
	TO_CMD_REMOVE_MSG_FLOW_EID,
	TO_CMD_SEND_DIAG_EID,

/** \brief <tt> This is a count of all the app events and should not be used. </tt> */
    TO_EVT_CNT
} TO_EventIds_t;


#ifdef __cplusplus
}
#endif

#endif /* TO_EVENTS_H */

/************************/
/*  End of File Comment */
/************************/
    
