#include <stdint.h>
#include <UserCEvents.h>
#include <UserCAPI.h>
#include "vartab.h"
#include "objtab.h"
#include "control.h"
#include "ctrl_front_attach.h"
#include "RCText.h"
#include "io.h"
#include "io_types.h"
#include "gsemcm.h"
#include "pinout.h"
#include "param.h"
#include "profile.h"
#include "gseerrorlist.h"
#include "errorlist.h"
#include "ctrl_brush.h"

#define PloughAutoParkOff 0
#define PloughAutoParkOn 1

#define movePloughUp 0
#define movePloughTiltUp 1
#define movePloughLeft 2
#define PloughStop 3


typedef struct taqComValue {
  int16_t UpDown;
  int16_t UpDown_2;
  int16_t ValveUpDownSecondary;
  int16_t ValveUpDownSecondary_2;
  int16_t LeftRight;
  int16_t Lock;
  int16_t Float;
  int16_t Tilt;
  int16_t TiltSecondary;
  int16_t TiltLock;
}ComValue;

uint8_t moveAutoPark = 0;
uint8_t flagAutoPark = 0;

void Ctrl_Plough_AutoPark(tControl * Ctrl, ComValue * ComSide);
void Ctrl_Plough_AK_AutoPark(tControl * Ctrl, ComValue * ComSide);


int S_UpDown[5];
int S_LeftRight[5];
int S_Float[5];
int S_Lock[5];
int S_3rdFunc[5];

void PinOut_Side_Init(void) {
  int16_t( * func[])(uint8_t Action, uint8_t Equip) = {
    PinOut_Get_MCM_A,
    PinOut_Get_Pin_A,
    PinOut_Get_MCM_B,
    PinOut_Get_Pin_B,
    PinOut_Get_ValveType,
  };
  for (uint32_t i = 0; i < GS_ARRAYELEMENTS(func); i++) {
    S_UpDown[i] = func[i](0, EQUIP_C);
    S_LeftRight[i] = func[i](1, EQUIP_C);
    S_Float[i] = func[i](2, EQUIP_C);
    S_Lock[i] = func[i](3, EQUIP_C);
    S_3rdFunc[i] = func[i](8, EQUIP_C);
  }
}

void Ctrl_Side_BUCHER_AK_Init(tControl * Ctrl) {
  const tParamData * Param = ParameterGet();
  if ((Param -> VIN.ActualHydDistributor == HydDistributor_OMFB) || (Param -> VIN.ActualHydDistributor == HydDistributor_OMFBv2)) { // OMFB
    Plough_UpDown_Init( & Ctrl -> ABC[EQUIP_C].ABC_Attach, S_UpDown[0], S_UpDown[1], S_UpDown[2], S_UpDown[3], S_UpDown[4]);
    OMFB_UpDownSecond_Init( & Ctrl -> ABC[EQUIP_C].ABC_Attach, S_UpDown[2], S_UpDown[3], -1, -1, S_UpDown[4]);
    Plough_Lock_Init( & Ctrl -> ABC[EQUIP_C].ABC_Attach, S_Lock[0], S_Lock[1]);
    Plough_WorkTransport( & Ctrl -> ABC[EQUIP_C].ABC_Attach, S_LeftRight[0], S_LeftRight[1], S_LeftRight[2], S_LeftRight[3], S_LeftRight[4]);
    Ctrl -> ABC[EQUIP_C].ABC_Attach.MaxSpeed = 40;
  } else {
    Plough_UpDown_Init( & Ctrl -> ABC[EQUIP_C].ABC_Attach, MCM250_1, 6, MCM250_1, 4, 0);
    Plough_UpDown_Second_Init( & Ctrl -> ABC[EQUIP_C].ABC_Attach, MCM250_2, 6, MCM250_2, 4, 0);
    Plough_Float_Init( & Ctrl -> ABC[EQUIP_C].ABC_Attach, MCM250_2, 9);
    Plough_Lock_Init( & Ctrl -> ABC[EQUIP_C].ABC_Attach, MCM250_2, 0);
    Plough_WorkTransport( & Ctrl -> ABC[EQUIP_C].ABC_Attach, MCM250_2, 5, MCM250_2, 7, 0);
    Ctrl -> ABC[EQUIP_C].ABC_Attach.MaxSpeed = 40;
  }
}

