#ifndef CTRL_ABC_ATTACH_H
#define CTRL_ABC_ATTACH_H
#include "gsemaint_runtime.h"
#include "hydvalvectrl.h"

typedef struct tagABCAttach_ {
	tMaintRuntime* MaintRunntime;
	tMaintRuntime* MaintRunntime_Reset;
	tMaintRuntime*		Runtime_Ploughs;
	tMaintRuntime*		Runtime_Ploughs_Reset;
	tValv ValveUpDown;
	tValv ValveUpDownSecondary;
	tValv ValveFloat;
	tValv ValveRotation;
	tValv ValveLeftRight;
	tValv ValveExRetract;
	tValv ValveSteelUpDown;
	tValv ValveTilt;
	tValv ValveTiltSecondary;
	tValv ValveWorkTransport;
	tValv ValveSecondSection;
	tValv ValveLock;
	tValv ValveTiltLock;

  //FMM600
  tValv Motor;
  tValv Turn;
  tValv Divertor;// in task steelruber

	// OKB600
	tValv OKB600MainValve;
	tValv fSectionUp;
	tValv fSectionDown;
	tValv sSectionUp;
	tValv sSectionDown;
	tValv ValveTurnRight;
	tValv ValveTurnLeft;
	tValv ValveZoom;
	tValv ValveDecrease;
	tValv ValveTiltClockwise;
	tValv ValveTiltCounterClockwise;
	tValv ValveDischarge;
	tValv ValveCrawl;
	tValv ValvePTO;

	uint32_t timer_cnt;
	uint32_t MaxSpeed;

} tABC_Attach;

#endif // #ifndef CTRL_ABC_ATTACH_H
