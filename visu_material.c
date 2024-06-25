/****************************************************************************
*
* File:         VISU_MATERIAL.c
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
#include "gselist.h"
#include "visu_material.h"
#include "config/libconfig.h"
#include "gse_msgbox.h"
#include "RCText.h"

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

tReagentTable _RegTable;

tReagent DefaultReagents[] = {
	{.Name = "Соль",	.MaterialDensity = 1100,	.SpreadingDensity_Max = 100,	.SpreadingDensity_Step = 5},
	{.Name = "ПСС",		.MaterialDensity = 1400,	.SpreadingDensity_Max = 400,	.SpreadingDensity_Step = 40},
	{.Name = "Песок",	.MaterialDensity = 1600,	.SpreadingDensity_Max = 400,	.SpreadingDensity_Step = 40},
	{.Name = "Щебень",	.MaterialDensity = 2000,	.SpreadingDensity_Max = 400,	.SpreadingDensity_Step = 40},
};

void Reagent_LoadDefault( tReagentTable* RegTable ) {
	tGSList* List = &RegTable->ReagentList;

	for( uint32_t i = 0; i < GS_ARRAYELEMENTS( DefaultReagents ); i++ ) {
		DefaultReagents[i].idx = List->numItems;
		GSList_AddData( List, &DefaultReagents[i], sizeof( tReagent ) );
	}
}

int32_t Reagent_AddReagentToCfg( config_t* cfg, tReagent* Reagent ) {
	config_setting_t* set_Reagent;
	config_setting_t* root = config_root_setting( cfg );
	config_setting_t* setting;
	if( NULL == (set_Reagent = config_setting_get_member( root, "Reagents" )) ) {
		set_Reagent = config_setting_add( root, "Reagents", CONFIG_TYPE_LIST );
	}
	config_setting_t* group = config_setting_add( set_Reagent, "Reagent", CONFIG_TYPE_GROUP );
	setting = config_setting_add( group, "idx", CONFIG_TYPE_INT );
	config_setting_set_int( setting, Reagent->idx );
	setting = config_setting_add( group, "Name", CONFIG_TYPE_STRING );
	config_setting_set_string( setting, Reagent->Name );
	setting = config_setting_add( group, "MaterialDenisty", CONFIG_TYPE_INT );
	config_setting_set_int( setting, Reagent->MaterialDensity );
	setting = config_setting_add( group, "SpreadingDens_Max", CONFIG_TYPE_INT );
	config_setting_set_int( setting, Reagent->SpreadingDensity_Max );
	setting = config_setting_add( group, "SpreadingDens_Step", CONFIG_TYPE_INT );
	config_setting_set_int( setting, Reagent->SpreadingDensity_Step );
	return 0;
}

int32_t Reagent_SaveData( tReagentTable* RegTab ) {
	tGsFile* fp = FileOpen( PATH_REAGENTS, "w" );
	if( NULL == fp )
		return -1;
	config_t cfg;
	config_init( &cfg );
	tReagent* Reagent = GSList_GetFirstData( &RegTab->ReagentList );
	while( Reagent != NULL ) {
		Reagent_AddReagentToCfg( &cfg, Reagent );
		Reagent = GSList_GetNextData( &RegTab->ReagentList );
	}
	config_write( &cfg, fp );
	FileSync( fp );
	FileClose( fp );
	return 0;
}

int32_t Reagent_Search_cb( void* Data, void* arg ) {
	int32_t idx = (int32_t)Data;
	tReagent* Reagent = (tReagent*)arg;
	return idx - Reagent->idx;
}

void Reagent_Load( tReagentTable* RegTab ) {
	tGsFile* fp = FileOpen( PATH_REAGENTS, "r" );
	if( NULL == fp ) {
		Reagent_LoadDefault( RegTab );
		Reagent_SaveData( RegTab );
		fp = FileOpen( PATH_REAGENTS, "r" );
	}
	config_t cfg;
	config_init( &cfg );
	//File auslesen

	if( CONFIG_FALSE == config_read( &cfg, fp ) ) {
//		db_out( "InitFile_ReadFile: Error reading InitFile" );
//		db_out( "File: %s,Line: %s,Text: %s,Type: %s", cfg.error_file, cfg.error_line, cfg.error_text, cfg.error_type );
	}
	FileClose( fp );

	config_setting_t* set_Reagents = config_lookup( &cfg, "Reagents" );
	if( NULL != set_Reagents ) {
		GSList_Destroy( &RegTab->ReagentList );
		RegTab->Actual = NULL;
		GSList_Init( &RegTab->ReagentList );
		for( int32_t i = 0; i < config_setting_length( set_Reagents ); i++ ) {
			tReagent Reagent;
			config_setting_t* set;
			config_setting_t* entry = config_setting_get_elem( set_Reagents, i );
			set = config_setting_get_member( entry, "idx" );
			Reagent.idx = config_setting_get_int( set );
			set = config_setting_get_member( entry, "MaterialDenisty" );
			Reagent.MaterialDensity = config_setting_get_int( set );
			set = config_setting_get_member( entry, "SpreadingDens_Max" );
			Reagent.SpreadingDensity_Max = config_setting_get_int( set );
			set = config_setting_get_member( entry, "SpreadingDens_Step" );
			Reagent.SpreadingDensity_Step = config_setting_get_int( set );
			set = config_setting_get_member( entry, "Name" );
			strncpy( Reagent.Name, config_setting_get_string( set ), sizeof( Reagent.Name ) );

			GSList_AddData( &RegTab->ReagentList, &Reagent, sizeof( tReagent ) );
		}
	}
	RegTab->Actual = GSList_SearchData( &RegTab->ReagentList, (void*)GetVarIndexed( IDX_REAGENT ) - 1, Reagent_Search_cb );
	if( NULL == RegTab->Actual ) {
		RegTab->Actual = GSList_GetFirstData( &RegTab->ReagentList );
	}
}

const tReagent* GetActualReagent( void ) {
	return _RegTable.Actual;
}

void Reagent_Draw( void ) {
	if( NULL != _RegTable.Actual ) {
		if( NULL != _RegTable.Actual->Name ) {
			SetVisObjData( OBJ_REAGENTNAME, _RegTable.Actual->Name, strlen( _RegTable.Actual->Name ) + 1 );
			SetVisObjData( OBJ_REAGENTNAME_1, _RegTable.Actual->Name, strlen( _RegTable.Actual->Name ) + 1 );
		}
		SetVar( HDL_MATERIALDENSITY, _RegTable.Actual->MaterialDensity );
		SetVar( HDL_DENSITYMAX, _RegTable.Actual->SpreadingDensity_Max );
		SetVar( HDL_DENSITYSTEPWIDTH, _RegTable.Actual->SpreadingDensity_Step );
		SetVar( HDL_REAGENT, _RegTable.Actual->idx +1 );
	}
	else {
		SetVisObjData( OBJ_REAGENTNAME, "No reagent", strlen( "No reagent" ) + 1 );
		SetVisObjData( OBJ_REAGENTNAME_1, "No reagent", strlen( "No reagent" ) + 1 );
		SetVar( HDL_MATERIALDENSITY, 0 );
		SetVar( HDL_DENSITYMAX, 0 );
		SetVar( HDL_DENSITYSTEPWIDTH, 0 );
		SetVar( HDL_REAGENT, 0 );
	}

}

void Reagent_Next( void ) {
	_RegTable.Actual = GSList_GetNextData( &_RegTable.ReagentList );
	if( NULL == _RegTable.Actual ) {
		_RegTable.Actual = GSList_GetFirstData( &_RegTable.ReagentList );
	}
}
void Reagent_Prev( void ) {
	_RegTable.Actual = GSList_GetPrevData( &_RegTable.ReagentList );
	if( NULL == _RegTable.Actual ) {
		_RegTable.Actual = GSList_GetLastData( &_RegTable.ReagentList );
	}
}

int32_t Reagent_GetNum( void ) {
	return _RegTable.ReagentList.numItems;
}

const tReagent* Reagent_SetIdx( uint32_t idx ) {
	if( idx >= _RegTable.ReagentList.numItems ) {
		return NULL;
	}
	_RegTable.Actual = GSList_GetFirstData( &_RegTable.ReagentList );
	for( uint32_t i = 0; i < idx; i++ ) {
		_RegTable.Actual = GSList_GetNextData( &_RegTable.ReagentList );
	}
	return _RegTable.Actual;
}

int32_t Reagent_GetIdx( void ) {
	int32_t idx = 0;
	tGSList_Item* ActItem = _RegTable.ReagentList.first;
	if( ActItem == NULL )
		return -1;
	while( ActItem->data != _RegTable.Actual ) {
		ActItem = ActItem->next;
		idx++;
	}
	return idx;
}

void Reagent_VisuToEntry( void ) {
	if( NULL != _RegTable.Actual ) {
		GetVisObjData( OBJ_REAGENTNAME_1, _RegTable.Actual->Name, sizeof( _RegTable.Actual->Name ) );
		_RegTable.Actual->MaterialDensity = GetVar( HDL_MATERIALDENSITY );
		_RegTable.Actual->SpreadingDensity_Max = GetVar( HDL_DENSITYMAX );
		_RegTable.Actual->SpreadingDensity_Step = GetVar( HDL_DENSITYSTEPWIDTH );
	}
}

void Reagent_Save( void ) {

	Reagent_VisuToEntry();
	Reagent_SaveData( &_RegTable );
}

void Reagent_Add( void ) {
	tReagent NewReagent;
	if( NULL != _RegTable.ReagentList.last ) {
		tReagent* LastReagent;
		LastReagent = _RegTable.ReagentList.last->data;
		if( NULL != LastReagent ) {
			NewReagent.idx = LastReagent->idx + 1;
		}
	}
	strncpy( NewReagent.Name, "New Regent", sizeof( NewReagent.Name ) );
	NewReagent.MaterialDensity = 100;
	NewReagent.SpreadingDensity_Max = 50;
	NewReagent.SpreadingDensity_Step = 5;
	_RegTable.Actual = GSList_AddData( &_RegTable.ReagentList, &NewReagent, sizeof( tReagent ) );
	Reagent_Draw();
}

void Reagent_Remove( void ) {
	if( NULL != &_RegTable.Actual ) {
		GSList_DeleteData( &_RegTable.ReagentList, _RegTable.Actual );
		_RegTable.Actual = GSList_GetFirstData( &_RegTable.ReagentList );
		Reagent_Draw();
	}
}

void Reagent_Export( void ) {
	if( CopyFile( PATH_REAGENTS, PATH_REAGENTS_USB, NULL ) )
		MsgBoxOK_RCText( RCTEXT_T_COPYTOUSB, RCTEXT_T_COPYTOUSBSUCCESS, NULL, NULL );
	else
		MsgBoxOK_RCText( RCTEXT_T_COPYTOUSB, RCTEXT_T_COPYTOUSBERROR, NULL, NULL );
}

void Reagent_Import( void ) {
	if( CopyFile( PATH_REAGENTS_USB, PATH_REAGENTS, NULL ) ) {
		MsgBoxOK_RCText( RCTEXT_T_COPYFROMUSB, RCTEXT_T_COPYFROMUSBSUCCESS, NULL, NULL );
		Reagent_Load( &_RegTable );
		Reagent_Draw();
	}
	else
		MsgBoxOK_RCText( RCTEXT_T_COPYFROMUSB, RCTEXT_T_COPYFROMUSBERROR, NULL, NULL );

}
void Reagent_Import_FOBACKUP( void ) {
  if(FileSize(PATH_REAGENTS_USB_BACKUP)>0){
    if( CopyFile( PATH_REAGENTS_USB_BACKUP, PATH_REAGENTS, NULL ) ) {// different file path
      Reagent_Load( &_RegTable );
      Reagent_Draw();
    }
  } else {
     char stringinf[100];
     snprintf(stringinf, GS_ARRAYELEMENTS(stringinf), "%s %s", RCTextGetText(RCTEXT_T_FILENOTFOUND, GetVarIndexed(IDX_SYS_LANGUAGE)), "reagent.cfg");
     MsgBoxOk( RCTextGetText( RCTEXT_T_INFO, GetVarIndexed( IDX_SYS_LANGUAGE ) ), stringinf, NULL, NULL );
  }
}

void VisuMaterial_Init( const tVisuData* Data ) {
	GSList_Init( &_RegTable.ReagentList );
	Reagent_Load( &_RegTable );
}

void VisuMaterial_Cycle( const tVisuData* Data, uint32_t evtc, tUserCEvt* evtv ) {
	for( uint32_t i = 0; i < evtc; i++ ) {
		if( CEVT_EDIT_END == evtv[i].Type ) {
			if( OBJ_MAXDENSITY == evtv[i].Content.mEditEnd.ObjID ) {
				SetVar( HDL_DENSITYSTEPWIDTH, GetVar( HDL_DENSITYMAX ) / 12 );
			}
		}
	}

	switch( GetCurrentMaskShown() ) {
	case MSK_MATERIAL: {
		if( IsKeyPressedNew( 7 ) ||
			IsKeyPressedNew( 104 ) ) {
			Reagent_Prev();
			Reagent_Draw();
		}
		if( IsKeyPressedNew( 9 ) ||
			IsKeyPressedNew( 103 ) ) {
			Reagent_Next();
			Reagent_Draw();
		}
		//Open Edit Container only for admin for admin
		if( GetVarIndexed( IDX_ATT_USER_ADMIN ) ) {
			void(*func[])(void) = {
			  Reagent_Add,
			  Reagent_Remove,
			  Reagent_Save,
			  Reagent_Export,
			  Reagent_Import
			};
			for( uint32_t i = 0; i < GS_ARRAYELEMENTS( func ); i++ ) {
				if( IsKeyPressedNew( i + 1 ) ) {
					func[i]();
				}
			}
			if( IsKeyPressedNew( 6 ) ) {
				Reagent_VisuToEntry();
				Reagent_Draw();
				PrioMaskOn( MSK_MATERIALEDIT );
			}
		}
		SetVarIndexed( IDX_ATTR_EDIT, ATTR_VISIBLE | ATTR_TRANSPARENT );
	}
					 break;
					 /*
						 case MSK_MAIN:
							if(IsKeyPressedNew(7))
							 {
							   Reagent_Next();
							 Reagent_Draw();
							 }
						   break;
					 */

	case MSK_MATERIALEDIT:
		//go back
		if( IsKeyPressedNew( 6 ) ) {
			PrioMaskOn( MSK_MATERIAL );
			Reagent_VisuToEntry();
			Reagent_Draw();
		}
		SetVarIndexed( IDX_ATTR_EDIT, ATTR_VISIBLE | ATTR_TRANSPARENT | ATTR_NOMINAL );
		break;
	default:
		PrioMaskOn( MSK_MATERIAL );
		break;
	}
	Visu_HomeKeyPressed();
}

void VisuMaterial_Open( const tVisuData* Data ) {
	PrioMaskOn( MSK_MATERIAL );

	Reagent_Draw();
}
void VisuMaterial_Close( const tVisuData* Data ) {
	MaskOff( MSK_MATERIALEDIT );
	MaskOff( MSK_MATERIAL );
}
