
#include <stdint.h>
#include <UserCEvents.h>
#include <UserCAPI.h>
#include "vartab.h"
#include "objtab.h"
#include "visu_material.h"
#include "j1939.h"
#include "control.h"
#include "ctrl_top_adrive.h"
#include "bnso.h"
#include "param.h"
#include "ctrl_brush.h"
#include "profile.h"
#include "RCText.h"
#include "ctrl_top_summer_attachment.h"
#include "rtr_uh.h"

extern uint8_t moveAutoPark;
extern uint8_t flagAutoPark;

int32_t BNSO_ValvChangeToJ1939(const tValv * Valv, int8_t * value) {
  if (NULL == value) {
    return 0;
  }
  switch (Valv -> Type) {
  case VALVE_TYPE_PVE_O:
    if (Valv -> Valve.PVEO[0].val == 1) {
      * value = 1;
      return 1;
    } else if (Valv -> Valve.PVEO[1].val == 1) {
      * value = 0x2;
      return 1;
    } else {
      * value = 0x00;
      return 1;
    }
    break;

  case VALVE_TYPE_DISK:
    if (Valv -> Valve.PVEO[0].val == 1) {
      * value = 1;
      return 0x01;
    } else {
      * value = 0;
      return 0x01;
    }

    break;

  case VALVE_TYPE_PVE_AHS:
    if (Valv -> Valve.PVEH.duty > 550) {
      * value = 0x01;
      return 1;
    } else if (Valv -> Valve.PVEH.duty < 450) {
      * value = 0x02;
      return 1;
    } else {
      * value = 0x00;
      return 1;
    }

    break;

  case VALVE_TYPE_PVE_AHS32:
    if (Valv -> Valve.PVEH.duty > 550) {
      * value = 0x01;
      return 1;
    } else if (Valv -> Valve.PVEH.duty < 450) {
      * value = 0x02;
      return 1;
    } else {
      * value = 0x00;
      return 1;
    }

    break;
  default:

    break;
  }
  return 0;
}

int FrontEquip_NoFloat[] = {
	RCTEXT_LI_A_ATTACHMENT_ATTACHMENT_OMB1,
	RCTEXT_LI_A_ATTACHMENT_ATTACHMENT_OMT1,
	RCTEXT_LI_A_ATTACHMENT_ATTACHMENT_MF300,
	RCTEXT_LI_A_ATTACHMENT_ATTACHMENT_MF500,
};


