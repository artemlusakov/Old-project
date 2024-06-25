#include	<math.h>
#include	<stdint.h>
#include <UserCEvents.h>
#include <UserCAPI.h>
#include "vartab.h"
#include "objtab.h"
#include "io.h"
#include "io_types.h"
#include "io_joystick_v85_b25.h"
#include "gsemcm.h"
#include "hydvalvectrl.h"
#include "gse_msgbox.h"
#include "profile.h"
#include "RCText.h"
#include "gseerrorlist.h"
#include "errorlist.h"
#include "j1939.h"
#include "param.h"
#include "gsemaint.h"
#include "gsemaint_lifetime.h"
#include "ctrl_brush.h"
#include "commands.h"
#include "ctrl_plough.h"
#include "bnso.h"
#include "visu_statistik.h"
#include	"response_error.h"
#include	"pinout.h"
#include	"okb600.h"
#include "ctrl_top_summer_attachment.h"

/****************************************************************************/
const int8_t dev_ctrl = 0; // 0 - стандартный режим, 1 - режим разработчика
/****************************************************************************/

typedef struct tagCtrlIO_Buttons {
	tIOT_Button* Button;
	uint8_t Device;
	uint8_t idx;
} tCtrlIO_Buttons;

typedef struct tagCtrlIO_DOs {
	uint8_t* DO;
	uint8_t Device;
	uint8_t Idx;
	int8_t Status; // 00 - выключено, 01 - включено, 10 - ошибка, 11 - отсутствует
	int32_t Error;
	uint32_t ErrorText;
} tCtrlIO_DOs;

typedef struct tagCtrlIO_DIs {
	eDISensorState* DI;
	uint8_t Device;
	uint8_t idx;
} tCtrlIO_DIs;

typedef struct tagCtrlIO_AIs {
	tIOT_AI* AI;
	uint8_t Device;
	uint8_t idx;
} tCtrlIO_AIs;

typedef struct tagCtrlIO_Freqs {
	uint32_t* Freq;
	uint8_t Device;
	uint8_t idx;
} tCtrlIO_Freqs;


tControl CTRL;

tCtrlIO_Buttons MCM_StandardButtons[] = {
	{.Button = &CTRL.emcy,						.Device = MCM250_3,		.idx = 1},
	{.Button = &CTRL.emcy_2,					.Device = MCM250_6,		.idx = 1},
};

// Порядок не изменять. Новые элементы добавлять в конец
tCtrlIO_DOs MCM_StandardDOs[] = {
	{.DO = &CTRL.light.On.FrontBeaconCab,	.Device = MCM250_3,		.Idx = 6,	.Status = 0,	.Error = ERRID_CHECKFRONTBEACON,		.ErrorText = RCTEXT_T_CHECK_FRONTBEACON},
	{.DO = &CTRL.light.On.WorkSide,			.Device = MCM250_3,		.Idx = 4,	.Status = 0,	.Error = ERRID_CHECKWORKSIDE,			.ErrorText = RCTEXT_T_CHECK_WORKSIDE},
	{.DO = &CTRL.light.On.BrushLight,		.Device = MCM250_3,		.Idx = 0,	.Status = 0,	.Error = ERRID_CHECKBRUSHLIGHT,			.ErrorText = RCTEXT_T_CHECKBRUSHLIGHT},
	{.DO = &CTRL.light.On.OKB600_WorkRear,				.Device = MCM250_3, .Idx = 1, .Status = 0, .Error = ERRID_CHECKOKB600WORKREAR,        .ErrorText = RCTEXT_T_CHECKOKB600WORKREAR},
	// RTR
	{.DO = &CTRL.light.On.RTR_LeftRoadSign,	.Device = MCM250_4,		.Idx = 0,	.Status = 0,	.Error = ERRID_CHECKRTRLEFTROADSIGN,	.ErrorText = RCTEXT_T_CHECKRTRLEFTROADSIGN},
	{.DO = &CTRL.light.On.RTR_RightRoadSign,	.Device = MCM250_4,	.Idx = 2,	.Status = 0,	.Error = ERRID_CHECKRTRRIGHTROADSIGN,	.ErrorText = RCTEXT_T_CHECKRTRRIGHTROADSIGN},
	{.DO = &CTRL.light.On.RTR_WorkRear,		.Device = MCM250_4,		.Idx = 4,	.Status = 0,	.Error = ERRID_CHECKRTRWORKREAR,		.ErrorText = RCTEXT_T_CHECKRTRWORKREAR},
	{.DO = &CTRL.light.On.RTR_RearBeacon,	.Device = MCM250_4,		.Idx = 6,	.Status = 0,	.Error = ERRID_CHECKRTRREARBEACON,		.ErrorText = RCTEXT_T_CHECKRTRREARBEACON},
	{.DO = &CTRL.light.On.RTR_HopperTop,	.Device = MCM250_3,		.Idx = 7,	.Status = 0,	.Error = ERRID_CHECKRTRHOPPERTOP,		.ErrorText = RCTEXT_T_CHECKRTRHOPPERTOP},
	// LRS
	{.DO = &CTRL.light.On.LRS_LeftRoadSign,	.Device = MCM250_5,		.Idx = 0,	.Status = 0, .Error = ERRID_CHECKLRSLEFTROADSIGN,	.ErrorText = RCTEXT_T_CHECKLRSLEFTROADSIGN},
	{.DO = &CTRL.light.On.LRS_RightRoadSign,	.Device = MCM250_5,	.Idx = 2,	.Status = 0, .Error = ERRID_CHECKLRSRIGHTROADSIGN,	.ErrorText = RCTEXT_T_CHECKLRSRIGHTROADSIGN},
	{.DO = &CTRL.light.On.LRS_RearBeacon,	.Device = MCM250_5,		.Idx = 6,	.Status = 0, .Error = ERRID_CHECKLRSREARBEACON,		.ErrorText = RCTEXT_T_CHECKLRSREARBEACON},
	{.DO = &CTRL.light.On.LRS_WorkRear,		.Device = MCM250_6,		.Idx = 5,	.Status = 0,	.Error = ERRID_CHECKLRSWORKREAR,		.ErrorText = RCTEXT_T_CHECKLRSWORKREAR},
};

void CtrlI_StandardDOs_SetRearWorkLight_ForCR600( void ) {
	MCM_StandardDOs[6].Device = MCM250_5;
	MCM_StandardDOs[6].Idx = 7;
}

/**
 * @brief Инициализация датчиков
 */
