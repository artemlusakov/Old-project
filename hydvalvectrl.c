/****************************************************************************
*
* File:         HYDVALVCTRL.c
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
#include "hydvalvectrl.h"
#include "sql_table.h"
#include "control.h"
#include "param.h"
/****************************************************************************/

/* macro definitions ********************************************************/
#define PVE_OFFSET (-40)
/* type definitions *********************************************************/

/* prototypes ***************************************************************/

/* global constants *********************************************************/

/* global variables *********************************************************/

tSQLTable OMFB_Table = {
	.TableName = "OMFB_Valves",
	.ColumnNames = {"Liters", "100 l/min", "80 l/min",  "EMPTY"},
	.Cell = {
		{0, 0,  0, 0},
		{10000, 300,  350, 0},
		{20000, 330,  380, 0},
		{30000, 360,  410, 0},
		{40000, 390,  440, 0},
		{50000, 420,  470, 0},
		{60000, 450,  500, 0},
		{70000, 480,  530, 0},
		{80000, 510,  560, 0},
		{90000, 540,  590, 0},
		{100000, 1000,  1000, 0},
}	};

tSQLTable RotateML_Table = {
	.TableName = "RotateML",
	.ColumnNames = {"Liters", "PWM PVG16", "PWM PVG32",  "PWM OMFB"},
	.Cell = {
		{-65000, 200,  350, 200},
		{-50000, 246,  375, 246},
		{-30000, 264,  400, 264},
		{-20000, 280,  425, 280},
		{-1, 450,  450, 0},
		{0, 500,  500, 0},
		{1, 550,  550, 0},
		{20000, 630,  575, 630},
		{30000, 644,  600, 644},
		{50000, 660,  625, 660},
		{65000, 750,  650, 800},
}	};

tSQLTable PVE_Table = {
	.TableName = "PVE_Valves",
	.ColumnNames = {"Liters", "PWM PVG16", "PWM PVG32",  "PWM OMFB"},
	.Cell = {
		{-65000, 200,  350, 200},
		{-40000, 246,  375, 246},
		{-30000, 264,  400, 264},
		{-20000, 280,  425, 280},
		{-1, 450,  450, 450},
		{0, 500,  500, 500},
		{1, 550,  550, 550},
		{20000, 630,  575, 630},
		{30000, 644,  600, 644},
		{40000, 660,  625, 660},
		{65000, 750,  650, 750},
}	};

tSQLTable Prop12V_Table = {
	.TableName = "Prop12V_Valves",
	.ColumnNames = {"Percent", "Empty_0", "PWM",  "EMPTY"},
	.Cell = {
		{0, 0,  0,   0},
		{10, 0,  25,  0},
		{20, 0,  50,  0},
		{30, 0,  75,  0},
		{40, 0,  100, 0},
		{50, 0,  125, 0},
		{60, 0,  150, 0},
		{70, 0,  175, 0},
		{80, 0,  200, 0},
		{90, 0,  225, 0},
		{100, 0,  250, 0},
}	};
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

void CtrlSetDigitalOut(uint8_t MCM, int8_t Out, uint8_t value) {
	if(GetVar(HDL_IO_MODECTRL)) {
		// Пустое условие необходимо для того, чтобы в ручном режиме
		// не отправлялись сигналы на МСМ.
	} else if( GetVarIndexed( IDX_OILLEVEL ) > 0  && !GetVarIndexed(IDX_EMCY_ON)) {
		GSeMCM_SetDigitalOut(MCM, Out, value);
	}	else {
		GSeMCM_SetDigitalOut(MCM, Out, 0);
	}
}

int32_t _Valve_GetPWMOutput( uint32_t DO_Output ) {
	if( 0 == (DO_Output % 2) ) //only uneven outputs are PWM outputs.
	{
		return -1;
	}

	int32_t out = (DO_Output - 1) / 2;

	if( out > 3 ) //maxmum = 3 PWM-Outputs
	{
		return -1;
	}
	return out;
}

