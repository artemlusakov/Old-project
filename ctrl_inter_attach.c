#include <stdint.h>
#include <UserCEvents.h>
#include <UserCAPI.h>

#include "vartab.h"
#include "objtab.h"
#include "control.h"
#include "io.h"
#include "ctrl_brush.h"
#include "ctrl_plough.h"
#include "pinout.h"
#include "param.h"
#include "gsToVisu.h"

int I_UpDown[5];
int I_LeftRight[5];
int I_Float[5];
int I_Lock[5];
int I_Rotate[5];

void PinOut_Inter_Init(void) {
  int16_t( * func[])(uint8_t Action, uint8_t Equip) = {
    PinOut_Get_MCM_A,
    PinOut_Get_Pin_A,
    PinOut_Get_MCM_B,
    PinOut_Get_Pin_B,
    PinOut_Get_ValveType,
  };
  for (uint32_t i = 0; i < GS_ARRAYELEMENTS(func); i++) {
    I_UpDown[i] = func[i](0, EQUIP_B);
    I_LeftRight[i] = func[i](1, EQUIP_B);
    I_Float[i] = func[i](2, EQUIP_B);
    I_Lock[i] = func[i](3, EQUIP_B);
    I_Rotate[i] = func[i](7, EQUIP_B);
  }
}

// Щетки
void Ctrl_Inter_OMP220_Init(tControl * Ctrl) {
  const tParamData * Param = ParameterGet();
  tBrushValve * BrushValve = & Ctrl -> ABC[EQUIP_B].BrushValve;
  Brush_UpDown_Init( BrushValve, I_UpDown[0], I_UpDown[1], I_UpDown[2], I_UpDown[3], I_UpDown[4]);
  Equip_Down_OMFB_Init( BrushValve, I_UpDown[2], I_UpDown[3], -1, -1, I_UpDown[4]);
  Brush_Float_Init(BrushValve, I_Float[0], I_Float[1]);
  Brush_Lock_Init( BrushValve, I_Lock[0], I_Lock[1]);
  Brush_Rotate_Init(BrushValve, I_Rotate[0], I_Rotate[1], I_Rotate[4]);
  Brush_A_B(BrushValve, PVE_PORT_A);
  BrushValve->MaxSpeed = 30;
  BrushValve->isFloat = 1;
  BrushValve->VRotatePWM = Param->OMFB.PWM_Rotate_OMP;
}
void Ctrl_Inter_OMP220_Humid_Init(tControl * Ctrl) {
  const tParamData * Param = ParameterGet();
  tBrushValve * BrushValve = & Ctrl -> ABC[EQUIP_B].BrushValve;
  Brush_UpDown_Init( BrushValve, I_UpDown[0], I_UpDown[1], I_UpDown[2], I_UpDown[3], I_UpDown[4]);
  Equip_Down_OMFB_Init( BrushValve, I_UpDown[2], I_UpDown[3], -1, -1, I_UpDown[4]);
  Brush_Float_Init(BrushValve, I_Float[0], I_Float[1]);
  Brush_Lock_Init( BrushValve, I_Lock[0], I_Lock[1]);
  Brush_Rotate_Init(BrushValve, I_Rotate[0], I_Rotate[1], I_Rotate[4]);
  Brush_A_B(BrushValve, PVE_PORT_A);
  BrushValve -> MaxSpeed = 30;
  BrushValve->isFloat = 1;
  BrushValve->VRotatePWM = Param->OMFB.PWM_Rotate_OMP;

  // Отобращение иконки увлажнения OMP
  SendToVisuObj( OBJ_SIMPLEBITMAP362, GS_TO_VISU_SET_ATTR_VISIBLE, 1);
  SendToVisuObj( OBJ_INDEXBITMAP1324, GS_TO_VISU_SET_ATTR_VISIBLE, 1);
}
void Ctrl_Inter_OZP231_Init(tControl * Ctrl) {
  const tParamData * Param = ParameterGet();
  tBrushValve * BrushValve = & Ctrl -> ABC[EQUIP_B].BrushValve;
  Brush_UpDown_Init( BrushValve, I_UpDown[0], I_UpDown[1], I_UpDown[2], I_UpDown[3], I_UpDown[4]);
  Equip_Down_OMFB_Init( BrushValve, I_UpDown[2], I_UpDown[3], -1, -1, I_UpDown[4]);
  Brush_Float_Init(BrushValve, I_Float[0], I_Float[1]);
  Brush_Lock_Init( BrushValve, I_Lock[0], I_Lock[1]);
  Brush_Rotate_Init(BrushValve, I_Rotate[0], I_Rotate[1], I_Rotate[4]);
  Brush_A_B(BrushValve, PVE_PORT_A);
  BrushValve -> MaxSpeed = 30;
  BrushValve->isFloat = 1;
  BrushValve->VRotatePWM = Param->OMFB.PWM_Rotate_OZP231;
}