tCtrlIO_DIs MCM_DIs[] = {
	{.DI = &CTRL.sens.DI.OilFilter ,		.Device = MCM250_1 ,	.idx = 0} ,
	{.DI = &CTRL.sens.DI.SelectedGear ,		.Device = MCM250_3 ,	.idx = 4} ,
	{.DI = &CTRL.sens.DI.ClutchSensor ,		.Device = MCM250_3 ,	.idx = 5} ,
	{.DI = &CTRL.sens.DI.UpperOilLevel ,	.Device = MCM250_1 ,	.idx = 1} ,
	{.DI = &CTRL.sens.DI.LowerOilLevel ,	.Device = MCM250_1 ,	.idx = 2} ,
	{.DI = &CTRL.sens.DI.CheckOilLevelSensor ,	.Device = MCM250_1 ,	.idx = 3} ,
	{.DI = &CTRL.sens.DI.Ignition ,			.Device = MCM250_3 ,	.idx = 2} ,
	{.DI = &CTRL.sens.DI.PTO_Active ,		.Device = MCM250_3 ,	.idx = 0} ,
	{.DI = &CTRL.sens.DI.Spreading ,		.Device = MCM250_4 ,	.idx = 0} ,
	{.DI = &CTRL.sens.DI.Humidifier ,		.Device = MCM250_4 ,	.idx = 5} ,
	{.DI = &CTRL.sens.DI.tail_pos ,			.Device = MCM250_4 ,	.idx = 2} ,
	{.DI = &CTRL.sens.DI.MaterialPresense ,	.Device = MCM250_4 ,	.idx = 2} ,
	{.DI = &CTRL.sens.DI.cr_pressure ,		.Device = MCM250_6 ,	.idx = CR_PIN_PRESS} ,
	{.DI = &CTRL.sens.DI.cr_metliq_err ,	.Device = MCM250_6 ,	.idx = CR_PIN_METLIQ_ERR} ,
	{.DI = &CTRL.sens.DI.MF500Bar ,	.Device = MCM250_2 ,	.idx = 0} ,
	{.DI = &CTRL.sens.DI.pump_airdry,		.Device = MCM250_5, .idx = 0},
};

tCtrlIO_AIs MCM_AIs[] = {
	{.AI = &CTRL.sens.AI.UsSens_1 ,			.Device = MCM250_7 ,	.idx = 0},
	{.AI = &CTRL.sens.AI.UsSens_2 ,			.Device = MCM250_7 ,	.idx = 1},
	{.AI = &CTRL.sens.AI.OilTemp ,			.Device = MCM250_1 ,	.idx = 0},
	{.AI = &CTRL.sens.AI.OilPressure ,		.Device = MCM250_1 ,	.idx = 1},
	{.AI = &CTRL.sens.AI.cr_dispens_pos ,	.Device = MCM250_6 ,	.idx = CR_PIN_DISPENS_POS},
	{.AI = &CTRL.sens.AI.cr_liqmeter ,		.Device = MCM250_6 ,	.idx = CR_PIN_LIQMETER},
};

tCtrlIO_Freqs MCM_FreqIn[] = {
	{.Freq = &CTRL.sens.Freq.ConveryRotation ,	.Device = MCM250_4 ,	.idx = 3},
	{.Freq = &CTRL.sens.Freq.humid_pump_rot ,	.Device = MCM250_4 ,	.idx = 1},
};

enum DeviceState {
	DEVICESTATE_MCM250_0,
	DEVICESTATE_MCM250_1,
	DEVICESTATE_MCM250_2,
	DEVICESTATE_MCM250_3,
	DEVICESTATE_MCM250_4,
	DEVICESTATE_K2_3X4,
	DEVICESTATE_K2_PLOUGH,
	DEVICESTATE_JOYSTICK,
	DEVICESTATE_ENGINE,
	DEVICESTATE_BNSO,
	DEVICESTATE_NUM
};

const tControl* CtrlGet( void ) {
	return &CTRL;
}
tControl* ctrl_get( void ) {
	return &CTRL;
}

void Ctrl_DeviceStates(tControl * Ctrl) {
  //Check MCM-Modules
  static struct {
    uint32_t DeviceStates;
    int32_t DeviceTemp;
    uint32_t MCM_Handle;
    eErrorID ErrID_Offline;
    eErrorID ErrID_Temp;
  }
	MCM_States[] = {
		//state temp device		Error ID offline		Error ID Temperature
		{0,		0,	MCM250_1,	ERRID_MCM250_0_OFFLINE,	ERRID_MCM250_0_TEMP},
		{0,		0,	MCM250_2,	ERRID_MCM250_1_OFFLINE,	ERRID_MCM250_1_TEMP},
		{0,		0,	MCM250_3,	ERRID_MCM250_2_OFFLINE,	ERRID_MCM250_2_TEMP},
		{0,		0,	MCM250_4,	ERRID_MCM250_3_OFFLINE,	ERRID_MCM250_3_TEMP},
		{0,		0,	MCM250_5,	ERRID_MCM250_4_OFFLINE,	ERRID_MCM250_4_TEMP},
		{0,		0,	MCM250_6,	ERRID_MCM250_5_OFFLINE,	ERRID_MCM250_5_TEMP},
	};

  for (uint32_t i = 0; i < GS_ARRAYELEMENTS(MCM_States); i++) {
    if (MCM_States[i].DeviceStates != (uint32_t) GSeMCM_GetActiveStatus(MCM_States[i].MCM_Handle)) {
      MCM_States[i].DeviceStates = GSeMCM_GetActiveStatus(MCM_States[i].MCM_Handle);
      if (i < MCM250_4 || (i == MCM250_6 && GetVar(HDL_MCM250_6_INIT))) {
        if (MCM_States[i].DeviceStates) {
          EList_ResetErr(MCM_States[i].ErrID_Offline);
        } else {
          EList_SetErrLevByRCText(VBL_ERROR_CRITICAL, MCM_States[i].ErrID_Offline, RCTEXT_T_MCM250_OFFLINE, i + 1);
        }
      } else if (i == MCM250_4 || i == MCM250_5) {
        //Reset MCM-Errors if MCM250_4 or MCM250_5 are online
        if (MCM_States[MCM250_4].DeviceStates || MCM_States[MCM250_5].DeviceStates) {
          EList_ResetErr(MCM_States[MCM250_4].ErrID_Offline);
          EList_ResetErr(MCM_States[MCM250_5].ErrID_Offline);
        } else {
          EList_SetErrLevByRCText(VBL_ERROR_CRITICAL, MCM_States[MCM250_4].ErrID_Offline, RCTEXT_T_MCM250_4OR250_5_OFFLINE);
        }
      }
    }
  }
  Ctrl_MCM7n8_Check();

  static uint32_t JoystickState;
  if (JoystickState != IO_Joystick_IsActive( & Ctrl -> joystick)) {
    JoystickState = IO_Joystick_IsActive( & Ctrl -> joystick);
    if (JoystickState) {
      EList_ResetErr(ERRID_JOYSTICK_OFFLINE);
    } else {
      EList_SetErrLevByRCText(VBL_ERROR_CRITICAL, ERRID_JOYSTICK_OFFLINE, RCTEXT_T_JOYSTICKOFFLLINE);
    }
  }
  static uint32_t EngineState;
  if (EngineState != Ctrl -> engine.Active && Ctrl -> sens.DI.Ignition) {
    EngineState = Ctrl -> engine.Active;
    if (EngineState) {
      EList_ResetErr(ERRID_ENGINE_OFFLINE);
    } else {
      EList_SetErrLevByRCText(VBL_ERROR_CRITICAL, ERRID_ENGINE_OFFLINE, RCTEXT_T_ENGINEOFFLINE);
    }
  }
  static uint32_t BNSOState;
  if (BNSOState != Ctrl -> BNSO.Active) {
    BNSOState = Ctrl -> BNSO.Active;
    if (BNSOState) {
      EList_ResetErr(ERRID_BNSO_OFFLINE);
    } else {
      EList_SetErrLevByRCText(VBL_ERROR_CRITICAL, ERRID_BNSO_OFFLINE, RCTEXT_T_BNSO_OFFLINE);
    }
  }
}

