/****************************************************************************
*
* File:         VISU_MAIN.c
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

#include "visu_material.h"

#include "vartab.h"     /* Variable Table definition                      */
/* include this file in every C-Source to access  */
/* the variable table of your project             */
/* vartab.h is generated automatically            */
#include "objtab.h"     /* Object ID definition                           */
						/* include this file in every C-Source to access  */
						/* the object ID's of the visual objects          */
						/* objtab.h is generated automatically            */
#include "visu.h"
#include "RCText.h"
#include "profile.h"
#include "gseerrorlist.h"
#include "gse_msgbox.h"
#include "gsToVisu.h"
#include "RCColor.h"
#include "visu_learning.h"
#include "control.h"
#include "param.h"
#include "gsemcm.h"
#include "ctrl_top_winter_attachment.h"
#include "io.h"

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

void Visu_Main_Open(const tVisuData * VData) {
  PrioMaskOn(MSK_MAIN);
//  db_out("Draw Mainscreen:\r\n");
  Prof_Draw();
//  db_out("Mainscreen drawn\r\n");
}

void Visu_Main_Close(const tVisuData * VData) {
  MsgContainerOff(CNT_MAINMENU);
  MsgContainerOff(CNT_PASSWORD);
  ProfHide();
  Visu_Learning_Close(VData);
}

uint32_t Password_Merkator = 0;

void Password_Open(const tVisuData * VData) {
  SetVar(HDL_PASSWORD_ENTER, 0);
  MsgContainerOff(CNT_MAINMENU);
  MsgContainerOn(CNT_PASSWORD);

}

void Password_Cycle(const tVisuData * VData, uint32_t evtc, tUserCEvt * evtv) {
  uint32_t Password = (uint32_t) GetVarIndexed(IDX_PASSWORD_ENTER);

  if (Password == (uint32_t) GetVar(HDL_PASSWORD_MERKATOR) || Password == 32546) {
    MsgContainerOff(CNT_PASSWORD);
    SetVar(HDL_USER, RCTEXT_LI_USER_PASSWORD_MERKATOR);
    Visu_OpenScreen(SCREEN_MENU_MERKATOR);
  } else if (Password == (uint32_t) GetVar(HDL_PASSWORD_ADMIN)) {
    MsgContainerOff(CNT_PASSWORD);
    SetVar(HDL_USER, RCTEXT_LI_USER_PASSWORD_ADMIN);
    Visu_OpenScreen(SCREEN_MENU_ADMIN);
  }
}

void MenuContainer_Cycle(const tVisuData * VData, uint32_t evtc, tUserCEvt * evtv) {
  for (uint32_t i = 0; i < evtc; i++) {
    if (CEVT_MENU_INDEX == evtv[i].Type) {
      uint32_t obj = evtv[i].Content.mMenuIndex.ObjID;
      if (OBJ_MENUTEXT_ERRORS == obj) {
        Visu_OpenScreen(SCREEN_ERROR);
      } else if (OBJ_MENU_TEXT_PROPERTIES == obj) {
        Visu_OpenScreen(SCREEN_PROPERTIES);
      } else if (OBJ_MENUTEXT_LEARNING == obj) {
        SetVar(HDL_LEARNING, 1);
        InfoContainerOn(CNT_LEARNING);
        MsgContainerOff(CNT_MAINMENU);
      } else if (OBJ_MENUTEXT_SERVICEMENU_1 == obj) {
        if (GetVarIndexed(IDX_USER) == RCTEXT_LI_USER_PASSWORD_MERKATOR) {
          Visu_OpenScreen(SCREEN_MENU_MERKATOR);
        } else if (GetVarIndexed(IDX_USER) == RCTEXT_LI_USER_PASSWORD_ADMIN) {
          Visu_OpenScreen(SCREEN_MENU_ADMIN);
        } else {
          Password_Open(VData);
        }
      } else if (OBJ_MENUTEXT_SPREADSETTING == obj) {
        Visu_OpenScreen(SCREEN_SPREADINGSETTINGS);
      }
    }
  }

  static uint8_t flagShowOBJ = 0;
  static uint8_t flagHideOBJ = 0;
  if(GetVarIndexed(IDX_LEARNING)) {
    if(flagHideOBJ == 0) {
      SendToVisuObj(OBJ_MENUTEXT_SERVICEMENU_1, GS_TO_VISU_SET_ATTR_VISIBLE, 0);
      SendToVisuObj(OBJ_MENU_TEXT_PROPERTIES, GS_TO_VISU_SET_ATTR_VISIBLE, 0);
      SendToVisuObj(OBJ_ATRESTTTEXT426, GS_TO_VISU_SET_ATTR_VISIBLE, 1);
      flagShowOBJ = 0;
      flagHideOBJ = 1;
    }
  } else {
    if(flagShowOBJ == 0) {
      SendToVisuObj(OBJ_MENUTEXT_SERVICEMENU_1, GS_TO_VISU_SET_ATTR_VISIBLE, 1);
      SendToVisuObj(OBJ_MENU_TEXT_PROPERTIES, GS_TO_VISU_SET_ATTR_VISIBLE, 1);
      SendToVisuObj(OBJ_ATRESTTTEXT426, GS_TO_VISU_SET_ATTR_VISIBLE, 0);
      flagHideOBJ = 0;
      flagShowOBJ = 1;
    }
  }
}

