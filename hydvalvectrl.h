/************************************************************************
*
* File:         HYDVALVECTRL.h
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
#ifndef HYDVALVECTRL_H
#define HYDVALVECTRL_H

/*************************************************************************/
#include	<stdint.h>
/* macro definitions *****************************************************/

/* type definitions ******************************************************/

/* prototypes ************************************************************/

/* global constants ******************************************************/

/* global variables ******************************************************/

/* global function prototypes ********************************************/

typedef enum eagValvType {
	VALVE_TYPE_NONE,
	VALVE_TYPE_DISK,
	VALVE_TYPE_PVE_AHS,
	VALVE_TYPE_PVE_O,
	VALVE_TYPE_PROP_12V,
	VALVE_TYPE_PVE_AHS32,
	VALVE_TYPE_OMFB
} eValvType;

typedef struct tagValveProp_0_50_100 {
  eValvType Type;
	uint8_t wobble;
	uint8_t wobble_cnt;
	uint32_t output;
	uint32_t freq;
	uint32_t duty;  //0 - 1000;
	uint32_t MCM_Device;
	uint32_t ChangeTime;
} tValveProp;

typedef struct tagValvDig_2_2 {
  eValvType Type;
	uint8_t MCM_Device;
	uint8_t pos;
	uint8_t val;
	uint32_t ChangeTime;
} tValvDig_2_2;

typedef struct tagValv {
	eValvType Type;
	union   {
		tValveProp PVEH;
		tValvDig_2_2 PVEO[2];
	} 			Valve;
	uint32_t state_old;
} tValv;

/* prototypes ************************************************************/

/* global constants ******************************************************/

/* global variables ******************************************************/

/* global function prototypes ********************************************/

/**
 * @brief Функция питания секции распределителя.
 *
 * @param MCM блок МСМ
 * @param Out цифровой выход
 * @param Value значение
 */
void CtrlSetDigitalOut(uint8_t MCM, int8_t Out, uint8_t value);

/**	@brief This Function generates a SQL-List with a Table to
*/
void   GlobalInitSqlTableLiter( void );

void     ValveProp_Init( tValveProp* valv, uint32_t MCM_Device, uint32_t Output, uint32_t freq, uint32_t wobble );
void     ValveProp_NoPort_Init( tValveProp* valv, uint32_t MCM_Device, uint32_t Output );
void     ValveProp12V_Init( tValv* valv, int32_t MCM_Device, uint32_t Output, uint32_t freq, uint32_t wobble );

/**
* @brief Controls danfoss vales. Use -1000 for controling channel A and 1000 for channel
*
*
* @param valv
* @param value -1000->25%PWM, 0->50%PWM, 1000->75%PWM
*/
void     ValvProp_25_50_75_Set( tValveProp* valv, int32_t value );
void		 Ctrl_Valve_SetLiter(tValveProp* valv, int32_t ml);
void		 Ctrl_IntakeValve_SetLiter(tValveProp* valv, int32_t ml);
void     ValvPropPVEP_SetLiter( tValveProp* valv, int32_t ml );
void     ValvPropPVEP_PVG32_SetLiter( tValveProp* valv, int32_t ml );
int32_t Rotate_OMFB_SetLiter(int32_t ml );
void     ValvProp_Set( tValveProp* valv, uint32_t value );
int32_t  ValvProp_Timer_100ms( tValveProp* valv );
void ValvDig_2_2_Init( tValvDig_2_2* Valv, uint8_t Device, uint8_t pos );
void	ValvDig_2_2_Set( tValvDig_2_2* Valv, uint8_t value );
int32_t ValvDig_GetState( const tValv* Valv );
/**	@brief Check if the connected va
* @param Valv
* @return uint32_t */
uint32_t ValvDig_2_2_CheckState( tValvDig_2_2* Valv );

/**	@brief if the outpuit of this valve needs more than 50 mA, the function return 1;
* Else it returns 0.
* @param Valv
* @return uint32_t */
int32_t ValvDig_2_2_GetState( const tValvDig_2_2* Valv );

//Init function to set a standard valve.
/**	@brief
* @param Valv Pointer to the valve
* @param Device1 MCM Device of Output.  if -1 Valvetype = VALVE_TYPE_NONE else see Device
* @param Pos1
* @param Device2 MCM Device of Output. if Device1 != -1: Device2 = -1 -> ValveType = VALVE_TYPE_PVE_AHS, else ValveType = VALVE_TYPE_PVE_O
* @param Pos2
*/
void Valv_Init( tValv* Valv, int32_t  Device1, int32_t Pos1, int32_t Device2, int32_t Pos2 );
void Valv_PVG32_Init( tValv* Valv, int32_t  Device1, int32_t Pos1, int32_t Device2, int32_t Pos2 );
void Valv_OMFB_Init( tValv* Valv, int32_t  Device1, int32_t Pos1, int32_t Device2, int32_t Pos2 );

/**	@brief
* @param Valv Pointer to the valve
* @param Device MCM Device of Output.  if -1 Valvetype = VALVE_TYPE_NONE else VALVE_TYPE_DISK
* @param Pos
*/
void ValvDisk_Init( tValv* Valv, int32_t Device, int32_t Pos );
void Valv_Set( tValv* Valv, int32_t value );

int32_t Valv_GetState( const tValv* Valv );
int32_t Valv_GetValue( const tValv* Valv );

/**	@brief Returns Position of a Valve.
* @param Valv
* @return int32_t returns -1 if invalid, 0/1 if diskret valve, 0-1000 if analouge Valve */
int32_t Valv_GetPos( const tValv* Valv );

void Valv_Timer_100ms( tValv* Valv );
/*************************************************************************/
/* Protection against multiple includes ends here ! Stop editing here !  */
/*************************************************************************/
#endif  // #ifndef HYDVALVECTRL_H
