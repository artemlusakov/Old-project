/****************************************************************************
*
* File:         VISU_STATISTIK.c
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

#include <stdint.h>

#include "vartab.h"       /* Variable Table definition                      */
/* include this file in every C-Source to access  */
/* the variable table of your project             */
/* vartab.h is generated automatically            */
#include "objtab.h"       /* Object ID definition                           */
						  /* include this file in every C-Source to access  */
						  /* the object ID's of the visual objects          */
						  /* objtab.h is generated automatically            */
#include "visu_statistik.h"
#include "gsemaint_lifetime.h"
#include "control.h"
#include "gsToVisu.h"
#include "RCText.h"
#include "gse_msgbox.h"
#include "gseerrorlist.h"
#include "backup.h"

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

void Visu_Statistic_Open( const tVisuData* Data ) {
	PrioMaskOn( MSK_STATISTIK );
}

int OBJ_Selected[] = {
	OBJ_BRUSHFRONT,
	OBJ_BRUSHINTER,
	OBJ_PLOUGHFRONT,
	OBJ_PLOUGSIDE,
	OBJ_RTR,
	OBJ_LRS300,
	OBJ_LRS600,
	OBJ_HYDLOAD,
	OBJ_HYDNOLOAD,
};
int Variables_NoReset[] = {
	IDX_BRUSH_WARN_0,
	IDX_BRUSH_WARN_2,
	IDX_PLOUGH_WARN_0,
	IDX_PLOUGH_WARN_1,
	IDX_RUNTIME_RTR,
	IDX_RUNTIME_LRS_300,
	IDX_RUNTIME_LRS_600,
	IDX_RUNTIME_HYDLOAD_SHOW,
	IDX_RUNTIME_HYDNOLOAD_SHOW,
};
int Variables_Reset[] = {
	IDX_BRUSH_WARN_0_RESET,
	IDX_BRUSH_WARN_2_RESET,
	IDX_PLOUGH_WARN_0_RESET,
	IDX_PLOUGH_WARN_1_RESET,
	IDX_RUNTIME_RTR_RESET,
	IDX_RUNTIME_LRS_RESET300,
	IDX_RUNTIME_LRS_RESET600,
	IDX_RUNTIME_HYDLOAD_SHOW_RESET,
	IDX_RUNTIME_HYDNOLOAD_SHOW_RESET,
};
/*
HYDLOAD
HYDNOLOAD

WARNING!!!
do not change the location
*/
int Variables_Distance_Work[] = {
	HDL_DISTANCE_BRUSH_0_LOAD,
	HDL_DISTANCE_BRUSH_2_LOAD,
	HDL_DISTANCE_PLOUGH_0_LOAD,
	HDL_DISTANCE_PLOUGH_1_LOAD,
	HDL_DISTANCE_RTR_LOAD,
	HDL_DISTANCE_LRS_300_LOAD,
	HDL_DISTANCE_LRS_600_LOAD,
	HDL_DISTANCE_HYDLOAD,
	HDL_DISTANCE_HYDNOLOAD,
};
/*
HYDNOLOAD1
HYDLOAD1

WARNING!!!
do not change the location
*/
int Variables_Distance_Reset[] = {
	HDL_DISTANCE_BRUSH_0_NOLOAD,
	HDL_DISTANCE_BRUSH_2_NOLOAD,
  HDL_DISTANCE_PLOUGH_0_NOLOAD,
	HDL_DISTANCE_PLOUGH_1_NOLOAD,
	HDL_DISTANCE_RTR_NOLOAD,
  HDL_DISTANCE_LRS_300_NOLOAD,
	HDL_DISTANCE_LRS_600_NOLOAD,
	HDL_DISTANCE_HYDNOLOAD1,
	HDL_DISTANCE_HYDLOAD1,
};

char newpowerdaystat[15];
char newpowermonthstat[15];
char newpoweryearstat[15];

int32_t daystat;
int32_t monthstat;
int32_t yearstat;