/**
 * @brief Цикл пункта меню "Обучение" и его графическое отображение
 */
void Visu_Main_Learning_Ctrl(const tVisuData * VData, uint32_t evtc, tUserCEvt * evtv) {
  static uint8_t flagShowLearnText = 0;
  if (GetVarIndexed(IDX_LEARNING)) {
    Visu_Learning_Cycle(VData, evtc, evtv);
    if (flagShowLearnText == 0) {
      // Отображение текстового индикатора включения режима обучения
      SendToVisuObj(OBJ_TRUETYPETIMEANDDATA368, GS_TO_VISU_SET_ATTR_VISIBLE, 0);
      SendToVisuObj(OBJ_ATRESTTTEXT1058, GS_TO_VISU_SET_ATTR_VISIBLE, 1);
      flagShowLearnText = 1;
    }
  } else {
    if (flagShowLearnText == 1) {
      // Скрытие текстового индикатора режима обучение и отображение часов
      SendToVisuObj(OBJ_ATRESTTTEXT1058, GS_TO_VISU_SET_ATTR_VISIBLE, 0);
      SendToVisuObj(OBJ_TRUETYPETIMEANDDATA368, GS_TO_VISU_SET_ATTR_VISIBLE, 1);
      flagShowLearnText = 0;
    }
  }
}

/**
 * @brief Управление и переключение контейнеров света, светосигнального оборудования.
 */
