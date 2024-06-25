#include <stdint.h>
#include <UserCEvents.h>
#include <UserCAPI.h>
#include "vartab.h"
#include "objtab.h"
#include "gsToVisu.h"
#include "ctrl_top_conveyor.h"
#include "visu_material.h"
#include "RCColor.h"
#include "drawfunction.h"
#include "ctrl_top_adrive.h"
#include "ctrl_top_adrive_feedback.h"
#include "ctrl_top_winter_attachment.h"
#include "rtr_uh.h"
#include "param.h"
#include "profile.h"
#include "pinout.h"
#include "control.h"
#include "io.h"
#include "RCText.h"
#include "gse_msgbox.h"
#include "calibr_omfb.h"


void VisuMain_Draw_LiquidSensor(const tControl * Ctrl);

void Ctrl_RTR_Init(tControl * Ctrl) {
  const tParamData * Param = ParameterGet();
  switch (Param->RTR.TypeRTR) {
  case 1: // KS/SH, Соргаз
  case 2:
  case 3:
    Ctrl_RTR_Default_Init(&Ctrl->top.WinterTop);
    break;

  case 4: // Уника
  case 5:
    Ctrl_RTR_UH_init(Ctrl);
    break;

  default:
    break;
  }
}

void Ctrl_RTR_Cycle(tControl * Ctrl, uint32_t evtc, tUserCEvt * evtv) {
  const tParamData * Param = ParameterGet();
  switch (Param->RTR.TypeRTR) {
  case 1: // KS/SH, Соргаз
  case 2:
  case 3:
    Ctrl_RTR_Default_Cycle(&Ctrl->top.WinterTop, evtc, evtv);
    break;

  case 4: // Уника
  case 5: // Уника
    Ctrl_RTR_UH_cycle(Ctrl, evtc, evtv);
    break;

  default:
    break;
  }
}

void Ctrl_RTR_Timer_10ms(tControl * Ctrl) {
  const tParamData * Param = ParameterGet();
  tWinterTop * wTop = & Ctrl -> top.WinterTop;
  static int32_t count = 0;
  // Таймер используется только для KH/SH, Соргаз
  switch (Param -> RTR.TypeRTR) {
  case 1: // KS/SH, Соргаз
  case 2:
  case 3:
    count++;
    if (0 == count % 10) {
      ConveyorTimer( & wTop -> SpreadConv);
      SpreaderDiskTimer( & wTop -> SpreadDisk);
      Valv_Timer_100ms( & wTop -> ValveMain);
    }
    if (Param -> RTR.AsymType == 1) { // SH
      ADrive_Timer_10ms( & Ctrl -> top.WinterTop.Adrive);
    }
    break;

  case 4:
  case 5:
    ctrl_top_RTR_UH_timer10ms(Ctrl);
    break;
  }
}

void Ctrl_RTR_Default_Init(tWinterTop * wTop) {
  tControl * Ctrl = CtrlGet();
  // tWinterTop * wTop = & Ctrl -> top.WinterTop;
  const tParamData * Param = ParameterGet();

  Ctrl->top.TopType = TOP_WINTER;

  // Инициализация клапана подачи ПС
  int RTR[3] = {
    PinOut_Get_MCM_A(0, 3),
    PinOut_Get_Pin_A(0, 3),
    PinOut_Get_ValveType(0, 3)
  };
  switch (RTR[2]) {
  case Valve_PVEO_1:
  case Valve_RG:
    ValvDisk_Init( & wTop -> ValveMain, RTR[0], RTR[1]);
    break;
  case Valve_PVEH_16:
    Valv_Init( & wTop -> ValveMain, RTR[0], RTR[1], -1, -1);
    break;
  case Valve_PVEH_32:
    Valv_PVG32_Init( & wTop -> ValveMain, RTR[0], RTR[1], -1, -1);
    break;
  case Valve_PVEH_OMFB:
    Valv_OMFB_Init( & wTop -> ValveMain, RTR[0], RTR[1], -1, -1);
    break;
  default:
    break;
  }

  // Инициализация конвейера и диска
  ConveyorInit( & wTop -> SpreadConv, MCM250_4, 1);
  SpreaderDiskInit( & wTop -> SpreadDisk, MCM250_4, 3);

  // Инициализация асимметрии
  switch (Param->RTR.AsymType) {
  case 0: // Отсутствует
    DrawNoAsym();
    break;
  case 1: // KH/SH
    AsymDrive_Init(wTop);
    break;
  case 2: // Соргаз
    AsymDriveFB_Init(wTop);
    break;
  default:
    break;
  }

  // Инициализация увлажнения
  switch (Param->RTR.HumidType) {
    case 0: // Отсутствует
      break;
    case 1: // KH/SH
      HumidAvailable_Switch(&wTop->RTRHumid, 1);
      break;
    case 2: // Соргаз
      RTR_Humid_Init(&wTop->RTRHumid);
      break;
  }

  // Инициализация шибера
  wTop -> SpreadConv.initShield = 1;

  wTop -> SpreadDensity = GetVarIndexed(IDX_DENSITY);
  wTop -> SpreadWidth = GetVarIndexed(IDX_SPREADERWIDTH);
  wTop -> RTRHumid.HumidPercent = GetVarIndexed(IDX_RTR_HUMIDPERCENT);

  PaintText_Init( &wTop->SpreadConv.ColourDensity, RCCOLOR_NAME_SELECT, RCCOLOR_NAME_FRONT, OBJ_RTR_DENSITY);
  PaintText_Init( &wTop->SpreadDisk.ColourWidth, RCCOLOR_NAME_SELECT, RCCOLOR_NAME_FRONT, OBJ_RTR_WIDTH);
  PaintText_Init( &wTop->RTRHumid.ColourPercent, RCCOLOR_NAME_SELECT, RCCOLOR_NAME_FRONT, OBJ_RTR_HUMID_PERCENT);
}

