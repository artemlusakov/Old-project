/****************************************************************************
*
* File:         VISU_ERROR.c
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
#include "gseerrorlist.h"
#include "visu.h"
#include "RCText.h"
#include "gse_msgbox.h"
#include "gsToVisu.h"
#include <string.h>
#include "visu_statistik.h"
#include "RCColor.h"
#include "gsToVisu.h"
/****************************************************************************/

/* macro definitions ********************************************************/

/* type definitions *********************************************************/

/* prototypes ***************************************************************/

/* global constants *********************************************************/

/* global variables *********************************************************/
uint8_t LearningError;
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
void Visu_Error_Init( const tVisuData* VData ) {
	ErrDraw_AddLine( 0, OBJ_ERRORLINE_0, HDL_ERRCHECK_0 );
	ErrDraw_AddLine( 0, OBJ_ERRORLINE_1, HDL_ERRCHECK_1 );
	ErrDraw_AddLine( 0, OBJ_ERRORLINE_2, HDL_ERRCHECK_2 );
	ErrDraw_AddLine( 0, OBJ_ERRORLINE_3, HDL_ERRCHECK_3 );
	ErrDraw_AddLine( 0, OBJ_ERRORLINE_4, HDL_ERRCHECK_4 );
	ErrDraw_AddLine( 0, OBJ_ERRORLINE_5, HDL_ERRCHECK_5 );
	ErrDraw_AddLine( 0, OBJ_ERRORLINE_6, HDL_ERRCHECK_6 );
	ErrDraw_AddLine( 0, OBJ_ERRORLINE_7, HDL_ERRCHECK_7 );
	ErrDraw_AddLine( 0, OBJ_ERRORLINE_8, HDL_ERRCHECK_8 );
	ErrDraw_AddLine( 0, OBJ_ERRORLINE_9, HDL_ERRCHECK_9 );
	ErrDraw_InitSetFrameObj( OBJ_ERRORFRAME );
	ErrDraw_InitErrorFrame( 58, 36, 50, 700 );
	ErrDraw_SetTextColor( ES_NOERROR, 0x005500 );
	ErrDraw_SetTextColor( ES_UNCHECKED_ERROR_ACTIVE, 0xEE2222 );
	ErrDraw_SetTextColor( ES_UNCHECKED_ERROR_INACTIVE, 0x7F9646 );
	ErrDraw_SetTextColor( ES_CHECKED_ERROR_ACTIVE, 0x4F81BD );
	ErrDraw_SetTextColor( ES_CHECKED_ERROR_INACTIVE, 0xAAAAAA );
	//EList_SetPrintErrorOut((tGsPrintErrOutCallback)db_out);

}

void Visu_Error_Open( const tVisuData* VData ) {
	//ErrDraw_Draw();
	PrioMaskOn( MSK_ERROR );
	LearningError = 0;
}

void Visu_Error_Close( const tVisuData* VData ) {
	MaskOff( MSK_ERROR );
	SetVarIndexed( IDX_LEARNING, 0 );
	InfoContainerOff( CNT_LEARNINGERROR );
}

void Visu_Error_Back( const tVisuData* VData ) {
	uint8_t isLearningMode = 0;
	if( IsKeyPressedNew( 1 ) ) {
		if( GetVar( HDL_LEARNING ) == 1 ) {
			isLearningMode = 1;
		}
		Visu_Error_Close( VData );
		Visu_OpenScreen( SCREEN_MAIN );
		MsgContainerOn( CNT_MAINMENU );
	}
	if( isLearningMode == 1 ) {
		SetVar( HDL_LEARNING, 1 );
		isLearningMode = 0;
	}
}

void Visu_Error_Learning( void ) {
	static int32_t count = 0;
	count++;
	if( GetVar( HDL_LEARNING ) ) {
		if( 0 == (count % 20) ) {
			if( LearningError == 0 ) {
				InfoContainerOn( CNT_LEARNINGERROR );
			}
		}
	}
}

