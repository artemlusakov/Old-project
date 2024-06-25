/************************************************************************
*
* File:         MAINT_LIFETIME.h
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
#ifndef MAINT_LIFETIME_H
#define MAINT_LIFETIME_H
#include "stdint.h"

#include "gsemaint.h"
/*************************************************************************/

/* macro definitions *****************************************************/

/* type definitions ******************************************************/

/* prototypes ************************************************************/

/* global constants ******************************************************/

/* global function prototypes ********************************************/
typedef struct tagMaintLifeTime {
	uint32_t RemVarIdx_Service;
	uint32_t SecondsWarnBefore;
}tMaintLifetime;

tMaintLifetime* Maint_Lifetime_Add( tMaint* Maint, int32_t id, int32_t RessourceWarn, int32_t RessourceService, int32_t RemVarIdx_Service, int32_t HoursWarnBefore );

void  Maint_Lifetime_SetNextServiceInSeconds( tMaintLifetime* MLifetime, int32_t WarnBefore_s, int32_t NextService_s );

int32_t Maint_Lifetime_TimeUntilNextService_d( tMaintLifetime* MLifetime );
/*************************************************************************/
/* Protection against multiple includes ends here ! Stop editing here !  */
/*************************************************************************/
#endif  // #ifndef MAINT_LIFETIME_H
