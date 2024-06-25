/****************************************************************************
*
* File:         VISU_MENU.c
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
#include "visu.h"
#include "RCText.h"
#include "visu_main.h"
#include "param.h"
#include "gsToVisu.h"
#include "pinout.h"

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

void Visu_Menu_Admin_Open( const tVisuData* VData ) {
	PrioMaskOn( MSK_MENU_ADMIN );
	if( RCTEXT_LI_USER_PASSWORD_MERKATOR == GetVarIndexed( IDX_USER ) ) {
		SetVar( HDL_ATTR_USER_MERKATOR, ATTR_VISIBLE | ATTR_TRANSPARENT | ATTR_NOMINAL );
		SetVar( HDL_ATT_USER_ADMIN, ATTR_VISIBLE | ATTR_TRANSPARENT | ATTR_NOMINAL );
	}
	else {
		SetVar( HDL_USER, RCTEXT_LI_USER_PASSWORD_ADMIN );
		SetVar( HDL_ATTR_USER_MERKATOR, 0 );
		SetVar( HDL_ATT_USER_ADMIN, ATTR_VISIBLE | ATTR_TRANSPARENT | ATTR_NOMINAL );
	}
}

void Visu_Menu_Admin_Cycle( const tVisuData* VData, uint32_t evtc, tUserCEvt* evtv ) {
	if( IsKeyPressedNew( 1 ) || IsKeyPressedNew( 6 ) ) {
		if( RCTEXT_LI_USER_PASSWORD_MERKATOR == GetVarIndexed( IDX_USER ) ) {
			Visu_OpenScreen( SCREEN_MENU_MERKATOR );
		}
		else {
			Password_Open( VData );
		}
	}
	if( IsMsgContainerOn( CNT_PASSWORD ) ) {
		Password_Cycle( VData, evtc, evtv );
	}

	for( uint32_t i = 0; i < evtc; i++ ) {
		if( CEVT_MENU_INDEX == evtv[i].Type ) {
			uint32_t ObjId = evtv[i].Content.mMenuIndex.ObjID;
			if( OBJ_MENUTEXT_PROFILE == ObjId ) {
				Visu_OpenScreen( SCREEN_PROFILE );
			}
			else if( OBJ_MENUTEXT_STATISTIC == ObjId ) {
				Visu_OpenScreen( SCREEN_STATISTIC );
			}
			else if( OBJ_MENUTEXT_BACKUP == ObjId ) {
				Visu_OpenScreen( SCREEN_BACKUP );
			}
			else if( OBJ_MENUTEXTMATERIAL == ObjId ) {
				Visu_OpenScreen( SCREEN_MATERIAL );
			}
			else if( OBJ_MENUTEXT_LEARNING == ObjId ) {
				Visu_OpenScreen( SCREEN_LEARNING );
			}
			else if( OBJ_MENUTEXT_SENSORS_1 ) {
				Visu_OpenScreen( SCREEN_SENSORS );
			}
		}
	}
	Visu_HomeKeyPressed();
}

void Visu_Menu_Merkator_Open( const tVisuData* VData ) {
	tParamData * Param = ParameterGet();
	PrioMaskOn( MSK_MENU_MERKATOR );
	SetVar( HDL_USER, RCTEXT_LI_USER_PASSWORD_MERKATOR );
	SetVar( HDL_ATTR_USER_MERKATOR, ATTR_VISIBLE | ATTR_TRANSPARENT | ATTR_NOMINAL );
	SetVar( HDL_ATT_USER_ADMIN, ATTR_VISIBLE | ATTR_TRANSPARENT | ATTR_NOMINAL );
	if((Param->VIN.ActualHydDistributor == HydDistributor_OMFB) || (Param->VIN.ActualHydDistributor == HydDistributor_OMFBv2)) { // OMFB
		SendToVisuObj(OBJ_MENUTEXT_BRUSHCALIBR, GS_TO_VISU_SET_ATTR_VISIBLE, 1);
	}
}

void Visu_Menu_Merkator_Cycle( const tVisuData* VData, uint32_t evtc, tUserCEvt* evtv ) {
	if( IsKeyPressedNew( 1 ) || IsKeyPressedNew( 6 ) ) {
		Visu_OpenScreen( SCREEN_MENU_ADMIN );
	}
	//Check Event system
	for( uint32_t i = 0; i < evtc; i++ ) {
		if( CEVT_MENU_INDEX == evtv[i].Type ) {
			uint32_t ObjId = evtv[i].Content.mMenuIndex.ObjID;
			if( OBJ_MENU_IO == ObjId ) {
				Visu_OpenScreen( SCREEN_IO_DEVICES );
			} else if(OBJ_MENU_PINOUT == ObjId) {
				Visu_OpenScreen( SCREEN_PINOUT );
			} else if( OBJ_MENUTEXT_MCM_CONFIG == ObjId ) {
				Visu_OpenScreen( SCREEN_MCM_CONFIG );
			}	else if( OBJ_MENU_PARAMETER == ObjId ) {
				Visu_OpenScreen( SCREEN_PARAMETERS );
			} else if( OBJ_MENUTEXT_FINECALIBRATION == ObjId ) {
				Visu_OpenScreen( SCREEN_FINE_CALIBRATION );
			} else if( OBJ_MENUTEXT_BRUSHCALIBR == ObjId ) {
				Visu_OpenScreen( SCREEN_CALIBRBRUSH );
			}
		}
	}
	Visu_HomeKeyPressed();
}
