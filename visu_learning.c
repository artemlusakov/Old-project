/****************************************************************************
*
* File:         VISU_LEARNING.c
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
#include <UserCEvents.h>  /* Events send to the Cycle function              */
#include <UserCAPI.h>     /* API-Function declarations                      */

#include "vartab.h"       /* Variable Table definition                      */
 /* include this file in every C-Source to access  */
 /* the variable table of your project             */
 /* vartab.h is generated automatically            */
#include "objtab.h"       /* Object ID definition                           */
						  /* include this file in every C-Source to access  */
						  /* the object ID's of the visual objects          */
						  /* objtab.h is generated automatically            */
#include "visu_learning.h"
#include "visu_main.h"
#include "control.h"
#include "profile.h"
#include "RCText.h"
#include "gsToVisu.h"
#include "ctrl_top_summer_attachment.h"
/****************************************************************************/

/* macro definitions ********************************************************/

/* type definitions *********************************************************/

/* prototypes ***************************************************************/

/* global constants *********************************************************/

/* global variables *********************************************************/
uint8_t loadingPanel = 0;
uint8_t topPanel = 0;
uint8_t startPanel = 0;
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

void Visu_Learning_Init(const tVisuData * VData) {}

void Visu_Learning_Open(const tVisuData * VData) {
  Visu_Main_Open(VData);
  InfoContainerOn(CNT_LEARNING);
}

void Visu_Learning_Close(const tVisuData * VData) {
  InfoContainerOff(CNT_LEARNING);

  MsgContainerOff(CNT_LEARNINGSTARTPANEL);
  startPanel = 0;
  MsgContainerOff(CNT_LEARNINGTOPPANEL);
  topPanel = 0;
  SendToVisuObj(OBJ_LOADINGTEXT, GS_TO_VISU_SET_ATTR_VISIBLE, 0);
  SendToVisuObj(OBJ_LOADINGBOX, GS_TO_VISU_SET_ATTR_VISIBLE, 0);
}

void Visu_L_DrawBackground(const tControl * Ctrl) {
  if (Ctrl -> cmd_ABC[EQUIP_A].Active) {
    SetVarIndexed(IDX_LEARNING_BACK, 2);
  } else if (Ctrl -> cmd_ABC[EQUIP_B].Active) {
    SetVarIndexed(IDX_LEARNING_BACK, 1);
  } else if (Ctrl -> cmd_ABC[EQUIP_C].Active) {
    SetVarIndexed(IDX_LEARNING_BACK, 3);
  } else {
    SetVarIndexed(IDX_LEARNING_BACK, 0);
  }
}

