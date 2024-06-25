#include <stdint.h>
#include <UserCEvents.h>
#include <UserCAPI.h>
#include "vartab.h"
#include "objtab.h"
#include "visu.h"
#include "visu_material.h"
#include "gsToVisu.h"
#include "control.h"
#include "RCText.h"
#include "ctrl_top_summer_attachment.h"

void Winter_LBox(uint8_t Visible);
void Summer_LBox(uint8_t Visible);

extern int32_t f_adrv_calibr;
extern int32_t Import_table_cr;

void Material_Draw(void) {
  const tReagent * Reagent = GetActualReagent();
  if (NULL != Reagent) {
    SetVisObjData(OBJ_MATERIALNAME, Reagent -> Name, strlen(Reagent -> Name) + 1);
  }
}

void Winter_SpreadSettings_Open( void ) {
  PrioMaskOn(MSK_WINTER_SPREADINGSETTINGS);
  SetVar(HDL_REAGENT, Reagent_GetIdx() + 1);
  SetVar(HDL_SETTINGSNUM, Reagent_GetNum());
  VarEnableEvent( HDL_REAGENT );
  Material_Draw();
}

void Summer_SpreadSettings_Open( void ) {
  PrioMaskOn(MSK_SUMMER_SPREADINGSETTINGS);
}

void Visu_SpreadSettings_Open(const tVisuData * VData) {
  const tControl * Ctrl = CtrlGet();
  if (Ctrl -> top.TopType == TOP_WINTER) {
    Winter_SpreadSettings_Open();
  } else if (Ctrl -> top.TopType == TOP_SUMMER) {
    Summer_SpreadSettings_Open();
  }
}

void Visu_SpreadSettings_Close(const tVisuData * VData) {
  VarDisableEvent( HDL_REAGENT );
  SetVarIndexed(IDX_LEARNING, 0);
  Winter_LBox(0);
  Summer_LBox(0);
}

void Visu_SpreadSettings_Back(const tVisuData * VData) {
  uint8_t isLearningMode = 0;
  if (IsKeyPressedNew(1)) {
    if (GetVar(HDL_LEARNING) == 1) {
      isLearningMode = 1;
    }
    Visu_SpreadSettings_Close(VData);
    Visu_OpenScreen(SCREEN_MAIN);
    MsgContainerOn(CNT_MAINMENU);
  }
  if (isLearningMode == 1) {
    SetVar(HDL_LEARNING, 1);
    isLearningMode = 0;
  }
}

int Winter_ObjSelected[] = {
  OBJ_ATRESTTTEXTLIST139,
  OBJ_TRUETYPEVARIABLE428,
  OBJ_TRUETYPEVARIABLE713,
  OBJ_TRUETYPEVARIABLE407,
  OBJ_ATRESTTTEXTLIST798,
  OBJ_TRUETYPEVARIABLE712,
  OBJ_ATRESTTTEXTLIST433,
  OBJ_BTN_ASYMCALIBR,
};

uint32_t Winter_Learning_RCTEXT[] = {
  RCTEXT_T_SPS1,
  RCTEXT_T_SPS2,
  RCTEXT_T_SPS6,
  RCTEXT_T_SPS3,
  RCTEXT_T_SPS4,
  RCTEXT_T_SPS5,
  RCTEXT_T_SPS7,
  RCTEXT_T_SPS8,
};

void Winter_LBox(uint8_t Visible) {
  SendToVisuObj(OBJ_W_LSPS_TEXT, GS_TO_VISU_SET_ATTR_VISIBLE, Visible);
  SendToVisuObj(OBJ_W_LSPSMODE, GS_TO_VISU_SET_ATTR_VISIBLE, Visible);
  SendToVisuObj(OBJ_W_LSPSBOX, GS_TO_VISU_SET_ATTR_VISIBLE, Visible);
}

int Summer_ObjSelected[] = {
  OBJ_TRUETYPEVARIABLE691, // Скорость
  OBJ_ATRESTTTEXTLIST914, // OKB600 сторона копирования
  OBJ_ATRESTTTEXTLIST60, // CR600A текущий насос
  OBJ_INDEXBITMAP689, // CR600A пожарный шланг
  OBJ_IMPORT_TABLE, // CR таблица калибровки
  OBJ_BTN_CR_CALIBR, // CR калибровка

};

uint32_t Summer_Learning_RCTEXT[] = {
  RCTEXT_T_SPS6,
  RCTEXT_T_SPS9,
  RCTEXT_T_SPS10,
  RCTEXT_T_SPS11,
  RCTEXT_T_SPS12,
  RCTEXT_T_SPS13,
};

void Summer_LBox(uint8_t Visible) {
  SendToVisuObj(OBJ_S_LSPS_TEXT, GS_TO_VISU_SET_ATTR_VISIBLE, Visible);
  SendToVisuObj(OBJ_S_LSPSMODE, GS_TO_VISU_SET_ATTR_VISIBLE, Visible);
  SendToVisuObj(OBJ_S_LSPSBOX, GS_TO_VISU_SET_ATTR_VISIBLE, Visible);
}