void RuntimeWriteFileError(tVisuData * VData, int c, tGsFile ** fp) {
  char StatisticState[100];
  const char * choistext = NULL;
  const char * FileDate = RCTextGetText(RCTEXT_T_FILECREATEDATE, GetVarIndexed(IDX_SYS_LANGUAGE));
  const char * RESETMH = RCTextGetText(RCTEXT_T_RESETMH, GetVarIndexed(IDX_SYS_LANGUAGE));
  switch (c) {
  case 0:
    choistext = RCTextGetText(RCTEXT_T_BRUSHFRONT, GetVarIndexed(IDX_SYS_LANGUAGE));
    break;
  case 1:
    choistext = RCTextGetText(RCTEXT_T_BRUSHINTER, GetVarIndexed(IDX_SYS_LANGUAGE));
    break;
  case 2:
    choistext = RCTextGetText(RCTEXT_T_PLOUGHFRONT, GetVarIndexed(IDX_SYS_LANGUAGE));
    break;
  case 3:
    choistext = RCTextGetText(RCTEXT_T_PLOUGHSIDE, GetVarIndexed(IDX_SYS_LANGUAGE));
    break;
  case 4:
    choistext = RCTextGetText(RCTEXT_T_RTR, GetVarIndexed(IDX_SYS_LANGUAGE));
    break;
  case 5:
    choistext = RCTextGetText(RCTEXT_T_LRS300, GetVarIndexed(IDX_SYS_LANGUAGE));
    break;
  case 6:
    choistext = RCTextGetText(RCTEXT_T_LRS600, GetVarIndexed(IDX_SYS_LANGUAGE));
    break;
  case 7:
    choistext = RCTextGetText(RCTEXT_T_HYDLOAD, GetVarIndexed(IDX_SYS_LANGUAGE));
    break;
  case 8:
    choistext = RCTextGetText(RCTEXT_T_HYDNOLOAD, GetVarIndexed(IDX_SYS_LANGUAGE));
    break;
  default:
    sprintf(choistext, "Error");
    break;
  }
  sprintf(StatisticState, choistext);
  if (FileSize(PATH_ERROR_FLASH) > 0 && GetVar(HDL_KONTROLWRITE) == 0) // добавлено для 2.0
  {
    SetVar(HDL_KONTROLWRITE, 1);
  }
  if (GetVar(HDL_KONTROLWRITE) == 0) {
    FilePrintf( * fp, "                                      \n");
    FilePrintf( * fp, "                                      \n");
    FilePrintf( * fp, "                                      \n");
    FilePrintf( * fp, "\n");
    RTCGetDate( & VData -> Date);
    RTCGetTime( & VData -> Time);
    snprintf(newpowerdaystat, GS_ARRAYELEMENTS(newpowerdaystat), "%02d", VData -> Date.Day);
    snprintf(newpowermonthstat, GS_ARRAYELEMENTS(newpowermonthstat), "%02d", VData -> Date.Month);
    snprintf(newpoweryearstat, GS_ARRAYELEMENTS(newpoweryearstat), "%02d", VData -> Date.Year);
    daystat = atoi(newpowerdaystat);
    monthstat = atoi(newpowermonthstat);
    yearstat = atoi(newpoweryearstat);
    SetVar(HDL_KOLTIMEDAY, daystat);
    SetVar(HDL_KOLTIMEMONTH, monthstat);
    SetVar(HDL_KOLTIMEYEAR, yearstat);
    FilePrintf( * fp, " %s = %02d.%02d.%02d |\n", FileDate, daystat, monthstat, yearstat);
    SetVar(HDL_KONTROLWRITE, 1);
  }
  char StatisticTime[12];
  char stringdate[12];
  int32_t GetLastSbros = GetVarIndexed(Variables_Reset[c]); //Всего секунд
  int32_t GetLastSbrosM = GetLastSbros / 60; //Всего минут
  int32_t GetLastSbrosH = GetLastSbrosM / 60; //Всего часов
  int32_t Min = GetLastSbrosM - GetLastSbrosH * 60; //расчёт минут
  int32_t Sec = GetLastSbros - GetLastSbrosM * 60; //расчёт секунд

  int32_t GetLastSbrosALL = GetVarIndexed(Variables_NoReset[c]); //Всего секунд
  int32_t GetLastSbrosMALL = GetLastSbrosALL / 60; //Всего минут
  int32_t GetLastSbrosHALL = GetLastSbrosMALL / 60; //Всего часов
  int32_t MinALL = GetLastSbrosMALL - GetLastSbrosHALL * 60; //расчёт минут
  int32_t SecALL = GetLastSbrosALL - GetLastSbrosMALL * 60; //расчёт секунд
  RTCGetDate( & VData -> Date);
  RTCGetTime( & VData -> Time);
  snprintf(stringdate, GS_ARRAYELEMENTS(stringdate), "%02d.%02d.%02d", VData -> Date.Day, VData -> Date.Month, VData -> Date.Year);
  snprintf(StatisticTime, GS_ARRAYELEMENTS(StatisticTime), "%02d:%02d:%02d", VData -> Time.Hours, VData -> Time.Minutes, VData -> Time.Seconds);

  FilePrintf( * fp, " %s |", stringdate);
  FilePrintf( * fp, " %s |", StatisticTime);
  FilePrintf( * fp, " %s |", StatisticState);
  FilePrintf( * fp, " %d:%02d:%02d |", GetLastSbrosH, Min, Sec);
  FilePrintf( * fp, " %d:%02d:%02d |", GetLastSbrosHALL, MinALL, SecALL);
  FilePrintf( * fp, " %s |\n", RESETMH);
  FilePrintf( * fp, " %.1f km |\n", GetVarFloat(Variables_Distance_Work[c]));
}

