#include <UserCEvents.h>
#include <UserCAPI.h>
#include "vartab.h"
#include "objtab.h"
#include "profile.h"
#include "visu_profile.h"
#include "gsToVisu.h"
#include "gse_msgbox.h"
#include "RCText.h"
#include "param.h"

uint32_t ProfileTextList[8] = {
  OBJ_PROFILENAME_1,
  OBJ_PROFILENAME_2,
  OBJ_PROFILENAME_3,
  OBJ_PROFILENAME_4,
  OBJ_PROFILENAME_5,
  OBJ_PROFILENAME_6,
  OBJ_PROFILENAME_7,
  OBJ_PROFILENAME_8
};

static uint32_t _offset = 0;

static tProfile * _prof_edit;
static tProfile * _prof_select;

void Profile_Change_Draw(void) {
  SetVisObjData(OBJ_PROFILECHANGEACTUALPROFILE, Profile_GetCurrentProfile() -> name, strlen(Profile_GetCurrentProfile() -> name) + 1);
  tGSList * List = Profile_GetProfileList();
  //read first profile;
  tProfile * Profile = GSList_GetFirstData(List);
  uint32_t i;
  for (i = 0; i < GS_ARRAYELEMENTS(ProfileTextList); i++) {
    if (NULL == Profile) {
      break;
    }
    SetVisObjData(ProfileTextList[i], Profile -> name, strlen(Profile -> name) + 1);
    Profile = GSList_GetNextData(List);
    SendToVisuObj(ProfileTextList[i], GS_TO_VISU_SET_ATTR_ALL, ATTR_VISIBLE | ATTR_TRANSPARENT | ATTR_NOMINAL);

  }
  //fill empty entries;
  for (; i < GS_ARRAYELEMENTS(ProfileTextList); i++) {
    SetVisObjData(ProfileTextList[i], " ", strlen(" ") + 1);
    SendToVisuObj(ProfileTextList[i], GS_TO_VISU_SET_ATTR_ALL, 0);
  }
}

void Visu_Profile_Open(const tVisuData * VData) {
  PrioMaskOn(MSK_PROFILE_CHANGE);
  Profile_Change_Draw();
}

tProfile * Visu_Profile_GetSelectedProfile(uint32_t line) {
  tGSList * List = Profile_GetProfileList();
  tProfile * Profile = GSList_GetFirstData(List);
  line = line + _offset;
  if (line >= List -> numItems)
    return NULL;
  for (uint32_t k = 0; k < line; k++) {
    Profile = GSList_GetNextData(List);
    if (Profile == NULL) {
      break;
    }
  }
  return Profile;

}

void Visu_Profile_Add(void) {
  tGSList * List = Profile_GetProfileList();
  GSList_GetLastData(List);
  tProfile Profile;
  memset( & Profile, 0, sizeof(tProfile));
  strncpy(Profile.name, RCTextGetText(RCTEXT_T_NEWPROFILE, GetVarIndexed(IDX_SYS_LANGUAGE)), sizeof(Profile.name));
  GSList_AddData(List, & Profile, sizeof(tProfile));
  Profile_Change_Draw();
}

void Visu_ProfileDel(void) {
  if (NULL != _prof_select) {
    tGSList * List = Profile_GetProfileList();
    GSList_DeleteData(List, _prof_select);
    if (_prof_select == Profile_GetCurrentProfile()) {
      Prof_SetCurrentProfile(GSList_GetFirstData(List));
    }
    _prof_select = NULL;
    Profile_Change_Draw();
  }
}

void Visu_Profile_OpenEdit_cb(void * arg) {
  Visu_OpenScreen(SCREEN_PROFILE_EDIT);
}

void Visu_Profile_MsgBoxNewProfile_cb(void * arg) {
  Prof_SetCurrentProfile(_prof_select);
  Profile_Change_Draw();
  int32_t ProfIdx = Profile_GetIndex(_prof_select);
  if (-1 < ProfIdx)
    SetVarIndexed(IDX_CURRENTPROFILE, ProfIdx); //saves the idx of the current profile as remanent variable
  q_chg_profile = 0;
}
/*
 */
