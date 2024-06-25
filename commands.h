/************************************************************************
*
* File:         COMMANDS.h
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
#ifndef COMMANDS_H
#define COMMANDS_H

#include	<stdint.h>

/*************************************************************************/

/* macro definitions *****************************************************/
#define FLOAT_OFF 0
#define FLOAT_ON 1
#define FLOAT_BREAK 2

/* type definitions ******************************************************/
/* prototypes ************************************************************/

/* global constants ******************************************************/

/* global variables ******************************************************/

/* global function prototypes ********************************************/
typedef struct tagJSCommands {
	int8_t Active;
	int32_t Updown;
	int32_t LeftRight;
	int32_t X;
	int32_t Y;
	int32_t Z;
	int8_t X_Change;
	int8_t Y_Change;
	int8_t Z_Change;
	int8_t F_New;
	int8_t G_New;
	int8_t Float;
	int8_t FloatTilt;
	int8_t Boost;
	int8_t E_L;
	int16_t E_Time;
	int8_t E_S;
	int8_t D_L;
	int8_t D_S;
} tJSCommands;

void Ctrl_JS_Cmd_SetCommands( tJSCommands* JS_Cmd, int32_t ActiveButtonPressed, uint8_t Equip );
void Ctrl_JS_Buzzer( void );
/*************************************************************************/
/* Protection against multiple includes ends here ! Stop editing here !  */
/*************************************************************************/
#endif // #ifndef COMMANDS_H
