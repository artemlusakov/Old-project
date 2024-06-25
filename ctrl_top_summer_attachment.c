#include <stdint.h>
#include <UserCEvents.h>
#include <UserCAPI.h>
#include "vartab.h"
#include "objtab.h"
#include "control.h"
#include "gsToVisu.h"
#include "RCText.h"
#include "param.h"
#include "pinout.h"
#include "io.h"
#include "profile.h"
#include "ctrl_brush.h"
#include "calibr_omfb.h"
#include "ctrl_top_summer_attachment.h"

int32_t Liter = 0;


void WaterPump_WaterIntake_C610H(tSummerTop * sTop);
void WaterPump_WaterIntake_NCR(tSummerTop * sTop);
void Ctrl_MF500Bar(tMF500Bar * MF500Bar);
void WaterPump_Switch(tWaterPump * Pump, uint32_t value);
uint8_t WaterPump_IsOn(tWaterPump * Pump);
void WaterPumpIntake_Switch(tWaterPumpIntake * PumpIntake, uint32_t Value);
// void WaterPump_AntiBoost(tSummerTop * sTop, int32_t ABLiter);

void WaterPump_Init(tSummerTop * sTop, uint32_t Type) {
  tControl * Ctrl = CtrlGet();
  const tParamData * Param = ParameterGet();
  const tProfile * Prof = Profile_GetCurrentProfile();

  Ctrl->top.TopType = TOP_SUMMER;

  int C610H[3] = {
    PinOut_Get_MCM_A(1, 3),
    PinOut_Get_Pin_A(1, 3),
    PinOut_Get_ValveType(1, 3)
  };
  int BP300[3] = {
    PinOut_Get_MCM_A(2, 3),
    PinOut_Get_Pin_A(2, 3),
    PinOut_Get_ValveType(2, 3)
  };

  if (Prof -> module[1] == RCTEXT_LI_A_ATTACHMENT_ATTACHMENT_OMB1 ||
    Prof -> module[1] == RCTEXT_LI_A_ATTACHMENT_ATTACHMENT_OMT1 ||
    Prof -> module[1] == RCTEXT_LI_A_ATTACHMENT_ATTACHMENT_OFF_OPF_245) {

    if (!Param -> FrontBrush.NoHumid && !Param -> FrontBrush.HumidFromRoration) { // Нет увлажнения
      sTop -> Pump.Type = Type;
      sTop -> Pump.PumpInit = 1;
    }
  } else {
    sTop -> Pump.Type = Type;
    sTop -> Pump.PumpInit = 1;
  }

  if (Type == WATERPUMP_TYPE_C610H) {
    ValvDisk_Init( & Ctrl -> top.SummerTop.MainValve, MCM250_5, 5);
    switch (C610H[2]) {
    case Valve_PVEO_1:
    case Valve_RG:
      ValvDisk_Init( & sTop -> Pump.Valv, C610H[0], C610H[1]);
      break;
    case Valve_PVEH_16:
      Valv_Init( & sTop -> Pump.Valv, C610H[0], C610H[1], -1, -1);
      break;
    case Valve_PVEH_32:
      Valv_PVG32_Init( & sTop -> Pump.Valv, C610H[0], C610H[1], -1, -1);
      break;
    case Valve_PVEH_OMFB:
      if (sTop -> Pump.PumpInit) {
        Valv_OMFB_Init( & sTop -> Pump.Valv, C610H[0], C610H[1], -1, -1);
        Ctrl_Valve_SetLiter( & sTop -> Pump.Valv.Valve.PVEH, 0);
      }
      break;
    }
  } else if (Type == WATERPUMP_TYPE_BP300) {
    switch (BP300[2]) {
    case Valve_PVEO_1:
    case Valve_RG:
      ValvDisk_Init( & sTop -> Pump.Valv, BP300[0], BP300[1]);
      break;
    case Valve_PVEH_16:
      Valv_Init( & sTop -> Pump.Valv, BP300[0], BP300[1], -1, -1);
      break;
    case Valve_PVEH_32:
      Valv_PVG32_Init( & sTop -> Pump.Valv, BP300[0], BP300[1], -1, -1);
      break;
    case Valve_PVEH_OMFB:
      if (sTop -> Pump.PumpInit) {
        Valv_OMFB_Init( & sTop -> Pump.Valv, BP300[0], BP300[1], -1, -1);
        Ctrl_Valve_SetLiter( & sTop -> Pump.Valv.Valve.PVEH, 0);
      }
      break;
    }
  }
}