void Ctrl_RTR_Default_Cycle(tWinterTop * wTop, uint32_t evtc, tUserCEvt * evtv) {
  tControl * Ctrl = CtrlGet();
  const tParamData * Param = ParameterGet();

  // Рабочий режим
  if (Ctrl -> cmd_no_key.E_S) {
    // Подача на ПС
    switch (wTop -> ValveMain.Type) {
    case VALVE_TYPE_DISK:
      Valv_Set( & wTop -> ValveMain, 1);
      break;
    case VALVE_TYPE_PVE_AHS:
      ValvPropPVEP_SetLiter( & wTop -> ValveMain.Valve.PVEH, 65000);
      break;
    case VALVE_TYPE_PVE_AHS32:
    case VALVE_TYPE_OMFB:
      if(!GetFlagTopCalibr()) {
        Valv_Set( & wTop -> ValveMain, Param -> OMFB.PWM_RTR);
      }
      break;
    default:
      break;
    }

    // Включение конвейера и диска
    Conveyor_Switch( & wTop -> SpreadConv, 1);
    SpreaderDisk_Switch( & wTop -> SpreadDisk, 1);

    // Включение асимметрии
    switch (Param -> RTR.AsymType) {
    case 0:
      break;
    case 1: // KH/SH
      adrv_on( & Ctrl -> top.WinterTop.Adrive, GetVar(HDL_MODEASSIMETRY));
      break;
    case 2: // Соргаз
      AdriveFeedback_on( & Ctrl -> top.WinterTop.AdriveFB, 3);
      break;
    default:
      break;
    }

    // Разгрузка
  } else if (RTRDump_IsOn( & Ctrl -> top.WinterTop)) {
    switch (wTop -> ValveMain.Type) {
    case VALVE_TYPE_DISK:
      Valv_Set( & wTop -> ValveMain, 1);
      break;
    case VALVE_TYPE_PVE_AHS:
      ValvPropPVEP_SetLiter( & wTop -> ValveMain.Valve.PVEH, 65000);
      break;
    case VALVE_TYPE_PVE_AHS32:
    case VALVE_TYPE_OMFB:
      Valv_Set( & wTop -> ValveMain, Param -> OMFB.PWM_RTR);
      break;
    default:
      break;
    }

    Conveyor_Switch( & wTop -> SpreadConv, 1);
    SpreaderDisk_Switch( & wTop -> SpreadDisk, 0);

    switch (Param -> RTR.AsymType) {
    case 0:
      break;
    case 1: // KH/SH
      adrv_off( & Ctrl -> top.WinterTop.Adrive, GetVar(HDL_MODEASSIMETRY));
      break;
    case 2: // Соргаз
      AdriveFeedback_off( & Ctrl -> top.WinterTop.AdriveFB, GetVar(HDL_MODEASSIMETRY));
      break;
    default:
      break;
    }

  } else {
    Valv_Set( & wTop -> ValveMain, 0);

    Conveyor_Switch( & wTop -> SpreadConv, 0);
    SpreaderDisk_Switch( & wTop -> SpreadDisk, 0);

    switch (Param -> RTR.AsymType) {
    case 0:
      break;
    case 1: // KH/SH
      adrv_off( & Ctrl -> top.WinterTop.Adrive, GetVar(HDL_MODEASSIMETRY));
      break;
    case 2: // Соргаз
      AdriveFeedback_off( & Ctrl -> top.WinterTop.AdriveFB, GetVar(HDL_MODEASSIMETRY));
      break;
    default:
      break;
    }
  }

  // Управление конвейером и диском
  ConveyorCycle( & wTop -> SpreadConv, IOT_NormAI( & Ctrl -> sens.AI.OilTemp));
  SpreaderDiskCycle( & wTop -> SpreadDisk, wTop -> SpreadWidth);

  // Управление асимметрией
  switch (Param -> RTR.AsymType) {
  case 0:
    break;
  case 1: // KH/SH
    AdriveCommand( & Ctrl -> top.WinterTop.Adrive, Ctrl -> cmd_no_key.Z_Change);
    ADrive_Draw( & Ctrl -> top.WinterTop.Adrive);
    break;
  case 2: // Соргаз
    Ctrl_AdriveFeedback_Cycle( & Ctrl -> top.WinterTop.AdriveFB);
    break;
  default:
    break;
  }

  // Управление плотностью и шириной посыпания
  RTR_Command(wTop);

  // Контроль вращения конвейера и наличия материала
  Material_Spreading_Sensor_Cycle();

  // Контроль увлажнения
  switch (Param->RTR.HumidType) {
  case 0:
    break;
  case 1:
    VisuMain_Draw_LiquidSensor(Ctrl);
    break;
  case 2:
    RTR_Humid_Cycle(wTop);
    VisuMain_Draw_LiquidSensor(Ctrl);
  default:
    break;
  }
  // Управление светом на КДМ
  Light_Draw( & Ctrl -> light);

  // Буст
  Top_Boost_Cycle(&Ctrl->top);

  // Отрисовка графики
  RTR_Draw(wTop);

}