void GeneralOperatingTime(tVisuData * VData, tGsFile ** fp) {
  int count_obj = sizeof(OBJ_Selected) / sizeof( * OBJ_Selected);
  for (int i = 0; i < count_obj; i++) {
    char StatisticState[100];
    const char * choistext = NULL;
    const char * FileDate = RCTextGetText(RCTEXT_T_FILECREATEDATE, GetVarIndexed(IDX_SYS_LANGUAGE));
    const char * TEQP = RCTextGetText(RCTEXT_T_TOTALMH, GetVarIndexed(IDX_SYS_LANGUAGE));
    switch (i) {
    case 0:
      choistext = RCTextGetText(RCTEXT_T_BRUSHFRONT, GetVarIndexed(IDX_SYS_LANGUAGE));
      break;
    case 1:
      choistext = RCTextGetText(RCTEXT_T_BRUSHINTER, GetVarIndexed(IDX_SYS_LANGUAGE));
      break;
    case 2:
      choistext = RCTextGetText(RCTEXT_T_PLOUGHFRONT, GetVarIndexed(IDX_SYS_LANGUAGE));
      break;
    case 3:
      choistext = RCTextGetText(RCTEXT_T_PLOUGHSIDE, GetVarIndexed(IDX_SYS_LANGUAGE));
      break;
    case 4:
      choistext = RCTextGetText(RCTEXT_T_RTR, GetVarIndexed(IDX_SYS_LANGUAGE));
      break;
    case 5:
      choistext = RCTextGetText(RCTEXT_T_LRS300, GetVarIndexed(IDX_SYS_LANGUAGE));
      break;
    case 6:
      choistext = RCTextGetText(RCTEXT_T_LRS600, GetVarIndexed(IDX_SYS_LANGUAGE));
      break;
    case 7:
      choistext = RCTextGetText(RCTEXT_T_HYDLOAD, GetVarIndexed(IDX_SYS_LANGUAGE));
      break;
    case 8:
      choistext = RCTextGetText(RCTEXT_T_HYDNOLOAD, GetVarIndexed(IDX_SYS_LANGUAGE));
      break;
    default:
	  //sprintf(choistext, "Error");
	  choistext = "Error";
	  break;
    }
    sprintf(StatisticState, choistext);
    if (FileSize(PATH_ERROR_FLASH) > 0 && GetVar(HDL_KONTROLWRITE) == 0) // добавлено для 2.0
    {
      SetVar(HDL_KONTROLWRITE, 1);
    }
    if (GetVar(HDL_KONTROLWRITE) == 0) {
      FilePrintf( * fp, "                                      \n");
      FilePrintf( * fp, "                                      \n");
      FilePrintf( * fp, "                                      \n");
      //FilePrintf( * fp, "");
      FilePrintf( * fp, "\n");
      RTCGetDate( & VData -> Date);
      RTCGetTime( & VData -> Time);
      snprintf(newpowerdaystat, GS_ARRAYELEMENTS(newpowerdaystat), "%02d", VData -> Date.Day);
      snprintf(newpowermonthstat, GS_ARRAYELEMENTS(newpowermonthstat), "%02d", VData -> Date.Month);
      snprintf(newpoweryearstat, GS_ARRAYELEMENTS(newpoweryearstat), "%02d", VData -> Date.Year);
      daystat = atoi(newpowerdaystat);
      monthstat = atoi(newpowermonthstat);
      yearstat = atoi(newpoweryearstat);
      SetVar(HDL_KOLTIMEDAY, daystat);
      SetVar(HDL_KOLTIMEMONTH, monthstat);
      SetVar(HDL_KOLTIMEYEAR, yearstat);
      FilePrintf( * fp, " %s = %02d.%02d.%02d |\n", FileDate, daystat, monthstat, yearstat);
      SetVar(HDL_KONTROLWRITE, 1);
    }

    char StatisticTime[12];
    char stringdate[12];
    int32_t GetLastSbros = GetVarIndexed(Variables_Reset[i]); //Всего секунд
    int32_t GetLastSbrosM = GetLastSbros / 60; //Всего минут
    int32_t GetLastSbrosH = GetLastSbrosM / 60; //Всего часов
    int32_t Min = GetLastSbrosM - GetLastSbrosH * 60; //расчёт минут
    int32_t Sec = GetLastSbros - GetLastSbrosM * 60; //расчёт секунд

    int32_t GetLastSbrosALL = GetVarIndexed(Variables_NoReset[i]); //Всего секунд
    int32_t GetLastSbrosMALL = GetLastSbrosALL / 60; //Всего минут
    int32_t GetLastSbrosHALL = GetLastSbrosMALL / 60; //Всего часов
    int32_t MinALL = GetLastSbrosMALL - GetLastSbrosHALL * 60; //расчёт минут
    int32_t SecALL = GetLastSbrosALL - GetLastSbrosMALL * 60; //расчёт секунд
    RTCGetDate( & VData -> Date);
    RTCGetTime( & VData -> Time);
    snprintf(stringdate, GS_ARRAYELEMENTS(stringdate), "%02d.%02d.%02d", VData -> Date.Day, VData -> Date.Month, VData -> Date.Year);
    snprintf(StatisticTime, GS_ARRAYELEMENTS(StatisticTime), "%02d:%02d:%02d", VData -> Time.Hours, VData -> Time.Minutes, VData -> Time.Seconds);
    if (i == 0) {
      FilePrintf( * fp, "------------------------------\n");
      FilePrintf( * fp, " %s: \n", TEQP);
    }
    FilePrintf( * fp, " %s |", stringdate);
    FilePrintf( * fp, " %s |", StatisticTime);
    FilePrintf( * fp, " %s |", StatisticState);
    FilePrintf( * fp, " %d:%02d:%02d |", GetLastSbrosH, Min, Sec);
    FilePrintf( * fp, " %d:%02d:%02d |\n", GetLastSbrosHALL, MinALL, SecALL);
    FilePrintf( * fp, " %.1f km |\n", GetVarFloat(Variables_Distance_Work[i]));
    if (i == count_obj - 1) {
      FilePrintf( * fp, "------------------------------\n");
    }
  }
}