void WaterPump_Intake_Init(tSummerTop * sTop, int32_t Device, int32_t Pos, int32_t TypeIntake) {
  if (Device != -1 && Pos != -1) {
    ValvDisk_Init( & sTop -> PumpIntake.StraitValv, Device, Pos);
  }

  sTop -> PumpIntake.TypeIntake = TypeIntake;
  sTop -> PumpIntake.FuncWaterIntake = 1;

  int C610H[3] = {
    PinOut_Get_MCM_A(1, 3),
    PinOut_Get_Pin_A(1, 3),
    PinOut_Get_ValveType(1, 3)
  };
  // int BP300[3] = {
  //   PinOut_Get_MCM_A(2, 3),
  //   PinOut_Get_Pin_A(2, 3),
  //   PinOut_Get_ValveType(2, 3)
  // };

  if (TypeIntake == WATERPUMP_TYPE_C610H || TypeIntake == WATERPUMP_TYPE_NCR) {
    if(TypeIntake == WATERPUMP_TYPE_C610H) {
      ValvDisk_Init( & sTop->MainValve, MCM250_5, 5);
    }
    switch (C610H[2]) {
    case Valve_PVEO_1:
    case Valve_RG:
      ValvDisk_Init( & sTop -> PumpIntake.ValvIntake, C610H[0], C610H[1]);
      break;
    case Valve_PVEH_16:
      Valv_Init( & sTop -> PumpIntake.ValvIntake, C610H[0], C610H[1], -1, -1);
      break;
    case Valve_PVEH_32:
      Valv_PVG32_Init( & sTop -> PumpIntake.ValvIntake, C610H[0], C610H[1], -1, -1);
      break;
    case Valve_PVEH_OMFB:
      Valv_OMFB_Init( & sTop -> PumpIntake.ValvIntake, C610H[0], C610H[1], -1, -1);
      Ctrl_Valve_SetLiter( & sTop -> PumpIntake.ValvIntake.Valve.PVEH, 0);
      break;
    }
  }

  // Отображение иконки забора воды на маске Main
  SendToVisuObj(OBJ_INDEXBITMAP845, GS_TO_VISU_SET_ATTR_VISIBLE, 1);
  SendToVisuObj(OBJ_SIMPLEBITMAP840, GS_TO_VISU_SET_ATTR_VISIBLE, 1);
  SendToVisuObj(OBJ_INDEXBITMAP1216, GS_TO_VISU_SET_ATTR_VISIBLE, 1);
}

void WaterPump_Cycle(tSummerTop * sTop) {
  const tControl * Ctrl = CtrlGet();
  Ctrl_MF500Bar( & sTop -> MF500Bar);

  if (Ctrl -> cmd_no_key.E_S) {
    WaterPump_Switch( & sTop -> Pump, PUMP_ON);
  } else {
    WaterPump_Switch( & sTop -> Pump, PUMP_OFF);
  }

  if (WaterPump_IsOn( & sTop -> Pump)) {
    // Управление подачей на насосы
    if(!GetFlagTopCalibr()) { // Проверка включения калибровки
      if (sTop -> Pump.Type == WATERPUMP_TYPE_BP300) {
        Ctrl_Valve_SetLiter( & sTop -> Pump.Valv.Valve.PVEH, -Liter);
        // WaterPump_AntiBoost( & Ctrl -> top.SummerTop, -17000);
      } else if (sTop -> Pump.Type == WATERPUMP_TYPE_C610H) {
        Ctrl_Valve_SetLiter( & sTop -> Pump.Valv.Valve.PVEH, Liter);
        // WaterPump_AntiBoost( & Ctrl -> top.SummerTop, 17000);
        Valv_Set( & sTop -> MainValve, 1);
      }
    }
  } else {
    Ctrl_Valve_SetLiter( & sTop -> Pump.Valv.Valve.PVEH, 0);
    Valv_Set( & sTop -> MainValve, 0);
  }

  // Выбор типа забора воды в зависимости от установленного насоса
  if (sTop -> PumpIntake.TypeIntake == WATERPUMP_TYPE_C610H) {
    WaterPump_WaterIntake_C610H( & Ctrl -> top.SummerTop);
  } else if (sTop -> PumpIntake.TypeIntake == WATERPUMP_TYPE_NCR) {
    WaterPump_WaterIntake_NCR( & Ctrl -> top.SummerTop);
  }
}