extern uint8_t flagOKB600_Init;
void Visu_Main_Light_Ctrl(const tVisuData * VData, uint32_t evtc, tUserCEvt * evtv) {
  const tControl * Ctrl = CtrlGet();

  // Отображение иконки маяков и заднего света
  SetVar(HDL_ATT_BEACON, ATTR_VISIBLE);
  SetVar(HDL_ATT_REAR, ATTR_VISIBLE);

  // Долгое нажатие кнопки света
  if (IOT_Button_IsPressedLongNew( & Ctrl -> D3510.Button[5])) {
    SetVarIndexed(IDX_LIGHT_ALARM, 1 - GetVarIndexed(IDX_LIGHT_ALARM));
  }

  // Открытие первого контейнера света
  if (IOT_Button_IsReleasedShortNew( & Ctrl -> D3510.Button[5])) {
    if (IsMsgContainerOn(CNT_LIGHTKEYS) || IsMsgContainerOn(CNT_LIGHTKEYS_2)) {
      MsgContainerOff(CNT_LIGHTKEYS);
      MsgContainerOff(CNT_LIGHTKEYS_2);
    } else {
      MsgContainerOn(CNT_LIGHTKEYS);
    }
  }

  if (GetVarIndexed(IDX_ROADSIGN_LEFT) || GetVarIndexed(IDX_ROADSIGN_RIGHT)) {
    if (GetVarIndexed(IDX_CHECK_SIDELIGHT) || GetVarIndexed(IDX_CHECK_HOPPERLIGHT) || GetVarIndexed(IDX_CHECK_INTERLIGHT) || flagOKB600_Init) {
      SetVar(HDL_ATT_RIGHTARROW_1_, ATTR_VISIBLE);

      // Переключение между контейнерами
      if (IOT_Button_IsReleasedShortNew( & Ctrl -> D3510.Button[4])) {
        if (IsMsgContainerOn(CNT_LIGHTKEYS)) {
          MsgContainerOff(CNT_LIGHTKEYS);
          MsgContainerOn(CNT_LIGHTKEYS_2);
        } else if (IsMsgContainerOn(CNT_LIGHTKEYS_2)) {
          MsgContainerOff(CNT_LIGHTKEYS_2);
          MsgContainerOn(CNT_LIGHTKEYS);
        }
      }
    }
  } else {
    SetVar(HDL_ATT_RIGHTARROW_1_, 0);
  }

  // Отображение иконок знаков при установленных знаках
  if (GetVarIndexed(IDX_ROADSIGN_LEFT)) {
    SetVar(HDL_ATT_LEFTSIGN, ATTR_VISIBLE);
  } else {
    SetVar(HDL_ATT_LEFTSIGN, 0);
  }
  if (GetVarIndexed(IDX_ROADSIGN_RIGHT)) {
    SetVar(HDL_ATT_RIGHTSIGN, ATTR_VISIBLE);
  } else {
    SetVar(HDL_ATT_RIGHTSIGN, 0);
  }

  if (GetVarIndexed(IDX_ROADSIGN_LEFT) || GetVarIndexed(IDX_ROADSIGN_RIGHT)) {
    SetVar(HDL_ATT_RIGHTARROW_2_, ATTR_VISIBLE);
    // Отображение иконок бокового света и подсветки бункера на второй вкладке
    if (GetVarIndexed(IDX_CHECK_SIDELIGHT)) {
      SetVar(HDL_ATT_SIDE_1_, 0);
      SetVar(HDL_ATT_SIDE_2_, ATTR_VISIBLE);
    } else {
      SetVar(HDL_ATT_SIDE_1_, 0);
      SetVar(HDL_ATT_SIDE_2_, 0);
    }
    if (GetVarIndexed(IDX_CHECK_HOPPERLIGHT)) {
      SetVar(HDL_ATT_HOPPER_2_, ATTR_VISIBLE);
      SetVar(HDL_ATT_HOPPER_1_, 0);
    } else {
      SetVar(HDL_ATT_HOPPER_1_, 0);
      SetVar(HDL_ATT_HOPPER_2_, 0);
    }
    if (GetVarIndexed(IDX_CHECK_INTERLIGHT)) {
      SetVar(HDL_ATT_INTER_2_, ATTR_VISIBLE);
      SetVar(HDL_ATT_INTER_1_, 0);
    } else {
      SetVar(HDL_ATT_INTER_1_, 0);
      SetVar(HDL_ATT_INTER_2_, 0);
    }
    if(flagOKB600_Init) {
      SetVar(HDL_ATT_OKB600_2_, ATTR_VISIBLE);
      SetVar(HDL_ATT_OKB600_1_, 0);
    } else {
      SetVar(HDL_ATT_OKB600_1_, 0);
      SetVar(HDL_ATT_OKB600_2_, 0);
    }
  } else {
    // Отображение иконок бокового света и подсветки бункера на первой вкладке
    if (GetVarIndexed(IDX_CHECK_SIDELIGHT)) {
      SetVar(HDL_ATT_SIDE_1_, ATTR_VISIBLE);
    } else {
      SetVar(HDL_ATT_SIDE_1_, 0);
    }
    if (GetVarIndexed(IDX_CHECK_HOPPERLIGHT)) {
      SetVar(HDL_ATT_HOPPER_1_, ATTR_VISIBLE);
    } else {
      SetVar(HDL_ATT_HOPPER_1_, 0);
    }
    if (GetVarIndexed(IDX_CHECK_INTERLIGHT)) {
      SetVar(HDL_ATT_INTER_1_, ATTR_VISIBLE);
    } else {
      SetVar(HDL_ATT_INTER_1_, 0);
    }
    if(flagOKB600_Init) {
      SetVar(HDL_ATT_OKB600_1_, ATTR_VISIBLE);
    } else {
      SetVar(HDL_ATT_OKB600_1_, 0);
    }
  }

  if (GetVarIndexed(IDX_ROADSIGN_LEFT) || GetVarIndexed(IDX_ROADSIGN_RIGHT)) {
    if (GetVarIndexed(IDX_CHECK_SIDELIGHT) && GetVarIndexed(IDX_CHECK_INTERLIGHT) && (GetVarIndexed(IDX_CHECK_HOPPERLIGHT) || flagOKB600_Init)) {
      SendToVisuObj(OBJ_INDEXBITMAP423, GS_TO_VISU_SET_ORIGIN_X, 171);
      SendToVisuObj(OBJ_INDEXBITMAP579, GS_TO_VISU_SET_ORIGIN_X, 305);
      SendToVisuObj(OBJ_INDEXBITMAP1315, GS_TO_VISU_SET_ORIGIN_X, 305);
    }
    if (!GetVarIndexed(IDX_CHECK_SIDELIGHT) && GetVarIndexed(IDX_CHECK_INTERLIGHT) && (GetVarIndexed(IDX_CHECK_HOPPERLIGHT) || flagOKB600_Init)) {
      SendToVisuObj(OBJ_INDEXBITMAP423, GS_TO_VISU_SET_ORIGIN_X, 37);
      SendToVisuObj(OBJ_INDEXBITMAP579, GS_TO_VISU_SET_ORIGIN_X, 171);
      SendToVisuObj(OBJ_INDEXBITMAP1315, GS_TO_VISU_SET_ORIGIN_X, 171);
    }
    if (!GetVarIndexed(IDX_CHECK_SIDELIGHT) && GetVarIndexed(IDX_CHECK_INTERLIGHT) && !GetVarIndexed(IDX_CHECK_HOPPERLIGHT)) {
      SendToVisuObj(OBJ_INDEXBITMAP423, GS_TO_VISU_SET_ORIGIN_X, 37);
    }
    if (!GetVarIndexed(IDX_CHECK_SIDELIGHT) && !GetVarIndexed(IDX_CHECK_INTERLIGHT) && (GetVarIndexed(IDX_CHECK_HOPPERLIGHT) || flagOKB600_Init)) {
      SendToVisuObj(OBJ_INDEXBITMAP579, GS_TO_VISU_SET_ORIGIN_X, 37);
      SendToVisuObj(OBJ_INDEXBITMAP1315, GS_TO_VISU_SET_ORIGIN_X, 37);
    }
    if (GetVarIndexed(IDX_CHECK_SIDELIGHT) && GetVarIndexed(IDX_CHECK_INTERLIGHT) && !GetVarIndexed(IDX_CHECK_HOPPERLIGHT)) {
      SendToVisuObj(OBJ_INDEXBITMAP423, GS_TO_VISU_SET_ORIGIN_X, 171);
    }
    if (GetVarIndexed(IDX_CHECK_SIDELIGHT) && !GetVarIndexed(IDX_CHECK_INTERLIGHT) && (GetVarIndexed(IDX_CHECK_HOPPERLIGHT) || flagOKB600_Init)) {
      SendToVisuObj(OBJ_INDEXBITMAP579, GS_TO_VISU_SET_ORIGIN_X, 171);
    }
    if (GetVarIndexed(IDX_CHECK_SIDELIGHT) && !GetVarIndexed(IDX_CHECK_INTERLIGHT) && (!GetVarIndexed(IDX_CHECK_HOPPERLIGHT) || flagOKB600_Init)) {
      SendToVisuObj(OBJ_INDEXBITMAP579, GS_TO_VISU_SET_ORIGIN_X, 171);
      SendToVisuObj(OBJ_INDEXBITMAP1315, GS_TO_VISU_SET_ORIGIN_X, 171);
    }
  } else {
    if (GetVarIndexed(IDX_CHECK_SIDELIGHT) && GetVarIndexed(IDX_CHECK_INTERLIGHT) && (GetVarIndexed(IDX_CHECK_HOPPERLIGHT) || flagOKB600_Init)) {
      SendToVisuObj(OBJ_INDEXBITMAP420, GS_TO_VISU_SET_ORIGIN_X, 305);
      SendToVisuObj(OBJ_INDEXBITMAP757, GS_TO_VISU_SET_ORIGIN_X, 443);
      SendToVisuObj(OBJ_INDEXBITMAP113, GS_TO_VISU_SET_ORIGIN_X, 572);
      SendToVisuObj(OBJ_INDEXBITMAP1314, GS_TO_VISU_SET_ORIGIN_X, 570);
    }
    if (!GetVarIndexed(IDX_CHECK_SIDELIGHT) && GetVarIndexed(IDX_CHECK_INTERLIGHT) && (GetVarIndexed(IDX_CHECK_HOPPERLIGHT) || flagOKB600_Init)) {
      SendToVisuObj(OBJ_INDEXBITMAP757, GS_TO_VISU_SET_ORIGIN_X, 305);
      SendToVisuObj(OBJ_INDEXBITMAP113, GS_TO_VISU_SET_ORIGIN_X, 443);
      SendToVisuObj(OBJ_INDEXBITMAP1314, GS_TO_VISU_SET_ORIGIN_X, 443);
    }
    if (!GetVarIndexed(IDX_CHECK_SIDELIGHT) && GetVarIndexed(IDX_CHECK_INTERLIGHT) && !GetVarIndexed(IDX_CHECK_HOPPERLIGHT)) {
      SendToVisuObj(OBJ_INDEXBITMAP757, GS_TO_VISU_SET_ORIGIN_X, 305);
    }
    if (!GetVarIndexed(IDX_CHECK_SIDELIGHT) && !GetVarIndexed(IDX_CHECK_INTERLIGHT) && (GetVarIndexed(IDX_CHECK_HOPPERLIGHT) || flagOKB600_Init)) {
      SendToVisuObj(OBJ_INDEXBITMAP113, GS_TO_VISU_SET_ORIGIN_X, 305);
      SendToVisuObj(OBJ_INDEXBITMAP1314, GS_TO_VISU_SET_ORIGIN_X, 305);
    }
    if (GetVarIndexed(IDX_CHECK_SIDELIGHT) && GetVarIndexed(IDX_CHECK_INTERLIGHT) && !GetVarIndexed(IDX_CHECK_HOPPERLIGHT)) {
      SendToVisuObj(OBJ_INDEXBITMAP420, GS_TO_VISU_SET_ORIGIN_X, 305);
      SendToVisuObj(OBJ_INDEXBITMAP757, GS_TO_VISU_SET_ORIGIN_X, 443);
    }
    if (GetVarIndexed(IDX_CHECK_SIDELIGHT) && !GetVarIndexed(IDX_CHECK_INTERLIGHT) && (GetVarIndexed(IDX_CHECK_HOPPERLIGHT) || flagOKB600_Init)) {
      SendToVisuObj(OBJ_INDEXBITMAP420, GS_TO_VISU_SET_ORIGIN_X, 305);
      SendToVisuObj(OBJ_INDEXBITMAP113, GS_TO_VISU_SET_ORIGIN_X, 443);
      SendToVisuObj(OBJ_INDEXBITMAP1314, GS_TO_VISU_SET_ORIGIN_X, 443);
    }
    if (GetVarIndexed(IDX_CHECK_SIDELIGHT) && !GetVarIndexed(IDX_CHECK_INTERLIGHT) && !GetVarIndexed(IDX_CHECK_HOPPERLIGHT)) {
      SendToVisuObj(OBJ_INDEXBITMAP420, GS_TO_VISU_SET_ORIGIN_X, 305);
    }
  }
}

