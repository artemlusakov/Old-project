#include <stdint.h>
#include <UserCEvents.h>
#include <UserCAPI.h>
#include "vartab.h"
#include "objtab.h"
#include "visu.h"
#include "control.h"
#include "gsemcm.h"
#include "param.h"
#include "gsToVisu.h"
#include "RCText.h"
#include "gsLkfCodes.h"

uint8_t flagTopCalibr = 0;
uint8_t flagBrushCalibr = 0;
int32_t RotatePWM = 500;
uint8_t flagChoiceInput = 0;
uint8_t flagInputPWM = 0;
uint8_t stepPWM = 1;

void CalibrBrush_Save(void);
void CalibrTop_Save(void);

void Visu_ChoiceInput_Cycle(uint32_t evtc, tUserCEvt * evtv, uint32_t objID, uint32_t varIdx) {
  // Изменение значения ШИМа
  // Если ввод вручную, то изменение значения через эндкодер отключается
  for (uint32_t i = 0; i < evtc; i++) {
    if (CEVT_EDIT_START == evtv[i].Type) {
      if (evtv[i].Content.mEditStart.ObjID == objID) {
        flagInputPWM = 1;
      }
    }
    if (CEVT_EDIT_END == evtv[i].Type) {
      if (evtv[i].Content.mEditEnd.ObjID == objID) {
        flagInputPWM = 0;
        RotatePWM = GetVarIndexed(varIdx);
      }
    }
  }

  static uint8_t arrStep[3] = {1,5,10};
  SetVarIndexed(IDX_CHOICESTEPPWM, stepPWM);
  if (!flagInputPWM && flagChoiceInput) {
    if (IsKeyPressedNew(103)) { // По часовой
      RotatePWM = RotatePWM + arrStep[stepPWM];
    }
    if (IsKeyPressedNew(104)) { // Против часовой
      RotatePWM = RotatePWM - arrStep[stepPWM];
    }
  }
  CheckRange( & RotatePWM, 0, 1000);
  SetVarIndexed(IDX_BRUSHCALIBR_PWM, RotatePWM);
  SetVarIndexed(IDX_TOPCALIBR_PWM, RotatePWM);
}

void Visu_Graph(uint32_t obj_1, uint32_t obj_2) {
  tControl * Ctrl = CtrlGet();

  static uint8_t flagOneTime, flagOneTime1 = 0;
  static uint32_t old_obj1, old_obj2 = 0;

  if(old_obj1 != obj_1 && old_obj2 != obj_2) {
    flagOneTime = 0;
    flagOneTime1 = 0;
    old_obj1 = obj_1;
    old_obj2 = obj_2;
  }
  if (!flagChoiceInput) {
    if (!flagOneTime) {
      SendToVisuObj(obj_1, GS_TO_VISU_SET_ATTR_NOMINALFLAG, 1);
      SendToVisuObj(obj_1, GS_TO_VISU_SET_ATTR_UNDERLINE, 1);
      SendKeyToVisu(GS_KEY_TO_VISU_TYPE_LKF | GS_VISU_LKF_MOVEUP);
      SendToVisuObj(obj_2, GS_TO_VISU_SET_ATTR_NOMINALFLAG, 0);
      SendToVisuObj(obj_2, GS_TO_VISU_SET_ATTR_UNDERLINE, 0);
      flagOneTime = 1;
      flagOneTime1 = 0;
    }
  } else {
    if (!flagOneTime1) {
      SendToVisuObj(obj_2, GS_TO_VISU_SET_ATTR_NOMINALFLAG, 1);
      SendToVisuObj(obj_2, GS_TO_VISU_SET_ATTR_UNDERLINE, 1);
      SendKeyToVisu(GS_KEY_TO_VISU_TYPE_LKF | GS_VISU_LKF_MOVEUP);
      SendToVisuObj(obj_1, GS_TO_VISU_SET_ATTR_NOMINALFLAG, 0);
      SendToVisuObj(obj_1, GS_TO_VISU_SET_ATTR_UNDERLINE, 0);
      flagOneTime1 = 1;
      flagOneTime = 0;
    }
  }

  if (Brush_Active( & Ctrl -> ABC[EQUIP_A].BrushValve)) {
    SetVarIndexed(IDX_BRUSHROTATEINFO, 1);
  } else if (Brush_Active( & Ctrl -> ABC[EQUIP_B].BrushValve)) {
    SetVarIndexed(IDX_BRUSHROTATEINFO, 2);
  } else {
    SetVarIndexed(IDX_BRUSHROTATEINFO, 0);
  }

  static int8_t old_flagBrushCalibr, old_flagTopCalibr = -1;
  if(old_flagBrushCalibr != flagBrushCalibr) {
    SendToVisuObj(OBJ_CALIBRON, GS_TO_VISU_SET_ATTR_VISIBLE, flagBrushCalibr);
    old_flagBrushCalibr = flagBrushCalibr;
  }

  if(old_flagTopCalibr != flagTopCalibr) {
    SendToVisuObj(OBJ_CALIBRON_TOP, GS_TO_VISU_SET_ATTR_VISIBLE, flagTopCalibr);
    old_flagTopCalibr = flagTopCalibr;
  }

  SetVarIndexed(IDX_BRUSHCALIBRCHOICEINPUT, flagChoiceInput);
}

