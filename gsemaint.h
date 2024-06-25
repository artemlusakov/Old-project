/************************************************************************
*
* File:         MAINT.h
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
#ifndef MAINT_H
#define MAINT_H

#include "gseerrorlist.h"
/*************************************************************************/

/* macro definitions *****************************************************/

/* type definitions ******************************************************/

/* prototypes ************************************************************/

/* global constants ******************************************************/

/* global variables ******************************************************/

/* global function prototypes ********************************************/
typedef int32_t( *Maint_Condition_cb )(void* arg);

typedef struct tagMaintConditon {
	uint32_t        	id;
	uint32_t			res_txt;
	eErrLevel			lvl;
	eErrorState			stt;
	void*				arg;
	Maint_Condition_cb	cb;
}tMaintCondition;

typedef struct tagMaint {
	tMaintCondition*	cond;
	uint32_t			num_cond;

}tMaint;

/**
* @brief Saves the pointer of the structer Maint in a List. This way all Maintanance can be checked by Maint_CheckAll during the Runtime.
*
* @param Maint
* @return int32_t -1 if an error occured, else 0.
*/
int32_t Maint_Init( tMaint* Maint );

/**
* @brief Adds a new condition to a Maintanance.
*
* @param Maint Pointer to a structure tMaint.
* @param id Error ID of an error which will be generated, if the Maintanance condition is met
* @param RessoureText Text which will be written into the Error
* @param Lev Error level
* @param cb Callback funtino to check  the condition.
* @param arg Argument for the callback function.
* @return int32_t
*/
int32_t Maint_AddCondition( tMaint* Maint, uint32_t id, uint32_t RessoureText, eErrLevel Lev, Maint_Condition_cb cb, void* arg );

/**
* @brief Checks all conditions of a Maintanance and sets / resets errrors.
*
*
* @param Maint
*/
void Maint_Check( tMaint* Maint );

/**
* @brief Checks all conditions of all Maintanance and sets / resets errrors.
*
*/
void Maint_CheckAll( void );

/*************************************************************************/
/* Protection against multiple includes ends here ! Stop editing here !  */
/*************************************************************************/
#endif  // #ifndef MAINT_H
