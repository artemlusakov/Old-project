#ifndef OKB600_H
#define OKB600_H

typedef enum {
  MODE_INIT = 0,
  MODE_VERTICAL_COPY = 1,
  MODE_SECURITY_ERROR = 2,
  MODE_HORIZONTAL_COPY = 3,
} OKB600ModeWrk;

typedef struct tagOKB600Valve {
  tValv MainValve;
  int32_t MainValveValue;

	tValv fSectionUp;
	int32_t fSectionUpValue;

	tValv fSectionDown;
	int32_t fSectionDownValue;

	tValv sSectionUp;
	int32_t sSectionUpValue;

	tValv sSectionDown;
	int32_t sSectionDownValue;

	tValv ValveTurnRight;
	int32_t ValveTurnRightValue;

	tValv ValveTurnLeft;
	int32_t ValveTurnLeftValue;

	tValv ValveZoom;
	int32_t ValveZoomValue;

	tValv ValveDecrease;
	int32_t ValveDecreaseValue;

	tValv ValveTiltClockwise;
	int32_t ValveTiltClockwiseValue;

	tValv ValveTiltCounterClockwise;
	int32_t ValveTiltCounterClockwiseValue;

	tValv ValveDischarge;
	int32_t ValveDischargeValue;

	tValv ValveCrawl;
	int32_t ValveCrawlValue;

	tValv ValvePTO;
	int32_t ValvePTOValue;

  uint32_t tmrOKB600;
} tOKB600Valve;

void Ctrl_OKB600_Init(tOKB600Valve * Valves);
void Ctrl_OKB600_Cycle(tOKB600Valve * Valves, tJSCommands * Cmd);
void Ctrl_OKB600_Timer_10ms(tOKB600Valve * Valves);
void Ctrl_MCM7n8_Check(void);

#endif  // #ifndef OKB600_H