extern uint8_t flagOKB600_Init;
void Check_Light(void) {
	tPGN_FF06 * FF06 = J1939_GetPGN_FF06();
  for (uint32_t i = 0; i < GS_ARRAYELEMENTS(MCM_StandardDOs); i++) {
    static int countLightError[GS_ARRAYELEMENTS(MCM_StandardDOs)] = {0};

    uint16_t DO_mA = GSeMCM_GetAnalogIn(MCM_StandardDOs[i].Device, MCM_StandardDOs[i].Idx + 4);
    if ( * MCM_StandardDOs[i].DO == 1) {
      if (DO_mA < 50 && DO_mA > 1) {
        countLightError[i]++;
        if (countLightError[i] == 1500) {
					MCM_StandardDOs[i].Status = 0b10;
          EList_SetErrLevByRCText(VBL_ERROR, MCM_StandardDOs[i].Error, MCM_StandardDOs[i].ErrorText);
        }
      } else {
				MCM_StandardDOs[i].Status = 0b01;
        EList_ResetErr(MCM_StandardDOs[i].Error);
        countLightError[i] = 0;
      }
    } else {
      countLightError[i] = 0;
			MCM_StandardDOs[i].Status = 0;
    }

		if(!GetVarIndexed(IDX_ROADSIGN_LEFT)) {
			MCM_StandardDOs[4].Status = 0b11;
		}
		if(!GetVarIndexed(IDX_ROADSIGN_RIGHT)) {
			MCM_StandardDOs[5].Status = 0b11;
		}
		if(!GetVarIndexed(IDX_CHECK_SIDELIGHT)) {
			MCM_StandardDOs[1].Status = 0b11;
		}
		if(!GetVarIndexed(IDX_CHECK_INTERLIGHT)) {
			MCM_StandardDOs[2].Status = 0b11;
		}
		if(!GetVarIndexed(IDX_CHECK_HOPPERLIGHT)) {
			MCM_StandardDOs[8].Status = 0b11;
		}
		if(!flagOKB600_Init) {
			MCM_StandardDOs[3].Status = 0b11;
		}

    // Условие необходимо для того, чтобы в ручном режиме не отправлялись сигналы на МСМ.
    if (!GetVar(HDL_IO_MODECTRL) && (i != 12 || IsInfoContainerOn( CNT_TOP_CK ))) {
      GSeMCM_SetDigitalOut(MCM_StandardDOs[i].Device, MCM_StandardDOs[i].Idx, * MCM_StandardDOs[i].DO);
    }
  }

	// Заполнение структуры для передачи на БНСО
	FF06->FrontBeaconCab = MCM_StandardDOs[0].Status;
	FF06->WorkSide = MCM_StandardDOs[1].Status;
	FF06->BrushLight = MCM_StandardDOs[2].Status;
	FF06->RTR_HopperTop = MCM_StandardDOs[8].Status;
	FF06->RearBeacon = MCM_StandardDOs[7].Status;
	FF06->WorkRear = MCM_StandardDOs[6].Status;
	FF06->LeftRoadSign = MCM_StandardDOs[4].Status;
	FF06->RightRoadSign = MCM_StandardDOs[5].Status;
	FF06->OKB600_WorkRear = MCM_StandardDOs[3].Status;
}


void CheckRange( int32_t* value, int32_t min, int32_t max ) {
	if( *value < min )
		*value = min;
	else if( *value > max ) {
		*value = max;
	}
}

void Ctrl_SystemErrors(tControl * Ctrl) {
  static uint32_t count = 10000; // first 10 seconds no errordetection
  //Every 2 Seconds
  if (2000 < (int32_t)(GetMSTick() - count)) {
    count = GetMSTick();

    if (0 == Ctrl -> sens.DI.CheckOilLevelSensor) {
      EList_SetErrLevByRCText(VBL_ERROR, ERRID_NO_OIL_SENSOR, RCTEXT_T_OILSENSORNOTAVAILABLE);
      SetVar(HDL_OILLEVEL, -1); //Warning, grey Icon
    } else {
      EList_ResetErr(ERRID_NO_OIL_SENSOR);
      if ((0 == Ctrl -> sens.DI.UpperOilLevel) && (0 == Ctrl -> sens.DI.LowerOilLevel)) {
        EList_SetErrLevByRCText(VBL_ERROR, ERRID_DECREASINGOILEVEL, RCTEXT_T_DECREASEINOILLEVEL);
        EList_ResetErr(ERRID_CRITICAL_OIL_LEVEL);
        SetVar(HDL_OILLEVEL, 1); //Warning, orange Icon
      } else if ((1 == Ctrl -> sens.DI.LowerOilLevel) && (0 == Ctrl -> sens.DI.UpperOilLevel)) {
        EList_SetErrLevByRCText(VBL_ERROR, ERRID_CRITICAL_OIL_LEVEL, RCTEXT_T_CRITICALOILLEVEL);
        EList_ResetErr(ERRID_DECREASINGOILEVEL);
        SetVar(HDL_OILLEVEL, 0); //Critical, red Icon
      } else if ((1 == Ctrl -> sens.DI.UpperOilLevel) &&
        (0 == Ctrl -> sens.DI.LowerOilLevel)) {

        SetVar(HDL_OILLEVEL, 2); //Warning, green Icon
        EList_ResetErr(ERRID_DECREASINGOILEVEL);
        EList_ResetErr(ERRID_CRITICAL_OIL_LEVEL);
      } else {
        EList_SetErrLevByRCText(VBL_ERROR, ERRID_NO_OIL_SENSOR, RCTEXT_T_OILSENSORNOTAVAILABLE);
        SetVar(HDL_OILLEVEL, -1); //Warning, grey Icon
      }
    }
  }
}

int32_t EmcyBuzzer = 0;
void Emcy_OK_cb(tControl * Ctrl) {
  if (!Ctrl -> emcy.val || (!Ctrl -> emcy_2.val && GetVar(HDL_MCM250_6_INIT))) {
    MsgBoxOk(RCTextGetText(RCTEXT_T_EMCY_BUTTON, GetVarIndexed(IDX_SYS_LANGUAGE)), RCTextGetText(RCTEXT_T_EMCY_MSG, GetVarIndexed(IDX_SYS_LANGUAGE)), Emcy_OK_cb, Ctrl);
    EmcyBuzzer = 0;
  } else {
    GSeMCM_Emcy_Set(0);
    Ctrl -> q_emcy = 0;
  }
}

int IsEmcyNeeded( void ) {
  return (!CTRL.emcy.val && GSeMCM_GetActiveStatus(MCM250_3)) ||
    (!CTRL.emcy_2.val && GSeMCM_GetActiveStatus(MCM250_6) &&
      (Profile_GetCurrentProfile() -> module[P_TYPE_TOP] == RCTEXT_LI_TOPATTACHMENT_ATTACHMENT_BUCHER_CR));
}

