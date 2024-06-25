#include <stdint.h>
#include <UserCEvents.h>
#include <UserCAPI.h>
#include "vartab.h"
#include "objtab.h"
#include "visu.h"
#include "gse_msgbox.h"
#include "visu_start.h"
#include "visu_menu.h"
#include "visu_main.h"
#include "visu_io.h"
#include "visu_mcm_config.h"
#include "visu_error.h"
#include "visu_properties.h"
#include "visu_learning.h"
#include "visu_profile.h"
#include "visu_param.h"
#include "visu_spreadsettings.h"
#include "sql_table.h"
#include "visu_statistik.h"
#include "visu_pinout.h"
#include "visu_material.h"
#include "visu_sensors.h"
#include "gseerrorlist.h"
#include "RCText.h"
#include "bnso.h"
#include "backup.h"
#include "response_error.h"
#include "calibr_omfb.h"


tVisuScreen VisuScreens[] = {
	//Handle to open the screen         // called at device start           // Called every Cycle if the Screen is active //Called every 10 ms if screen active //Called if screen opens       // called if screen closed
	{.Hdl = SCREEN_START,			.Init_cb = NULL,				.Evt_cb = VisuStart_Cycle,			.Timer_10ms_cb = NULL,			.Open_cb = Visu_Start_Open,			.Close_cb = NULL},
	{.Hdl = SCREEN_MAIN,			.Init_cb = NULL,				.Evt_cb = Visu_Main_Cycle,			.Timer_10ms_cb = Visu_Main_Timer,	.Open_cb = Visu_Main_Open,		.Close_cb = Visu_Main_Close},
	{.Hdl = SCREEN_MENU_ADMIN,		.Init_cb = NULL,				.Evt_cb = Visu_Menu_Admin_Cycle,	.Timer_10ms_cb = NULL,			.Open_cb = Visu_Menu_Admin_Open,	.Close_cb = NULL},
	{.Hdl = SCREEN_MENU_MERKATOR,	.Init_cb = NULL,				.Evt_cb = Visu_Menu_Merkator_Cycle,	.Timer_10ms_cb = NULL,			.Open_cb = Visu_Menu_Merkator_Open,	.Close_cb = NULL},
	{.Hdl = SCREEN_IO_DEVICES,		.Init_cb = Visu_IO_Init,		.Evt_cb = Visu_IO_Cycle_cb,			.Timer_10ms_cb = Visu_IO_Timer_cb,	.Open_cb = Visu_IO_Open_cb,		.Close_cb = Visu_IO_Close_cb},
	{.Hdl = SCREEN_PINOUT,		.Init_cb = Visu_PinOut_Init,		.Evt_cb = Visu_PinOut_Cycle,			.Timer_10ms_cb = NULL,	.Open_cb = Visu_PinOut_Open,		.Close_cb = Visu_PinOut_Close},
	{.Hdl = SCREEN_MCM_CONFIG,	.Init_cb = NULL,			.Evt_cb = Reset_MCM_Cycle,	.Timer_10ms_cb = NULL,		.Open_cb = Reset_MCM_Open,	.Close_cb = NULL},
	{.Hdl = SCREEN_BACKUP,	.Init_cb = Visu_SQL_Table_Init,			.Evt_cb = Save_Repair_Cycle,	.Timer_10ms_cb = NULL,		.Open_cb = Backup_Open,	.Close_cb = NULL},
  {.Hdl = SCREEN_ERROR,			.Init_cb = Visu_Error_Init,		.Evt_cb = Visu_Error_Cycle,			.Timer_10ms_cb = NULL,			.Open_cb = Visu_Error_Open,			.Close_cb = Visu_Error_Close},
  {.Hdl = SCREEN_CHANGE_ERROR,		.Init_cb = Error_Signal_Init,	.Evt_cb = Error_Signal_Cycle,	.Timer_10ms_cb = NULL,		.Open_cb = Error_Signal_Open,	.Close_cb = NULL},
  {.Hdl = SCREEN_SENSORS,			.Init_cb = Visu_Sensor_Init,	.Evt_cb = Visu_Sensor_Cycle,		.Timer_10ms_cb = NULL,			.Open_cb = Visu_Sensor_Open,		.Close_cb = NULL},
	{.Hdl = SCREEN_PROPERTIES,		.Init_cb = Visu_Properties_Init,	.Evt_cb = Visu_Properties_Cycle,	.Timer_10ms_cb = NULL,		.Open_cb = Visu_Properties_Open,	.Close_cb = Visu_Properties_Close},
	{.Hdl = SCREEN_LEARNING,		.Init_cb = Visu_Learning_Init,	.Evt_cb = Visu_Learning_Cycle,		.Timer_10ms_cb = NULL,			.Open_cb = Visu_Learning_Open,		.Close_cb = Visu_Learning_Close},
	{.Hdl = SCREEN_PROFILE,			.Init_cb = NULL,				.Evt_cb = Visu_Profile_Cycle,		.Timer_10ms_cb = NULL,			.Open_cb = Visu_Profile_Open,		.Close_cb = NULL},
	{.Hdl = SCREEN_PROFILE_EDIT,	.Init_cb = NULL,				.Evt_cb = Visu_ProfileEdit_Cycle,	.Timer_10ms_cb = NULL,			.Open_cb = Visu_ProfileEdit_Open,	.Close_cb = NULL},
	{.Hdl = SCREEN_PROFILE_EDIT_RTR,	.Init_cb = NULL,				.Evt_cb = Visu_RTR_Edit_Cycle,	.Timer_10ms_cb = NULL,			.Open_cb = Visu_RTR_Edit_Open,	.Close_cb = NULL},
	{.Hdl = SCREEN_PARAMETERS,		.Init_cb = Visu_Param_Init,		.Evt_cb = Visu_Param_Cycle,			.Timer_10ms_cb = NULL,			.Open_cb = Visu_Param_Open,			.Close_cb = NULL},
	{.Hdl = SCREEN_FINE_CALIBRATION,	.Init_cb = Visu_SQL_Table_Init,	.Evt_cb = Visu_SQL_Table_Cycle,	.Timer_10ms_cb = NULL,			.Open_cb = Visu_SQL_Table_Open,		.Close_cb = NULL},
	{.Hdl = SCREEN_STATISTIC,		.Init_cb = NULL,				.Evt_cb = Visu_Statistic_Cycle,		.Timer_10ms_cb = NULL,			.Open_cb = Visu_Statistic_Open,		.Close_cb = NULL},
	{.Hdl = SCREEN_MATERIAL,		.Init_cb = VisuMaterial_Init,	.Evt_cb = VisuMaterial_Cycle,		.Timer_10ms_cb = NULL,			.Open_cb = VisuMaterial_Open,		.Close_cb = VisuMaterial_Close},
	{.Hdl = SCREEN_SPREADINGSETTINGS,	.Init_cb = NULL,			.Evt_cb = Visu_SpreadSettings_Cycle,	.Timer_10ms_cb = NULL,		.Open_cb = Visu_SpreadSettings_Open,	.Close_cb = Visu_SpreadSettings_Close},
	{.Hdl = SCREEN_CALIBRBRUSH,	.Init_cb = NULL,			.Evt_cb = Visu_CalibrBrush_Cycle,	.Timer_10ms_cb = NULL,		.Open_cb = Visu_CalibrBrush_Open,	.Close_cb = Visu_CalibrBrush_Close},
	{.Hdl = SCREEN_CALIBRTOP,	.Init_cb = NULL,			.Evt_cb = Visu_CalibrTop_Cycle,	.Timer_10ms_cb = NULL,		.Open_cb = Visu_CalibrTop_Open,	.Close_cb = Visu_CalibrTop_Close},
};

