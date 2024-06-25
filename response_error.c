/****************************************************************************
 *
 * Project:   name
 *
 * @file      filename.c
 * @author    author
 * @date      [Creation date in format %02d.%02d.20%02d]
 *
 * @brief     description
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
#include "response_error.h"
#include "errorlist.h"
#include "param.h"
#include "RCText.h"
#include "RCColor.h"
#include "gsToVisu.h"
#include "gsescrolllist.h"
#include "gse_msgbox.h"
/* macro definitions ********************************************************/

/* type definitions *********************************************************/

/* prototypes ***************************************************************/

/* global constants *********************************************************/

/* global variables *********************************************************/

/* local function prototypes ************************************************/

/* functions ****************************************************************/

typedef struct tagErrorReaction {
	uint32_t ErrorText;
	uint8_t ReactSignal;
	uint8_t ReactDoing;
} tErrorReaction;

tErrorReaction Reacts[] = {
  {.ErrorText = RCTEXT_T_MCM250_1_OFFLINE,.ReactSignal = 1,.ReactDoing = 1 },
  {.ErrorText = RCTEXT_T_MCM250_2_OFFLINE,.ReactSignal = 1,.ReactDoing = 1 },
  {.ErrorText = RCTEXT_T_MCM250_3_OFFLINE,.ReactSignal = 1,.ReactDoing = 1 },
  {.ErrorText = RCTEXT_T_MCM250_4OR250_5_OFFLINE,.ReactSignal = 1,.ReactDoing = 1 },
  {.ErrorText = RCTEXT_T_JOYSTICKOFFLLINE,.ReactSignal = 1,.ReactDoing = 1 },
  {.ErrorText = RCTEXT_T_ENGINEOFFLINE,.ReactSignal = 1,.ReactDoing = 1 },
  {.ErrorText = RCTEXT_T_NOREAGENT,.ReactSignal = 1,.ReactDoing = 1 },
  {.ErrorText = RCTEXT_T_NOCONVEYORROTATION,.ReactSignal = 1,.ReactDoing = 1 },
  {.ErrorText = RCTEXT_T_CHECK_WORKSIDE,.ReactSignal = 1,.ReactDoing = 1 },
  {.ErrorText = RCTEXT_T_CHECKLRSLEFTROADSIGN,.ReactSignal = 1,.ReactDoing = 1 },
  {.ErrorText = RCTEXT_T_CHECKLRSRIGHTROADSIGN,.ReactSignal = 1,.ReactDoing = 1 },
  {.ErrorText = RCTEXT_T_CHECK_FRONTBEACON,.ReactSignal = 1,.ReactDoing = 1 },
  {.ErrorText = RCTEXT_T_CHECKLRSREARBEACON,.ReactSignal = 1,.ReactDoing = 1 },
  {.ErrorText = RCTEXT_T_CHECKLRSREARBEACON,.ReactSignal = 1,.ReactDoing = 1 },
  {.ErrorText = RCTEXT_T_CHECKLRSWORKREAR,.ReactSignal = 1,.ReactDoing = 1 },
  {.ErrorText = RCTEXT_T_CHECKRTRLEFTROADSIGN,.ReactSignal = 1,.ReactDoing = 1 },
  {.ErrorText = RCTEXT_T_CHECKRTRRIGHTROADSIGN,.ReactSignal = 1,.ReactDoing = 1 },
  {.ErrorText = RCTEXT_T_CHECKRTRWORKREAR,.ReactSignal = 1,.ReactDoing = 1 },
  {.ErrorText = RCTEXT_T_CHECKRTRHOPPERTOP,.ReactSignal = 1,.ReactDoing = 1},
  {.ErrorText = RCTEXT_T_OVERSPEED,.ReactSignal = 1,.ReactDoing = 0},
  {.ErrorText = RCTEXT_T_HUMIDIFIER,.ReactSignal = 1,.ReactDoing = 1},
};
static int8_t poseerrnow = 0;
static int8_t poseerrold = 0;
static int8_t posscroll = 0;
static int16_t scrolnow = 0;
static int16_t activereact = 0;

void SetErrorReaction( void ) { //Set in column Signal and Doing variable from array
  static int chanq = -1;
  if (activereact != chanq) {
    SetVar(HDL_ERRORREACTSIGNAL, Reacts[activereact].ReactSignal);
    SetVar(HDL_ERRORREACTDOING, Reacts[activereact].ReactDoing);
  }
  chanq = activereact;
}

