/****************************************************************************
*
* File:         J1939.c
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
#include "bnso.h"
#include "userj1939.h"
#include "param.h"
#include "control.h"
#include "j1939.h"
#include "profile.h"
/****************************************************************************/

/* macro definitions ********************************************************/

/* type definitions *********************************************************/
#define J1939_THIS_DEVICE_ID 0x81
/* prototypes ***************************************************************/

void Can_SendProfile(void);

typedef enum eagEngineType {
	ENGINE_TYPE_UNKNOWN = 0,
	ENGINE_TYPE_DEFAULT,
	ENGINE_TYPE_J1939,
} eEngineType;

eEngineType EngineType;

typedef struct tagPGN_F004 {
	unsigned int res_0 : 24;
	unsigned int EngineSpeed_LB : 8; //SPN 190
	unsigned int EngineSpeed_HB : 8; //SPN 190
	unsigned int res_1 : 24;
} tPGN_F004;

typedef struct tagPGN_F005 {
	uint8_t SelectedGear; //SPN 524
	uint8_t res_0;
	uint16_t res_1;
	uint32_t res_2;
} tPGN_F005;

//Air Supply Pressure
typedef struct tagPGN_FEAE {
	uint8_t PneumaticSupplyPressure; //SPN 46
	uint8_t res_0;
	uint8_t ServicePracePressure_1; //1087
	uint8_t ServicePracePressure_2; //1088
	uint32_t res_1;
} tPGN_FEAE;

//Engine Temperature 1
typedef struct tagPGN_FEEE {
	uint8_t EngineCoolantTemp;	//SPN 110
	uint8_t EngineFuelTemp;		//SPN 174
	uint16_t EngineOilTemp;	//SPN 175
	uint32_t res_0;
} tPGN_FEEE;

//Engine Fluid Level/Pressure 1
typedef struct tagPGN_FEEF {
	uint16_t res_0;
	uint8_t OilLevel; //SPN 98
	uint8_t OilPressure; //SPN 100
	uint16_t res_1;
	uint8_t CoolantPressure; //SPN 109
	uint8_t CoolantLevel; //SPN 111
} tPGN_FEEF;

//Ambient Conditions
typedef struct tagPGN_FEF5 {
	uint16_t res_0;
	uint8_t res_1;
	uint8_t AmbientAirTemp_LB; //SPN 171
	uint8_t AmbientAirTemp_HB; //SPN 171
	uint8_t res_2;
	uint16_t res_3;
} tPGN_FEF5;

//speed
typedef struct tagPGN_FEF1 {
	unsigned int res_0 : 8;
	unsigned int WheelSpeed : 16;
	unsigned int res_1 : 24 /*8*/;
	unsigned int PTOstate : 8; //AK NOT TESTED actually size is 5
	unsigned int res_2 : 8 /*32*/; //actually size is 11
} tPGN_FEF1;

//speed
typedef struct tagPGN_FE6C {
	unsigned int res_0 : 16;
	unsigned int res_1 : 16;
	unsigned int PTOstate : 16;
	unsigned int WheelSpeed : 16;
} tPGN_FE6C;

/* global variables *********************************************************/

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

tPG PG_FF01;
tPG PG_FF02;
tPG PG_FF03;

tPG PG_F004;
tPG PG_F005;
tPG PG_FEAE;
tPG PG_FEEE;
tPG PG_FEEF;
tPG PG_FEF5;
tPG PG_FEF1;
tPG PG_FE6C;
tPG PG_FF10;

int32_t JFifo = 0;
//PGNs for BNSO receive
tPGN_FF01 PGN_FF01;
tPGN_FF02 PGN_FF02;
tPGN_FF06 PGN_FF06;
tPGN_FF07 PGN_FF07;

//RPM OF ENGINE
tPGN_F004 PGN_F004;

//selected gear
tPGN_F005 PGN_F005;

//Air pressure
tPGN_FEAE PGN_FEAE;

//OIL TEMPERATURE OF ENGINE
//COOLING TEMPERATURE OF ENGINE
tPGN_FEEE PGN_FEEE;

//COOLING PRESSURE OF ENGINE
//OIL PRESSURE OF ENGINE
tPGN_FEEF PGN_FEEF;

//AMBIENT TEMPERATURE
tPGN_FEF5 PGN_FEF5;
//SPEED
tPGN_FEF1 PGN_FEF1;
tPGN_FE6C PGN_FE6C;

tPGN_FF01* J1939_GetPGN_FF01( void ) {
	return &PGN_FF01;
}

tPGN_FF02* J1939_GetPGN_FF02( void ) {
	return &PGN_FF02;
}

tPGN_FF06* J1939_GetPGN_FF06( void ) {
	return &PGN_FF06;
}

