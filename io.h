/************************************************************************
*
* File:         IO.h
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
#ifndef IO_H
#define IO_H

#include "io_joystick_v85_b25.h"
#include "control.h"
/*************************************************************************/

/* macro definitions *****************************************************/

#define MCM250_1 0
#define MCM250_2 1
#define MCM250_3 2
#define MCM250_4 3
#define MCM250_5 4
#define MCM250_6 5
#define MCM250_7 6
#define MCM250_8 7

#define K2_4X3   8
//Add NEW MCM/K2-Devices here
#define K2_PLOUGH 9
#define JOYSTICK  8
#define ENGINE    9

#define MCM250_CURRENT_DO_0 4
#define MCM250_CURRENT_DO_1 5
#define MCM250_CURRENT_DO_2 6
#define MCM250_CURRENT_DO_3 7
#define MCM250_CURRENT_DO_4 8
#define MCM250_CURRENT_DO_5 9
#define MCM250_CURRENT_DO_6 10
#define MCM250_CURRENT_DO_7 11

#define CAN0 0
#define CAN1 1
#define BIT11 0
#define BIT29 1

/* type definitions ******************************************************/

/* prototypes ************************************************************/

/* global constants ******************************************************/

/* global variables ******************************************************/

/* global function prototypes ********************************************/

/**
* @brief This Function initalises the Devices at CAN0. MCM-Modules, Keyboard and Joystick
*
* @return uint32_t
*/
uint32_t IO_Init( tControl* Ctrl );

/**
* @brief this function is needed to actualize states of Joystick and MCM-Modules.
*
*
* @param evtc
* @param evtv
* @return uint32_t
*/
uint32_t IO_Cycle( tControl* Ctrl, uint32_t evtc, tUserCEvt* evtv );

/*************************************************************************/
/* Protection against multiple includes ends here ! Stop editing here !  */
/*************************************************************************/
#endif  // #ifndef IO_H
