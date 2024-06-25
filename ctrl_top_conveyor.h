#ifndef CTRL_TOP_CONVEYOR_H
#define CTRL_TOP_CONVEYOR_H

#include "gselist.h"
#include "sql_table.h"
#include "gsemaint_runtime.h"
#include "hydvalvectrl.h"
#include "drawfunction.h"

#define TEMPERATURE_PER_REAGENT 3
#define NUMREAGENTS 5

typedef struct tagTemperatureTable {
	int32_t    temp;
	tSQLTable* Table;
}
tTemperatureTable;

typedef struct tagConveyor {
	uint8_t				running;
	uint8_t				on;
	uint8_t				initShield;
	uint32_t			g_min;
	uint32_t      g_minvisu;
	uint32_t			g_min_max;
	uint32_t			pwm;
	tMaintRuntime*		Runtime;
	tMaintRuntime*		Runtime_Reset;
	tValv				Valve;
	tPaintText			ColourDensity;
	tTemperatureTable	TempTables[TEMPERATURE_PER_REAGENT];
}tConveyor;

void ConveyorCreate( tConveyor* Conveyor );
void Conveyor_Switch( tConveyor* Conveyor, uint32_t value );
void ConveyorInit( tConveyor* Conveyor, uint32_t Device, uint32_t Pos );
void ConveyorCycle( tConveyor* Conveyor, int32_t temp );
void ConveyorCycle_UH( tConveyor* Conveyor, int32_t temp );
int32_t  ConveyorGetMaxDensity( tConveyor* Conveyor );
void ConveyorDraw( const tConveyor* Conveyor );
void ConveyorTimer( tConveyor* Conveyor );
uint8_t getFlagUnloading (void);
void setFlagUnloading(uint8_t flag);
uint32_t GetPWM_RTRHumid(void);

#endif  // #ifndef CTRL_TOP_CONVEYOR_H
