#include <stdint.h>
#include <UserCEvents.h>
#include <UserCAPI.h>
#include "vartab.h"
#include "objtab.h"
#include "ctrl_front_attach.h"
#include "control.h"
#include "pinout.h"
#include "profile.h"
#include "io.h"
#include "RCText.h"
#include "ctrl_brush.h"
#include "okb600.h"
#include "param.h"
#include "ctrl_top_summer_attachment.h"

int F_UpDown[5];
int F_LeftRight[5];
int F_Float[5];
int F_Lock[5];
int F_3rdFunc[5];
int F_Rotate[5];

void PinOut_Front_Init(void) {
  int16_t( * func[])(uint8_t Action, uint8_t Equip) = {
    PinOut_Get_MCM_A,
    PinOut_Get_Pin_A,
    PinOut_Get_MCM_B,
    PinOut_Get_Pin_B,
    PinOut_Get_ValveType,
  };
  for (uint32_t i = 0; i < GS_ARRAYELEMENTS(func); i++) {
    F_UpDown[i] = func[i](0, EQUIP_A);
    F_LeftRight[i] = func[i](1, EQUIP_A);
    F_Float[i] = func[i](2, EQUIP_A);
    F_Lock[i] = func[i](3, EQUIP_A);
    F_3rdFunc[i] = func[i](4, EQUIP_A);
    F_Rotate[i] = func[i](7, EQUIP_A);
  }
}

// Плуги
void Ctrl_Front_FK_Init(tControl * Ctrl) {
  Plough_UpDown_Init( & Ctrl -> ABC[EQUIP_A].ABC_Attach, F_UpDown[0], F_UpDown[1], F_UpDown[2], F_UpDown[3], F_UpDown[4]);
  OMFB_UpDownSecond_Init( & Ctrl -> ABC[EQUIP_A].ABC_Attach, F_UpDown[2], F_UpDown[3], -1, -1, F_UpDown[4]);
  Plough_Float_Init( & Ctrl -> ABC[EQUIP_A].ABC_Attach, F_Float[0], F_Float[1]);
  Plough_Lock_Init( & Ctrl -> ABC[EQUIP_A].ABC_Attach, F_Lock[0], F_Lock[1]);
  Ctrl -> ABC[EQUIP_A].ABC_Attach.MaxSpeed = 60;
}

void Ctrl_Front_TN34_Init(tControl * Ctrl) {
  Plough_UpDown_Init( & Ctrl -> ABC[EQUIP_A].ABC_Attach, F_UpDown[0], F_UpDown[1], F_UpDown[2], F_UpDown[3], F_UpDown[4]);
  OMFB_UpDownSecond_Init( & Ctrl -> ABC[EQUIP_A].ABC_Attach, F_UpDown[2], F_UpDown[3], -1, -1, F_UpDown[4]);
  Plough_Float_Init( & Ctrl -> ABC[EQUIP_A].ABC_Attach, F_Float[0], F_Float[1]);
  Plough_LeftRight_Init( & Ctrl -> ABC[EQUIP_A].ABC_Attach, F_LeftRight[0], F_LeftRight[1], F_LeftRight[2], F_LeftRight[3], F_LeftRight[4]);
  Plough_Lock_Init( & Ctrl -> ABC[EQUIP_A].ABC_Attach, F_Lock[0], F_Lock[1]);
  Ctrl -> ABC[EQUIP_A].ABC_Attach.MaxSpeed = 40;
}

void Ctrl_Front_MN_Init(tControl * Ctrl) {
  Plough_UpDown_Init( & Ctrl -> ABC[EQUIP_A].ABC_Attach, F_UpDown[0], F_UpDown[1], F_UpDown[2], F_UpDown[3], F_UpDown[4]);
  OMFB_UpDownSecond_Init( & Ctrl -> ABC[EQUIP_A].ABC_Attach, F_UpDown[2], F_UpDown[3], -1, -1, F_UpDown[4]);
  Plough_Float_Init( & Ctrl -> ABC[EQUIP_A].ABC_Attach, F_Float[0], F_Float[1]);
  Plough_LeftRight_Init( & Ctrl -> ABC[EQUIP_A].ABC_Attach, F_LeftRight[0], F_LeftRight[1], F_LeftRight[2], F_LeftRight[3], F_LeftRight[4]);
  Plough_Lock_Init( & Ctrl -> ABC[EQUIP_A].ABC_Attach, F_Lock[0], F_Lock[1]);
  Plough_SteelRubber_Init( & Ctrl -> ABC[EQUIP_A].ABC_Attach, F_3rdFunc[0], F_3rdFunc[1], F_3rdFunc[2], F_3rdFunc[3]);
  Ctrl -> ABC[EQUIP_A].ABC_Attach.MaxSpeed = 60;
}


