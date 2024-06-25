/****************************************************************************
*
* File:         VISU_PARAM.c
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
#include "gs_easy_config_menu.h"
#include "param.h"
#include "gse_msgbox.h"
#include "RCText.h"
#include "sql_table.h"
/****************************************************************************/

/* macro definitions ********************************************************/

/* type definitions *********************************************************/

/* prototypes ***************************************************************/

/* global constants *********************************************************/

/* global variables *********************************************************/
tCfgMenu CfgMenu;
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

void VisuParamSave_cb( void* arg ) {
	CfgMenu_WriteFile( &CfgMenu, PATH_PARAM_FLASH );
  InitParam();
  Visu_SQL_Vin_Equip();//VIN EQP update on SQL screen
  //Vin Eqp update on Properties screen
  const tParamData* Parameter = ParameterGet();
  SetVisObjData( OBJ_TRUETYPETEXT847, Parameter->VIN.VIN, strlen( Parameter->VIN.VIN ) + 1 );
	SetVisObjData( OBJ_TRUETYPETEXT849, Parameter->VIN.Equip1, strlen( Parameter->VIN.Equip1 ) + 1 );
	SetVisObjData( OBJ_TRUETYPETEXT859, Parameter->VIN.Equip2, strlen( Parameter->VIN.Equip2 ) + 1 );

	Visu_OpenScreen( SCREEN_MENU_MERKATOR );
}

void VisuParamCancel_cb( void* arg ) {
	Visu_OpenScreen( SCREEN_MENU_MERKATOR );
}

void Visu_Param_Init( const tVisuData* Data ) {
//	db_out( "Init Parameter page\r\n" );
	CfgMenu_Init( &CfgMenu );
	CfgMenu_AddLine( &CfgMenu, OBJ_PARAM_MENU_0, OBJ_PARAM_TEXT_0, OBJ_PARAM_VAR_0, HDL_PARAM_0 );
	CfgMenu_AddLine( &CfgMenu, OBJ_PARAM_MENU_1, OBJ_PARAM_TEXT_1, OBJ_PARAM_VAR_1, HDL_PARAM_1 );
	CfgMenu_AddLine( &CfgMenu, OBJ_PARAM_MENU_2, OBJ_PARAM_TEXT_2, OBJ_PARAM_VAR_2, HDL_PARAM_2 );
	CfgMenu_AddLine( &CfgMenu, OBJ_PARAM_MENU_3, OBJ_PARAM_TEXT_3, OBJ_PARAM_VAR_3, HDL_PARAM_3 );
	CfgMenu_AddLine( &CfgMenu, OBJ_PARAM_MENU_4, OBJ_PARAM_TEXT_4, OBJ_PARAM_VAR_4, HDL_PARAM_4 );
	CfgMenu_AddLine( &CfgMenu, OBJ_PARAM_MENU_5, OBJ_PARAM_TEXT_5, OBJ_PARAM_VAR_5, HDL_PARAM_5 );
	CfgMenu_AddLine( &CfgMenu, OBJ_PARAM_MENU_6, OBJ_PARAM_TEXT_6, OBJ_PARAM_VAR_6, HDL_PARAM_6 );
	CfgMenu_AddLine( &CfgMenu, OBJ_PARAM_MENU_7, OBJ_PARAM_TEXT_7, OBJ_PARAM_VAR_7, HDL_PARAM_7 );
	CfgMenu_AddLine( &CfgMenu, OBJ_PARAM_MENU_8, OBJ_PARAM_TEXT_8, OBJ_PARAM_VAR_8, HDL_PARAM_8 );
	CfgMenu_AddLine( &CfgMenu, OBJ_PARAM_MENU_9, OBJ_PARAM_TEXT_9, OBJ_PARAM_VAR_9, HDL_PARAM_9 );
	CfgMenu_SetTextColor( &CfgMenu, CONFIG_TYPE_GROUP, 0x00FFE97F );
	CfgMenu_SetTextColor( &CfgMenu, CONFIG_TYPE_LIST, 0x00FFBE93 );
	CfgMenu_SetTextColor( &CfgMenu, CONFIG_TYPE_INT, 0x007FFFFF );
	CfgMenu_SetTextColor( &CfgMenu, CONFIG_TYPE_STRING, 0x00A17FFF );
	CfgMenu_SetTextColor( &CfgMenu, CONFIG_TYPE_ARRAY, 0x00A5FF7F );
	CfgMenu_SetPathObj( &CfgMenu, OBJ_PARAMPATH );
}