void SaveStatistic( void ) {
const tControl * Ctrl = CtrlGet();
  tGsFile * fp = FileOpen(PATH_STATISTIC_FLASH, "w+");
  int count_obj = sizeof(OBJ_Selected) / sizeof( * OBJ_Selected);
  FilePrintf(fp, "%d ", Maint_Lifetime_TimeUntilNextService_d(Ctrl -> anual_maint)); //write next Maintenance
  for (int i = 0; i < count_obj; i++) {
    FilePrintf(fp, "%d ", GetVarIndexed(Variables_Reset[i])); //equip time work
  }
  for (int i = 0; i < count_obj; i++) {
    FilePrintf(fp, "%.1f ", GetVarFloat(Variables_Distance_Work[i])); //Distance
  }
  FileClose(fp);
  CopyFile(PATH_STATISTIC_FLASH, PATH_STATISTIC_USB, NULL); //copy to usb
  FileUnlink(fp);
}

void RepairStatistic( void ) {
  const tControl * Ctrl = CtrlGet();
  CopyFile(PATH_STATISTIC_USB, PATH_STATISTIC_FLASH, NULL);
  char buf[2000] = {
    0
  };
  tGsFile * fp = FileOpen(PATH_STATISTIC_FLASH, "r+");
  FileRead( & buf, 1, sizeof(buf), fp);
  if (FileSize(PATH_STATISTIC_USB) > 0 ) { //Check avability file and hash
    char delim[] = {
      " "
    };
    char SearchBufVal[100];
    memset(SearchBufVal, 0, sizeof(SearchBufVal));
    int i = 0; //count delim in file
    int t = 0; //first delim
    int j = 0; //count all symbol
    uint32_t valbuff = 0;
    int count_obj = sizeof(OBJ_Selected) / sizeof( * OBJ_Selected);
    while (t == 0) {
      if (buf[j] == delim[0]) {
        int timeqp = atoi(SearchBufVal);
        Maint_Lifetime_SetNextServiceInSeconds(Ctrl -> anual_maint, 30 * 60 * 60 * 24, timeqp * 60 * 60 * 24); //Write Maintenance
        memset(SearchBufVal, 0, sizeof(SearchBufVal));
        valbuff = 0;
        t++;
      } else {
        SearchBufVal[valbuff] = buf[j];
        valbuff++;
      }
      j++;
    }
    int space = 0;
    int space_count = 0;
    for (int q = 0; q <= FileSize(PATH_STATISTIC_USB); q++) {
      if (buf[space_count] == delim[0]) {
        space++;
      }
      space_count++;
    }

    while (i < count_obj) {
      if (buf[j] == delim[0]) {
        int timeqp = atoi(SearchBufVal);
        if (space == 13 && i == 6) {
          i = 7;
        }
        if (space == 13 && i == 2) {
          i = 4;
        }
        if (i == 5) {
          SetVarIndexed(Variables_Reset[i], timeqp); //Set equip time work
          SetVarIndexed(Variables_Reset[i + 1], timeqp); //Set equip time work
        } else {
          SetVarIndexed(Variables_Reset[i], timeqp); //Set equip time work
        }
        memset(SearchBufVal, 0, sizeof(SearchBufVal));
        valbuff = 0;
        i++;
      } else {
        SearchBufVal[valbuff] = buf[j];
        valbuff++;
      }
      j++;
    }
    while (i >= count_obj && i < count_obj * 2) {
      if (buf[j] == delim[0]) {
        float timeqp = atof(SearchBufVal);
        if (space == 13 && i == 15) {
          i = 16;
        }
        if (space == 13 && i == 11) {
          i = 13;
        }
        if (i == 14) {
          //Set File Distance
          SetVarFloat(Variables_Distance_Reset[i - count_obj], Ctrl -> engine.PassedWay + (Ctrl -> engine.PassedWay - timeqp) - Ctrl -> engine.PassedWay);
          SetVarFloat(Variables_Distance_Work[i - count_obj], Ctrl -> engine.PassedWay - GetVarFloat(Variables_Distance_Reset[i - count_obj]));

          SetVarFloat(Variables_Distance_Reset[i + 1 - count_obj], Ctrl -> engine.PassedWay + (Ctrl -> engine.PassedWay - timeqp) - Ctrl -> engine.PassedWay);
          SetVarFloat(Variables_Distance_Work[i + 1 - count_obj], Ctrl -> engine.PassedWay - GetVarFloat(Variables_Distance_Reset[i + 1 - count_obj]));

        } else {
          //Set File Distance
          SetVarFloat(Variables_Distance_Reset[i - count_obj], Ctrl -> engine.PassedWay + (Ctrl -> engine.PassedWay - timeqp) - Ctrl -> engine.PassedWay);
          SetVarFloat(Variables_Distance_Work[i - count_obj], Ctrl -> engine.PassedWay - GetVarFloat(Variables_Distance_Reset[i - count_obj]));
        }
        memset(SearchBufVal, 0, sizeof(SearchBufVal));
        valbuff = 0;
        i++;
      } else {
        SearchBufVal[valbuff] = buf[j];
        valbuff++;
      }
      j++;
    }
  } else {
    char stringinf[100];
    snprintf(stringinf, GS_ARRAYELEMENTS(stringinf), "%s %s", RCTextGetText(RCTEXT_T_FILENOTFOUND, GetVarIndexed(IDX_SYS_LANGUAGE)), "STATList.cfg");
    MsgBoxOk(RCTextGetText(RCTEXT_T_INFO, GetVarIndexed(IDX_SYS_LANGUAGE)), stringinf, NULL, NULL);
  }
  FileClose(fp);
}