void Ctrl_EMCY_Cycle(void) {
  if (CTRL.sens.DI.Ignition) {
    if (IsEmcyNeeded()) {
      CTRL.cmd_no_key.E_L = 0; //stops the Pump and rtr
      CTRL.cmd_no_key.E_S = 0;
      SetVarIndexed(IDX_EMCY_ON, 1); //@
      if (!CTRL.q_emcy) {
        CTRL.q_emcy = 1;
        MsgBoxOk(RCTextGetText(RCTEXT_T_EMCY_BUTTON, GetVarIndexed(IDX_SYS_LANGUAGE)), RCTextGetText(RCTEXT_T_EMCY_MSG, GetVarIndexed(IDX_SYS_LANGUAGE)), Emcy_OK_cb, & CTRL);
        GSeMCM_Emcy_Set(1);
        SetVar(HDL_EMCYSTOPNUM, 1 + GetVar(HDL_EMCYSTOPNUM));
        EmcyBuzzer = 1;
      }
      if (EmcyBuzzer) {
        SetBuzzer(1, 10, 10, 1);
      }
    } else SetVarIndexed(IDX_EMCY_ON, 0);
  }
}

/**************************************************
* Функция активирует режим разработчика
**************************************************/
void Developer_Mode( void ) {
	if( dev_ctrl ) {
		SetVar( HDL_OILLEVEL, 1 ); // Отображение ШИМов
		SetBuzzer( 0, 0, 0, 0 ); // Отключение звука выпадения ошибок
		SetVarIndexed( IDX_PASSWORD_ENTER, 4848 ); // Убран ввод пароля
	}
}

void Ctrl_Automatic_Toggle(void) {
  OperatingMode tAutoOpMd = CTRL.automatic.on;
  if (++tAutoOpMd > MANUAL_MODE_30km)
    tAutoOpMd = 0;
  CTRL.automatic.on = tAutoOpMd;
}

void Ctrl_JS_Commands(tControl * Ctrl) {
  //Get Commands for A, C read and inter attachment
  int32_t press_ = 0;
  int32_t sum_ = 0;
  for (int32_t i_ = 0; i_ <= EQUIP_C; i_++) {
    if (IOT_Button_IsDown( & Ctrl -> joystick.Button[i_])) {
      sum_++;
    }
  }
  Ctrl -> pressed_ABC = sum_;
  for (int32_t i_ = 0; i_ <= EQUIP_C; i_++) {
    int32_t key_ = 0;
    if (sum_ == 1) //only if one key is pressed.
      key_ = (IOT_Button_IsDown( & Ctrl -> joystick.Button[i_]) ? 1 : 0);
    Ctrl_JS_Cmd_SetCommands( & Ctrl -> cmd_ABC[i_], key_, i_ + 1);
    if (key_) {
      press_ = 1;
    }
  }
  if (press_) {
    Ctrl_JS_Cmd_SetCommands( & Ctrl -> cmd_no_key, 0, NULL);
  } else {
    Ctrl_JS_Cmd_SetCommands( & Ctrl -> cmd_no_key, 1, NULL);
  }
  Ctrl_JS_Buzzer();
}

int32_t TopGetSpeed(void) {
  const tControl * Ctrl = CtrlGet();
  const tParamData * Param = ParameterGet();
  static int32_t init = 0;
  static int32_t speedImpulses = 0;

  if (!init) {
    if (Param -> Speedometer.Impulses != 0) {
      speedImpulses = 1;
    }
    init = 1;
  }

  if (CTRL.automatic.on == MANUAL_MODE_30km) {
    CTRL.engine.speed = 30;
    return 30;
  }

  if (speedImpulses == 1) {
    return GSeMCM_GetFrequencyInput(MCM250_3, 3) / ((float) Param -> Speedometer.Impulses / (float) 10);
  } else {
    return Ctrl -> engine.speed;
  }
}

int32_t GetSpeedKDM(void) {
  const tControl * Ctrl = CtrlGet();
  const tParamData * Param = ParameterGet();
  static int32_t init = 0;
  static int32_t speedImpulses = 0;

  if (!init) {
    if (Param -> Speedometer.Impulses != 0) {
      speedImpulses = 1;
    }
    init = 1;
  }

  if (speedImpulses == 1) {
    return GSeMCM_GetFrequencyInput(MCM250_3, 3) / ((float) Param -> Speedometer.Impulses / (float) 10);
  } else {
    return Ctrl -> engine.speedKDM;
  }
}

void Ctrl_ResetMaxSpeed( void ) {
	CTRL.max_speed = 0;
}

void Ctrl_SetMaxSpeed(uint32_t MaxSpeed) {
  if (CTRL.max_speed == 0) {
    CTRL.max_speed = MaxSpeed;
  }
  if (MaxSpeed < CTRL.max_speed) {
    CTRL.max_speed = MaxSpeed;
  }
  CTRL.max_speed = 5 * ceil((float) CTRL.max_speed / 5.);
}

void Ctrl_CheckMaxSpeed(tControl * Ctrl) {
  static int32_t ErrState;
  static int32_t count;

  if ((Ctrl -> max_speed == 0) || (TopGetSpeed() < ((double) Ctrl -> max_speed + 10))) {
    if (ErrState) { //Reset Error
      EList_ResetErr(ERRID_OVERSPEED);
      ErrState = 0;
    }
    count = GetMSTick();
  } else {
    if (ErrState == 0) //only if error is not set, yet
    {
      //Set Overspeed error if overspeed longer than 2 seconds
      if (2000 < GetMSTick() - count) {
        ErrState = 1;
        EList_SetErrLevByRCText(VBL_WARNING, ERRID_OVERSPEED, RCTEXT_T_OVERSPEED);
      }
    }
  }
}

void Ctrl_MaxSpeed_Draw(tControl * Ctrl) {
  static int32_t DrawAttr_Old = -1;
  static int8_t onesignal = 0;
  static uint32_t timer_speed = 0;
  int32_t DrawAttr = 0;
  if (0 == Ctrl -> max_speed) {
    DrawAttr = 0;
  } else if (TopGetSpeed() > (int16_t) Ctrl -> max_speed) {
    DrawAttr = ATTR_BLINKING | ATTR_VISIBLE | ATTR_TRANSPARENT | ATTR_SMOOTH;
    if (ReactSignalInError(SizeOfReact() - 2) == 1 && !onesignal) {
      SetBuzzer(1, 20, 20, 2);
      onesignal = 1;
    }

    if (ReactSignalInError(SizeOfReact() - 2) == 2) {
      SetBuzzer(2, 20, 20, 2);
    }

    if (ReactSignalInError(SizeOfReact() - 2) == 3 && timer_speed + 5000 < GetMSTick()) {
      timer_speed = GetMSTick();
      SetBuzzer(1, 20, 20, 2);
    }
  } else {
    DrawAttr = ATTR_VISIBLE | ATTR_TRANSPARENT | ATTR_SMOOTH;
    onesignal = 0;
    timer_speed = 0;
  }

  if (DrawAttr != DrawAttr_Old) {
    DrawAttr_Old = DrawAttr;
    SetVarIndexed(IDX_MAXSPEEDATT, DrawAttr);
  }
  SetVarIndexed(IDX_MAXSPEED, Ctrl -> max_speed);
}

