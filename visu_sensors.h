/************************************************************************
*
* File:         VISU_SENSORS.h
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
#ifndef VISU_SENSORS_H
#define VISU_SENSORS_H
#include "visu.h"
/*************************************************************************/

/* macro definitions *****************************************************/

/* type definitions ******************************************************/

/* prototypes ************************************************************/

/* global constants ******************************************************/

/* global variables ******************************************************/

/* global function prototypes ********************************************/
void Visu_Sensor_Init( const tVisuData* Data );
void Visu_Sensor_Open( const tVisuData* VData );
void Visu_Sensor_Cycle( const tVisuData* VData, uint32_t evtc, tUserCEvt* evtv );

void Sensors_LoadBacup( void );
void Sensors_SaveBacup( void );

/*************************************************************************/
/* Protection against multiple includes ends here ! Stop editing here !  */
/*************************************************************************/
#endif  // #ifndef VISU_SENSORS_H