int32_t RuntimeCheckStateforError(tVisuData * VData, int ib) {
  tGsFile * fp = FileOpen(PATH_ERROR_FLASH, "a+");
  RuntimeWriteFileError(VData, ib, & fp);
  SetVarIndexed(IDX_ERRORSSIZE, FileSize(PATH_ERROR_FLASH));
  FileClose(fp);
  return 0;
}

int32_t GeneralOperatingTimeforError(tVisuData * VData) {
  tGsFile * fp = FileOpen(PATH_ERROR_FLASH, "a+");
  GeneralOperatingTime(VData, & fp);
  SetVarIndexed(IDX_ERRORSSIZE, FileSize(PATH_ERROR_FLASH));
  FileClose(fp);
  return 0;
}

int numpos = 0;
int8_t OK_Press = 0;
int OK_PRESSED( void ) {
  OK_Press = 1;
  return 0;
}

int CANCEL_Press( void ) {
  OK_Press = 0;
  return 0;
}

int count_obj = sizeof(OBJ_Selected) / sizeof( * OBJ_Selected);
int Can_Statistic( void ) {
  static uint32_t tmrADDMT = 0;
  static int dataTODays = 0;
  static uint8_t idxArr = 0;
  tCanMessage canMsg;
  canMsg.channel = 1;
  canMsg.ext = 1;
  canMsg.res = 0;
  canMsg.len = 8;
  canMsg.id = 0x18FF0581;
  dataTODays = GetVar(HDL_NEXTSERVICE);

  static int iQueneSend = 0;

  static uint32_t GetHNoReset = 0;
  static uint32_t GetHReset = 0;
  static uint32_t KM = 0;
  if (GetMSTick() >= (tmrADDMT + 100)) {
    switch (iQueneSend) {
    case 1:
    case 2:
      idxArr = 0;
      break;
    case 3:
    case 4:
      idxArr = 1;
      break;
    case 5:
    case 6:
      idxArr = 2;
      break;
    case 7:
    case 8:
      idxArr = 3;
      break;
    case 9:
      idxArr = 4;
      break;
    case 10:
      idxArr = 5;
      break;
    case 11:
      idxArr = 6;
      break;
    case 12:
      idxArr = 7;
      break;
    case 13:
      idxArr = 8;
      break;

    default:
      break;
    }

    GetHNoReset = GetVarIndexed(Variables_NoReset[idxArr] )/ 60 / 60;
    GetHReset = GetVarIndexed(Variables_Reset[idxArr] )/ 60 / 60;
    KM = (uint32_t) GetVarFloat(Variables_Distance_Work[idxArr]);

    if (iQueneSend == 0) {
      canMsg.data.s8[0] = 0;
      canMsg.data.s8[1] = dataTODays;
      canMsg.data.s8[2] = dataTODays >> 8;
      canMsg.data.u8[3] = 0;
      canMsg.data.u8[4] = 0;
      canMsg.data.u8[5] = 0;
      canMsg.data.u8[6] = 0;
      canMsg.data.u8[7] = 0;
    } else if (iQueneSend > 0 && iQueneSend < 10) {
      if (iQueneSend % 2 != 0) {
        // РќРµС‡РµС‚РЅС‹Рµ РёРЅРґРµРєСЃС‹ - РѕР±С‰Р°СЏ РЅР°СЂР°Р±РѕС‚РєР°
        canMsg.data.u8[0] = iQueneSend;
        canMsg.data.u8[1] = 0xFF & GetHNoReset;
        canMsg.data.u8[2] = 0xFF & (GetHNoReset >> 8);
        canMsg.data.u8[3] = 0xFF & GetHReset;
        canMsg.data.u8[4] = 0xFF & (GetHReset >> 8);
        canMsg.data.u8[5] = 0;
        canMsg.data.u8[6] = 0;
        canMsg.data.u8[7] = 0;
      } else {
        // Р§РµС‚РЅС‹Рµ РёРЅРґРµРєСЃС‹ - РѕР±С‰РёР№ РїСЂРѕР±РµРі
        canMsg.data.u8[0] = iQueneSend;
        canMsg.data.u8[1] = 0xFF & KM;
        canMsg.data.u8[2] = 0xFF & (KM >> 8);
        canMsg.data.u8[3] = 0xFF & (KM >> 16);
        canMsg.data.u8[4] = 0;
        canMsg.data.u8[5] = 0;
        canMsg.data.u8[6] = 0;
        canMsg.data.u8[7] = 0;
      }
    } else {
      // РћСЃРЅРѕРІРЅРѕРµ РѕР±РѕСЂСѓРґРѕРІР°РЅРёРµ
      canMsg.data.u8[0] = iQueneSend;
      canMsg.data.u8[1] = 0xFF & GetHNoReset;
      canMsg.data.u8[2] = 0xFF & (GetHNoReset >> 8);
      canMsg.data.u8[3] = 0xFF & GetHReset;
      canMsg.data.u8[4] = 0xFF & (GetHReset >> 8);
      canMsg.data.u8[5] = 0;
      canMsg.data.u8[6] = 0;
      canMsg.data.u8[7] = 0;
    }

    CANSendMsg( & canMsg);

    if (++iQueneSend > 13) iQueneSend = 0;
    tmrADDMT = GetMSTick();
  }
  return 0;
}
void Visu_Statistic_Cycle(const tVisuData * VData, uint32_t evtc, tUserCEvt * evtv) {
  const tControl * Ctrl = CtrlGet();
  int count_obj = sizeof(OBJ_Selected) / sizeof( * OBJ_Selected);
  SetVar(HDL_NEXTSERVICE, Maint_Lifetime_TimeUntilNextService_d(Ctrl -> anual_maint));
  for (uint32_t i = 0; i < evtc; i++) {
    int16_t obj = evtv[i].Content.mMenuIndex.ObjID;
    if (CEVT_MENU_INDEX == evtv[i].Type) {
      //Set next Service
      if (OBJ_SETSERVICE == evtv[i].Content.mMenuIndex.ObjID) {
        Maint_Lifetime_SetNextServiceInSeconds(Ctrl -> anual_maint, 30 * 60 * 60 * 24, GetVar(HDL_SETNEXTSERVICE) * 60 * 60 * 24);
      }
      for (int ib = 0; ib < count_obj; ib++) {
        if (obj == OBJ_Selected[ib]) {
          numpos = ib;
          MsgBoxOkCancel(RCTextGetText(RCTEXT_T_SAVECHANGES, GetVarIndexed(IDX_SYS_LANGUAGE)),
            RCTextGetText(RCTEXT_T_SAVECHANGESTEXT, GetVarIndexed(IDX_SYS_LANGUAGE)),
            OK_PRESSED, NULL, CANCEL_Press, NULL);
        }
      }

    }
    if (CEVT_SELECTED == evtv[i].Type) {
      int Time_Obj_Color[] = {
        OBJ_TRUETYPEHOURS597,
        OBJ_TRUETYPEHOURS598,
        OBJ_TRUETYPEHOURS1107,
        OBJ_TRUETYPEHOURS1102,
        OBJ_TRUETYPEHOURS599,
        OBJ_TRUETYPEHOURS1111,
        OBJ_TRUETYPEHOURS600,
        OBJ_TRUETYPEHOURS608,
        OBJ_TRUETYPEHOURS621,
      };

      int Distance_Obj_Color[] = {
        OBJ_TRUETYPEVARIABLE1091,
        OBJ_TRUETYPEVARIABLE1092,
        OBJ_TRUETYPEVARIABLE1108,
        OBJ_TRUETYPEVARIABLE1101,
        OBJ_TRUETYPEVARIABLE1093,
        OBJ_TRUETYPEVARIABLE1113,
        OBJ_TRUETYPEVARIABLE1094,
        OBJ_TRUETYPEVARIABLE1095,
        OBJ_TRUETYPEVARIABLE1096,
      };
      uint32_t obj1 = evtv[i].Content.mSelected.ObjID;
      if (obj1 != OBJ_TRUETYPEVARIABLE405 && obj1 != OBJ_SETSERVICE) {
        SendToVisuObj(OBJ_ATRESTTTEXT786, GS_TO_VISU_SET_ATTR_VISIBLE, 1);
      } else SendToVisuObj(OBJ_ATRESTTTEXT786, GS_TO_VISU_SET_ATTR_VISIBLE, 0);
      for (int ib = 0; ib < count_obj; ib++) {
        if (obj1 == OBJ_Selected[ib]) {
          SendToVisuObj(OBJ_ATRESTTTEXT786, GS_TO_VISU_SET_ORIGIN_X, 555);
          SendToVisuObj(OBJ_ATRESTTTEXT786, GS_TO_VISU_SET_ORIGIN_Y, 104 + (33 * ib));
          SendToVisuObj(Time_Obj_Color[ib], GS_TO_VISU_SET_FG_COLOR, 0xFF4C00);
          SendToVisuObj(Distance_Obj_Color[ib], GS_TO_VISU_SET_FG_COLOR, 0xFF4C00);
        } else {
          SendToVisuObj(Time_Obj_Color[ib], GS_TO_VISU_SET_FG_COLOR, 0xFFFFFF);
          SendToVisuObj(Distance_Obj_Color[ib], GS_TO_VISU_SET_FG_COLOR, 0xFFFFFF);
        }
      }
    }
  }
  if (OK_Press == 1) {
    RuntimeCheckStateforError(VData, numpos);

    SetVarFloat(Variables_Distance_Reset[numpos], Ctrl -> engine.PassedWay);
    SetVarFloat(Variables_Distance_Work[numpos], Ctrl -> engine.PassedWay - GetVarFloat(Variables_Distance_Reset[numpos]));

    SetVarIndexed(Variables_Reset[numpos], 0);
    OK_Press = 0;
  }

  Can_Statistic();
  Visu_HomeKeyPressed();
}

