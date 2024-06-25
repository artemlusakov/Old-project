
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
#ifndef CTRL_TOP_ATTACHMENT_CR600_H
#define CTRL_TOP_ATTACHMENT_CR600_H

#define MAX_LITER_PER_MIN 600
#define PRESSURE_TIMER 1
#define PRESSURE_TIMER_MS 15000
#define DENSIT_TIMER 2
#define DENSIT_TIMER_MS 350
#define LITER_SET_TIMER 3
#include "control.h"
/*************************************************************************/

/* macro definitions *****************************************************/

/* type definitions ******************************************************/



typedef struct tCR_DO {
  uint8_t     vavl_hose;

	uint8_t			valv_oil;
	uint8_t			valv_l_rail1;
	uint8_t			valv_c_rail1;
	uint8_t			valv_r_rail1;

  uint8_t			valv_l_rail2;
	uint8_t			valv_c_rail2;
	uint8_t			valv_r_rail2;

  uint8_t			valv_l_rail3;
	uint8_t			valv_c_rail3;
	uint8_t			valv_r_rail3;
	uint8_t			recycl;
	uint8_t			rail1;
	uint8_t			rail2;
	uint8_t			rail3;
	uint8_t			dispens_on;
	uint8_t			dispens_off;
	uint8_t			air_dry;
	uint8_t			drive_heating;
} CR_DO_t;

/* prototypes ************************************************************/

/**
 * @brief Отрисовка рабочих зон
 *
 * @param IsOpen Флаг работает ли установка
 */
void SetSaidOpen(int IsOpen);
void AirDryPump( void );
void AirDry( void );
void DispensControl( void );

void RailOpen_Close(int IsOpen);
void SetRailOpen(int CurrentRailOpen);
/**
 * @brief Расчет значения тока для дозирующего клапана
 * в зависимости от целевого расхода
 * @param par целевой расход жидкости
 * @return Сила тока для пропорционального клапана
 */
double CurrCalc(int par);
/**
 * @brief Расчет значения тока для дозирующего клапана
 * в зависимости от целевого расхода
 * @param CurrentLiterPerMin целевой расход жидкости
 * @return Сила тока для пропорционального клапана
 */
double CurrentByLiters(int CurrentLiterPerMin);
/**
 * @brief Расчет значения тока для дозирующего клапана
 * в зависимости от целевого расхода
 * @param CurrentLiterPerMin целевой расход жидкости
 * @param IsOpen флаг работает ли установка
 * @return Сила тока для пропорционального клапана
 */
double SetDozParameter(int CurrentLiterPerMin, int IsOpen);
void Ctrl_DispensOpenClose( void );
void Ctrl_CR600_DispensOpenClose(int isOpen);
void DicpensOpenClose(int IsOpen);
int IsMainMaskCR( void );
void SetRecycleState( void );
/**
 * @brief При опускании температуры ниже 5С включить подокгрев клапана
 */
void SetValveHeating( void );
void WorkModeSwitch(int isWorking);
void PumpWorking( void );
void Pump_NOT_Working( void );
/**
 * @return Рассчетное целевое значение рахода верхнего бака
 */
int32_t CalcLiterPerMin( void );
/**
 * @brief Изменение объемов распределяемого вещества по команде с рокера X
 */
void SetDensity(int min, int max, uint8_t step);
/**
 * @brief Увеличение зоны распределения
 */
void AddSector( void );
/**
 * @brief Уменьшение зоны распределения.
 * Не может быть меньше одного сектора
 */
void RemoveSector( void );
/**
 * @brief Установка зоны распределения по командам с рокеров джойстика
 */
void SetLinesToDraw( void );
/**
 * @brief Отрисовка зон распределения на основе переменной dispensSectors
 *
 * @param IsOpen = 1 установка работает. = 0 установка НЕ работает
 */
void DrawRoadLines(int IsOpen);
/**
 * @brief Установка значения распределения, зон распределения и их отрисовка
 *
 * @param IsOpen = 1 установка работает. = 0 установка НЕ работает
 */
void SetSaidOpen(int IsOpen);
void DispensControl( void );
void AirDry( void );
void AirDryPump( void );
void AirDry_Cycle( void );
int LiqPressureCheck( tControl * ctrl_ );
void SendToMCM( void );
/**
 * @brief Проверка таймера timerHDL и его установка на timeout мс, если его время вышло
 *
 * @return 1 - таймер вышел, 0 - таймер еще стоит
 */
int TimerCheck(int timerHDL, int timeout);
/**
 * @brief Рассчетное целевое значение рахода верхнего бака для скорости с шагом step
 *
 * @param step шаг пересчета целевого расхода
 * @return int
 */
int CheckLiterOnSpeed(int step);
/**
 * @brief Логика режима полива через шланг
 *
 */
void WaterHose( void );

/* global constants ******************************************************/

/* global variables ******************************************************/

/* global function prototypes ********************************************/

typedef struct tCR_dispens_pos {
	int32_t		min;		///< минимальное положение дозирующего клапана
	int32_t		max;		///< максимальное положение дозирующего клапан
	int32_t		cur;		///< Текущее положение дозирующего клапана
	int32_t		set;		///< Желаемое (нужное) положение дозирующего клапана
	uint32_t	tmt_set;	///< время между опросами положения
	float		vol_max[3];	///< максимальный расход жидкости в 3-х ширинах
	float		dens_max;	///< текущая макисмльно возможная плотность полива
	float		dens_stp;	///< текущий шаг изменения плотности полива
} CR_dispens_pos_t;

typedef struct tCR_ctrl {
	uint8_t		md;			///< включение CR, главного клапана и входной клапан
	uint8_t		chg;		///< смена управления
	uint8_t		boost;		///< максимальная плотность
	float		density;	///< плотность полива: кг/мин
	float		vol;		///< объём полива: л/мин
	int8_t		width;		///< ширина полива: 0 1 2 3
	int8_t		roll;		///< наклон полива: -1 = лево,  0 = центр, 1 = право
	int8_t		rail_dir;	///< 0x1 0x2 0x4 = рейки R1 R2 R3, 0x10 0x20 0x40 = направления L C R
	int8_t		dir_disp;	///< направление: 0 = стоим, -1 = закрываем, 1 = открываем
	int8_t		press_liq;	///< состояние датчика давления жидкости
	int8_t		q_valv_oil;	///< что делать с клапаном системы
	uint16_t	vol_rev;	///< текущий уровень включения реверса
	int8_t			speed_lim;				///< км/ч, предел скорости для этого объёма
}	CR_ctrl_t ;

void Ctrl_top_LRS_CR600_Cycle(uint32_t evtc, tUserCEvt* evtv );
void Ctrl_top_LRS_CR600_Timer_10ms( void );
void Ctrl_top_CR600_BP300(uint32_t evtc, tUserCEvt * evtv);
void Ctrl_top_LRS_CR600_300_Init( void );
/*************************************************************************/
/* Protection against multiple includes ends here ! Stop editing here !  */
/*************************************************************************/
#endif  // #ifndef CTRL_TOP_ATTACHMENT_CR_H
