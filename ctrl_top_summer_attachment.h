#ifndef CTRL_TOP_SUMMER_ATTACHMENT_H
#define CTRL_TOP_SUMMER_ATTACHMENT_H

#include "hydvalvectrl.h"
#include "gsemaint_runtime.h"
#include "sql_table.h"

//
#define PUMP_OFF 0
#define PUMP_ON 1
//

// LRS
#define Consumption_RR400 400
#define Width_Line 4
#define WATERPUMP_TYPE_BP300 0
#define WATERPUMP_TYPE_C610H 1
#define WATERPUMP_TYPE_NCR 2

#define LRS_SECTION_BIN_RIGHT 1
#define LRS_SECTION_BIN_CENTER 2
#define LRS_SECTION_BIN_LEFT 4


typedef struct tagWaterPump {
	uint8_t  PumpInit;
	uint8_t  on;
	tValv		 Valv; // Насос
	uint8_t  Type;
} tWaterPump;

typedef struct tagWaterPumpIntake {
	uint8_t  FuncWaterIntake;
	uint8_t  on;
	tValv    ValvIntake; // Насос
	uint8_t  TypeIntake;
	tValv		 StraitValv; // Клапан забора воды
} tWaterPumpIntake;

typedef struct tagMF500Bar {
	uint8_t on;
	uint8_t Device;
	uint8_t Pos;
} tMF500Bar;

typedef enum eagLRS_SECTIONS {
	LRS_SECTION_NONE,           //000
	LRS_SECTION_RIGHT,          //001
	LRS_SECTION_CENTER,         //010
	LRS_SECTION_CENTER_RIGHT,   //011
	LRS_SECTION_LEFT,           //100
	LRS_SECTION_LEFT_RIGHt,     //101
	LRS_SECTION_LEFT_CENTER,    //110
	LRS_SECTION_ALL,            //111
	LRS_SECTION_NUM
} eLRS_SECTIONS;

typedef struct tagLRS {
	eLRS_SECTIONS  ActiveSection;
	tSQLTable* 	 	 SectionTables[LRS_SECTION_NUM];
	uint32_t       Density;
	uint32_t       pwm;
	tValv          DensityValve;
	tValv          CenterValve, LeftValve, RightValve;
	tMaintRuntime* Runtime300;
	tMaintRuntime* Runtime600;
	tMaintRuntime* Runtime_Reset300;
	tMaintRuntime* Runtime_Reset600;
} tLRS;

typedef struct tagSummerTop {
  tWaterPump 			 Pump;
	tWaterPumpIntake PumpIntake;
  tValv 					 MainValve;

	tMF500Bar 			 MF500Bar;
	uint8_t  				 WasherAntiBoost;
	tLRS 						 LRS;

} tSummerTop;


void WaterPump_Init(tSummerTop * sTop, uint32_t Type);
void WaterPump_Intake_Init(tSummerTop * sTop, int32_t Device, int32_t Pos, int32_t TypeIntake);
void WaterPump_Cycle(tSummerTop * sTop);
uint8_t WaterPump_IsOn(tWaterPump * Pump);
void WaterPump_ChangePinout(tWaterPump * Pump, int8_t MCM, int8_t Out);
void WaterPump_Draw(tSummerTop * sTop);
void WaterPumpIntake_Switch(tWaterPumpIntake * PumpIntake, uint32_t Value);
uint8_t WaterIntake_IsOn(tWaterPumpIntake * PumpIntake);
uint8_t MF500Bar_IsOn(tMF500Bar * MF500Bar);

void WaterPump_SetMaxLiter(int32_t ml);

void RR300_Init(tSummerTop * sTop);
void RR300_Cycle(tSummerTop * sTop);

void RR400_Init(tSummerTop * sTop);
void RR400_Cycle(tSummerTop * sTop);

void RR300_RR400_Draw(tSummerTop * sTop);
void LRS_Timer_10ms(tLRS* lrs);

#endif  // #ifndef CTRL_TOP_SUMMER_ATTACHMENT_H