void ValveProp_Init( tValveProp* valv, uint32_t MCM_Device, uint32_t Output, uint32_t freq, uint32_t wobble ) {
	valv->MCM_Device = MCM_Device;
	valv->freq = freq;
	valv->duty = 500;
	valv->output = Output;
	valv->wobble = wobble;
	valv->wobble_cnt = 0;
	int32_t output = _Valve_GetPWMOutput( valv->output );
	GSeMCM_SetPWMOut( valv->MCM_Device, output, valv->duty, valv->freq, 1 );
}

void ValveProp_NoPort_Init( tValveProp* valv, uint32_t MCM_Device, uint32_t Output ) {
	valv->MCM_Device = MCM_Device;
	valv->freq = 200;
	valv->duty = 0;
	valv->output = Output;
	valv->wobble = 10;
	valv->wobble_cnt = 0;
	int32_t output = _Valve_GetPWMOutput( valv->output );
	GSeMCM_SetPWMOut( valv->MCM_Device, output, valv->duty, valv->freq, 1 );
}

void ValveProp12V_Init( tValv* valv, int32_t MCM_Device, uint32_t Output, uint32_t freq, uint32_t wobble ) {
	if(MCM_Device >= 0) {
		valv->Type = VALVE_TYPE_PROP_12V;
		valv->Valve.PVEH.MCM_Device = MCM_Device;
		valv->Valve.PVEH.freq = freq;
		valv->Valve.PVEH.duty = 0;
		valv->Valve.PVEH.output = Output;
		valv->Valve.PVEH.wobble = wobble;
		valv->Valve.PVEH.wobble_cnt = 0;
	} else {
		valv->Type = VALVE_TYPE_NONE;
	}
}

void ValvProp_25_50_75_Set(tValveProp * valv, int32_t value) {
  const int32_t offset = PVE_OFFSET; // offset to eliminate offset voltage.
  const int32_t range = 1000 - offset;

  if (value < 0) {
    value = 450 + value * 2 / 10;
  } else if (value > 0) {
    value = 550 + value * 2 / 10;
  } else value = 500;

  value = value * range / 1000;
  valv -> duty = value + offset;

  int32_t output = _Valve_GetPWMOutput(valv -> output);

  if (GetVar(HDL_IO_MODECTRL)) {
    // Пустое условие необходимо для того, чтобы в ручном режиме
    // не отправлялись сигналы на МСМ.
  } else if (GetVarIndexed(IDX_OILLEVEL) > 0 && !GetVarIndexed(IDX_EMCY_ON)) {
    GSeMCM_SetPWMOut(valv -> MCM_Device, output, valv -> duty, valv -> freq, 1);
  } else {
    GSeMCM_SetPWMOut(valv -> MCM_Device, output, 0, valv -> freq, 1);
  }
}

void ValvProp_PVG32_25_50_75_Set(tValveProp * valv, int32_t value) {
  if (value < 0) {
    value = (450 + value * 2 / 20) - 10;
  } else if (value > 0) {
    value = 550 + value * 2 / 20;
  } else value = 480;

  valv -> duty = value;

  int32_t output = _Valve_GetPWMOutput(valv -> output);

  if (GetVar(HDL_IO_MODECTRL)) {
    // Пустое условие необходимо для того, чтобы в ручном режиме
    // не отправлялись сигналы на МСМ.
  } else if (GetVarIndexed(IDX_OILLEVEL) > 0 && !GetVarIndexed(IDX_EMCY_ON)) {
    GSeMCM_SetPWMOut(valv -> MCM_Device, output, valv -> duty, valv -> freq, 1);
  } else {
    GSeMCM_SetPWMOut(valv -> MCM_Device, output, 0, valv -> freq, 1);
  }
}