tPGN_FF07* J1939_GetPGN_FF07( void ) {
	return &PGN_FF07;
}

void J1939_SendPGN( uint32_t id, void* data, size_t size_data ) {
	if( size_data > 8 ) {
		return;
	}
	tCanMessage msg;
	msg.channel = 1;
	msg.ext = 1;
	msg.id = id;
	msg.len = 8;
	msg.res = 0;
	msg.data.u32[0] = 0;
	msg.data.u32[1] = 0;
	memcpy( &msg.data, data, size_data );
	CANSendMsg( &msg );
}

void J1939_Engine_Standard( void ) {
}

void J1939_Init( uint32_t CAN, tControl* Ctrl ) {
	EngineType = ENGINE_TYPE_DEFAULT;
	JFifo = CANCreateFiFo( 256 );
	CANAddRxFilterFiFo( 1, 0, 0, 1, JFifo );
}

void J1939_SetData( int16_t* Destination, uint32_t Source, uint8_t SourceSize, float Factor, float Offset ) {
	uint32_t MaxSize = (1 << (SourceSize * 8)) - 1;
	if( Source < MaxSize ) {
		*Destination = ((double)Source * Factor) + Offset;
	}
}

uint32_t J1939_GetPGN( uint32_t id ) {
	int32_t pgn = id & 0xFFFF00;
	pgn = pgn >> 8;
	return pgn;
}

