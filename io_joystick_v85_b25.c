/****************************************************************************
*
* File:         IO_JOYSTICK_V85_B25.c
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
****************************************************************************/
#include <stdint.h>
#include <UserCEvents.h>  /* Events send to the Cycle function              */
#include <UserCAPI.h>     /* API-Function declarations                      */

#include "vartab.h"       /* Variable Table definition                      */
 /* include this file in every C-Source to access  */
 /* the variable table of your project             */
 /* vartab.h is generated automatically            */
#include "objtab.h"       /* Object ID definition                           */
						  /* include this file in every C-Source to access  */
						  /* the object ID's of the visual objects          */
						  /* objtab.h is generated automatically            */
#include "io_types.h"
#include "io_joystick_v85_b25.h"
#include "param.h"
/****************************************************************************/

/* macro definitions ********************************************************/

/* type definitions *********************************************************/

/* prototypes ***************************************************************/

/* global constants *********************************************************/

/* global variables *********************************************************/
int32_t FiFo_Joystick;

/* local function prototypes*************************************************/

/****************************************************************************/

/* function code ************************************************************/

/****************************************************************************
**
**    Function      :
**
**    Description   :
**
**
**
**    Returnvalues  : none
**
*****************************************************************************/

typedef enum eagJ1939_BOOL {
	JBOOL_FALSE,
	JBOOL_TRUE,
	JBOOL_INVALID,
	JBOOL_UNKNOWN,
}eJ1939_BOOL;

/**
* @brief Description of digital inputs
*
*/
const char* V85B25_DI_Description[] = {
  "A",
  "B",
  "C",
  "D",
  "E",
  "F",
  "G"
};

/**
* @brief Description of analog inputs
*
*/
const char* V85B25_AI_Description[] = {
  "Forward / Backward",
  "Left / Right",
  "X",
  "Y",
  "Z",
};

int16_t JS_InterpretJoyDataToPerMill( uint16_t data ) {
	int16_t value = 0;
	//GetDirection:
	if( 1 == (data & 0x01) ) {
		value = 0;
	}
	else if( 1 == ((data >> 2) & 0x01) ) {
		value = -1 * ((data >> 6) & 0x3FF); // Calc Value out of Position and direction
	}
	else if( 1 == ((data >> 4) & 0x01) ) {
		value = (data >> 6) & 0x3FF; // Calc Value out of Position and direction
	}
	return value;
}
int16_t JS_InterpretJoyDataToPerMill_Joy2( uint16_t data ,int8_t StartBit) {//For China joy
	int16_t value = 0;
	//GetDirection:
 if((data>>StartBit)&0x1){
	 value = 1;
 }else if((data>>StartBit+1)&0x1){
 value = -1;
 }else value = 0;
	return value;
}

/**
* @brief interprets a 2Bit-Value of J1939 and sets the destination to TRUE or FALSE
*  at a valid information
*
* @param Bit           DataByte of J1939
* @param StartByte     At which bit the informaiton is beginning
* @param Dest          Byte the result will be written in.. This values will be 0 or 1
*/
void JS_InterpretJoyDataToKeyPress( int8_t Byte, int8_t StartBit, uint8_t* Dest ) {
	int8_t value = (Byte >> StartBit) & 0x3;
	if( value == JBOOL_TRUE )
		*Dest = 1;
	else if( value == JBOOL_FALSE )
		*Dest = 0;
}
void JS_InterpretJoyDataToKeyPress_Joy2( int8_t Byte, int8_t StartBit, uint8_t* Dest ) {//For China joy
	int8_t value = (Byte >> StartBit) & 0x1;
	if( value == JBOOL_TRUE )
		*Dest = 1;
	else if( value == JBOOL_FALSE )
		*Dest = 0;
}
/**
* @brief returns the PGN out of a ID
*
* @param id ID of the J1939-Message
* @return int32_t PGN of the ID
*/
int32_t GetPGN( int32_t id ) {
	id = ((id >> 8) & 0x1FFFF);
	return id;
}

void IO_Joystick_Init_J1939( t_Joystick* Joystick ) {
	//Init analog keys
	IOT_AnaJoy_Init( &Joystick->UpDown, -1000, 0, 1000, 100 );
	IOT_AnaJoy_Init( &Joystick->LeftRight, -1000, 0, 1000, 100 );
	IOT_AnaJoy_Init( &Joystick->X, -1000, 0, 1000, 100 );
	IOT_AnaJoy_Init( &Joystick->Y, -1000, 0, 1000, 100 );
	IOT_AnaJoy_Init( &Joystick->Z, -1000, 0, 1000, 100 );
}

void IO_Joystick_InitCanOpen( t_Joystick* Joystick ) {

}

void IO_Joystick_init( t_Joystick* Joystick, const uint32_t id,const uint32_t id2, const uint32_t can, const uint32_t ext ) {
	Joystick->fifo = CANCreateFiFo( 265 );
	if( 0 > Joystick->fifo ) {
	}
	Joystick->id = id;
  Joystick -> id2 = id2;
	Joystick->can = can;
	Joystick->ext = ext;

	//Init Buttons
	for( int32_t i = 0; i < JSB_NUM_BUTTONS; i++ ) {
		IOT_Button_Init( &Joystick->Button[i], 500, 1500 ); //second value 2000 replaced with 1500
	}

	Joystick->time = 10000 + GetMSTick(); // start timeout in 10 seconds
	Joystick->Type = JS_TYPE_J1939;
	CANAddRxFilterFiFo( Joystick->can, Joystick->id, 0xFF, 1, Joystick->fifo );
  CANAddRxFilterFiFo(Joystick -> can, Joystick -> id2, 0xFF, 1, Joystick -> fifo);//For China joy
	IO_Joystick_Init_J1939( Joystick );
	char * name = "J1939";
	strncpy( Joystick->name, name, sizeof( Joystick->name ) );
}

