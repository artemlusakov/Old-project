#ifndef CTRL_TOP_ADRIVE_FEEDBACK_H
#define CTRL_TOP_ADRIVE_FEEDBACK_H

#include "ctrl_top_adrive.h"

typedef struct tagAsymerticDriveFeedback {
  uint8_t EngineMCM;
  uint8_t EngineOut;
  uint8_t DirectionMCM;
  uint8_t DirectionOut;
  uint8_t AnalogInputMCM;
  uint8_t AnalogInputOut;

  uint8_t AsymMode;

  uint8_t AsymSetPos;
  uint8_t AsymActPos;
  uint16_t AsymPosLeftAmperage;
  uint16_t AsymPosRightAmperage;
} tAsymerticDriveFeedback;

typedef enum {
  POS_STOP      = 0,
  POS_RIGHT     = 5,
  POS_PRE_RIGHT	= 4,
  POS_CENTER    = 3,
  POS_PRE_LEFT	= 2,
  POS_LEFT      = 1,
} AsymPos;

typedef enum {
  MODE_INI    = 0,
  MODE_WORK   = 1,
  MODE_CALIBR	= 2,
} AsymMode;

void AdriveFeedback_Move_Init(tAsymerticDriveFeedback * AsymFb, uint8_t MCM_Device_Engine, uint8_t MCM_Output_Engine, uint8_t MCM_Device_Direction, uint8_t MCM_Output_Direction );
void AdriveFeedback_Init(tAsymerticDriveFeedback * AsymFb, uint8_t AnalogInputMCM, uint8_t AnalogInputOut );
void AdriveFeedback_on(tAsymerticDriveFeedback * AsymFb, uint8_t setPos);
void AdriveFeedback_off(tAsymerticDriveFeedback * AsymFb, uint8_t setPos);
void Ctrl_AdriveFeedback_Cycle(tAsymerticDriveFeedback * AsymFb);

#endif  // #ifndef CTRL_TOP_ADRIVE_FEEDBACK_H