void Visu_Param_Open( const tVisuData* Data ) {
//	db_out( "Read file form Flash: %s\r\n", PATH_PARAM_FLASH );
	CfgMenu_ReadFile( &CfgMenu, PATH_PARAM_FLASH );
//	db_out( "Draw:\r\n" );
	CfgMenu_Draw( &CfgMenu );
	PrioMaskOn( MSK_PARAMETER );
}

void Visu_Param_Cycle( const tVisuData* Data, uint32_t evtc, tUserCEvt* evtv ) {
	//&CfgMenu = CfgMenu_Create();
	CfgMenu_Evt( &CfgMenu, evtc, evtv );
	//Scroll up
	if( IsKeyPressedNew( 1 ) ) {
		CfgMenu_ScrollUp( &CfgMenu );
		CfgMenu_Draw( &CfgMenu );
	}
	//Scroll down
	else if( IsKeyPressedNew( 2 ) ) {
		CfgMenu_ScrollDown( &CfgMenu );
		CfgMenu_Draw( &CfgMenu );
	}
	//Save
	else if( IsKeyPressedNew( 3 ) ) {
//		db_out( "Save config\r\n" );
		CfgMenu_WriteFile( &CfgMenu, PATH_PARAM_FLASH );

	}
	//export to usb
	else if( IsKeyPressedNew( 4 ) ) {
		if( CopyFile( PATH_PARAM_FLASH, PATH_PARAM_USB, NULL ) ) {
			MsgBoxOk( RCTextGetText( RCTEXT_T_COPYTOUSB, GetVarIndexed( IDX_SYS_LANGUAGE ) ), RCTextGetText( RCTEXT_T_COPYTOUSBSUCCESS, GetVarIndexed( IDX_SYS_LANGUAGE ) ), NULL, NULL );
			Visu_Param_Open( NULL );
		}
		else {
			MsgBoxOk( RCTextGetText( RCTEXT_T_COPYTOUSB, GetVarIndexed( IDX_SYS_LANGUAGE ) ), RCTextGetText( RCTEXT_T_COPYTOUSBERROR, GetVarIndexed( IDX_SYS_LANGUAGE ) ), NULL, NULL );
		}
	}
	//import to usb
	else if( IsKeyPressedNew( 5 ) ) {
		if( CopyFile( PATH_PARAM_USB, PATH_PARAM_FLASH, NULL ) ) {
			MsgBoxOk( RCTextGetText( RCTEXT_T_COPYFROMUSB, GetVarIndexed( IDX_SYS_LANGUAGE ) ), RCTextGetText( RCTEXT_T_COPYFROMUSBSUCCESS, GetVarIndexed( IDX_SYS_LANGUAGE ) ), NULL, NULL );
			Visu_Param_Open( NULL );

		}
		else {
			MsgBoxOk( RCTextGetText( RCTEXT_T_COPYTOUSB, GetVarIndexed( IDX_SYS_LANGUAGE ) ), RCTextGetText( RCTEXT_T_COPYFROMUSBERROR, GetVarIndexed( IDX_SYS_LANGUAGE ) ), NULL, NULL );
		}
	}
	//go back in the file
	else if( IsKeyPressedNew( 6 ) ) {
		CfgMenu_Back( &CfgMenu );
	}
	//go home
	else if( IsKeyPressedNew( 10 ) ) {
		MsgBoxOkCancel( RCTextGetText( RCTEXT_T_SAVECHANGES, GetVarIndexed( IDX_SYS_LANGUAGE ) ), RCTextGetText( RCTEXT_T_SAVECHANGESTEXT, GetVarIndexed( IDX_SYS_LANGUAGE ) ), VisuParamSave_cb, NULL, VisuParamCancel_cb, NULL );
	}
}