void Visu_L_GetText(const tControl * Ctrl) {
  tProfile * Prof = Profile_GetCurrentProfile();
  if (NULL == Prof)
    return;

  //Get Attachment name
  uint32_t idx = 0;
  uint32_t list = 0;

  for (int32_t i = 0; i < 3; i++) {
    if (Ctrl -> cmd_ABC[i].Active) {
      if (!idx) idx = i + 1;
      break;
    }
  }

  switch (idx) {
  case 1:
    list = RCTEXT_L_A_ATTACHMENT;
    break;
  case 2:
    list = RCTEXT_L_B_ATTACHMENT;
    break;
  case 3:
    list = RCTEXT_L_C_ATTACHMENT;
    break;

  case 0:
  default:
    list = RCTEXT_L_TOPATTACHMENT;
    break;
  }
  const char * EquipName = RCTextGetListElement(list, Prof -> module[idx], GetVarIndexed(IDX_SYS_LANGUAGE));
  const char * Cmd = NULL;
  //if(ABC-Attachment):
  if (idx) {

    const tABC_Attach * Atta = & Ctrl -> ABC[idx - 1].ABC_Attach;
    const tJSCommands * ABCCmd = & Ctrl -> cmd_ABC[idx - 1];
    if (Atta -> ValveUpDown.Type != VALVE_TYPE_NONE) {
      if (0 < ABCCmd -> Updown) {
        Cmd = RCTextGetText(RCTEXT_T_RISE, GetVarIndexed(IDX_SYS_LANGUAGE));
      } else if (0 > ABCCmd -> Updown) {
        Cmd = RCTextGetText(RCTEXT_T_LOWER, GetVarIndexed(IDX_SYS_LANGUAGE));
      }
    }

    if (Atta -> ValveLeftRight.Type != VALVE_TYPE_NONE) {
      if (0 < ABCCmd -> LeftRight) {
        Cmd = RCTextGetText(RCTEXT_T_LEFT, GetVarIndexed(IDX_SYS_LANGUAGE));
        if (idx == 2 && strstr(EquipName, "Meiren MSPN")) {
          Cmd = RCTextGetText(RCTEXT_T_SHIFTLEFT, GetVarIndexed(IDX_SYS_LANGUAGE));
        }
      } else if (0 > ABCCmd -> LeftRight) {
        Cmd = RCTextGetText(RCTEXT_T_RIGHT, GetVarIndexed(IDX_SYS_LANGUAGE));
        if (idx == 2 && strstr(EquipName, "Meiren MSPN")) {
          Cmd = RCTextGetText(RCTEXT_T_SHIFTRIGHT, GetVarIndexed(IDX_SYS_LANGUAGE));
        }
      }
    }

    if (Atta -> ValveRotation.Type != VALVE_TYPE_NONE) {
      if (IOT_Button_IsDown( & Ctrl -> joystick.Button[JSB_F])) {
        Cmd = RCTextGetText(RCTEXT_T_BRUSH_, GetVarIndexed(IDX_SYS_LANGUAGE));
      }
    } else if (Atta -> ValveFloat.Type != VALVE_TYPE_NONE) {
      if (IOT_Button_IsDown( & Ctrl -> joystick.Button[JSB_F])) {
        Cmd = RCTextGetText(RCTEXT_T_FLOATING, GetVarIndexed(IDX_SYS_LANGUAGE));
      }
    }

    //_______________________________
    //ex-/retract
    if (Atta -> ValveExRetract.Type != VALVE_TYPE_NONE) {
      //if two sectoins:
      if (IOT_AnaJoy_GetInPercent( & Ctrl -> joystick.Z)) {
        //two sections?
        if (Ctrl -> ABC[EQUIP_C].ABC_Attach.ValveSecondSection.Type != VALVE_TYPE_NONE) {
          if (IOT_Button_IsDown( & Ctrl -> joystick.Button[JSB_A])) {

            Cmd = RCTextGetText(RCTEXT_T_EXTRACTREATRACTLEFT, GetVarIndexed(IDX_SYS_LANGUAGE));
          } else if (IOT_Button_IsDown( & Ctrl -> joystick.Button[JSB_C])) {
            Cmd = RCTextGetText(RCTEXT_T_EXTRACTRETRACTRIGHT, GetVarIndexed(IDX_SYS_LANGUAGE));
          }
        } else //one section
        {
          Cmd = RCTextGetText(RCTEXT_T_EXTRACTRETRACT, GetVarIndexed(IDX_SYS_LANGUAGE));
        }
      }
    }
    //blade up/down added by A.K.
    if (Atta -> ValveSteelUpDown.Type != VALVE_TYPE_NONE) {
      if (IOT_AnaJoy_GetInPercent( & Ctrl -> joystick.Z)) {
        Cmd = RCTextGetText(RCTEXT_T_STEELBLADEUPDOWN, GetVarIndexed(IDX_SYS_LANGUAGE));
      }
    }

    //_______________________________
    //Work/Transportposition
    if (Atta -> ValveWorkTransport.Type != VALVE_TYPE_NONE) {
      if (0 > ABCCmd -> LeftRight) {
        Cmd = RCTextGetText(RCTEXT_T_WORKPOSITION, GetVarIndexed(IDX_SYS_LANGUAGE));
      } else if (0 < ABCCmd -> LeftRight) {
        Cmd = RCTextGetText(RCTEXT_T_TRANSPORTPOSITON, GetVarIndexed(IDX_SYS_LANGUAGE));
      }
    }
  } else //TopAttachment
  {
    const tJSCommands * CmdNoKey = & Ctrl -> cmd_no_key;
    const tTopEquip * Top = & Ctrl -> top;
    //Check boost
    if (Top->BoostOn) {
      Cmd = RCTextGetText(RCTEXT_T_BOOSTATPRESS, GetVarIndexed(IDX_SYS_LANGUAGE));
    }
    //__________________
    //RTR-Controle
    if (Top -> WinterTop.SpreadConv.Valve.Type != VALVE_TYPE_NONE) {
      if (CmdNoKey -> X) {
        Cmd = RCTextGetText(RCTEXT_T_CHANGEDENSITY, GetVarIndexed(IDX_SYS_LANGUAGE));
      }
      if (CmdNoKey -> Y) {
        Cmd = RCTextGetText(RCTEXT_T_CHANGESPREADINGWIDTH, GetVarIndexed(IDX_SYS_LANGUAGE));
      }
      if (CmdNoKey -> Z) {
        Cmd = RCTextGetText(RCTEXT_T_CHANGESPREADINGPOSITION, GetVarIndexed(IDX_SYS_LANGUAGE));
      }
      if (IOT_Button_IsDown( & Ctrl -> joystick.Button[JSB_E])) {
        Cmd = RCTextGetText(RCTEXT_T_RTR_ONOFF, GetVarIndexed(IDX_SYS_LANGUAGE));
      }
    }
    //____________________
    //CK-Controle
    const tSummerTop * TopLRS = & Ctrl -> top.SummerTop;
    if (TopLRS->LRS.DensityValve.Type != VALVE_TYPE_NONE) {
      if (CmdNoKey -> Z) {
        Cmd = RCTextGetText(RCTEXT_T_CHANGESPREADINGPOSITION, GetVarIndexed(IDX_SYS_LANGUAGE));
      }
    }
    if (TopLRS->Pump.on != NULL) {
      if (IOT_Button_IsDown( & Ctrl -> joystick.Button[JSB_E])) {
        Cmd = RCTextGetText(RCTEXT_T_PUMPONOFF, GetVarIndexed(IDX_SYS_LANGUAGE));
      }
    }
    if (TopLRS->MainValve.Type != VALVE_TYPE_NONE) {
      if (IOT_Button_IsDown( & Ctrl -> joystick.Button[JSB_D])) {
        Cmd = RCTextGetText(RCTEXT_T_TOPMAINVALVE, GetVarIndexed(IDX_SYS_LANGUAGE));
      }
    }
    if (TopLRS->LRS.LeftValve.Type != VALVE_TYPE_NONE) {
      if (CmdNoKey -> Z > 0) {
        Cmd = RCTextGetText(RCTEXT_T_LEFTWATERVALVE, GetVarIndexed(IDX_SYS_LANGUAGE));
      }
    }
    if (TopLRS->LRS.RightValve.Type != VALVE_TYPE_NONE) {
      if (CmdNoKey -> Z < 0) {
        Cmd = RCTextGetText(RCTEXT_T_RIGHTWATERVALVE, GetVarIndexed(IDX_SYS_LANGUAGE));
      } else if (CmdNoKey -> Y > 0) {
        Cmd = RCTextGetText(RCTEXT_T_ALLWATERWALVES, GetVarIndexed(IDX_SYS_LANGUAGE));
      }
    }
  }

  char buffer[256];
  static char buffer_old[256];

  if ((NULL != Cmd) &&
    (NULL != EquipName)) {
    snprintf(buffer, sizeof(buffer), Cmd, EquipName);
  } else if (idx) {
    if ((idx > 0) && (idx <= 3)) {
      char Button[8] = {
        0
      };
      switch (idx) {
      case 1:
        snprintf(Button, sizeof(Button), "A");
        break;

      case 2:
        snprintf(Button, sizeof(Button), "B");
        break;

      case 3:
        snprintf(Button, sizeof(Button), "C");
        break;
      default:

        break;
      }
      if (NULL != EquipName) {
        Cmd = RCTextGetText(RCTEXT_T_BUTTONPRESSED, GetVarIndexed(IDX_SYS_LANGUAGE));
        snprintf(buffer, sizeof(buffer), Cmd, Button, EquipName);
      } else {
        Cmd = RCTextGetText(RCTEXT_T_BUTTONPRESSEDNOEQUIP, GetVarIndexed(IDX_SYS_LANGUAGE));
        snprintf(buffer, sizeof(buffer), Cmd, Button);
      }
    }

  } else {
    Cmd = RCTextGetText(RCTEXT_T_USEJOYSTICK, GetVarIndexed(IDX_SYS_LANGUAGE));
    snprintf(buffer, sizeof(buffer), Cmd);
  }
  if (0 != strncmp(buffer, buffer_old, sizeof(buffer))) {
    strncpy(buffer_old, buffer, sizeof(buffer));
    SetVisObjData(OBJ_LEARNTEXT, buffer, strlen(buffer) + 1);
  }
}