void BNSO_SetData(void) {
  tPGN_FF01 * FF01 = J1939_GetPGN_FF01();
  tPGN_FF02 * FF02 = J1939_GetPGN_FF02();
  tPGN_FF07 * FF07 = J1939_GetPGN_FF07();

  const tControl * Ctrl = CtrlGet();
  const tProfile * Prof = Profile_GetCurrentProfile();

  static int32_t count = 0;
  if (500 < GetMSTick() - count) {
    count = GetMSTick();
    int8_t v;

    // Front equip
    {
      if (Prof -> module[1]) {
        //Floating
        if (Ctrl -> cmd_ABC[EQUIP_A].Float) {
          FF01 -> FrontAttach_FloatingMode = 1 & 0x03;
        } else {
          FF01 -> FrontAttach_FloatingMode = 0 & 0x03;
        }

        for (uint8_t i = 0; i < GS_ARRAYELEMENTS(FrontEquip_NoFloat); i++) {
          if (Prof -> module[1] == FrontEquip_NoFloat[i]) {
            FF01 -> FrontAttach_FloatingMode = 0 & 0x03;
          }
        }

        //UpDown
        if (Ctrl -> cmd_ABC[EQUIP_A].Updown < 0) { // вниз
          FF01 -> FrontAttach_VerticalPos = 0x01 & 0x03;
        } else if (Ctrl -> cmd_ABC[EQUIP_A].Updown > 0) { // вверх
          FF01 -> FrontAttach_VerticalPos = 0x02 & 0x03;
        } else {
          FF01 -> FrontAttach_VerticalPos = 0x00 & 0x03;
        }

        //LeftRight
        if (1 == BNSO_ValvChangeToJ1939( & Ctrl -> ABC[EQUIP_A].ABC_Attach.ValveLeftRight, & v)) {
          FF01 -> FrontAttach_HorizontalPos = v & 0x03;
        }
        if (1 == BNSO_ValvChangeToJ1939( & Ctrl -> ABC[EQUIP_A].BrushValve.VLeftRight, & v)) {
          FF01 -> FrontAttach_HorizontalPos = v & 0x03;
        }


        //Rotation
        if (Brush_Active( & Ctrl -> ABC[EQUIP_A].BrushValve)) {
          FF01 -> FrontAttach_Rotation = 0x01 & 0x03;
        } else {
          FF01 -> FrontAttach_Rotation = 0x00 & 0x03;
        }

        //Bladetype
        if (1 == BNSO_ValvChangeToJ1939( & Ctrl -> ABC[EQUIP_A].ABC_Attach.ValveSteelUpDown, & v))
          FF01 -> FrontAttach_BladeType = v & 0x03;

        if (1 == BNSO_ValvChangeToJ1939( & Ctrl -> ABC[EQUIP_A].BrushValve.VOptional, & v))
          FF01 -> FrontAttach_BladeType = v & 0x03;

        if (1 == BNSO_ValvChangeToJ1939( & Ctrl -> ABC[EQUIP_A].BrushValve.VSideSec, & v))
          FF01 -> FrontAttach_BladeType = v & 0x03;

      }
    }

    //Interax equip
    {
      if (Prof -> module[2]) {
        //Floating
        if (Ctrl -> cmd_ABC[EQUIP_B].Float) {
          FF01 -> Interaxle_FloatingMode = 1 & 0x03;
        } else {
          FF01 -> Interaxle_FloatingMode = 0 & 0x03;
        }

        //UpDown
        if (Ctrl -> cmd_ABC[EQUIP_B].Updown < 0) { // вниз
          FF01 -> Interaxle_VerticalPos = 0x01 & 0x03;
        } else if (Ctrl -> cmd_ABC[EQUIP_B].Updown > 0) { // вверх
          FF01 -> Interaxle_VerticalPos = 0x02 & 0x03;
        } else {
          FF01 -> Interaxle_VerticalPos = 0x00 & 0x03;
        }
        if (IsBrushAutoUp()) { // Автоподъем щетки
          FF01 -> Interaxle_VerticalPos = 0x02 & 0x03;
        }

        //LeftRight
        if (1 == BNSO_ValvChangeToJ1939( & Ctrl -> ABC[EQUIP_B].ABC_Attach.ValveLeftRight, & v))
          FF01 -> Interaxle_HorizontalPos = v & 0x03;
        if (1 == BNSO_ValvChangeToJ1939( & Ctrl -> ABC[EQUIP_B].BrushValve.VLeftRight, & v))
          FF01 -> Interaxle_HorizontalPos = v & 0x03;

        //Rotation
        if (Brush_Active( & Ctrl -> ABC[EQUIP_B].BrushValve)) {
          FF01 -> Interaxle_Rotation = 0x02 & 0x03;
        } else {
          FF01 -> Interaxle_Rotation = 0x00 & 0x03;
        }

        //Bladetype
        if (1 == BNSO_ValvChangeToJ1939( & Ctrl -> ABC[EQUIP_B].ABC_Attach.ValveSteelUpDown, & v))
          FF01 -> Interaxle_BladeType = v & 0x03;
        if (1 == BNSO_ValvChangeToJ1939( & Ctrl -> ABC[EQUIP_B].BrushValve.VOptional, & v))
          FF01 -> Interaxle_BladeType = v & 0x03;
        if (1 == BNSO_ValvChangeToJ1939( & Ctrl -> ABC[EQUIP_B].BrushValve.VSideSec, & v))
          FF01 -> Interaxle_BladeType = v & 0x03;
      }
    }

    //Side equip
    {
      if (Prof -> module[3]) {
        //Floating
        if (Ctrl -> cmd_ABC[EQUIP_C].Float) {
          FF01 -> SideAttach_FloatingMode = 1 & 0x03;
        } else {
          FF01 -> SideAttach_FloatingMode = 0 & 0x03;
        }

        //UpDown
        if (Ctrl -> cmd_ABC[EQUIP_C].Updown < 0) { // вниз
          FF01 -> SideAttach_VerticalPos = 0x01 & 0x03;
        } else if (Ctrl -> cmd_ABC[EQUIP_C].Updown > 0) { // вверх
          FF01 -> SideAttach_VerticalPos = 0x02 & 0x03;
        } else {
          FF01 -> SideAttach_VerticalPos = 0x00 & 0x03;
        }
        if (flagAutoPark == 1) {
          if (moveAutoPark == 0) {
            FF01 -> SideAttach_VerticalPos = 0x02 & 0x03;
          }
          if (moveAutoPark == 1) {
            FF01 -> SideAttach_HorizontalPos = 0x01 & 0x03;
          }
        }

        //Tilt
        if (1 == BNSO_ValvChangeToJ1939( & Ctrl -> ABC[EQUIP_C].ABC_Attach.ValveTilt, & v))
          FF01 -> SideAttach_Tilt = v & 0x03;

        //LeftRight
        if (1 == BNSO_ValvChangeToJ1939( & Ctrl -> ABC[EQUIP_C].ABC_Attach.ValveLeftRight, & v))
          FF01 -> SideAttach_HorizontalPos = v & 0x03;
        else if (1 == BNSO_ValvChangeToJ1939( & Ctrl -> ABC[EQUIP_C].ABC_Attach.ValveWorkTransport, & v))
          FF01 -> SideAttach_HorizontalPos = v & 0x03;
      }
    }

    FF01 -> SprayWidth = Ctrl -> top.WinterTop.SpreadWidth;
    FF01 -> SprayDensity = GetVarIndexed(IDX_DENSITY) / 4;
    FF01 -> PositionAsymmetryDrive = Ctrl -> top.WinterTop.Adrive.idx_pos_act;
    FF01 -> ReagentFeedRate = Ctrl -> sens.Freq.ConveryRotation; // 0 to 8000 rpm
    FF01 -> Dummy = 0x3; // 3
    FF01 -> SpeedSimulation = Ctrl -> automatic.on & 0x03;


    FF02 -> HydSysTemp = (IOT_NormAI( & Ctrl -> sens.AI.OilTemp) + 5) / 10 * 10; // test for msg
    FF02 -> HydSysPres = IOT_NormAI( & Ctrl -> sens.AI.OilPressure) * 0.5;
    //hydraulic Oil level
    uint8_t Oillevel = 0;
    if (0 == Ctrl -> sens.DI.CheckOilLevelSensor) {
      Oillevel = 0xFE; //Fault
    } else if ((1 == Ctrl -> sens.DI.UpperOilLevel) && (0 == Ctrl -> sens.DI.LowerOilLevel)) {
      Oillevel = 100.0 * 2.5;
    } else if ((0 == Ctrl -> sens.DI.UpperOilLevel) && (0 == Ctrl -> sens.DI.LowerOilLevel)) {
      Oillevel = 50.0 * 2.5;
    } else
      Oillevel = 0;

    FF02 -> HydSysOilLev = Oillevel;

    if (((int8_t) FF02 -> HydSysTemp) < -50) // Out of range
    {
      FF02 -> OilTemp = 0;
    } else if (((int8_t) FF02 -> HydSysTemp) < -20) // low temperature
    {
      FF02 -> OilTemp = 1;
    } else if (FF02 -> HydSysTemp < 30) // operating temperature range
    {
      FF02 -> OilTemp = 2;
    } else {
      FF02 -> OilTemp = 3;
    }

    //* Отправка на CAN сообщения о работе ПС или активного насоса
    FF01 -> TopEquip = 0; // по-умолчанию выключено всё
    //FF02->Reagent = 0;
    if (Ctrl -> top.WinterTop.SpreadConv.on) {
      FF01 -> TopEquip = 1; // RTR
      //FF02->Reagent = Reagent_GetIdx() + 1;
    } else if (WaterPump_IsOn( & Ctrl -> top.SummerTop.Pump) == 1) {
      if (Ctrl -> cmd_no_key.E_S == 1) {
        if (Ctrl -> top.SummerTop.Pump.Type == WATERPUMP_TYPE_BP300) {
          FF01 -> TopEquip = 2; //BP300
        } else if (Ctrl -> top.SummerTop.Pump.Type == WATERPUMP_TYPE_C610H) {
          FF01 -> TopEquip = 3; //C610H
        }
      }
    } else if (WaterIntake_IsOn( & Ctrl -> top.SummerTop.PumpIntake) == 1) { //= Отправка на CAN сообщения о работе насоса при заборе воды
      if (Ctrl -> top.SummerTop.PumpIntake.TypeIntake == WATERPUMP_TYPE_C610H) {
        FF01 -> TopEquip = 3; //C610H
      } else if (Ctrl -> top.SummerTop.PumpIntake.TypeIntake == WATERPUMP_TYPE_NCR) {
        FF01 -> TopEquip = 5; //NCR
      }
    }

    switch (uh.stt.tail_pos) {
    case OFF:
      FF02 -> sens_tail = 0;
      FF02 -> sw_tail = 1;
      break;
    case ON:
      FF02 -> sens_tail = 1;
      FF02 -> sw_tail = 1;
      break;
    case UNK:
      FF02 -> sens_tail = 0;
      FF02 -> sw_tail = 0;
      break;
    }
    switch (uh.stt.humid_sens) {
    case OFF:
      FF02 -> sens_liq_humid = 0;
      FF02 -> sw_liq_humid = 1;
      break;
    case ON:
      FF02 -> sens_liq_humid = 1;
      FF02 -> sw_liq_humid = 1;
      break;
    case UNK:
      FF02 -> sens_liq_humid = 0;
      FF02 -> sw_liq_humid = 0;
      break;
    }
    switch (uh.stt.spread_sens) {
    case OFF:
      FF02 -> sens_spread = 0;
      FF02 -> sw_spread = 1;
      break;
    case ON:
      FF02 -> sens_spread = 1;
      FF02 -> sw_spread = 1;
      break;
    case UNK:
      FF02 -> sens_spread = 0;
      FF02 -> sw_spread = 0;
      break;
    }
    FF02 -> rate_conv = uh.rd.conv_rot;
    switch (uh.stt.conv) {
    case OFF:
      FF02 -> sw_rot_conv = 1;
      break;
    case ON:
      FF02 -> sw_rot_conv = 1;
      break;
    case UNK:
      FF02 -> sw_rot_conv = 0;
      break;
    }
    FF02 -> rate_humid = uh.rd.humid_rot;
    switch (uh.stt.humid_pump) {
    case OFF:
      FF02 -> sw_rot_humid = 1;
      break;
    case ON:
      FF02 -> sw_rot_humid = 1;
      break;
    case UNK:
      FF02 -> sw_rot_humid = 0;
      break;
    }
    FF02 -> prcnt_humid = uh.humid_prcnt;
    FF02 -> typeReagent = GetVarIndexed(IDX_REAGENT);
    const tReagent * rgnt_ = GetActualReagent();

    if (rgnt_) {
      FF02 -> mat_dens = (rgnt_ -> MaterialDensity - 1000) / 100;
    } else {
      FF02 -> mat_dens = 0;
    }

    if (Ctrl -> top.WinterTop.SpreadConv.initShield) {
      FF07 -> ShieldPos = GetVarIndexed(IDX_SHIELDPOSITION);
    } else {
      FF07 -> ShieldPos = 0xF;
    }
  }
}

void BNSO_SendErrMsg(uint16_t ErrMsgId, uint8_t state) {
  uint16_t data = (ErrMsgId << 4) + (state & 0xF);
  J1939_SendPGN(J1939_ERR_MSG_ID, & data, sizeof(uint16_t));
}