/**
 * @brief Цикл работы забора воды при насосе NCR
 *
 * @param sTop
 */
void WaterPump_WaterIntake_NCR(tSummerTop * sTop) {
  tParamData * Param = ParameterGet();

  static int32_t Count = 0;
  static int32_t GlobalTmr = 0;
  static uint8_t FlagPumpOFF;

  if (TopGetSpeed()) {
    WaterPumpIntake_Switch( & sTop -> PumpIntake, 0);
  }

  if (WaterIntake_IsOn( & sTop -> PumpIntake) == 1) {
    SetVar(HDL_WATERINTAKE, 1);
    Ctrl_IntakeValve_SetLiter( & sTop -> PumpIntake.ValvIntake.Valve.PVEH, 0);
    if (1000 < (int32_t)(GetMSTick() - Count)) {
      Count = GetMSTick();
      GlobalTmr = GlobalTmr + 1;
    }

    if (GlobalTmr < 15) {
      Valv_Set( & sTop -> PumpIntake.StraitValv, 1);
    } else {
      Valv_Set( & sTop -> PumpIntake.StraitValv, 0);
    }

    if (GlobalTmr >= 16) {
      Ctrl_IntakeValve_SetLiter( & sTop -> PumpIntake.ValvIntake.Valve.PVEH, Param->OMFB.PWM_C610H);
    }

    if (GlobalTmr >= 720) {
      Ctrl_IntakeValve_SetLiter( & sTop -> PumpIntake.ValvIntake.Valve.PVEH, 0);
      Valv_Set( & sTop -> PumpIntake.StraitValv, 0);
      WaterPumpIntake_Switch( & sTop -> PumpIntake, 0);
    }
    FlagPumpOFF = 1;
  } else {
    if (FlagPumpOFF == 1) {
      Ctrl_IntakeValve_SetLiter( & sTop -> PumpIntake.ValvIntake.Valve.PVEH, 0);
      Valv_Set( & sTop -> PumpIntake.StraitValv, 0);
      SetVar(HDL_WATERINTAKE, 0);

      GlobalTmr = 0;
      FlagPumpOFF = 0;

      WaterPumpIntake_Switch( & sTop -> PumpIntake, 0);
    }
  }
}

/**
 * @brief Цикл работы забора воды при насосе C610H
 *
 * @param sTop
 */