/**************************************************
* Отображение контейнера для просмотра файла ошибок
* и моточасов. Вывод информации из файла.
**************************************************/
int8_t press = 0;
int otsechqa = -1; //this perem need for start write new upper limit
int otsechqakol = 0; // count all \n
int massotsech[20000]; // array for move up or down
int otsechqalast = 0; // last \n
int8_t stop = 0; //stop otsechqa
int8_t reed = 0; // stop start reed
char bufferfo[1000000]; //buffer for reed file
int DateTM[611331]; //date month and year count by -  (year * 10000 + month * 100 + day)
uint32_t SizeFil = 0; //File size now
int8_t per = 0;
int16_t _Load(void) {
  //Open File with configturation.
  tGsFile * fp = FileOpen(PATH_ERROR_FLASH, "rb");
  SizeFil = FileSize(PATH_ERROR_FLASH) + 100;
  otsechqalast = 0;
  // stop sign for write in array
  char del[] = {
    "\n"
  };
  //memset - for free mem perem
  memset(bufferfo, 0, sizeof(bufferfo));
  memset(DateTM, 0, sizeof(DateTM));
  memset(massotsech, 0, sizeof(massotsech));
  FileRead( & bufferfo, 1, sizeof(bufferfo), fp);
  for (int64_t i = 0; i < SizeFil - 100; i++) {
    if (bufferfo[i] == del[0]) {
      if (otsechqakol == 0) {
        massotsech[otsechqakol] = -1;
        otsechqakol = otsechqakol + 1;
        massotsech[otsechqakol] = i;
        otsechqakol = otsechqakol + 1;
      } else {
        //buffer for transfer from char to int
        char SearchBufD[3];
        char SearchBufM[3];
        char SearchBufY[3];
        massotsech[otsechqakol] = i;
        for (int h = 0; h < 9; h++) {
          if (h >= 0 && h < 3) {
            SearchBufD[h] = bufferfo[i + h + 1];
          }
          if (h >= 4 && h < 6) {
            SearchBufM[h - 4] = bufferfo[i + h + 1];
          }
          if (h >= 7) {
            SearchBufY[h - 7] = bufferfo[i + h + 1];
          }
        }
        int srD = atoi(SearchBufD);
        int srM = atoi(SearchBufM);
        int srY = atoi(SearchBufY);
        int fulltime = srY * 10000 + srM * 100 + srD;
        if (DateTM[fulltime] == NULL) {
          DateTM[fulltime] = otsechqakol;
        }
        memset(SearchBufD, 0, sizeof(SearchBufD));
        memset(SearchBufM, 0, sizeof(SearchBufM));
        memset(SearchBufY, 0, sizeof(SearchBufY));
        otsechqakol = otsechqakol + 1;
      }
      otsechqalast = otsechqakol;
    }
  }
  FileClose(fp);
  return 0;
}

//array line obj
int arrsize = 0;
int m_obj[] = {
  OBJ_LINEFILE_1,
  OBJ_LINEFILE_2,
  OBJ_LINEFILE_3,
  OBJ_LINEFILE_4,
  OBJ_LINEFILE_5,
  OBJ_LINEFILE_6,
  OBJ_LINEFILE_7,
  OBJ_LINEFILE_8,
  OBJ_LINEFILE_9,
  OBJ_LINEFILE_10,
};
int l_obj[] = {
  OBJ_1_LINE,
  OBJ_2_LINE,
  OBJ_3_LINE,
  OBJ_4_LINE,
  OBJ_5_LINE,
  OBJ_6_LINE,
  OBJ_7_LINE,
  OBJ_8_LINE,
  OBJ_9_LINE,
  OBJ_10_LINE,
};

int32_t currsize = 0;
int16_t _Load_Reed(void) {
  if (reed == 0) {
    char viewline[currsize];
    int Summ = massotsech[otsechqakol + arrsize];
    char del[] = {
      "\n"
    };
    int j = 0;
    int checkfull = 0;
    memset(viewline, 0, sizeof(viewline));
    for (int i = otsechqa; i <= Summ; i++) {
      if (i > otsechqa && stop == 0) {
        if (bufferfo[i] == del[0]) {
          SetVisObjData(m_obj[checkfull], & viewline, strlen(viewline) + 1);
          if (checkfull == (arrsize - 1)) {
            memset(viewline, 0, sizeof(viewline));
            stop = 1;
            reed = 1;
            j = 0;
          }
          checkfull = checkfull + 1;
          j = 0;
          memset(viewline, 0, sizeof(viewline));
        } else {
          viewline[j] = bufferfo[i];
          j++;
        }
      }
    }
  }
  return 0;
}