void ValvProp_OMFB_25_50_75_Set(tValveProp * valv, int32_t value) {
  valv -> duty = value;
  int32_t output = _Valve_GetPWMOutput(valv -> output);

  if (GetVar(HDL_IO_MODECTRL)) {
    // Пустое условие необходимо для того, чтобы в ручном режиме
    // не отправлялись сигналы на МСМ.
  } else if (GetVarIndexed(IDX_OILLEVEL) > 0 && !GetVarIndexed(IDX_EMCY_ON)) {
    if (value == 1) { // при получении 1 открывается Дискретный выход
    GSeMCM_SetDigitalOut(valv -> MCM_Device, valv -> output, 1);
  } else {
    GSeMCM_SetDigitalOut(valv -> MCM_Device, valv -> output, 0);
  }
    GSeMCM_SetPWMOut(valv -> MCM_Device, output, valv -> duty, valv -> freq, 1);
  } else {
    GSeMCM_SetPWMOut(valv -> MCM_Device, output, 0, valv -> freq, 1);
    GSeMCM_SetDigitalOut(valv -> MCM_Device, valv -> output, 0);
  }
}

void ValvProp_Set( tValveProp* valv, uint32_t value ) {
	if( valv->duty != value ) {
		valv->duty = value;

		int32_t output = _Valve_GetPWMOutput( valv->output );
		//GSeMCM_SetPWMOut(valv->MCM_Device,output,valv->duty, valv->freq,1);
		if(GetVar(HDL_IO_MODECTRL)) {
			// Пустое условие необходимо для того, чтобы в ручном режиме
			// не отправлялись сигналы на МСМ.
		} else if( GetVarIndexed( IDX_OILLEVEL ) > 0  && !GetVarIndexed(IDX_EMCY_ON)) {
			GSeMCM_SetPWMOut( valv->MCM_Device, output, valv->duty, valv->freq, 1 );
		}	else {
			GSeMCM_SetPWMOut( valv->MCM_Device, output, 0, valv->freq, 1 );
		}
		if( output == 0 ) {
			SetVar( HDL_SHIMCONVEYOR, valv->duty );
		}
		if( output == 1 ) {
			SetVar( HDL_SHIMSPREADERDISK, valv->duty );
		}
	}
}

int32_t ValvProp_Timer_100ms( tValveProp* valv ) {
	valv->wobble_cnt++;
	if( valv->wobble_cnt > 2 )
		valv->wobble_cnt = -1;
	int32_t out = valv->duty + valv->wobble_cnt * valv->wobble;
	if( out < 0 ) out = 0;
	if( out > 1000 ) out = 1000;
	int32_t output = _Valve_GetPWMOutput( valv->output );
	if( output < 0 )
		return -1;
	if(GetVar(HDL_IO_MODECTRL)) {
		// Пустое условие необходимо для того, чтобы в ручном режиме
		// не отправлялись сигналы на МСМ.
	} else if( GetVar( HDL_OILLEVEL ) > 0  && !GetVarIndexed(IDX_EMCY_ON)) {
		GSeMCM_SetPWMOut( valv->MCM_Device, output, valv->duty, valv->freq, 1 );
	}
	else {
		GSeMCM_SetPWMOut( valv->MCM_Device, output, 0, valv->freq, 1 );
	}
	return 0;
}

void ValvDig_2_2_Init( tValvDig_2_2* Valv, uint8_t Device, uint8_t pos ) {
	Valv->MCM_Device = Device;
	Valv->pos = pos;
	Valv->val = 0;
}

void ValvDig_2_2_Set( tValvDig_2_2* Valv, uint8_t value ) {
	if( NULL == Valv )
		return;
	if( value != Valv->val ) {
		Valv->ChangeTime = GetMSTick();
	}
	Valv->val = value;
	if(GetVar(HDL_IO_MODECTRL)) {
		// Пустое условие необходимо для того, чтобы в ручном режиме
		// не отправлялись сигналы на МСМ.
	} else if( GetVarIndexed( IDX_OILLEVEL ) > 0  && !GetVarIndexed(IDX_EMCY_ON)) {
		GSeMCM_SetDigitalOut( Valv->MCM_Device, Valv->pos, value );
	}
	else {
		GSeMCM_SetDigitalOut( Valv->MCM_Device, Valv->pos, 0 );
	}
}