void Ctrl_Side_AA_AC_Init(tControl * Ctrl) {
  Plough_UpDown_Init( & Ctrl -> ABC[EQUIP_C].ABC_Attach, S_UpDown[0], S_UpDown[1], S_UpDown[2], S_UpDown[3], S_UpDown[4]);
  OMFB_UpDownSecond_Init( & Ctrl -> ABC[EQUIP_C].ABC_Attach, S_UpDown[2], S_UpDown[3], -1, -1, S_UpDown[4]);
  Plough_Float_Init( & Ctrl -> ABC[EQUIP_C].ABC_Attach, S_Float[0], S_Float[1]);
  Plough_WorkTransport( & Ctrl -> ABC[EQUIP_C].ABC_Attach, S_LeftRight[0], S_LeftRight[1], S_LeftRight[2], S_LeftRight[3], S_LeftRight[4]);
  Plough_TiltUpDown( & Ctrl -> ABC[EQUIP_C].ABC_Attach, S_3rdFunc[0], S_3rdFunc[1], S_3rdFunc[2], S_3rdFunc[3], S_3rdFunc[4]);
  Plough_Lock_Init( & Ctrl -> ABC[EQUIP_C].ABC_Attach, S_Lock[0], S_Lock[1]);
  Plough_TiltLock_Init( & Ctrl -> ABC[EQUIP_C].ABC_Attach, S_Lock[2], S_Lock[3]);
  Ctrl -> ABC[EQUIP_C].ABC_Attach.MaxSpeed = 40;
}

void Ctrl_Side_MSN37_Init(tControl * Ctrl) {
  Plough_UpDown_Init( & Ctrl -> ABC[EQUIP_C].ABC_Attach, S_UpDown[0], S_UpDown[1], S_UpDown[2], S_UpDown[3], S_UpDown[4]);
  OMFB_UpDownSecond_Init( & Ctrl -> ABC[EQUIP_C].ABC_Attach, S_UpDown[2], S_UpDown[3], -1, -1, S_UpDown[4]);
  Plough_Float_Init( & Ctrl -> ABC[EQUIP_C].ABC_Attach, S_Float[0], S_Float[1]);
  Plough_WorkTransport( & Ctrl -> ABC[EQUIP_C].ABC_Attach, S_LeftRight[0], S_LeftRight[1], S_LeftRight[2], S_LeftRight[3], S_LeftRight[4]);
  Plough_Lock_Init( & Ctrl -> ABC[EQUIP_C].ABC_Attach, S_Lock[0], S_Lock[1]);
  Ctrl -> ABC[EQUIP_C].ABC_Attach.MaxSpeed = 60;
}

