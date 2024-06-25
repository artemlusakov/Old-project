/************************************************************************
*
* File:         VISU_LEARNING.h
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
#ifndef VISU_LEARNING_H
#define VISU_LEARNING_H

/*************************************************************************/
#include "visu.h"
#include "control.h"
/*************************************************************************/

/* macro definitions *****************************************************/

/* type definitions ******************************************************/

/* prototypes ************************************************************/

/* global constants ******************************************************/

/* global variables ******************************************************/

/* global function prototypes ********************************************/
void Visu_Learning_Init( const tVisuData* VData );
void Visu_Learning_Open( const tVisuData* VData );
void Visu_Learning_Close( const tVisuData* VData );
void Visu_Learning_Cycle( const tVisuData* VData, uint32_t evtc, tUserCEvt* evtv );

void Visu_L_DrawBackground( const tControl* Ctrl );

/*************************************************************************/
/* Protection against multiple includes ends here ! Stop editing here !  */
/*************************************************************************/
#endif  // #ifndef VISU_LEARNING_H
