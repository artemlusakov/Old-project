/****************************************************************************
*
* File:         CTRL_PLOUGH.c
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
#include <UserCEvents.h> /* Events send to the Cycle function              */
#include <UserCAPI.h>	 /* API-Function declarations                      */

#include "vartab.h" /* Variable Table definition                      */
 /* include this file in every C-Source to access  */
 /* the variable table of your project             */
 /* vartab.h is generated automatically            */
#include "objtab.h" /* Object ID definition                           */
					/* include this file in every C-Source to access  */
					/* the object ID's of the visual objects          */
					/* objtab.h is generated automatically            */
#include "ctrl_plough.h"
#include "io_types.h"
#include "control.h"
#include "gsemcm.h"
#include "param.h"
#include "profile.h"
#include "gsToVisu.h"
#include "errorlist.h"
#include "pinout.h"
#include "RCText.h"
/****************************************************************************/

/* macro definitions ********************************************************/

/* type definitions *********************************************************/

/* prototypes ***************************************************************/

/* global constants *********************************************************/

/* global variables *********************************************************/

/* local function prototypes*************************************************/

void Ctrl_Plough_CH2800_AutoUp(tABC_Attach * ABC_Attach, tJSCommands * Cmd, int8_t * float_CH2800, uint32_t * float_tmrCH2800);

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
void Plough_Maint_Create(tABC_Attach * ABC_Attach, tMaint * Maint, eErrorID ID, uint32_t RCText_Warn, uint32_t RCText_Service, uint32_t HdlRemVar_Warn, uint32_t Hdl_RemVar_Service) {
  ABC_Attach -> Runtime_Ploughs = Maint_Runtime_Add(Maint, ID, RCText_Warn, RCText_Service, HdlRemVar_Warn, Hdl_RemVar_Service, 500);
}
void Plough_Maint_Create_Reset(tABC_Attach * ABC_Attach, tMaint * Maint, eErrorID ID, uint32_t RCText_Warn, uint32_t RCText_Service, uint32_t HdlRemVar_Warn, uint32_t Hdl_RemVar_Service) {
  ABC_Attach -> Runtime_Ploughs_Reset = Maint_Runtime_Add(Maint, ID, RCText_Warn, RCText_Service, HdlRemVar_Warn, Hdl_RemVar_Service, 500);
}

void Plough_UpDown_Init(tABC_Attach * Plough, uint32_t Device1, uint32_t Pos1, uint32_t Device2, uint32_t Pos2, uint32_t ValveType) {
  switch (ValveType) {
  case Valve_PVEH_32:
    Valv_PVG32_Init( & Plough -> ValveUpDown, Device1, Pos1, Device2, Pos2);
    break;
  case Valve_PVEH_OMFB:
    Valv_OMFB_Init( & Plough -> ValveUpDown, Device1, Pos1, -1, -1);
    break;
  default:
    Valv_Init( & Plough -> ValveUpDown, Device1, Pos1, Device2, Pos2);
    break;
  }
}

void OMFB_UpDownSecond_Init(tABC_Attach * Plough, uint32_t Device1, uint32_t Pos1, uint32_t Device2, uint32_t Pos2, uint32_t ValveType) {
  if (ValveType == Valve_PVEH_OMFB) {
    Valv_OMFB_Init( & Plough -> ValveUpDownSecondary, Device1, Pos1, Device2, Pos2);
  }
}

void Plough_Lock_Init(tABC_Attach * Plough, uint32_t Device1, uint32_t Pos1) {
  ValvDisk_Init( & Plough -> ValveLock, Device1, Pos1);
}

void Plough_TiltLock_Init(tABC_Attach * Plough, uint32_t Device1, uint32_t Pos1) {
  ValvDisk_Init( & Plough -> ValveTiltLock, Device1, Pos1);
}

void Plough_UpDown_Second_Init(tABC_Attach * Plough, uint32_t Device1, uint32_t Pos1, uint32_t Device2, uint32_t Pos2, int32_t ValveType) {
  switch (ValveType) {
  case -1:
    break;
  case Valve_PVEH_OMFB:
    Valv_OMFB_Init( & Plough -> ValveUpDownSecondary, Device1, Pos1, Device2, Pos2);
    break;
  default:
    Valv_Init( & Plough -> ValveUpDownSecondary, Device1, Pos1, Device2, Pos2);
    break;
  }
}