void Top_Boost_Cycle(tTopEquip *top) {
  const tControl * Ctrl = CtrlGet();

  if (Ctrl -> cmd_no_key.Boost) {
    top->BoostOn = 1;
  } else {
    top->BoostOn = 0;
  }

  if (top->BoostOn) {
    if (!IsMsgContainerOn(CNT_BOOST)) {
      MsgContainerOn(CNT_BOOST);
    }
  } else {
    if (IsMsgContainerOn(CNT_BOOST)) {
      MsgContainerOff(CNT_BOOST);
    }
  }
}

/**************************************************************
* Функция инициализации переменных при первом запуске пульта.
* При повторных запусках данная функция вызываться не будет
**************************************************************/
void FS_Variable_Init(void) {
  if (GetVar(HDL_FIRSTSTART) == 0) {
    SetVarFloat(HDL_COEFBOOST, 2.0);
    if (FileSize(PATH_PINOUT) <= 0) {
      PinOut_Write_Struct();
    }
    if (FileSize(PATH_ERRORREACT_FLASH) <= 0) {
      WrInFileReact();
    }
    SetVar(HDL_FIRSTSTART, 1);
  }
}

void Ctrl_Init(void) {
  FS_Variable_Init();

  J1939_Init(CAN1, & CTRL);

  //Init CAN-Modules
  IO_Init( & CTRL);

  ConveyorCreate( & CTRL.top.WinterTop.SpreadConv);
  SpreaderDiskCreate( & CTRL.top.WinterTop.SpreadDisk);

  //Load characteristic curve for PVEP-Valv.
  GlobalInitSqlTableLiter();

  Maint_Init( & CTRL.maint);
  Maint_Init( & CTRL.maint_hour_cnt);

  Brush_Maint_Create( & CTRL.ABC[EQUIP_A].BrushValve, & CTRL.maint_hour_cnt, ERRID_MAINTENANCE_BRUSH_FRONT, RCTEXT_T_FRONTBRUSHWARN, RCTEXT_T_FRONTBRUSHSERVICE, IDX_BRUSH_WARN_0, IDX_BRUSH_SERVICE_0);
  Plough_Maint_Create( & CTRL.ABC[EQUIP_A].ABC_Attach, & CTRL.maint_hour_cnt, ERRID_MAINTENANCE_BRUSH_FRONT, RCTEXT_T_PLOUGHFRONT, RCTEXT_T_FRONTBRUSHSERVICE, IDX_PLOUGH_WARN_0, NULL);
  Plough_Maint_Create_Reset( & CTRL.ABC[EQUIP_A].ABC_Attach, & CTRL.maint_hour_cnt, ERRID_MAINTENANCE_BRUSH_FRONT, RCTEXT_T_PLOUGHFRONT, RCTEXT_T_FRONTBRUSHSERVICE, IDX_PLOUGH_WARN_0_RESET, NULL);
  Plough_Maint_Create( & CTRL.ABC[EQUIP_C].ABC_Attach, & CTRL.maint_hour_cnt, ERRID_MAINTENANCE_BRUSH_FRONT, RCTEXT_T_PLOUGHSIDE, RCTEXT_T_FRONTBRUSHSERVICE, IDX_PLOUGH_WARN_1, NULL);
  Plough_Maint_Create_Reset( & CTRL.ABC[EQUIP_C].ABC_Attach, & CTRL.maint_hour_cnt, ERRID_MAINTENANCE_BRUSH_FRONT, RCTEXT_T_PLOUGHSIDE, RCTEXT_T_FRONTBRUSHSERVICE, IDX_PLOUGH_WARN_1_RESET, NULL);
  Brush_Maint_Create_Reset( & CTRL.ABC[EQUIP_A].BrushValve, & CTRL.maint_hour_cnt, ERRID_MAINTENANCE_BRUSH_FRONT, RCTEXT_T_FRONTBRUSHWARN, RCTEXT_T_FRONTBRUSHSERVICE, IDX_BRUSH_WARN_0_RESET, IDX_BRUSH_SERVICE_0);
  Brush_Maint_Create( & CTRL.ABC[EQUIP_C].BrushValve, & CTRL.maint_hour_cnt, ERRID_MAINTENANCE_Brush_SIDE, RCTEXT_T_SIDEBRUSHWARN, RCTEXT_T_SIDEBRUSHSERVICE, IDX_BRUSH_WARN_1, IDX_BRUSH_SERVICE_1);
  Brush_Maint_Create( & CTRL.ABC[EQUIP_B].BrushValve, & CTRL.maint_hour_cnt, ERRID_MAINTENANCE_BRUSH_INTERAXIS, RCTEXT_T_INTERAXISBRUSHWARN, RCTEXT_T_INTERAXISBRUSHWARN, IDX_BRUSH_WARN_2, IDX_BRUSH_SERVICE_2);
  Brush_Maint_Create_Reset( & CTRL.ABC[EQUIP_B].BrushValve, & CTRL.maint_hour_cnt, ERRID_MAINTENANCE_BRUSH_INTERAXIS, RCTEXT_T_INTERAXISBRUSHWARN, RCTEXT_T_INTERAXISBRUSHWARN, IDX_BRUSH_WARN_2_RESET, IDX_BRUSH_SERVICE_2);
  CTRL.hydr.load = Maint_Runtime_Add( & CTRL.maint_hour_cnt, ERRID_MAINTENANCE_OILPUMP_LOAD, RCTEXT_T_HYD_PUMP_LOAD_WARN, RCTEXT_T_HYD_PUMP_LOAD_SERVICE, IDX_RUNTIME_HYDLOAD, IDX_SERVICE_HYDLOAD, 100);
  CTRL.hydr.load_Reset = Maint_Runtime_Add( & CTRL.maint_hour_cnt, ERRID_MAINTENANCE_OILPUMP_LOAD, RCTEXT_T_HYD_PUMP_LOAD_WARN, RCTEXT_T_HYD_PUMP_LOAD_SERVICE, IDX_RUNTIME_HYDLOAD_SHOW_RESET, IDX_SERVICE_HYDLOAD, 100);
  CTRL.hydr.unload = Maint_Runtime_Add( & CTRL.maint_hour_cnt, ERRID_MAINTENANCE_OILPUMP_UNLOAD, RCTEXT_T_HYD_PUMP_LOAD_WARN, RCTEXT_T_HYD_PUMP_UNLOAD_SERVICE, IDX_RUNTIME_HYDNOLOAD, IDX_SERVICE_HYDNOLOAD, 100);
  CTRL.hydr.unload_Reset = Maint_Runtime_Add( & CTRL.maint_hour_cnt, ERRID_MAINTENANCE_OILPUMP_UNLOAD, RCTEXT_T_HYD_PUMP_LOAD_WARN, RCTEXT_T_HYD_PUMP_UNLOAD_SERVICE, IDX_RUNTIME_HYDNOLOAD_SHOW_RESET, IDX_SERVICE_HYDNOLOAD, 100);
  CTRL.top.WinterTop.SpreadConv.Runtime = Maint_Runtime_Add( & CTRL.maint_hour_cnt, ERRID_MAINTENANCE_RTR, RCTEXT_T_RTR_WARN, RCTEXT_T_RTR_SERVICE, IDX_RUNTIME_RTR, IDX_SERVICE_RTR, 100);
  CTRL.top.WinterTop.SpreadConv.Runtime_Reset = Maint_Runtime_Add( & CTRL.maint_hour_cnt, ERRID_MAINTENANCE_RTR, RCTEXT_T_RTR_WARN, RCTEXT_T_RTR_SERVICE, IDX_RUNTIME_RTR_RESET, IDX_SERVICE_RTR, 100);
  //_Ctrl.Top.Conveyor.Runtime = Maint_Runtime_Add(&_Ctrl.MaintHourcounter, ERRID_MAINTENANCE_RTR_HUMID     , RCTEXT_T_RTR_H_WARN           , RCTEXT_T_RTR_H_SERVICE          ,IDX_RUNTIME_RTRH     , IDX_SERVICE_RTRH, 100 );
  CTRL.top.SummerTop.LRS.Runtime300 = Maint_Runtime_Add( & CTRL.maint_hour_cnt, ERRID_MAINTENANCE_LRS, RCTEXT_T_LRS_WARN, RCTEXT_T_LRS_SERVICE, IDX_RUNTIME_LRS_300, IDX_SERVICE_LRS, 100);
  CTRL.top.SummerTop.LRS.Runtime600 = Maint_Runtime_Add( & CTRL.maint_hour_cnt, ERRID_MAINTENANCE_LRS, RCTEXT_T_LRS_WARN, RCTEXT_T_LRS_SERVICE, IDX_RUNTIME_LRS_600, IDX_SERVICE_LRS, 100);
  CTRL.top.SummerTop.LRS.Runtime_Reset300 = Maint_Runtime_Add( & CTRL.maint_hour_cnt, ERRID_MAINTENANCE_LRS, RCTEXT_T_LRS_WARN, RCTEXT_T_LRS_SERVICE, IDX_RUNTIME_LRS_RESET300, IDX_SERVICE_LRS, 100);
  CTRL.top.SummerTop.LRS.Runtime_Reset600 = Maint_Runtime_Add( & CTRL.maint_hour_cnt, ERRID_MAINTENANCE_LRS, RCTEXT_T_LRS_WARN, RCTEXT_T_LRS_SERVICE, IDX_RUNTIME_LRS_RESET600, IDX_SERVICE_LRS, 100);
  CTRL.anual_maint = Maint_Lifetime_Add( & CTRL.maint, ERRID_MAINTENANCE_ANNUAL, RCTEXT_T_ANNUALMAINTENANCEWARN, RCTEXT_T_ANNUALMAINT, IDX_ANNUALSERVICE, 30 * 24); // Warn 30 days before

  CTRL.automatic.on = AUTO_MODE;

  const tParamData * Parameter = ParameterGet();
  if (Parameter -> Engine.PTOatEngine || CTRL.sens.DI.PTO_Active) {
    Maint_Runtime_SetDiv(CTRL.hydr.unload, 1);
    Maint_Runtime_SetDiv(CTRL.hydr.unload_Reset, 1);
  }

  const tParamData * Param = ParameterGet();
  IOT_NormAI_Init( & CTRL.sens.AI.OilPressure, Param -> Sensors.OilPressure.x1, Param -> Sensors.OilPressure.x2, Param -> Sensors.OilPressure.y1, Param -> Sensors.OilPressure.y2, 0, 0);
  IOT_NormAI_Init( & CTRL.sens.AI.OilTemp, Param -> Sensors.OilTemp.x1, Param -> Sensors.OilTemp.x2, Param -> Sensors.OilTemp.y1, Param -> Sensors.OilTemp.y2, 0, 0);
  IOT_NormAI_Init( & CTRL.sens.AI.UsSens_1, Param -> Sensors.UsSens_1.x1, Param -> Sensors.UsSens_1.x2, Param -> Sensors.UsSens_1.y1, Param -> Sensors.UsSens_1.y2, 0, 0);
  IOT_NormAI_Init( & CTRL.sens.AI.UsSens_2, Param -> Sensors.UsSens_2.x1, Param -> Sensors.UsSens_2.x2, Param -> Sensors.UsSens_2.y1, Param -> Sensors.UsSens_2.y2, 0, 0);

  for (uint32_t i = 1; i < GS_ARRAYELEMENTS(CTRL.D3510.Button); i++) {
    IOT_Button_Init( & CTRL.D3510.Button[i], 100, 500);
  }
  // SQL_Close();
}