uint32_t ValvDig_2_2_CheckState( tValvDig_2_2* Valv ) {
	if( Valv == NULL )
		return -1;
	if( 500 < GetMSTick() - Valv->ChangeTime ) {
		if( Valv->val ) {
			if( 20 >= GSeMCM_GetAnalogIn( Valv->MCM_Device, Valv->pos + 4 ) ) {
				return -1;
			}
			else return 1;
		}
		else {
			if( 20 < GSeMCM_GetAnalogIn( Valv->MCM_Device, Valv->pos + 4 ) ) {
				return -1;
			}
			else return 0;
		}

	}
	return Valv->val;
}

int32_t ValvDig_2_2_GetState( const tValvDig_2_2* Valv ) {
	return Valv->val;
}

int32_t ValvDig_GetState( const tValv* Valv ) {
	if( Valv->Valve.PVEO[0].val ) {
		return -1;
	}
	else if( Valv->Valve.PVEO[1].val ) {
		return 1;
	}
	else return 0;
}

void Valv_Init(tValv * Valv, int32_t Device1, int32_t Pos1, int32_t Device2, int32_t Pos2) {
  if (Device1 == -1) {
    Valv->Type = VALVE_TYPE_NONE;
  } else if (Device2 == -1) {
		Valv->Type = VALVE_TYPE_PVE_AHS;
    ValveProp_Init( & Valv -> Valve.PVEH, Device1, Pos1, 200, 10);
  } else {
    Valv -> Type = VALVE_TYPE_PVE_O;
    ValvDig_2_2_Init( & Valv -> Valve.PVEO[0], Device1, Pos1);
    ValvDig_2_2_Init( & Valv -> Valve.PVEO[1], Device2, Pos2);
  }
  Valv -> state_old = 0;
}

void Valv_PVG32_Init(tValv * Valv, int32_t Device1, int32_t Pos1, int32_t Device2, int32_t Pos2) {
  if (Device1 == -1) {
    Valv->Type = VALVE_TYPE_NONE;
  } else if (Device2 == -1) {
    Valv -> Type = VALVE_TYPE_PVE_AHS32;
    ValveProp_Init( & Valv -> Valve.PVEH, Device1, Pos1, 200, 10);
  } else {
    Valv -> Type = VALVE_TYPE_PVE_O;
    ValvDig_2_2_Init( & Valv -> Valve.PVEO[0], Device1, Pos1);
    ValvDig_2_2_Init( & Valv -> Valve.PVEO[1], Device2, Pos2);
  }
  Valv -> state_old = 0;
}

void Valv_OMFB_Init(tValv * Valv, int32_t Device1, int32_t Pos1, int32_t Device2, int32_t Pos2) {
  if (Device1 == -1) {
    Valv->Type = VALVE_TYPE_NONE;
  } else if (Device2 == -1) {
    Valv -> Type = VALVE_TYPE_OMFB;
    ValveProp_Init( & Valv -> Valve.PVEH, Device1, Pos1, 200, 10);
  } else {
    Valv -> Type = VALVE_TYPE_PVE_O;
    ValvDig_2_2_Init( & Valv -> Valve.PVEO[0], Device1, Pos1);
    ValvDig_2_2_Init( & Valv -> Valve.PVEO[1], Device2, Pos2);
  }
  Valv -> state_old = 0;
}