void Plough_Float_Init(tABC_Attach * Plough, int32_t Device, int32_t Pos) {
  ValvDisk_Init( & Plough -> ValveFloat, Device, Pos);
}

void Plough_LeftRight_Init(tABC_Attach * Plough, uint32_t DeviceTurnLeft, uint32_t PosTurnLeft, uint32_t DeviceTurnRight, uint32_t PosTurnRight, uint32_t ValveType) {
  switch (ValveType) {
  case Valve_PVEH_32:
    Valv_PVG32_Init( & Plough -> ValveLeftRight, DeviceTurnLeft, PosTurnLeft, DeviceTurnRight, PosTurnRight);
    break;
  default:
    Valv_Init( & Plough -> ValveLeftRight, DeviceTurnLeft, PosTurnLeft, DeviceTurnRight, PosTurnRight);
    break;
  }
}

void Plough_SteelRubber_Init(tABC_Attach * Plough, uint32_t DeviceSteelUp, uint32_t PosSteelUp, uint32_t DeviceSteelDown, uint32_t PosSteelDown) {
  Valv_Init( & Plough -> ValveSteelUpDown, DeviceSteelUp, PosSteelUp, DeviceSteelDown, PosSteelDown);
}

void Plough_WorkTransport(tABC_Attach * Plough, uint32_t Device1, uint32_t Pos1, uint32_t Device2, uint32_t Pos2, uint32_t ValveType) {
  switch (ValveType) {
  case Valve_PVEH_32:
    Valv_PVG32_Init( & Plough -> ValveWorkTransport, Device1, Pos1, Device2, Pos2);
    break;
  default:
    Valv_Init( & Plough -> ValveWorkTransport, Device1, Pos1, Device2, Pos2);
    break;
  }
}

void Plough_TiltUpDown(tABC_Attach * Plough, uint32_t Device1, uint32_t Pos1, uint32_t Device2, uint32_t Pos2, uint32_t ValveType) {
  switch (ValveType) {
  case Valve_PVEH_OMFB:
    Valv_OMFB_Init( & Plough -> ValveTilt, Device1, Pos1, -1, -1);
    Valv_OMFB_Init( & Plough -> ValveTiltSecondary, Device2, Pos2, -1, -1);
    break;
  default:
    Valv_Init( & Plough -> ValveTilt, Device1, Pos1, Device2, Pos2);
    break;
  }
}

void Plough_ExRetract_Init(tABC_Attach * Plough, uint32_t DeviceRetract, uint32_t PosRetract, uint32_t DeviceExtract, uint32_t PosExtract) {
  Valv_Init( & Plough -> ValveExRetract, DeviceRetract, PosRetract, DeviceExtract, PosExtract);
}

void Plough_ExRetract_RightSection(tABC_Attach * Attachment, uint32_t DeviceRightSection, uint32_t PosRightSection) {
  ValvDisk_Init( & Attachment -> ValveSecondSection, DeviceRightSection, PosRightSection);
}

void Plough_UpDown_Timer_10ms(tABC_Attach * Plough) {
  Plough -> timer_cnt++;
  //call only every 10 times
  if (0 == (Plough -> timer_cnt % 10)) {
    Valv_Timer_100ms( & Plough -> ValveUpDown);
  }
}

/**
 * @brief Standard function to controle the plough.
 * @param Plough
 */
