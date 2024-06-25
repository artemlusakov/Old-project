/************************************************************************
*
* File:         BNSO.h
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
#ifndef BNSO_H
#define BNSO_H

/*************************************************************************/

/* macro definitions *****************************************************/
#define J1939_ERR_MSG_ID 0x18FF0381
/* type definitions ******************************************************/

/* prototypes ************************************************************/

/* global constants ******************************************************/

/* global variables ******************************************************/

/* global function prototypes ********************************************/

void BNSO_SetData( void );
void BNSO_SendErrMsg( uint16_t ErrMsgId, uint8_t state );

/*************************************************************************/
/* Protection against multiple includes ends here ! Stop editing here !  */
/*************************************************************************/
#endif // #ifndef BNSO_H
