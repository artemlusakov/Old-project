#ifndef CTRL_MANIPULATOR_H
#define CTRL_MANIPULATOR_H

typedef struct tagManipulatorTM  {
uint32_t TM;
uint8_t Active;
uint8_t Set;
uint8_t Icon;
} tManipulatorTM;

void Ctrl_Manipulator_FMM600_Init( tControl* Ctrl );
void Ctrl_Manipulator_Standard_Cycle( tControl* Ctrl, uint32_t evtc, tUserCEvt* evtv );

#endif  // #ifndef CTRL_MANIPULATOR_H