void Valv_Set(tValv * Valv, int32_t value) {
  Valv -> state_old = Valv_GetState(Valv);
  if (Valv -> Type == VALVE_TYPE_PVE_O) {
    if (value < 0) {
      ValvDig_2_2_Set( & Valv -> Valve.PVEO[0], 1);
      ValvDig_2_2_Set( & Valv -> Valve.PVEO[1], 0);
    } else if (value > 0) {
      ValvDig_2_2_Set( & Valv -> Valve.PVEO[0], 0);
      ValvDig_2_2_Set( & Valv -> Valve.PVEO[1], 1);
    } else {
      ValvDig_2_2_Set( & Valv -> Valve.PVEO[0], 0);
      ValvDig_2_2_Set( & Valv -> Valve.PVEO[1], 0);
    }
  } else if (Valv -> Type == VALVE_TYPE_PVE_AHS) {
    ValvProp_25_50_75_Set( & Valv -> Valve.PVEH, value);
  } else if (Valv -> Type == VALVE_TYPE_PVE_AHS32) {
    ValvProp_PVG32_25_50_75_Set( & Valv -> Valve.PVEH, value);
  } else if (Valv -> Type == VALVE_TYPE_DISK) {
    if (value)
      ValvDig_2_2_Set( & Valv -> Valve.PVEO[0], 1);
    else {
      ValvDig_2_2_Set( & Valv -> Valve.PVEO[0], 0);
    }
  } else if (Valv -> Type == VALVE_TYPE_PROP_12V) {
    int32_t PWM = TableGetLSEValue( & Prop12V_Table, 0, 2, value);
    ValvProp_Set( & Valv -> Valve.PVEH, PWM);
  } else if (Valv -> Type == VALVE_TYPE_OMFB) {
    ValvProp_OMFB_25_50_75_Set( & Valv -> Valve.PVEH, value);
  }
}

int32_t ValvPVE_CheckState( const tValveProp* Valv ) {
	if( Valv->duty < (450 + PVE_OFFSET / 2) )
		return PVE_PORT_A;
	else if( Valv->duty > ( 550 + PVE_OFFSET / 2 ) )
		return PVE_PORT_B;
	else
		return PVE_PORT_NONE;
}

void Valv_Timer_100ms( tValv* Valv ) {
	if( Valv->Type == VALVE_TYPE_PVE_AHS ) {
		ValvProp_Timer_100ms( &Valv->Valve.PVEH );
	}
}

int32_t Valv_GetState( const tValv* Valv ) {
	switch( Valv->Type ) {
	case VALVE_TYPE_PVE_O:
	case VALVE_TYPE_DISK:
		return ValvDig_GetState( Valv );
		break;
	case VALVE_TYPE_PVE_AHS:
		return ValvPVE_CheckState( &Valv->Valve.PVEH );
	default:
		return 0;
		break;
	}
	return 0;
}

int32_t Valv_GetValue( const tValv* Valv ) {
	switch( Valv->Type ) {
	case VALVE_TYPE_PVE_O:
	case VALVE_TYPE_DISK:
		return Valv->Valve.PVEO[0].val;
		break;
	case VALVE_TYPE_PVE_AHS:
		return Valv->Valve.PVEH.output;
	default:
		return 0;
		break;
	}
	return 0;
}

int32_t Valv_GetPos( const tValv* Valv ) {
	switch( Valv->Type ) {
	case VALVE_TYPE_PVE_O:
	case VALVE_TYPE_DISK:
		return Valv->Valve.PVEO[0].val;
		break;
	case VALVE_TYPE_PVE_AHS:
		return Valv->Valve.PVEH.duty;
	case VALVE_TYPE_PVE_AHS32:
		return Valv->Valve.PVEH.duty;
	default:
		return -1;
		break;
	}
	return -1;
}

void ValvDisk_Init( tValv* Valv, int32_t Device, int32_t Pos ) {
	if( Device < 0 ) {
		Valv->Type = VALVE_TYPE_NONE;
	}
	else {
		Valv->Type = VALVE_TYPE_DISK;
	}
	Valv->Valve.PVEO[0].pos = Pos;
	Valv->Valve.PVEO[0].MCM_Device = Device;
	Valv->Valve.PVEO[0].val = 0;
}