void Visu_Profile_Cycle(const tVisuData * VData, uint32_t evtc, tUserCEvt * evtv) {
  Visu_HomeKeyPressed();
  for (uint32_t i = 0; i < evtc; i++) {
    if (CEVT_MENU_INDEX == evtv[i].Type) {
      for (uint32_t j = 0; j < GS_ARRAYELEMENTS(ProfileTextList); j++) {
        if (ProfileTextList[j] == evtv[i].Content.mMenuIndex.ObjID) {

          tProfile * Profile = Visu_Profile_GetSelectedProfile(j);
          if (NULL != Profile) {
            const char * Head = RCTextGetText(RCTEXT_T_CHANGEPROFILE, GetVarIndexed(IDX_SYS_LANGUAGE));
            char text[128];
            snprintf(text, sizeof(text), RCTextGetText(RCTEXT_T_USETHISPROFILE, GetVarIndexed(IDX_SYS_LANGUAGE)), Profile -> name);
            MsgBoxOkCancel(Head, text, Visu_Profile_MsgBoxNewProfile_cb, NULL, NULL, NULL);
          }
        }
      }
    } else if (CEVT_SELECTED == evtv[i].Type) {
      for (uint32_t j = 0; j < GS_ARRAYELEMENTS(ProfileTextList); j++) {
        if (ProfileTextList[j] == evtv[i].Content.mSelected.ObjID) {
          tProfile * Profile = Visu_Profile_GetSelectedProfile(j);
          if (NULL != Profile) {
            _prof_select = Profile;
            break;
          }
        }
      }
    }
  }

  // Уровень доступа Админ (только просмотр и импорт/экспорт профилей)
  if (GetVarIndexed(IDX_ATT_USER_ADMIN)) {
    if (IsKeyPressedNew(4)) {
      if (NULL != _prof_select) {
        _prof_edit = _prof_select;
        Visu_OpenScreen(SCREEN_PROFILE_EDIT);
      }
    }
  }

  if (GetVarIndexed(IDX_ATTR_USER_MERKATOR)) {
    if (IsKeyPressedNew(1)) {
      Visu_Profile_Add();
    }
    if (IsKeyPressedNew(2)) {
      Visu_ProfileDel();
    }
    if (IsKeyPressedNew(3)) {
      Prof_Save();
    }
    if (IsKeyPressedNew(4)) {
      if (NULL != _prof_select) {
        _prof_edit = _prof_select;
        Visu_OpenScreen(SCREEN_PROFILE_EDIT);
        q_chg_profile |= PROF_CHG;
      }
    }
    static uint8_t flagShowCheckEntry = 0;
    if (!flagShowCheckEntry) {
      SendToVisuObj(OBJ_ATRESTTTEXT1255, GS_TO_VISU_SET_ATTR_VISIBLE, 0);
      flagShowCheckEntry = 1;
    }
  }

  if (IsKeyPressedNew(5)) {
    if (1 == Prof_CopyToUsb()) {
      MsgBoxOK_RCText(RCTEXT_T_INFO, RCTEXT_T_COPYTOUSBSUCCESS, NULL, NULL);
    } else MsgBoxOK_RCText(RCTEXT_T_INFO, RCTEXT_T_COPYTOUSBERROR, NULL, NULL);
    q_chg_profile |= PROF_EXP;
  }
  if (IsKeyPressedNew(6)) {
    if (1 == Prof_LoadFromUsb()) {
      MsgBoxOK_RCText(RCTEXT_T_INFO, RCTEXT_T_COPYFROMUSBSUCCESS, NULL, NULL);
    } else MsgBoxOK_RCText(RCTEXT_T_INFO, RCTEXT_T_COPYFROMUSBERROR, NULL, NULL);
    Prof_Load();
    Profile_Change_Draw();
    q_chg_profile |= PROF_IMP;
  }
}

void Visu_ProfileEdit_ToVisu(void) {
  if (NULL != _prof_edit) {
    SetVarIndexed(IDX_ATTACH_TOP, _prof_edit -> module[0]);
    SetVarIndexed(IDX_ATTACH_A, _prof_edit -> module[1]);
    SetVarIndexed(IDX_ATTACH_B, _prof_edit -> module[2]);
    SetVarIndexed(IDX_ATTACH_C, _prof_edit -> module[3]);
    SetVisObjData(OBJ_PROFILEEDITNAME, _prof_edit -> name, strlen(_prof_edit -> name) + 1);
  }
}

void Visu_ProfileEdit_Save(void) {
  if (NULL != _prof_edit) {
    _prof_edit -> module[0] = GetVarIndexed(IDX_ATTACH_TOP);
    _prof_edit -> module[1] = GetVarIndexed(IDX_ATTACH_A);
    _prof_edit -> module[2] = GetVarIndexed(IDX_ATTACH_B);
    _prof_edit -> module[3] = GetVarIndexed(IDX_ATTACH_C);
    GetVisObjData(OBJ_PROFILEEDITNAME, _prof_edit -> name, sizeof(_prof_edit -> name));
  }
  Prof_Save();
}

void Visu_ProfileEdit_Open(const tVisuData * VData) {
  Visu_ProfileEdit_ToVisu();
  PrioMaskOn(MSK_PROFILEEDIT);
}