void IO_Joystick_J1939_Cycle( t_Joystick* Joystick ) {
	tCanMessage msg;

	while( 1 == CANReadFiFo( Joystick->fifo, &msg, 1 ) ) {
		Joystick->time = GetMSTick(); // reset timer to detect, if device is active

      if (0xF0F1 == GetPGN(msg.id)) {//For China joy
			IOT_AnaJoy_Set_JoyPos( & Joystick -> UpDown, JS_InterpretJoyDataToPerMill_Joy2( msg.data.u8[0] ,0),msg.data.u8[1]); //"X"
			IOT_AnaJoy_Set_JoyPos( & Joystick -> LeftRight, -JS_InterpretJoyDataToPerMill_Joy2( msg.data.u8[0] ,2),msg.data.u8[2]); //"X"

      static uint8_t Key[7];

      //Read Data from CAN-Message
      JS_InterpretJoyDataToKeyPress_Joy2(msg.data.u8[7], 0, & Key[0]); //"A"
      JS_InterpretJoyDataToKeyPress_Joy2(msg.data.u8[7], 1, & Key[1]); //"B"
      JS_InterpretJoyDataToKeyPress_Joy2(msg.data.u8[7], 2, & Key[2]); //"C"
      JS_InterpretJoyDataToKeyPress_Joy2(msg.data.u8[7], 3, & Key[3]); //"D"
      JS_InterpretJoyDataToKeyPress_Joy2(msg.data.u8[7], 4, & Key[4]); //"E"
      JS_InterpretJoyDataToKeyPress_Joy2(msg.data.u8[7], 5, & Key[5]); //"F"

      //Set State for Buttons
      for (int32_t i = 0; i < JSB_NUM_BUTTONS; i++) {
        IOT_Button_SetDI( & Joystick -> Button[i], Key[i]);
      }
      // IOT_AnaJoy_Set( & Joystick -> X, -JS_InterpretJoyDataToPerMill(msg.data.u16[0])); //"X"
       IOT_AnaJoy_Set_Rocker( & Joystick -> Y, JS_InterpretJoyDataToPerMill_Joy2( msg.data.u8[3] ,0),msg.data.u8[4]); //"X"
       IOT_AnaJoy_Set_Rocker( & Joystick -> X, JS_InterpretJoyDataToPerMill_Joy2( msg.data.u8[3] ,2),msg.data.u8[5]); //"X"
       IOT_AnaJoy_Set_Rocker( & Joystick -> Z, -JS_InterpretJoyDataToPerMill_Joy2( msg.data.u8[3] ,4),msg.data.u8[6]); //"X"
      // IOT_AnaJoy_Set( & Joystick -> Y, JS_InterpretJoyDataToPerMill(msg.data.u16[1])); //"Y"
      // IOT_AnaJoy_Set( & Joystick -> Z, JS_InterpretJoyDataToPerMill(msg.data.u16[2])); //"Z"
    }

		if( 0xFDD6 == GetPGN( msg.id ) ) {
			IOT_AnaJoy_Set( &Joystick->LeftRight, JS_InterpretJoyDataToPerMill( msg.data.u16[0] ) );
			IOT_AnaJoy_Set( &Joystick->UpDown, JS_InterpretJoyDataToPerMill( msg.data.u16[1] ) );
			static uint8_t Key[7];
			//Read Data from CAN-Message
			JS_InterpretJoyDataToKeyPress( msg.data.u8[5], 0, &Key[3] ); //"A"
			JS_InterpretJoyDataToKeyPress( msg.data.u8[5], 2, &Key[2] ); //"B"
			JS_InterpretJoyDataToKeyPress( msg.data.u8[5], 4, &Key[1] ); //"C"
			JS_InterpretJoyDataToKeyPress( msg.data.u8[5], 6, &Key[0] ); //"D"
			JS_InterpretJoyDataToKeyPress( msg.data.u8[6], 4, &Key[5] ); //"E"
			JS_InterpretJoyDataToKeyPress( msg.data.u8[6], 6, &Key[4] ); //"F"
			JS_InterpretJoyDataToKeyPress( msg.data.u8[6], 2, &Key[6] ); //"G"
			//Set State for Buttons
			for( int32_t i = 0; i < JSB_NUM_BUTTONS; i++ ) {
				IOT_Button_SetDI( &Joystick->Button[i], Key[i] );
			}

		}
		else if( 0xFDD7 == GetPGN( msg.id ) ) {
			IOT_AnaJoy_Set( &Joystick->X, -JS_InterpretJoyDataToPerMill( msg.data.u16[0] ) ); //"X"
			IOT_AnaJoy_Set( &Joystick->Y, JS_InterpretJoyDataToPerMill( msg.data.u16[1] ) ); //"Y"
			IOT_AnaJoy_Set( &Joystick->Z, JS_InterpretJoyDataToPerMill( msg.data.u16[2] ) ); //"Z"

		}
	}
	if( 1000 < (GetMSTick() - Joystick->time) ) {
		Joystick->active = 0;
	}
	else {
		Joystick->active = 1;
	}
}


void IO_Joystick_Cycle( t_Joystick* Joystick ) {
	if( Joystick->Type == JS_TYPE_J1939 ) {
		IO_Joystick_J1939_Cycle( Joystick );
	}
}

uint32_t IO_Joystick_IsActive( const t_Joystick* Joystick ) {
	return Joystick->active;
}
