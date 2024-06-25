#include <stdint.h>
#include <UserCEvents.h>
#include <UserCAPI.h>
#include "vartab.h"
#include "objtab.h"
#include "ctrl_top_adrive.h"
#include "ctrl_top_adrive_feedback.h"
#include "control.h"
#include "io.h"
#include "gsemcm.h"
#include "param.h"
#include "RCText.h"
#include "gse_msgbox.h"

void AdriveFeedback_Move_Init(tAsymerticDriveFeedback * AsymFb, uint8_t MCM_Device_Engine, uint8_t MCM_Output_Engine, uint8_t MCM_Device_Direction, uint8_t MCM_Output_Direction) {
  AsymFb -> EngineMCM = MCM_Device_Engine;
  AsymFb -> EngineOut = MCM_Output_Engine;
  AsymFb -> DirectionMCM = MCM_Device_Direction;
  AsymFb -> DirectionOut = MCM_Output_Direction;
}

void AdriveFeedback_Init(tAsymerticDriveFeedback * AsymFb, uint8_t AnalogInputMCM, uint8_t AnalogInputOut) {
  AsymFb -> AnalogInputMCM = AnalogInputMCM;
  AsymFb -> AnalogInputOut = AnalogInputOut;
}

float k = 0.3;  // коэффициент фильтрации, 0.0-1.0
int32_t AsymFilter(int32_t newVal) {
  static float filVal = 0;
  filVal += (newVal - filVal) * k;
  return filVal;
}

int32_t getPosAsym(tAsymerticDriveFeedback * AsymFb) {
  return AsymFilter(GSeMCM_GetAnalogIn(AsymFb -> AnalogInputMCM, AsymFb -> AnalogInputOut));
}

int32_t getAmperageAsymEngine(tAsymerticDriveFeedback * AsymFb) {
  return GSeMCM_GetAnalogIn(AsymFb -> EngineMCM, AsymFb -> EngineOut + 4);
}

void CheckAsymRange(int8_t * value, int32_t min, int32_t max) {
  if ( * value < min) {
    * value = min;
  } else if ( * value > max) {
    * value = max;
  }
}

void Draw_AdriveFeedback(tAsymerticDriveFeedback * AsymFb) {
  tControl * Ctrl = CtrlGet();
  if (AsymFb->AsymMode == MODE_WORK) {
    SetVarIndexed(IDX_SPREADINGSECTOR, AsymFb -> AsymSetPos); // Установка
    SetVarIndexed(IDX_SPREADINGSECTOR_ACT, AsymFb -> AsymActPos); // Реальное положение
  } else {
    SetVarIndexed(IDX_SPREADINGSECTOR, 0); // Установка
    SetVarIndexed(IDX_SPREADINGSECTOR_ACT, 0); // Реальное положение
  }

  if (2 == Ctrl -> top.WinterTop.AsymCalibrStatus) {
    Ctrl -> top.WinterTop.AsymCalibrStatus = 0;
    MsgBoxOk(RCTextGetText(RCTEXT_T_WARNING, GetVarIndexed(IDX_SYS_LANGUAGE)), RCTextGetText(RCTEXT_T_ADRV_CALBR_ERR, GetVarIndexed(IDX_SYS_LANGUAGE)), NULL, NULL);
  } else if (ON == Ctrl -> top.WinterTop.AsymCalibrStatus) {
    Ctrl -> top.WinterTop.AsymCalibrStatus = 0;
    MsgBoxOk(RCTextGetText(RCTEXT_T_WARNING, GetVarIndexed(IDX_SYS_LANGUAGE)), RCTextGetText(RCTEXT_T_ADRV_CALBR_OK, GetVarIndexed(IDX_SYS_LANGUAGE)), NULL, NULL);
  }
}

static uint8_t flagOn = 0;
static uint8_t flagOff = 0;
void AdriveFeedback_on(tAsymerticDriveFeedback * AsymFb, uint8_t setPos) {
  if(!flagOn) {
    AsymFb->AsymMode = MODE_WORK;
    AsymFb->AsymSetPos = setPos;
    flagOff = 0;
    flagOn = 1;
  }
}

void AdriveFeedback_off(tAsymerticDriveFeedback * AsymFb, uint8_t setPos) {
  static uint8_t asymPosOff[4] = {0,1,3,5};
  if(setPos == 0) {
    AsymFb->AsymSetPos = AsymFb->AsymActPos;
  } else {
    AsymFb->AsymSetPos = asymPosOff[setPos];
  }

  if(AsymFb->AsymActPos == AsymFb->AsymSetPos) {
    if(!flagOff) {
      AsymFb->AsymMode = MODE_INI;
      flagOff = 1;
    }
  }
  flagOn = 0;
}