typedef struct tagVisuStatLine {
  int32_t textHDL;
  int timeHDL;
  int timeHDL_R;
  int distHDL;
}tVisuStat;

enum {
  STAT_ITEM_COUNTER = 0,
  STAT_NAME,
  STAT_TIME,
  STAT_TIME_RESET,
  STAT_DISTANCE,
  STAT_NEXTTO,
};

char headers[][32] = { {"Item_count = "}, {"Name = "}, {"Time = "}, {"Time_Reset = "}, {"Distance = "}, {"Next_TO_in "} };
tVisuStat visuStat[] = {
  {.textHDL = RCTEXT_T_BRUSHFRONT,  .timeHDL = HDL_BRUSH_WARN_0,      .timeHDL_R = HDL_BRUSH_WARN_0_RESET,            .distHDL = HDL_DISTANCE_BRUSH_0_LOAD},
  {.textHDL = RCTEXT_T_BRUSHINTER,  .timeHDL = HDL_BRUSH_WARN_2,      .timeHDL_R = HDL_BRUSH_WARN_2_RESET,            .distHDL = HDL_DISTANCE_BRUSH_2_LOAD},
  {.textHDL = RCTEXT_T_PLOUGHFRONT, .timeHDL = HDL_PLOUGH_WARN_0,     .timeHDL_R = HDL_PLOUGH_WARN_0_RESET,           .distHDL = HDL_DISTANCE_PLOUGH_0_LOAD},
  {.textHDL = RCTEXT_T_PLOUGHSIDE,  .timeHDL = HDL_PLOUGH_WARN_1,     .timeHDL_R = HDL_PLOUGH_WARN_1_RESET,           .distHDL = HDL_DISTANCE_PLOUGH_1_LOAD},
  {.textHDL = RCTEXT_T_RTR,         .timeHDL = HDL_RUNTIME_RTR,       .timeHDL_R = HDL_RUNTIME_RTR_RESET,             .distHDL = HDL_DISTANCE_RTR_LOAD},
  {.textHDL = RCTEXT_T_LRS300,      .timeHDL = HDL_RUNTIME_LRS_300,   .timeHDL_R = HDL_RUNTIME_LRS_RESET300,          .distHDL = HDL_DISTANCE_LRS_300_LOAD},
  {.textHDL = RCTEXT_T_LRS600,      .timeHDL = HDL_RUNTIME_LRS_600,   .timeHDL_R = HDL_RUNTIME_LRS_RESET600,          .distHDL = HDL_DISTANCE_LRS_600_LOAD},
  {.textHDL = RCTEXT_T_HYDLOAD,     .timeHDL = HDL_RUNTIME_HYDLOAD,   .timeHDL_R = HDL_RUNTIME_HYDLOAD_SHOW_RESET,    .distHDL = HDL_DISTANCE_HYDLOAD},
  {.textHDL = RCTEXT_T_HYDNOLOAD,   .timeHDL = HDL_RUNTIME_HYDNOLOAD, .timeHDL_R = HDL_RUNTIME_HYDNOLOAD_SHOW_RESET,  .distHDL = HDL_DISTANCE_HYDNOLOAD},
};