// Грейдерные отвалы
void Ctrl_Inter_CH2600_Init(tControl * Ctrl) {
  tABC_Attach * ABC_Attach = & Ctrl -> ABC[EQUIP_B].ABC_Attach;
  Plough_UpDown_Init( & Ctrl -> ABC[EQUIP_B].ABC_Attach, I_UpDown[0], I_UpDown[1], I_UpDown[2], I_UpDown[3], I_UpDown[4]);
  OMFB_UpDownSecond_Init( & Ctrl -> ABC[EQUIP_B].ABC_Attach, I_UpDown[2], I_UpDown[3], -1, -1, I_UpDown[4]);
  Plough_Lock_Init( & Ctrl -> ABC[EQUIP_B].ABC_Attach, I_Lock[0], I_Lock[1]);
  ABC_Attach -> MaxSpeed = 50;
}
void Ctrl_Inter_CH2800_Init(tControl * Ctrl) {
  tABC_Attach * ABC_Attach = & Ctrl -> ABC[EQUIP_B].ABC_Attach;
  Plough_UpDown_Init( & Ctrl -> ABC[EQUIP_B].ABC_Attach, I_UpDown[0], I_UpDown[1], I_UpDown[2], I_UpDown[3], I_UpDown[4]);
  OMFB_UpDownSecond_Init( & Ctrl -> ABC[EQUIP_B].ABC_Attach, I_UpDown[2], I_UpDown[3], -1, -1, I_UpDown[4]);
  Plough_Float_Init( & Ctrl -> ABC[EQUIP_B].ABC_Attach, I_Float[0], I_Float[1]);
  Plough_Lock_Init( & Ctrl -> ABC[EQUIP_B].ABC_Attach, I_Lock[0], I_Lock[1]);
  ABC_Attach -> MaxSpeed = 30;
}

void Ctrl_Front_MSPN_B(tControl * Ctrl) {
  Plough_LeftRight_Init( & Ctrl -> ABC[EQUIP_B].ABC_Attach, I_LeftRight[0], I_LeftRight[1], I_LeftRight[2], I_LeftRight[3], I_LeftRight[4]);
  Plough_ExRetract_RightSection( & Ctrl -> ABC[EQUIP_B].ABC_Attach, I_Lock[2], I_Lock[3]);
}

void Ctrl_Inter_Draw(tControl * Ctrl) {
  if (Brush_Active( & Ctrl -> ABC[EQUIP_B].BrushValve)) {
    static int32_t _time = 0;
    if (250 < GetMSTick() - _time) {
      _time = GetMSTick();
      int32_t pos = GetVarIndexed(IDX_BRUSHON_INTER) + 1;
      if (pos > 3) {
        pos = 1;
      }
      SetVarIndexed(IDX_BRUSHON_INTER, pos);
    }
  } else {
    SetVarIndexed(IDX_BRUSHON_INTER, 0);
  }

  if (Ctrl -> cmd_ABC[EQUIP_B].Float == FLOAT_ON) {
    SetVarIndexed(IDX_FLOATING_INTERAX, 1);
  } else {
    SetVarIndexed(IDX_FLOATING_INTERAX, 0);
  }
}

void Ctrl_Inter_Brush_Standard_Cycle(tControl * Ctrl, uint32_t evtc, tUserCEvt * evtv) {
  Brush_Cycle( & Ctrl -> ABC[EQUIP_B].BrushValve, & Ctrl -> cmd_ABC[EQUIP_B], EQUIP_B);
  Ctrl_Inter_Draw(Ctrl);
}

void Ctrl_Inter_Brush_Timer_10ms(tControl * Ctrl) {
  Brush_Timer_10ms( & Ctrl -> ABC[EQUIP_B].BrushValve);
}

void Ctrl_Inter_Plough_Standard_Cycle(tControl * Ctrl, uint32_t evtc, tUserCEvt * evtv) {
  Plough_Cycle( & Ctrl -> ABC[EQUIP_B].ABC_Attach, & Ctrl -> cmd_ABC[EQUIP_B], EQUIP_B);
}

void Ctrl_Inter_CH2800_Standard_Cycle(tControl * Ctrl, uint32_t evtc, tUserCEvt * evtv) {
  Ctrl_Inter_Plough_CH2800_Cycle( & Ctrl -> ABC[EQUIP_B].ABC_Attach, & Ctrl -> cmd_ABC[EQUIP_B]);
}

void Ctrl_Inter_Plough_Timer_10ms(tControl * Ctrl) {
  static int32_t count = 0;
  count++;
  if (0 == (count % 10)) {
    Valv_Timer_100ms( & Ctrl -> ABC[EQUIP_B].ABC_Attach.ValveUpDown);
  }
}
