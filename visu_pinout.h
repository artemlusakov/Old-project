/****************************************************************************
 *
 * Project:   name
 *
 * @file      filename.h
 * @author    author
 * @date      [Creation date in format %02d.%02d.20%02d]
 *
 * @brief     description
 *
 ****************************************************************************/

/* Protection against multiple includes.                                    */
/* Do not code anything outside the following defines (except comments)     */
#ifndef VISU_PINOUT_H
#define VISU_PINOUT_H

#include "visu.h"

/* macro definitions ********************************************************/

/* type definitions *********************************************************/

/* prototypes ***************************************************************/

/* global constants *********************************************************/

/* global variables *********************************************************/

/* global function prototypes ***********************************************/

void Visu_PinOut_Init(void);
void Visu_PinOut_Open(void);
void Visu_PinOut_Close(void);
void PinOut_Import_Backup( void );
void Visu_PinOut_Cycle(uint32_t evtc, tUserCEvt* evtv);

/****************************************************************************/
/* Protection against multiple includes ends here ! Stop editing here !     */
/****************************************************************************/
#endif  // #ifndef VISU_PINOUT_H
