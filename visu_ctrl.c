/****************************************************************************
*
* File:         VISU_CTRL.c
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
#include "gsLkfCodes.h"
/****************************************************************************/

/* macro definitions ********************************************************/

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

void Visu_Ctrl_Cycle( uint32_t evtc, tUserCEvt* evtv ) {
	//Controle Keys to Visu
	if( IsKeyPressedNew( 7 ) ) {
		SendKeyToVisu( GS_KEY_TO_VISU_TYPE_LKF | GS_VISU_LKF_MOVELEFT );
	}
	if( (IsKeyPressedNew( 8 ))
	|| (IsKeyPressedNew( 103 )) ) {
		SendKeyToVisu( GS_KEY_TO_VISU_TYPE_LKF | GS_VISU_LKF_MOVEUP );
		//SendKeyToVisu(GS_KEY_TO_VISU_TYPE_LKF | GS_VISU_LKF_INCVAL);
	}
	if( IsKeyPressedNew( 9 ) ) {
		SendKeyToVisu( GS_KEY_TO_VISU_TYPE_LKF | GS_VISU_LKF_MOVERIGHT );
	}
	if( (IsKeyPressedNew( 11 ))
	|| (IsKeyPressedNew( 104 )) ) {
		SendKeyToVisu( GS_KEY_TO_VISU_TYPE_LKF | GS_VISU_LKF_MOVEDOWN );
		//SendKeyToVisu(GS_KEY_TO_VISU_TYPE_LKF | GS_VISU_LKF_DECVAL);
	}
	if( IsKeyPressedNew( 12 ) ) {
		SendKeyToVisu( GS_KEY_TO_VISU_TYPE_LKF | GS_VISU_LKF_ENTER );
	}
	if( IsKeyPressedNew( 105 ) ) {
		SendKeyToVisu( GS_KEY_TO_VISU_TYPE_LKF | GS_VISU_LKF_ENTER );
	}

}