// Увлажнение
void HumidAvailable_Switch( tRTRHumid * Humid, int32_t value ) {
	Humid->HumidAvailable = value;
  SendToVisuObj(OBJ_ICONLIQUIDSTATE, GS_TO_VISU_SET_ATTR_VISIBLE, value);
  SendToVisuObj(OBJ_SIMPLEBITMAP380, GS_TO_VISU_SET_ATTR_VISIBLE, value);
  SendToVisuObj(OBJ_INDEXBITMAP139, GS_TO_VISU_SET_ATTR_VISIBLE, value);

  SetVar(HDL_RTRIMG, 1);
}

void RTR_Humid_Switch(tRTRHumid * Humid, int32_t value) {
  Humid->on = value;
  SetVar(HDL_WETTINGSENSORACTIVE, value);
}

uint8_t RTR_Humid_IsOn(tRTRHumid * Humid) {
  if (Humid -> on) {
    return 1;
  } else {
    return 0;
  }
}

// Используется для Соргаза
void RTR_Humid_Init(tRTRHumid * Humid) {
  HumidAvailable_Switch(Humid, 1);
  ValveProp_Init( & Humid -> ValvHumid, MCM250_4, 7, 1000, 10);

  SendToVisuObj(OBJ_RTR_HUMID_PERCENT, GS_TO_VISU_SET_ATTR_VISIBLE, 1);
  SendToVisuObj(OBJ_TRUETYPETEXT101, GS_TO_VISU_SET_ATTR_VISIBLE, 1);
  SendToVisuObj(OBJ_SIMPLEBITMAP1370, GS_TO_VISU_SET_ATTR_VISIBLE, 1);
  SendToVisuObj(OBJ_TRUETYPETEXT1377, GS_TO_VISU_SET_ATTR_VISIBLE, 1);
}


void RTR_Humid_Cycle(tWinterTop * wTop) {
  const tControl * Ctrl = CtrlGet();
  if(RTR_Humid_IsOn(&wTop->RTRHumid) && wTop->SpreadConv.on) {
    if(GetVar(HDL_WETTINGSENSORSTATE) == 1) { // IDXBITMAP_WETTINGSENSOR_FULL
      ValvProp_Set( & Ctrl->top.WinterTop.RTRHumid.ValvHumid, GetPWM_RTRHumid());
      EList_ResetErr( ERRID_NO_WATERSENSOR );
    } else if(GetVar(HDL_WETTINGSENSORSTATE) == 2) { // IDXBITMAP_WETTINGSENSOR_EMPTY
      EList_SetErrLevByRCText( VBL_WARNING, ERRID_NO_WATERSENSOR, RCTEXT_T_WATERSENSORNOTAVAILABLE );
      ValvProp_Set( & Ctrl->top.WinterTop.RTRHumid.ValvHumid, 0);
    }
  } else {
    RTR_Humid_Switch(&wTop->RTRHumid, 0);
    ValvProp_Set( & Ctrl->top.WinterTop.RTRHumid.ValvHumid, 0);
    EList_ResetErr( ERRID_NO_WATERSENSOR );
  }
  RTR_Humid_Percent(wTop);
}