typedef struct tagStatLine {
  char name[64];
  int maint_time;
  int maint_time_reset;
  double maint_dist;
}tStat;

void Statistics_Save( void ) {
  tGsFile* fp = FileOpen(PATH_STATISTIC_FLASH, "w");
  if(fp == NULL) {
    return;
  }

  FilePrintf(fp, "%s%d\r\n", headers[STAT_NEXTTO], Maint_Lifetime_TimeUntilNextService_d(CtrlGet()->anual_maint));

  int size = GS_ARRAYELEMENTS(visuStat);
  FilePrintf(fp, "%s%d\r\n", headers[STAT_ITEM_COUNTER], size);

  for(int i = 0; i < size; i++) {
    FilePrintf(fp, "%s%s, ", headers[STAT_NAME], RCTextGetText(visuStat[i].textHDL,GetVarIndexed(IDX_SYS_LANGUAGE)));
    FilePrintf(fp, "%s%d, ", headers[STAT_TIME], GetVar(visuStat[i].timeHDL));
    FilePrintf(fp, "%s%d, ", headers[STAT_TIME_RESET], GetVar(visuStat[i].timeHDL_R));
    FilePrintf(fp, "%s%f\r\n", headers[STAT_DISTANCE], GetVarFloat(visuStat[i].distHDL));
  }

  FileClose(fp);
}