void Ctrl_Side_Plough_Cycle(tControl * Ctrl, uint32_t evtc, tUserCEvt * evtv) {
  ComValue ComSide;
  const tParamData * Parameter = ParameterGet();
  tABC_Attach * Plough = & Ctrl -> ABC[EQUIP_C].ABC_Attach;
  tJSCommands * Cmd = & Ctrl -> cmd_ABC[EQUIP_C];

  ComSide.UpDown = -Cmd -> Updown;
  ComSide.ValveUpDownSecondary = 0;
  ComSide.LeftRight = Cmd -> LeftRight;
  ComSide.Lock = 0;
  ComSide.Tilt = Cmd -> Z;
  ComSide.TiltSecondary = 0;
  ComSide.TiltLock = 0;

  switch (Plough -> ValveUpDown.Type) {
  case VALVE_TYPE_PVE_AHS:
    ComSide.Float = 0;
    if (FLOAT_ON == Cmd -> Float) {
      ComSide.Lock = 1;
      ComSide.Float = 1;
    }
    Valv_Set( & Plough -> ValveFloat, ComSide.Float);
    break;
  case VALVE_TYPE_PVE_AHS32:
    if (FLOAT_ON == Cmd -> Float) {
      ComSide.UpDown = 2500; // ШИМ 800
      ComSide.Lock = 1;
    }
    break;
  case VALVE_TYPE_OMFB:
    // Управление вверх/вниз для OMFB
    if (!Parameter -> OMFB.Invert_UpDown_C) {
      if (Cmd -> Updown < 0) {
        ComSide.UpDown = Parameter -> OMFB.PWM_UpDown_C;
      } else if (Cmd -> Updown > 0) {
        ComSide.UpDown = 0;
        ComSide.ValveUpDownSecondary = 1;
      }
    } else {
      if (Cmd -> Updown > 0) {
        ComSide.UpDown = Parameter -> OMFB.PWM_UpDown_C;
      } else if (Cmd -> Updown < 0) {
        ComSide.UpDown = 0;
        ComSide.ValveUpDownSecondary = 1;
      }
    }

    if (FLOAT_ON == Cmd -> Float) {
      ComSide.UpDown = 1000;
      ComSide.Lock = 1;
    }
    break;
  case VALVE_TYPE_PVE_O:
    if (FLOAT_ON == Cmd -> Float) {
      ComSide.UpDown = 0;
      ComSide.Lock = 1;
    }
    break;
  }

  switch (Plough -> ValveTilt.Type) {
  case VALVE_TYPE_OMFB:
    if (Cmd -> Z > 0) {
      ComSide.Tilt = Parameter -> OMFB.PWM_UpDown_C;
    } else if (Cmd -> Z < 0) {
      ComSide.TiltSecondary = 1;
      ComSide.Tilt = 0;
    }
    if (FLOAT_ON == Cmd -> FloatTilt) {
      ComSide.Tilt = 1000;
      ComSide.TiltLock = 1;
    }
    break;
  case VALVE_TYPE_PVE_O:
    if (FLOAT_ON == Cmd -> FloatTilt) {
      ComSide.TiltLock = 1;
    }
    break;
  }

  // Управление гидрозамком
  if (Cmd -> Updown) {
    ComSide.Lock = 1;
  } else {
    if (Cmd -> Float == FLOAT_OFF) {
      ComSide.Lock = 0;
    }
  }
  // Управление гидрозамком наклона (Plough AA)
  if (Cmd -> Z) {
    ComSide.TiltLock = 1;
  } else {
    if (Cmd -> FloatTilt == FLOAT_OFF) {
      ComSide.TiltLock = 0;
    }
  }

  if (Cmd -> Float == FLOAT_ON) {
    Ctrl_SetMaxSpeed(Plough -> MaxSpeed);
    SetVarIndexed(IDX_FLOATING_SIDE, 1);
  } else {
    SetVarIndexed(IDX_FLOATING_SIDE, 0);
  }

  if (Cmd -> FloatTilt == FLOAT_ON) {
    Ctrl_SetMaxSpeed(Plough -> MaxSpeed);
    SetVarIndexed(IDX_FLOATINGTILT_SIDE, 1);
  } else {
    SetVarIndexed(IDX_FLOATINGTILT_SIDE, 0);
  }

  Ctrl_Plough_AutoPark(Ctrl, & ComSide);

  Valv_Set( & Plough -> ValveUpDown, ComSide.UpDown);
  Valv_Set( & Plough -> ValveLock, ComSide.Lock);
  Valv_Set( & Plough -> ValveUpDownSecondary, ComSide.ValveUpDownSecondary);
  Valv_Set( & Plough -> ValveWorkTransport, ComSide.LeftRight);
  Valv_Set( & Plough -> ValveTilt, ComSide.Tilt);
  Valv_Set( & Plough -> ValveTiltSecondary, ComSide.TiltSecondary);
  Valv_Set( & Plough -> ValveTiltLock, ComSide.TiltLock);
}

void Ctrl_Joystic_AutoPark(tControl * Ctrl) {
  static uint8_t neutralJoyst = 0;
  if (IOT_AnaJoy_GetInPercent( & Ctrl -> joystick.UpDown) == 0 &&
    IOT_AnaJoy_GetInPercent( & Ctrl -> joystick.LeftRight) == 0) {
    neutralJoyst = 1;
  }
  if (neutralJoyst) {
    if (Ctrl -> cmd_ABC[EQUIP_C].Updown || Ctrl -> cmd_ABC[EQUIP_C].LeftRight) {
      neutralJoyst = 0;
      flagAutoPark = PloughAutoParkOff;
    }
  }
}

int8_t AutoParkTimeout(uint8_t StateMove) {
  const tParamData * Param = ParameterGet();
  static int8_t oldStateMove = -1;
  static uint32_t tmtUp = 0;
  if (oldStateMove != StateMove) {
    oldStateMove = StateMove;
    if (StateMove == movePloughUp) {
      tmtUp = GetMSTick() + Param -> SideAutoPark.TimerUp;
    } else if (StateMove == movePloughLeft) {
      tmtUp = GetMSTick() + Param -> SideAutoPark.TimerLeft;
    } else if (StateMove == movePloughTiltUp) {
      tmtUp = GetMSTick() + Param -> SideAutoPark.TimerTiltUp;
    }
  }
  if (GetMSTick() <= tmtUp) {
    return 1;
  } else {
    return 0;
  }
}