tSQLTable* pPVE_Tab;
tSQLTable* RotateML_Tab;

void GlobalInitSqlTableLiter( void ) {
	pPVE_Tab = SQL_Tabel_Init( &PVE_Table );
	RotateML_Tab = SQL_Tabel_Init( &RotateML_Table );
}

void Ctrl_Valve_SetLiter(tValveProp* valv, int32_t ml) {
	const tControl* Ctrl = CtrlGet();
	const tParamData * Param = ParameterGet();
	static int32_t PWM = 0;
	switch (Ctrl->top.SummerTop.Pump.Valv.Type)	{
	case VALVE_TYPE_DISK:
		if(ml) {
			Valv_Set( & Ctrl->top.SummerTop.Pump.Valv, 1);
		} else {
			Valv_Set( & Ctrl->top.SummerTop.Pump.Valv, 0);
		}
		break;
	case VALVE_TYPE_PVE_AHS:
		PWM = TableGetLSEValue( pPVE_Tab, 0, 1, ml );
		ValvProp_Set( valv, PWM );
		break;
	case VALVE_TYPE_PVE_AHS32:
		PWM = TableGetLSEValue( pPVE_Tab, 0, 2, ml );
		ValvProp_Set( valv, PWM );
		break;
	case VALVE_TYPE_OMFB:
		if(ml) {
			if(Ctrl->top.SummerTop.Pump.Type == WATERPUMP_TYPE_BP300) {
				PWM = Param->OMFB.PWM_BP300;
				ValvProp_Set( valv, PWM );
			} else if (Ctrl->top.SummerTop.Pump.Type == WATERPUMP_TYPE_C610H) {
				PWM = Param->OMFB.PWM_C610H;
				ValvProp_Set( valv, PWM );
			}
		} else {
			ValvProp_Set( valv, 0 );
		}
		break;
	default:
		break;
	}
}

void Ctrl_IntakeValve_SetLiter(tValveProp * valv, int32_t ml) {
  const tControl * Ctrl = CtrlGet();
  const tParamData * Param = ParameterGet();
  static int32_t PWM = 0;
  switch (Ctrl -> top.SummerTop.PumpIntake.ValvIntake.Type) {
  case VALVE_TYPE_DISK:
    if (ml) {
      Valv_Set( & Ctrl -> top.SummerTop.Pump.Valv, 1);
    } else {
      Valv_Set( & Ctrl -> top.SummerTop.Pump.Valv, 0);
    }
    break;
  case VALVE_TYPE_PVE_AHS:
    PWM = TableGetLSEValue(pPVE_Tab, 0, 1, ml);
    ValvProp_Set(valv, PWM);
    break;
  case VALVE_TYPE_PVE_AHS32:
    PWM = TableGetLSEValue(pPVE_Tab, 0, 2, ml);
    ValvProp_Set(valv, PWM);
    break;
  case VALVE_TYPE_OMFB:
    if (ml) {
      PWM = Param -> OMFB.PWM_C610H;
      ValvProp_Set(valv, PWM);
    } else {
      ValvProp_Set(valv, 0);
    }
    break;
  default:
    break;
  }
}

void ValvPropPVEP_SetLiter( tValveProp* valv, int32_t ml ) {
	int32_t PWM = TableGetLSEValue( pPVE_Tab, 0, 1, ml );
	ValvProp_Set( valv, PWM );
}

void ValvPropPVEP_PVG32_SetLiter( tValveProp* valv, int32_t ml ) {
	int32_t PWM = TableGetLSEValue( pPVE_Tab, 0, 2, ml );
	ValvProp_Set( valv, PWM );
}

int32_t Rotate_OMFB_SetLiter(int32_t ml ) {
	int32_t PWM = TableGetLSEValue( RotateML_Tab, 0, 3, ml );
	return PWM;
}