/**
* @brief Checks if a brush is connected to the Attachment and if it is
* turning. If so, it starts the Counter for the runtime.
*
* @param ABC_Attach
*/
void CtrlHourCounter_tABCAttachCycle(tBrushValve* BrushValve, tJSCommands * Cmd) {
  if (Brush_Active(BrushValve)) {
    Maint_Runtime_Start(BrushValve -> MaintRunntime);
    Maint_Runtime_Start(BrushValve -> MaintRunntime_Reset);
  } else {
    Maint_Runtime_Stop(BrushValve -> MaintRunntime);
    Maint_Runtime_Stop(BrushValve -> MaintRunntime_Reset);
  }
}

int HydPumpOn( void ){
  const tParamData * Param = ParameterGet();
  const tControl * Ctrl = CtrlGet();
  if ((Param -> Engine.PTOatEngine) && (Ctrl -> engine.rpm > 0)) {
    return 1;
  } else if ((Ctrl -> sens.DI.PTO_Active) && (Ctrl -> engine.rpm > 0)) {
    return 1;
  } else {
    return 0;
  }
}
/**
* @brief Controles the Hourcounters for the different Attachments and the Engine
*
* @param Ctrl
*/
void Ctrl_HourConter_Cycle(tControl * Ctrl) {

  //Check if Hydraulik Pump is working and if it is under load or not.

  if (HydPumpOn() && !Ctrl -> q_emcy) { //Hydraulic Pump is running.
    //Check tABCAttaches:
    CtrlHourCounter_tABCAttachCycle( & Ctrl -> ABC[EQUIP_A].BrushValve, & Ctrl -> cmd_ABC[EQUIP_A]);
    CtrlHourCounter_tABCAttachCycle( & Ctrl -> ABC[EQUIP_B].BrushValve, & Ctrl -> cmd_ABC[EQUIP_B]);
    CtrlHourCounter_tABCAttachCycle( & Ctrl -> ABC[EQUIP_C].BrushValve, & Ctrl -> cmd_ABC[EQUIP_C]);

    //RTR Working hours
    if (Ctrl -> top.WinterTop.SpreadConv.on) {
      Maint_Runtime_Start(Ctrl -> top.WinterTop.SpreadConv.Runtime);
      Maint_Runtime_Start(Ctrl -> top.WinterTop.SpreadConv.Runtime_Reset);
    } else {
      Maint_Runtime_Stop(Ctrl -> top.WinterTop.SpreadConv.Runtime);
      Maint_Runtime_Stop(Ctrl -> top.WinterTop.SpreadConv.Runtime_Reset);

    }
    //LRS Working hours:
    if (Ctrl -> top.SummerTop.Pump.Type == WATERPUMP_TYPE_C610H && WaterPump_IsOn( & Ctrl -> top.SummerTop.Pump) == 1) {
      Maint_Runtime_Start(Ctrl -> top.SummerTop.LRS.Runtime600);
      Maint_Runtime_Start(Ctrl -> top.SummerTop.LRS.Runtime_Reset600);
    } else if (Ctrl -> top.SummerTop.Pump.Type == WATERPUMP_TYPE_BP300 && WaterPump_IsOn( & Ctrl -> top.SummerTop.Pump) == 1) {
      Maint_Runtime_Start(Ctrl -> top.SummerTop.LRS.Runtime300);
      Maint_Runtime_Start(Ctrl -> top.SummerTop.LRS.Runtime_Reset300);
    } else {
      Maint_Runtime_Stop(Ctrl -> top.SummerTop.LRS.Runtime300);
      Maint_Runtime_Stop(Ctrl -> top.SummerTop.LRS.Runtime_Reset300);
      Maint_Runtime_Stop(Ctrl -> top.SummerTop.LRS.Runtime600);
      Maint_Runtime_Stop(Ctrl -> top.SummerTop.LRS.Runtime_Reset600);

    }

    if (GetVarIndexed(IDX_FLOATING_SIDE)) {
      Maint_Runtime_Start(Ctrl -> ABC[EQUIP_C].ABC_Attach.Runtime_Ploughs);
      Maint_Runtime_Start(Ctrl -> ABC[EQUIP_C].ABC_Attach.Runtime_Ploughs_Reset);
    } else {
      Maint_Runtime_Stop(Ctrl -> ABC[EQUIP_C].ABC_Attach.Runtime_Ploughs);
      Maint_Runtime_Stop(Ctrl -> ABC[EQUIP_C].ABC_Attach.Runtime_Ploughs_Reset);
    }

    if (GetVarIndexed(IDX_FLOATING_FRONT)) {
      Maint_Runtime_Start(Ctrl -> ABC[EQUIP_A].ABC_Attach.Runtime_Ploughs);
      Maint_Runtime_Start(Ctrl -> ABC[EQUIP_A].ABC_Attach.Runtime_Ploughs_Reset);
    } else {
      Maint_Runtime_Stop(Ctrl -> ABC[EQUIP_A].ABC_Attach.Runtime_Ploughs);
      Maint_Runtime_Stop(Ctrl -> ABC[EQUIP_A].ABC_Attach.Runtime_Ploughs_Reset);
    }

    //does the Pump work with load or without?
    if (Brush_Active( & Ctrl -> ABC[EQUIP_A].BrushValve) ||
      Brush_Active( & Ctrl -> ABC[EQUIP_B].BrushValve) ||
      WaterPump_IsOn( & Ctrl -> top.SummerTop.Pump) == 1 ||
      GetVarIndexed(IDX_FLOATING_SIDE) ||
      GetVarIndexed(IDX_FLOATING_FRONT) ||
      GetVarIndexed(IDX_FLOATING_INTERAX) ||
      Maint_Runtime_IsRunning(Ctrl -> top.WinterTop.SpreadConv.Runtime)) {
      Maint_Runtime_Start(Ctrl -> hydr.load);
      Maint_Runtime_Stop(Ctrl -> hydr.unload);
      Maint_Runtime_Start(Ctrl -> hydr.load_Reset);
      Maint_Runtime_Stop(Ctrl -> hydr.unload_Reset);
    } else {
      Maint_Runtime_Stop(Ctrl -> hydr.load);
      Maint_Runtime_Start(Ctrl -> hydr.unload);
      Maint_Runtime_Stop(Ctrl -> hydr.load_Reset);
      Maint_Runtime_Start(Ctrl -> hydr.unload_Reset);
    }
  } else {
    Maint_Runtime_Stop(Ctrl -> hydr.load);
    Maint_Runtime_Stop(Ctrl -> hydr.unload);
    Maint_Runtime_Stop(Ctrl -> top.WinterTop.SpreadConv.Runtime);
    Maint_Runtime_Stop(Ctrl -> top.SummerTop.LRS.Runtime300);
    Maint_Runtime_Stop(Ctrl -> top.SummerTop.LRS.Runtime600);

    Maint_Runtime_Stop(Ctrl -> hydr.load_Reset);
    Maint_Runtime_Stop(Ctrl -> hydr.unload_Reset);
    Maint_Runtime_Stop(Ctrl -> top.WinterTop.SpreadConv.Runtime_Reset);
    Maint_Runtime_Stop(Ctrl -> top.SummerTop.LRS.Runtime_Reset300);
    Maint_Runtime_Stop(Ctrl -> top.SummerTop.LRS.Runtime_Reset600);
  }
  SetVarIndexed(IDX_RUNTIME_HYDLOAD_SHOW, GetVarIndexed(IDX_RUNTIME_HYDLOAD));
  SetVarIndexed(IDX_RUNTIME_HYDNOLOAD_SHOW, GetVarIndexed(IDX_RUNTIME_HYDNOLOAD));
}