void Visu_ProfileEdit_Cycle(const tVisuData * VData, uint32_t evtc, tUserCEvt * evtv) {
  // Уровень доступа админ. Запрет изменения названия профиля
  if (GetVarIndexed(IDX_ATT_USER_ADMIN)) {
    SendToVisuObj(OBJ_PROFILEEDITNAME, GS_TO_VISU_SET_ATTR_NOMINALFLAG, 0);
  }

  if (GetVarIndexed(IDX_ATTR_USER_MERKATOR)) {
    SendToVisuObj(OBJ_PROFILEEDITNAME, GS_TO_VISU_SET_ATTR_NOMINALFLAG, 1);
    if (IsKeyPressedNew(3)) {
      if (!MsgBox_IsAnyBoxActive()) {
        MsgBoxOkCancel(RCTextGetText(RCTEXT_T_CHANGEPROFILE, GetVarIndexed(IDX_SYS_LANGUAGE)),
                       RCTextGetText(RCTEXT_T_CHANGEPROFILETEXT, GetVarIndexed(IDX_SYS_LANGUAGE)),
                       Visu_ProfileEdit_Save, NULL, NULL, NULL);
      }
    }

    // Конфигурация ПС
    if (GetVarIndexed(IDX_ATTACH_TOP) == 1) { // Первый профиль RTR
      SendToVisuObj(OBJ_SIMPLEBITMAP194, GS_TO_VISU_SET_ATTR_VISIBLE, 1);
      SendToVisuObj(OBJ_ATRESTTTEXT386, GS_TO_VISU_SET_ATTR_VISIBLE, 1);
      if (IsKeyPressedNew(6)) {
        Visu_OpenScreen(SCREEN_PROFILE_EDIT_RTR);
      }
    } else {
      SendToVisuObj(OBJ_SIMPLEBITMAP194, GS_TO_VISU_SET_ATTR_VISIBLE, 0);
      SendToVisuObj(OBJ_ATRESTTTEXT386, GS_TO_VISU_SET_ATTR_VISIBLE, 0);
    }
  }

  if (IsKeyPressedNew(1)) {
    Visu_OpenScreen(SCREEN_PROFILE);
  }

  Visu_HomeKeyPressed();
}

void Set_RTR_Config( void ) {
  static uint8_t oldRTRType = 0;
  if (oldRTRType != GetVarIndexed(IDX_RTR_CONFIGTYPE)) {
    oldRTRType = GetVarIndexed(IDX_RTR_CONFIGTYPE);

    switch (GetVarIndexed(IDX_RTR_CONFIGTYPE)) {
    case 1: // KH
      SetVarIndexed(IDX_RTR_CONFIGTYPE, 1);
      SetVarIndexed(IDX_RTR_CONFIGASYM, 0);
      SetVarIndexed(IDX_RTR_CONFIGHUMID, 0);
      break;
    case 2: // SH
      SetVarIndexed(IDX_RTR_CONFIGTYPE, 2);
      SetVarIndexed(IDX_RTR_CONFIGASYM, 1);
      SetVarIndexed(IDX_RTR_CONFIGHUMID, 1);
      break;

    case 3: // Sorgaz
      SetVarIndexed(IDX_RTR_CONFIGTYPE, 3);
      SetVarIndexed(IDX_RTR_CONFIGASYM, 2);
      SetVarIndexed(IDX_RTR_CONFIGHUMID, 2);
      break;

    case 4: // UH
      SetVarIndexed(IDX_RTR_CONFIGTYPE, 4);
      SetVarIndexed(IDX_RTR_CONFIGASYM, 0);
      SetVarIndexed(IDX_RTR_CONFIGHUMID, 0);
      break;

    case 5: // UH
      SetVarIndexed(IDX_RTR_CONFIGTYPE, 5);
      SetVarIndexed(IDX_RTR_CONFIGASYM, 0);
      SetVarIndexed(IDX_RTR_CONFIGHUMID, 0);
      break;
    default:
      break;
    }
  }
}

void Visu_RTR_Edit_Open(const tVisuData * VData) {
  tParamData * Param = ParameterGet();
  PrioMaskOn(MSK_PROFILEEDIT_RTR);

  SetVarIndexed(IDX_RTR_CONFIGTYPE, Param->RTR.TypeRTR);
  SetVarIndexed(IDX_RTR_CONFIGASYM, Param->RTR.AsymType);
  SetVarIndexed(IDX_RTR_CONFIGHUMID, Param->RTR.HumidType);
}

void RTR_Config_Save( void ) {
  tParamData * Param = ParameterGet();
  Param->RTR.TypeRTR = GetVarIndexed(IDX_RTR_CONFIGTYPE);
  Param->RTR.AsymType = GetVarIndexed(IDX_RTR_CONFIGASYM);
  Param->RTR.HumidType = GetVarIndexed(IDX_RTR_CONFIGHUMID);
  writeParameters();
}

void Visu_RTR_Edit_Cycle(const tVisuData * VData, uint32_t evtc, tUserCEvt * evtv) {
  if (IsKeyPressedNew(1)) {
    Visu_OpenScreen(SCREEN_PROFILE_EDIT);
  }
  if (IsKeyPressedNew(3)) {
    if (!MsgBox_IsAnyBoxActive()) {
      MsgBoxOkCancel(RCTextGetText(RCTEXT_T_SAVERTRCONFIG, GetVarIndexed(IDX_SYS_LANGUAGE)),
        RCTextGetText(RCTEXT_T_SAVERTRCONFIGTEXT, GetVarIndexed(IDX_SYS_LANGUAGE)),
        RTR_Config_Save, NULL, NULL, NULL);
    }
  }

  Set_RTR_Config();

  Visu_HomeKeyPressed();
}
