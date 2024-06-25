/****************************************************************************
*
* File:         VISU_PROPERTIES.c
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
#include "visu_properties.h"

#include "param.h"
#include "control.h"
#include "gse_msgbox.h"
#include "RCText.h"

/****************************************************************************/

/* macro definitions ********************************************************/

/* type definitions *********************************************************/

/* prototypes ***************************************************************/

/* global constants *********************************************************/

/* global variables *********************************************************/
extern tParamData		Parameter;
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
void Visu_Properties_Init(const tVisuData * VData) {
  SetVisObjData(OBJ_TRUETYPETEXT847, Parameter.VIN.VIN, strlen(Parameter.VIN.VIN) + 1);
  SetVisObjData(OBJ_TRUETYPETEXT849, Parameter.VIN.Equip1, strlen(Parameter.VIN.Equip1) + 1);
  SetVisObjData(OBJ_TRUETYPETEXT859, Parameter.VIN.Equip2, strlen(Parameter.VIN.Equip2) + 1);
}

void Visu_Properties_Open(const tVisuData * VData) {
  PrioMaskOn(MSK_PROPERTIES);
}

void Visu_Properties_Close(const tVisuData * VData) {

}

void Visu_Properties_Back(const tVisuData * VData) {
  uint8_t isLearningMode = 0;

  if (IsKeyPressedNew(1)) {
    if (GetVar(HDL_LEARNING) == 1) {
      isLearningMode = 1;
    }

    Visu_OpenScreen(SCREEN_MAIN);
    MsgContainerOn(CNT_MAINMENU);
  }
  if (isLearningMode == 1) {
    SetVar(HDL_LEARNING, 1);
    isLearningMode = 0;
  }
}

void Visu_Properties_Cycle(const tVisuData * VData, uint32_t evtc, tUserCEvt * evtv) {
  for (uint32_t i = 0; i < evtc; i++) {
    if (CEVT_MENU_INDEX == evtv[i].Type) {
      uint32_t obj = evtv[i].Content.mMenuIndex.ObjID;
      if (OBJ_MENUTEXT_LIGHTEQUIP == obj) {
        PrioMaskOn(MSK_LIGHTINGEQUIP);
      } else if (obj == OBJ_RESPONSEERROR) {
        Visu_OpenScreen(SCREEN_CHANGE_ERROR);
      } else if (obj == OBJ_JOY_SIGN) {
        PrioMaskOn(MSK_RESPONSEJOYSTICK);
      }
    }
  }

  if (IsMaskOn(MSK_RESPONSEJOYSTICK) == 1) {
    if (IsKeyPressedNew(1)) PrioMaskOn(MSK_PROPERTIES);
  } else {
    Visu_Properties_Back(VData);
  }
  Visu_HomeKeyPressed();
}

