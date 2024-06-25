#ifndef CONTROL_H
#define CONTROL_H
#include "io_types.h"
#include "ctrl_plough.h"
#include "ctrl_brush.h"
#include "okb600.h"
#include "gsemaint.h"
#include "gsemaint_lifetime.h"
#include "io_joystick_v85_b25.h"
#include "commands.h"
#include "ctrl_light.h"
#include "ctrl_top_summer_attachment.h"
#include "ctrl_top_winter_attachment.h"

#define TOP_WINTER 1
#define TOP_SUMMER 2

enum	TCONST {
  MOVE        = 2,
	ERR			= -1,
	OK			= 0,
	UNK			= -1,
	OFF			= 0,
	ON			= 1,
	MD_UNDEF	= UNK,				///< Режим неопределён
	MD_INI		= OFF,				///< Начальный
	MD_WRK		= ON,				///< Рабочий
	MD_SET,							///< Установка
	MD_CLR,							///< Сброс
	MD_CTRL,						///< Контроль
	MD_CHK,							///< Проверка
	MD_DUMP,						///< разгрузка
	MD_AIRDRY,						///< Продувка
	MD_AIRDRY_PUMP,						///< Продувка
	//*
	COLOR_BLACK		= 0x00000000,
	COLOR_WHITE		= 0x00FFFFFF,
	COLOR_GRAYL		= 0x00808080,
	COLOR_GRAYD		= 0x00333333,
	COLOR_RED		= 0x007F0000,
	COLOR_REDL		= 0x00FF0000,
	COLOR_REDD		= 0x00330000,
	COLOR_GREEN		= 0x00007F00,
	COLOR_GREENL	= 0x0000FF00,
	COLOR_GREEND	= 0x00003300,
	COLOR_BLUE		= 0x0000007F,
	COLOR_BLUEL		= 0x000000FF,
	COLOR_BLUED		= 0x00000033,
	COLOR_YELLOW	= 0x007F7F00,
	COLOR_YELLOWL	= 0x00FFFF00,
	COLOR_YELLOWD	= 0x00333300,
	COLOR_CYAN		= 0x00007F7F,
	COLOR_MAGENT	= 0x007F007F,
};

enum {
	//* CR MCM250_5
	CR_PIN_SIGN_LEFT	= 0,	///< 3	-> Левый знак
	CR_PIN_SIGN_RIGHT	= 2,	///< 4	-> Правый знак
	CR_PIN_VALV_OIL		= 1,	///< 5	-> основной клапан масла
	CR_PIN_VALV_L		= 3,	///< 6	-> левый клапан
	CR_PIN_VALV_C		= 5,	///< 7	-> центральный клапан
	CR_PIN_VALV_R		= 7,	///< 8	-> правый клапан
	CR_PIN_DISPENS_ON	= 1,	///< 5	-> дозирующий клапан, открытие
	CR_PIN_DISPENS_OFF	= 3,	///< 6	-> дозирующий клапан, закрытие
	CR_PIN_RECYCL		= 0,	///< 5	-> клапан рециркуляции
	CR_PIN_RAIL1		= 2,	///< 4	-> клапан рейки 1
	CR_PIN_RAIL2		= 4,	///< 9	-> клапан рейки 2
	CR_PIN_RAIL3		= 6,	///< 10	-> клапан рейки 3
	//* CR MCM250_6
	CR_PIN_PRESS		= 0,	///< 24	<- датчик давления
	CR_PIN_DISPENS_POS	= 0,	///< 20	<| Потенциометр положения пропорционального клапана
	CR_PIN_LIQMETER		= 1,	///< 21 <| mA, расходомер
	CR_BTN_ALARM		= 1,	///< 25 <- кнопка аврийной остановки
	CR_PIN_METLIQ_ERR		= 2,	///< 26 <- ошибка данных расходомера
  };

typedef enum eagABC_Equipment {
	EQUIP_A = 0,
	EQUIP_B = 1,
	EQUIP_C = 2
} eagABC_Equipment;

typedef struct tagCtrlAutomatic {
	OperatingMode on;
} tCtrlAutomatic;

typedef struct tagEngine {
	char     name[48];
	uint32_t Active;
	uint32_t Address;
	uint32_t Hdl;
	uint32_t Hdl2;
	int16_t rpm;
  float PassedWay;
	int8_t	SelectedGear;
	int16_t AirPressure;
	int16_t OilTemp;
	int16_t OilPressure;
	int16_t OilLevel;
	int16_t CoolentTemp;
	int16_t CoolentPressure;
	int16_t CoolentLevel;
	int16_t AmbientAirTemp;
	int16_t FuelTemp;
	int16_t speed;
	int16_t speedKDM;
	int8_t PTO_Engaged;
} tEngine;

typedef struct tagBNSO {
	uint32_t Hdl;
	uint32_t Active;
	uint32_t Address;
} tBNSO;