void Visu_CalibrBrush_Open(const tVisuData * VData) {
  PrioMaskOn(MSK_CALIBRBRUSH);
}
void Visu_CalibrBrush_Close(const tVisuData * VData) {
  flagBrushCalibr = 0;
  flagChoiceInput = 0;
  flagInputPWM = 0;
}

void Visu_CalibrBrush_Cycle(const tVisuData * VData, uint32_t evtc, tUserCEvt * evtv) {
  tControl * Ctrl = CtrlGet();

  Visu_ChoiceInput_Cycle(evtc, evtv, OBJ_ROTATEPWM, IDX_BRUSHCALIBR_PWM);
  Visu_Graph(OBJ_ATRESTTTEXTLIST446, OBJ_ROTATEPWM);

  if (flagBrushCalibr) {
    if (Brush_Active( & Ctrl -> ABC[EQUIP_A].BrushValve)) {
      Ctrl -> ABC[EQUIP_A].BrushValve.VRotationValue = RotatePWM;
    } else if (Brush_Active( & Ctrl -> ABC[EQUIP_B].BrushValve)) {
      Ctrl -> ABC[EQUIP_B].BrushValve.VRotationValue = RotatePWM;
    } else {
      Ctrl -> ABC[EQUIP_B].BrushValve.VRotationValue = 0;
    }
  }

  // Выбор ввода щетки/ШИМа
  if (IsKeyPressedNew(1)) {
    flagChoiceInput = 1 - flagChoiceInput;
    SetVarIndexed(IDX_BRUSHCALIBRCHOICEINPUT, flagChoiceInput);
  }
  // Активация калибровки
  if (IsKeyPressedNew(2)) {
    flagBrushCalibr = 1 - flagBrushCalibr;
  }
  // Сохранение ШИМа
  if (IsKeyPressedNew(3)) {
    CalibrBrush_Save();
  }
  // Изменение шага ШИМа
  if (IsKeyPressedNew(4)) {
    stepPWM = stepPWM + 1;
    if (stepPWM == 3) {
      stepPWM = 0;
    }
  }
  // Переход на калибровку основного оборудования
  if (IsKeyPressedNew(6)) {
    Visu_OpenScreen(SCREEN_CALIBRTOP);
  }

  Visu_HomeKeyPressed();
}

void CalibrBrush_Save(void) {
  tParamData * Param = ParameterGet();
  switch (GetVarIndexed(IDX_CHOICEBRUSHCALIBR)) {
  case 0:
    Param -> OMFB.PWM_Rotate_OMB1 = RotatePWM;
    break;

  case 1:
    Param -> OMFB.PWM_Rotate_OMT1 = RotatePWM;
    break;

  case 2:
    Param -> OMFB.PWM_Rotate_OPF245 = RotatePWM;
    break;

  case 3:
    Param -> OMFB.PWM_Rotate_OMP = RotatePWM;
    break;

  case 4:
    Param -> OMFB.PWM_Rotate_OZP231 = RotatePWM;
    break;

  default:
    break;
  }
  writeParameters();
}

