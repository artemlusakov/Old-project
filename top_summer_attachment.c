#include <stdint.h>
#include <UserCEvents.h>
#include <UserCAPI.h>
#include "vartab.h"
#include "objtab.h"
#include "RCText.h"
#include "control.h"
#include "gsToVisu.h"
#include "RCColor.h"
#include "pinout.h"
#include "io.h"
#include "ctrl_top_summer_attachment.h"


void DefaultCyclePump(tControl * Ctrl) {
  WaterPump_Cycle( & Ctrl->top.SummerTop );
  WaterPump_Draw( & Ctrl -> top.SummerTop );
  Light_Draw( & Ctrl -> light);
}


// E2000_BP300
void Ctrl_top_E2000_BP300_Init(tControl * Ctrl) {
  WaterPump_Init( & Ctrl -> top.SummerTop, WATERPUMP_TYPE_BP300);
}
void Ctrl_top_E2000_BP300_Cycle(tControl * Ctrl, uint32_t evtc, tUserCEvt * evtv) {
  DefaultCyclePump(Ctrl);
}


// E2000_C610H
void Ctrl_top_E2000_C610H_Init(tControl * Ctrl) {
  WaterPump_Init( & Ctrl -> top.SummerTop, WATERPUMP_TYPE_C610H);
  WaterPump_SetMaxLiter(65000);
}
void Ctrl_top_E2000_C610H_Cycle(tControl * Ctrl, uint32_t evtc, tUserCEvt * evtv) {
  DefaultCyclePump(Ctrl);
}


// E2000_BP300_C610H
void Ctrl_top_E2000_BP300_C610H_Init(tControl * Ctrl) {
  Ctrl_top_E2000_BP300_Init(Ctrl);
  WaterPump_Intake_Init( & Ctrl -> top.SummerTop, -1, -1, WATERPUMP_TYPE_C610H);
}
void Ctrl_top_E2000_BP300_C610H_Cycle(tControl * Ctrl, uint32_t evtc, tUserCEvt * evtv) {
  DefaultCyclePump(Ctrl);
}


// E2000_C610H_C610H
void Ctrl_top_E2000_C610H_C610H_Init(tControl * Ctrl) {
  Ctrl_top_E2000_C610H_Init(Ctrl);
  WaterPump_Intake_Init( & Ctrl -> top.SummerTop, -1, -1, WATERPUMP_TYPE_C610H);
}
void Ctrl_top_E2000_C610H_C610H_Cycle(tControl * Ctrl, uint32_t evtc, tUserCEvt * evtv) {
  DefaultCyclePump(Ctrl);
}


// EKM10
void Ctrl_top_EKM10_Init(tControl * Ctrl) {
  Ctrl_top_E2000_C610H_Init(Ctrl);
  WaterPump_Intake_Init( & Ctrl -> top.SummerTop, -1, -1, WATERPUMP_TYPE_NCR);
}
void Ctrl_top_EKM10_Cycle(tControl * Ctrl, uint32_t evtc, tUserCEvt * evtv) {
  DefaultCyclePump(Ctrl);
}


// E2000_C610H_RR300
void Ctrl_top_E2000_C610H_RR300_Init(tControl * Ctrl) {
  RR300_Init( & Ctrl -> top.SummerTop);
  Ctrl_top_E2000_C610H_Init(Ctrl);
}
void Ctrl_top_E2000_C610H_RR300_Cycle(tControl * Ctrl, uint32_t evtc, tUserCEvt * evtv) {
  DefaultCyclePump(Ctrl);

	RR300_Cycle( & Ctrl -> top.SummerTop);
  RR300_RR400_Draw( &Ctrl->top.SummerTop );
}


// E2000_C610H_BP300_RR300
void Ctrl_top_E2000_C610H_BP300_RR300_Init(tControl * Ctrl) {
  RR300_Init( & Ctrl -> top.SummerTop);
  Ctrl_top_E2000_C610H_Init(Ctrl);
}
void Ctrl_top_E2000_C610H_BP300_RR300_Cycle(tControl * Ctrl, uint32_t evtc, tUserCEvt * evtv) {
  DefaultCyclePump(Ctrl);

  RR300_Cycle( & Ctrl -> top.SummerTop);
  RR300_RR400_Draw( &Ctrl->top.SummerTop );
}


// E2000_C610H_RR400
void Ctrl_top_E2000_C610H_RR400_Init(tControl * Ctrl) {
  RR400_Init( & Ctrl -> top.SummerTop);
  Ctrl_top_E2000_C610H_Init(Ctrl);
}
void Ctrl_top_E2000_C610H_RR400_Cycle(tControl * Ctrl, uint32_t evtc, tUserCEvt * evtv) {
  DefaultCyclePump(Ctrl);

	RR400_Cycle( & Ctrl -> top.SummerTop);
  RR300_RR400_Draw( &Ctrl->top.SummerTop );
}


// E2000_C610H_BP300_RR400
void Ctrl_top_E2000_C610H_BP300_RR400_Init(tControl * Ctrl) {
  RR400_Init( & Ctrl -> top.SummerTop);
  Ctrl_top_E2000_C610H_Init(Ctrl);
}
void Ctrl_top_E2000_C610H_BP300_RR400_Cycle(tControl * Ctrl, uint32_t evtc, tUserCEvt * evtv) {
  DefaultCyclePump(Ctrl);

  RR400_Cycle( & Ctrl -> top.SummerTop);
  RR300_RR400_Draw( &Ctrl->top.SummerTop );
}
