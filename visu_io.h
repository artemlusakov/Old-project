/************************************************************************
*
* File:         VISU_IO.h
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
#ifndef VISU_IO_H
#define VISU_IO_H

/*************************************************************************/

/* macro definitions *****************************************************/

/* type definitions ******************************************************/

/* prototypes ************************************************************/

/* global constants ******************************************************/

/* global variables ******************************************************/

/* global function prototypes ********************************************/

void Visu_IO_Init( const tVisuData* Data );
void Visu_IO_Open_cb( const tVisuData* Data );
void Visu_IO_Cycle_cb( const tVisuData* Data, uint32_t evtc, tUserCEvt* evtv );
void Visu_IO_Timer_cb( const tVisuData* Data );
void Visu_IO_Close_cb( const tVisuData* Data );

/*************************************************************************/
/* Protection against multiple includes ends here ! Stop editing here !  */
/*************************************************************************/
#endif  // #ifndef VISU_IO_H
