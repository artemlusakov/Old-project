/****************************************************************************
*
* File:         MAINT.c
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
#include "gsemaint.h"
/****************************************************************************/

/* macro definitions ********************************************************/

/* type definitions *********************************************************/

/* prototypes ***************************************************************/

/* global constants *********************************************************/

/* global variables *********************************************************/

/* local function prototypes*************************************************/
tMaint** MaintList = NULL;
uint32_t MaintListNum = 0;
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
int32_t Maint_Init( tMaint* Maint ) {
	memset( Maint, 0, sizeof( tMaint ) );
	tMaint** NewList = realloc( MaintList, sizeof( tMaint* ) * (1 + MaintListNum) );
	if( !NewList )
		return -1;
	MaintList = NewList;
	MaintList[MaintListNum] = Maint;
	MaintListNum++;
	return 0;
}

int32_t Maint_AddCondition( tMaint* Maint, uint32_t id, uint32_t RessoureText, eErrLevel Lev, Maint_Condition_cb cb, void* arg ) {
	tMaintCondition* NewCon = realloc( Maint->cond, sizeof( tMaintCondition ) * (1 + Maint->num_cond) );
	if( NULL == NewCon ) {
//		db_out( "Error Adding Maintanance Condition ID = %d, RessourceTextId: %d\r\n", id, RessoureText );
		return -1;
	}
	Maint->cond = NewCon;
	tMaintCondition* ActCon = &(Maint->cond[Maint->num_cond]);
	Maint->num_cond++;
	ActCon->id = id;
	ActCon->stt = ES_NOERROR;
	ActCon->res_txt = RessoureText;
	ActCon->cb = cb;
	ActCon->lvl = Lev;
	ActCon->arg = arg;
	//In Liste aller Maintenance-Objekte einfügen.

	return 0;
}

void Maint_DeInit( tMaint* Maint ) {
	free( Maint->cond );
	Maint->num_cond = 0;
}

void Maint_Check( tMaint* Maint ) {
	if( !Maint ) return;
	for( uint32_t i = 0; i < Maint->num_cond; i++ ) {
		tMaintCondition* ActCon;
		ActCon = &Maint->cond[i];
		if( ActCon )//Fehler setzen
		{
			int32_t ret;
			if( (ret = ActCon->cb( ActCon->arg )) ) {
				if( (ActCon->stt != ES_UNCHECKED_ERROR_ACTIVE)
					&& (ActCon->stt != ES_CHECKED_ERROR_ACTIVE) ) {
					ActCon->stt = ES_CHECKED_ERROR_ACTIVE;
					EList_SetErrLevByRCText( ActCon->lvl, ActCon->id, ActCon->res_txt, ret );
				}
			}
			else //Fehler zurück setzen
			{
				if( ActCon->stt == ES_UNCHECKED_ERROR_ACTIVE ) {
					ActCon->stt = ES_UNCHECKED_ERROR_INACTIVE;
					EList_ResetErr( ActCon->id );
				}
				else if( ActCon->stt == ES_CHECKED_ERROR_ACTIVE ) {
					ActCon->stt = ES_CHECKED_ERROR_INACTIVE;
					EList_ResetErr( ActCon->id );
				}
			}
		}
	}
}

void Maint_CheckAll( void ) {
	for( uint32_t i = 0; i < MaintListNum; i++ ) {
		Maint_Check( MaintList[i] );
	}
}
