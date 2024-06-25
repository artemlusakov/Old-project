/************************************************************************
*
* File:         IO_JOYSTICK_V85_B25.h
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
#ifndef IO_JOYSTICK_V85_B25_H
#define IO_JOYSTICK_V85_B25_H
#include "io_types.h"

/*************************************************************************/

/* macro definitions *****************************************************/

/* type definitions ******************************************************/

/* prototypes ************************************************************/
typedef enum eagJoystickButtons {
	JSB_A,
	JSB_B,
	JSB_C,
	JSB_D,
	JSB_E,
	JSB_F,
	JSB_G,
	JSB_NUM_BUTTONS
} eJS_Buttons;

/**
* @brief these are all available Joystick types. the actual type will be read out of the config file.
*
*/
typedef enum eagJoystickTypes {
	JS_TYPE_CURTISS,
	JS_TYPE_J1939,
	JS_TYPE_CANOPEN,
}eJoystickTypes;

typedef struct tag_Joystick {
	int32_t id;
	int32_t id2;
	int32_t fifo;
	int32_t can;
	int32_t ext;
	uint32_t time;
	tIOT_Button Button[7];
	tIOT_AnaJoy UpDown, LeftRight, X, Y, Z;
	char name[64];
	uint8_t active;
	eJoystickTypes Type;
}t_Joystick;

/* global constants ******************************************************/

/* global variables ******************************************************/

/* global function prototypes ********************************************/

/**
* @brief Initializes the joystick.
*
* @param Joystick Pointer to a structure of the type t_Joystick
* @param id       id of the Joystick This depends on the type of Joystick you use.
*                 At a Joystick using CanOPEN this will be the NodeID (1-127)
*                 At a Joystick using J1939 this will be the Sourceaddress.
* @param can      Used CAN
* @param ext      0 for 11 Bit, 1 for 29 Bit
*/
void IO_Joystick_init( t_Joystick* Joystick, const uint32_t id,const uint32_t id2, const uint32_t can, const uint32_t ext );

/**
* @brief This Function will read the CAN-FiFo of the Joystick and will actualize the States of the Keys
*
* @param Joystick
*/
void IO_Joystick_Cycle( t_Joystick* Joystick );

/**
* @brief Checks, if the communication with the Joystick ist active.
* If the Device doesn't receive a Can-Message from teh Joystick for more then
* 1000ms, the Joystick will be detected as Inactive.
*
* @param Joystick
* @return uint32_t 1 if Joystick active, else 0
*/
uint32_t IO_Joystick_IsActive( const t_Joystick* Joystick );

/*************************************************************************/
/* Protection against multiple includes ends here ! Stop editing here !  */
/*************************************************************************/
#endif  // #ifndef IO_JOYSTICK_V85_B25_H