// Щетки
void Ctrl_Front_OFF_OPF_245(tControl * Ctrl) {
  const tParamData * Param = ParameterGet();
  tBrushValve * BrushValve = & Ctrl -> ABC[EQUIP_A].BrushValve;
  Brush_UpDown_Init( BrushValve, F_UpDown[0], F_UpDown[1], F_UpDown[2], F_UpDown[3], F_UpDown[4]);
  Equip_Down_OMFB_Init( BrushValve, F_UpDown[2], F_UpDown[3], -1, -1, F_UpDown[4]);
  Brush_Lock_Init( BrushValve, F_Lock[0], F_Lock[1]);
  Brush_Float_Init(BrushValve, F_Float[0], F_Float[1]);
  Brush_Rotate_Init(BrushValve, F_Rotate[0], F_Rotate[1], F_Rotate[4]);
  Brush_LeftRight_Init(BrushValve, F_LeftRight[0], F_LeftRight[1], F_LeftRight[2], F_LeftRight[3], F_LeftRight[4]);
  Brush_A_B(BrushValve, PVE_PORT_B);
  BrushValve->MaxSpeed = 30;
  BrushValve->isFloat = 1;
  BrushValve->VRotatePWM = Param->OMFB.PWM_Rotate_OPF245;

  if(!Param->FrontBrush.NoHumid && !Param->FrontBrush.HumidFromRoration) {
    WaterPump_SetMaxLiter(20000);
  }
}
void Ctrl_Front_OMB1(tControl * Ctrl) {
  const tParamData * Param = ParameterGet();
  tBrushValve * BrushValve = & Ctrl -> ABC[EQUIP_A].BrushValve;
  Brush_UpDown_Init(BrushValve, F_UpDown[0], F_UpDown[1], F_UpDown[2], F_UpDown[3], F_UpDown[4]);
  Equip_Down_OMFB_Init( BrushValve, F_UpDown[2], F_UpDown[3], -1, -1, F_UpDown[4]);
  Brush_Lock_Init( BrushValve, F_Lock[0], F_Lock[1]);
  Brush_Rotate_Init(BrushValve, F_Rotate[0], F_Rotate[1], F_Rotate[4]);
  Brush_LeftRight_Init(BrushValve, F_LeftRight[0], F_LeftRight[1], F_LeftRight[2], F_LeftRight[3], F_LeftRight[4]);
  Brush_A_B(BrushValve, PVE_PORT_B);
  BrushValve->MaxSpeed = 5;
  BrushValve->VRotatePWM = Param->OMFB.PWM_Rotate_OMB1;

  if(!Param->FrontBrush.NoHumid && !Param->FrontBrush.HumidFromRoration) {
    WaterPump_SetMaxLiter(20000);
  }
}
void Ctrl_Front_OMT1(tControl * Ctrl) {
  const tParamData * Param = ParameterGet();
  tBrushValve * BrushValve = & Ctrl -> ABC[EQUIP_A].BrushValve;
  Brush_UpDown_Init( BrushValve, F_UpDown[0], F_UpDown[1], F_UpDown[2], F_UpDown[3], F_UpDown[4]);
  Equip_Down_OMFB_Init( BrushValve, F_UpDown[2], F_UpDown[3], -1, -1, F_UpDown[4]);
  Brush_Lock_Init( BrushValve, F_Lock[0], F_Lock[1]);
  Brush_Rotate_Init(BrushValve, F_Rotate[0], F_Rotate[1], F_Rotate[4]);
  Brush_LeftRight_Init(BrushValve, F_LeftRight[0], F_LeftRight[1], F_LeftRight[2], F_LeftRight[3], F_LeftRight[4]);
  Brush_Optional_Init(BrushValve, F_3rdFunc[0], F_3rdFunc[1], F_3rdFunc[2], F_3rdFunc[3]);
  Brush_A_B(BrushValve, PVE_PORT_B);
  BrushValve->MaxSpeed = 5;
  BrushValve->VRotatePWM = Param->OMFB.PWM_Rotate_OMT1;

  if(!Param->FrontBrush.NoHumid && !Param->FrontBrush.HumidFromRoration) {
    WaterPump_SetMaxLiter(20000);
  }
}
void Ctrl_Front_MF300(tControl * Ctrl) {
  tBrushValve * BrushValve = & Ctrl -> ABC[EQUIP_A].BrushValve;
  Brush_UpDown_Init( BrushValve, F_UpDown[0], F_UpDown[1], F_UpDown[2], F_UpDown[3], F_UpDown[4]);
  Equip_Down_OMFB_Init(BrushValve, F_UpDown[2], F_UpDown[3], -1, -1, F_UpDown[4]);
  Brush_LeftRight_Init(BrushValve, F_LeftRight[0], F_LeftRight[1], F_LeftRight[2], F_LeftRight[3], F_LeftRight[4]);
  Brush_VSideSec_Init(BrushValve, F_3rdFunc[0], F_3rdFunc[1], F_3rdFunc[2], F_3rdFunc[3]);
  Brush_Lock_Init( BrushValve, F_Lock[0], F_Lock[1]);
  WaterPump_SetMaxLiter(65000);
  Ctrl -> top.SummerTop.WasherAntiBoost = 1;
}
void Ctrl_Front_MF500(tControl * Ctrl) {
  // tBrushValve * BrushValve = & Ctrl -> ABC[EQUIP_A].BrushValve;
  // Brush_Lock_Init( & Ctrl -> ABC[EQUIP_A].ABC_Attach, F_Lock[0], F_Lock[1]);
  // Ctrl -> top.SummerTop.WasherAntiBoost = 1;
}

