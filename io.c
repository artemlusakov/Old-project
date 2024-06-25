/****************************************************************************
*
* File:         IO.c
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

#include "gsemcm.h"
#include "io_joystick_v85_b25.h"
#include "io_types.h"
#include <config/libconfig.h>
#include "control.h"
#include "io.h"
#include "param.h"
/****************************************************************************/

/* macro definitions ********************************************************/

/* type definitions *********************************************************/
#define MAPPINGFILE "/gs/data/map.cfg"
/* prototypes ***************************************************************/
typedef struct tagIO_MCM_Device {
	uint8_t CAN;
	uint8_t Ext;
	uint32_t TX_ID;
	uint32_t RX_ID;
	const char* name;
} tIO_MCM_Device;

//Data to initialize MCM-Modules
tIO_MCM_Device MCM250[] =
{
  {.CAN = CAN0, .Ext = BIT11, .RX_ID = 25, .TX_ID = 24, .name = "MCM250_1"}, //0x18
  {.CAN = CAN0, .Ext = BIT11, .RX_ID = 27, .TX_ID = 26, .name = "MCM250_2"}, //0x1A
  {.CAN = CAN0, .Ext = BIT11, .RX_ID = 29, .TX_ID = 28, .name = "MCM250_3"},
  {.CAN = CAN0, .Ext = BIT11, .RX_ID = 31, .TX_ID = 30, .name = "MCM250_4"},
  {.CAN = CAN0, .Ext = BIT11, .RX_ID = 33, .TX_ID = 32, .name = "MCM250_5"},
  {.CAN = CAN0, .Ext = BIT11, .RX_ID = 35, .TX_ID = 34, .name = "MCM250_6"},
  {.CAN = CAN0, .Ext = BIT11, .RX_ID = 37, .TX_ID = 36, .name = "MCM250_7"},
  {.CAN = CAN0, .Ext = BIT11, .RX_ID = 39, .TX_ID = 38, .name = "MCM250_8"},
};

tIO_MCM_Device K2_4x3 = {
  .CAN = CAN0, .Ext = BIT11, .RX_ID = 35, .TX_ID = 34, .name = "K2_4x3" //tx = 0x23, rx = 0x22
  // .CAN = CAN0, .Ext = BIT11, .RX_ID = 37, .TX_ID = 36, .name = "K2_plough"
};
tIO_MCM_Device K2_plough = {
  .CAN = CAN0, .Ext = BIT11, .RX_ID = 37, .TX_ID = 36, .name = "K2_plough"
};

t_Joystick _Joystick = {
  .id = 0x33,
  .can = CAN0,
  .ext = BIT11,
};

static int32_t Birghtness = 0;
/* global constants *********************************************************/

/* global variables *********************************************************/

/* local function prototypes*************************************************/

/****************************************************************************/

/* function code ************************************************************/
typedef struct tagIO_Di {
	uint32_t* dest;
	uint32_t MCM_Device;
	uint32_t idx;
} tIO_DI;

typedef struct tagIO_Ai {
	uint32_t* dest;
	uint32_t MCM_Device;
	uint32_t idx;
} tIO_AI;

/**
* @brief This function sets the Brightness of the Displaybacklight and the Keyboards
*
*/
void IO_SetBrightness( void ) {
	SetDisplayBacklight( 0, Birghtness * 10 );
	SetKeyBacklight( 0, GS_KEY_BACKLIGHT_BRIGHTNESS( Birghtness / 3/*2*/ ) | 1 ); // AK not tested
	//Switch on LEDs
	/*for(int32_t i = 0; i < 12; i++)
	{
	  GSeMCM_SetKeyBacklight_White(K2_4X3, i+1,1 );
	  GSeMCM_SetKeyBacklight_White(K2_PLOUGH, i+1,1 );
	}
	//Set Brightnesss of the colors.
	// Only white is used with the current keyboard.
	GSeMCM_SetKeyBacklight_Brightness(K2_4X3,GSeMCM_LED_WHITE, Birghtness/2);
	GSeMCM_SetKeyBacklight_Brightness(K2_PLOUGH,GSeMCM_LED_WHITE, Birghtness/2);
	//GSeMCM_SetKeyBacklight_Brightness(K2_4X3,GSeMCM_LED_RED, Birghtness);
	//GSeMCM_SetKeyBacklight_Brightness(K2_4X3,GSeMCM_LED_GREEN, Birghtness);
  */
}

uint32_t IO_Init( tControl* Ctrl ) {

	//Add MCM-Modules
	for( uint32_t i = 0; i <= MCM250_8; i++ ) {
		GSeMCM_InitDevice( MCMTYPE_MCM250, MCM250[i].CAN, MCM250[i].Ext, MCM250[i].TX_ID, MCM250[i].RX_ID );
		GSeMCM_SetPolling( i, 500 );
//		db_out( "Init MCM %d\r\n", i );
	}
	//Add Keyboard
	/* GSeMCM_InitDevice(MCMTYPE_GSK_K2_4x3, K2_4x3.CAN, K2_4x3.Ext, K2_4x3.TX_ID, K2_4x3.RX_ID);
	 GSeMCM_InitDevice(MCMTYPE_GSK_K2_4x3, K2_plough.CAN, K2_plough.Ext, K2_plough.TX_ID, K2_plough.RX_ID);
	 //Set Default brightness of Display and Keyboard
	 Birghtness = GetVarIndexed(IDX_BRIGHTNESS);
	 IO_SetBrightness();*/

	 //Inits the Joystick
	const tParamData* Param = ParameterGet();
	_Joystick.id = Param->Joystick.Id;
	_Joystick.id2 = Param->Joystick.Id2;
	IO_Joystick_init( &Ctrl->joystick, _Joystick.id,_Joystick.id2, _Joystick.can, _Joystick.ext );
	return 0;
}

uint32_t IO_Cycle( tControl* Ctrl, uint32_t evtc, tUserCEvt* evtv ) {

	IOT_Cycle();
	//Read MCM-Modules
	GSeMCM_Cycle();
	//Read Joystick
	IO_Joystick_Cycle( &Ctrl->joystick );

	//Check change of brightness
	if( Birghtness != GetVarIndexed( IDX_BRIGHTNESS ) ) {
		Birghtness = GetVarIndexed( IDX_BRIGHTNESS );
		//Change brightness;
		IO_SetBrightness();
	}
	return 0;
}