typedef struct tagABCEquip {
	tABC_Attach  ABC_Attach;
	tBrushValve BrushValve;
	tOKB600Valve OKB600Valve;
} tABCEquip;

typedef enum eagDISensorState {
	DI_SENSOR_OFF = 0,
	DI_SENSOR_ON = 1,
	DI_SENSOR_UNKNOWN = 2,
	DI_SENSOR_INACTIVE = 3
} eDISensorState;

typedef struct tagSensorDI {
	eDISensorState Spreading;
	eDISensorState Ignition;
	eDISensorState SelectedGear;
	eDISensorState ClutchSensor;
	eDISensorState Humidifier;
	eDISensorState MaterialPresense;
	eDISensorState OilFilter;
	eDISensorState UpperOilLevel;
	eDISensorState LowerOilLevel;
	eDISensorState CheckOilLevelSensor;
	eDISensorState PTO_Active;
	eDISensorState tail_pos;
  eDISensorState cr_pressure;
	eDISensorState cr_metliq_err;
	eDISensorState MF500Bar;
	eDISensorState pump_airdry;
} tSensorDI;

typedef struct tagSensorAI {
	tIOT_AI  OilTemp;
	tIOT_AI  UsSens_1;
	tIOT_AI  UsSens_2;
	tIOT_AI  OilPressure;
  tIOT_AI  cr_dispens_pos;
	tIOT_AI  cr_liqmeter;
} tSensorAI;

typedef struct tagSensorFreq {
	uint32_t ConveryRotation;
	uint32_t humid_pump_rot;
} tSensorFreq;

typedef struct tagSensors {
	tSensorDI   DI;
	tSensorAI   AI;
	tSensorFreq Freq;
} tSensors;

typedef struct tagHydraulicSystem {
	tMaintRuntime* load;
	tMaintRuntime* load_Reset;
	tMaintRuntime* unload;
	tMaintRuntime* unload_Reset;
} tHydraulicSystem;

typedef struct tagDevice {
	tIOT_Button		Button[12];
} tDevice;

typedef struct tagTopEquip {
	uint8_t TopType; // 1 - Зимнее оборудование, 2 - Летнее оборудование
	uint8_t BoostOn;

	tSummerTop SummerTop;
	tWinterTop WinterTop;
} tTopEquip;

typedef struct tagControl {
	tIOT_Button		emcy;
  tIOT_Button		emcy_2;
	uint8_t			q_emcy;
	tCtrlAutomatic	automatic;
	t_Joystick		joystick;
	tJSCommands		cmd_no_key;
	tJSCommands		cmd_ABC[3];
	uint8_t			pressed_ABC;

	tLight			light;
	tEngine			engine;
	tBNSO             BNSO;
	tHydraulicSystem	hydr;
	tSensors		sens;
	tMaint			maint;
	tMaint			maint_hour_cnt;
	tMaintLifetime*	anual_maint;
	tABCEquip         ABC[3];
	tTopEquip         top;
	tDevice           D3510;
	uint32_t          max_speed;
	eDISensorState	cr_emcy;
} tControl;



typedef void (*fpModuleInit)( tControl* ctrl_ );
typedef void (*fpModuleCycle)( tControl* ctrl_, uint32_t evtc, tUserCEvt* evtv );
typedef void (*fpModuleTimer)( tControl* ctrl_ );
typedef void (*fpModuleDeInit)( tControl* ctrl_ );

const tControl* CtrlGet( void );
tControl*		ctrl_get( void );

void Ctrl_Init( void );
void Ctrl_Cycle( uint32_t evtc, tUserCEvt* evtv );
void Ctrl_Timer_MS( void );

void CheckRange( int32_t* value, int32_t min, int32_t max );

void Ctrl_Automatic_Toggle( void );


int32_t TopGetSpeed( void );
int32_t GetSpeedKDM( void );
void Top_Boost_Cycle(tTopEquip *top);
/**
* @brief Use this function in the attachments to set the maximum speed of a vehicle.
* If the acutally set maximum speed is smaller then Maxspeed, the maximum Speed
* won't be overwritten.
*
* @param MaxSpeed
*/
void Ctrl_SetMaxSpeed( uint32_t MaxSpeed );

/**
* @brief Resets the maximum speed of the vehicle. It has no Limit, now anymore,
* until you set a new maximum speed with Ctrl_SetMaxSpeed
*/
void Ctrl_ResetMaxSpeed( void );

/**
* @brief Deinits the Controle structure at device shut down.
*
*/
void Ctrl_DeInit( void );

void CtrlI_StandardDOs_SetRearWorkLight_ForCR600( void );

float		liner_interpret2( float x1_, float x2_, float y1_, float y2_, float x0_ );

/*************************************************************************/
/* Protection against multiple includes ends here ! Stop editing here !  */
/*************************************************************************/
#endif  // #ifndef CONTROL_H
