#ifndef CTRL_BRUSH_H
#define CTRL_BRUSH_H


#include "gsemaint_runtime.h"
#include "hydvalvectrl.h"
#include "io_types.h"
#include "errorlist.h"
#include "commands.h"
#include "ctrl_abc_attach.h"

typedef enum eagPVE_PORT {
	PVE_PORT_A = -1,
	PVE_PORT_B = 1,
	PVE_PORT_NONE = 0,
} ePVE_PORT;

typedef struct tagBrushValve {
	tMaintRuntime* MaintRunntime;
	tMaintRuntime* MaintRunntime_Reset;

	tValv VUpDown;
  int32_t VUpDownValue;

	tValv VUpDownSecondary;
	int32_t VUpDownSecondaryValue;

	tValv VLeftRight;
	int32_t VLeftRightValue;

	tValv VLock;
	int32_t VLockValue;

	tValv VFloat;
	int32_t VFloatValue;

	tValv VRotation;
	int32_t VRotationValue;
	uint16_t VRotatePWM;

	tValv VOptional;
	int32_t VOptionalValue;

  tValv VSideSec;
	int32_t VSideSecValue;

  uint32_t MaxSpeed;
  int8_t BrushInvert;

  int8_t isFloat;
  uint32_t tmrBrush;
} tBrushValve;

void Brush_Maint_Create( tBrushValve* BrushValve, tMaint* Maint, eErrorID ID, uint32_t RCText_Warn, uint32_t RCText_Service, uint32_t HdlRemVar_Warn, uint32_t Hdl_RemVar_Service );
void Brush_Maint_Create_Reset( tBrushValve* BrushValve, tMaint* Maint, eErrorID ID, uint32_t RCText_Warn, uint32_t RCText_Service, uint32_t HdlRemVar_Warn, uint32_t Hdl_RemVar_Service );

void Brush_UpDown_Init( tBrushValve* BrushValve, uint32_t Device1, uint32_t Pos1, uint32_t Device2, uint32_t Pos2, uint32_t ValveType );
void Equip_Down_OMFB_Init( tBrushValve* BrushValve, uint32_t Device1, uint32_t Pos1, uint32_t Device2, uint32_t Pos2, uint32_t ValveType );

void Brush_Float_Init( tBrushValve* BrushValve, int32_t Device1, int32_t Pos1 );
void Brush_Rotate_Init( tBrushValve* BrushValve, int32_t Device1, int32_t Pos1, int32_t ValveType );
void Brush_LeftRight_Init( tBrushValve* BrushValve, uint32_t Device1, uint32_t Pos1, uint32_t Device2, uint32_t Pos2, uint32_t ValveType );
void Brush_Lock_Init( tBrushValve* BrushValve, uint32_t Device1, uint32_t Pos1 );
void Brush_Optional_Init( tBrushValve* BrushValve, uint32_t Device1, uint32_t Pos1, uint32_t Device2, uint32_t Pos2 );
void Brush_VSideSec_Init( tBrushValve* BrushValve, uint32_t Device1, uint32_t Pos1, uint32_t Device2, uint32_t Pos2 );
void Brush_A_B( tBrushValve* BrushValve, int32_t Val );
void Brush_Timer_10ms( tBrushValve* BrushValve );
void Brush_Cycle( tBrushValve* BrushValve, tJSCommands* Cmd, uint8_t Equip );
int32_t Brush_Active( const tBrushValve* BrushValve );
void Brush_Timer_100ms( tBrushValve* BrushValve, tJSCommands* Cmd );
int8_t IsBrushAutoUp(void);
#endif // #ifndef CTRL_BRUSH_H
