/************************************************************************
*
* File:         j1939.h
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
#ifndef J1939_H
#define J1939_H
#include "control.h"
/*************************************************************************/

/* macro definitions *****************************************************/

/* type definitions ******************************************************/
/**
* @brief first PGN for sending data to the BNSO
*
*/
typedef struct tagPGN_FF01 {
/*-----------------------------0--------------------------------------*/
	unsigned int FrontAttach_FloatingMode : 2;     // 0 off , 1 on
	unsigned int FrontAttach_VerticalPos : 2;     // 1 raised, 2 pressed
	unsigned int FrontAttach_HorizontalPos : 2;     // 1 left, 2 right
	unsigned int FrontAttach_Rotation : 2;     // 0 no rot, 1 rot, not available
  /*-----------------------------1--------------------------------------*/
	unsigned int FrontAttach_BladeType : 2;     // 1 rubber, 2 steel
	unsigned int Interaxle_FloatingMode : 2;     // 0 on , 1 off
	unsigned int Interaxle_VerticalPos : 2;     // 1 raised, 2 pressed
	unsigned int Interaxle_HorizontalPos : 2;     // 1 left, 2 right
  /*-----------------------------2--------------------------------------*/
	unsigned int Interaxle_Rotation : 2;     // 0 no rot, 1 rot, not available
	unsigned int Interaxle_BladeType : 2;     // 1 rubber, 2 steel
	unsigned int SideAttach_FloatingMode : 2;     // 0 on , 1 off
	unsigned int SideAttach_VerticalPos : 2;     // 1 raised, 2 pressed
  /*-----------------------------3--------------------------------------*/
	unsigned int SideAttach_Tilt : 2;     // 1 raised, 2 pressed
	unsigned int SideAttach_HorizontalPos : 2;     // 1 left, 2 right
	//unsigned int SideAttach_Rotation : 2;     // 0 no rot, 1 rot, not available
	unsigned int TopEquip: 4;     //  1-// RTR, 2-BP300 3-C610H 4-Hypro6500C
  /*-----------------------------4--------------------------------------*/
	//unsigned int SideAttach_BladeType : 2;     // 1 rubber, 2 steel
	unsigned int ReagentFeedRate : 8;        // 0 to 8000 rpm
   /*-----------------------------5--------------------------------------*/
	unsigned int SprayDensity : 8;        // 1 Bit = 10g/mÃ‚Â²
   /*-----------------------------6--------------------------------------*/
	unsigned int SprayWidth : 8;        // 1 Bit = 1m
   /*-----------------------------7--------------------------------------*/
	unsigned int SpeedSimulation : 2;    // Режим имитации скорости КДМ 0 – выкл. 1 – вкл. (30км/ч), другие значения зарезервироаны для будущего
	unsigned int Dummy : 2;     // for future using
	unsigned int PositionAsymmetryDrive : 4;     // 0 extreme left, 1 left, 2 slacker, 3 right, 4 extreme right


}tPGN_FF01;

// Таким должен быть FF02
//typedef struct tagPGN_FF02 {
//    unsigned int  HydSysTemp    :8;
//    unsigned int  HydSysPres    :16;
//    unsigned int  HydSysOilLev  :8;
//    unsigned int  OilTemp       :2;
//    unsigned int  res_0         :6;
//    unsigned int  Reagent       :4;
//    unsigned int  res_1         :10;
//    unsigned int  res_2         :8;
//}tPGN_FF02;

/**
*  @brief second PGN for sending data to the BNSO
*
*/
typedef struct tagPGN_FF02 {
	//* [0-1-2-3]
	unsigned int	HydSysTemp		: 8;
	unsigned int	HydSysPres		: 16;
	unsigned int 	HydSysOilLev	: 8;
	//* [4]
	unsigned int	OilTemp			: 2;
	unsigned int	sens_tail		: 1;
	unsigned int	sens_liq_humid	: 1;
	unsigned int	sens_spread		: 1;
//	unsigned int	pos_adrv		: 3;		///< 0 - неопределённо, 1 - 5 = положение
  unsigned int typeReagent   :3;  //1 - соль 2 -ПСС 3 - песок
	//* [5]
	unsigned int	sw_tail			: 1;
	unsigned int	sw_spread		: 1;
	unsigned int	sw_liq_humid	: 1;
	unsigned int	sw_rot_humid	: 1;
	unsigned int	sw_rot_conv		: 1;
	unsigned int	prcnt_humid		: 3;		///< 0 -:- 7 ==> 0% -:- 70%
	//* [5-7]
	unsigned int	rate_humid		: 7;		///< 0 - 127
	unsigned int	rate_conv		: 5;		///< 0 - 31
	unsigned int	mat_dens		: 4;		///< 1024 + 128*V или (1000 + 100*V)
}tPGN_FF02;