void WrErrorReaction( void ) { //Set in array variable from Signal and Doing
  Reacts[activereact].ReactSignal = GetVar(HDL_ERRORREACTSIGNAL);
  Reacts[activereact].ReactDoing = GetVar(HDL_ERRORREACTDOING);
}

void WrInFileReact( void ) { //Write Struct in file
  tGsFile * fp = FileOpen(PATH_ERRORREACT_FLASH, "wb");
  FileWrite( & Reacts, sizeof(Reacts), 1, fp);
  FileClose(fp);
}

void ReadErrorReaction( void ) { //Read in struct from file
  tGsFile * fp = FileOpen(PATH_ERRORREACT_FLASH, "rb");
  FileRead( & Reacts, sizeof(Reacts), 1, fp);
  FileClose(fp);
}

//******************************************************************************************
// Backup
//******************************************************************************************

// Перечисление заголовков бэкапа для читабельности
enum {
  RCTEXT = 0,
  SIGNAL,
  DOING,
};
// Заголовки бэкапа
char errorBackupHeader[][128] = {
  {"RCText = "},
  {"Сигнал = "},
  {"Действие = "},
};
// Размер буфера чтения бэкапа
#define READ_BUFFER_SIZE 256

void ErrorReaction_Export_Backup( void ) {
  // Открытия файла для записи
  tGsFile * fp = FileOpen(PATH_ERRORREACT_USB_BACKUP, "w+");
  if(fp == NULL) {
    return;
  }

  // Построчная запись всех структур массива Reacts
  for(int i = 0; i < GS_ARRAYELEMENTS(Reacts); i++) {
    FilePrintf(fp, "%s%d ", errorBackupHeader[RCTEXT], Reacts[i].ErrorText);
    FilePrintf(fp, "%s%d ", errorBackupHeader[SIGNAL], Reacts[i].ReactSignal);
    FilePrintf(fp, "%s%d ", errorBackupHeader[DOING], Reacts[i].ReactDoing);
    FilePrintf(fp, "\n");
  }

  FileClose(fp);
}

/**
 * @brief Поиск следующего символа после полностью совпадающего с header участка buf
 *
 * @param buf Строка, где ищется участок
 * @param header Участок, за которым нужно найти значения
 * @return char*
 */
char * ErrorReaction_Backup_GetValPtrByHeader(const char * buf, const char * header) {
  if(buf == NULL || header == NULL) {
    return NULL;
  }

  char * pos = strstr(buf, header);
  if(pos) {
    return pos + strlen(header);
  }

  return NULL;
}

void ErrorReaction_Import_Backup(void) {
  if (FileSize(PATH_ERRORREACT_USB_BACKUP) > 0) {
    // Открытие файла на чтение, выделение под это буфера
    tGsFile * fp = FileOpen(PATH_ERRORREACT_USB_BACKUP, "r");
    if(fp == NULL) {
      return;
    }
    char buffer[READ_BUFFER_SIZE];

    // Пока читаются строчки из файла
    // Ищем в строке значение за хедером для RCText'а, а само это значение в нашем массиве реакций
    // Если нашлось - записываем в найденную структуру значение из файла
    while(FileGets(buffer, READ_BUFFER_SIZE, fp)) {
      int strRCtext = atoi(ErrorReaction_Backup_GetValPtrByHeader(buffer, errorBackupHeader[RCTEXT]));
      for(int i = 0; i < GS_ARRAYELEMENTS(Reacts); i++) {
        if(Reacts[i].ErrorText == strRCtext) {
          Reacts[i].ReactDoing = atoi(ErrorReaction_Backup_GetValPtrByHeader(buffer, errorBackupHeader[DOING]));
          Reacts[i].ReactSignal = atoi(ErrorReaction_Backup_GetValPtrByHeader(buffer, errorBackupHeader[SIGNAL]));

          break;
        }
      }
    }

     FileClose(fp);
  // Если фалйа нет - выдаем сообщение
  } else {
    char stringinf[100];
    snprintf(stringinf, GS_ARRAYELEMENTS(stringinf), "%s %s", RCTextGetText(RCTEXT_T_FILENOTFOUND, GetVarIndexed(IDX_SYS_LANGUAGE)), "errorreact.cfg");
    MsgBoxOk(RCTextGetText(RCTEXT_T_INFO, GetVarIndexed(IDX_SYS_LANGUAGE)), stringinf, NULL, NULL);
  }
}