void Visu_L_Loading(const tVisuData * VData, uint32_t evtc, tUserCEvt * evtv) {
  static int32_t count = 0;
  static int32_t showCNTLoading = 0;
  static int32_t flagShowLoadingBox = 0;

  if (count <= 150) {
    count++;
  }
  if (showCNTLoading == 0) {
    if (flagShowLoadingBox == 0) {
      SendToVisuObj(OBJ_LOADINGTEXT, GS_TO_VISU_SET_ATTR_VISIBLE, 1);
      SendToVisuObj(OBJ_LOADINGBOX, GS_TO_VISU_SET_ATTR_VISIBLE, 1);
      flagShowLoadingBox = 1;
    }
    if (count == 20) {
      SendToVisuObj(OBJ_INDEXBITMAP518, GS_TO_VISU_SET_ATTR_VISIBLE, 1);
    }
  }
  if (count == 150) {
    SendToVisuObj(OBJ_LOADINGTEXT, GS_TO_VISU_SET_ATTR_VISIBLE, 0);
    SendToVisuObj(OBJ_LOADINGBOX, GS_TO_VISU_SET_ATTR_VISIBLE, 0);
    showCNTLoading = 1;
    loadingPanel = 1;
    count = 0;
    flagShowLoadingBox = 0;
  }
}

