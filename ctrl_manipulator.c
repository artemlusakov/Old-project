#include <stdint.h>
#include <UserCEvents.h>
#include <UserCAPI.h>
#include "vartab.h"
#include "objtab.h"
#include "control.h"
#include "io.h"
#include "ctrl_manipulator.h"
#include "ctrl_top_summer_attachment.h"

extern F_UpDown[5];
extern F_LeftRight[5];
extern F_Float[5];
extern F_Lock[5];
extern F_3rdFunc[5];
extern F_Rotate[5];

void Manipulator_Section1_Init(tABC_Attach * Manipulator, uint32_t Device1, uint32_t Pos1, uint32_t Device2, uint32_t Pos2) {
  Valv_Init( & Manipulator -> ValveUpDown, Device1, Pos1, Device2, Pos2);
}
void Manipulator_Lock_Init(tABC_Attach * Manipulator, uint32_t Device1, uint32_t Pos1) {
  ValvDisk_Init( & Manipulator -> ValveLock, Device1, Pos1);
}
void Manipulator_Section2_Init(tABC_Attach * Manipulator, uint32_t DeviceTurnLeft, uint32_t PosTurnLeft, uint32_t DeviceTurnRight, uint32_t PosTurnRight) {
  Valv_Init( & Manipulator -> ValveLeftRight, DeviceTurnLeft, PosTurnLeft, DeviceTurnRight, PosTurnRight);
}
void Manipulator_Section3_Init(tABC_Attach * Manipulator, uint32_t DeviceSteelUp, uint32_t PosSteelUp, uint32_t DeviceSteelDown, uint32_t PosSteelDown) {
  Valv_Init( & Manipulator -> ValveSteelUpDown, DeviceSteelUp, PosSteelUp, DeviceSteelDown, PosSteelDown);
}
void Manipulator_Motor_Init(tABC_Attach * Manipulator, uint32_t Device1, uint32_t Pos1, uint32_t Device2, uint32_t Pos2) {
  Valv_Init( & Manipulator -> Motor, Device1, Pos1, Device2, Pos2);
}
void Manipulator_Turn_Init(tABC_Attach * Manipulator, uint32_t Device1, uint32_t Pos1, uint32_t Device2, uint32_t Pos2) {
  Valv_Init( & Manipulator -> Turn, Device1, Pos1, Device2, Pos2);
}
void Manipulator_Divertor_Init(tABC_Attach * Manipulator, uint32_t Device, uint32_t Pos) {
  ValvDisk_Init( & Manipulator -> Divertor, Device, Pos);
}

tManipulatorTM ManipulatorTM;

void Ctrl_Manipulator_FMM600_Init(tControl * Ctrl) {
  Manipulator_Section1_Init( & Ctrl -> ABC[EQUIP_A].ABC_Attach, F_UpDown[0], F_UpDown[1], -1, -1); //гц1 UD // y - div on ГЦ2
  Manipulator_Section2_Init( & Ctrl -> ABC[EQUIP_A].ABC_Attach, F_LeftRight[0], F_LeftRight[1], F_LeftRight[2], F_LeftRight[3]); //гц4 5 LR
  Manipulator_Section3_Init( & Ctrl -> ABC[EQUIP_A].ABC_Attach, F_3rdFunc[0], F_3rdFunc[1], F_3rdFunc[2], F_3rdFunc[3]); //гц3 X
  Manipulator_Motor_Init( & Ctrl -> ABC[EQUIP_A].ABC_Attach, MCM250_3, 2, MCM250_3, 3); //z
  Manipulator_Divertor_Init( & Ctrl -> ABC[EQUIP_A].ABC_Attach, MCM250_3, 1); //job section 2
  WaterPump_SetMaxLiter(65000);
  Ctrl -> ABC[EQUIP_A].ABC_Attach.MaxSpeed = 60;
  SetVarIndexed(IDX_JOYSTICKSTATE, 1);
  ManipulatorTM.Set = CNT_FMM600;
  ManipulatorTM.Icon = 18;
}