void Ctrl_Plough_AutoPark(tControl * Ctrl, ComValue * ComSide) {
  const tParamData * Param = ParameterGet();
  const tProfileModule * ProfModule = GetCurrentModule(3);
  const tProfile * Prof = Profile_GetCurrentProfile();
  static uint64_t timerStop = 0; // Таймер времени упора
  tABC_Attach * PloughSide = & Ctrl -> ABC[EQUIP_C].ABC_Attach;
  uint32_t globalTimer = Param -> SideAutoPark.SideAutoParkGlobalTimer;

  // Активация автопарковки
  if (IOT_Button_IsDown( & Ctrl -> joystick.Button[2]) &&
    IOT_Button_IsDown( & Ctrl -> joystick.Button[5]) &&
    IOT_AnaJoy_GetInPercent( & Ctrl -> joystick.UpDown) <= -30) {
    flagAutoPark = PloughAutoParkOn;

    if (Prof -> module[3] == RCTEXT_LI_C_ATTACHMENT_ATTACHMENT_AA_AC) {
      moveAutoPark = movePloughTiltUp;
    } else {
      moveAutoPark = movePloughUp;
    }
    timerStop = GetMSTick() + globalTimer;
  }

  if (flagAutoPark == PloughAutoParkOn) {

    InfoContainerOn(CNT_ATTACHMOVE);
    SetVarIndexed(IDX_ATTACHPIC, ProfModule -> Icon);

    // Деактивация автопарковки при управлении джойстиком
    Ctrl_Joystic_AutoPark(Ctrl);

    switch (moveAutoPark) {
    case movePloughTiltUp:
      if (AutoParkTimeout(movePloughTiltUp)) {
        if (PloughSide -> ValveTilt.Type != VALVE_TYPE_OMFB) {
          ComSide -> Tilt = 1000;
        } else {
          ComSide -> Tilt = Param -> OMFB.PWM_UpDown_C;
        }
        ComSide -> TiltLock = 1;

        SetVarIndexed(IDX_ATTACHMENT_SIDETILT, 2);
        SetVarIndexed(IDX_ATTACHMENT_UP, 1);
      } else {
        moveAutoPark = movePloughUp;
      }
      break;
    case movePloughUp:
      if (AutoParkTimeout(movePloughUp)) {
        if (PloughSide -> ValveUpDown.Type != VALVE_TYPE_OMFB) {
          ComSide -> UpDown = -1000;
        } else {
          if (!Param -> OMFB.Invert_UpDown_C) {
            ComSide -> UpDown = 0;
            ComSide -> ValveUpDownSecondary = 1;
          } else {
            ComSide -> UpDown = Param -> OMFB.PWM_UpDown_C;
          }
        }
        ComSide -> Lock = 1;

        SetVarIndexed(IDX_ATTACHMENT_UP, 2);
        SetVarIndexed(IDX_ATTACHMENT_SIDETILT, 0);
      } else {
        moveAutoPark = movePloughLeft;
      }
      break;

    case movePloughLeft:
      if (AutoParkTimeout(movePloughLeft)) {
        ComSide -> UpDown = 0;
        ComSide -> LeftRight = -1000;

        SetVarIndexed(IDX_ATTACHMENT_UP, 1);
        SetVarIndexed(IDX_ATTACHMENT_WORK, 3);
        SetVarIndexed(IDX_ATTACHMENT_SIDETILT, 0);
      } else {
        moveAutoPark = PloughStop;
      }
      break;

    case PloughStop:
      flagAutoPark = PloughAutoParkOff;
      break;

    default:
      break;
    }

    if (GetMSTick() >= timerStop) {
      flagAutoPark = PloughAutoParkOff;
    }
  }
}