/**
 * @brief Управление функцией забора воды (Активация и деактивация).
 */
void Visu_Main_WaterIntake_Ctrl(const tVisuData * VData, uint32_t evtc, tUserCEvt * evtv) {
  const tControl * Ctrl = CtrlGet();
  if (Ctrl -> top.TopType == TOP_SUMMER) {
    if (IsKeyPressedNew(5) && !IsMsgContainerOn(CNT_TOP_E2000)) {
      if (!WaterIntake_IsOn( & Ctrl -> top.SummerTop.PumpIntake)) {
        WaterPumpIntake_Switch( & Ctrl -> top.SummerTop.PumpIntake, 1);
      } else {
        WaterPumpIntake_Switch( & Ctrl -> top.SummerTop.PumpIntake, 0);
      }
    }
  }
}

/**
 * @brief Активация COM с кнопки 1 ПУ. Доступно при отборе мощности от КПП.
 */
void Visu_Main_PTO_Ctrl( void ) {
  const tParamData * Param = ParameterGet();
  const tProfile* Prof = Profile_GetCurrentProfile();

  static uint8_t eqiupIgnore = 0;
  if(Prof->module[1] == RCTEXT_LI_A_ATTACHMENT_ATTACHMENT_OKB600) {
    eqiupIgnore = 1;
  }

  static uint8_t iconPTOInit = 0;
  if(!Param -> Engine.PTOatEngine && !eqiupIgnore) {
    if(!iconPTOInit) {
      SendToVisuObj(OBJ_INDEXBITMAP1323, GS_TO_VISU_SET_ATTR_VISIBLE, 1);
      SendToVisuObj(OBJ_SIMPLEBITMAP1322, GS_TO_VISU_SET_ATTR_VISIBLE, 1);
      iconPTOInit = 1;
    }
  }
}

