/************************************************************************
*
* File:         ERRORLIST.h
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
#ifndef ERRORLIST_H
#define ERRORLIST_H

/*************************************************************************/

/* macro definitions *****************************************************/

/* type definitions ******************************************************/

/* prototypes ************************************************************/

/* global constants ******************************************************/

/* global variables ******************************************************/

/* global function prototypes ********************************************/
typedef enum eagErrorID {
	ERRID_MCM250_0_OFFLINE = 0x100,
	ERRID_MCM250_1_OFFLINE,
	ERRID_MCM250_2_OFFLINE,
	ERRID_MCM250_3_OFFLINE,
	ERRID_MCM250_4_OFFLINE,
	ERRID_MCM250_5_OFFLINE,
	ERRID_MCM250_6_OFFLINE,
	ERRID_MCM250_7_OFFLINE,
	ERRID_MCM250_8_OFFLINE,
	ERRID_K2_OFFLINE,
	ERRID_JOYSTICK_OFFLINE,
	ERRID_ENGINE_OFFLINE,
	ERRID_BNSO_ERROR,
	ERRID_BNSO_OFFLINE= 0x110,

	ERRID_MCM250_0_TEMP,
	ERRID_MCM250_1_TEMP,
	ERRID_MCM250_2_TEMP,
	ERRID_MCM250_3_TEMP,
	ERRID_MCM250_4_TEMP,
	ERRID_MCM250_5_TEMP,
	ERRID_K2_TEMP,
	ERRID_D3510_TEMP,

	ERRID_MAINTENANCE_BRUSH_FRONT,
	ERRID_MAINTENANCE_BRUSH_INTERAXIS= 0x120,
	ERRID_MAINTENANCE_Brush_SIDE,
	ERRID_MAINTENANCE_Brush_REAR ,
	ERRID_MAINTENANCE_PTO,
	ERRID_MAINTENANCE_OILPUMP_LOAD,
	ERRID_MAINTENANCE_OILPUMP_UNLOAD,
	ERRID_MAINTENANCE_RTR,
	ERRID_MAINTENANCE_RTR_HUMID,
	ERRID_MAINTENANCE_LRS,
	ERRID_MAINTENANCE_ANNUAL,

	ERRID_CRITICAL_OIL_LEVEL,
	ERRID_NO_OIL_SENSOR= 0x130,
	ERRID_NO_OIL_PRESSURE,
	ERRID_CHECKUSSENS1,
	ERRID_CHECKUSSENS2,
	ERRID_DECREASINGOILEVEL ,
	ERRID_HYDRAULICOILFILTER,
	ERRID_CRITICAL_LIQUID_LEVEL_E2000,
	ERRID_NO_REAGENT,
	ERRID_OVERLOAD,
	ERRID_HIGH_VOLTAGE,
	ERRID_LOW_VOLTAGE,
	ERRID_OVERSPEED,
	ERRID_NO_MATERIALSENSOR= 0x140,
	ERRID_NO_WATERSENSOR,
	ERRID_HYDRAULIC_SYSTEM_NOT_AVAILABLE ,
	ERRID_ANNUAL_MAINTENANCE,
	ERRID_HYDRAULICVALVETIPPINGERROR_0,
	ERRID_HYDRAULICVALVETIPPINGERROR_1,
	ERRID_HYDRAULICVALVETIPPINGERROR_2,
	ERRID_HYDRAULICVALVETIPPINGERROR_3,

	ERRID_CONVEYOR_NO_ROTATION,
	ERRID_MAX_ENGINE_RPM,

	ERRID_CHECKLIGHTS= 0x150,
	ERRID_CHECKPLOUGHLIGHTS ,
	ERRID_CHECKFRONTBEACON,
	ERRID_CHECKWORKSIDE,
	ERRID_CHECKOKB600WORKREAR,
	ERRID_CHECKBRUSHLIGHT,
	ERRID_CHECKLRSLEFTROADSIGN,
	ERRID_CHECKLRSRIGHTROADSIGN,
	ERRID_CHECKLRSREARBEACON,
	ERRID_CHECKLRSWORKREAR,
	ERRID_CHECKRTRLEFTROADSIGN,
	ERRID_CHECKRTRRIGHTROADSIGN= 0x160,
	ERRID_CHECKRTRREARBEACON,
	ERRID_CHECKRTRWORKREAR ,
	ERRID_CHECKRTRHOPPERTOP,

	ERRID_CR_COMMON,
	ERRID_CR_VALV_OIL,
	ERRID_CR_VALV_L,
	ERRID_CR_VALV_C,
	ERRID_CR_VALV_R,
	ERRID_CR_RECYCL,
	ERRID_CR_RAIL1 = 0x170,
	ERRID_CR_RAIL2,
	ERRID_CR_RAIL3,
	ERRID_CR_DISPENS_ON,
	ERRID_CR_DISPENS_OFF,
}eErrorID;

/*************************************************************************/
/* Protection against multiple includes ends here ! Stop editing here !  */
/*************************************************************************/
#endif  // #ifndef ERRORLIST_H