uint8_t GetFlagBrushCalibr(void) {
  tControl * Ctrl = CtrlGet();
  if (Brush_Active( & Ctrl -> ABC[EQUIP_A].BrushValve)) {
    return flagBrushCalibr;
  } else if (Brush_Active( & Ctrl -> ABC[EQUIP_B].BrushValve)) {
    return flagBrushCalibr;
  } else {
    return 0;
  }
}


void Visu_CalibrTop_Open(const tVisuData * VData) {
  PrioMaskOn(MSK_CALIBRTOP);
}
void Visu_CalibrTop_Close(const tVisuData * VData) {
  flagTopCalibr = 0;
  flagChoiceInput = 0;
  flagInputPWM = 0;
}

void Visu_CalibrTop_Cycle(const tVisuData * VData, uint32_t evtc, tUserCEvt * evtv) {
  tControl * Ctrl = CtrlGet();

  Visu_ChoiceInput_Cycle(evtc, evtv, OBJ_TOPPWM, IDX_TOPCALIBR_PWM);
  Visu_Graph(OBJ_ATRESTTTEXTLIST589, OBJ_TOPPWM);

  if (flagTopCalibr) {
    if (Ctrl -> cmd_no_key.E_S) {
      switch (GetVarIndexed(IDX_CHOICETOPCALIBR)) {
      case 0: // ПС
        Valv_Set( & Ctrl -> top.WinterTop.ValveMain, RotatePWM);
        break;
      case 1: // BP300
        ValvProp_Set( & Ctrl -> top.SummerTop.Pump.Valv.Valve.PVEH, RotatePWM);
        break;
      case 2: // C610H
        ValvProp_Set( & Ctrl -> top.SummerTop.Pump.Valv.Valve.PVEH, RotatePWM);
        break;

      default:
        break;
      }
    } else {
      switch (GetVarIndexed(IDX_CHOICETOPCALIBR)) {
      case 0: // ПС
        Valv_Set( & Ctrl -> top.WinterTop.ValveMain, 0);
        break;
      case 1: // BP300
        ValvProp_Set( & Ctrl -> top.SummerTop.Pump.Valv.Valve.PVEH, 0);
        break;
      case 2: // C610H
        ValvProp_Set( & Ctrl -> top.SummerTop.Pump.Valv.Valve.PVEH, 0);
        break;
      default:
        break;
      }
    }
  }

  // Выбор ввода щетки/ШИМа
  if (IsKeyPressedNew(1)) {
    flagChoiceInput = 1 - flagChoiceInput;
  }
  // Активация калибровки
  if (IsKeyPressedNew(2)) {
    flagTopCalibr = 1 - flagTopCalibr;
  }
  // Сохранение ШИМа
  if (IsKeyPressedNew(3)) {
    CalibrTop_Save();
  }
  // Изменение шага ШИМа
  if (IsKeyPressedNew(4)) {
    stepPWM = stepPWM + 1;
    if (stepPWM == 3) {
      stepPWM = 0;
    }
  }

  // Переход на калибровку основного оборудования
  if (IsKeyPressedNew(6)) {
    Visu_OpenScreen(SCREEN_CALIBRBRUSH);
  }

  Visu_HomeKeyPressed();
}

void CalibrTop_Save(void) {
  tParamData * Param = ParameterGet();
  switch (GetVarIndexed(IDX_CHOICETOPCALIBR)) {
  case 0: // ПС
    Param -> OMFB.PWM_RTR = RotatePWM;
    break;

  case 1: // BP300
    Param -> OMFB.PWM_BP300 = RotatePWM;
    break;

  case 2: // C610H
    Param -> OMFB.PWM_C610H = RotatePWM;
    break;

  default:
    break;
  }
  writeParameters();
}

uint8_t GetFlagTopCalibr(void) {
  tControl * Ctrl = CtrlGet();
  if (Ctrl -> cmd_no_key.E_S) {
    return flagTopCalibr;
  } else {
    return 0;
  }
}
