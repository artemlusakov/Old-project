/****************************************************************************
*
* File:         	gseMaint_Runtime.c
* Project:			EasyLibs
* Author(s):		Marc Schartel
* Date:			22.01.2019
*
* Description:
*
*
*
*
*
****************************************************************************/
#include <stdint.h>
#include <UserCAPI.h>     /* API-Function declarations                      */
#include "gsemaint_runtime.h"

 /****************************************************************************/

 /* macro definitions ********************************************************/

 /* type definitions *********************************************************/

 /* prototypes ***************************************************************/

 /* global constants *********************************************************/

 /* global variables *********************************************************/
tMaintRuntime** _MaintRun;
uint32_t       _numMaintRun;
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

int32_t Runtime_Warn_cb( void* arg ) {
	tMaintRuntime* Runtime = (tMaintRuntime*)arg;
	if( ((uint32_t)GetVarIndexed( Runtime->RemVarIdx_Act ) + Runtime->HoursWarnBefore) >= (uint32_t)GetVarIndexed( Runtime->RemVarIdx_Service ) )
		return 1;
	else return 0;
}

int32_t Runtime_Service_cb( void* arg ) {
	tMaintRuntime* Runtime = (tMaintRuntime*)arg;
	if( GetVarIndexed( Runtime->RemVarIdx_Act ) >= GetVarIndexed( Runtime->RemVarIdx_Service ) )
		return 1;
	else return 0;
}

tMaintRuntime* Maint_Runtime_Add( tMaint* Maint, int32_t id, int32_t RessourceWarn, int32_t RessourceService, int32_t RemVarIdx_Act, int32_t RemVarIdx_Service, int32_t HoursWarnBefore ) {

	tMaintRuntime* new = malloc( sizeof( tMaintRuntime ) );
	if( NULL == new ) {
		return NULL;
	}
	new->RemVarIdx_Act = RemVarIdx_Act;
	new->RemVarIdx_Service = RemVarIdx_Service;
	new->running = 0;
	new->HoursWarnBefore = HoursWarnBefore;
	new->divsion = 0;
	tMaintRuntime** NewArray = realloc( _MaintRun, sizeof( tMaintRuntime* ) * (1 + _numMaintRun) );
	if( NewArray == NULL ) {
		free( new );
		return NULL;
	}
	_MaintRun = NewArray;
	_MaintRun[_numMaintRun] = new;
	_numMaintRun++;

	Maint_AddCondition( Maint, id, RessourceWarn, VBL_INFO, Runtime_Warn_cb, new );
	Maint_AddCondition( Maint, id, RessourceService, VBL_WARNING, Runtime_Service_cb, new );
	return new;
}

void Maint_Runtime_Start( tMaintRuntime* pRunTime ) {
	if( !pRunTime )
		return;
	pRunTime->running = 1;
}

void Maint_Runtime_Stop( tMaintRuntime* pRunTime ) {
	if( !pRunTime )
		return;
	pRunTime->running = 0;
}

int32_t Maint_Runtime_IsRunning( tMaintRuntime* pRunTime ) {
	return pRunTime->running;
}

void Maint_Runtime_SetNextServiceInHours( tMaintRuntime* pRunTime, uint32_t NextServiceInHours ) {
	if( !pRunTime )
		return;
	SetVarIndexed( pRunTime->RemVarIdx_Service, (double)GetVarIndexed( pRunTime->RemVarIdx_Act ) + NextServiceInHours * 60 * 60 );
}

void Maint_Runtime_SetNextServiceInMinutes( tMaintRuntime* pRunTime, uint32_t NextServiceInMinutes ) {
	if( !pRunTime )
		return;
	SetVarIndexed( pRunTime->RemVarIdx_Service, (double)GetVarIndexed( pRunTime->RemVarIdx_Act ) + NextServiceInMinutes * 60 );
}

int32_t Maint_Runtime_TimeUntilNextService_s( tMaintRuntime* pRunTime ) {
	return GetVarIndexed( pRunTime->RemVarIdx_Service ) - (double)GetVarIndexed( pRunTime->RemVarIdx_Act );
}

int32_t Maint_Runtime_TimeUntilNextService_m( tMaintRuntime* pRunTime ) {
	return (GetVarIndexed( pRunTime->RemVarIdx_Service ) - (double)GetVarIndexed( pRunTime->RemVarIdx_Act )) / 60;
}

int32_t Maint_Runtime_TimeUntilNextService_h( tMaintRuntime* pRunTime ) {
	return (GetVarIndexed( pRunTime->RemVarIdx_Service ) - (double)GetVarIndexed( pRunTime->RemVarIdx_Act )) / 3600;
}

void Maint_Runtime_Cycle( void ) {
	static uint32_t TimeOld = 0;
	if( TimeOld != HourCounterGet( 0 ) ) {
		TimeOld = HourCounterGet( 0 );
		for( uint32_t i = 0; i < _numMaintRun; i++ ) {
			if( _MaintRun[i]->running ) {
				if( _MaintRun[i]->divsion ) {
					if( 0 == (TimeOld % _MaintRun[i]->divsion) ) {
						SetVarIndexed( _MaintRun[i]->RemVarIdx_Act, GetVarIndexed( _MaintRun[i]->RemVarIdx_Act ) + 1 );
					}
				}
				else {
					SetVarIndexed( _MaintRun[i]->RemVarIdx_Act, GetVarIndexed( _MaintRun[i]->RemVarIdx_Act ) + 1 );
				}
			}
		}
	}
}

void Maint_Runtime_SetDiv( tMaintRuntime* pRunTime, uint32_t div ) {
	pRunTime->divsion = div;
}

uint32_t Maint_GetRuntime( tMaintRuntime* pRuntime ) {
	return (double)GetVarIndexed( pRuntime->RemVarIdx_Act );
}
