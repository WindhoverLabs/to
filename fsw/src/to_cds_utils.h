
#ifndef TO_CDS_UTILS_H
#define TO_CDS_UTILS_H

/************************************************************************
** Pragmas
*************************************************************************/

/************************************************************************
** Includes
*************************************************************************/
#include "to_tbldefs.h"

#ifdef __cplusplus
extern "C" {
#endif

/************************************************************************
** Local Defines
*************************************************************************/

/************************************************************************
** Local Structure Declarations
*************************************************************************/

/************************************************************************
** External Global Variables
*************************************************************************/

/************************************************************************
** Global Variables
*************************************************************************/

/************************************************************************
** Local Variables
*************************************************************************/

/************************************************************************
** Local Function Prototypes
*************************************************************************/

/************************************************************************/
/** \brief Init TO CDS tables
**
**  \par Description
**       This function initializes TO's CDS tables
**
**  \par Assumptions, External Events, and Notes:
**       None
**
**  \returns
**  \retcode #CFE_SUCCESS  \retdesc \copydoc CFE_SUCCESS \endcode
**  \retstmt Return codes from #CFE_ES_RegisterCDS       \endcode
**  \retstmt Return codes from #CFE_ES_CopyToCDS         \endcode
**  \endreturns
**
*************************************************************************/
int32  TO_InitCdsTbl(void);

/************************************************************************/
/** \brief Update TO CDS tables
**
**  \par Description
**       This function updates TO's CDS tables
**
**  \par Assumptions, External Events, and Notes:
**       None
**
*************************************************************************/
void   TO_UpdateCdsTbl(void);

/************************************************************************/
/** \brief Save TO CDS tables
**
**  \par Description
**       This function saves TO's CDS tables
**
**  \par Assumptions, External Events, and Notes:
**       None
**
*************************************************************************/
void   TO_SaveCdsTbl(void);


#ifdef __cplusplus
}
#endif
 
#endif /* TO_CDS_UTILS_H */

/************************/
/*  End of File Comment */
/************************/
    