void WaterPump_WaterIntake_C610H(tSummerTop * sTop) {
  tParamData * Param = ParameterGet();
  static int32_t Count = 0;
  static int32_t GlobalTmr = 0;
  static uint8_t FlagPumpOFF;

  if (TopGetSpeed()) {
    WaterPumpIntake_Switch( & sTop -> PumpIntake, 0);
  }

  if (WaterIntake_IsOn( & sTop -> PumpIntake) == 1) {
    SetVar(HDL_WATERINTAKE, 1);
    Valv_Set( & sTop -> MainValve, 1);
    Ctrl_IntakeValve_SetLiter( & sTop -> PumpIntake.ValvIntake.Valve.PVEH, Param->OMFB.PWM_C610H);

    if (1000 < (int32_t)(GetMSTick() - Count)) {
      Count = GetMSTick();
      GlobalTmr = GlobalTmr + 1;
    }

    if (GlobalTmr >= 720) {
      Valv_Set( & sTop -> MainValve, 0);
      Ctrl_IntakeValve_SetLiter( & sTop -> PumpIntake.ValvIntake.Valve.PVEH, 0);
      WaterPumpIntake_Switch( & sTop -> PumpIntake, 0);
    }
    FlagPumpOFF = 1;
  } else {
    if (FlagPumpOFF == 1) {
      Valv_Set( & sTop -> MainValve, 0);
      Ctrl_IntakeValve_SetLiter( & sTop -> PumpIntake.ValvIntake.Valve.PVEH, 0);
      SetVar(HDL_WATERINTAKE, 0);

      GlobalTmr = 0;
      FlagPumpOFF = 0;
    }
  }
}

void WaterPump_Draw(tSummerTop * sTop) {
  const tParamData * Param = ParameterGet();
  const tProfile * Prof = Profile_GetCurrentProfile();
  const tControl * Ctrl = CtrlGet();

  static int32_t flagVisiblePump = 0;
  static uint8_t flagSpray = 0;

  if (sTop -> Pump.Valv.Type != VALVE_TYPE_NONE) {
    if (!flagVisiblePump) {
      SendToVisuObj(OBJ_WATERPUMP, GS_TO_VISU_SET_ATTR_VISIBLE, 1);
      if (!sTop -> Pump.PumpInit) {
        SendToVisuObj(OBJ_WATERPUMP, GS_TO_VISU_SET_ATTR_VISIBLE, 0);
      }
      flagVisiblePump = 1;
    }
    if (WaterPump_IsOn( & sTop -> Pump)) {
      flagSpray = 1;
      SetVarIndexed(IDX_ICON_PUMP, 1);
    } else if (WaterIntake_IsOn( & sTop -> PumpIntake) == 1) {
      SetVarIndexed(IDX_ICON_PUMP, 1);
      flagSpray = 0;
    } else if (MF500Bar_IsOn( & sTop -> MF500Bar)) {
      SetVarIndexed(IDX_ICON_PUMP, 1);
      flagSpray = 0;
    } else {
      flagSpray = 0;
      SetVarIndexed(IDX_ICON_PUMP, 0);
    }

    if (Param -> FrontBrush.HumidFromRoration) {
      if (Prof -> module[1] == RCTEXT_LI_A_ATTACHMENT_ATTACHMENT_OMB1 ||
        Prof -> module[1] == RCTEXT_LI_A_ATTACHMENT_ATTACHMENT_OMT1 ||
        Prof -> module[1] == RCTEXT_LI_A_ATTACHMENT_ATTACHMENT_OFF_OPF_245) {
        if (Brush_Active( & Ctrl -> ABC[EQUIP_A].BrushValve)) {
          flagSpray = 1;
        } else {
          flagSpray = 0;
        }
      }
    }
  } else {
    if (!flagVisiblePump) {
      SendToVisuObj(OBJ_WATERPUMP, GS_TO_VISU_SET_ATTR_VISIBLE, 0);
      flagVisiblePump = 1;
    }
  }
  SetVarIndexed(IDX_WATER_SPRAY, flagSpray);
}

/**
 * @brief Переключатель состояния работы насоса
 *
 * @param Pump
 * @param Value 1 - вкл, 0 - выкл
 */
void WaterPump_Switch(tWaterPump * Pump, uint32_t Value) {
  if (Pump -> PumpInit) {
    if (Value == PUMP_ON) {
      if ((TopGetSpeed() >= GetVar(HDL_WORKSPEED)) &&
        (!GetVarIndexed(IDX_JOYSTICKSTATE))) {
        Pump -> on = Value;
      }
    } else {
      Pump -> on = Value;
    }
  }
}

/**
 * @brief Функция проверки состояния работы насоса
 *
 * @param Pump
 * @return uint8_t 1 - вкл, 0 - выкл
 */