void Plough_Cycle(tABC_Attach * Plough, tJSCommands * Cmd, uint8_t Equip) {
  const tParamData * Param = ParameterGet();
  const tProfile * Prof = Profile_GetCurrentProfile();

  int UpDown = -Cmd -> Updown;

  switch (Plough -> ValveUpDown.Type) {
  case VALVE_TYPE_PVE_AHS:
    if (FLOAT_ON == Cmd -> Float) {
      Valv_Set( & Plough -> ValveFloat, 1);
      Valv_Set( & Plough -> ValveLock, 1);
    } else {
      Valv_Set( & Plough -> ValveFloat, 0);
    }
    break;
  case VALVE_TYPE_PVE_AHS32:
    if (FLOAT_ON == Cmd -> Float) {
      UpDown = 2500; // ШИМ 800
      Valv_Set( & Plough -> ValveLock, 1);
    }
    break;
  case VALVE_TYPE_OMFB:
    if (FLOAT_ON == Cmd -> Float) {
      UpDown = 1000;
      Valv_Set( & Plough -> ValveUpDownSecondary, 0);
      Valv_Set( & Plough -> ValveLock, 1);
    }
    break;
  case VALVE_TYPE_PVE_O:
    if (FLOAT_ON == Cmd -> Float) {
      UpDown = 0;
      Valv_Set( & Plough -> ValveLock, 1);
    }
    break;
  default:
    break;
  }

  // Графическое отображение ограничения скорости
  if (FLOAT_ON == Cmd -> Float) {
    if(Prof->module[2] != RCTEXT_LI_B_ATTACHMENT_ATTACHMENT_CH2600) {
      Ctrl_SetMaxSpeed(Plough -> MaxSpeed);
    }
  }

  // Управление гидрозамком
  if (Cmd -> Updown) {
    Valv_Set( & Plough -> ValveLock, 1);
  } else {
    if (Cmd -> Float == FLOAT_OFF) {
      Valv_Set( & Plough -> ValveLock, 0);
    }
  }

  // Управление вверх/вниз для OMFB
  if (Plough -> ValveUpDown.Type == VALVE_TYPE_OMFB) {
    int8_t invertEquip[2] = {
      Param -> OMFB.Invert_UpDown_A,
      Param -> OMFB.Invert_UpDown_B,
    };
    uint16_t PWM_UpDownEquip[2] = {
      Param -> OMFB.PWM_UpDown_A,
      Param -> OMFB.PWM_UpDown_B,
    };
    if(!Cmd -> Updown) {
      if (FLOAT_OFF == Cmd -> Float) {
        UpDown = 0;
        Valv_Set( & Plough -> ValveUpDownSecondary, 0);
      }
    }
    if (!invertEquip[Equip]) {
      if (Cmd -> Updown < 0) {
        UpDown = PWM_UpDownEquip[Equip];
        Valv_Set( & Plough -> ValveUpDownSecondary, 0);
      } else if (Cmd -> Updown > 0) {
        UpDown = 0;
        Valv_Set( & Plough -> ValveUpDownSecondary, 1);;
      }
    } else {
      if (Cmd -> Updown > 0) {
        UpDown = PWM_UpDownEquip[Equip];
        Valv_Set( & Plough -> ValveUpDownSecondary, 0);
      } else if (Cmd -> Updown < 0) {
        UpDown = 0;
        Valv_Set( & Plough -> ValveUpDownSecondary, 1);
      }
    }
  }

  //MoveUpDown;
  Valv_Set( & Plough -> ValveUpDown, UpDown);
  //Left right:
  Valv_Set( & Plough -> ValveLeftRight, -Cmd -> LeftRight);
  //Steel Rubber
  Valv_Set( & Plough -> ValveSteelUpDown, Cmd -> Z);
  //Extract/Retract
  Valv_Set( & Plough -> ValveExRetract, Cmd -> Z);
}

