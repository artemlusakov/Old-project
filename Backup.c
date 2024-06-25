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

#include <stdint.h>
#include <UserCEvents.h>  /* Events send to the Cycle function              */
#include <UserCAPI.h>     /* API-Function declarations                      */

#include "vartab.h"       /* Variable Table definitions:                    */
                          /* include this file in every C-Source to access  */
                          /* the variable table of your project.            */
                          /* vartab.h is generated automatically.           */

#include "objtab.h"       /* Object ID definitions:                         */
#include "sql_table.h"       /* Object ID definitions:                         */
#include "param.h"
#include "profile.h"
#include "visu_material.h"
#include "visu_statistik.h"
#include "pinout.h"
#include "visu_pinout.h"
#include "response_error.h"
#include "gse_msgbox.h"
#include "RCText.h"
#include "visu_sensors.h"
#include "gs_easy_config.h"
                          /* include this file in every C-Source to access  */
                          /* the object ID's of the visual objects.         */
                          /* objtab.h is generated automatically.           */

/* macro definitions ********************************************************/
#define PATH_VIN_EQUPS_VERSION_FLASH "/gs/data/Info.txt"
#define PATH_VIN_EQUPS_VERSION_USB "/gs/usb/BACKUP_LAST/Info.txt"
/* type definitions *********************************************************/

/* prototypes ***************************************************************/

/* global constants *********************************************************/

/* global variables *********************************************************/

/* local function prototypes ************************************************/

/* functions ****************************************************************/

/****************************************************************************
 * @brief  description
 *
 * @param  yourParam1
 * @param  yourParam1
 * @return none
 ****************************************************************************/
extern tParamData		Parameter;

void Backup_Open( void ) {
  PrioMaskOn(MSK_BACKUP);
}

void Backup_DirCheck( void ) {
  tGsDir * dir = DirOpen("/gs/usb/BACKUP_LAST/");
  if(dir == NULL) {
    MakeDir("/gs/usb/BACKUP_LAST");
  }
  DirClose(dir);
}

void VinEquipSV(tVisuData * VData) {
  char newpowerdaystat[15];
  char newpowermonthstat[15];
  char newpoweryearstat[15];
  tGsFile * fp = FileOpen(PATH_VIN_EQUPS_VERSION_FLASH, "w+");
  const char * FileDate = RCTextGetText(RCTEXT_T_FILECREATEDATE, GetVarIndexed(IDX_SYS_LANGUAGE));
  RTCGetDate( & VData -> Date);
  snprintf(newpowerdaystat, GS_ARRAYELEMENTS(newpowerdaystat), "%02d", VData -> Date.Day);
  snprintf(newpowermonthstat, GS_ARRAYELEMENTS(newpowermonthstat), "%02d", VData -> Date.Month);
  snprintf(newpoweryearstat, GS_ARRAYELEMENTS(newpoweryearstat), "%02d", VData -> Date.Year);
  FilePrintf(fp, "%s %s.%s.%s\n", FileDate, newpowerdaystat, newpowermonthstat, newpoweryearstat);
  FilePrintf(fp, "VIN %s\n", Parameter.VIN.VIN);
  FilePrintf(fp, "Equip1 %s\n", Parameter.VIN.Equip1);
  FilePrintf(fp, "Equip2 %s\n", Parameter.VIN.Equip2);
  FilePrintf(fp, "App %d\n", GetVar(HDL_APP));
  FilePrintf(fp, "OS %d\n", GetVar(HDL_OS));
  FileClose(fp);
  CopyFile(PATH_VIN_EQUPS_VERSION_FLASH, PATH_VIN_EQUPS_VERSION_USB, NULL);
  FileUnlink(fp);//delete at flash after write on usb
}