void Visu_File_Open(const tVisuData * Data) {
  InfoContainerOn(CNT_FILEVISIBLE);
}

void Visu_File_Off(const tVisuData * Data) {
  InfoContainerOff(CNT_FILEVISIBLE);
}

int8_t flagsreach = 0;
int8_t searchfail = 0;
int8_t OK_P = 0;
void OK_PR( void ) {
  OK_P = 0;
}
void IntoConteiner(tVisuData * VData) {
  if (IsKeyPressedNew(4) && OK_P == 0) {
    if (FileSize(PATH_ERROR_FLASH) > 0) {
      arrsize = sizeof(m_obj) / sizeof(m_obj[0]);
      per = arrsize - 1;
      OK_P = 1;
      press = 1;
      stop = 0;
      reed = 0;
      _Load();
      otsechqakol = 0;
      otsechqa = massotsech[otsechqakol];
      if (otsechqakol < arrsize && massotsech[otsechqakol + arrsize] == NULL) {
        currsize = SizeFil;
        massotsech[otsechqakol + arrsize] = SizeFil;
      } else currsize = massotsech[otsechqakol + arrsize] - otsechqa;
      SendToVisuObj(l_obj[per], GS_TO_VISU_SET_FG_COLOR, RCColorGetColor(GetVarIndexed(IDX_SYS_COLOR), RCCOLOR_NAME_SELECT));
      _Load_Reed();
      Visu_File_Open(VData);

    }
  }
}

void CloseConteiner(tVisuData * VData) {
  if (IsInfoContainerOn(CNT_FILEVISIBLE)) {
    if (IsKeyPressedNew(1) || IsKeyPressedNew(10) || searchfail == 1) {
      memset(bufferfo, 0, sizeof(bufferfo));
      memset(DateTM, 0, sizeof(DateTM));
      memset(massotsech, 0, sizeof(massotsech));
      otsechqa = -1;
      otsechqakol = 0;
      press = 0;
      reed = 0;
      stop = 0;
      _Load_Reed();
      SendToVisuObj(l_obj[per], GS_TO_VISU_SET_FG_COLOR, RCColorGetColor(GetVarIndexed(IDX_SYS_COLOR), RCCOLOR_NAME_FRONT));
      for (int8_t i = 0; i < arrsize; i++) {
        SetVisObjData(m_obj[i], & bufferfo, strlen(bufferfo) + 1);
      }
      Visu_File_Off(VData);
      if (searchfail == 1) {
        MsgBoxOk(RCTextGetText(RCTEXT_T_INFO, GetVarIndexed(IDX_SYS_LANGUAGE)), RCTextGetText(RCTEXT_T_SEARCHFAIL, GetVarIndexed(IDX_SYS_LANGUAGE)), OK_PR, NULL);
      } else OK_P = 0;
      searchfail = 0;
    }
  }
}

void Flagsearch_Strview( void ) {
  if (IsKeyPressedNew(6) && IsInfoContainerOn(CNT_FILEVISIBLE)) {
    flagsreach = 1 - flagsreach;
    SendToVisuObj(OBJ_TRUETYPEVARIABLE394, GS_TO_VISU_SET_ATTR_NOMINALFLAG, flagsreach);
    SendToVisuObj(OBJ_TRUETYPEVARIABLE381, GS_TO_VISU_SET_ATTR_NOMINALFLAG, flagsreach);
    SendToVisuObj(OBJ_TRUETYPEVARIABLE408, GS_TO_VISU_SET_ATTR_NOMINALFLAG, flagsreach);
    SendToVisuObj(OBJ_TRUETYPEVARIABLE413, GS_TO_VISU_SET_ATTR_NOMINALFLAG, flagsreach);
  }
  SetVar(HDL_STRVIEW, otsechqakol + arrsize);
  for (int8_t i = 1; i < arrsize; i++) {
    SetVar(HDL_STRVIEW1 + (i - 1), otsechqakol + i);
  }
  SetVar(HDL_STRALL, otsechqalast - 1);
}

