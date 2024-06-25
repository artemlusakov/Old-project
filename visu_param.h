/************************************************************************
*
* File:         VISU_PARAM.h
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
#ifndef VISU_PARAM_H
#define VISU_PARAM_H

#include "visu.h"
/*************************************************************************/

/* macro definitions *****************************************************/

/* type definitions ******************************************************/

/* prototypes ************************************************************/

/* global constants ******************************************************/

/* global variables ******************************************************/

/* global function prototypes ********************************************/

void Visu_Param_Init( const tVisuData* Data );
void Visu_Param_Open( const tVisuData* Data );
void Visu_Param_Cycle( const tVisuData* Data, uint32_t evtc, tUserCEvt* evtv );

/*************************************************************************/
/* Protection against multiple includes ends here ! Stop editing here !  */
/*************************************************************************/
#endif  // #ifndef VISU_PARAM_H
