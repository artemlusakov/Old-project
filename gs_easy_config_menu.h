/************************************************************************
*
* File:         GS_EASY_CONFIG_MENU.h
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
************************************************************************/

 /************************************************************************/
 /* Protection against multiple includes.                                */
 /* Do not code anything outside the following defines (except comments) */
#ifndef GS_EASY_CONFIG_MENU_H
#define GS_EASY_CONFIG_MENU_H

#include	<stdint.h>
#include	"config/libconfig.h"
/*************************************************************************/

/* macro definitions *****************************************************/

/* type definitions ******************************************************/
#define CFGM_OK                        0
#define CFGM_ERROR_ALLOCATING_MEMMORY  1
#define CFGM_ERROR_LINE_DOES_NOT_EXIST 2
#define CFGM_ERROR_INVALID_CONFIG_TPPE 3
#define CFGM_ERROR_READING_CONFIG_FILE 4
#define CFGM_ERROR_WRITING_CONFIG_FILE 5

/* prototypes ************************************************************/

/* global constants ******************************************************/

/* global variables ******************************************************/

/* global function prototypes ********************************************/

typedef struct tagCfgMenuLine {
	uint32_t MenuText_ObjId;
	uint32_t EditRessourceText_ObjId;
	uint32_t TrueTypeVar_ObjId;
	uint32_t TrueTypeVar_Hdl;
	int32_t TrueTypeVarFloat_ObjId;
	uint32_t TrueTypeVarFloat_Hdl;
}tCfgMenuLine;

typedef struct tagCfgMenu {
	uint32_t         PathObj;
	tCfgMenuLine* Lines;
	uint32_t         Lines_Num;
	uint32_t         offset;
	config_t         cfg;
	config_setting_t* ActSet;
	uint32_t         TextColor[9];
}
tCfgMenu;

void       CfgMenu_Init( tCfgMenu* cfgm );

uint32_t   CfgMenu_AddLine( tCfgMenu* cfgm, uint32_t MenuTextObjId, uint32_t EditRessoureTextObjId, uint32_t TrueTypeVarObjId, uint32_t TrueTypeVarHdl );

uint32_t   CfgMenu_AddFloatToLine( tCfgMenu* cfgm, uint32_t line, int32_t TrueTypeVarFloatObj, int32_t TrueTypeVarFloatHdl );

uint32_t   CfgMenu_SetTextColor( tCfgMenu* cfgm, uint8_t ConfigType, uint32_t color );

uint32_t   CfgMenu_SetPathObj( tCfgMenu* cfgm, uint32_t TextObjId );

uint32_t   CfgMenu_ReadFile( tCfgMenu* cfgm, const char* path );

uint32_t   CfgMenu_WriteFile( tCfgMenu* cfgm, const char* path );

int32_t    CfgMenu_GetPath( tCfgMenu* cfgm, char* buf, size_t bufsize );

void       CfgMenu_ScrollDown( tCfgMenu* cfgm );

void       CfgMenu_ScrollUp( tCfgMenu* cfgm );

void       CfgMenu_Back( tCfgMenu* cfgm );

void       CfgMenu_Draw( tCfgMenu* cfgm );

int32_t    CfgMenu_Evt( tCfgMenu* cfgm, uint32_t evtc, tUserCEvt* evtv );

/*************************************************************************/
/* Protection against multiple includes ends here ! Stop editing here !  */
/*************************************************************************/
#endif  // #ifndef GS_EASY_CONFIG_MENU_H