void Bttn( void ) {
  if (flagsreach == 0 && IsInfoContainerOn(CNT_FILEVISIBLE)) {
    if (IsKeyPressedNew(104) || IsKeyPressedNew(11)) {
      if (otsechqa < massotsech[otsechqalast - arrsize - 1] && GetVar(HDL_STRALL) > arrsize && per == 9) {
        otsechqakol = otsechqakol + 1;
        otsechqa = massotsech[otsechqakol];
        stop = 0;
        reed = 0;
        per = 9;
        SendToVisuObj(l_obj[per], GS_TO_VISU_SET_FG_COLOR, RCColorGetColor(GetVarIndexed(IDX_SYS_COLOR), RCCOLOR_NAME_SELECT));
      } else {
        if (per <= 8) {
          SendToVisuObj(l_obj[per], GS_TO_VISU_SET_FG_COLOR, RCColorGetColor(GetVarIndexed(IDX_SYS_COLOR), RCCOLOR_NAME_FRONT));
          per = per + 1;
          SendToVisuObj(l_obj[per], GS_TO_VISU_SET_FG_COLOR, RCColorGetColor(GetVarIndexed(IDX_SYS_COLOR), RCCOLOR_NAME_SELECT));
        }
      }
    }
    if (IsKeyPressedNew(103) || IsKeyPressedNew(8)) {
      if (otsechqakol > 0 && otsechqa > 1 && per == 0) {
        otsechqakol = otsechqakol - 1;
        otsechqa = massotsech[otsechqakol];
        stop = 0;
        reed = 0;
        per = 0;
        SendToVisuObj(l_obj[per], GS_TO_VISU_SET_FG_COLOR, RCColorGetColor(GetVarIndexed(IDX_SYS_COLOR), RCCOLOR_NAME_SELECT));
      } else {
        if (per >= 1) {
          SendToVisuObj(l_obj[per], GS_TO_VISU_SET_FG_COLOR, RCColorGetColor(GetVarIndexed(IDX_SYS_COLOR), RCCOLOR_NAME_FRONT));
          per = per - 1;
          SendToVisuObj(l_obj[per], GS_TO_VISU_SET_FG_COLOR, RCColorGetColor(GetVarIndexed(IDX_SYS_COLOR), RCCOLOR_NAME_SELECT));
        }
      }
      if (otsechqa == 0) {
        otsechqa = -1;
      }
    }
  }
}

void Bttn_Search( void ) {
  if (IsKeyPressedNew(4)) {
    if (0 != GetVar(HDL_SEARCHDATEDAY) && 0 != GetVar(HDL_SEARCHDATEMONTH) && 0 != GetVar(HDL_SEARCHDATEYEAR)) {
      int fulltimeDT = (GetVar(HDL_SEARCHDATEYEAR) * 10000) + (GetVar(HDL_SEARCHDATEMONTH) * 100) + GetVar(HDL_SEARCHDATEDAY);

      if (DateTM[fulltimeDT] < arrsize) {
        otsechqakol = 0;
        SendToVisuObj(l_obj[per], GS_TO_VISU_SET_FG_COLOR, RCColorGetColor(GetVarIndexed(IDX_SYS_COLOR), RCCOLOR_NAME_FRONT));
        per = DateTM[fulltimeDT];
        SendToVisuObj(l_obj[per], GS_TO_VISU_SET_FG_COLOR, RCColorGetColor(GetVarIndexed(IDX_SYS_COLOR), RCCOLOR_NAME_SELECT));
      } else {
        SendToVisuObj(l_obj[per], GS_TO_VISU_SET_FG_COLOR, RCColorGetColor(GetVarIndexed(IDX_SYS_COLOR), RCCOLOR_NAME_FRONT));
        per = arrsize - 1;
        SendToVisuObj(l_obj[per], GS_TO_VISU_SET_FG_COLOR, RCColorGetColor(GetVarIndexed(IDX_SYS_COLOR), RCCOLOR_NAME_SELECT));
        otsechqakol = DateTM[fulltimeDT] - (arrsize - 1);
      }
      //SetVar(HDL_PROFILEICON_3, DateTM[fulltimeDT]);
      otsechqa = massotsech[otsechqakol];
      if (DateTM[fulltimeDT] != NULL) {
        stop = 0;
        reed = 0;
      } else {
        searchfail = 1;
      }
    }
  }
  if (GetVar(HDL_STR) > GetVar(HDL_STRALL) && GetVar(HDL_STRALL) > arrsize) {
    SetVar(HDL_STR, GetVar(HDL_STRALL));
  }
  if (IsKeyPressedNew(5)) {
    if (GetVar(HDL_STR) <= (otsechqalast - 1) && GetVar(HDL_STR) > 0) {
      if (GetVar(HDL_STR) > (arrsize - 1)) {
        SendToVisuObj(l_obj[per], GS_TO_VISU_SET_FG_COLOR, RCColorGetColor(GetVarIndexed(IDX_SYS_COLOR), RCCOLOR_NAME_FRONT));
        per = arrsize - 1;
        SendToVisuObj(l_obj[per], GS_TO_VISU_SET_FG_COLOR, RCColorGetColor(GetVarIndexed(IDX_SYS_COLOR), RCCOLOR_NAME_SELECT));
        otsechqakol = GetVar(HDL_STR) - arrsize;
      } else {
        otsechqakol = 0;
        SendToVisuObj(l_obj[per], GS_TO_VISU_SET_FG_COLOR, RCColorGetColor(GetVarIndexed(IDX_SYS_COLOR), RCCOLOR_NAME_FRONT));
        per = GetVar(HDL_STR) - 1;
        SendToVisuObj(l_obj[per], GS_TO_VISU_SET_FG_COLOR, RCColorGetColor(GetVarIndexed(IDX_SYS_COLOR), RCCOLOR_NAME_SELECT));
      }
      otsechqa = massotsech[otsechqakol];
      stop = 0;
      reed = 0;
    }
  }
}
//