uint32_t GetLearningText = 0;
const char * OutLearningText = NULL;
void SpreadLearning(const tVisuData * VData, uint32_t evtc, tUserCEvt * evt) {
  const tControl * Ctrl = CtrlGet();
  if (GetVar(HDL_LEARNING)) {
    static int32_t count = 0;
    static int32_t flag_out = 0;
    char buffer[256];

    if (Ctrl -> top.TopType == TOP_WINTER) {
      Winter_LBox(1);
      for (uint32_t i = 0; i < evtc; i++) {
        if (evt[i].Type == CEVT_SELECTED) {
          uint32_t obj = evt[i].Content.mSelected.ObjID;
          for (uint8_t j = 0; j < sizeof(Winter_ObjSelected); j++) {
            if (obj == Winter_ObjSelected[j]) {
              GetLearningText = Winter_Learning_RCTEXT[j];
            }
          }
        }
      }
    } else if (Ctrl -> top.TopType == TOP_SUMMER) {
      Summer_LBox(1);
      for (uint32_t i = 0; i < evtc; i++) {
        if (evt[i].Type == CEVT_SELECTED) {
          uint32_t obj = evt[i].Content.mSelected.ObjID;
          for (uint8_t j = 0; j < sizeof(Summer_ObjSelected); j++) {
            if (obj == Summer_ObjSelected[j]) {
              GetLearningText = Summer_Learning_RCTEXT[j];
            }
          }
        }
      }
    }

    OutLearningText = RCTextGetText(GetLearningText, GetVarIndexed(IDX_SYS_LANGUAGE));
    snprintf(buffer, sizeof(buffer), OutLearningText);

    if (flag_out == 0) {
      count++;
      if (count == 10) {
        flag_out = 1;
      }
    }
    if (flag_out == 1) {
			if (Ctrl -> top.TopType == TOP_WINTER) {
				SetVisObjData(OBJ_W_LSPS_TEXT, buffer, strlen(buffer) + 1);
			} else if(Ctrl -> top.TopType == TOP_SUMMER){
				SetVisObjData(OBJ_S_LSPS_TEXT, buffer, strlen(buffer) + 1);
			}

    }
  }
}

void Visu_SpreadSettings_Cycle(const tVisuData * VData, uint32_t evtc, tUserCEvt * evt) {
  tControl * Ctrl = CtrlGet();
  SpreadLearning(VData, evtc, evt);

  for (uint32_t i = 0; i < evtc; i++) {
    if (evt[i].Type == CEVT_VAR_CHANGE) {
      if (evt[i].Content.mVarChange.VarHandle == HDL_REAGENT) {
        Reagent_SetIdx(GetVar(HDL_REAGENT) - 1);
        Material_Draw();
      }
    }
    if (CEVT_MENU_INDEX == evt[i].Type) {
      if (evt[i].Content.mMenuIndex.ObjID == OBJ_BTN_ASYMCALIBR) {
        Ctrl->top.WinterTop.AsymCalibrOn = 1;
      }
      if (evt[i].Content.mMenuIndex.ObjID == OBJ_IMPORT_TABLE) {
        Import_table_cr = 1;
      }
      if (evt[i].Content.mMenuIndex.ObjID == OBJ_BTN_CR_CALIBR) {
        f_adrv_calibr = 1;
      }
    }
  }

  if (Ctrl -> top.TopType == TOP_WINTER) {
    if (Ctrl->top.WinterTop.AsymCalibrOn) {
      SendToVisuObj(OBJ_BTN_ASYMCALIBR, GS_TO_VISU_SET_ATTR_BLINK, 1);
      SendToVisuObj(OBJ_PROCESSCALIBASYM, GS_TO_VISU_SET_ATTR_VISIBLE, 1);
    } else {
      SendToVisuObj(OBJ_BTN_ASYMCALIBR, GS_TO_VISU_SET_ATTR_BLINK, 0);
      SendToVisuObj(OBJ_PROCESSCALIBASYM, GS_TO_VISU_SET_ATTR_VISIBLE, 0);
    }
  } else if (Ctrl -> top.TopType == TOP_SUMMER) {
    if (1 == f_adrv_calibr) {
      SendToVisuObj(OBJ_BTN_CR_CALIBR, GS_TO_VISU_SET_ATTR_BLINK, 1);
      SendToVisuObj(OBJ_PROCESSCALIBCR, GS_TO_VISU_SET_ATTR_VISIBLE, 1);
    } else {
      SendToVisuObj(OBJ_BTN_CR_CALIBR, GS_TO_VISU_SET_ATTR_BLINK, 0);
      SendToVisuObj(OBJ_PROCESSCALIBCR, GS_TO_VISU_SET_ATTR_VISIBLE, 0);
    }
  }

  Visu_SpreadSettings_Back(VData);
  Visu_HomeKeyPressed();
}