uint32_t CurrentScreenIdx = 0;
uint32_t PrevScreenIdx = 0;

tVisuData* _pVisuData;


void Visu_HomeKeyPressed(void) {
  if (IsKeyPressedNew(10)) {
    Visu_OpenScreen(SCREEN_MAIN);
  }
}

const tVisuData * Visu_GetVisuData(void) {
  return _pVisuData;
}

void Visu_MsgBoxErrCb(tGsError * Error) {
  if (Error -> newErr) { //only if the error hasn't been there, before.
    if (Error -> State == ES_UNCHECKED_ERROR_ACTIVE) {
      char Headline[128] = {
        0
      };

      switch (Error -> Level) {
      case VBL_INFO:
        snprintf(Headline, sizeof(Headline), RCTextGetText(RCTEXT_T_INFO, GetVarIndexed(IDX_SYS_LANGUAGE)));
        break;
      case VBL_WARNING:
        snprintf(Headline, sizeof(Headline), RCTextGetText(RCTEXT_T_WARNING, GetVarIndexed(IDX_SYS_LANGUAGE)));
        break;
      case VBL_ERROR:
        snprintf(Headline, sizeof(Headline), RCTextGetText(RCTEXT_T_ERROR, GetVarIndexed(IDX_SYS_LANGUAGE)));
        break;
      case VBL_ERROR_CRITICAL:
        snprintf(Headline, sizeof(Headline), RCTextGetText(RCTEXT_T_INFO, GetVarIndexed(IDX_SYS_LANGUAGE)));
        break;
      }

      if (RCTextGetText(RCTEXT_T_OVERSPEED, GetVarIndexed(IDX_SYS_LANGUAGE)) != Error -> text) {
        MsgBoxOk(Headline, Error -> text, WriteLastError, NULL);
      }
    }
  }
  BNSO_SendErrMsg(Error -> id, Error -> State);
}