void Visu_Main_OMP220_Humid( void ){
  const tProfile* Prof = Profile_GetCurrentProfile();
  if(Prof->module[2] == RCTEXT_LI_B_ATTACHMENT_ATTACHMENT_OMP220_HUMID) {
    if(IsKeyPressedNew(2)) {
      SetVarIndexed(IDX_HUMIDON, 1- GetVarIndexed(IDX_HUMIDON));
    }
  }
}


/**
 * @brief Управление кнопками первого ряда пульта. Нижняя часть кнопок маски Main.
 */
void Visu_Main_BottomPanel_Ctrl(const tVisuData * VData, uint32_t evtc, tUserCEvt * evtv) {
  const tControl * Ctrl = CtrlGet();
  const tParamData * Param = ParameterGet();

  static uint8_t flagWrkHumid = 0;
  static uint8_t flagModeHumid = 0;
  if (IsMsgContainerOn(CNT_LIGHTKEYS) == 0 && IsMsgContainerOn(CNT_LIGHTKEYS_2) == 0) {
    if (IsKeyPressedNew(3)) {
      Ctrl_Automatic_Toggle();
    }

    if(Param->RTR.TypeRTR) {
      if(Ctrl->top.WinterTop.RTRHumid.HumidAvailable) {
        if(Param->RTR.HumidType == 2) { // Соргаз
          if (IsKeyPressedNew(2)) {
            flagModeHumid = 1 - flagModeHumid;
            RTR_Humid_ManualMode_Switch(&Ctrl->top.WinterTop.RTRHumid, flagModeHumid);
          }
        }

        if (IsKeyPressedNew(4)) {
          flagWrkHumid = 1 - flagWrkHumid;
          RTR_Humid_Switch(&Ctrl->top.WinterTop.RTRHumid, flagWrkHumid);
        }
      }
      Ctrl_RTR_Dump( & Ctrl -> top.WinterTop);
    }

    Visu_Main_WaterIntake_Ctrl(VData, evtc, evtv);
    Visu_Main_PTO_Ctrl();
    Visu_Main_OMP220_Humid();
  }
}

