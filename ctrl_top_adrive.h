#ifndef CTRL_TOP_ADRIVE_H
#define CTRL_TOP_ADRIVE_H

#include	<stdint.h>

typedef struct tagAsymerticDrive {
  uint32_t on;
  uint32_t tm_swoff;
  int32_t tm_pos;
  int32_t tm_pos_act;
  int8_t eng_dev;
  int8_t eng_out;
  int8_t pol_dev;
  int8_t pol_out;
  int8_t dir_old;
  int8_t posJost;
  uint8_t cntrlMove;
  uint8_t f_move; // +++
  uint16_t max_I; ///< maximum current in mA
  uint32_t timeReversEnable;
  uint32_t timeReaction;
  int32_t I_leftStop;
  int32_t I_rightStop;
  int32_t timeRight;
  int32_t timeLeft;
  uint32_t timeCalibrationAside;
  uint32_t timeCntrlMoveCalibrationStop;
  uint32_t timeCntrlMoveStop;
  uint32_t timeCntrlErrorStop;
  uint32_t tm1_max_I;
  uint32_t tm2_max_I; // --- (добавлено)
  uint32_t tm_mv_stop; // +++
  uint32_t f_tm_mv_stop; // +++
  uint8_t f_maxL; ///< В крайнем левом положении
  uint8_t f_maxR; ///< В крайнем правом положении
  uint8_t f_calibr; ///< включение калибровки 1 - вкл; 0 - выкл
  int8_t idx_pos_act;
  int8_t idx_pos;
  int8_t idx_step_move;
} tAsymerticDrive;

typedef enum {
  NOW_POSITION = 0,
  LEFT_POSITION = 1,
  NEUTRAL_POSITION = 2,
  RIGHT_POSITION = 3
}
ADrivePos;

typedef enum {
  START_MOVE = 0,
  FIRST_MOVE = 1,
  CONTINUE_MOVE = 2,
  STOP_MOVE = 3
}
stateMove_t;

typedef enum {
  MOVE_START = 0,
  MOVE_STOP = 1,
  MOVE_STOP_CONTROL = 2,
  MOVE_DAWN = 3
}
move_t;

typedef enum {
  CURRENT_CAPTURE = 0,
  CURRENT_CHECK = 1
}
eCurrent_t;

typedef enum {
  LEFT_ONE_MOVE = -1,
  LEFT_TWO_MOVE = -2,
  LEFT_THREE_MOVE = -3,
  LEFT_FOUR_MOVE = -4,
  NO_MOVE = 0,
  RIGHT_ONE_MOVE = 1,
  RIGHT_TWO_MOVE = 2,
  RIGHT_THREE_MOVE = 3,
  RIGHT_FOUR_MOVE = 4
}
StepMove_t;

typedef enum {
  CALIBRATION_START = 0,
  CALIBRATION_CNTRL = 1,
  CALIBRATION_END = 2
}
stateCalibration_t;

typedef enum {
  CLB_PRE_LEFT = 0,
  CLB_PRE_RIGHT = 1,
  CLB_LEFT = 2,
  CLB_RIGHT = 3,
  CLB_STOP = 4,
  CLB_ERROR = 5
}
stateClb_t;

/**
 * @brief Initalises the asymmetrc drive Sets one output to drive the engine, and one to change the directoin of the engine
 *
 * @param ad
 * @param MCM_Device_Engine MCM-Device of the Engine output
 * @param MCM_Output_Engine Output for the Engine
 * @param MCM_Device_Direction MCM-Device for directoin output
 * @param MCM_Output_Direction Output to change directoin of the engine
 */
void adrv_init(tAsymerticDrive * ad, uint8_t MCM_Device_Engine, uint8_t MCM_Output_Engine, uint8_t MCM_Device_Direction, uint8_t MCM_Output_Direction);

/**
 * @brief Sets the maximum output current of the engine. If the current is higher then mA_Max
 * for more then 500 ms, the endpostion of the drive will be detected.
 *
 * @param ad
 * @param mA_Max maximum current in mA
 */
void ADrive_ICtrl_Init(tAsymerticDrive * ad, int32_t I_left, int32_t I_right);

/**
 * @brief Call this Function every 10 ms.
 *
 * @param ad
 */
void ADrive_Timer_10ms(tAsymerticDrive * ad);

/**
 * @brief switches the asymmetry drive on. It will go to the center position.
 *
 * @param ad
 */
void adrv_on(tAsymerticDrive * ad, int32_t pos);

/**
 * @brief switches the asymmetry drive off. It will go to the left.
 *
 * @param ad
 */
void adrv_off(tAsymerticDrive * ad, int32_t Position);

void ADrive_Draw(const tAsymerticDrive * ad);
void ADrive_MoveCntrl_Init(tAsymerticDrive * ad, uint8_t cntrlMove);
void ADrive_TimeCtrl_Init(tAsymerticDrive * ad, uint32_t timeLeft, uint32_t timeRight,
  uint32_t timeStopMove, uint32_t timeStopMoveCalibration,
  uint32_t timeCalibrationAside, uint32_t timeErrorStop,
  uint32_t timeReverse, uint32_t timeReaction);
void AdriveCommand(tAsymerticDrive * ad, int32_t move);
void AdriveStep(tAsymerticDrive * ad);
int aDriveMoveColibration(tAsymerticDrive * ad, int8_t wayMove, uint32_t timeControl, int I_stop);
void AdriveMove(tAsymerticDrive * ad, int8_t wayMove);
void ADriveICntrl(tAsymerticDrive * ad_, int32_t I_stop, uint8_t wayMove, move_t * stateMove);
void AdriveStopControlMove(tAsymerticDrive * ad, int32_t pos, uint8_t * flagEnd, uint8_t wayMove, int I_stop);
#endif // #ifndef CTRL_TOP_ADRIVE_H