/****************************************************************************
*
* Project:   name
*
* @file      filename.h
* @author    author
* @date      [Creation date in format %02d.%02d.20%02d]
*
* @brief     description
*
****************************************************************************/

 /* Protection against multiple includes.                                    */
 /* Do not code anything outside the following defines (except comments)     */
#ifndef VISU_SPREADSETTINGS_H
#define VISU_SPREADSETTINGS_H
#include "visu.h"
/* macro definitions ********************************************************/

/* type definitions *********************************************************/

/* prototypes ***************************************************************/

/* global constants *********************************************************/

/* global variables *********************************************************/

/* global function prototypes ***********************************************/
void Visu_SpreadSettings_Open( const tVisuData* VData );
void Visu_SpreadSettings_Cycle( const tVisuData* VData, uint32_t evtc, tUserCEvt* evt );
void Visu_SpreadSettings_Close( const tVisuData* VData );

/****************************************************************************/
/* Protection against multiple includes ends here ! Stop editing here !     */
/****************************************************************************/
#endif  // #ifndef VISU_SPREADSETTINGS_H