uint8_t WaterPump_IsOn(tWaterPump * Pump) {
  if ((Pump -> on == PUMP_ON) &&
      (TopGetSpeed() >= GetVar(HDL_WORKSPEED))) {
    return 1;
  } else {
    return 0;
  }
}

/**
 * @brief Изменение распиновки водяного насоса для OKB600
 *
 */
void WaterPump_ChangePinout(tWaterPump * Pump, int8_t MCM, int8_t Out) {
  Pump->Valv.Valve.PVEH.MCM_Device = MCM;
  Pump->Valv.Valve.PVEH.output = Out;
}

/**
 * @brief Переключатель состояния работы насоса на забор воды
 *
 * @param PumpIntake
 * @param Value 1 - вкл, 0 - выкл
 */
void WaterPumpIntake_Switch(tWaterPumpIntake * PumpIntake, uint32_t Value) {
  if (PumpIntake -> FuncWaterIntake) {
    if (Value == PUMP_ON) {
      if (!TopGetSpeed()) {
        PumpIntake -> on = Value;
      }
    } else {
      PumpIntake -> on = Value;
    }
  }
}

/**
 * @brief Функция проверки состояния работы насоса на забор воды
 *
 * @return uint8_t 1 - вкл, 0 - выкл
 */
uint8_t WaterIntake_IsOn(tWaterPumpIntake * PumpIntake) {
  if (PumpIntake -> on == PUMP_ON) {
    return 1;
  } else {
    return 0;
  }
}

/**
 * @brief Управление мойкой высокого давления 500 бар
 *
 * @param MF500Bar
 */
void Ctrl_MF500Bar(tMF500Bar * MF500Bar) {
  tControl * Ctrl = CtrlGet();
  static uint8_t flagOff = 0;

  int Pinout_MF500Bar[2] = {
    PinOut_Get_MCM_A(3, 3),
    PinOut_Get_Pin_A(3, 3),
  };

  if (Ctrl -> sens.DI.MF500Bar) {
    MF500Bar -> on = 1;
  } else {
    MF500Bar -> on = 0;
  }

  if (MF500Bar -> on) {
    CtrlSetDigitalOut(Pinout_MF500Bar[0], Pinout_MF500Bar[1], 1);
    flagOff = 1;
  } else {
    if (flagOff) {
      CtrlSetDigitalOut(Pinout_MF500Bar[0], Pinout_MF500Bar[1], 0);
      flagOff = 0;
    }
  }
}

/**
 * @brief Функция проверки состояния работы мойки высокого давления
 *
 * @param MF500Bar
 * @return uint8_t 1 - вкл, 0 - выкл
 */
uint8_t MF500Bar_IsOn(tMF500Bar * MF500Bar) {
  if (MF500Bar -> on == PUMP_ON) {
    return 1;
  } else {
    return 0;
  }
}

void LRS_Timer_10ms(tLRS * lrs) {
  static int32_t count = 0;
  count++;
  if (0 == (count % 10)) {
    ValvProp_Timer_100ms( & lrs -> DensityValve.Valve.PVEH);
  }
}

int32_t SetBit(uint32_t * Byte, int8_t Pos, int8_t value) {
  if (Pos >= 32)
    return -1;
  if (value) {
    * Byte = * Byte | (0x01 << Pos);
  } else {
    * Byte = * Byte & (~(0x01 << Pos));
  }
  return 0;
}

void RR300_Init(tSummerTop * sTop) {
  ValvDisk_Init( & sTop->LRS.CenterValve, MCM250_5, 5);
}

void RR300_Cycle(tSummerTop * sTop) {
  const tControl * Ctrl = CtrlGet();
  tLRS * lrs = & sTop -> LRS;
  Valv_Set( & sTop -> LRS.CenterValve, Ctrl -> cmd_no_key.E_S);
  if (Ctrl -> cmd_no_key.E_S) {
    lrs -> ActiveSection = LRS_SECTION_BIN_CENTER;
  } else {
    lrs -> ActiveSection = 0;
  }
}

