/************************************************************************
*
* File:         CTRL_TOP_ATTACHMENT.h
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
#ifndef CTRL_TOP_ATTACHMENT_CR_H
#define CTRL_TOP_ATTACHMENT_CR_H
#include "control.h"
/*************************************************************************/

/* macro definitions *****************************************************/

/* type definitions ******************************************************/

/* prototypes ************************************************************/

/* global constants ******************************************************/

/* global variables ******************************************************/

/* global function prototypes ********************************************/

typedef struct tCR_VALVE {
	uint8_t			valv_oil;
	uint8_t			valv_l;
	uint8_t			valv_c;
	uint8_t			valv_r;
	uint8_t			recycl;
	uint8_t			rail1;
	uint8_t			rail2;
	uint8_t			rail3;
	uint8_t			dispens_on;
	uint8_t			dispens_off;
} CR_VALVE_t;
void Ctrl_CR_Dump( void );
void Ctrl_top_LRS_CR_Init( tControl* Ctrl );
void Ctrl_top_LRS_CR_Cycle( tControl* Ctrl, uint32_t evtc, tUserCEvt* evtv );
void Ctrl_top_LRS_CR_Timer_10ms( tControl* Ctrl );

/*************************************************************************/
/* Protection against multiple includes ends here ! Stop editing here !  */
/*************************************************************************/
#endif  // #ifndef CTRL_TOP_ATTACHMENT_CR_H
