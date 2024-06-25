/****************************************************************************
*
* File:         MAINT_Lifetime.c
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
#include "gsemaint.h"
#include "gsemaint_lifetime.h"

 /****************************************************************************/

 /* macro definitions ********************************************************/

 /* type definitions *********************************************************/

 /* prototypes ***************************************************************/

 /* global constants *********************************************************/

 /* global variables *********************************************************/
 /* global variables ******************************************************/
tMaintLifetime** _MaintLife;
uint32_t       _numMaintLife;
/* local function prototypes*************************************************/
uint32_t time( uint32_t* seconds );
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

int32_t Lifetime_Warn_cb( void* arg ) {
	tMaintLifetime* Lifetime = (tMaintLifetime*)arg;
	if( (time( NULL ) + Lifetime->SecondsWarnBefore) >= (uint32_t)GetVarIndexed( Lifetime->RemVarIdx_Service ) )
		return 1;
	else return 0;
}

int32_t Lifetime_Service_cb( void* arg ) {
	tMaintLifetime* Lifetime = (tMaintLifetime*)arg;
	if( time( NULL ) >= (uint32_t)GetVarIndexed( Lifetime->RemVarIdx_Service ) )
		return 1;
	else return 0;
}

tMaintLifetime* Maint_Lifetime_Add( tMaint* Maint, int32_t id, int32_t RessourceWarn, int32_t RessourceService, int32_t RemVarIdx_Service, int32_t HoursWarnBefore ) {

	tMaintLifetime* new = malloc( sizeof( tMaintLifetime ) );
	if( NULL == new ) {
		return NULL;
	}
	new->RemVarIdx_Service = RemVarIdx_Service;
	new->SecondsWarnBefore = HoursWarnBefore * 3600;
	tMaintLifetime** NewArray = realloc( _MaintLife, sizeof( tMaintLifetime* ) * (1 + _numMaintLife) );
	if( NewArray == NULL ) {
		free( new );
		return NULL;
	}
	_MaintLife = NewArray;
	_MaintLife[_numMaintLife] = new;
	_numMaintLife++;

	Maint_AddCondition( Maint, id, RessourceWarn, VBL_INFO, Lifetime_Warn_cb, new );
	Maint_AddCondition( Maint, id, RessourceService, VBL_WARNING, Lifetime_Service_cb, new );
	return new;
}

void Maint_Lifetime_SetNextServiceInSeconds( tMaintLifetime* MLifetime, int32_t WarnBefore_s, int32_t NextService_s ) {
	MLifetime->SecondsWarnBefore = WarnBefore_s;
	SetVarIndexed( MLifetime->RemVarIdx_Service, time( NULL ) + NextService_s );
}

int32_t Maint_Lifetime_TimeUntilNextService_d( tMaintLifetime* MLifetime ) {
	int32_t time_left = GetVarIndexed( MLifetime->RemVarIdx_Service ) - time( NULL );
	time_left = time_left / 60 / 60 / 24; //Seconds -> Hours
	return time_left;
}