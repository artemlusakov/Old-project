/************************************************************************
*
* File:         VISU_START.h
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
#include "visu.h"

 /************************************************************************/
 /* Protection against multiple includes.                                */
 /* Do not code anything outside the following defines (except comments) */
#ifndef VISU_START_H
#define VISU_START_H

/*************************************************************************/

/* macro definitions *****************************************************/

/* type definitions ******************************************************/

/* prototypes ************************************************************/

/* global constants ******************************************************/

/* global variables ******************************************************/

/* global function prototypes ********************************************/

void Visu_Start_Open( const tVisuData* VData );
void VisuStart_Cycle( const tVisuData* VData, uint32_t evtc, tUserCEvt* );

/*************************************************************************/
/* Protection against multiple includes ends here ! Stop editing here !  */
/*************************************************************************/
#endif  // #ifndef VISU_START_H