void Plough_2_Section_MSPN_Cycle(tABC_Attach * Plough_A, tJSCommands * Cmd_A, tABC_Attach * Plough_B, tJSCommands * Cmd_B) {
  const tParamData * Param = ParameterGet();

  int UpDown = -Cmd_A -> Updown;
  Cmd_B -> Float = 0;

  // Float
  if((Param -> VIN.ActualHydDistributor == HydDistributor_OMFB) || (Param -> VIN.ActualHydDistributor == HydDistributor_OMFBv2)) {
    switch (Plough_A->ValveUpDown.Type) {
    case VALVE_TYPE_OMFB:
      if (FLOAT_ON == Cmd_A -> Float) {
        UpDown = 1000;
        Valv_Set( & Plough_A -> ValveUpDownSecondary, 0);
        Valv_Set( & Plough_A -> ValveLock, 1);
        Ctrl_SetMaxSpeed(Plough_A -> MaxSpeed);
      }
      break;
    case VALVE_TYPE_PVE_O:
      if (FLOAT_ON == Cmd_A -> Float) {
        UpDown = 0;
        Valv_Set( & Plough_A -> ValveLock, 1);
        Ctrl_SetMaxSpeed(Plough_A -> MaxSpeed);
      }
    break;

    default:
      break;
    }
  } else {
    if (FLOAT_ON == Cmd_A -> Float) {
      Valv_Set( & Plough_A -> ValveFloat, 1);
      Valv_Set( & Plough_A -> ValveLock, 1);
      Ctrl_SetMaxSpeed(Plough_A -> MaxSpeed);
    } else {
      Valv_Set( & Plough_A -> ValveFloat, 0);
    }
  }

  // Управление гидрозамком
  if (Cmd_A -> Updown) {
    Valv_Set( & Plough_A -> ValveLock, 1);
  } else {
    if (Cmd_A -> Float == FLOAT_OFF) {
      Valv_Set( & Plough_A -> ValveLock, 0);
    }
  }

  // Управление вверх/вниз для OMFB
  if (Plough_A -> ValveUpDown.Type == VALVE_TYPE_OMFB) {
    if (!Cmd_A -> Updown) {
      if (FLOAT_OFF == Cmd_A -> Float) {
        UpDown = 0;
        Valv_Set( & Plough_A -> ValveUpDownSecondary, 0);
      }
    }
    if (!Param -> OMFB.Invert_UpDown_A) {
      if (Cmd_A -> Updown < 0) {
        UpDown = Param -> OMFB.PWM_UpDown_A;
        Valv_Set( & Plough_A -> ValveUpDownSecondary, 0);
      } else if (Cmd_A -> Updown > 0) {
        UpDown = 0;
        Valv_Set( & Plough_A -> ValveUpDownSecondary, 1);;
      }
    } else {
      if (Cmd_A -> Updown > 0) {
        UpDown = Param -> OMFB.PWM_UpDown_A;
        Valv_Set( & Plough_A -> ValveUpDownSecondary, 0);
      } else if (Cmd_A -> Updown < 0) {
        UpDown = 0;
        Valv_Set( & Plough_A -> ValveUpDownSecondary, 1);
      }
    }
  }

  if (Cmd_B -> Active) {
    Valv_Set( & Plough_B -> ValveSecondSection, 1);
  } else {
    Valv_Set( & Plough_B -> ValveSecondSection, 0);
  }
  //MoveUpDown;
  Valv_Set( & Plough_A -> ValveUpDown, UpDown);
  //Left right:
  Valv_Set( & Plough_A -> ValveLeftRight, -Cmd_A -> LeftRight);
  //Steel Rubber
  Valv_Set( & Plough_A -> ValveSteelUpDown, Cmd_A -> Z);
  // Сдвиг
  Valv_Set( & Plough_B -> ValveLeftRight, -Cmd_B -> LeftRight);

}

