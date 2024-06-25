/************************************************************************
*
* File:         CTRL_INTER_ATTACH.h
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
#ifndef CTRL_INTER_ATTACH_H
#define CTRL_INTER_ATTACH_H
#include "control.h"
/*************************************************************************/

/* macro definitions *****************************************************/

/* type definitions ******************************************************/

/* prototypes ************************************************************/

/* global constants ******************************************************/

/* global variables ******************************************************/

/* global function prototypes ********************************************/

void PinOut_Inter_Init(void);

void Ctrl_Inter_OMP220_Init( tControl* Ctrl );
void Ctrl_Inter_OMP220_Humid_Init( tControl* Ctrl );
void Ctrl_Inter_OZP231_Init( tControl* Ctrl );
void Ctrl_Inter_CH2600_Init( tControl* Ctrl );
void Ctrl_Inter_CH2800_Init( tControl* Ctrl );
void Ctrl_Front_MSPN_B( tControl *Ctrl );

void Ctrl_Inter_Brush_Standard_Cycle( tControl* Ctrl, uint32_t evtc, tUserCEvt* evtv );
void Ctrl_Inter_Plough_Standard_Cycle( tControl* Ctrl, uint32_t evtc, tUserCEvt* evtv );
void Ctrl_Inter_CH2800_Standard_Cycle( tControl* Ctrl, uint32_t evtc, tUserCEvt* evtv );

void Ctrl_Inter_Plough_Timer_10ms( tControl* Ctrl );
void Ctrl_Inter_Brush_Timer_10ms( tControl* Ctrl );
/*************************************************************************/
/* Protection against multiple includes ends here ! Stop editing here !  */
/*************************************************************************/
#endif  // #ifndef CTRL_INTER_ATTACH_H