void Ctrl_Side_AK_Cycle(tControl * Ctrl, uint32_t evtc, tUserCEvt * evtv) {
  const tParamData * Param = ParameterGet();
  const tProfile * Prof = Profile_GetCurrentProfile();
  tABC_Attach * Plough = & Ctrl -> ABC[EQUIP_C].ABC_Attach;
  tJSCommands * Cmd = & Ctrl -> cmd_ABC[EQUIP_C];
  ComValue ComSide;

  uint8_t LockSL = 0;
  ComSide.UpDown = -Cmd -> Updown;
  ComSide.UpDown_2 = 0;
  ComSide.ValveUpDownSecondary = 0;
  ComSide.ValveUpDownSecondary_2 = 0;
  ComSide.LeftRight = Cmd -> LeftRight;
  ComSide.Lock = 0;

  if ((Param -> VIN.ActualHydDistributor == HydDistributor_OMFB) || (Param -> VIN.ActualHydDistributor == HydDistributor_OMFBv2)) { // OMFB
    switch (Plough -> ValveUpDown.Type) {
    case VALVE_TYPE_OMFB:
      if (!Param -> OMFB.Invert_UpDown_C) {
        if (Cmd -> Updown < 0) {
          ComSide.UpDown = Param -> OMFB.PWM_UpDown_C;
          // LockSL = 1;
        } else if (Cmd -> Updown > 0) {
          ComSide.UpDown = 0;
          ComSide.ValveUpDownSecondary = 1;
        }
      } else {
        if (Cmd -> Updown > 0) {
          ComSide.UpDown = Param -> OMFB.PWM_UpDown_C;
        } else if (Cmd -> Updown < 0) {
          // LockSL = 1;
          ComSide.UpDown = 0;
          ComSide.ValveUpDownSecondary = 1;
        }
      }

      if (FLOAT_ON == Cmd -> Float) {
        ComSide.UpDown = 1000;
        ComSide.Lock = 1;
      }
      break;

    case VALVE_TYPE_PVE_O:
      if (Cmd -> Updown < 0) {
        ComSide.UpDown = 1;
        // LockSL = 1;
      } else if (Cmd -> Updown > 0) {
        ComSide.UpDown = -1;
      }

      if (FLOAT_ON == Cmd -> Float) {
        ComSide.Lock = 1;
      }
      break;
    }
  } else {
    ComSide.Float = 0;
    if (Cmd -> Updown < 0) { // Вниз
      ComSide.UpDown = 1;
      ComSide.ValveUpDownSecondary = 1;
      // LockSL = 1;
    } else if (Cmd -> Updown > 0) { // Вверх
      ComSide.UpDown = 0;
      ComSide.UpDown_2 = 1;
      ComSide.ValveUpDownSecondary_2 = 1;
    }

    if (FLOAT_ON == Cmd -> Float) {
      ComSide.UpDown = 1;
      ComSide.UpDown_2 = 1;
      ComSide.Lock = 1;
    }

    if (Cmd -> Updown || Cmd -> LeftRight) {
      ComSide.Float = 1;
    }
  }

  // Управление гидрозамком
  if (Prof -> module[3] == RCTEXT_LI_C_ATTACHMENT_ATTACHMENT_BUCHER_AK) {
    if (Cmd -> Updown) {
      ComSide.Lock = 1;
    } else {
      if (Cmd -> Float == FLOAT_OFF) {
        ComSide.Lock = 0;
      }
    }
  } else if (Prof -> module[3] == RCTEXT_LI_C_ATTACHMENT_ATTACHMENT_SL) {
    if (LockSL) {
      ComSide.Lock = 1;
    } else {
      if (Cmd -> Float == FLOAT_OFF) {
        ComSide.Lock = 0;
      }
    }
  }

  if (Cmd -> Float == FLOAT_ON) {
    Ctrl_SetMaxSpeed(Plough -> MaxSpeed);
    SetVarIndexed(IDX_FLOATING_SIDE, 1);
  } else {
    SetVarIndexed(IDX_FLOATING_SIDE, 0);
  }

  Ctrl_Plough_AK_AutoPark(Ctrl, & ComSide);

  if ((Param -> VIN.ActualHydDistributor == HydDistributor_OMFB) || (Param -> VIN.ActualHydDistributor == HydDistributor_OMFBv2)) { // OMFB
    if (Plough -> ValveUpDown.Type == VALVE_TYPE_OMFB) {
      Valv_Set( & Plough -> ValveUpDown, ComSide.UpDown);
      Valv_Set( & Plough -> ValveUpDownSecondary, ComSide.ValveUpDownSecondary);
    } else if (Plough -> ValveUpDown.Type == VALVE_TYPE_PVE_O) {
      Valv_Set( & Plough -> ValveUpDown, ComSide.UpDown);
    }
  } else {
    ValvDig_2_2_Set( & Plough -> ValveUpDown.Valve.PVEO[0], ComSide.UpDown);
    ValvDig_2_2_Set( & Plough -> ValveUpDown.Valve.PVEO[1], ComSide.UpDown_2);
    ValvDig_2_2_Set( & Plough -> ValveUpDownSecondary.Valve.PVEO[0], ComSide.ValveUpDownSecondary);
    ValvDig_2_2_Set( & Plough -> ValveUpDownSecondary.Valve.PVEO[1], ComSide.ValveUpDownSecondary_2);
  }

  Valv_Set( & Plough -> ValveFloat, ComSide.Float);
  Valv_Set( & Plough -> ValveLock, ComSide.Lock);
  Valv_Set( & Plough -> ValveWorkTransport, ComSide.LeftRight);
}

