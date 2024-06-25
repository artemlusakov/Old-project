#include <stdint.h>
#include <stdlib.h>
#include <math.h>
#include <UserCEvents.h>
#include <UserCAPI.h>
#include "gsemcm.h"
#include "vartab.h"
#include "objtab.h"
#include "ctrl_plough.h"
#include "RCText.h"
#include "param.h"
#include "errorlist.h"
#include "control.h"
#include "profile.h"
#include "gse_msgbox.h"
#include "pinout.h"
#include "calibr_omfb.h"


void Ctrl_Brush_AutoUp(tBrushValve * BrushValve, tJSCommands * Cmd);

int8_t BrushOn;
int8_t flagAutoUp;

void Brush_Maint_Create(tBrushValve * BrushValve, tMaint * Maint, eErrorID ID, uint32_t RCText_Warn, uint32_t RCText_Service, uint32_t HdlRemVar_Warn, uint32_t Hdl_RemVar_Service) {
  BrushValve -> MaintRunntime = Maint_Runtime_Add(Maint, ID, RCText_Warn, RCText_Service, HdlRemVar_Warn, Hdl_RemVar_Service, 500);
}
void Brush_Maint_Create_Reset(tBrushValve * BrushValve, tMaint * Maint, eErrorID ID, uint32_t RCText_Warn, uint32_t RCText_Service, uint32_t HdlRemVar_Warn, uint32_t Hdl_RemVar_Service) {
  BrushValve -> MaintRunntime_Reset = Maint_Runtime_Add(Maint, ID, RCText_Warn, RCText_Service, HdlRemVar_Warn, Hdl_RemVar_Service, 500);
}

void Brush_UpDown_Init(tBrushValve * BrushValve, uint32_t Device1, uint32_t Pos1, uint32_t Device2, uint32_t Pos2, uint32_t ValveType) {
  switch (ValveType) {
  case Valve_PVEH_32:
    Valv_PVG32_Init( & BrushValve -> VUpDown, Device1, Pos1, Device2, Pos2);
    break;
  case Valve_PVEH_OMFB:
    Valv_OMFB_Init( & BrushValve -> VUpDown, Device1, Pos1, -1, -1);
    break;
  default:
    Valv_Init( & BrushValve -> VUpDown, Device1, Pos1, Device2, Pos2);
    break;
  }
}

void Equip_Down_OMFB_Init(tBrushValve * BrushValve, uint32_t Device1, uint32_t Pos1, uint32_t Device2, uint32_t Pos2, uint32_t ValveType) {
  if (ValveType == Valve_PVEH_OMFB) {
    Valv_OMFB_Init( & BrushValve -> VUpDownSecondary, Device1, Pos1, Device2, Pos2);
  }
}

void Brush_Lock_Init(tBrushValve * BrushValve, uint32_t Device1, uint32_t Pos1) {
  ValvDisk_Init( & BrushValve -> VLock, Device1, Pos1);
}

void Brush_Float_Init(tBrushValve * BrushValve, int32_t Device1, int32_t Pos1) {
  ValvDisk_Init( & BrushValve -> VFloat, Device1, Pos1);
}

void Brush_Rotate_Init(tBrushValve * BrushValve, int32_t Device1, int32_t Pos1, int32_t ValveType) {
  switch (ValveType) {
  case Valve_PVEO_1:
  case Valve_RG:
    ValvDisk_Init( & BrushValve -> VRotation, Device1, Pos1);
    break;
  case Valve_PVEH_16:
    Valv_Init( & BrushValve -> VRotation, Device1, Pos1, -1, -1);
    break;
  case Valve_PVEH_32:
    Valv_PVG32_Init( & BrushValve -> VRotation, Device1, Pos1, -1, -1);
  case Valve_PVEH_OMFB:
    Valv_OMFB_Init( & BrushValve -> VRotation, Device1, Pos1, -1, -1);
    break;
  }
}

void Brush_LeftRight_Init(tBrushValve * BrushValve, uint32_t Device1, uint32_t Pos1, uint32_t Device2, uint32_t Pos2, uint32_t ValveType) {
  switch (ValveType) {
  case Valve_PVEH_32:
    Valv_PVG32_Init( & BrushValve -> VLeftRight, Device1, Pos1, Device2, Pos2);
    break;
  default:
    Valv_Init( & BrushValve -> VLeftRight, Device1, Pos1, Device2, Pos2);
    break;
  }
}

