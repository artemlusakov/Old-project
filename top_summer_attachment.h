#ifndef TOP_SUMMER_ATTACHMENT_H
#define TOP_SUMMER_ATTACHMENT_H
#include "control.h"

void Ctrl_top_E2000_BP300_Init( tControl* Ctrl );
void Ctrl_top_E2000_BP300_Cycle( tControl* Ctrl, uint32_t evtc, tUserCEvt* evtv );

void Ctrl_top_EKM10_Init( tControl* Ctrl );
void Ctrl_top_EKM10_Cycle( tControl* Ctrl, uint32_t evtc, tUserCEvt* evtv );

void Ctrl_top_E2000_C610H_Init( tControl* Ctrl );
void Ctrl_top_E2000_C610H_Cycle( tControl* Ctrl, uint32_t evtc, tUserCEvt* evtv );

void Ctrl_top_E2000_C610H_C610H_Init( tControl* Ctrl );
void Ctrl_top_E2000_C610H_C610H_Cycle( tControl* Ctrl, uint32_t evtc, tUserCEvt* evtv );

void Ctrl_top_E2000_BP300_C610H_Init( tControl* Ctrl );
void Ctrl_top_E2000_BP300_C610H_Cycle( tControl* Ctrl, uint32_t evtc, tUserCEvt* evtv );

void Ctrl_top_E2000_C610H_RR300_Init( tControl* Ctrl );
void Ctrl_top_E2000_C610H_RR300_Cycle( tControl* Ctrl, uint32_t evtc, tUserCEvt* evtv );

void Ctrl_top_E2000_C610H_RR400_Init( tControl* Ctrl );
void Ctrl_top_E2000_C610H_RR400_Cycle( tControl* Ctrl, uint32_t evtc, tUserCEvt* evtv );

void Ctrl_top_E2000_C610H_BP300_RR300_Init( tControl* Ctrl );
void Ctrl_top_E2000_C610H_BP300_RR300_Cycle( tControl* Ctrl, uint32_t evtc, tUserCEvt* evtv );

void Ctrl_top_E2000_C610H_BP300_RR400_Init( tControl* Ctrl );
void Ctrl_top_E2000_C610H_BP300_RR400_Cycle( tControl* Ctrl, uint32_t evtc, tUserCEvt* evtv );

#endif  // #ifndef TOP_SUMMER_ATTACHMENT_H