void AdriveFeedback_Calibration(tAsymerticDriveFeedback * AsymFb) {
  tControl * Ctrl = CtrlGet();
  tParamData * Param = ParameterGet();

  static uint8_t stepCalibr = POS_RIGHT;
  static uint32_t tmrCheckAmperage = 0;
  if (Ctrl -> top.WinterTop.SpreadConv.on) {
    switch (stepCalibr) {
    case POS_RIGHT:
      // Движение вправо (шток втягивается)
      GSeMCM_SetDigitalOut(AsymFb -> EngineMCM, AsymFb -> EngineOut, 1);
      GSeMCM_SetDigitalOut(AsymFb -> DirectionMCM, AsymFb -> DirectionOut, 0);
      tmrCheckAmperage++;
      if (tmrCheckAmperage >= 150) {
        if (getAmperageAsymEngine( & Ctrl -> top.WinterTop.AdriveFB) < Param -> AsymDriveFeedback.I_rightStop) {
          AsymFb -> AsymPosRightAmperage = getPosAsym( & Ctrl -> top.WinterTop.AdriveFB);
          Param -> AsymDriveFeedback.RightAmperage = AsymFb -> AsymPosRightAmperage;
          tmrCheckAmperage = 0;
          stepCalibr = POS_LEFT;
        }
      }
      break;
    case POS_LEFT:
      // Движение влево (шток вытягивается)
      GSeMCM_SetDigitalOut(AsymFb -> EngineMCM, AsymFb -> EngineOut, 1);
      GSeMCM_SetDigitalOut(AsymFb -> DirectionMCM, AsymFb -> DirectionOut, 1);

      tmrCheckAmperage++;
      if (tmrCheckAmperage >= 150) {
        if (getAmperageAsymEngine( & Ctrl -> top.WinterTop.AdriveFB) < Param -> AsymDriveFeedback.I_leftStop) {
          AsymFb -> AsymPosLeftAmperage = getPosAsym( & Ctrl -> top.WinterTop.AdriveFB);
          Param -> AsymDriveFeedback.LeftAmperage = AsymFb -> AsymPosLeftAmperage;
          writeParameters();
          tmrCheckAmperage = 0;
          stepCalibr = POS_RIGHT;
          AsymFb -> AsymMode = MODE_WORK;
          Ctrl->top.WinterTop.AsymCalibrOn = 0;
          Ctrl->top.WinterTop.AsymCalibrStatus = 1;
        }
      }
    default:
      break;
    }
  } else {
    Ctrl->top.WinterTop.AsymCalibrStatus = 2;
    Ctrl->top.WinterTop.AsymCalibrOn = 0;
    stepCalibr = POS_RIGHT;
    AsymFb -> AsymMode = MODE_INI;
  }
}

void Ctrl_AdriveFeedback_Cycle(tAsymerticDriveFeedback * AsymFb) {
  tControl * Ctrl = CtrlGet();
  const tParamData * Param = ParameterGet();

  switch (Ctrl -> cmd_no_key.Z_Change) {
  case 1:
    AsymFb -> AsymSetPos = AsymFb -> AsymSetPos - 1;
    CheckAsymRange( & AsymFb -> AsymSetPos, 1, 5);
    break;
  case -1:
    AsymFb -> AsymSetPos = AsymFb -> AsymSetPos + 1;
    CheckAsymRange( & AsymFb -> AsymSetPos, 1, 5);
    break;
  default:
    break;
  }

  static uint16_t asymPosAmperage[6] = {0};
  uint16_t asymStepAmperage = (Param -> AsymDriveFeedback.LeftAmperage - Param -> AsymDriveFeedback.RightAmperage) / 4;
  for (uint8_t i = 0; i < GS_ARRAYELEMENTS(asymPosAmperage); i++) {
    if (i == POS_STOP) {
      asymPosAmperage[0] = 0;
    } else if (i == POS_RIGHT) {
      asymPosAmperage[5] = Param -> AsymDriveFeedback.RightAmperage;
    } else if (i == POS_LEFT) {
      asymPosAmperage[1] = Param -> AsymDriveFeedback.LeftAmperage;
    } else {
      asymPosAmperage[i] = asymPosAmperage[i - 1] - asymStepAmperage;
    }
  }

  static int16_t asymDifPosAmperage = 0;
  asymDifPosAmperage = asymPosAmperage[AsymFb -> AsymSetPos] - getPosAsym( & Ctrl -> top.WinterTop.AdriveFB);
  uint16_t AsymAccuracy = Param -> AsymDriveFeedback.AsymAccuracy;

  for (uint8_t i = 1; i < GS_ARRAYELEMENTS(asymPosAmperage); i++) {
    static int16_t asymCheckActPos = 0;
    asymCheckActPos = asymPosAmperage[i] - getPosAsym( & Ctrl -> top.WinterTop.AdriveFB);
    if (fabs(asymCheckActPos) < AsymAccuracy) {
      AsymFb -> AsymActPos = i;
    }
  }

  switch (AsymFb -> AsymMode) {
  case MODE_INI:
    break;
  case MODE_WORK:
    if (asymDifPosAmperage > 0 && fabs(asymDifPosAmperage) > AsymAccuracy) {
      GSeMCM_SetDigitalOut(AsymFb -> EngineMCM, AsymFb -> EngineOut, 1);
      GSeMCM_SetDigitalOut(AsymFb -> DirectionMCM, AsymFb -> DirectionOut, 1);
    } else if (asymDifPosAmperage < 0 && fabs(asymDifPosAmperage) > AsymAccuracy) {
      GSeMCM_SetDigitalOut(AsymFb -> EngineMCM, AsymFb -> EngineOut, 1);
      GSeMCM_SetDigitalOut(AsymFb -> DirectionMCM, AsymFb -> DirectionOut, 0);
    } else if (fabs(asymDifPosAmperage) <= AsymAccuracy) {
      GSeMCM_SetDigitalOut(AsymFb -> EngineMCM, AsymFb -> EngineOut, 0);
      GSeMCM_SetDigitalOut(AsymFb -> DirectionMCM, AsymFb -> DirectionOut, 0);
    }
    break;
  case MODE_CALIBR:
    AdriveFeedback_Calibration( & Ctrl -> top.WinterTop.AdriveFB);
    break;
  default:
    break;
  }

  if (Ctrl->top.WinterTop.AsymCalibrOn) {
    AsymFb -> AsymMode = MODE_CALIBR;
  }

  Draw_AdriveFeedback( & Ctrl -> top.WinterTop.AdriveFB);
}