void Brush_Optional_Init(tBrushValve * BrushValve, uint32_t Device1, uint32_t Pos1, uint32_t Device2, uint32_t Pos2) {
  Valv_Init( & BrushValve -> VOptional, Device1, Pos1, Device2, Pos2);
}

void Brush_VSideSec_Init(tBrushValve * BrushValve, uint32_t Device1, uint32_t Pos1, uint32_t Device2, uint32_t Pos2) {
  Valv_Init( & BrushValve -> VSideSec, Device1, Pos1, Device2, Pos2);
}

void Brush_A_B(tBrushValve * BrushValve, int32_t Val) {
  BrushValve -> BrushInvert = Val;
}

int32_t Brush_Active(const tBrushValve * BrushValve) {
  if (BrushOn && (BrushValve -> BrushInvert == BrushOn)) {
    return 1;
  } else {
    return 0;
  }
}

extern tReactBuzzer React;
void Brush_Cycle(tBrushValve * BrushValve, tJSCommands * Cmd, uint8_t Equip) {
  tControl * Ctrl = CtrlGet();
  const tProfile * Prof = Profile_GetCurrentProfile();
  const tParamData * Param = ParameterGet();

  if (GetVarIndexed(IDX_EMCY_ON) == 0 && React.ReactDoing != 2) {
    if (Cmd -> Active) {
      if (BrushValve -> VRotation.Type != VALVE_TYPE_NONE) {
        if (Cmd -> F_New) {
          if (PVE_PORT_A == BrushValve -> BrushInvert) {
            if (PVE_PORT_A == BrushOn) {
              BrushOn = PVE_PORT_NONE;
            } else
              BrushOn = PVE_PORT_A;
          } else {
            if (PVE_PORT_B == BrushOn)
              BrushOn = PVE_PORT_NONE;
            else
              BrushOn = PVE_PORT_B;
          }
        }
      }
    }
  } else {
    BrushOn = PVE_PORT_NONE;
  }

  // Если есть плавание при подъеме щетки отключаем вращение
  if (BrushValve -> isFloat) {
    if (Cmd -> Updown > 0) {
      BrushOn = PVE_PORT_NONE;
      Cmd -> Float = FLOAT_OFF;
    }
  }
  if(!Brush_Active(BrushValve)) {
    Cmd -> Float = FLOAT_OFF;
  }

  BrushValve -> VUpDownValue = -Cmd -> Updown;
  BrushValve -> VUpDownSecondaryValue = 0;
  BrushValve -> VLeftRightValue = -Cmd -> LeftRight;
  BrushValve -> VLockValue = 0;
  BrushValve -> VFloatValue = 0;
  BrushValve -> VOptionalValue = Cmd -> Z;
  BrushValve -> VSideSecValue = Cmd -> Z;

  if(BrushValve->isFloat) {
    switch (BrushValve -> VUpDown.Type) {
    case VALVE_TYPE_PVE_AHS:
      if (FLOAT_ON == Cmd -> Float) {
        BrushValve -> VFloatValue = 1;
        BrushValve -> VLockValue = 1;
      }
      break;
    case VALVE_TYPE_PVE_AHS32:
      if (FLOAT_ON == Cmd -> Float) {
        BrushValve -> VUpDownValue = 2500;
        BrushValve -> VLockValue = 1;
      }
      break;
    case VALVE_TYPE_OMFB:
      if (FLOAT_ON == Cmd -> Float) {
        BrushValve -> VUpDownValue = 1000;
        BrushValve -> VLockValue = 1;
      }
      break;
    case VALVE_TYPE_PVE_O:
      if (FLOAT_ON == Cmd -> Float) {
        BrushValve -> VLockValue = 1;
      }
      break;
    default:
      break;
    }
  }

  // Управление гидрозамком
  if (Cmd -> Updown) {
    BrushValve -> VLockValue = 1;
  }

  // Управление вверх/вниз для OMFB
  if (BrushValve -> VUpDown.Type == VALVE_TYPE_OMFB) {
    int8_t invertEquip[2] = {
      Param -> OMFB.Invert_UpDown_A,
      Param -> OMFB.Invert_UpDown_B,
    };
    uint16_t PWM_UpDownEquip[2] = {
      Param -> OMFB.PWM_UpDown_A,
      Param -> OMFB.PWM_UpDown_B,
    };

    if (!invertEquip[Equip]) {
      if (Cmd -> Updown < 0) {
        BrushValve -> VUpDownValue = PWM_UpDownEquip[Equip];
      } else if (Cmd -> Updown > 0) {
        BrushValve -> VUpDownValue = 0;
        BrushValve -> VUpDownSecondaryValue = 1;
      }
    } else {
      if (Cmd -> Updown > 0) {
        BrushValve -> VUpDownValue = PWM_UpDownEquip[Equip];
      } else if (Cmd -> Updown < 0) {
        BrushValve -> VUpDownValue = 0;
        BrushValve -> VUpDownSecondaryValue = 1;
      }
    }
  }

  // Задержка включения вращения межосевой щетки
  static int8_t flagInterRotate = 0;
  static uint8_t init = 0;
  static uint32_t timer = 0;
  if (BrushOn == PVE_PORT_A) {
    if (!init) {
      timer = GetMSTick() + Param -> EquipTimers.DelayInterRotate_timer;
      init = 1;
    }
    if (GetMSTick() >= timer) {
      flagInterRotate = BrushOn;
    }
  } else {
    flagInterRotate = 0;
    init = 0;
  }

  // Рабочий режим. При калибровке щеток блок не выполняется.
  if(!GetFlagBrushCalibr()) {
    BrushValve -> VRotationValue = 0;
    switch (BrushValve -> VRotation.Type) {
    case VALVE_TYPE_PVE_AHS:
    case VALVE_TYPE_PVE_AHS32:
      if (BrushOn == PVE_PORT_A) {
        BrushValve -> VRotationValue = flagInterRotate * 1000;
      } else if (BrushOn == PVE_PORT_B) {
        BrushValve -> VRotationValue = fabs(BrushOn) * 1000;
      }
      break;
    case VALVE_TYPE_OMFB:
      if (BrushOn == PVE_PORT_A) { // Inter
        Ctrl -> ABC[EQUIP_A].BrushValve.VRotationValue = 0;
        Ctrl -> ABC[EQUIP_B].BrushValve.VRotationValue = fabs(flagInterRotate) * Ctrl->ABC[EQUIP_B].BrushValve.VRotatePWM;
      } else if (BrushOn == PVE_PORT_B) { // Front
        Ctrl -> ABC[EQUIP_A].BrushValve.VRotationValue = fabs(BrushOn) * Ctrl->ABC[EQUIP_A].BrushValve.VRotatePWM;
        Ctrl -> ABC[EQUIP_B].BrushValve.VRotationValue = 0;
      } else if (BrushOn == PVE_PORT_NONE) {
        Ctrl -> ABC[EQUIP_A].BrushValve.VRotationValue = 0;
        Ctrl -> ABC[EQUIP_B].BrushValve.VRotationValue = 0;
      }
      break;
    case VALVE_TYPE_DISK:
      if (BrushOn == PVE_PORT_A) { // Port A
        Ctrl -> ABC[EQUIP_A].BrushValve.VRotationValue = 0;
        Ctrl -> ABC[EQUIP_B].BrushValve.VRotationValue = fabs(flagInterRotate) * 1000;
      } else if (BrushOn == PVE_PORT_B) { // Port B
        Ctrl -> ABC[EQUIP_A].BrushValve.VRotationValue = fabs(BrushOn) * 1000;
        Ctrl -> ABC[EQUIP_B].BrushValve.VRotationValue = 0;
      } else if (BrushOn == PVE_PORT_NONE) {
        Ctrl -> ABC[EQUIP_A].BrushValve.VRotationValue = 0;
        Ctrl -> ABC[EQUIP_B].BrushValve.VRotationValue = 0;
      }
      break;
    default:
      break;
    }
  }

  if (BrushOn) {
    Ctrl_SetMaxSpeed(BrushValve -> MaxSpeed);
  }

  // Управление увлажнением OMP
  if (Prof -> module[2] == RCTEXT_LI_B_ATTACHMENT_ATTACHMENT_OMP220_HUMID) {
    GSeMCM_SetDigitalOut(2, 2, GetVar(HDL_HUMIDON));
  }
  Ctrl_Brush_AutoUp( & Ctrl -> ABC[EQUIP_B].BrushValve, & Ctrl -> cmd_ABC[EQUIP_B]);
}