void RR400_Init(tSummerTop * sTop) {
  ValvDisk_Init( & sTop -> LRS.CenterValve, MCM250_5, 5);
  ValvDisk_Init( & sTop -> LRS.LeftValve, MCM250_5, 3);
  ValvDisk_Init( & sTop -> LRS.RightValve, MCM250_5, 7);
}

void RR400_Cycle(tSummerTop * sTop) {
  const tControl * Ctrl = CtrlGet();
  int linecounter = Ctrl -> cmd_no_key.E_S; // Количество активных линий
  tLRS * lrs = & sTop -> LRS;
  Valv_Set( & sTop -> LRS.CenterValve, Ctrl -> cmd_no_key.E_S);

  SetBit( & lrs -> ActiveSection, 1, Ctrl -> cmd_no_key.E_S);
  if (Ctrl -> cmd_no_key.E_S) {
    if (Ctrl -> cmd_no_key.Active) {
      if (0 < IOT_AnaJoy_GetInPercent( & Ctrl -> joystick.Y)) {
        lrs -> ActiveSection = LRS_SECTION_ALL;
      }
      if (0 > IOT_AnaJoy_GetInPercent( & Ctrl -> joystick.Y)) {
        lrs -> ActiveSection = LRS_SECTION_CENTER;
      }
      if (0 < IOT_AnaJoy_PressedNew( & Ctrl -> joystick.Z)) {
        if (lrs -> ActiveSection & LRS_SECTION_BIN_LEFT) {
          SetBit( & lrs -> ActiveSection, 2, 0);
        } else {
          SetBit( & lrs -> ActiveSection, 2, 1);
        }
      } else if (0 > IOT_AnaJoy_PressedNew( & Ctrl -> joystick.Z)) {
        if (lrs -> ActiveSection & LRS_SECTION_BIN_RIGHT) {
          SetBit( & lrs -> ActiveSection, 0, 0);
        } else {
          SetBit( & lrs -> ActiveSection, 0, 1);
        }
      }
    }
  } else {
    lrs -> ActiveSection = 0;
  }

  //left valve
  Valv_Set( & lrs -> LeftValve, lrs -> ActiveSection & LRS_SECTION_BIN_LEFT);

  //right valve
  Valv_Set( & lrs -> RightValve, lrs -> ActiveSection & LRS_SECTION_BIN_RIGHT);

  if (lrs -> ActiveSection % 3 == 0 && lrs -> ActiveSection > 0) { // Если ActiveSection 3 или 6 значит активных линий 2
    linecounter = 2;
  } else if (lrs -> ActiveSection == 7) { // Если ActiveSection 7 зачит активных линий 3
    linecounter = 3;
  }

  // Если есть активные линии и скорость больше рабочей то выщитываем мл/м2 по формуле
  if (lrs -> ActiveSection > 0 && TopGetSpeed() > GetVar(HDL_WORKSPEED)) {
    SetVar(HDL_DENSITYRR400, (int)((double) Consumption_RR400 / (double) Width_Line / (double) linecounter / (double) TopGetSpeed() * (double) 60));
  } else {
    SetVar(HDL_DENSITYRR400, 0);
  }
}

void RR300_RR400_Draw(tSummerTop * sTop) {
  SetVarIndexed(IDX_ROADLINEM, (sTop -> LRS.ActiveSection & LRS_SECTION_BIN_CENTER) ? 1 : 0);
  SetVarIndexed(IDX_ROADLINEL, (sTop -> LRS.ActiveSection & LRS_SECTION_BIN_LEFT) ? 1 : 0);
  SetVarIndexed(IDX_ROADLINER, (sTop -> LRS.ActiveSection & LRS_SECTION_BIN_RIGHT) ? 1 : 0);
}

void WaterPump_SetMaxLiter(int32_t ml) {
  if (0 == Liter)
    Liter = ml;
  else if (ml < Liter) {
    Liter = ml;
  }
}
