/****************************************************************************
*
* File:         COMMANDS.c
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
#include "commands.h"
#include "control.h"
#include "profile.h"
#include "RCText.h"

/****************************************************************************/

/* macro definitions ********************************************************/

/* type definitions *********************************************************/

/* prototypes ***************************************************************/

/* global constants *********************************************************/

/* global variables *********************************************************/

/* local function prototypes*************************************************/

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
void Ctrl_JS_Cmd_SetCommands(tJSCommands * JS_Cmd, int32_t ActiveButtonPressed, uint8_t Equip) {
  const tControl * Ctrl = CtrlGet();
  const tProfile * Prof = Profile_GetCurrentProfile();
  if (ActiveButtonPressed) {
    JS_Cmd -> Active = 1;
    JS_Cmd -> Updown = -10 * IOT_AnaJoy_GetInPercent( & Ctrl -> joystick.UpDown); //Inverted Y-Achses
    JS_Cmd -> LeftRight = 10 * IOT_AnaJoy_GetInPercent( & Ctrl -> joystick.LeftRight);
    JS_Cmd -> X = 10 * IOT_AnaJoy_GetInPercent( & Ctrl -> joystick.X);
    JS_Cmd -> Y = 10 * IOT_AnaJoy_GetInPercent( & Ctrl -> joystick.Y);
    JS_Cmd -> Z = 10 * IOT_AnaJoy_GetInPercent( & Ctrl -> joystick.Z);
    JS_Cmd -> X_Change = IOT_AnaJoy_PressedNew( & Ctrl -> joystick.X);
    JS_Cmd -> Y_Change = IOT_AnaJoy_PressedNew( & Ctrl -> joystick.Y);
    JS_Cmd -> Z_Change = IOT_AnaJoy_PressedNew( & Ctrl -> joystick.Z);
    //		db_out( "<CTRL> Z(%d)%d\r\n", Ctrl->_joystick.Z.d, JS_Cmd->Z_Change );

    if (Prof -> module[Equip]) {
      if (IOT_Button_IsPressedNew( & Ctrl -> joystick.Button[JSB_F])) {
        if (JS_Cmd -> Float)
          JS_Cmd -> Float = FLOAT_OFF;
        else
          JS_Cmd -> Float = FLOAT_ON;
        JS_Cmd -> F_New = 1;
      } else {
        JS_Cmd -> F_New = 0;
      }
    } else {
      JS_Cmd -> F_New = 0;
      JS_Cmd -> Float = FLOAT_OFF;
    }

    if (Prof -> module[Equip] == RCTEXT_LI_C_ATTACHMENT_ATTACHMENT_AA_AC) {
      if (IOT_Button_IsPressedNew( & Ctrl -> joystick.Button[JSB_G])) {
        if (JS_Cmd -> FloatTilt)
          JS_Cmd -> FloatTilt = FLOAT_OFF;
        else
          JS_Cmd -> FloatTilt = FLOAT_ON;
        JS_Cmd -> G_New = 1;
      } else {
        JS_Cmd -> G_New = 0;
      }
    } else {
      JS_Cmd -> G_New = 0;
      JS_Cmd -> FloatTilt = FLOAT_OFF;
    }

    if (JS_Cmd -> Updown > 0) { //Push up->Deactivate Float and tABC_Attach
      JS_Cmd -> Float = FLOAT_OFF; //Float off
    } else if (JS_Cmd -> Updown < 0) { //Push down->break for Float
      if (JS_Cmd -> Float == FLOAT_ON)
        JS_Cmd -> Float = FLOAT_BREAK;
    } else {
      if (FLOAT_BREAK == JS_Cmd -> Float) {
        JS_Cmd -> Float = FLOAT_ON;
      }
    }

    if(JS_Cmd->Z) {
      JS_Cmd -> FloatTilt = FLOAT_OFF;
    }
    //Boost Button
    if (IOT_Button_IsDown( & Ctrl -> joystick.Button[JSB_D])) {
      JS_Cmd -> Boost = 1;
    } else
      JS_Cmd -> Boost = 0;
    if (IOT_Button_IsReleasedShortNew( & Ctrl -> joystick.Button[JSB_D])) {
      JS_Cmd -> D_S = 1 - JS_Cmd -> D_S;
    }
    if (IOT_Button_IsPressedLongNew( & Ctrl -> joystick.Button[JSB_D])) {
      JS_Cmd -> D_L = 1 - JS_Cmd -> D_L;
    }
    //Power On/Off
    if (IOT_Button_IsReleasedShortNew( & Ctrl -> joystick.Button[JSB_E])) {
      if (JS_Cmd -> E_L == 1) {
        JS_Cmd -> E_L = 0;
      } else {
        JS_Cmd -> E_S = 1 - JS_Cmd -> E_S; /* code */
      }
    }
    if (IOT_Button_IsPressedLongNew( & Ctrl -> joystick.Button[JSB_E])) {
      JS_Cmd -> E_L = 1 - JS_Cmd -> E_L;
    }
    JS_Cmd -> E_Time = IOT_Button_IsDown( & Ctrl -> joystick.Button[JSB_E]);
  } else { //Key is not pressed anymore.
    JS_Cmd -> Updown = 0;
    JS_Cmd -> LeftRight = 0;
    JS_Cmd -> X = 0;
    JS_Cmd -> Y = 0;
    JS_Cmd -> Z = 0;

    JS_Cmd -> X_Change = 0;
    JS_Cmd -> Y_Change = 0;
    JS_Cmd -> Z_Change = 0;
    JS_Cmd -> Active = 0;

    JS_Cmd -> E_Time = 0;

    if (FLOAT_BREAK == JS_Cmd -> Float) {
      JS_Cmd -> Float = FLOAT_ON;
    }
  }
}

void Ctrl_JS_Buzzer( void ) {
  const tControl * Ctrl = CtrlGet();
  int statebuzz[] = {
    HDL_JOY_BUZZ_A,
    HDL_JOY_BUZZ_B,
    HDL_JOY_BUZZ_C,
    HDL_JOY_BUZZ_D,
    HDL_JOY_BUZZ_E,
    HDL_JOY_BUZZ_F,
    HDL_JOY_BUZZ_G,
  };
  int ctrl_js_buzzer = 0;
  static int buzzer_old = 0;
  for (int Button = JSB_A; Button < JSB_NUM_BUTTONS; Button++) {
    if (IOT_Button_IsDown( & Ctrl -> joystick.Button[Button]) && GetVar(statebuzz[Button])) {
      ctrl_js_buzzer = ctrl_js_buzzer + 1;
    }
  }
  if (IOT_AnaJoy_PressedNew( & Ctrl -> joystick.X) && GetVar(HDL_JOY_BUZZ_X)) {
    ctrl_js_buzzer = ctrl_js_buzzer + 1;
  }
  if (IOT_AnaJoy_PressedNew( & Ctrl -> joystick.Y) && GetVar(HDL_JOY_BUZZ_Y)) {
    ctrl_js_buzzer = ctrl_js_buzzer + 1;
  }
  if (IOT_AnaJoy_PressedNew( & Ctrl -> joystick.Z) && GetVar(HDL_JOY_BUZZ_Z)) {
    ctrl_js_buzzer = ctrl_js_buzzer + 1;
  }
  if (ctrl_js_buzzer != buzzer_old && ctrl_js_buzzer >= buzzer_old && ctrl_js_buzzer != 0) {
    SetBuzzer(1, 10, 10, 1);
  }
  buzzer_old = ctrl_js_buzzer;
}