//******************************************************************************************
// /Backup
//******************************************************************************************

char * ReactTxt(int i) { //Get ErrID
  return RCTextGetText(Reacts[i].ErrorText, GetVarIndexed(IDX_SYS_LANGUAGE));
}

int ReactSignalInError(int i) { //Get ReactSignal
  return Reacts[i].ReactSignal;
}

int ReactDoingInError(int i) { //Get ReactDoing
  return Reacts[i].ReactDoing;
}

int SizeOfReact( void ) { //Get Size array Reacts
  return GS_ARRAYELEMENTS(Reacts);
}

void ErrColorSet( void ) { //Set Color for new txt obj
  int colrerrs[] = {
    OBJ_ATRESTTTEXTLIST703,
    OBJ_ATRESTTTEXTLIST714,
    OBJ_ATRESTTTEXTLIST766,
    OBJ_ATRESTTTEXTLIST1147,
    OBJ_ATRESTTTEXTLIST1148,
    OBJ_ATRESTTTEXTLIST1149,
  };

  if (poseerrnow != poseerrold) {
    SendToVisuObj(colrerrs[poseerrnow], GS_TO_VISU_SET_FG_COLOR, RCColorGetColor(GetVarIndexed(IDX_SYS_COLOR), RCCOLOR_NAME_FRONT));
    SendToVisuObj(colrerrs[poseerrold], GS_TO_VISU_SET_FG_COLOR, RCColorGetColor(GetVarIndexed(IDX_SYS_COLOR), RCCOLOR_NAME_FRAMECOLORUNSELECT));
    poseerrold = poseerrnow;
  }
}

static int8_t nominalfeedback = 0;
void SelectChangeErr(int nominlflag) { //Change Select Error or Doing and Signal
  nominalfeedback = nominlflag;
  int reacterr[] = {
    OBJ_ATRESTTTEXTLIST761,
    OBJ_ATRESTTTEXTLIST1205,
  };
  SendToVisuObj(OBJ_SIMPLEBITMAP762, GS_TO_VISU_SET_ATTR_VISIBLE, 1 - nominlflag);
  SendToVisuObj(OBJ_SIMPLEBITMAP1164, GS_TO_VISU_SET_ATTR_VISIBLE, nominlflag);
  for (int i = 0; i < 2; i++) {
    SendToVisuObj(reacterr[i], GS_TO_VISU_SET_ATTR_NOMINALFLAG, nominlflag);
    SendToVisuObj(reacterr[0], GS_TO_VISU_OBJ_SELECT, nominlflag);
  }
}

void CheckElipsePosition( void ) { //Slider Position
  if (posscroll > scrolnow && poseerrnow >= 1) {
    poseerrnow = poseerrnow - 1;
    SendToVisuObj(OBJ_SIMPLEBITMAP762, GS_TO_VISU_SET_ORIGIN_Y, 91 + 47 * poseerrnow);
  } else if (posscroll < scrolnow && poseerrnow <= 4) {
    poseerrnow = poseerrnow + 1;
    SendToVisuObj(OBJ_SIMPLEBITMAP762, GS_TO_VISU_SET_ORIGIN_Y, 91 + 47 * poseerrnow);
  } else if (scrolnow <= 0) {
    poseerrnow = 0;
  } else if (scrolnow >= SizeOfReact() - 1) {
    poseerrnow = 5;
  }

  posscroll = scrolnow;
}

void ChangeErrorColumn( void ) { //Set Text for new txt obj
  int column[] = {
    HDL_CHANGEERR_COLUMN_0,
    HDL_CHANGEERR_COLUMN_1,
    HDL_CHANGEERR_COLUMN_2,
    HDL_CHANGEERR_COLUMN_3,
    HDL_CHANGEERR_COLUMN_4,
    HDL_CHANGEERR_COLUMN_5,
  };
  for (int i = 0; i < GS_ARRAYELEMENTS(column); i++) {
    SetVar(column[i], scrolnow + i - poseerrnow);
    activereact = scrolnow;
  }
}