void RTR_Humid_Percent(tWinterTop * wTop) {
  const tControl * ctrl_ = CtrlGet();
  static uint64_t tmr, tmr1 = 0;
  static uint8_t flagTmr, flagTmr1 = 0;
  static int8_t Percent = 0;
  if ((!IOT_Button_IsDown( & ctrl_ -> joystick.Button[JSB_A])) &&
    (!IOT_Button_IsDown( & ctrl_ -> joystick.Button[JSB_B])) &&
    (!IOT_Button_IsDown( & ctrl_ -> joystick.Button[JSB_C]))) {
    if (IOT_Button_IsDown( & ctrl_ -> joystick.Button[JSB_F])) {
      if ((IOT_AnaJoy_GetInPercent( & ctrl_ -> joystick.X) * 10) > 500) {
        flagTmr1 = 0;
        if (!flagTmr) {
          tmr1 = GetMSTick();
          flagTmr = 1;
        }
        if (GetMSTick() - tmr > 250) {
          tmr = GetMSTick();
          Percent = 5;
        } else {
          Percent = 0;
        }
      } else if ((IOT_AnaJoy_GetInPercent( & ctrl_ -> joystick.X) * 10) < -500) {
        flagTmr = 0;
        if (!flagTmr1) {
          tmr = GetMSTick();
          flagTmr1 = 1;
        }
        if (GetMSTick() - tmr1 > 250) {
          tmr1 = GetMSTick();
          Percent = -5;
        } else {
          Percent = 0;
        }
      } else {
        Percent = 0;
      }
      wTop -> RTRHumid.HumidPercent = wTop -> RTRHumid.HumidPercent + Percent;
      CheckRange((int32_t * ) & wTop -> RTRHumid.HumidPercent, 0, 40);
      PaintText_Cycle( & wTop -> RTRHumid.ColourPercent, IOT_AnaJoy_GetInPercent( & ctrl_ -> joystick.X));
    }
  } else {
    PaintText_Cycle( & wTop -> RTRHumid.ColourPercent, 0);
  }
  SetVar(HDL_RTR_HUMIDPERCENT, wTop -> RTRHumid.HumidPercent);
}


void RTR_Humid_ManualMode_Switch(tRTRHumid * Humid, int32_t value) {
  static int8_t oldMode = -1;
  Humid->ManualMode = value;

  if(oldMode != Humid->ManualMode) {
    oldMode = Humid->ManualMode;

    if (Humid->ManualMode) {
      SendToVisuObj(OBJ_TRUETYPETEXT1377, GS_TO_VISU_SET_FG_COLOR, 0x00FF00);
    } else {
      SendToVisuObj(OBJ_TRUETYPETEXT1377, GS_TO_VISU_SET_FG_COLOR, 0xEEEEEE);
    }
  }
}

void VisuMain_Draw_LiquidSensor(const tControl * Ctrl) {
  const tParamData * Param = ParameterGet();
  uint32_t liquid_sens_;
  static uint32_t _liquid_sens_old_ = INT32_MAX;
  static uint32_t tmtHumidSens = 0;

  // Определение типа датчика
  static int8_t LiquidSensorType = -1;
  if (Param -> Sensors.NormOpen) {
    LiquidSensorType = 0; // Нормально открытый
  } else if (Param -> Sensors.NormClosed) {
    LiquidSensorType = 1; // Нормально закрытый
  }

  if (RTR_Humid_IsOn(&Ctrl->top.WinterTop.RTRHumid)) {
    if (GetVar(HDL_SW_HUMID_SENS)) {
      switch (LiquidSensorType) {
      case 0: // Нормально открытый
        if (Ctrl -> sens.DI.Humidifier == DI_SENSOR_ON) {
          tmtHumidSens = GetMSTick();
        } else {
          // Нет воды
        }
        break;

      case 1: // Нормально закрытый
        if (Ctrl -> sens.DI.Humidifier == DI_SENSOR_ON) {
          // Нет воды
        } else {
          tmtHumidSens = GetMSTick();
        }
        break;

      default:
        break;
      }
    } else {
      tmtHumidSens = GetMSTick();
    }

    if (4000 < (GetMSTick() - tmtHumidSens)) {
      liquid_sens_ = IDXBITMAP_WETTINGSENSOR_EMPTY;
    } else {
      liquid_sens_ = IDXBITMAP_WETTINGSENSOR_FULL;
    }
  } else {
    liquid_sens_ = IDXBITMAP_WETTINGSENSOR_NO_SENSOR;
  }
  if (_liquid_sens_old_ != liquid_sens_) {
    _liquid_sens_old_ = liquid_sens_;
    SetVar(HDL_WETTINGSENSORSTATE, liquid_sens_);
    if (IDXBITMAP_WETTINGSENSOR_EMPTY == liquid_sens_) {
      SendToVisuObj(OBJ_ICONLIQUIDSTATE, GS_TO_VISU_SET_ATTR_BLINK, 1);
      SetBuzzer(1, 100, 100, 3);
    } else {
      SendToVisuObj(OBJ_ICONLIQUIDSTATE, GS_TO_VISU_SET_ATTR_BLINK, 0);
    }
  }
}


