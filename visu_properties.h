/************************************************************************
*
* File:         VISU_PROPERTIES.h
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
#ifndef VISU_PROPERTIES_H
#define VISU_PROPERTIES_H

/*************************************************************************/
#include "visu.h"
/*************************************************************************/

/* macro definitions *****************************************************/

/* type definitions ******************************************************/

/* prototypes ************************************************************/

/* global constants ******************************************************/

/* global variables ******************************************************/

/* global function prototypes ********************************************/
void Visu_Properties_Init( const tVisuData* VData );
void Visu_Properties_Open( const tVisuData* VData );
void Visu_Properties_Close( const tVisuData* VData );
void Visu_Properties_Cycle( const tVisuData* VData, uint32_t evtc, tUserCEvt* evtv );

/*************************************************************************/
/* Protection against multiple includes ends here ! Stop editing here !  */
/*************************************************************************/
#endif  // #ifndef VISU_PROPERTIES_H
