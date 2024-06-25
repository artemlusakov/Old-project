/************************************************************************
*
* File:         MAINT_RUNTIME.h
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
#ifndef MAINT_RUNTIME_H
#define MAINT_RUNTIME_H
#include "gsemaint.h"
/*************************************************************************/

/* macro definitions *****************************************************/

/* type definitions ******************************************************/

/* prototypes ************************************************************/

/* global constants ******************************************************/

/* global variables ******************************************************/

/* global function prototypes ********************************************/
typedef struct tagMaintRunTime {
	uint32_t RemVarIdx_Act;
	uint32_t RemVarIdx_Service;
	uint32_t HoursWarnBefore;
	uint32_t running;
	uint32_t divsion;
	double factor;
}tMaintRuntime;

tMaintRuntime* Maint_Runtime_Add( tMaint* Maint, int32_t id, int32_t RessourceWarn, int32_t RessourceService, int32_t RemVarIdx_Act, int32_t RemVarIdx_Service, int32_t HoursWarnBefore );
void Maint_Runtime_Start( tMaintRuntime* pRunTime );
void Maint_Runtime_Stop( tMaintRuntime* pRunTime );

void Maint_Runtime_SetNextServiceInMinutes( tMaintRuntime* pRunTime, uint32_t NextServiceInMinutes );
void Maint_Runtime_SetNextServiceInHours( tMaintRuntime* pRunTime, uint32_t NextServiceInHours );
int32_t Maint_Runtime_TimeUntilNextService_s( tMaintRuntime* pRunTime );
int32_t Maint_Runtime_TimeUntilNextService_m( tMaintRuntime* pRunTime );
int32_t Maint_Runtime_TimeUntilNextService_h( tMaintRuntime* pRunTime );
void    Maint_Runtime_Cycle( void );
uint32_t Maint_GetRuntime( tMaintRuntime* pRuntime );
int32_t  Maint_Runtime_IsRunning( tMaintRuntime* pRunTime );
void     Maint_Runtime_SetDiv( tMaintRuntime* pRunTime, uint32_t div );
/*************************************************************************/
/* Protection against multiple includes ends here ! Stop editing here !  */
/*************************************************************************/
#endif  // #ifndef MAINT_RUNTIME_H