// Контроль управления разгрузкой
void Ctrl_RTR_Dump(tWinterTop * wTop) {
  static uint32_t keyDump = 0;
  static uint32_t tmrDumpOn = 0;

  if (TopGetSpeed() == 0) {
    if (IsKeyPressedNew(5)) {
      if (OFF == keyDump) {
        keyDump = UNK;
        tmrDumpOn = GetMSTick() + 2000;
      }
    }
  }
  if (IsKeyReleasedNew(5)) { // кнопка опущена
    if (UNK == keyDump && GetMSTick() >= tmrDumpOn) {
      keyDump = ON;
    } else {
      keyDump = OFF;
    }
  }
  if (ON == keyDump) {
    if (5 < TopGetSpeed()) {
      keyDump = OFF;
      SetVar(HDL_DUMP_REAGENT, 0);
    }
  }

  if (OFF == keyDump) {
    SendToVisuObj(OBJ_RTR_DUMP, GS_TO_VISU_SET_ATTR_BLINK, 0);
    SetVar(HDL_DUMP_REAGENT, 0);
    wTop->DumpOn = 0;
  } else if (UNK == keyDump) {
    SendToVisuObj(OBJ_RTR_DUMP, GS_TO_VISU_SET_ATTR_BLINK, 1);
    if (tmrDumpOn < GetMSTick()) {
      SetVar(HDL_DUMP_REAGENT, 1);
      SendToVisuObj(OBJ_RTR_DUMP, GS_TO_VISU_SET_ATTR_BLINK, 0);
    } else {
      SetVar(HDL_DUMP_REAGENT, 0);
    }
  } else if (ON == keyDump) {
    SendToVisuObj(OBJ_RTR_DUMP, GS_TO_VISU_SET_ATTR_BLINK, 0);
    SetVar(HDL_DUMP_REAGENT, 1);
    wTop->DumpOn = 1;
  }
}

uint8_t RTRDump_IsOn(tWinterTop * wTop) {
  if (wTop->DumpOn) {
    return 1;
  } else {
    return 0;
  }
}

/**
 * @brief Инициализация привода асимметрии без обратной связи
 *
 * @param wTop
 */
void AsymDrive_Init(tWinterTop * wTop) {
  const tParamData * Param = ParameterGet();

  // Отображение индикации асимметрии
  SendToVisuObj( OBJ_RTR_ASYM, GS_TO_VISU_SET_ATTR_VISIBLE, 1 );
  SendToVisuObj( OBJ_RTR_ASYM_ACT, GS_TO_VISU_SET_ATTR_VISIBLE, 1 );

  // Настройки распределения
  SendToVisuObj( OBJ_ATRESTTTEXT856, GS_TO_VISU_SET_ATTR_VISIBLE, 1 );
  SendToVisuObj( OBJ_ATRESTTTEXTLIST433, GS_TO_VISU_SET_ATTR_VISIBLE, 1 );
  SendToVisuObj( OBJ_BTN_ASYMCALIBR, GS_TO_VISU_SET_ATTR_VISIBLE, 1 );

  adrv_init( & wTop -> Adrive, MCM250_4, 5, MCM250_4, 8);
  ADrive_TimeCtrl_Init( & wTop -> Adrive, Param -> AsymDrive.timeFullLeft, Param -> AsymDrive.timeFullRight,
    Param -> AsymDrive.timeCntrlMoveStop, Param -> AsymDrive.timeCntrlMoveCalibrationStop,
    Param -> AsymDrive.timeCalibrationAside, Param -> AsymDrive.timeCntrlErrorStop,
    Param -> AsymDrive.timeReversEnable, Param -> AsymDrive.timeReaction);
  ADrive_ICtrl_Init( & wTop -> Adrive, Param -> AsymDrive.I_leftStop, Param -> AsymDrive.I_rightStop);
  ADrive_MoveCntrl_Init( & wTop -> Adrive, Param -> AsymDrive.cntrlMove);
}