void J1939_Cycle( tEngine* Engine ) {
	const tParamData * Param = ParameterGet();
	//Set Data for BNSO
	if( EngineType == ENGINE_TYPE_DEFAULT ) {
		tCanMessage msg;
		static int32_t EngineTime = 10000;
		while( CANReadFiFo( JFifo, &msg, 1 ) ) {
			EngineTime = GetMSTick();
			switch( J1939_GetPGN( msg.id ) ) {
			case 0xF004:
				memcpy( &PGN_F004, &msg.data.u32[0], sizeof( msg.data ) );
				break;

			case 0xF005:
				memcpy( &PGN_F005, &msg.data.u32[0], sizeof( msg.data ) );
				break;

			case 0xFEAE:
				memcpy( &PGN_FEAE, &msg.data.u32[0], sizeof( msg.data ) );
				break;

			case 0xFEEE:
				memcpy( &PGN_FEEE, &msg.data.u32[0], sizeof( msg.data ) );
				break;

			case 0xFEEF:
				memcpy( &PGN_FEEF, &msg.data.u32[0], sizeof( msg.data ) );
				break;

			case 0xFEF5:
				memcpy( &PGN_FEF5, &msg.data.u32[0], sizeof( msg.data ) );
				break;

			case 0xFEF1:
				memcpy( &PGN_FEF1, &msg.data.u32[0], sizeof( msg.data ) );
				break;

			case 0xFE6C:
				memcpy( &PGN_FE6C, &msg.data.u32[0], sizeof( msg.data ) );
				break;

			case 0xFEC1:
				if(GetVarIndexed(IDX_PASSEDWAY) == 0) {SetVarIndexed(IDX_PASSEDWAY, msg.id);}
					if( msg.id == GetVarIndexed(IDX_PASSEDWAY)) {
					Engine->PassedWay = (msg.data.u8[0] | (msg.data.u8[1] << 8) |
          (msg.data.u8[2] << 16) | (msg.data.u8[3] << 24)) / (float) 200;
				}
			default:
				break;
			}
		}

		if( 1000 < (GetMSTick() - EngineTime) ) {
			Engine->Active = 0;
		}
		else Engine->Active = 1;

		uint16_t rpm = PGN_F004.EngineSpeed_LB | (PGN_F004.EngineSpeed_HB << 8);
		J1939_SetData( &Engine->rpm, rpm, sizeof( uint16_t ), 0.125, 0 );
		//Engine->AirPressure = PGN_FEAE.PneumaticSupplyPressure * 8;  //mBar
		//Engine->AirPressure = PGN_FEAE.ServicePracePressure_1 * 8;  //mBar
		J1939_SetData( &Engine->SelectedGear, PGN_F005.SelectedGear, sizeof( uint8_t ), 1, -125 );
		J1939_SetData( &Engine->AirPressure, PGN_FEAE.ServicePracePressure_2, sizeof( uint8_t ), 8, 0 ); //mBar
		if(!Param -> Speedometer.TypeSpeedMsg) {
			J1939_SetData( &Engine->speed, PGN_FEF1.WheelSpeed, sizeof( uint16_t ), (1.0 / 256.0), 0 ); //m/h
		} else {
			J1939_SetData( &Engine->speed, PGN_FE6C.WheelSpeed, sizeof( uint16_t ), (1.0 / 256.0), 0 ); //m/h
		}
		Engine->speedKDM = Engine->speed;

		J1939_SetData( &Engine->CoolentTemp, PGN_FEEE.EngineCoolantTemp, sizeof( uint8_t ), 1, -40 ); //В°C
		J1939_SetData( &Engine->FuelTemp, PGN_FEEE.EngineFuelTemp, sizeof( uint8_t ), 1, -40 ); //В°C
		J1939_SetData( &Engine->OilTemp, PGN_FEEE.EngineOilTemp, sizeof( uint16_t ), (1.0 / 32.0), -273 ); //В°C
		J1939_SetData( &Engine->OilLevel, PGN_FEEF.OilLevel, sizeof( uint8_t ), 0.4, 0 ); //0-100%
		J1939_SetData( &Engine->OilPressure, PGN_FEEF.OilPressure, sizeof( uint8_t ), 4, 0 );
		J1939_SetData( &Engine->CoolentLevel, PGN_FEEF.CoolantLevel, sizeof( uint8_t ), 0.4, 0 );
		J1939_SetData( &Engine->CoolentPressure, PGN_FEEF.CoolantPressure, sizeof( uint8_t ), 2, 0 );
		int16_t AmbTemp = PGN_FEF5.AmbientAirTemp_LB | (PGN_FEF5.AmbientAirTemp_HB << 8);
		J1939_SetData( &Engine->AmbientAirTemp, AmbTemp, sizeof( uint16_t ), (1.0 / 32.0), -273 ); //В°C

		static int32_t timer = 0;
		static int32_t timer_1sec = 0;
		static int32_t timer_10sec = 0;
		if( 500 < (GetMSTick() - timer) ) {
			timer = GetMSTick();

			msg.id = 0x18FF0181;
			msg.channel = 1;
			msg.ext = 1;
			msg.len = 8;
			msg.res = GS_CAN_DATA_FRAME;
			memcpy( &msg.data.u32[0], &PGN_FF01, sizeof( msg.data ) );
			CANSendMsg( &msg );
			//
			msg.id = 0x18FF0281;
			memcpy( &msg.data.u32[0], &PGN_FF02, sizeof( msg.data ) );
			CANSendMsg( &msg );
			//
			msg.id = 0x18FF0781;
			memcpy( &msg.data.u32[0], &PGN_FF07, sizeof( msg.data ) );
			CANSendMsg( &msg );
		}

		if(1000 < (GetMSTick() - timer_1sec)) {
			timer_1sec = GetMSTick();

			msg.id = 0x18FF0681;
			msg.channel = 1;
			msg.ext = 1;
			msg.len = 8;
			msg.res = GS_CAN_DATA_FRAME;
			memcpy( &msg.data.u32[0], &PGN_FF06, sizeof( msg.data ) );
			CANSendMsg( &msg );
		}

		if(10000 < (GetMSTick() - timer_10sec)) {
			timer_10sec = GetMSTick();

			Can_SendProfile();
		}
	}
}
void VIN_SEND( void ){
	tCanMessage msg;
	const tParamData* Parameter = ParameterGet();
	int msgN = 1;

	msg.channel = 1;
	msg.ext = 1;
	msg.id =  0x18FF0981;
	msg.len = 8;
	msg.res = 0;

	int i = 0;
	for(; i < strlen(Parameter->VIN.VIN); i++) {
		msg.data.u8[i % 7 + 1] = Parameter->VIN.VIN[i];

		if(i % 7 == 6) {
			msg.data.u8[0] = msgN++;
			CANSendMsg( &msg );
		}
	}

	if(i % 7 != 6) {
		for(i = i % 7 + 1; i < 8; msg.data.u8[i++] = 0xFF)  {;}
		msg.data.u8[0] = msgN;
		CANSendMsg( &msg );
	}
}
void Can_SendProfile(void) {
	tCanMessage msg;
  tProfile * prof_ = Profile_GetCurrentProfile();
	uint32_t val_;
	ConfigGetInt("System.Version.User", &val_); //Version of User Project
	msg.id = 0x18FF0481;
	msg.channel = 1;
	msg.ext = 1;
	msg.len = 8;
	msg.res = 0;
	msg.data.u8[0] = prof_->module[1]; // Передняя навеска
	msg.data.u8[1] = prof_->module[2]; // Межосевая навеска
	msg.data.u8[2] = prof_->module[3]; // Боковая навеска
	msg.data.u8[3] = prof_->module[0]; // Основаная (top) навеска
	msg.data.u32[1] = val_;
	CANSendMsg( &msg );
}

void J1939_Timer( tControl* Ctrl ) {
	//Ctrl->engine.Active = UserJ1939_ECU_Active( Ctrl->engine.Hdl );
	Ctrl->BNSO.Active = UserJ1939_ECU_Active( Ctrl->BNSO.Hdl );
}