void Visu_Init(tVisuData * VisuData) {

  _pVisuData = VisuData;
  for (uint32_t i = 0; i < GS_ARRAYELEMENTS(VisuScreens); i++) {
    if (NULL != VisuScreens[i].Init_cb) {
      VisuScreens[i].Init_cb(_pVisuData);
    }
  }
  Visu_OpenScreen(SCREEN_START);

  MsgBoxInit(CNT_MSGBOX,
    OBJ_MSGBOX_HEADLINE,
    OBJ_MSGBOXTEXT,
    OBJ_MSGBOXBUTTON_0,
    OBJ_MSGBOXBUTTON_1,
    OBJ_MSGBOXBUTTON_2,
    OBJ_MSGBOXTEXT_0,
    OBJ_MSGBOXTEXT_1,
    OBJ_MSGBOXTEXT_2);

  EList_SetErrorOut(Visu_MsgBoxErrCb);
}

tVisuScreen * Visu_GetVisuScreen(eVisuHdl VisuHdl) {
  for (uint32_t i = 0; i < GS_ARRAYELEMENTS(VisuScreens); i++) {
    if (VisuScreens[i].Hdl == VisuHdl) {
      return & VisuScreens[i];
    }
  }
  return NULL;
}

int32_t Visu_GetVisuScreenIdx(eVisuHdl VisuHdl) {
  for (uint32_t i = 0; i < GS_ARRAYELEMENTS(VisuScreens); i++) {
    if (VisuScreens[i].Hdl == VisuHdl) {
      return i;
    }
  }
  return -1;
}

/**
 * @brief Opens a new Screen. The function Visu_Close_cb of the current Screen,
 * definded in the array "const tVisuScreen Screens[]" in the file "visu.c" will be
 * called. After that the function Open_cb of the new Screen, also defined in
 * "const tVisuScreen Screens[]" will be called. The Actual Screen will be changed
 * to the new.
 *
 * @param Hdl Element of the enum eVisuHdl.
 */
void Visu_OpenScreen(eVisuHdl Hdl) {

  //PrioMaskOn(MSK_SQL_LIST);
  int32_t NewScreenIdx = Visu_GetVisuScreenIdx(Hdl);
  if (-1 != NewScreenIdx) {
    if (NULL != VisuScreens[CurrentScreenIdx].Close_cb) {
      const tVisuData * VisuData = Visu_GetVisuData();
      VisuScreens[CurrentScreenIdx].Close_cb(VisuData);
    }
    PrevScreenIdx = CurrentScreenIdx;
    CurrentScreenIdx = NewScreenIdx;
    if (NULL != VisuScreens[CurrentScreenIdx].Open_cb) {
      const tVisuData * VisuData = Visu_GetVisuData();
      VisuScreens[CurrentScreenIdx].Open_cb(VisuData);
    }
  }

}

/**
 * @brief this will call the Evt-Function of the actual screen
 *
 * @param evtc number of events
 * @param evtv Array of events
 */
void Visu_Cycle(uint32_t evtc, tUserCEvt * evtv) {
  WriteLastError();
  const tVisuData * VisuData = Visu_GetVisuData();
  if (NULL != VisuScreens[CurrentScreenIdx].Evt_cb) {
    VisuScreens[CurrentScreenIdx].Evt_cb(VisuData, evtc, evtv);
  } else {
    Visu_HomeKeyPressed();
  }
  UpdateErrorFile(VisuData);
  Reset();

  MsgBoxCycle(evtc, evtv);

  // Управление контейнером с тестовыми переменными.
  if (IsMaskOn(MSK_MAIN) == 1 && GetVar(HDL_LEARNING) != 1) {
    InfoContainerOn(CNT_IO_VAR_OPEN);
  } else {
    InfoContainerOff(CNT_IO_VAR_OPEN);
  }
}

/**
 * @brief this will call the timer function of the actual Screen
 *
 */
void Visu_Timer(void) {
  if (NULL != VisuScreens[CurrentScreenIdx].Timer_10ms_cb) {
    const tVisuData * VisuData = Visu_GetVisuData();
    VisuScreens[CurrentScreenIdx].Timer_10ms_cb(VisuData);
  }
}