// Двухсекционные плуги
void Ctrl_Front_MSPN_A(tControl * Ctrl) {
  const tParamData * Param = ParameterGet();
  if ((Param -> VIN.ActualHydDistributor == HydDistributor_OMFB) || (Param -> VIN.ActualHydDistributor == HydDistributor_OMFBv2)) { // OMFB
    Plough_UpDown_Init( & Ctrl -> ABC[EQUIP_A].ABC_Attach, F_UpDown[0], F_UpDown[1], F_UpDown[2], F_UpDown[3], F_UpDown[4]);
    OMFB_UpDownSecond_Init( & Ctrl -> ABC[EQUIP_A].ABC_Attach, F_UpDown[2], F_UpDown[3], -1, -1, F_UpDown[4]);
    Plough_LeftRight_Init( & Ctrl -> ABC[EQUIP_A].ABC_Attach, F_LeftRight[0], F_LeftRight[1], F_LeftRight[2], F_LeftRight[3], F_LeftRight[4]);
    Plough_Lock_Init( & Ctrl -> ABC[EQUIP_A].ABC_Attach, F_Lock[0], F_Lock[1]);
    Plough_SteelRubber_Init( & Ctrl -> ABC[EQUIP_A].ABC_Attach, F_3rdFunc[0], F_3rdFunc[1], F_3rdFunc[2], F_3rdFunc[3]);
  } else {
    Plough_UpDown_Init( & Ctrl -> ABC[EQUIP_A].ABC_Attach, F_UpDown[0], F_UpDown[1], F_UpDown[2], F_UpDown[3], 0);
    Plough_Float_Init( & Ctrl -> ABC[EQUIP_A].ABC_Attach, F_Float[0], F_Float[1]);
    Plough_LeftRight_Init( & Ctrl -> ABC[EQUIP_A].ABC_Attach, F_LeftRight[0], F_LeftRight[1], F_LeftRight[2], F_LeftRight[3], 0);
    Plough_Lock_Init( & Ctrl -> ABC[EQUIP_A].ABC_Attach, F_Lock[0], F_Lock[1]);
    Plough_SteelRubber_Init( & Ctrl -> ABC[EQUIP_A].ABC_Attach, F_3rdFunc[0], F_3rdFunc[1], F_3rdFunc[2], F_3rdFunc[3]);
  }
  Ctrl -> ABC[EQUIP_A].ABC_Attach.MaxSpeed = 40;
}

