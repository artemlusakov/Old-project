/************************************************************************
*
* File:        GS_EASY_CONFIG.h
* Project:
* Author(s):   Marc Schartel
* Date:        10.01.2018
*
* Description: This libary provides a simple possiblity to store parameters into
*              a file.
*
*
*
*
************************************************************************/

 /************************************************************************/
 /* Protection against multiple includes.                                */
 /* Do not code anything outside the following defines (except comments) */
#ifndef GS_EASY_CONFIG_H
#define GS_EASY_CONFIG_H
#include <config/libconfig.h>

/*************************************************************************/

/* macro definitions *****************************************************/

/* type definitions ******************************************************/
typedef enum eagEasyConfigType {
	EASY_CONFIG_TYPE_INT = 2,
	EASY_CONFIG_TYPE_INT64 = 3,
	EASY_CONFIG_TYPE_FLOAT = 4,
	EASY_CONFIG_TYPE_STRING = 5,
	EASY_CONFIG_TYPE_BOOL = 6,
}eEasyConfigType;

/**
* Structure of Array used by gsEasyConfig_Read & gsEasyConfig_Write
* @param CType   Type of the Data "Entry" is pointing at.
* @param Entry   Pointer to the data, which has to be saved in the config file
* @param Default Default value for Entry, if the value hasn't been found in the
*                config file. Saved as String
* @param Tag     Tag to save the Data in the file
*
* Possible Tags:
*                Variable:             Name
*                Structure:            Structure.Entry_1
*                                      Structure.Entry_2
*                                      ...
*                Arrays of Structures: Structure.[0].Entry_1
*                                      Structure.[0].Entry_2
*                                      Structure.[2].Entry_1
*                                      ...
**/
typedef struct tagEasyConfigEntry {
	eEasyConfigType CType;
	void* Entry;
	char            Default[48];
	char            Tag[48];
}tEasyConfigEntry;
/* prototypes ************************************************************/

typedef void (*db_func)(const char* fmt, ...);

/* global constants ******************************************************/

/* global variables ******************************************************/

/* global function prototypes ********************************************/

/**
* Roots the error messages to an formatet String function.
*
*
* @param db  output function like PrintToDebug or printf
*/

void     gsEasyConfig_set_debug_out( db_func db );

/**
* Reads a configuration from the file at path and writes the data into the
* variables defined in the List pEntries.  If no File or Entry has been found,
* the default values from the List pEntries will be written into the variables.
*
* @param pEntries   List for Entries which should be read out of the File
* @param numEntries number of Entries of the list pEntries
* @param path       filename and path of the configuration file.
* @return           0 if data has been read successfully
*/
int32_t  gsEasyConfig_Read( const tEasyConfigEntry* pEntries, int32_t numEntries, const char* path );

/**
* Writes data into a configuration file. The values written into the file are
* defined by the list pEntries.
*
* @param pEntries   List for Entries which should be written into the File
* @param numEntries number of Entries of the list pEntries
* @param path       filename and path of the configuration file.
* @return            0 if data has been written successfully
*/
int32_t  gsEasyConfig_Write( const tEasyConfigEntry* pEntries, int32_t numEntries, const char* path );
int32_t gsEasyConfig_Add_Path( const char* path, const tEasyConfigEntry* pEntry, config_setting_t* setting );

/*************************************************************************/
/* Protection against multiple includes ends here ! Stop editing here !  */
/*************************************************************************/
#endif  // #ifndef GS_EASY_CONFIG_H