void Statistics_SaveBacup( void ) {
  Statistics_Save();
  CopyFile(PATH_STATISTIC_FLASH, PATH_STATISTIC_USB, NULL);
}

char * Statistics_GetValPtrByHeader(const char * buf, const char * header) {
  if(buf == NULL || header == NULL) {
    return NULL;
  }

  char * pos = strstr(buf, header);
  if(pos) {
    return pos + strlen(header);
  }

  return NULL;
}

int Statistics_GetNameSize(const char * buf) {
  if(buf == NULL) {
    return 0;
  }

  char *pos = Statistics_GetValPtrByHeader(buf, headers[STAT_NAME]);
  if(pos) {
    return strchr(pos, ',') - pos;
  }

  return 0;
}

tStat * Statistics_FindByName(tStat * arr, int size, const char * name) {
  if(arr == NULL || size < 1 || name == NULL) {
    return NULL;
  }

  for(int i = 0; i < size; i++) {
    if(strcmp(arr[i].name, name) == 0) {
      return arr + i;
    }
  }

  return NULL;
}

#define READ_BUFFER_SIZE 128
void Statistics_Load( void ) {
  tGsFile* fp = FileOpen(PATH_STATISTIC_FLASH, "r");
  if(fp == NULL) {
    return;
  }

  char buffer[READ_BUFFER_SIZE];
  FileGets(buffer, READ_BUFFER_SIZE, fp);
  int timeqp = atoi(Statistics_GetValPtrByHeader(buffer, headers[STAT_NEXTTO]));
  Maint_Lifetime_SetNextServiceInSeconds(CtrlGet()->anual_maint, 30 * 60 * 60 * 24, timeqp * 60 * 60 * 24); //Write Maintenance

  FileGets(buffer, READ_BUFFER_SIZE, fp);
  int size = atoi(Statistics_GetValPtrByHeader(buffer, headers[STAT_ITEM_COUNTER]));
  if(size < 1) {
    FileClose(fp);
    return;
  }

  tStat * fileStats = calloc(sizeof(tStat), size);
  int filledSize = 0;
  for(; filledSize < size; filledSize++) {
    if(!FileGets(buffer, READ_BUFFER_SIZE, fp)) {
      break;
    }
    memcpy(fileStats[filledSize].name, Statistics_GetValPtrByHeader(buffer, headers[STAT_NAME]), Statistics_GetNameSize(buffer));
    fileStats[filledSize].maint_time = atoi(Statistics_GetValPtrByHeader(buffer, headers[STAT_TIME]));
    fileStats[filledSize].maint_time_reset = atoi(Statistics_GetValPtrByHeader(buffer, headers[STAT_TIME_RESET]));
    fileStats[filledSize].maint_dist = atof(Statistics_GetValPtrByHeader(buffer, headers[STAT_DISTANCE]));
  }

  FileClose(fp);

  for(int i = 0; i < GS_ARRAYELEMENTS(visuStat); i++) {
    tStat * tmp = Statistics_FindByName(fileStats, size, RCTextGetText(visuStat[i].textHDL,GetVarIndexed(IDX_SYS_LANGUAGE))); 
    if(tmp) {
      SetVar(visuStat[i].timeHDL, tmp->maint_time);
      SetVar(visuStat[i].timeHDL_R, tmp->maint_time_reset);
      SetVarFloat(visuStat[i].distHDL, tmp->maint_dist);
    }
    else {
      SetVar(visuStat[i].timeHDL, 0);
      SetVar(visuStat[i].timeHDL_R, 0);
      SetVarFloat(visuStat[i].distHDL, 0);
    }
  }

  free(fileStats);
}

void Statistics_LoadBacup( void ) {
  CopyFile(PATH_STATISTIC_USB, PATH_STATISTIC_FLASH, NULL);
  Statistics_Load();
}
