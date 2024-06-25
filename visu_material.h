/************************************************************************
*
* File:         VISU_MATERIAL.h
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
#ifndef VISU_MATERIAL_H
#define VISU_MATERIAL_H
#include "visu.h"
#include "gselist.h"
/*************************************************************************/

/* macro definitions *****************************************************/

#define PATH_REAGENTS "/gs/data/reagent.cfg"
#define PATH_REAGENTS_USB "/gs/usb/reagent.cfg"
#define PATH_REAGENTS_USB_BACKUP "/gs/usb/BACKUP_LAST/reagent.cfg"

/* type definitions ******************************************************/
typedef struct tagReagent {
	int32_t idx;
	char Name[64];
	uint32_t MaterialDensity;
	uint32_t SpreadingDensity_Max;
	uint32_t SpreadingDensity_Step;
}tReagent;

typedef struct tReagentTable {
	tGSList ReagentList;
	tReagent* Actual;
}tReagentTable;
/* prototypes ************************************************************/

/* global constants ******************************************************/

/* global variables ******************************************************/

/* global function prototypes ********************************************/
void VisuMaterial_Init( const tVisuData* Data );
void VisuMaterial_Open( const tVisuData* Data );
void VisuMaterial_Close( const tVisuData* Data );
void VisuMaterial_Cycle( const tVisuData* Data, uint32_t evtc, tUserCEvt* evtv );
void Reagent_Next( void );
void Reagent_Draw( void );

const tReagent* Reagent_SetIdx( uint32_t idx );
int32_t Reagent_GetIdx( void );
int32_t Reagent_GetNum( void );
void Reagent_Import_FOBACKUP( void );
const tReagent* GetActualReagent( void );

/*************************************************************************/
/* Protection against multiple includes ends here ! Stop editing here !  */
/*************************************************************************/
#endif  // #ifndef VISU_MATERIAL_H