/**
 * @brief Инициализация привода асимметрии с обратной связью
 *
 * @param wTop
 */
void AsymDriveFB_Init(tWinterTop * wTop) {

  // Отображение индикации асимметрии
  SendToVisuObj( OBJ_RTR_ASYM, GS_TO_VISU_SET_ATTR_VISIBLE, 1 );
  SendToVisuObj( OBJ_RTR_ASYM_ACT, GS_TO_VISU_SET_ATTR_VISIBLE, 1 );

  // Настройки распределения
  SendToVisuObj( OBJ_ATRESTTTEXT856, GS_TO_VISU_SET_ATTR_VISIBLE, 1 );
  SendToVisuObj( OBJ_ATRESTTTEXTLIST433, GS_TO_VISU_SET_ATTR_VISIBLE, 1 );
  SendToVisuObj( OBJ_BTN_ASYMCALIBR, GS_TO_VISU_SET_ATTR_VISIBLE, 1 );

  AdriveFeedback_Move_Init( & wTop -> AdriveFB, MCM250_4, 5, MCM250_4, 8);
  AdriveFeedback_Init( & wTop -> AdriveFB, MCM250_4, 0);
}

/**
 * @brief Перенос барграфа ширины посыпания под индикацию распределения
 *
 */
void DrawNoAsym(void) {
  SendToVisuObj( OBJ_RTR_SPREAD_BAR, GS_TO_VISU_SET_ORIGIN_Y, 270 );
  SendToVisuObj( OBJ_ATRESTTTEXT715, GS_TO_VISU_SET_ORIGIN_Y, 280 );
  SendToVisuObj( OBJ_RTR_WIDTH, GS_TO_VISU_SET_ORIGIN_Y, 280 );
}

/**
 * @brief Отображение индикации посыпания ПС
 */
void Draw_E_State(void) {
  tControl * Ctrl = CtrlGet();
  static int currentpos;
  static int ImgOnLine;
  static int ImgOffLine;
  if (Ctrl -> cmd_no_key.E_S) {
    if (getFlagUnloading())
      SetVarIndexed(IDX_CHANGE_DRAW_RTR, 2);
    else {
      ImgOffLine = 0;
      if (currentpos <= 2) {
        ImgOnLine++;
        if (ImgOnLine % 20 == 0 && currentpos < 2) {
          currentpos++;
        }
        SetVarIndexed(IDX_CHANGE_DRAW_RTR, currentpos);
      }
    }
  } else if(RTRDump_IsOn( & Ctrl -> top.WinterTop)) {
    SetVarIndexed(IDX_CHANGE_DRAW_RTR, 2);
  } else {
    if (currentpos >= 0 && currentpos <= 3) {
      if (ImgOffLine % 20 == 0) {
        currentpos--;
      }
      ImgOffLine++;
      SetVarIndexed(IDX_CHANGE_DRAW_RTR, currentpos);
    } else {
      SetVarIndexed(IDX_CHANGE_DRAW_RTR, 4);
    }
    ImgOnLine = 0;
  }
}

/**
 * @brief Управление плотностью и шириной распределения
 *
 * @param top_
 */
