/************************************************************************
*
* File:         VISU.h
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
#ifndef VISU_H
#define VISU_H

#include	"UserCEvents.h"
/*************************************************************************/

/* macro definitions *****************************************************/

/* type definitions ******************************************************/

/* prototypes ************************************************************/
typedef enum eagVisuHdl {
	SCREEN_START,
	SCREEN_MAIN,
	SCREEN_MENU_ADMIN,
	SCREEN_MENU_MERKATOR,
	SCREEN_IO_DEVICES,
	SCREEN_PINOUT,
  SCREEN_MCM_CONFIG,
	SCREEN_BACKUP,
  SCREEN_ERROR,
  SCREEN_CHANGE_ERROR,
	SCREEN_SENSORS,
	SCREEN_LEARNING,
	SCREEN_PROPERTIES,
	SCREEN_PROFILE,
	SCREEN_PROFILE_EDIT,
	SCREEN_PROFILE_EDIT_RTR,
	SCREEN_PARAMETERS,
	SCREEN_FINE_CALIBRATION,
	SCREEN_STATISTIC,
	SCREEN_MATERIAL,
	SCREEN_SPREADINGSETTINGS,
	SCREEN_CALIBRBRUSH,
	SCREEN_CALIBRTOP,
}eVisuHdl;

typedef struct tagVisuData {
	uint32_t		dummy;
	tSysDate		Date;       //!< Actual date an visu occured the last time.
	tSysTime		Time;       //!< Actual time an visu occured the last time.
}tVisuData;

typedef void (*Visu_Init_cb)      (const tVisuData* VData);
typedef void (*Visu_Evt_cb)       (const tVisuData* VData, uint32_t evtc, tUserCEvt* evt);
typedef void (*Visu_Timer_10ms_cb)(const tVisuData* VData);
typedef void (*Visu_Open_cb)      (const tVisuData* VData);
typedef void (*Visu_Close_cb)     (const tVisuData* VData);

/**
* @brief this Structure defines a Screen. All Screens will be defined in the Array
* "const tVisuScreen Screens[]" in the file "visu.c".
*
*/
typedef struct tagVisuScreenPage {
	eVisuHdl           Hdl;             /// Handle of this Screen. Needed to open a new Visu_OpenScreen. All Handles should be defined in the enum eVisuHdl
	Visu_Init_cb       Init_cb;         /// This init Function will be called at the startup of the application
	Visu_Evt_cb        Evt_cb;          /// This Function will be called every cycle, if this Screen is the actual shown one.
	Visu_Timer_10ms_cb Timer_10ms_cb;   /// This Function will be called every 10 ms, if this Screen is the actual shown one.
	Visu_Open_cb       Open_cb;         /// This Function will be called if the Screen is opened.
	Visu_Close_cb      Close_cb;        /// This Function will be called if the Screen is closed.
}tVisuScreen;

/* global constants ******************************************************/

/* global variables ******************************************************/

/* global function prototypes ********************************************/

/**
* @brief Opens a new Screen. The function Visu_Close_cb of the current Screen,
* definded in the array "const tVisuScreen Screens[]" in the file "visu.c" will be
* called. After that the function Open_cb of the new Screen, also defined in
* "const tVisuScreen Screens[]" will be called. The Actual Screen will be changed
* to the new.
*
* @param Hdl Element of the enum eVisuHdl.
*/
void Visu_OpenScreen( eVisuHdl Hdl );

void Visu_Init( tVisuData* VisuData );

void Visu_Cycle( uint32_t evtc, tUserCEvt* evtv );

void Visu_Timer( void );

void Visu_HomeKeyPressed( void );

/*************************************************************************/
/* Protection against multiple includes ends here ! Stop editing here !  */
/*************************************************************************/
#endif  // #ifndef VISU_H
