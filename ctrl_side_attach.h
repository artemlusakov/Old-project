/************************************************************************
*
* File:         CTRL_SIDE_ATTACH.h
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
#ifndef CTRL_SIDE_ATTACH_H
#define CTRL_SIDE_ATTACH_H
#include "control.h"
/*************************************************************************/

/* macro definitions *****************************************************/

/* type definitions ******************************************************/

/* prototypes ************************************************************/

/* global constants ******************************************************/

/* global variables ******************************************************/

/* global function prototypes ********************************************/

/**************************************************************************
* Functions to Init Side Attachments.
* The valves for using the Sideattachment will be defined.
* ***********************************************************************/

void PinOut_Side_Init(void);

void Ctrl_Side_BUCHER_AK_Init( tControl* Ctrl );
void Ctrl_Side_MSN37_Init( tControl* Ctrl );
void Ctrl_Side_AA_AC_Init( tControl* Ctrl );
void Ctrl_Side_AK_Cycle( tControl* Ctrl, uint32_t evtc, tUserCEvt* evtv );

void Ctrl_Side_Plough_Cycle( tControl* Ctrl, uint32_t evtc, tUserCEvt* evtv );

void Ctrl_Side_Plough_Timer_10ms( tControl* Ctrl );

/*************************************************************************/
/* Protection against multiple includes ends here ! Stop editing here !  */
/*************************************************************************/
#endif // #ifndef CTRL_SIDE_ATTACH_H
