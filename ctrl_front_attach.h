/************************************************************************
*
* File:         CTRL_FRONT_ATTACH.h
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
#ifndef CTRL_FRONT_ATTACH_H
#define CTRL_FRONT_ATTACH_H
#include "control.h"
/*************************************************************************/

/* macro definitions *****************************************************/

/* type definitions ******************************************************/

/* prototypes ************************************************************/

/* global constants ******************************************************/

/* global variables ******************************************************/

/* global function prototypes ********************************************/

/*************************************************************************/
/* Protection against multiple includes ends here ! Stop editing here !  */
/*************************************************************************/
#endif  // #ifndef CTRL_FRONT_ATTACH_H

void PinOut_Front_Init(void);

void Ctrl_Front_FK_Init( tControl* Ctrl );
void Ctrl_Front_TN34_Init( tControl* Ctrl );
void Ctrl_Front_MN_Init( tControl* Ctrl );

void Ctrl_Front_OFF_OPF_245( tControl* Ctrl );
void Ctrl_Front_OMB1( tControl* Ctrl );
void Ctrl_Front_OMT1( tControl* Ctrl );
void Ctrl_Front_MF300( tControl* Ctrl );
void Ctrl_Front_MF500( tControl* Ctrl );
void Ctrl_Front_MSPN_A ( tControl *Ctrl );

void Ctrl_Front_Plough_Standard_Cycle( tControl* Ctrl, uint32_t evtc, tUserCEvt* evtv );
void Ctrl_Front_Plough_Standard_Timer_10ms( tControl* Ctrl );

void Ctrl_Front_Brush_Standard_Cycle( tControl* Ctrl, uint32_t evtc, tUserCEvt* evtv );
void Ctrl_Front_Plough_2_Sections_MSPN_Cycle(tControl *Ctrl, uint32_t evtc, tUserCEvt *evtv);
void Ctrl_Front_Brush_Timer_10ms( tControl* Ctrl );

// OKB600
void Ctrl_Front_OKB600_Init(tControl * Ctrl);
void Ctrl_Front_OKB600_Cycle(tControl * Ctrl);
void Ctrl_Front_OKB600_Timer_10ms(tControl * Ctrl);

void Ctrl_Front_Brush_Draw( tControl* Ctrl );