//!	@brief PGN ошибки и предупреждения
//!	это первый набор
typedef struct tagPGN_FF03_0 {
	// [0]
	unsigned int	code: 4;	///< поле кода, можно использовать для расширения списка
	unsigned int	: 1;		///< MCM250_1 недоступно,	Предупреждение
	unsigned int	: 1;		///< MCM250_2 недоступно,	Предупреждение
	unsigned int	: 1;		///< MCM250_3 недоступно,	Предупреждение
	unsigned int	: 1;		///< MCM250_4 недоступно,	Предупреждение
	// [1]
	unsigned int	: 1;		///< MCM250_5 недоступно,	Предупреждение
	unsigned int	: 1;		///< MCM250_6 недоступно,	Предупреждение
	unsigned int	: 1;		///< Клавиатура недоступна,	Предупреждение
	unsigned int	: 1;		///< Джойстик недоступен,	Предупреждение
	unsigned int	: 1;		///< БНСО недоступен,	Предупреждение
	unsigned int	: 1;		///< Двигатель недоступен,	Предупреждение
	unsigned int	: 1;		///< MCM250_1 Высокая температура,	Критичный
	unsigned int	: 1;		///< MCM250_2 Высокая температура,	Критичный
	// [2]
	unsigned int	: 1;		///< MCM250_3 Высокая температура,	Критичный
	unsigned int	: 1;		///< MCM250_4 Высокая температура,	Критичный
	unsigned int	: 1;		///< MCM250_5 Высокая температура,	Критичный
	unsigned int	: 1;		///< MCM250_6 Высокая температура,	Критичный
	unsigned int	: 1;		///< Клавиатура 2 недоступна,	Предупреждение
	unsigned int	: 1;		///< К2_Высокая температура,	Критичный
	unsigned int	: 1;		///< ТО передней щётки,	Информация
	unsigned int	: 1;		///< ТО средней щётки,	Информация
	// [3]
	unsigned int	: 1;		///< SideBrushService,	Информация
	unsigned int	: 1;		///< ТО задней щётки,	Информация
	unsigned int	: 1;		///< HYD_PUMP_LOAD_SERVICE,	Информация
	unsigned int	: 1;		///< HYD_PUMP_UNLOAD_SERVICE,	Информация
	unsigned int	: 1;		///< ТО РТР,	Информация
	unsigned int	: 1;		///< ТО увлажнения РТР,	Информация
	unsigned int	: 1;		///< ТО водяного насоса,	Информация
	unsigned int	: 1;		///< Пройдите ТО,	Информация
	// [4]
	unsigned int	: 1;		///< Критический уровень масла,	Критичный
	unsigned int	: 1;		///< Датчик уровня масла недоступен,	Критичный
	unsigned int	: 1;		///< Снижение уровня масла,	Предупреждение
	unsigned int	: 1;		///< Масляный фильтр засорён,	Критичный
	unsigned int	: 1;		///< Мало жидкости в баках Е2000/2200,	Информация
	unsigned int	: 1;		///< Нет реагента,	Информация
	unsigned int	: 1;		///< Перегруз,	Информация
	unsigned int	: 1;		///< Высокое напряжение,	Предупреждение
	// [5]
	unsigned int	: 1;		///< Низкое напряжение,	Предупреждение
	unsigned int	: 1;		///< Превышение скорости,	Предупреждение
	unsigned int	: 1;		///< Датчик материала недоступен,	Предупреждение
	unsigned int	: 1;		///< Датчик увлажнения недоступен,	Предупреждение
	unsigned int	: 1;		///< Датчик жидкости недоступен,	Предупреждение
	unsigned int	: 1;		///< Ежегодное ТО,	Информация
	unsigned int	: 1;		///< Ошибка на секции 1, Значение = %d,	Предупреждение
	unsigned int	: 1;		///< Ошибка на секции 2, Значение = %d,	Предупреждение
	// [6]
	unsigned int	: 1;		///< Ошибка на секции 3, Значение = %d,	Предупреждение
	unsigned int	: 1;		///< Ошибка на секции 4, Значение = %d,	Предупреждение
	unsigned int	: 1;		///< Конвейер не вращается,	Информация
	unsigned int	: 1;		///< Высокие обороты двигателя. Макс.: %d,	Предупреждение
	unsigned int	: 1;		///< Проверь огни, не используется,	Предупреждение
	unsigned int	: 1;		///< Проверь фары,	не используется,	Предупреждение
	unsigned int	: 1;		///< Проверь передние маяки,	Предупреждение
	unsigned int	: 1;		///< Проверь боковое освещение	не используется	Предупреждение
	// [7]
	unsigned int	: 1;		///< Проверь левый знак ПМ,	Предупреждение
	unsigned int	: 1;		///< Проверь правый знак ПМ,	Предупреждение
	unsigned int	: 1;		///< Проверь задние маяки ПМ,	Предупреждение
	unsigned int	: 1;		///< Проверь заднее освещение ПМ,	Предупреждение
	unsigned int	: 1;		///< Проверь левый знак ПС,	Предупреждение
	unsigned int	: 1;		///< Проверь правый знак ПС,	Предупреждение
	unsigned int	: 1;		///< Проверь задние маяки ПС,	Предупреждение
	unsigned int	: 1;		///< Проверь заднее освещение ПС,	Предупреждение
}	tPGN_FF03_0;