void Ctrl_PassedWay_Cycle(tControl * Ctrl) {

  if (GetVarFloat(HDL_OLD_DISTANCE) == 0 && Ctrl -> engine.PassedWay > 0) {
    SetVarFloat(HDL_OLD_DISTANCE, Ctrl -> engine.PassedWay);
  }
  if (GetVarFloat(HDL_OLD_DISTANCE) < Ctrl -> engine.PassedWay && HydPumpOn()) {
    float new_passed_way = Ctrl -> engine.PassedWay - GetVarFloat(HDL_OLD_DISTANCE);
    if (Ctrl -> top.WinterTop.SpreadConv.on) {
      SetVarFloat(HDL_DISTANCE_RTR_LOAD, GetVarFloat(HDL_DISTANCE_RTR_LOAD) + new_passed_way);
    }
    if (Ctrl -> top.SummerTop.Pump.Type == WATERPUMP_TYPE_C610H && WaterPump_IsOn( & Ctrl -> top.SummerTop.Pump) == 1) {
      SetVarFloat(HDL_DISTANCE_LRS_600_LOAD, GetVarFloat(HDL_DISTANCE_LRS_600_LOAD) + new_passed_way);
    } else if (Ctrl -> top.SummerTop.Pump.Type == WATERPUMP_TYPE_BP300 && WaterPump_IsOn( & Ctrl -> top.SummerTop.Pump) == 1) {
      SetVarFloat(HDL_DISTANCE_LRS_300_LOAD, GetVarFloat(HDL_DISTANCE_LRS_300_LOAD) + new_passed_way);
    }
    int32_t arrIdxAttach[] = {
      IDX_FLOATING_SIDE,
      IDX_FLOATING_FRONT
    };
    int32_t arrIdxDistance[] = {
      HDL_DISTANCE_PLOUGH_1_LOAD,
      HDL_DISTANCE_PLOUGH_0_LOAD
    };
    int work_attach = 0;
    for (int i = 0; i < GS_ARRAYELEMENTS(arrIdxAttach); i++) {
      if (GetVarIndexed(arrIdxAttach[i])) {
        SetVarFloat(arrIdxDistance[i], GetVarFloat(arrIdxDistance[i]) + new_passed_way);
        work_attach = 1;
      }
    }
    if(Brush_Active( & Ctrl -> ABC[EQUIP_A].BrushValve)) {
      SetVarFloat(HDL_DISTANCE_BRUSH_0_LOAD, GetVarFloat(HDL_DISTANCE_BRUSH_0_LOAD) + new_passed_way);
      work_attach = 1;
    }
    if(Brush_Active( & Ctrl -> ABC[EQUIP_B].BrushValve)) {
      SetVarFloat(HDL_DISTANCE_BRUSH_2_LOAD, GetVarFloat(HDL_DISTANCE_BRUSH_2_LOAD) + new_passed_way);
      work_attach = 1;
    }

    if (work_attach || WaterPump_IsOn( & Ctrl -> top.SummerTop.Pump) || Ctrl -> top.WinterTop.SpreadConv.on) {
      SetVarFloat(HDL_DISTANCE_HYDLOAD, GetVarFloat(HDL_DISTANCE_HYDLOAD) + new_passed_way);
    } else {
      SetVarFloat(HDL_DISTANCE_HYDNOLOAD, GetVarFloat(HDL_DISTANCE_HYDNOLOAD) + new_passed_way);
    }
  }
  if(Ctrl -> engine.PassedWay>GetVarFloat(HDL_OLD_DISTANCE)){
    SetVarFloat(HDL_OLD_DISTANCE, Ctrl -> engine.PassedWay);
  }
}

