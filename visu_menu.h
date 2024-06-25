/************************************************************************
*
* File:         VISU_MENU.h
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
#ifndef VISU_MENU_H
#define VISU_MENU_H

#include "visu.h"
/*************************************************************************/

/* macro definitions *****************************************************/

/* type definitions ******************************************************/

/* prototypes ************************************************************/

/* global constants ******************************************************/

/* global variables ******************************************************/

/* global function prototypes ********************************************/
void Visu_Menu_Admin_Open( const tVisuData* VData );
void Visu_Menu_Admin_Cycle( const tVisuData* VData, uint32_t evtc, tUserCEvt* evt );
void Visu_Menu_Merkator_Open( const tVisuData* VData );
void Visu_Menu_Merkator_Cycle( const tVisuData* VData, uint32_t evtc, tUserCEvt* evt );

/*************************************************************************/
/* Protection against multiple includes ends here ! Stop editing here !  */
/*************************************************************************/
#endif  // #ifndef VISU_MENU_H