void Visu_L_StartPanel(const tVisuData * VData, uint32_t evtc, tUserCEvt * evtv) {
  if (IsInfoContainerOn(CNT_LEARNING) && startPanel == 0 && loadingPanel == 1) {
    if (!IsMsgContainerOn(CNT_LEARNINGSTARTPANEL)) {
      MsgContainerOn(CNT_LEARNINGSTARTPANEL);
    }
    for (uint32_t i = 0; i < evtc; i++) {
      if (CEVT_MENU_INDEX == evtv[i].Type) {
        uint32_t ObjId = evtv[i].Content.mMenuIndex.ObjID;
        if (OBJ_LSPBUTTON == ObjId) {
          startPanel = 1;
          MsgContainerOff(CNT_LEARNINGSTARTPANEL);
        }
      }
    }
  }
}

void Visu_L_TopPanel(const tVisuData * VData, uint32_t evtc, tUserCEvt * evtv) {
  if (IsInfoContainerOn(CNT_LEARNING) && topPanel == 0 && startPanel == 1) {
    if (!IsMsgContainerOn(CNT_LEARNINGTOPPANEL)) {
      MsgContainerOn(CNT_LEARNINGTOPPANEL);
    }
    for (uint32_t i = 0; i < evtc; i++) {
      if (CEVT_MENU_INDEX == evtv[i].Type) {
        uint32_t ObjId = evtv[i].Content.mMenuIndex.ObjID;
        if (OBJ_LTPBUTTON == ObjId) {
          topPanel = 1;
          MsgContainerOff(CNT_LEARNINGTOPPANEL);
        }
      }
    }
  }
}

void Visu_L_Back(const tVisuData * VData) {
  if (IsKeyPressedNew(1) && IsMsgContainerOn(CNT_LIGHTKEYS) == 0) {
    Visu_Learning_Close(VData);
    MsgContainerOn(CNT_MAINMENU);
  }
}

void Visu_Learning_Cycle(const tVisuData * VData, uint32_t evtc, tUserCEvt * evtv) {
  const tControl * Ctrl = CtrlGet();
  Visu_L_DrawBackground(Ctrl);
  Visu_L_Loading(VData, evtc, evtv);
  Visu_L_StartPanel(VData, evtc, evtv);
  Visu_L_TopPanel(VData, evtc, evtv);
  Visu_L_GetText(Ctrl);
  Visu_L_Back(VData);
}