void Visu_Main_MainMenu_Ctrl(const tVisuData * VData, uint32_t evtc, tUserCEvt * evtv) {
  if (IsKeyPressedNew(10)) {
    if (GetVarIndexed(IDX_LEARNING)) {
      SetVarIndexed(IDX_LEARNING, 0);
      Visu_Learning_Close(VData);
      MsgContainerOff(CNT_MAINMENU);
    } else if (IsMsgContainerOn(CNT_MAINMENU)) {
      MsgContainerOff(CNT_MAINMENU);
    } else {
      MsgContainerOn(CNT_MAINMENU);
      MsgContainerOff(CNT_PASSWORD);
      SetVarIndexed(IDX_LEARNING, 0);
      Visu_Learning_Close(VData);
    }
  }

  if (IsMsgContainerOn(CNT_MAINMENU)) {
    MenuContainer_Cycle(VData, evtc, evtv);
  }
}

void Visu_Main_Password_Ctrl(const tVisuData * VData, uint32_t evtc, tUserCEvt * evtv) {
  if (CNT_PASSWORD == GetCurrentMsgContainerShown()) {
    Password_Cycle(VData, evtc, evtv);
  }
}

/**
 * @brief Графическое управление верхней информационной панелью
 */
void Visu_Main_TopPanel_Ctrl(const tVisuData * VData, uint32_t evtc, tUserCEvt * evtv) {
  const tControl * Ctrl = CtrlGet();

  //Icon OilLevel
  static int32_t OilLevel_Old = -1;
  if (OilLevel_Old != GetVarIndexed(IDX_OILLEVEL)) {
    OilLevel_Old = GetVarIndexed(IDX_OILLEVEL);
    if (-1 == OilLevel_Old) {
      SendToVisuObj(OBJ_ICONOILLEVEL, GS_TO_VISU_SET_FG_COLOR, RCColorGetColor(GetVarIndexed(IDX_SYS_COLOR), RCCOLOR_NAME_FRONT));
      SendToVisuObj(OBJ_ICONOILLEVEL, GS_TO_VISU_SET_ATTR_BLINK, 0);
    } else if (2 == OilLevel_Old) {
      SendToVisuObj(OBJ_ICONOILLEVEL, GS_TO_VISU_SET_FG_COLOR, RCColorGetColor(GetVarIndexed(IDX_SYS_COLOR), RCCOLOR_NAME_GREEN));
      SendToVisuObj(OBJ_ICONOILLEVEL, GS_TO_VISU_SET_ATTR_BLINK, 0);
    } else if (1 == OilLevel_Old) {
      SendToVisuObj(OBJ_ICONOILLEVEL, GS_TO_VISU_SET_FG_COLOR, RCColorGetColor(GetVarIndexed(IDX_SYS_COLOR), RCCOLOR_NAME_YELLOW));
      SendToVisuObj(OBJ_ICONOILLEVEL, GS_TO_VISU_SET_ATTR_BLINK, 0);
    } else if (0 == OilLevel_Old) {
      SendToVisuObj(OBJ_ICONOILLEVEL, GS_TO_VISU_SET_FG_COLOR, RCColorGetColor(GetVarIndexed(IDX_SYS_COLOR), RCCOLOR_NAME_RED));
      SendToVisuObj(OBJ_ICONOILLEVEL, GS_TO_VISU_SET_ATTR_BLINK, 1);
    }
  }

  //Icon Oil Filter:
  static eDISensorState oil_fltr_ = -1;
  if (oil_fltr_ != Ctrl -> sens.DI.OilFilter) {
    oil_fltr_ = Ctrl -> sens.DI.OilFilter;
    if (oil_fltr_) {
      SendToVisuObj(OBJ_ICONFILTER, GS_TO_VISU_SET_FG_COLOR, RCColorGetColor(GetVarIndexed(IDX_SYS_COLOR), RCCOLOR_NAME_RED));
    } else {
      SendToVisuObj(OBJ_ICONFILTER, GS_TO_VISU_SET_FG_COLOR, RCColorGetColor(GetVarIndexed(IDX_SYS_COLOR), RCCOLOR_NAME_GREEN));
    }
  }

  //Auto
  static int8_t oldAutomatic = -1;
  if(oldAutomatic != Ctrl -> automatic.on) {
    oldAutomatic = Ctrl -> automatic.on;
    if (Ctrl -> automatic.on == AUTO_MODE) {
      SetVarIndexed(IDX_AUTOMANU, RCTEXT_LI_AUTOMANU_STANDARD_AUTO);
      SendToVisuObj(OBJ_TRUETYPEVARIABLE369, GS_TO_VISU_SET_FG_COLOR, RCColorGetColor(GetVarIndexed(IDX_SYS_COLOR), RCCOLOR_NAME_FRONT));
    } else {
      SetVarIndexed(IDX_AUTOMANU, RCTEXT_LI_AUTOMANU_STANDARD_MANU);
      SendToVisuObj(OBJ_TRUETYPEVARIABLE369, GS_TO_VISU_SET_FG_COLOR, RCColorGetColor(GetVarIndexed(IDX_SYS_COLOR), RCCOLOR_NAME_FRAMECOLORUNSELECT));
    }
  }

  //Speed
  SetVarIndexed(IDX_SPEED, TopGetSpeed());

  //Temperature
  SetVarIndexed(IDX_TEMPOIL, (IOT_NormAI( & Ctrl -> sens.AI.OilTemp) + 5) / 10 * 10);
  if (Ctrl -> engine.AmbientAirTemp == -273) {
    SetVarIndexed(IDX_TEMPOUTSIDE, 0);
    SendToVisuObj(OBJ_TRUETYPEVARIABLE396, GS_TO_VISU_SET_FG_COLOR, RCColorGetColor(GetVarIndexed(IDX_SYS_COLOR), RCCOLOR_NAME_FRAMECOLORUNSELECT));
  } else {
    SetVarIndexed(IDX_TEMPOUTSIDE, Ctrl -> engine.AmbientAirTemp);
    SendToVisuObj(OBJ_TRUETYPEVARIABLE396, GS_TO_VISU_SET_FG_COLOR, RCColorGetColor(GetVarIndexed(IDX_SYS_COLOR), RCCOLOR_NAME_FRONT));
  }

  static int8_t oldTempVis = -1;
  if(oldTempVis != GetVar(HDL_SW_TEMP)) {
    SendToVisuObj( OBJ_SIMPLEBITMAP395, GS_TO_VISU_SET_ATTR_VISIBLE, GetVar(HDL_SW_TEMP));
    SendToVisuObj( OBJ_TRUETYPEVARIABLE396, GS_TO_VISU_SET_ATTR_VISIBLE, GetVar(HDL_SW_TEMP));
    SendToVisuObj( OBJ_TRUETYPETEXT397, GS_TO_VISU_SET_ATTR_VISIBLE, GetVar(HDL_SW_TEMP));

    oldTempVis = GetVar(HDL_SW_TEMP);
  }
}

void Visu_Main_Cycle(const tVisuData * VData, uint32_t evtc, tUserCEvt * evtv) {
  Visu_Main_MainMenu_Ctrl(VData, evtc, evtv);
  Visu_Main_Password_Ctrl(VData, evtc, evtv);
  Visu_Main_TopPanel_Ctrl(VData, evtc, evtv);
  Visu_Main_BottomPanel_Ctrl(VData, evtc, evtv);
  Visu_Main_Light_Ctrl(VData, evtc, evtv);
  Prof_DrawMove();

  Visu_Main_Learning_Ctrl(VData, evtc, evtv);
}

void Visu_Main_Timer(const tVisuData * VData) {
  static int32_t count = 0;
  count++;
  // Изменение графического индикатора ошибок на Main маске
  if (0 == (count % 10)) {
    if (EList_GetErrors(ES_UNCHECKED_ERROR_ACTIVE | ES_UNCHECKED_ERROR_INACTIVE)) {
      SetVarIndexed(IDX_ERRORICON, 2);
    } else if (EList_GetErrors(ES_CHECKED_ERROR_ACTIVE)) {
      SetVarIndexed(IDX_ERRORICON, 1);
    } else {
      SetVarIndexed(IDX_ERRORICON, 0);
    }
  }
}