void RTR_Command(tWinterTop * top_) {
  tControl * ctrl_ = CtrlGet();
  tConveyor * conv_ = & ctrl_ -> top.WinterTop.SpreadConv;
  if ((!IOT_Button_IsDown( & ctrl_ -> joystick.Button[JSB_A])) &&
    (!IOT_Button_IsDown( & ctrl_ -> joystick.Button[JSB_B])) &&
    (!IOT_Button_IsDown( & ctrl_ -> joystick.Button[JSB_C]))) {
    uint32_t SpreadingDensity_Step = 0;
    const tReagent * Reag = GetActualReagent();
    if (NULL != Reag) {
      SpreadingDensity_Step = Reag -> SpreadingDensity_Step;
    }
    // Расчёт плотности посыпания
    if (0 == IOT_Button_IsDown( & ctrl_ -> joystick.Button[JSB_F])) {
      static uint64_t tmr, tmr1 = 0;
      static int8_t densityValue = 0;
      static uint8_t flagTmr, flagTmr1 = 0;
      if ((IOT_AnaJoy_GetInPercent( & ctrl_ -> joystick.X) * 10) > 500) {
        flagTmr1 = 0;
        if (!flagTmr) {
          tmr1 = GetMSTick();
          flagTmr = 1;
        }
        if (GetMSTick() - tmr > 250) {
          tmr = GetMSTick();
          densityValue = 1;
        } else {
          densityValue = 0;
        }
      } else if ((IOT_AnaJoy_GetInPercent( & ctrl_ -> joystick.X) * 10) < -500) {
        flagTmr = 0;
        if (!flagTmr1) {
          tmr = GetMSTick();
          flagTmr1 = 1;
        }
        if (GetMSTick() - tmr1 > 250) {
          tmr1 = GetMSTick();
          densityValue = -1;
        } else {
          densityValue = 0;
        }
      } else {
        densityValue = 0;
      }
      top_ -> SpreadDensity = top_ -> SpreadDensity + densityValue * SpreadingDensity_Step;
      CheckRange((int32_t * ) & top_ -> SpreadDensity, 0, ConveyorGetMaxDensity(conv_));
      PaintText_Cycle( & top_ -> SpreadConv.ColourDensity, IOT_AnaJoy_GetInPercent( & ctrl_ -> joystick.X));
    }
    // Расчёт ширины посыпания
    static uint64_t tmr, tmr1 = 0;
    static int8_t widthValue = 0;
    static uint8_t flagTmr, flagTmr1 = 0;
    if ((IOT_AnaJoy_GetInPercent( & ctrl_ -> joystick.Y) * 10) > 500) {
      flagTmr1 = 0;
      if (!flagTmr) {
        tmr1 = GetMSTick();
        flagTmr = 1;
      }
      if (GetMSTick() - tmr > 250) {
        tmr = GetMSTick();
        widthValue = 1;
      } else {
        widthValue = 0;
      }
    } else if ((IOT_AnaJoy_GetInPercent( & ctrl_ -> joystick.Y) * 10) < -500) {
      flagTmr = 0;
      if (!flagTmr1) {
        tmr = GetMSTick();
        flagTmr1 = 1;
      }
      if (GetMSTick() - tmr1 > 250) {
        tmr1 = GetMSTick();
        widthValue = -1;
      } else {
        widthValue = 0;
      }
    } else {
      widthValue = 0;
    }
    top_ -> SpreadWidth = top_ -> SpreadWidth + widthValue;

    const char * reagName = Reag->Name;
    static uint8_t maxWidth = 12;
    if(strstr(reagName, "Щебень")) {
      maxWidth = 6;
    } else {
      maxWidth = 12;
    }

    CheckRange((int32_t * )( & (top_ -> SpreadWidth)), 2, maxWidth);
    PaintText_Cycle( & top_ -> SpreadDisk.ColourWidth, IOT_AnaJoy_GetInPercent( & ctrl_ -> joystick.Y));
  } else {
    PaintText_Cycle( & top_ -> SpreadConv.ColourDensity, 0);
    PaintText_Cycle( & top_ -> SpreadDisk.ColourWidth, 0);
  }
}

/**
 * @brief Отображение барграфа ширины посыпания
 *
 * @param Width Ширина посыпания
 * @return uint32_t
 */
uint32_t SpreaderBargraphDraw(uint8_t Width) {
  const int32_t MiddleXPos = 110;
  const int32_t Width_Max = 220;
  const int32_t MeterMax = 12;
  static int8_t Width_old = -1;
  if (Width_old != Width) {
    SetVarIndexed(IDX_SPREADINGDUMMY, 100);
    int32_t width = Width * Width_Max / MeterMax;
    int32_t pos_x = MiddleXPos - (width / 2);
    SendToVisuObj(OBJ_RTR_SPREAD_BAR, GS_TO_VISU_SET_ORIGIN_X, pos_x);
    SendToVisuObj(OBJ_RTR_SPREAD_BAR, GS_TO_VISU_SET_WIDTH, width);
    SendToVisuObj(OBJ_BARGRAPH_UH, GS_TO_VISU_SET_ORIGIN_X, pos_x);
    SendToVisuObj(OBJ_BARGRAPH_UH, GS_TO_VISU_SET_WIDTH, width);

    Width_old = Width;
  }
  SetVarIndexed( IDX_SPREADERWIDTH, Width );
  return 0;
}

void RTR_Draw( tWinterTop * wTop ) {
	ConveyorDraw( &wTop->SpreadConv );
	SpreaderBargraphDraw( wTop->SpreadWidth );
	Draw_E_State();
}


/**
 * @brief Контроль вращения конвейера и наличия материала
 *
 */
