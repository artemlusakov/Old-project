/****************************************************************************
*
* File:         E_CONFIG.c
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

#include "gs_easy_config.h"

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
void (*mf_db_out)(const char*, ...) = NULL;

void gsEasyConfig_set_debug_out( void (*_db_out)(const char*, ...) ) {
	mf_db_out = _db_out;
}

void config_db_out( config_t* pCfg, const char* func ) {
	if( NULL != mf_db_out ) {
		char buffer[128] = {0};
		if( CONFIG_ERR_FILE_IO == pCfg->error_type ) {
			snprintf( buffer, sizeof( buffer ),
				"E-Config Error Function %s,  File: %s Line: %d, %s\r\n", func, pCfg->error_file, pCfg->error_line, pCfg->error_text );
		}
		else if( CONFIG_ERR_PARSE == pCfg->error_type ) {
			snprintf( buffer, sizeof( buffer ),
				"E-Config Error Function %s,  Parseing Line: %d, %s\r\n", func, pCfg->error_line, pCfg->error_text );
		}
		else {
			snprintf( buffer, sizeof( buffer ), "%s\r\n", func );
		}

		mf_db_out( buffer );
	}
}

int32_t gsEasyConfig_Add_Path( const char* path, const tEasyConfigEntry* pEntry, config_setting_t* setting ) {
	char* needle;
	needle = strchr( path, '.' );
	int32_t index = 0;
	config_setting_t* entry = NULL;
	if( path[0] == '[' ) {
		index = strtol( path + 1, NULL, 0 );
	}
	if( NULL == needle ) {
		for( int32_t i = 0; i <= index; i++ )
			entry = config_setting_add( setting, path, pEntry->CType );
		switch( pEntry->CType ) {
		case CONFIG_TYPE_INT: {
			int32_t value = strtol( pEntry->Default, NULL, 0 );
			config_setting_set_int( entry, value );
		}
							break;

		case CONFIG_TYPE_BOOL: {
			int32_t value = strtol( pEntry->Default, NULL, 0 );
			config_setting_set_bool( entry, value );
		}
							 break;
		case CONFIG_TYPE_INT64: {
			int64_t value = strtol( pEntry->Default, NULL, 0 );
			config_setting_set_int64( entry, value );
		}
							  break;

		case CONFIG_TYPE_FLOAT: {
			double value = strtod( pEntry->Default, NULL );
			config_setting_set_float( entry, value );
		}
							  break;

		case CONFIG_TYPE_STRING:
			config_setting_set_string( entry, pEntry->Default );
			break;

		default: break;
		}
		return 0;
	}
	else if( '[' == *(needle + 1) ) {
		//int32_t num  = atoi(needle + 2);
		char string[strlen( path ) + 1];
		strncpy( string, path, sizeof( string ) );
		char* ptr = strtok( string, "." );
		entry = config_setting_get_member( setting, ptr );
		if( NULL == entry ) {
			entry = config_setting_add( setting, ptr, CONFIG_TYPE_LIST );
		}
		gsEasyConfig_Add_Path( string + strlen( string ) + 1, pEntry, entry );

	}
	else //Gruppe
	{
		char string[strlen( path ) + 1];
		strcpy( string, path );
		char* ptr = strtok( string, "." ); //todo
		if( CONFIG_TYPE_LIST == setting->type ) {
			int32_t numEntries = config_setting_length( setting );
			while( numEntries < (index + 1) ) {
				config_setting_add( setting, ptr, CONFIG_TYPE_GROUP );
				numEntries = config_setting_length( setting );
			}
			entry = config_setting_get_elem( setting, index );
			gsEasyConfig_Add_Path( string + strlen( string ) + 1, pEntry, entry );
			return 0;
		}
		else {
			entry = config_setting_get_member( setting, ptr );
			if( NULL == entry ) {
				entry = config_setting_add( setting, ptr, CONFIG_TYPE_GROUP );
			}
			gsEasyConfig_Add_Path( string + strlen( string ) + 1, pEntry, entry );
		}
	}
	return 0;
}

int32_t gsEasyConfig_Read( const tEasyConfigEntry* pEntries, int32_t numEntries, const char* path ) {
	config_t cfg;
	tGsFile* f;
	config_init( &cfg );
	config_db_out( &cfg, "gsEasyConfig" );
	int32_t ret = 0;
	if( NULL != (f = FileOpen( path, "r" )) ) {
		if( CONFIG_FALSE == config_read( &cfg, f ) ) {
			config_db_out( &cfg, "Error reading Config\r\n" );;
			ret = -1;
		}
		FileClose( f );
		config_db_out( &cfg, "gsEasyConfigm Opened File\r\n" );
	}
	else {
		config_db_out( &cfg, "Error opening Config\r\n" );
		ret = -1;

	}
	config_setting_t* root = config_root_setting( &cfg );

	for( int32_t i = 0; i < numEntries; i++ ) {
		const tEasyConfigEntry* pActEntry = &pEntries[i];
		const char* string;
		double fValue;
		config_setting_t* setting;
		if( NULL == (setting = config_lookup( &cfg, pActEntry->Tag )) ) {
			//setting = config_setting_add(root, pActEntry->Tag, pActEntry->CType);
			gsEasyConfig_Add_Path( pActEntry->Tag, pActEntry, root );
			config_lookup( &cfg, pActEntry->Tag );
			ret = 1;
		}
		switch( pActEntry->CType ) {
		case CONFIG_TYPE_BOOL:
			//PrintToDebug("Bool: %s\r\n", pActEntry->Tag);
			if( CONFIG_TRUE != config_lookup_bool( &cfg, pActEntry->Tag, pActEntry->Entry ) ) {
				*(int32_t*)pActEntry->Entry = strtol( pActEntry->Default, NULL, 0 );
			}
			break;
		case CONFIG_TYPE_INT:
			//PrintToDebug("Int: %s\r\n", pActEntry->Tag);
			if( CONFIG_TRUE != config_lookup_int( &cfg, pActEntry->Tag, pActEntry->Entry ) ) {
				*(int32_t*)pActEntry->Entry = strtol( pActEntry->Default, NULL, 0 );
			}
			break;

		case CONFIG_TYPE_INT64:
			//PrintToDebug("Int64: %s\r\n", pActEntry->Tag);
			if( CONFIG_TRUE != config_lookup_int64( &cfg, pActEntry->Tag, pActEntry->Entry ) ) {
				*(int64_t*)pActEntry->Entry = strtol( pActEntry->Default, NULL, 0 );
			}
			break;

		case CONFIG_TYPE_FLOAT:
			//PrintToDebug("Float: %s\r\n", pActEntry->Tag);
			if( CONFIG_TRUE != config_lookup_float( &cfg, pActEntry->Tag, &fValue ) ) {
				fValue = strtod( pActEntry->Default, NULL );
			}
			*(float*)pActEntry->Entry = (float)fValue;
			break;

		case CONFIG_TYPE_STRING:
			//PrintToDebug("String: %s\r\n", pActEntry->Tag);
			if( CONFIG_TRUE == config_lookup_string( &cfg, pActEntry->Tag, &string ) ) {
				strncpy( pActEntry->Entry, string, sizeof( pActEntry->Default ) - 1 );
			}
			else {
				strncpy( pActEntry->Entry, pActEntry->Default, sizeof( pActEntry->Default ) - 1 );
			}
			break;

		default: break;
		}
	}
	config_destroy( &cfg );
	config_db_out( &cfg, "Config destroyed\r\n" );

	return ret;
}

int32_t gsEasyConfig_Write( const tEasyConfigEntry* pEntries, int32_t numEntries, const char* path ) {
	config_t cfg;
	tGsFile* f;
	config_init( &cfg );
	if( NULL != (f = FileOpen( path, "r" )) ) {
		if( CONFIG_FALSE == config_read( &cfg, f ) ) {
			config_db_out( &cfg, "error config_read" );
			return -1;
		}
		FileClose( f );
	}
	config_setting_t* root = config_root_setting( &cfg );
	for( int32_t i = 0; i < numEntries; i++ ) {
		config_setting_t* set;
		const tEasyConfigEntry* pActEntry = &pEntries[i];
		set = config_lookup( &cfg, pActEntry->Tag );
		if( NULL == set ) {
			//config_setting_add(config_root_setting(&cfg), pActEntry->Tag, pActEntry->CType);
			gsEasyConfig_Add_Path( pActEntry->Tag, pActEntry, root );
			set = config_lookup( &cfg, pActEntry->Tag );

		}
		if( (set != NULL) && (pActEntry->CType == (uint32_t)config_setting_type( set )) ) {
//			db_out( "Add Parameter: \"%s\"\r\n", pActEntry->Default );
			switch( pActEntry->CType ) {
			case CONFIG_TYPE_BOOL:
				config_setting_set_bool( set, *(int32_t*)pActEntry->Entry );
				break;

			case CONFIG_TYPE_INT:
				config_setting_set_int( set, *(int32_t*)pActEntry->Entry );
				break;

			case CONFIG_TYPE_INT64:
				config_setting_set_int64( set, *(int64_t*)pActEntry->Entry );
				break;

			case CONFIG_TYPE_FLOAT:
				config_setting_set_float( set, *(float*)pActEntry->Entry );
				break;

			case CONFIG_TYPE_STRING:
				config_setting_set_string( set, (char*)pActEntry->Entry );
				break;
			default: break;
			}
		}
		else {

		}
	}

	if( NULL != (f = FileOpen( path, "w" )) ) {
		config_write( &cfg, f );
		FileClose( f );
	}
	config_destroy( &cfg );

	return 0;
}

