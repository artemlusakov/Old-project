/************************************************************************
*
* File:         VISU_STATISTIK.h
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
#ifndef VISU_STATISTIK_H
#define VISU_STATISTIK_H
#include "visu.h"

/*************************************************************************/

/* macro definitions *****************************************************/
#define PATH_STATISTIC_FLASH "/gs/data/STATList.cfg"
#define PATH_STATISTIC_USB "/gs/usb/BACKUP_LAST/STATList.cfg"
/* type definitions ******************************************************/

/* prototypes ************************************************************/

/* global constants ******************************************************/

/* global variables ******************************************************/

/* global function prototypes ********************************************/

void Visu_Statistic_Open( const tVisuData* Data );
void Visu_Statistic_Cycle( const tVisuData* VData, uint32_t evtc, tUserCEvt* evtv );
int32_t GeneralOperatingTimeforError( tVisuData* VData );

void SaveStatistic( void );
void RepairStatistic( void );

int Can_Statistic( void );

void Statistics_LoadBacup( void );
void Statistics_SaveBacup( void );
/*************************************************************************/
/* Protection against multiple includes ends here ! Stop editing here !  */
/*************************************************************************/
#endif  // #ifndef VISU_STATISTIK_H