void VisuManipulator(tControl * Ctrl) {
  int ARROW_INVISIBLE = 0;
  int ARROW_WHITE = 1;
  int ARROW_GREEN_0 = 2;
  int ARROW_GREEN_1 = 3;

  int8_t opn = 0;
  int32_t updown = IOT_AnaJoy_GetInPercent( & Ctrl -> joystick.UpDown);
  int32_t leftright = IOT_AnaJoy_GetInPercent( & Ctrl -> joystick.LeftRight);
  int32_t extract_X = IOT_AnaJoy_GetInPercent( & Ctrl -> joystick.X);
  int32_t extract_Y = IOT_AnaJoy_GetInPercent( & Ctrl -> joystick.Y);
  int32_t extract_Z = IOT_AnaJoy_GetInPercent( & Ctrl -> joystick.Z);
  if (!GetVarIndexed(IDX_JOYSTICKSTATE)) {
    //Up / Down
    if (updown == 0) {
      SetVarIndexed(IDX_ATTACHMENT_UP, ARROW_INVISIBLE);
      SetVarIndexed(IDX_ATTACHMENT_DOWN, ARROW_INVISIBLE);
    } else if (updown < 0) {
      SetVarIndexed(IDX_FMM600_ICON, 1);
      opn = 1;
      SetVarIndexed(IDX_ATTACHMENT_UP, ARROW_GREEN_0);
      SetVarIndexed(IDX_ATTACHMENT_DOWN, ARROW_WHITE);
    } else if (updown > 0) {
      SetVarIndexed(IDX_FMM600_ICON, 1);
      opn = 1;
      SetVarIndexed(IDX_ATTACHMENT_DOWN, ARROW_GREEN_0);
      SetVarIndexed(IDX_ATTACHMENT_UP, ARROW_WHITE);
    } else {
      SetVarIndexed(IDX_ATTACHMENT_DOWN, ARROW_WHITE);
      SetVarIndexed(IDX_ATTACHMENT_UP, ARROW_WHITE);
    }

    //Left /Right
    if (leftright == 0) {
      SetVarIndexed(IDX_ATTACHMENT_RIGHT, ARROW_INVISIBLE);
      SetVarIndexed(IDX_ATTACHMENT_LEFT, ARROW_INVISIBLE);
    } else if (leftright > 0) {
      SetVarIndexed(IDX_FMM600_ICON, 1);
      opn = 1;
      SetVarIndexed(IDX_ATTACHMENT_RIGHT, ARROW_GREEN_0);
      SetVarIndexed(IDX_ATTACHMENT_LEFT, ARROW_WHITE);
    } else if (leftright < 0) {
      SetVarIndexed(IDX_FMM600_ICON, 1);
      opn = 1;
      SetVarIndexed(IDX_ATTACHMENT_LEFT, ARROW_GREEN_0);
      SetVarIndexed(IDX_ATTACHMENT_RIGHT, ARROW_WHITE);
    } else {
      SetVarIndexed(IDX_ATTACHMENT_LEFT, ARROW_WHITE);
      SetVarIndexed(IDX_ATTACHMENT_RIGHT, ARROW_WHITE);
    }

    //Extract /Retract
    if (extract_Y == 0) {
      SetVarIndexed(IDX_ATTACHMENT_UP_Y, ARROW_INVISIBLE);
      SetVarIndexed(IDX_ATTACHMENT_DOWN_Y, ARROW_INVISIBLE);
    } else if (extract_Y > 0) {
      SetVarIndexed(IDX_FMM600_ICON, 2);
      opn = 1;
      SetVarIndexed(IDX_ATTACHMENT_UP_Y, ARROW_GREEN_0);
      SetVarIndexed(IDX_ATTACHMENT_DOWN_Y, ARROW_WHITE);
    } else if (extract_Y < 0) {
      SetVarIndexed(IDX_FMM600_ICON, 2);
      opn = 1;
      SetVarIndexed(IDX_ATTACHMENT_DOWN_Y, ARROW_GREEN_1);
      SetVarIndexed(IDX_ATTACHMENT_UP_Y, ARROW_WHITE);
    } else {
      SetVarIndexed(IDX_ATTACHMENT_UP_Y, ARROW_WHITE);
      SetVarIndexed(IDX_ATTACHMENT_DOWN_Y, ARROW_WHITE);
    }

    //Extract /Retract
    if (extract_Z == 0) {
      SetVarIndexed(IDX_ATTACHMENT_UP_Z, ARROW_INVISIBLE);
    } else if (extract_Z > 0) {
      SetVarIndexed(IDX_FMM600_ICON, 4);
      opn = 1;
      SetVarIndexed(IDX_ATTACHMENT_UP_Z, ARROW_GREEN_0);
    } else if (extract_Z < 0) {
      SetVarIndexed(IDX_FMM600_ICON, 4);
      opn = 1;
      SetVarIndexed(IDX_ATTACHMENT_UP_Z, ARROW_GREEN_1);
    } else {
      SetVarIndexed(IDX_ATTACHMENT_UP_Z, ARROW_WHITE);
    }

    //Extract /Retract
    if (extract_X == 0) {
      SetVarIndexed(IDX_ATTACHMENT_UP_X, ARROW_INVISIBLE);
      SetVarIndexed(IDX_ATTACHMENT_DOWN_X, ARROW_INVISIBLE);
    } else if (extract_X < 0) {
      SetVarIndexed(IDX_FMM600_ICON, 3);
      opn = 1;
      SetVarIndexed(IDX_ATTACHMENT_DOWN_X, ARROW_GREEN_0);
      SetVarIndexed(IDX_ATTACHMENT_UP_X, ARROW_WHITE);
    } else if (extract_X > 0) {
      SetVarIndexed(IDX_FMM600_ICON, 3);
      opn = 1;
      SetVarIndexed(IDX_ATTACHMENT_UP_X, ARROW_GREEN_0);
      SetVarIndexed(IDX_ATTACHMENT_DOWN_X, ARROW_WHITE);
    } else {
      SetVarIndexed(IDX_ATTACHMENT_UP_X, ARROW_WHITE);
      SetVarIndexed(IDX_ATTACHMENT_DOWN_X, ARROW_WHITE);
    }
  } else {
    SetVarIndexed(IDX_ATTACHMENT_UP, ARROW_INVISIBLE);
    SetVarIndexed(IDX_ATTACHMENT_DOWN, ARROW_INVISIBLE);
    SetVarIndexed(IDX_ATTACHMENT_RIGHT, ARROW_INVISIBLE);
    SetVarIndexed(IDX_ATTACHMENT_LEFT, ARROW_INVISIBLE);
    SetVarIndexed(IDX_ATTACHMENT_UP_X, ARROW_INVISIBLE);
    SetVarIndexed(IDX_ATTACHMENT_DOWN_X, ARROW_INVISIBLE);
    SetVarIndexed(IDX_ATTACHMENT_UP_Y, ARROW_INVISIBLE);
    SetVarIndexed(IDX_ATTACHMENT_DOWN_Y, ARROW_INVISIBLE);
    SetVarIndexed(IDX_ATTACHMENT_UP_Z, ARROW_INVISIBLE);
  }

  if (!opn) {
    SetVarIndexed(IDX_FMM600_ICON, 0);
  }
}