void CheckScrollPosition( void ) { //Check events
  if ((IsKeyPressedNew(104) || IsKeyPressedNew(11)) && !nominalfeedback && scrolnow < SizeOfReact() - 1) {
    scrolnow = scrolnow + 1;
    CheckElipsePosition();
    ErrColorSet();
  }
  if ((IsKeyPressedNew(103) || IsKeyPressedNew(8)) && !nominalfeedback && scrolnow > 0) {
    scrolnow = scrolnow - 1;
    CheckElipsePosition();
    ErrColorSet();
  }

  if ((IsKeyPressedNew(12) || IsKeyPressedNew(105)) && !IsInfoContainerOn(CNT_CHANGEERRORREAKTIONSIGNAL) &&
    !IsInfoContainerOn(CNT_CHANGEERRORREAKTIONDOING) && !nominalfeedback) {
    SelectChangeErr(1);
  }
}

void Error_Signal_Open(const tVisuData * VData) {
  PrioMaskOn(MSK_CHANGEERROR);
}

void Error_Signal_Init(const tVisuData * VData) {
  ReadErrorReaction();
}

void State_SelectError(int state) {
  SetVarIndexed(IDX_SELECTSIGNAL, 0);
  WrErrorReaction();
  SendToVisuObj(OBJ_ATRESTTTEXTLIST761, GS_TO_VISU_SET_ATTR_GRABFOCUS, state);
  SendToVisuObj(OBJ_ATRESTTTEXTLIST761, GS_TO_VISU_OBJ_SELECT, state);
  SendToVisuObj(OBJ_ATRESTTTEXTLIST1205, GS_TO_VISU_SET_ATTR_GRABFOCUS, 1 - state);
  SendToVisuObj(OBJ_ATRESTTTEXTLIST1205, GS_TO_VISU_OBJ_SELECT, 1 - state);
  SendToVisuObj(OBJ_SIMPLEBITMAP1164, GS_TO_VISU_SET_ATTR_VISIBLE, 1);
}