void Ctrl_Front_Brush_Draw(tControl * Ctrl) {
  const tProfile* Prof = Profile_GetCurrentProfile();
  if (Brush_Active( & Ctrl -> ABC[EQUIP_A].BrushValve)) {
    static int32_t _time = 0;
    if (250 < GetMSTick() - _time) {
      _time = GetMSTick();
      int32_t pos = GetVarIndexed(IDX_BRUSHON_FRONT) + 1;
      if (pos > 3) {
        pos = 1;
      }
      SetVarIndexed(IDX_BRUSHON_FRONT, pos);
    }
  } else {
    SetVarIndexed(IDX_BRUSHON_FRONT, 0);
  }

  uint8_t flagFloat = 0;
  if (Ctrl->cmd_ABC[EQUIP_A].Float == FLOAT_ON) {
    flagFloat = 1;
  }

  if(Prof->module[1] == RCTEXT_LI_A_ATTACHMENT_ATTACHMENT_OMB1 ||
     Prof->module[1] == RCTEXT_LI_A_ATTACHMENT_ATTACHMENT_OMT1 ||
     Prof->module[1] == RCTEXT_LI_A_ATTACHMENT_ATTACHMENT_MF300 ||
     Prof->module[1] == RCTEXT_LI_A_ATTACHMENT_ATTACHMENT_MF500) {
      flagFloat = 0;
  }
  SetVarIndexed(IDX_FLOATING_FRONT, flagFloat);
}

void Ctrl_Front_Brush_Standard_Cycle(tControl * Ctrl, uint32_t evtc, tUserCEvt * evtv) {
  Ctrl_Front_Brush_Draw(Ctrl);
  Brush_Cycle( & Ctrl -> ABC[EQUIP_A].BrushValve, & Ctrl -> cmd_ABC[EQUIP_A], EQUIP_A);
}

void Ctrl_Front_Brush_Timer_10ms(tControl * Ctrl) {
  Brush_Timer_10ms( & Ctrl -> ABC[EQUIP_A].BrushValve);
}


// OKB600
void Ctrl_Front_OKB600_Init(tControl * Ctrl) {
  Ctrl_OKB600_Init( & Ctrl -> ABC[EQUIP_A].OKB600Valve);
}

void Ctrl_Front_OKB600_Cycle(tControl * Ctrl) {
  Ctrl_OKB600_Cycle( & Ctrl -> ABC[EQUIP_A].OKB600Valve, & Ctrl -> cmd_ABC[EQUIP_A]);
}

void Ctrl_Front_OKB600_Timer_10ms(tControl * Ctrl) {
  Ctrl_OKB600_Timer_10ms( & Ctrl -> ABC[EQUIP_A].OKB600Valve);
}


void Ctrl_Front_Plough_Draw(tControl * Ctrl) {
  if (Ctrl->cmd_ABC[EQUIP_A].Float == FLOAT_ON) {
    SetVarIndexed(IDX_FLOATING_FRONT, 1);
  } else {
    SetVarIndexed(IDX_FLOATING_FRONT, 0);
  }
}

void Ctrl_Front_Plough_Standard_Timer_10ms(tControl * Ctrl) {
  Plough_UpDown_Timer_10ms( & Ctrl -> ABC[EQUIP_A].ABC_Attach);
}


void Ctrl_Front_Plough_Standard_Cycle(tControl * Ctrl, uint32_t evtc, tUserCEvt * evtv) {
  Ctrl_Front_Plough_Draw(Ctrl);
  Plough_Cycle( & Ctrl -> ABC[EQUIP_A].ABC_Attach, & Ctrl -> cmd_ABC[EQUIP_A], EQUIP_A);
}

void Ctrl_Front_Plough_2_Sections_MSPN_Cycle(tControl * Ctrl, uint32_t evtc, tUserCEvt * evtv) {
  Ctrl_Front_Plough_Draw(Ctrl);
  Plough_2_Section_MSPN_Cycle( & Ctrl -> ABC[EQUIP_A].ABC_Attach, & Ctrl -> cmd_ABC[EQUIP_A], & Ctrl -> ABC[EQUIP_B].ABC_Attach, & Ctrl -> cmd_ABC[EQUIP_B]);
}