void Manipulator_Cycle(tControl * Ctrl, tABC_Attach * Manipulator, tJSCommands * Cmd) {

  if (!IsMsgContainerOn(CNT_LIGHTKEYS) && !IsMsgContainerOn(CNT_LIGHTKEYS_2) && IsInfoContainerOn(CNT_FMM600)) {
    if (IsKeyPressedNew(4)) {
      SetVarIndexed(IDX_JOYSTICKSTATE, 1 - GetVarIndexed(IDX_JOYSTICKSTATE));
    }
  }

  VisuManipulator(Ctrl);
  if (!GetVarIndexed(IDX_JOYSTICKSTATE)) {

    //Left right:
    if (Manipulator -> ValveLeftRight.Type == VALVE_TYPE_PVE_O) {
      Valv_Set( & Manipulator -> ValveLeftRight, -10 * IOT_AnaJoy_GetInPercent( & Ctrl -> joystick.LeftRight));
    } else if (Manipulator -> ValveLeftRight.Type == VALVE_TYPE_PVE_AHS) {
      ValvProp_25_50_75_Set( & Manipulator -> ValveLeftRight.Valve.PVEH, -10 * IOT_AnaJoy_GetInPercent( & Ctrl -> joystick.LeftRight));
    }
    if (IOT_AnaJoy_GetInPercent( & Ctrl -> joystick.Y)) {
      Valv_Set( & Manipulator -> Divertor, 1);
      ValvProp_25_50_75_Set( & Manipulator -> ValveUpDown.Valve.PVEH, 10 * IOT_AnaJoy_GetInPercent( & Ctrl -> joystick.Y));
    } else { //MoveUpDown;
      Valv_Set( & Manipulator -> Divertor, 0);
      ValvProp_25_50_75_Set( & Manipulator -> ValveUpDown.Valve.PVEH, 10 * IOT_AnaJoy_GetInPercent( & Ctrl -> joystick.UpDown));
    }

    //Steel Rubber
    if (Manipulator -> ValveSteelUpDown.Type == VALVE_TYPE_PVE_O) {
      Valv_Set( & Manipulator -> ValveSteelUpDown, IOT_AnaJoy_GetInPercent( & Ctrl -> joystick.X) * -10);
    } else if (Manipulator -> ValveSteelUpDown.Type == VALVE_TYPE_PVE_AHS) {
      ValvProp_25_50_75_Set( & Manipulator -> ValveSteelUpDown.Valve.PVEH, IOT_AnaJoy_GetInPercent( & Ctrl -> joystick.X) * -10);
    }

    //Motor
    if (Manipulator -> Motor.Type == VALVE_TYPE_PVE_O) {
      Valv_Set( & Manipulator -> Motor, IOT_AnaJoy_GetInPercent( & Ctrl -> joystick.Z));
    } else if (Manipulator -> Motor.Type == VALVE_TYPE_PVE_AHS) {
      ValvProp_25_50_75_Set( & Manipulator -> Motor.Valve.PVEH, 10 * IOT_AnaJoy_GetInPercent( & Ctrl -> joystick.Z));
    }

    //Turn
    if (Manipulator -> Turn.Type == VALVE_TYPE_PVE_O) {
      Valv_Set( & Manipulator -> Turn, IOT_AnaJoy_GetInPercent( & Ctrl -> joystick.Z) * -10);
    } else if (Manipulator -> Turn.Type == VALVE_TYPE_PVE_AHS) {
      ValvProp_Set( & Manipulator -> Turn.Valve.PVEH, IOT_AnaJoy_GetInPercent( & Ctrl -> joystick.Z) * -10);
    }
  } else {
    //MoveUpDown;
    ValvProp_25_50_75_Set( & Manipulator -> ValveUpDown.Valve.PVEH, 0);

    //Left right:
    if (Manipulator -> ValveLeftRight.Type == VALVE_TYPE_PVE_O) {
      Valv_Set( & Manipulator -> ValveLeftRight, 0);
    } else if (Manipulator -> ValveLeftRight.Type == VALVE_TYPE_PVE_AHS) {
      ValvProp_25_50_75_Set( & Manipulator -> ValveLeftRight.Valve.PVEH, 0);
    }
    Valv_Set( & Manipulator -> Divertor, 0);
    //Steel Rubber
    if (Manipulator -> ValveSteelUpDown.Type == VALVE_TYPE_PVE_O) {
      Valv_Set( & Manipulator -> ValveSteelUpDown, 0);
    } else if (Manipulator -> ValveSteelUpDown.Type == VALVE_TYPE_PVE_AHS) {
      ValvProp_25_50_75_Set( & Manipulator -> ValveSteelUpDown.Valve.PVEH, 0);
    }

    //Motor
    if (Manipulator -> Motor.Type == VALVE_TYPE_PVE_O) {
      Valv_Set( & Manipulator -> Motor, 0);
    } else if (Manipulator -> Motor.Type == VALVE_TYPE_PVE_AHS) {
      ValvProp_25_50_75_Set( & Manipulator -> Motor.Valve.PVEH, 0);
    }

    //Turn
    if (Manipulator -> Turn.Type == VALVE_TYPE_PVE_O) {
      Valv_Set( & Manipulator -> Turn, 0);
    } else if (Manipulator -> Turn.Type == VALVE_TYPE_PVE_AHS) {
      ValvProp_25_50_75_Set( & Manipulator -> Turn.Valve.PVEH, 0);
    }

    ManipulatorTM.TM = 0;

  }

}

void Ctrl_Manipulator_Standard_Cycle(tControl * Ctrl, uint32_t evtc, tUserCEvt * evtv) {
  //Ctrl_Front_Manipulator_Draw( Ctrl );
  Manipulator_Cycle(Ctrl, & Ctrl -> ABC[EQUIP_A].ABC_Attach, & Ctrl -> cmd_ABC[EQUIP_A]);
}