void CopyToBackups( void ) {//Copy files to archive path with VIN vehicle
  char stringinf[100];// set path
  char stringfoimport[100];//set files
  MakeDir("/gs/usb/ArchiveBackups");
  snprintf(stringinf, GS_ARRAYELEMENTS(stringinf), "%s%s", "/gs/usb/ArchiveBackups/BACKUP_", Parameter.VIN.VIN);
  MakeDir(stringinf);
  snprintf(stringfoimport, GS_ARRAYELEMENTS(stringfoimport), "%s%s", stringinf, "/SQLList.cfg");
  CopyFile(PATH_SQL_USB, stringfoimport, NULL);
  memset(stringfoimport, 0, sizeof(stringfoimport));
  SetVarIndexed(IDX_PROGRESSBAR_BUCKUP, 70);
  snprintf(stringfoimport, GS_ARRAYELEMENTS(stringfoimport), "%s%s", stringinf, "/param.cfg");
  CopyFile(PATH_PARAM_USB_BACKUP, stringfoimport, NULL);
  memset(stringfoimport, 0, sizeof(stringfoimport));
  snprintf(stringfoimport, GS_ARRAYELEMENTS(stringfoimport), "%s%s", stringinf, "/profiles.cfg");
  CopyFile(PROFILE_PATHUSB_BACKUP, stringfoimport, NULL);
  memset(stringfoimport, 0, sizeof(stringfoimport));
  SetVarIndexed(IDX_PROGRESSBAR_BUCKUP, 80);
  snprintf(stringfoimport, GS_ARRAYELEMENTS(stringfoimport), "%s%s", stringinf, "/reagent.cfg");
  CopyFile(PATH_REAGENTS_USB_BACKUP, stringfoimport, NULL);
  // memset(stringfoimport, 0, sizeof(stringfoimport));
  // snprintf(stringfoimport, GS_ARRAYELEMENTS(stringfoimport), "%s%s", stringinf, "/errorreact.cfg");
  // CopyFile(PATH_ERRORREACT_USB_BACKUP, stringfoimport, NULL);
  memset(stringfoimport, 0, sizeof(stringfoimport));
  snprintf(stringfoimport, GS_ARRAYELEMENTS(stringfoimport), "%s%s", stringinf, "/STATList.cfg");
  SetVarIndexed(IDX_PROGRESSBAR_BUCKUP, 90);
  // CopyFile(PATH_STATISTIC_USB, stringfoimport, NULL);

  memset(stringfoimport, 0, sizeof(stringfoimport));
  snprintf(stringfoimport, GS_ARRAYELEMENTS(stringfoimport), "%s%s", stringinf, "/pinout.cfg");
  CopyFile(PATH_PINOUT_BACKUP, stringfoimport, NULL);
  Light_SaveToBackup();
  PinOut_WriteAllToFile(PATH_PINOUT_BACKUP);
  // Statistics_SaveBacup();
  Sensors_SaveBacup();
  ErrorReaction_Export_Backup();

  // memset(stringfoimport, 0, sizeof(stringfoimport));
  // snprintf(stringfoimport, GS_ARRAYELEMENTS(stringfoimport), "%s%s", stringinf, "/hash.cfg");
  // CopyFile("/gs/usb/BACKUP_LAST/hash.cfg", stringfoimport, NULL);
  memset(stringfoimport, 0, sizeof(stringfoimport));
  snprintf(stringfoimport, GS_ARRAYELEMENTS(stringfoimport), "%s%s", stringinf, "/Info.txt");
  if (CopyFile(PATH_VIN_EQUPS_VERSION_USB, stringfoimport, NULL)) {
    SetVarIndexed(IDX_PROGRESSBAR_BUCKUP, 100);
    MsgBoxOK_RCText(RCTEXT_T_COPYTOUSB, RCTEXT_T_COPYTOUSBSUCCESS, NULL, NULL);
  } else {
    MsgBoxOK_RCText(RCTEXT_T_COPYTOUSB, RCTEXT_T_COPYTOUSBERROR, NULL, NULL);
  }
}

int8_t repair = 0;
void RepairAll( void ) {
  repair = 1;
}

