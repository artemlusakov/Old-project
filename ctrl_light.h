/************************************************************************
*
* File:         CTRL_LIGHT.h
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
#ifndef CTRL_LIGHT_H
#define CTRL_LIGHT_H

#include "io_types.h"

/*************************************************************************/

/* macro definitions *****************************************************/

/* type definitions ******************************************************/
typedef struct tagLight {
	struct   {
		uint8_t PloughHead[2];
		uint8_t FrontBeaconCab;
		uint8_t RTR_RearBeacon;
		uint8_t LRS_RearBeacon;
		uint8_t RTR_LeftRoadSign;
		uint8_t LRS_LeftRoadSign;
		uint8_t RTR_RightRoadSign;
		uint8_t LRS_RightRoadSign;
		uint8_t WorkSide;
		uint8_t OKB600_WorkRear;
		uint8_t BrushLight;
		uint8_t RTR_WorkRear;
		uint8_t RTR_HopperTop;
		uint8_t LRS_WorkRear;
	} On;
} tLight;

void Ctrl_Light( tLight* Light, uint32_t evtc, tUserCEvt* evtv );
void Light_Draw( const tLight* Light );

int Light_SaveToBackup( void );
int Light_LoadFromBackup( void );

/* prototypes ************************************************************/

/* global constants ******************************************************/

/* global variables ******************************************************/

/* global function prototypes ********************************************/

/*************************************************************************/
/* Protection against multiple includes ends here ! Stop editing here !  */
/*************************************************************************/
#endif  // #ifndef CTRL_LIGHT_H
