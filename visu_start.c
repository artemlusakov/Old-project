/****************************************************************************
*
* File:         VISU_START.c
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
#include "visu_start.h"
#include "profile.h"
/****************************************************************************/

/* macro definitions ********************************************************/
#define VISU_START_SHOWN_MS 1000
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
static uint32_t		_tm_start;

void Visu_Start_Open( const tVisuData* VData ) {
	_tm_start = GetMSTick();
	PrioMaskOn( MSK_START );
	tProfile*		cur_prof_ = Profile_GetCurrentProfile();
	if( NULL == cur_prof_ ) {
//		db_out( "No profile found!\r\n" );
		return;
	}
	SetVisObjData( OBJ_PROFILE, cur_prof_->name, strlen( cur_prof_->name ) + 1 );
	uint32_t		val_;
	if( 0 == ConfigGetInt( "System.Version.User", &val_ ) )    //Version of User Project
		SetVar( HDL_APP, val_ );
	if( 0 == ConfigGetInt( "System.Version.Boot", &val_ ) )    //Version of App of OS
		SetVar( HDL_OS, val_ );
}

void VisuStart_Cycle( const tVisuData* VData, uint32_t evtc, tUserCEvt* evt ) {
	if( VISU_START_SHOWN_MS < (GetMSTick() - _tm_start) ) {
		Visu_OpenScreen( SCREEN_MAIN );
	}
}