void Save_Repair_Cycle(const tVisuData * VData, uint32_t evtc, tUserCEvt * evtv) {
  for (uint32_t i = 0; i < evtc; i++) {
    if (CEVT_MENU_INDEX == evtv[i].Type) {
      uint32_t ObjId = evtv[i].Content.mMenuIndex.ObjID;
      if (OBJ_SAVE_OBJ == ObjId) {
        Backup_DirCheck();
        WrSQL(); //Table
        SetVarIndexed(IDX_PROGRESSBAR_BUCKUP, 10);

        CopyFile(PATH_PARAM_FLASH, PATH_PARAM_USB_BACKUP, NULL); //Param
        SetVarIndexed(IDX_PROGRESSBAR_BUCKUP, 20);

        CopyFile(PROFILE_PATH, PROFILE_PATHUSB_BACKUP, NULL); //Profile
        SetVarIndexed(IDX_PROGRESSBAR_BUCKUP, 30);

        CopyFile(PATH_REAGENTS, PATH_REAGENTS_USB_BACKUP, NULL); //Reagents

        // CopyFile(PATH_ERRORREACT_FLASH, PATH_ERRORREACT_USB_BACKUP, NULL); //Error
        SetVarIndexed(IDX_PROGRESSBAR_BUCKUP, 50);

        VinEquipSV(VData); //VIN EQUIPS APP only read
        SetVarIndexed(IDX_PROGRESSBAR_BUCKUP, 60);

        CopyToBackups(); //copy in archive path
      }
      if (OBJ_REPAIR_OBJ == ObjId) {
        MsgBoxOkCancel(RCTextGetText(RCTEXT_T_INFO, GetVarIndexed(IDX_SYS_LANGUAGE)), RCTextGetText(RCTEXT_T_BUCKUPRECOVER, GetVarIndexed(IDX_SYS_LANGUAGE)), RepairAll, NULL, NULL, NULL);
      }
    }
  }
  if (repair) {
    SetVarIndexed(IDX_PROGRESSBAR_BUCKUP, 10);
    RpSQL();
    char buf[2000] = {0};
    tGsFile * fp = FileOpen(PATH_PARAM_USB_BACKUP, "r+");
    FileRead( & buf, 1, sizeof(buf), fp);
    FileClose(fp);
    //Loading from usb at flash
    if (FileSize(PATH_PARAM_USB_BACKUP)>0) {//Check availability file and her hash
      CopyFile(PATH_PARAM_USB_BACKUP, PATH_PARAM_FLASH, NULL);
      gsEasyConfig_Read( Param_GetECE(), Param_GetECE_Size(), PATH_PARAM_FLASH );
    } else {
      char stringinf[100];
      snprintf(stringinf, GS_ARRAYELEMENTS(stringinf), "%s %s", RCTextGetText(RCTEXT_T_FILENOTFOUND, GetVarIndexed(IDX_SYS_LANGUAGE)), "param.cfg");
      MsgBoxOk(RCTextGetText(RCTEXT_T_INFO, GetVarIndexed(IDX_SYS_LANGUAGE)), stringinf, NULL, NULL);
    }
    SetVarIndexed(IDX_PROGRESSBAR_BUCKUP, 60);
    if (FileSize(PROFILE_PATHUSB_BACKUP) > 0) {
      CopyFile(PROFILE_PATHUSB_BACKUP, PROFILE_PATH, NULL);
      Prof_Load();
      Profile_Change_Draw();
    } else {
      char stringinf[100];
      snprintf(stringinf, GS_ARRAYELEMENTS(stringinf), "%s %s", RCTextGetText(RCTEXT_T_FILENOTFOUND, GetVarIndexed(IDX_SYS_LANGUAGE)), "profiles.cfg");
      MsgBoxOk(RCTextGetText(RCTEXT_T_INFO, GetVarIndexed(IDX_SYS_LANGUAGE)), stringinf, NULL, NULL);
    }

    PinOut_ReadBackup(PATH_PINOUT_BACKUP);
    Light_LoadFromBackup();
    Sensors_LoadBacup();
    SetVarIndexed(IDX_PROGRESSBAR_BUCKUP, 70);

    Reagent_Import_FOBACKUP();
    ErrorReaction_Import_Backup();
    SetVarIndexed(IDX_PROGRESSBAR_BUCKUP, 100);

    MsgBoxOk(RCTextGetText(RCTEXT_T_INFO, GetVarIndexed(IDX_SYS_LANGUAGE)),
            RCTextGetText(RCTEXT_T_CONFIGMCMSUCCESSFULLY, GetVarIndexed(IDX_SYS_LANGUAGE)), NULL, NULL);
    repair = 0;
  } else {
    Visu_HomeKeyPressed();
    if (IsKeyPressedNew(1)) {
      Backup_DirCheck();
      Statistics_SaveBacup();
      MsgBoxOK_RCText(RCTEXT_T_INFO, RCTEXT_T_COPYTOUSBSUCCESS, NULL, NULL);
    }
    if (IsKeyPressedNew(2)) {
      Statistics_LoadBacup();
      MsgBoxOK_RCText(RCTEXT_T_INFO, RCTEXT_T_COPYFROMUSBSUCCESS, NULL, NULL);
    }
  }
}

