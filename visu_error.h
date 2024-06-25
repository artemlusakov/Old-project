/************************************************************************
*
* File:         VISU_ERROR.h
* Project:
* Author(s):
* Date:
*
* Description:
*
*
*
*
*
************************************************************************/

 /************************************************************************/
 /* Protection against multiple includes.                                */
 /* Do not code anything outside the following defines (except comments) */
#ifndef VISU_ERROR_H
#define VISU_ERROR_H

#include "visu.h"
/*************************************************************************/

/* macro definitions *****************************************************/

/* type definitions ******************************************************/

/* prototypes ************************************************************/

/* global constants ******************************************************/

/* global variables ******************************************************/

/* global function prototypes ********************************************/
void Visu_Error_Init( const tVisuData* VData );
void Visu_Error_Open( const tVisuData* VData );
void Visu_Error_Close( const tVisuData* VData );
void Visu_Error_Cycle( const tVisuData* VData, uint32_t evtc, tUserCEvt* evtv );

/*************************************************************************/
/* Protection against multiple includes ends here ! Stop editing here !  */
/*************************************************************************/
#endif  // #ifndef VISU_ERROR_H