// Функция записи пробега машины
void mileage( void ) {

  static uint32_t tmt_1s = 0; //начальное значение таймера

  if (GetMSTick() > tmt_1s) //если GetMSTick > таймера то значение таймера + запись за каждую секунду
  {
    tmt_1s = GetMSTick() + 1000;
    SetVarFloat(HDL_MILEAGE, ((double) GetSpeedKDM() / 3.6 + GetVarFloat(HDL_MILEAGE))); // меняем значение HDL_MILEAGE на результат формулы актуальная скорость км.ч / на 3.6 м.с
    CTRL.engine.PassedWay = GetVarFloat(HDL_MILEAGE) / 1000.; //меняем значение статистики PassedWay на HDL_MILEAGE / 1000 чтобы в статистеке отображались км а не метры
  }
}

//Функция старта mileage
void StartMileage( void ) {
  if (CtrlGet() -> engine.rpm > 0) // Функция StartMileage будет выполнена если
  {
    /*Начальное значение таймера*/
    static uint32_t TMT_TIME = 0;

    if (TMT_TIME == 0) TMT_TIME = GetMSTick() + 1500; //здесь прописывается условие запуска функции через 1 секунду
    if (GetVarIndexed(IDX_PASSEDWAY) == 0 && GetMSTick() > TMT_TIME) {
      mileage();
    }
  }
}

tReactBuzzer React;
void StopEqp(tControl * Ctrl) {
  if (React.ReactDoing == 1 || React.ReactDoing == 2) {
    Ctrl -> cmd_no_key.E_L = 0;
    //Ctrl->cmd_no_key.E_Time= 0;
    Ctrl -> cmd_no_key.E_S = 0;
    if (React.ReactDoing == 1) return 0;
    for (int i = 0; i < 3; i++) {
      Ctrl -> cmd_ABC[i].Active = 0;
      Ctrl -> cmd_ABC[i].Updown = 0;
      Ctrl -> cmd_ABC[i].LeftRight = 0;
      Ctrl -> cmd_ABC[i].X = 0;
      Ctrl -> cmd_ABC[i].Y = 0;
      Ctrl -> cmd_ABC[i].Z = 0;
      Ctrl -> cmd_ABC[i].X_Change = 0;
      Ctrl -> cmd_ABC[i].Y_Change = 0;
      Ctrl -> cmd_ABC[i].Z_Change = 0;
      Ctrl -> cmd_ABC[i].F_New = 0;
      Ctrl -> cmd_ABC[i].Float = 0;
      Ctrl -> cmd_ABC[i].Boost = 0;
      Ctrl -> cmd_ABC[i].D_L = 0;
      Ctrl -> cmd_ABC[i].D_S = 0;
    }
  }
}

void Ctrl_Cycle(uint32_t evtc, tUserCEvt * evtv) {

  IO_Cycle( & CTRL, evtc, evtv);
  //Read Buttons
  for (uint32_t i = 0; i < GS_ARRAYELEMENTS(MCM_StandardButtons); i++) {
    IOT_Button_SetDI(MCM_StandardButtons[i].Button, GSeMCM_GetDigitalIn(MCM_StandardButtons[i].Device, MCM_StandardButtons[i].idx));
  }
  //Read digital Inputs
  for (uint32_t i = 0; i < GS_ARRAYELEMENTS(MCM_DIs); i++) {
    if ( * MCM_DIs[i].DI != DI_SENSOR_INACTIVE)
      * MCM_DIs[i].DI = GSeMCM_GetDigitalIn(MCM_DIs[i].Device, MCM_DIs[i].idx);
  }
  //Read analog Inputs
  for (uint32_t i = 0; i < GS_ARRAYELEMENTS(MCM_AIs); i++) {
    IOT_NormAI_Calc(MCM_AIs[i].AI, GSeMCM_GetAnalogIn(MCM_AIs[i].Device, MCM_AIs[i].idx));
  }
  //Read frequency Inputs
  for (uint32_t i = 0; i < GS_ARRAYELEMENTS(MCM_FreqIn); i++) {
    * MCM_FreqIn[i].Freq = GSeMCM_GetFrequencyInput(MCM_FreqIn[i].Device, MCM_FreqIn[i].idx);
  }
  //Read Keyinputs of D3510
  for (uint32_t i = 0; i < GS_ARRAYELEMENTS(CTRL.D3510.Button); i++) {
    IOT_Button_SetDI( & CTRL.D3510.Button[i], IsKeyDown(i + 1) ? 1 : 0);
  }
  // Check PTO
  if (CTRL.sens.DI.PTO_Active) {
    SetVarIndexed(IDX_PTO_ON, 1);
  } else {
    SetVarIndexed(IDX_PTO_ON, 0);
  }

  Ctrl_PassedWay_Cycle(&CTRL);
  StartMileage();

  Ctrl_EMCY_Cycle();
  J1939_Cycle( & CTRL.engine);
  Can_Statistic();
  StopEqp( & CTRL);
  Ctrl_JS_Commands( & CTRL);

  Developer_Mode();
  Ctrl_Light( & CTRL.light, evtc, evtv);
  Check_Light();
  Ctrl_ResetMaxSpeed();
  Prof_Cycle( & CTRL, evtc, evtv);

  static uint32_t timer = 0;
  static int32_t init = 0;
  if (!init) {
    init = 1;
    timer = GetMSTick() + 10000;
  }

  if (500 < (GetMSTick() - timer)) {
    timer = GetMSTick();
    Ctrl_SystemErrors( & CTRL);
    Ctrl_DeviceStates( & CTRL);
    Maint_Check( & CTRL.maint);

    J1939_Timer( & CTRL);
    Maint_Runtime_Cycle();
    Ctrl_HourConter_Cycle( & CTRL);
    Ctrl_CheckMaxSpeed( & CTRL);
    VIN_SEND();
  }
  Ctrl_MaxSpeed_Draw( & CTRL);
  BNSO_SetData();
}

void Ctrl_Timer_MS( void ) {
	Prof_Timer_10ms( &CTRL );
}

void Ctrl_DeInit( void ) {
	GSeMCM_DeInit();
}

// Ниже мусорка
float liner_interpret2(float x1_, float x2_, float y1_, float y2_, float x0_) {
  if (x1_ == x2_) {
    return y1_;
  }
  return y1_ + (y2_ - y1_) * (x0_ - x1_) / (x2_ - x1_);
}