void Error_Signal_Cycle(const tVisuData * VData, uint32_t evtc, tUserCEvt * evtv) {
  if (!IsInfoContainerOn(CNT_CHANGEERRORREAKTIONSIGNAL) && !IsInfoContainerOn(CNT_CHANGEERRORREAKTIONDOING)) {
    CheckScrollPosition();
    ChangeErrorColumn();
    SetErrorReaction();
    if (IsKeyPressedNew(1)) {
      if (nominalfeedback == 1) {
        SelectChangeErr(0);
        SendToVisuObj(OBJ_SIMPLEBITMAP1164, GS_TO_VISU_SET_ORIGIN_Y, 112);
        WrErrorReaction();
        // ReadErrorReaction();
      } else {
        WrInFileReact();
        // ReadErrorReaction();
        Visu_OpenScreen(SCREEN_PROPERTIES);
      }
    }
    if (IsKeyPressedNew(2)) {
      WrInFileReact();
      // ReadErrorReaction();
    }
    if (IsKeyPressedNew(10)) {
      WrInFileReact();
      // ReadErrorReaction();
      Visu_OpenScreen(SCREEN_MAIN);
    }
  } else {
    if (IsKeyPressedNew(1)) {
      SendToVisuObj(OBJ_SIMPLEBITMAP1164, GS_TO_VISU_SET_ATTR_VISIBLE, 1);
      WrErrorReaction();
      // ReadErrorReaction();
      InfoContainerOff(CNT_CHANGEERRORREAKTIONSIGNAL);
      InfoContainerOff(CNT_CHANGEERRORREAKTIONDOING);
      SendToVisuObj(OBJ_ATRESTTTEXTLIST761, GS_TO_VISU_SET_ATTR_GRABFOCUS, 1);
      SendToVisuObj(OBJ_ATRESTTTEXTLIST761, GS_TO_VISU_OBJ_SELECT, 1);
    }
    if (IsKeyPressedNew(2)) {
      WrInFileReact();
      // ReadErrorReaction();
    }
    if (IsKeyPressedNew(10)) {
      InfoContainerOff(CNT_CHANGEERRORREAKTIONSIGNAL);
      InfoContainerOff(CNT_CHANGEERRORREAKTIONDOING);
      SelectChangeErr(0);
      WrInFileReact();
      Visu_OpenScreen(SCREEN_MAIN);
    }
  }
  int ReactionDoing[] = {
    OBJ_ATRESTTTEXTLIST1227,
    OBJ_ATRESTTTEXTLIST1226,
    OBJ_ATRESTTTEXTLIST1225,
  };

  int ReactionSignal[] = {
    OBJ_ATRESTTTEXTLIST1212,
    OBJ_ATRESTTTEXTLIST1211,
    OBJ_ATRESTTTEXTLIST1210,
  };

  int settxtobj[] = {
    HDL_ERRDOING_LINE1,
    HDL_ERRDOING_LINE2,
    HDL_ERRDOING_LINE3,
  };
  int txtpos = 0;
  for (uint32_t i = 0; i < evtc; i++) {
    if (CEVT_SELECTED == evtv[i].Type) {
      int32_t ObjId = evtv[i].Content.mSelected.ObjID;
      if (IsInfoContainerOn(CNT_CHANGEERRORREAKTIONSIGNAL) || IsInfoContainerOn(CNT_CHANGEERRORREAKTIONDOING)) {
        for (int i = 0; i < 3; i++) {
          if (ObjId == ReactionDoing[i] || ObjId == ReactionSignal[i]) {
            SetVarIndexed(IDX_SELECTSIGNAL, i);
          }
        }

      } else {
        if (ObjId == OBJ_ATRESTTTEXTLIST761) {
          SendToVisuObj(OBJ_SIMPLEBITMAP1164, GS_TO_VISU_SET_ORIGIN_Y, 112);
        } else if (ObjId == OBJ_ATRESTTTEXTLIST1205) {
          SendToVisuObj(OBJ_SIMPLEBITMAP1164, GS_TO_VISU_SET_ORIGIN_Y, 203);
        }
      }
    }

    if (CEVT_EDIT_START == evtv[i].Type) {
      int32_t ObjId = evtv[i].Content.mEditStart.ObjID;
      if (!IsInfoContainerOn(CNT_CHANGEERRORREAKTIONSIGNAL) && !IsInfoContainerOn(CNT_CHANGEERRORREAKTIONDOING)) {
        if (ObjId == OBJ_ATRESTTTEXTLIST1205) {

          for (int i = 0; i < 4; i++) {
            if (i != GetVar(HDL_ERRORREACTSIGNAL)) {
              SetVar(settxtobj[txtpos], i);
              txtpos = txtpos + 1;
            }
          }
          SendToVisuObj(OBJ_SIMPLEBITMAP1164, GS_TO_VISU_SET_ATTR_VISIBLE, 0);
          InfoContainerOn(CNT_CHANGEERRORREAKTIONSIGNAL);
        }
        if (ObjId == OBJ_ATRESTTTEXTLIST761 && activereact != SizeOfReact()-2) {
          for (int i = 0; i < 4; i++) {
            if (i != GetVar(HDL_ERRORREACTDOING)) {
              SetVar(settxtobj[txtpos], i);
              txtpos = txtpos + 1;
            }
          }
          SendToVisuObj(OBJ_SIMPLEBITMAP1164, GS_TO_VISU_SET_ATTR_VISIBLE, 0);
          InfoContainerOn(CNT_CHANGEERRORREAKTIONDOING);
        }
        else if(ObjId == OBJ_ATRESTTTEXTLIST761 && activereact == SizeOfReact()-2){
        SendToVisuObj(OBJ_ATRESTTTEXTLIST761, GS_TO_VISU_SET_ATTR_UPDATEONEDIT, 0);
      SendToVisuObj(OBJ_ATRESTTTEXTLIST761, GS_TO_VISU_SET_ATTR_GRABFOCUS, 0);
      SendToVisuObj(OBJ_ATRESTTTEXTLIST761, GS_TO_VISU_OBJ_SELECT, 0);
        }
      } else {
        for (int i = 0; i < 3; i++) {
          if (ObjId == ReactionSignal[i]) {
            SetVar(HDL_ERRORREACTSIGNAL, GetVar(settxtobj[i]));
            InfoContainerOff(CNT_CHANGEERRORREAKTIONSIGNAL);
            State_SelectError(0);
          }
          if (ObjId == ReactionDoing[i]) {
            SetVar(HDL_ERRORREACTDOING, GetVar(settxtobj[i]));
            InfoContainerOff(CNT_CHANGEERRORREAKTIONDOING);
            State_SelectError(1);
          }
        }
      }
    }
  }
}
