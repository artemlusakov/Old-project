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
                          /* include this file in every C-Source to access  */
                          /* the object ID's of the visual objects.         */
                          /* objtab.h is generated automatically.           */
#include "visu.h"
#include "visu_pinout.h"
#include "pinout.h"
#include "gse_msgbox.h"
#include "RCText.h"
#include "config/libconfig.h"
#include "gselist.h"
#include "control.h"
#include "profile.h"
#include "gsToVisu.h"

/* macro definitions ********************************************************/

/* type definitions *********************************************************/

/* prototypes ***************************************************************/

/* global constants *********************************************************/

/* global variables *********************************************************/

/* local function prototypes ************************************************/

/* functions ****************************************************************/

void Visu_PinOut_Init(void) {
}

void Visu_PinOut_Open(void) {
  PrioMaskOn( MSK_PINOUT );
  PinOut_Read();
}

void Visu_PinOut_Close(void) {
  InfoContainerOff(CNT_PINOUT_TOP);
  SetVarIndexed(IDX_PIN_ATTACHNAME, 0);
}

void Visu_PinOut_Save(void) {
  MsgBoxOkCancel(RCTextGetText(RCTEXT_T_SAVECHANGES, GetVarIndexed(IDX_SYS_LANGUAGE)), RCTextGetText(RCTEXT_T_SAVECHANGESTEXT,
  GetVarIndexed(IDX_SYS_LANGUAGE)), PinOut_Write, NULL, NULL, NULL);
  // PinOut_Write();
}

void PinOut_Export( void ) {
	if(PinOut_WriteAllToFile(PATH_PINOUT_USB)) {
		MsgBoxOK_RCText(RCTEXT_T_COPYTOUSB, RCTEXT_T_COPYTOUSBSUCCESS, NULL, NULL);
	} else {
		MsgBoxOK_RCText(RCTEXT_T_COPYTOUSB, RCTEXT_T_COPYTOUSBERROR, NULL, NULL);
  }
}

void PinOut_Import( void ) {
	if( FileSize(PATH_PINOUT_USB) > 0) {
		MsgBoxOK_RCText(RCTEXT_T_COPYFROMUSB, RCTEXT_T_COPYFROMUSBSUCCESS, NULL, NULL);
    PinOut_ReadBackup(PATH_PINOUT_USB);
  }	else {
		MsgBoxOK_RCText(RCTEXT_T_COPYFROMUSB, RCTEXT_T_COPYFROMUSBERROR, NULL, NULL);
  }
}

void PinOut_Import_Backup(void) {
  if (FileSize(PATH_PINOUT_USB) > 0) {
    MsgBoxOK_RCText(RCTEXT_T_COPYFROMUSB, RCTEXT_T_COPYFROMUSBSUCCESS, NULL, NULL);
    PinOut_ReadBackup(PATH_PINOUT_USB);
  } else {
    char stringinf[100];
    snprintf(stringinf, GS_ARRAYELEMENTS(stringinf), "%s %s", RCTextGetText(RCTEXT_T_FILENOTFOUND, GetVarIndexed(IDX_SYS_LANGUAGE)), "pinout.cfg");
    MsgBoxOk(RCTextGetText(RCTEXT_T_INFO, GetVarIndexed(IDX_SYS_LANGUAGE)), stringinf, NULL, NULL);
  }
}

void Visu_ShowTiltLock( void ) {
  const tProfile * Prof = Profile_GetCurrentProfile();
  if(Prof->module[3] == RCTEXT_LI_C_ATTACHMENT_ATTACHMENT_AA_AC) {
    if(GetVarIndexed(IDX_PIN_ATTACHNAME) == 2) {
      SendToVisuObj(OBJ_ATRESTTTEXTLIST1142, GS_TO_VISU_SET_ATTR_VISIBLE, 1);
      SendToVisuObj(OBJ_ATRESTTTEXTLIST1142, GS_TO_VISU_SET_ORIGIN_X, 615);
      SendToVisuObj(OBJ_ATRESTTTEXTLIST1143, GS_TO_VISU_SET_ATTR_VISIBLE, 1);
      SendToVisuObj(OBJ_ATRESTTTEXTLIST1143, GS_TO_VISU_SET_ORIGIN_X, 736);
      SendToVisuObj(OBJ_ATRESTTMENUTEXT1329, GS_TO_VISU_SET_ATTR_VISIBLE, 1);
    } else {
      SendToVisuObj(OBJ_ATRESTTTEXTLIST1142, GS_TO_VISU_SET_ATTR_VISIBLE, 0);
      SendToVisuObj(OBJ_ATRESTTTEXTLIST1143, GS_TO_VISU_SET_ATTR_VISIBLE, 0);
      SendToVisuObj(OBJ_ATRESTTMENUTEXT1329, GS_TO_VISU_SET_ATTR_VISIBLE, 0);
    }
  }
}

void Visu_ShowSecondSection( void ) {
  const tProfile * Prof = Profile_GetCurrentProfile();
  if(Prof->module[1] == RCTEXT_LI_A_ATTACHMENT_ATTACHMENT_MSPN) {
    if(GetVarIndexed(IDX_PIN_ATTACHNAME) == 1) {
      SendToVisuObj(OBJ_ATRESTTTEXTLIST1142, GS_TO_VISU_SET_ATTR_VISIBLE, 1);
      SendToVisuObj(OBJ_ATRESTTTEXTLIST1142, GS_TO_VISU_SET_ORIGIN_X, 600);
      SendToVisuObj(OBJ_ATRESTTTEXTLIST1143, GS_TO_VISU_SET_ATTR_VISIBLE, 1);
      SendToVisuObj(OBJ_ATRESTTTEXTLIST1143, GS_TO_VISU_SET_ORIGIN_X, 725);
      SendToVisuObj(OBJ_ATRESTTMENUTEXT1330, GS_TO_VISU_SET_ATTR_VISIBLE, 1);
    } else {
      SendToVisuObj(OBJ_ATRESTTTEXTLIST1142, GS_TO_VISU_SET_ATTR_VISIBLE, 0);
      SendToVisuObj(OBJ_ATRESTTTEXTLIST1143, GS_TO_VISU_SET_ATTR_VISIBLE, 0);
      SendToVisuObj(OBJ_ATRESTTMENUTEXT1330, GS_TO_VISU_SET_ATTR_VISIBLE, 0);
    }
  }
}

void Visu_PinOut_Cycle(uint32_t evtc, tUserCEvt * evtv) {
  PinOut_Load();
  Visu_ShowTiltLock();
  Visu_ShowSecondSection();

  if(GetVarIndexed(IDX_PIN_ATTACHNAME) == 3) {
    InfoContainerOn(CNT_PINOUT_TOP);
  } else {
    InfoContainerOff(CNT_PINOUT_TOP);
  }

  if(IsKeyPressedNew(1)) {
    PinOut_Export();
  }
  if(IsKeyPressedNew(2)) {
    Visu_PinOut_Save();
  }
  if(IsKeyPressedNew(3)) {
    PinOut_Import();
  }

  Visu_HomeKeyPressed();
}