typedef struct tagPGN_FF06 {
	// 00 - выключено, 01 - включено, 10 - ошибка, 11 - отсутствует
/*-----------------------------0--------------------------------------*/
	unsigned int FrontBeaconCab : 2;
	unsigned int WorkSide : 2;
	unsigned int BrushLight : 2;
	unsigned int RTR_HopperTop : 2;
  /*-----------------------------1--------------------------------------*/
	unsigned int RearBeacon : 2;
	unsigned int WorkRear : 2;
	unsigned int LeftRoadSign : 2;
	unsigned int RightRoadSign : 2;
  /*-----------------------------2--------------------------------------*/
	unsigned int OKB600_WorkRear : 2;
	unsigned int res_0 : 6;
  /*-----------------------------3--------------------------------------*/
	unsigned int res_1 : 8;
  /*-----------------------------4--------------------------------------*/
	unsigned int res_2 : 8;
   /*-----------------------------5--------------------------------------*/
	unsigned int res_3 : 8;
   /*-----------------------------6--------------------------------------*/
	unsigned int res_4 : 8;
   /*-----------------------------7--------------------------------------*/
	unsigned int res_5 : 8;
}tPGN_FF06;

typedef struct tagPGN_FF07 {
/*-----------------------------0--------------------------------------*/
	unsigned int ShieldPos : 4;
	unsigned int res_0 : 4;
  /*-----------------------------1--------------------------------------*/
	unsigned int res_1 : 8;
  /*-----------------------------2--------------------------------------*/
	unsigned int res_2 : 8;
  /*-----------------------------3--------------------------------------*/
	unsigned int res_3 : 8;
  /*-----------------------------4--------------------------------------*/
	unsigned int res_4 : 8;
   /*-----------------------------5--------------------------------------*/
	unsigned int res_5 : 8;
   /*-----------------------------6--------------------------------------*/
	unsigned int res_6 : 8;
   /*-----------------------------7--------------------------------------*/
	unsigned int res_7 : 8;
}tPGN_FF07;

/* prototypes ************************************************************/

/* global constants ******************************************************/

/* global variables ******************************************************/
tPGN_FF01* J1939_GetPGN_FF01( void );
tPGN_FF02* J1939_GetPGN_FF02( void );
tPGN_FF06* J1939_GetPGN_FF06( void );
tPGN_FF07* J1939_GetPGN_FF07( void );

void J1939_SendPGN( uint32_t id, void* data, size_t size_data );
/* global function prototypes ********************************************/

/**
* @brief Initalizes the J1939 on CAN for the Engine and the BNSO
*
* @param CAN 0 for CAN0, 1 for CAN1
* @param Control
*/
void J1939_Init( uint32_t CAN, tControl* Control );

/**
* @brief Call this in the UserCCylce
*
* @param Ctrl
*/
void J1939_Cycle( tEngine* Ctrl );

void J1939_Timer( tControl* Ctrl );
void VIN_SEND( void );
/*************************************************************************/
/* Protection against multiple includes ends here ! Stop editing here !  */
/*************************************************************************/
#endif  // #ifndef J1939_H