void Ctrl_Inter_Plough_CH2800_Cycle(tABC_Attach * Plough, tJSCommands * Cmd) {
  const tParamData * Parameter = ParameterGet();
  static int8_t float_flag = 0;
  static uint32_t float_tmr = 0;
  int32_t UpDown = -Cmd -> Updown;

  /********************************************************
   * Включение/выключение плавания
   * Для активации плуг должен опуститься вниз по таймеру
   * Таймер задан в параметрах
   ********************************************************/
  switch (Plough -> ValveUpDown.Type) {
  case VALVE_TYPE_PVE_AHS:
    if (Cmd -> Float == FLOAT_ON) {
      if (float_flag == 1) {
        Valv_Set( & Plough -> ValveFloat, 1);
        Valv_Set( & Plough -> ValveLock, 1);
      }
    } else {
      Valv_Set( & Plough -> ValveFloat, 0);
      Valv_Set( & Plough -> ValveLock, 0);
      float_flag = 0;
    }
    break;
  case VALVE_TYPE_PVE_AHS32:
    if (Cmd -> Float == FLOAT_ON) {
      if (float_flag == 1) {
        UpDown = 2500;
      }
    } else {
      float_flag = 0;
    }
    break;
  case VALVE_TYPE_OMFB:
    if (Cmd -> Float == FLOAT_ON) {
      if (float_flag == 1) {
        UpDown = 1000;
        Valv_Set( & Plough -> ValveUpDownSecondary, 0);
      }
    } else {
      float_flag = 0;
    }
    break;
  case VALVE_TYPE_PVE_O:
    if (Cmd -> Float == FLOAT_ON) {
      if (float_flag == 1) {
        UpDown = 0;
        Valv_Set( & Plough -> ValveLock, 1);
      }
    } else {
      float_flag = 0;
    }
    break;
  default:
    break;
  }

  /********************************************************
   * Опускание плуга вниз, если плуг не был опущен,
   * или во время работы плавания был поднят вверх
   ********************************************************/
  if (FLOAT_ON == Cmd -> Float && float_flag == 0) {
    if (Cmd -> F_New) {
      // Получение global system timer в момент активации плавания
      float_tmr = GetMSTick();
    }
    // Опускание плуга в течение заданного времени CH2800_timer
    if ((GetMSTick() - float_tmr) < Parameter -> EquipTimers.InterDown_timer) {
      switch (Plough -> ValveUpDown.Type) {
      case VALVE_TYPE_PVE_AHS:
      case VALVE_TYPE_PVE_AHS32:
      case VALVE_TYPE_PVE_O:
        UpDown = 1000;
        Valv_Set( & Plough -> ValveFloat, 0);
        Valv_Set( & Plough -> ValveLock, 1);
        break;
      case VALVE_TYPE_OMFB:
        if(!Parameter->OMFB.Invert_UpDown_B) {
          UpDown = Parameter->OMFB.PWM_UpDown_B;
          Valv_Set( & Plough -> ValveUpDownSecondary, 0);
        } else {
          UpDown = 0;
          Valv_Set( & Plough -> ValveUpDownSecondary, 1);
        }
        Valv_Set( & Plough -> ValveLock, 1);
        break;
      default:
        break;
      }

      InfoContainerOn(CNT_ATTACHMOVE);
      SetVarIndexed(IDX_ATTACHPIC, ATPIC_PLOUGH_CH2600);
      SetVarIndexed(IDX_ATTACHMENT_UP, 1);
      SetVarIndexed(IDX_ATTACHMENT_DOWN, 2);
    } else {
      /*******************************************
       * Активация флага включения плавания
       * После этого на плуге включается плавание
       *******************************************/
      float_flag = 1;
    }
  }

  if (FLOAT_ON == Cmd -> Float) {
    SetVarIndexed(IDX_FLOATING_INTERAX, 1);
    Ctrl_SetMaxSpeed(Plough -> MaxSpeed);
  } else {
    SetVarIndexed(IDX_FLOATING_INTERAX, 0);
  }

  // Управление вверх/вниз для OMFB
  if (Plough -> ValveUpDown.Type == VALVE_TYPE_OMFB) {
    if(!Cmd -> Updown) {
      if (FLOAT_OFF == Cmd -> Float) {
        UpDown = 0;
        Valv_Set( & Plough -> ValveUpDownSecondary, 0);
      }
    }
    if (!Parameter->OMFB.Invert_UpDown_B) {
      if (Cmd -> Updown < 0) {
        UpDown = Parameter->OMFB.PWM_UpDown_B;
        Valv_Set( & Plough -> ValveUpDownSecondary, 0);
      } else if (Cmd -> Updown > 0) {
        UpDown = 0;
        Valv_Set( & Plough -> ValveUpDownSecondary, 1);;
      }
    } else {
      if (Cmd -> Updown > 0) {
        UpDown = Parameter->OMFB.PWM_UpDown_B;
        Valv_Set( & Plough -> ValveUpDownSecondary, 0);
      } else if (Cmd -> Updown < 0) {
        UpDown = 0;
        Valv_Set( & Plough -> ValveUpDownSecondary, 1);
      }
    }
  }

  // Управление плугом при подъеме/опускании
  if (Cmd -> Updown) {
    float_flag = 0;
    Valv_Set( & Plough -> ValveLock, 1);
  } else {
    if (Cmd -> Float == FLOAT_OFF) {
      Valv_Set( & Plough -> ValveLock, 0);
    }
  }
  Valv_Set( & Plough -> ValveUpDown, UpDown);

}