void Ctrl_Plough_AK_AutoPark(tControl * Ctrl, ComValue * ComSide) {
  const tParamData * Param = ParameterGet();
  const tProfile * Prof = Profile_GetCurrentProfile();
  static uint64_t timerStop = 0; // Таймер времени упора
  tABC_Attach * PloughSide = & Ctrl -> ABC[EQUIP_C].ABC_Attach;
  uint32_t globalTimer = Param -> SideAutoPark.SideAutoParkGlobalTimer;

  // Активация автопарковки
  if (IOT_Button_IsDown( & Ctrl -> joystick.Button[2]) &&
    IOT_Button_IsDown( & Ctrl -> joystick.Button[5]) &&
    IOT_AnaJoy_GetInPercent( & Ctrl -> joystick.UpDown) <= -30) {
    flagAutoPark = PloughAutoParkOn;
    moveAutoPark = movePloughUp;
    timerStop = GetMSTick() + globalTimer;
  }

  if (flagAutoPark == PloughAutoParkOn) {

    InfoContainerOn(CNT_ATTACHMOVE);
    SetVarIndexed(IDX_ATTACHPIC, 3);

    // Деактивация автопарковки при управлении джойстиком
    Ctrl_Joystic_AutoPark(Ctrl);

    // Подъем отвала вверх до упора
    if (moveAutoPark == movePloughUp) {
      if (AutoParkTimeout(movePloughUp)) {
        if ((Param -> VIN.ActualHydDistributor == HydDistributor_OMFB) || (Param -> VIN.ActualHydDistributor == HydDistributor_OMFBv2)) { // OMFB
          if (PloughSide -> ValveUpDown.Type == VALVE_TYPE_OMFB) {
            if (!Param -> OMFB.Invert_UpDown_C) {
              ComSide -> UpDown = 0;
              ComSide -> ValveUpDownSecondary = 1;
            } else {
              ComSide -> UpDown = Param -> OMFB.PWM_UpDown_C;
            }
          } else if (PloughSide -> ValveUpDown.Type == VALVE_TYPE_PVE_O) {
            ComSide -> UpDown = -1;
          }
        } else {
          ComSide -> Float = 1;
          ComSide -> UpDown = 0;
          ComSide -> UpDown_2 = 1;
          ComSide -> ValveUpDownSecondary_2 = 1;
        }

        if (Prof -> module[3] == RCTEXT_LI_C_ATTACHMENT_ATTACHMENT_BUCHER_AK) {
          ComSide -> Lock = 1;
        }
        SetVarIndexed(IDX_ATTACHMENT_UP, 2);
      } else {
        moveAutoPark = movePloughLeft;
      }
    }
    // Складывание отвала
    if (moveAutoPark == movePloughLeft) {
      if (AutoParkTimeout(movePloughLeft)) {
        ComSide -> LeftRight = -1000;
        ComSide -> Float = 1;
        SetVarIndexed(IDX_ATTACHMENT_UP, 1);
        SetVarIndexed(IDX_ATTACHMENT_WORK, 3);
      } else {
        moveAutoPark = PloughStop;
      }
    }
    if (moveAutoPark == PloughStop) {
      flagAutoPark = PloughAutoParkOff;
    }
    if (GetMSTick() >= timerStop) {
      flagAutoPark = PloughAutoParkOff;
    }
  }
}

void Ctrl_Side_Plough_Timer_10ms(tControl * Ctrl) {
  static int32_t count = 0;
  count++;
  if (0 == (count % 10)) {}
}