void Ctrl_Brush_AutoUp(tBrushValve * BrushValve, tJSCommands * Cmd) {
  tControl * Ctrl = CtrlGet();
  const tParamData * Param = ParameterGet();

  // Получение иконки межосевого оборудования
  tProfileModule * Module;
  eAttachmentPicIdx Pic = 0;
  Module = GetCurrentModule(2);
  Pic = Module -> Icon;

  // Проверка была ли включена щетка при включении задней передачи
  if (Brush_Active(BrushValve)) {
    if (Ctrl -> sens.DI.SelectedGear == 1) {
      flagAutoUp = 1;
    }
  }

  static uint8_t flagBrushUp = 0;
  static uint8_t flagTmr = 0;
  uint64_t tmr = Param -> EquipTimers.InterUp_timer;
  static uint64_t tmrStop = 0;

  if (flagAutoUp) {
    if (Param -> Engine.PTOatEngine) {
      if (!flagBrushUp) {
        if (!flagTmr) {
          tmrStop = tmr + GetMSTick();
          flagTmr = 1;
        }
        if (GetMSTick() <= tmrStop) {
          InfoContainerOn(CNT_ATTACHMOVE);
          SetVarIndexed(IDX_ATTACHMENT_UP, 2);
          SetVarIndexed(IDX_ATTACHPIC, Pic);

          BrushOn = PVE_PORT_NONE;
          Cmd -> Float = FLOAT_OFF;
          BrushValve->VLockValue = 1;

          if (BrushValve -> VUpDown.Type == VALVE_TYPE_OMFB) {
            if(!Param->OMFB.Invert_UpDown_B) {
              BrushValve->VUpDownSecondaryValue = 1;
            } else {
              BrushValve->VUpDownValue = Param->OMFB.Invert_UpDown_B;
            }
          } else {
            BrushValve->VUpDownValue = -1000;
          }
        }
      }
    } else if (Ctrl -> sens.DI.PTO_Active) {
      InfoContainerOn(CNT_ATTACHMOVE);
      SetVarIndexed(IDX_ATTACHMENT_UP, 2);
      SetVarIndexed(IDX_ATTACHPIC, Pic);

      BrushOn = PVE_PORT_NONE;
      Cmd -> Float = FLOAT_OFF;
      BrushValve->VLockValue = 1;

      if (BrushValve -> VUpDown.Type == VALVE_TYPE_OMFB) {
        if(Param->OMFB.Invert_UpDown_B) {
          BrushValve->VUpDownSecondaryValue = 1;
        } else {
          BrushValve->VUpDownValue = Param->OMFB.Invert_UpDown_B;
        }
      }
    }

    if (Ctrl -> sens.DI.SelectedGear == 0) {
      Cmd -> Float = FLOAT_ON;
      BrushOn = PVE_PORT_A;
      flagBrushUp = 0;
      flagTmr = 0;
      flagAutoUp = 0;
    }
  }
}

int8_t IsBrushAutoUp(void) {
  return flagAutoUp;
}

void Brush_Timer_10ms( tBrushValve* BrushValve ) {
	BrushValve->tmrBrush++;
	//call only every 10 times
	if( 0 == (BrushValve->tmrBrush % 10) ) {
    Valv_Set( & BrushValve -> VUpDown, BrushValve -> VUpDownValue);
    Valv_Set( & BrushValve -> VUpDownSecondary, BrushValve -> VUpDownSecondaryValue);
    Valv_Set( & BrushValve -> VLeftRight, BrushValve -> VLeftRightValue);
    Valv_Set( & BrushValve -> VLock, BrushValve -> VLockValue);
    Valv_Set( & BrushValve -> VFloat, BrushValve -> VFloatValue);
    Valv_Set( & BrushValve -> VRotation, BrushValve -> VRotationValue);
    Valv_Set( & BrushValve -> VOptional, BrushValve -> VOptionalValue);
    Valv_Set( & BrushValve -> VSideSec, BrushValve -> VSideSecValue);
  }
}
