/****************************************************************************
*
* File:         GS_EASY_CONFIG_MENU.c
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

#include "objtab.h"
#include "gsToVisu.h"
#include "gsLkfCodes.h"

#include "gs_easy_config_menu.h"
#include "param.h"

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

void CfgMenu_Init( tCfgMenu* cfgm ) {

	memset( cfgm, 0, sizeof( tCfgMenu ) );
	config_init( &cfgm->cfg );
	cfgm->ActSet = config_root_setting( &cfgm->cfg );
//	db_out( "Allocated Memory for Config Menu\r\n" );
}

uint32_t CfgMenu_AddLine( tCfgMenu* cfgm, uint32_t MenuTextObjId, uint32_t EditRessoureTextObjId, uint32_t TrueTypeVarObjId, uint32_t TrueTypeVarHdl ) {
	tCfgMenuLine* NewLines = realloc( cfgm->Lines, sizeof( tCfgMenuLine ) * (1 + cfgm->Lines_Num) );
	if( NULL == NewLines ) {
		return CFGM_ERROR_ALLOCATING_MEMMORY;
	}
	tCfgMenuLine* NewLine = &NewLines[cfgm->Lines_Num];
	cfgm->Lines_Num++;
	cfgm->Lines = NewLines;
	NewLine->MenuText_ObjId = MenuTextObjId;
	NewLine->EditRessourceText_ObjId = EditRessoureTextObjId;
	NewLine->TrueTypeVar_ObjId = TrueTypeVarObjId;
	NewLine->TrueTypeVar_Hdl = TrueTypeVarHdl;

	NewLine->TrueTypeVarFloat_ObjId = -1;
	NewLine->TrueTypeVarFloat_Hdl = -1;

	SendToVisuObj( NewLine->EditRessourceText_ObjId, GS_TO_VISU_SET_ATTR_ALL, 0 );
	SendToVisuObj( NewLine->MenuText_ObjId, GS_TO_VISU_SET_ATTR_ALL, 0 );
	SendToVisuObj( NewLine->TrueTypeVar_ObjId, GS_TO_VISU_SET_ATTR_ALL, 0 );
	return CFGM_OK;
}

uint32_t CfgMenu_AddFloatToLine( tCfgMenu* cfgm, uint32_t line, int32_t TrueTypeVarFloatObj, int32_t TrueTypeVarFloatHdl ) {
	if( line < cfgm->Lines_Num ) {
		cfgm->Lines[line].TrueTypeVarFloat_Hdl = TrueTypeVarFloatHdl;
		cfgm->Lines[line].TrueTypeVarFloat_ObjId = TrueTypeVarFloatObj;
		return CFGM_OK;
	}
	return CFGM_ERROR_LINE_DOES_NOT_EXIST;
}

uint32_t CfgMenu_SetTextColor( tCfgMenu* cfgm, uint8_t ConfigType, uint32_t color ) {
	if( ConfigType < 9 ) {
		cfgm->TextColor[ConfigType] = color;
	}
	else return CFGM_ERROR_INVALID_CONFIG_TPPE;
	return CFGM_OK;
}

uint32_t CfgMenu_SetPathObj( tCfgMenu* cfgm, uint32_t TextObjId ) {
	cfgm->PathObj = TextObjId;
	return CFGM_OK;
}

uint32_t CfgMenu_ReadFile( tCfgMenu* cfgm, const char* path ) {
//	db_out( "Cfg_OpenFile\r\n" );
	tGsFile* fp = FileOpen( path, "r" );
	if( NULL == fp )
		return CFGM_ERROR_READING_CONFIG_FILE;
	config_init( &(cfgm->cfg) );
//	db_out( "Cfg_Opened File successfully\r\n" );
	int ret = config_read( &(cfgm->cfg), fp );
	if( ret != CONFIG_TRUE ) {
//		db_out( "cfg Errotext: %s\r\n", config_error_text( &cfgm->cfg ) );
//		db_out( "cfg Erroline: %d\r\n", config_error_line( &cfgm->cfg ) );
		return 1;
	}
//	db_out( "Read config" );
	cfgm->ActSet = config_root_setting( &cfgm->cfg );
	FileClose( fp );
	const char** NewFilenames = realloc( cfgm->cfg.filenames, sizeof( char** ) * (1 + cfgm->cfg.num_filenames) );
	if( NULL == NewFilenames )
		return CFGM_ERROR_ALLOCATING_MEMMORY;
	cfgm->cfg.filenames = NewFilenames;
	cfgm->cfg.filenames[0] = strdup( path );
	cfgm->cfg.num_filenames = 1;
	if( ret == CONFIG_TRUE )
		return CFGM_OK;
	else return CFGM_ERROR_READING_CONFIG_FILE;
	//return config_read_file(&cfgm->cfg, path);
}

uint32_t CfgMenu_WriteFile( tCfgMenu* cfgm, const char* path ) {

	tGsFile* fp = FileOpen( path, "w" );
	if( NULL != fp ) {
		config_write( &cfgm->cfg, fp );
		FileClose( fp );
		return CFGM_OK;
	}
	return CFGM_ERROR_WRITING_CONFIG_FILE;
}

void _GetPath( config_setting_t* set, char* buf, size_t bufsize ) {
	if( NULL == set->parent ) {
		if( NULL != set->config->filenames ) {
			if( NULL != set->name ) {
				snprintf( buf, bufsize, "%s", set->name );
			}
			else snprintf( buf, bufsize, "Root" );
		}

	}
	else {
		_GetPath( set->parent, buf, bufsize );
		if( NULL != set->name ) {
			strncat( buf, ".", bufsize - strlen( buf ) );
			strncat( buf, set->name, bufsize - strlen( buf ) );
		}
		else {
			switch( set->parent->type ) {
			case CONFIG_TYPE_GROUP:
			case CONFIG_TYPE_LIST:
			case CONFIG_TYPE_ARRAY: {
				uint32_t i;
				for( i = 0; i < set->parent->value.list->length; i++ ) {
					if( set == set->parent->value.list->elements[i] ) {
						break;
					}
				}
				char string[32];
				snprintf( string, sizeof( string ), ".[%u]", i );
				strncat( buf, string, bufsize - strlen( buf ) );
			}
								  break;

			default:
				break;
			}
		}
	}
}

int32_t CfgMenu_GetPath( tCfgMenu* cfgm, char* buf, size_t bufsize ) {
	_GetPath( cfgm->ActSet, buf, bufsize );
	return strlen( buf );
}

void CfgMenu_DrawGroup( tCfgMenu* cfgm ) {
	if( NULL == cfgm->ActSet->value.list )
		return;
	config_setting_t** elements = cfgm->ActSet->value.list->elements;
	uint32_t numElements = cfgm->ActSet->value.list->length;
	uint32_t i;
	for( i = cfgm->offset; (i < numElements) & (i < cfgm->Lines_Num + cfgm->offset); i++ ) {
		config_setting_t* ActElement = elements[i];
		tCfgMenuLine* ActLine = &cfgm->Lines[i - cfgm->offset];
		if( NULL != ActElement->name ) {
			// SetVisObjData( ActLine->MenuText_ObjId, ActElement->name, strlen( ActElement->name ) + 1 );
			char* r = translateParam(ActElement->name);
			int len = strlen(r);
			SetVisObjData( ActLine->MenuText_ObjId, r, len + 1 );
		}
		else {
			char string[32];
			sprintf( string, "%0u:", i );
			SetVisObjData( ActLine->MenuText_ObjId, string, strlen( string ) + 1 );
		}

		switch( ActElement->type ) {
		case CONFIG_TYPE_NONE:
			SendToVisuObj( ActLine->EditRessourceText_ObjId, GS_TO_VISU_SET_ATTR_ALL, 0 );
			SendToVisuObj( ActLine->MenuText_ObjId, GS_TO_VISU_SET_ATTR_ALL, 0 );
			SendToVisuObj( ActLine->TrueTypeVar_ObjId, GS_TO_VISU_SET_ATTR_ALL, 0 );
			break;

		case CONFIG_TYPE_GROUP:
			SendToVisuObj( ActLine->EditRessourceText_ObjId, GS_TO_VISU_SET_ATTR_ALL, 0 );
			SendToVisuObj( ActLine->MenuText_ObjId, GS_TO_VISU_SET_ATTR_ALL, ATTR_VISIBLE | ATTR_SMOOTH | ATTR_NOMINAL );
			SendToVisuObj( ActLine->MenuText_ObjId, GS_TO_VISU_SET_FG_COLOR, cfgm->TextColor[CONFIG_TYPE_GROUP] );
			SendToVisuObj( ActLine->TrueTypeVar_ObjId, GS_TO_VISU_SET_ATTR_ALL, 0 );
			break;

		case CONFIG_TYPE_INT:
			SendToVisuObj( ActLine->EditRessourceText_ObjId, GS_TO_VISU_SET_ATTR_ALL, 0 );
			SendToVisuObj( ActLine->MenuText_ObjId, GS_TO_VISU_SET_ATTR_ALL, ATTR_VISIBLE | ATTR_SMOOTH );
			SendToVisuObj( ActLine->MenuText_ObjId, GS_TO_VISU_SET_FG_COLOR, cfgm->TextColor[CONFIG_TYPE_INT] );
			SendToVisuObj( ActLine->TrueTypeVar_ObjId, GS_TO_VISU_SET_ATTR_ALL, ATTR_VISIBLE | ATTR_SMOOTH | ATTR_NOMINAL );
			SendToVisuObj( ActLine->TrueTypeVar_ObjId, GS_TO_VISU_SET_FG_COLOR, cfgm->TextColor[CONFIG_TYPE_INT] );
			SetVar( ActLine->TrueTypeVar_Hdl, ActElement->value.ival );
			break;

		case CONFIG_TYPE_INT64:
			SendToVisuObj( ActLine->EditRessourceText_ObjId, GS_TO_VISU_SET_ATTR_ALL, 0 );
			SendToVisuObj( ActLine->MenuText_ObjId, GS_TO_VISU_SET_ATTR_ALL, ATTR_VISIBLE | ATTR_SMOOTH );
			SendToVisuObj( ActLine->MenuText_ObjId, GS_TO_VISU_SET_FG_COLOR, cfgm->TextColor[CONFIG_TYPE_INT64] );
			SendToVisuObj( ActLine->TrueTypeVar_ObjId, GS_TO_VISU_SET_ATTR_ALL, ATTR_VISIBLE | ATTR_SMOOTH | ATTR_NOMINAL );
			SendToVisuObj( ActLine->TrueTypeVar_ObjId, GS_TO_VISU_SET_FG_COLOR, cfgm->TextColor[CONFIG_TYPE_INT64] );
			SetVar( ActLine->TrueTypeVar_Hdl, ActElement->value.ival );
			break;

		case CONFIG_TYPE_FLOAT:
			SendToVisuObj( ActLine->EditRessourceText_ObjId, GS_TO_VISU_SET_ATTR_ALL, 0 );
			SendToVisuObj( ActLine->MenuText_ObjId, GS_TO_VISU_SET_ATTR_ALL, ATTR_VISIBLE | ATTR_SMOOTH );
			SendToVisuObj( ActLine->MenuText_ObjId, GS_TO_VISU_SET_FG_COLOR, cfgm->TextColor[CONFIG_TYPE_FLOAT] );
			SendToVisuObj( ActLine->TrueTypeVar_ObjId, GS_TO_VISU_SET_ATTR_ALL, ATTR_VISIBLE | ATTR_SMOOTH | ATTR_NOMINAL );
			SendToVisuObj( ActLine->TrueTypeVar_ObjId, GS_TO_VISU_SET_FG_COLOR, cfgm->TextColor[CONFIG_TYPE_FLOAT] );
			SetVar( ActLine->TrueTypeVar_Hdl, ActElement->value.ival );
			break;

		case CONFIG_TYPE_STRING:
			SendToVisuObj( ActLine->EditRessourceText_ObjId, GS_TO_VISU_SET_ATTR_ALL, ATTR_VISIBLE | ATTR_SMOOTH | ATTR_NOMINAL );
			SendToVisuObj( ActLine->EditRessourceText_ObjId, GS_TO_VISU_SET_FG_COLOR, cfgm->TextColor[CONFIG_TYPE_STRING] );
			SendToVisuObj( ActLine->MenuText_ObjId, GS_TO_VISU_SET_ATTR_ALL, ATTR_VISIBLE | ATTR_SMOOTH );
			SendToVisuObj( ActLine->MenuText_ObjId, GS_TO_VISU_SET_FG_COLOR, cfgm->TextColor[CONFIG_TYPE_STRING] );
			SendToVisuObj( ActLine->TrueTypeVar_ObjId, GS_TO_VISU_SET_ATTR_ALL, 0 );
			if( NULL != ActElement->value.sval ) {
				SetVisObjData( ActLine->EditRessourceText_ObjId, ActElement->value.sval, strlen( ActElement->value.sval ) + 1 );
			}
			break;

		case CONFIG_TYPE_BOOL:
			SendToVisuObj( ActLine->EditRessourceText_ObjId, GS_TO_VISU_SET_ATTR_ALL, 0 );
			SendToVisuObj( ActLine->MenuText_ObjId, GS_TO_VISU_SET_ATTR_ALL, ATTR_VISIBLE | ATTR_SMOOTH );
			SendToVisuObj( ActLine->MenuText_ObjId, GS_TO_VISU_SET_FG_COLOR, cfgm->TextColor[CONFIG_TYPE_INT] );
			SendToVisuObj( ActLine->TrueTypeVar_ObjId, GS_TO_VISU_SET_ATTR_ALL, ATTR_VISIBLE | ATTR_SMOOTH | ATTR_NOMINAL );
			SendToVisuObj( ActLine->TrueTypeVar_ObjId, GS_TO_VISU_SET_FG_COLOR, cfgm->TextColor[CONFIG_TYPE_BOOL] );
			SetVar( ActLine->TrueTypeVar_Hdl, ActElement->value.ival ? 1 : 0 );
			break;

		case CONFIG_TYPE_ARRAY:
			SendToVisuObj( ActLine->EditRessourceText_ObjId, GS_TO_VISU_SET_ATTR_ALL, 0 );
			SendToVisuObj( ActLine->MenuText_ObjId, GS_TO_VISU_SET_ATTR_ALL, ATTR_VISIBLE | ATTR_SMOOTH | ATTR_NOMINAL );
			SendToVisuObj( ActLine->MenuText_ObjId, GS_TO_VISU_SET_FG_COLOR, cfgm->TextColor[CONFIG_TYPE_ARRAY] );
			SendToVisuObj( ActLine->TrueTypeVar_ObjId, GS_TO_VISU_SET_ATTR_ALL, 0 );
			break;
		case CONFIG_TYPE_LIST:
			SendToVisuObj( ActLine->EditRessourceText_ObjId, GS_TO_VISU_SET_ATTR_ALL, 0 );
			SendToVisuObj( ActLine->MenuText_ObjId, GS_TO_VISU_SET_ATTR_ALL, ATTR_VISIBLE | ATTR_SMOOTH | ATTR_NOMINAL );
			SendToVisuObj( ActLine->MenuText_ObjId, GS_TO_VISU_SET_FG_COLOR, cfgm->TextColor[CONFIG_TYPE_LIST] );
			SendToVisuObj( ActLine->TrueTypeVar_ObjId, GS_TO_VISU_SET_ATTR_ALL, 0 );
			break;
		}

		if( -1 < ActLine->TrueTypeVarFloat_ObjId ) {
			if( CONFIG_TYPE_FLOAT == ActElement->type ) {
				SendToVisuObj( ActLine->TrueTypeVarFloat_ObjId, GS_TO_VISU_SET_ATTR_ALL, ATTR_VISIBLE | ATTR_SMOOTH | ATTR_NOMINAL );
				SendToVisuObj( ActLine->MenuText_ObjId, GS_TO_VISU_SET_FG_COLOR, cfgm->TextColor[CONFIG_TYPE_FLOAT] );
				SetVarFloat( ActLine->TrueTypeVarFloat_Hdl, ActElement->value.fval );
			}
			else {
				SendToVisuObj( ActLine->TrueTypeVarFloat_ObjId, GS_TO_VISU_SET_ATTR_ALL, 0 );
			}
		}
	}
	for( ; i - cfgm->offset < cfgm->Lines_Num; i++ ) {
		tCfgMenuLine* ActLine = &cfgm->Lines[i - cfgm->offset];
		SendToVisuObj( ActLine->EditRessourceText_ObjId, GS_TO_VISU_SET_ATTR_ALL, 0 );
		SendToVisuObj( ActLine->MenuText_ObjId, GS_TO_VISU_SET_ATTR_ALL, 0 );
		SendToVisuObj( ActLine->TrueTypeVar_ObjId, GS_TO_VISU_SET_ATTR_ALL, 0 );
	}
}

void CfgMenu_ScrollDown( tCfgMenu* cfgm ) {
	if( NULL != cfgm->ActSet ) {
		if( NULL != cfgm->ActSet->value.list ) {
			if( cfgm->offset < (cfgm->ActSet->value.list->length - 1) ) {
				cfgm->offset++;
			}
		}
	}
	SendKeyToVisu( GS_KEY_TO_VISU_TYPE_LKF | GS_VISU_LKF_ESCAPE );
}

void CfgMenu_ScrollUp( tCfgMenu* cfgm ) {
	if( NULL != cfgm->ActSet ) {
		if( NULL != cfgm->ActSet->value.list ) {
			if( cfgm->offset > 0 ) {
				cfgm->offset--;
			}
		}
	}
	SendKeyToVisu( GS_KEY_TO_VISU_TYPE_LKF | GS_VISU_LKF_ESCAPE );
}

void CfgMenu_Draw( tCfgMenu* cfgm ) {
	if( NULL == cfgm->ActSet )
		return;
	switch( config_setting_type( cfgm->ActSet ) ) {
	case CONFIG_TYPE_NONE:
		break;
	case CONFIG_TYPE_GROUP:
		CfgMenu_DrawGroup( cfgm );
		break;
	case CONFIG_TYPE_INT:
		break;
	case CONFIG_TYPE_INT64:
		break;
	case CONFIG_TYPE_FLOAT:
		break;
	case CONFIG_TYPE_STRING:
		break;
	case CONFIG_TYPE_BOOL:
		break;
	case CONFIG_TYPE_ARRAY:
		CfgMenu_DrawGroup( cfgm );
		break;
	case CONFIG_TYPE_LIST:
		CfgMenu_DrawGroup( cfgm );
		break;
	}
	char buffer[128];
	CfgMenu_GetPath( cfgm, buffer, sizeof( buffer ) );
	SetVisObjData( cfgm->PathObj, buffer, strlen( buffer ) + 1 );

}

int32_t CfgMenu_Evt( tCfgMenu* cfgm, uint32_t evtc, tUserCEvt* evtv ) {
	for( uint32_t i = 0; i < evtc; i++ ) {
		if( CEVT_EDIT_END == evtv[i].Type ) {
			for( uint32_t j = 0; j < cfgm->Lines_Num; j++ ) {

				if( evtv[i].Content.mEditEnd.ObjID == cfgm->Lines[j].TrueTypeVar_ObjId ) {
//					db_out( "Edit end Obj ID %d\r\n", evtv[i].Content.mEditEnd.ObjID );
					if( NULL == cfgm->ActSet->value.list )
						return -1;
					config_setting_t** elements = cfgm->ActSet->value.list->elements;
					if( NULL == elements )
						return -1;
					if( NULL == *elements )
						return -1;
					uint32_t numElements = cfgm->ActSet->value.list->length;
					if( numElements < j + cfgm->offset )
						return -1;
					config_setting_t* element = elements[j + cfgm->offset];
//					db_out( "Value read: %d\r\n", GetVar( cfgm->Lines[j].TrueTypeVar_Hdl ) );
					element->value.llval = GetVar( cfgm->Lines[j].TrueTypeVar_Hdl );
					element->value.ival = GetVar( cfgm->Lines[j].TrueTypeVar_Hdl );
					return 0;
				}
				else if( evtv[i].Content.mEditEnd.ObjID == cfgm->Lines[j].EditRessourceText_ObjId ) {
					if( NULL == cfgm->ActSet->value.list )
						return -1;
					config_setting_t** elements = cfgm->ActSet->value.list->elements;
					if( NULL == elements )
						return -1;
					if( NULL == *elements )
						return -1;
					uint32_t numElements = cfgm->ActSet->value.list->length;
					if( numElements < j + cfgm->offset )
						return -1;
					config_setting_t* element = elements[j + cfgm->offset];
					char buffer[128];
					if( 0 < GetVisObjData( cfgm->Lines[j].EditRessourceText_ObjId, buffer, 128 ) ) {
						if( NULL != element->value.sval )
							free( element->value.sval );
						element->value.sval = strdup( buffer );
					}
					return 0;
				}
				else if( evtv[i].Content.mEditEnd.ObjID == (uint32_t)cfgm->Lines[j].TrueTypeVarFloat_ObjId ) {
					if( NULL == cfgm->ActSet->value.list )
						return -1;
					config_setting_t** elements = cfgm->ActSet->value.list->elements;
					if( NULL == elements )
						return -1;
					if( NULL == *elements )
						return -1;
					uint32_t numElements = cfgm->ActSet->value.list->length;
					if( numElements < j + cfgm->offset )
						return -1;
					config_setting_t* element = elements[j + cfgm->offset];
					element->value.fval = GetVarFloat( cfgm->Lines[j].TrueTypeVar_Hdl );
					return 0;
				}
			}
		}
		else if( CEVT_MENU_INDEX == evtv[i].Type ) {
			for( uint32_t j = 0; j < cfgm->Lines_Num; j++ ) {
				if( evtv[i].Content.mMenuIndex.ObjID == cfgm->Lines[j].MenuText_ObjId ) {
					config_setting_t** elements = cfgm->ActSet->value.list->elements;
					if( NULL == elements )
						return -1;
					if( NULL == *elements )
						return -1;;
					uint32_t numElements = cfgm->ActSet->value.list->length;
					if( numElements < j + cfgm->offset )
						return -1;
					config_setting_t* element = elements[j + cfgm->offset];

					if( NULL != element->value.list ) {
						cfgm->ActSet = element;
						cfgm->offset = 0;
					}
					CfgMenu_Draw( cfgm );
					return 0;
				}
			}
		}
	}
	return 0;
}

void CfgMenu_Back( tCfgMenu* cfgm ) {
	if( NULL != cfgm->ActSet ) {
		if( NULL != cfgm->ActSet->parent ) {
			cfgm->ActSet = cfgm->ActSet->parent;
			cfgm->offset = 0;
		}
		CfgMenu_Draw( cfgm );
	}
}
