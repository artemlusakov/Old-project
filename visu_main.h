/************************************************************************
*
* File:         VISU_MAIN.h
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
#ifndef VISU_MAIN_H
#define VISU_MAIN_H
#include "visu.h"
/*************************************************************************/

/* macro definitions *****************************************************/

/* type definitions ******************************************************/

/* prototypes ************************************************************/

/* global constants ******************************************************/

/* global variables ******************************************************/

/* global function prototypes ********************************************/
//void Visu_Main_Init(const tVisuData * VData);
void Visu_Main_Open( const tVisuData* VData );
void Visu_Main_Cycle( const tVisuData* VData, uint32_t evtc, tUserCEvt* evtv );
void Visu_Main_Close( const tVisuData* VData );
void Visu_Main_Timer( const tVisuData* VData );

void Password_Open( const tVisuData* VData );
void Password_Cycle( const tVisuData* VData, uint32_t evtc, tUserCEvt* evtv );

/*************************************************************************/
/* Protection against multiple includes ends here ! Stop editing here !  */
/*************************************************************************/
#endif  // #ifndef VISU_MAIN_H