void Material_Spreading_Sensor_Cycle(void) {
  const tControl * ctrl_ = CtrlGet();
  static int32_t timeOut_Rotation;
  static int32_t timeOut_Material;
  static int32_t timeOut_Dump;
  uint8_t speed = TopGetSpeed();
  uint8_t workSpeed = GetVar(HDL_WORKSPEED);

  //Set Timeout for rotation.
  if ((0 == ctrl_ -> sens.Freq.ConveryRotation) && (ctrl_ -> top.WinterTop.SpreadConv.on)) {
    //if auto mode and speed of vehicle = 0
    if ((0 == speed) || (0 == ctrl_ -> top.WinterTop.SpreadWidth) || (workSpeed >= speed)) {
      timeOut_Rotation = GetMSTick();
    }
    // Ignoring sensors convery rotation
    if (!GetVar(HDL_SW_CONV_ROT)) {
      timeOut_Rotation = GetMSTick();
    }
    if(ctrl_ -> sens.DI.PTO_Active) {
      if(ctrl_->sens.DI.ClutchSensor == 0) {
        timeOut_Rotation = GetMSTick();
      }
    }
  } else {
    timeOut_Rotation = GetMSTick();
  }

  //Set Timeout for dump.
  if ((0 == ctrl_ -> sens.Freq.ConveryRotation) && RTRDump_IsOn(&ctrl_->top.WinterTop)) {
    // Ignoring sensors convery rotation
    if (!GetVar(HDL_SW_CONV_ROT)) {
      timeOut_Dump = GetMSTick();
    }
  } else {
    timeOut_Dump = GetMSTick();
  }

  //Set Timeout for Material Sensor.
  if (ctrl_ -> sens.Freq.ConveryRotation && (ctrl_ -> top.WinterTop.SpreadConv.on)) {
    if (DI_SENSOR_OFF == ctrl_ -> sens.DI.Spreading) {
      timeOut_Material = GetMSTick();
    }
    // Ignoring sensors material
    if (DI_SENSOR_ON == ctrl_ -> sens.DI.Spreading) {
      if (!GetVar(HDL_SW_SPREAD_SENS)) {
        timeOut_Material = GetMSTick();
      }
    }
  } else if ((ctrl_ -> sens.Freq.ConveryRotation == 0) && (ctrl_ -> top.WinterTop.SpreadConv.on)) {
    if ((0 == speed) || (0 == ctrl_ -> top.WinterTop.SpreadDensity) || (workSpeed >= speed)) {
      timeOut_Material = GetMSTick();
    }
    if (!GetVar(HDL_SW_CONV_ROT)) {
      if (DI_SENSOR_OFF == ctrl_ -> sens.DI.Spreading) {
        timeOut_Material = GetMSTick();
      }
    }
    // Ignoring sensors material
    if (DI_SENSOR_ON == ctrl_ -> sens.DI.Spreading) {
      if (!GetVar(HDL_SW_SPREAD_SENS)) {
        timeOut_Material = GetMSTick();
      }
    }
  } else {
    timeOut_Material = GetMSTick();
  }

  // Rotation or material spreading invalid for more then 2 seconds
  int32_t val_;
  if (4000 < (GetMSTick() - timeOut_Rotation)) {
    val_ = MAT_SENSOR_NO_ROT; //Icon no Rotation
    timeOut_Material = GetMSTick(); //this way no material error will come if rotation error goes away
  } else if (RTRDump_IsOn(&ctrl_->top.WinterTop) && (4000 < (GetMSTick() - timeOut_Dump))) {
    val_ = MAT_SENSOR_NO_ROT;
    timeOut_Material = GetMSTick();
  } else if ((DI_SENSOR_ON == ctrl_ -> sens.DI.Spreading) && (5000 < (GetMSTick() - timeOut_Material))) {
    val_ = MAT_SENSOR_EMPTY;
  } else {
    val_ = MAT_SENSOR_OK;
  }

  //only at event
  static int32_t val_old = -1;
  if (val_ != val_old) {
    val_old = val_;
    if (val_ == MAT_SENSOR_NO_ROT) {
      EList_SetErrLevByRCText(VBL_WARNING, ERRID_CONVEYOR_NO_ROTATION, RCTEXT_T_NOCONVEYORROTATION);
    } else {
      EList_ResetErr(ERRID_CONVEYOR_NO_ROTATION);
    }
    if (val_ == MAT_SENSOR_EMPTY) {
      MsgBoxOk(RCTextGetText(RCTEXT_T_WARNING, GetVarIndexed(IDX_SYS_LANGUAGE)), RCTextGetText(RCTEXT_T_NOMATERIAL, GetVarIndexed(IDX_SYS_LANGUAGE)), NULL, NULL);
    }
    SetVarIndexed(IDX_ICON_DENSITY, val_);
  }
  if (val_ == MAT_SENSOR_EMPTY) {
    //Blinking in 500 second steps
    if (500 > (GetMSTick() % 1000)) {
      SetVarIndexed(IDX_ICON_DENSITY, MAT_SENSOR_OK);
    } else {
      SetVarIndexed(IDX_ICON_DENSITY, MAT_SENSOR_EMPTY);
    }
  }
}