void Visu_Error_Cycle(const tVisuData * VData, uint32_t evtc, tUserCEvt * evtv) {

  for (uint32_t i = 0; i < evtc; i++) {
    if (CEVT_MENU_INDEX == evtv[i].Type) {
      uint32_t ObjId = evtv[i].Content.mMenuIndex.ObjID;
      if (OBJ_LEBUTTON == ObjId) {
        LearningError = 1;
        InfoContainerOff(CNT_LEARNINGERROR);
      }
    }
  }

  // Для окна отображения ошибок и моточасов
  IntoConteiner(VData);
  CloseConteiner(VData);
  Flagsearch_Strview();
  Bttn();

  if (!IsInfoContainerOn(CNT_FILEVISIBLE) && OK_P == 0) {
    SetVar(HDL_SEARCHDATEDAY, 1);
    SetVar(HDL_SEARCHDATEMONTH, 1);
    SetVar(HDL_SEARCHDATEYEAR, 19);
    SetVar(HDL_STR, 1);
    ErrorCheckState();
  uint32_t Password = (uint32_t) GetVarIndexed(IDX_PASSWORD_ENTER);
  if (Password == (uint32_t) GetVar(HDL_PASSWORD_MERKATOR) || Password == 32546) {
    SendToVisuObj(OBJ_ATRESTTTEXT929, GS_TO_VISU_SET_ATTR_VISIBLE, 1);
  } else {
    SendToVisuObj(OBJ_ATRESTTTEXT929, GS_TO_VISU_SET_ATTR_VISIBLE, 0);
  }
    if (IsKeyPressedNew(2)) {
      WriteLastError();
      GeneralOperatingTimeforError(VData);
      VinEquipUpdate();
      Visu_OpenScreen(SCREEN_MAIN);
      if (1 == Error_CopyToUsb()) {
        MsgBoxOK_RCText(RCTEXT_T_INFO, RCTEXT_T_COPYTOUSBSUCCESS, NULL, NULL);
      } else MsgBoxOK_RCText(RCTEXT_T_INFO, RCTEXT_T_COPYTOUSBERROR, NULL, NULL);
    }
    if (IsKeyPressedNew(3)) {
      FormattingErrorFile(VData);
    }

    if (!IsInfoContainerOn(CNT_LEARNINGERROR)) {
      ErrDraw_Cycle(evtc, evtv);
    }
    Visu_Error_Learning();
    EList_AutoErrorRemove();
    Visu_HomeKeyPressed();
  } else if (IsInfoContainerOn(CNT_FILEVISIBLE) && OK_P == 1) {
    if (press == 1) {
      _Load_Reed();
    }
    Bttn_Search();
  }

  Visu_Error_Back(VData);
}
