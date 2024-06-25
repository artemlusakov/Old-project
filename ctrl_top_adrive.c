#include <stdint.h>
#include <UserCEvents.h>
#include <UserCAPI.h>
#include "vartab.h"
#include "objtab.h"
#include "ctrl_top_adrive.h"
#include "gsemcm.h"
#include "control.h"
#include "param.h"
#include "control.h"
#include "io.h"
#include "RCText.h"
#include "gse_msgbox.h"

#define ADRIVE_MOVE_STOP 0
#define ADRIVE_MOVE_LEFT 1
#define ADRIVE_MOVE_RIGHT 2
#define ADRV_TM_SET 120
#define ADRV_TM_CHK 140 // время на проверку упора

#define ADRIVE_LEFT_DEF - 1800 // msec
#define ADRIVE_RIGHT_DEF 1800 // msec
#define ADRIVE_TM_FULL (ADRIVE_RIGHT_DEF - ADRIVE_LEFT_DEF)
#define TIME_MOVE_CONTROL 90 //msec
#define TIME_RUN 1500
#define ADRIVE_POS_LEFT 1
#define ADRIVE_POS_NEUTRAL 3
#define ADRIVE_POS_RIGHT 5
#define ADRIVE_STARTENGINE_TIME 200
#define LIMIT_VOLTAGE 260
#define ADRV_TM_SLEEP_MOVE 200
#define MCM250_4 3


static int32_t flagFirstEnable = 2;
static uint8_t flagStart = 0;
static int8_t flagAdriveStep = 0;
static int I_STOP = 0;
stateCalibration_t stClb = CALIBRATION_START;
stateClb_t stateClb = CLB_PRE_RIGHT;

void adrv_set_out(const tAsymerticDrive * ad_,
  const int8_t stt_) {
  GSeMCM_SetDigitalOut(ad_ -> eng_dev, ad_ -> eng_out, (ADRIVE_MOVE_STOP == stt_ ? 0 : 1));
  GSeMCM_SetDigitalOut(ad_ -> pol_dev, ad_ -> pol_out, (ADRIVE_MOVE_RIGHT == stt_ ? 1 : 0));
}

int32_t getCurentAsymerticDrive(const tAsymerticDrive * ad) {
  return GSeMCM_GetAnalogIn(ad -> eng_dev, ad -> eng_out + 4);
}

void InsertionSort(int l_, int a_[]) {
  int elm_, idx_;

  for (int i_ = 1; i_ < l_; ++i_) {
    elm_ = a_[i_];
    idx_ = i_ - 1;
    while (idx_ >= 0 && a_[idx_] > elm_) {
      a_[idx_ + 1] = a_[idx_];
      --idx_;
    }
    a_[idx_ + 1] = elm_;
  }
}

/**	@brief ADrive_Init
 * @param ad
 * @param MCM_Device_Engine
 * @param MCM_Output_Engine
 * @param MCM_Device_Direction
 * @param MCM_Output_Direction
 * @return		**/
void adrv_init(tAsymerticDrive * ad_, uint8_t MCM_Device_Engine, uint8_t MCM_Output_Engine, uint8_t MCM_Device_Direction, uint8_t MCM_Output_Direction) {
  ad_ -> eng_dev = MCM_Device_Engine;
  ad_ -> eng_out = MCM_Output_Engine;
  ad_ -> pol_dev = MCM_Device_Direction;
  ad_ -> pol_out = MCM_Output_Direction;
  ad_ -> on = 0;

  ad_ -> tm_pos_act = ad_ -> tm_pos = 0;
  ad_ -> idx_pos_act = ad_ -> idx_pos = 0;

  SetVar(HDL_ADRV_POS, ad_ -> tm_pos);
  SetVar(HDL_ADRV_TM_POS_ACT, ad_ -> tm_pos_act);

}

/**
 * @brief  Движение привода в сторону  dir_, если  dir_== STOP то остановка
 * @param I_left  Ток упора влево
 * @param  dir_ Направление движения привода
 * @param *ad указатель на структуру асимметрии
 */
void adrv_move(tAsymerticDrive * ad_, uint8_t dir_) {
  if (ADRIVE_MOVE_STOP == dir_) {
    adrv_set_out(ad_, ADRIVE_MOVE_STOP);
  } else if (ADRIVE_MOVE_LEFT == dir_) {
    adrv_set_out(ad_, ADRIVE_MOVE_LEFT);
  } else if (ADRIVE_MOVE_RIGHT == dir_) {
    adrv_set_out(ad_, ADRIVE_MOVE_RIGHT);
  }
}

/**
 * @brief  Инициализация тока упора
 *
 * @param I_left  Ток упора влево
 * @param I_right Ток упора вправо
 * @param *ad указатель на структуру асимметрии

 */
void ADrive_ICtrl_Init(tAsymerticDrive * ad, int32_t I_left, int32_t I_right) {

  ad -> I_leftStop = I_left;
  ad -> I_rightStop = I_right;
}

/**
 * @brief  Инициализация режима работы  крайних положений по току
 *
 * @param cntrlMove  1- Режим контроля крайних положений по току,
 *                   0 - режим контроля по времени и ток не учитывается
 *  @param *ad указатель на структуру асимметрии
 */
void ADrive_MoveCntrl_Init(tAsymerticDrive * ad, uint8_t cntrlMove) {
  ad -> cntrlMove = cntrlMove;
}

/**
 * @brief  Инициализация времен для работы привода асимметрии
 *
 * @param timeLeft - Время движения из крайнего правого в крайнее левое положение
 * @param timeRight - Время движения из крайнего левого в крайнее правое положение
 * @param timeStopMove - Время сколько мс должно быть постоянное превышение по току, чтоб детектировать, что есть упор
 * @param timeStopMoveCalibration - Время сколько мс должно быть постоянное превышение по току, чтоб детектировать, что есть упор режим калибровки
 * @param timeCalibrationAside - Максимательное время сколько мс может двигаться в одну сторону в режиме калибровки
 * @param  timeErrorStop - Время сколько мс есть, то того, как сработает остановка без детектирования крайнего положения.
 * @param  timeReverse  - Сколько мс пройдет прежде чем программа выйдет из переключения реле для реверса
 * @param  timeReaction - Сколько есть миллисекунд до запуска движения привода после изменения позиции,
 * каждый раз при нажатии курка джойстика  отсчитывается время от 0 до TimeReaction.
 * Как только время становится равным  TimeReaction происходит запуск привода
 *
 */
void ADrive_TimeCtrl_Init(tAsymerticDrive * ad, uint32_t timeLeft, uint32_t timeRight,
  uint32_t timeStopMove, uint32_t timeStopMoveCalibration,
  uint32_t timeCalibrationAside, uint32_t timeErrorStop,
  uint32_t timeReverse, uint32_t timeReaction) {
  float L = -20 * ceil((float) timeLeft / 40.);
  float R = 20 * ceil((float) timeRight / 40.);
  ad -> timeLeft = (int32_t) L;
  ad -> timeRight = (int32_t) R;
  ad -> timeCntrlMoveStop = timeStopMove;
  ad -> timeCntrlMoveCalibrationStop = timeStopMoveCalibration;
  ad -> timeCalibrationAside = timeCalibrationAside;
  ad -> timeCntrlErrorStop = timeErrorStop;
  ad -> timeReversEnable = timeReverse;
  ad -> timeReaction = timeReaction;
}

/**
 * @brief Куда необходимо увести привод асимметрии после выключения/включения по джостику.
 *
 *  @param  pos - в какую позицию переместить привод
 *  @param *ad указатель на структуру асимметрии
 */
void asymerticDrivePosition(tAsymerticDrive * ad, int32_t pos) {
  ADrivePos position_ = pos;
  uint8_t pst = 0;
  switch (position_) {
  case NOW_POSITION:
    pst = ad -> idx_pos;
    break;

  case LEFT_POSITION:
    pst = ADRIVE_POS_LEFT;
    break;

  case NEUTRAL_POSITION:
    pst = ADRIVE_POS_NEUTRAL;
    break;

  case RIGHT_POSITION:
    pst = ADRIVE_POS_RIGHT;
    break;

  default:
    pst = ad -> idx_pos;
    break;
  }
  if (ad -> idx_pos != ad -> idx_pos_act) {
    ad -> idx_pos = ad -> idx_pos_act;
  }
  if (pst > ad -> idx_pos) {

    ad -> idx_step_move = pst - ad -> idx_pos;
    ad -> idx_pos = ad -> idx_pos + ad -> idx_step_move;
  } else if (pst < ad -> idx_pos) {

    ad -> idx_step_move = (ad -> idx_pos - pst) * (-1);
    ad -> idx_pos = ad -> idx_pos + ad -> idx_step_move;
  } else {
    ad -> idx_step_move = 0;
  }

  AdriveStep(ad);
  flagAdriveStep = 1;
}

/**
 * @brief При самом первом включении привода и пульта не известна его стартовая позиция.
 * Эта функция нужна, чтоб увести его до упора и определить стартовую позицию. Если была произведена калибровка
 * до включения привода, эта функция не будет работать.
 *
 *  @param *ad указатель на структуру асимметрии
 *  @param  pos В какую сторону двигать привод для определения крайнего положения влево или вправо
 */
void AdriveStartPos(tAsymerticDrive * ad, int8_t wayMove) {
  int STATE = 0;
  if (wayMove == ADRIVE_MOVE_LEFT) {
    STATE = aDriveMoveColibration(ad, ADRIVE_MOVE_LEFT, 100, ad -> I_leftStop);
    switch (STATE) {
    case ERR:
      ad -> tm_pos = ad -> tm_pos_act = ad -> timeLeft;
      ad -> idx_pos = ad -> idx_pos_act = ADRIVE_POS_LEFT;
      asymerticDrivePosition(ad, 2);
      flagStart = 1;
      flagFirstEnable = 1;
      break;
    case MOVE:
      break;
    case OK:
      ad -> tm_pos = ad -> tm_pos_act = ad -> timeLeft;
      ad -> idx_pos = ad -> idx_pos_act = ADRIVE_POS_LEFT;
      asymerticDrivePosition(ad, 2);
      flagStart = 1;
      flagFirstEnable = 1;
      break;
    }
  } else if (wayMove == ADRIVE_MOVE_RIGHT) {
    STATE = aDriveMoveColibration(ad, ADRIVE_MOVE_RIGHT, 100, ad -> I_rightStop);
    switch (STATE) {
    case ERR:
      ad -> tm_pos = ad -> tm_pos_act = ad -> timeRight;
      ad -> idx_pos = ad -> idx_pos_act = ADRIVE_POS_RIGHT;
      asymerticDrivePosition(ad, 2);
      flagStart = 1;
      flagFirstEnable = 1;
      break;
    case MOVE:
      break;
    case OK:
      ad -> tm_pos = ad -> tm_pos_act = ad -> timeRight;
      ad -> idx_pos = ad -> idx_pos_act = ADRIVE_POS_RIGHT;
      asymerticDrivePosition(ad, 2);
      flagStart = 1;
      flagFirstEnable = 1;
      break;
    }
  }
}

/**
 * @brief  Функция отвечает за движения привода в режиме калибровки, вызывается внутри  aDriveMover.
 * Алгоритм работы функции заключается в том, что когда есть превышение по току, функция ждет 120 мс, после
 * чего начинает смотреть превышение по току в течение времени timeControl. Если за время  ad->timeCalibrationAside
 * определит, что было превышение по току в течении  timeControl. То вернет, что да верно крайнее положение.
 *
 *  @param *ad указатель на структуру асимметрии
 *  @param wayMove - в какую сторону переместить привод
 *  @param timeControl - время в течение которого должно быть превышение по току
 *  @param I_stop - ток  при котором считаем, что достигли упора
 *  @return Статусы движения привода. MOVE - движется. ERR - произошла ошибка. OK - Крайнее положение детектировано.
 */
int aDriveMoveColibration(tAsymerticDrive * ad, int8_t wayMove, uint32_t timeControl, int I_stop) {

  static uint32_t timeWorkMax = 0;
  static uint32_t timeStartCurrent = 0;
  static uint32_t timeMove = 0;
  static uint32_t timeStopper = 0;
  static uint32_t flMax_mA = 0;
  int32_t mA;
  static stateMove_t stateMove = START_MOVE;
  adrv_set_out(ad, wayMove);

  static uint8_t flagTime = 0;
  if (flagTime == 0) {
    timeWorkMax = GetMSTick() + ad -> timeCalibrationAside;
    timeStartCurrent = GetMSTick() + ADRV_TM_SET;
    flagTime = 1;
    stateMove = START_MOVE;
  }
  if (timeWorkMax > GetMSTick()) {
    mA = getCurentAsymerticDrive(ad);
    if (mA > I_stop) {
      switch (stateMove) {
      case START_MOVE:
        timeMove = GetMSTick();
        stateMove = CONTINUE_MOVE;

      case CONTINUE_MOVE:
        if (timeStartCurrent < GetMSTick()) {
          stateMove = STOP_MOVE;
        }
        break;

      case STOP_MOVE:
        if (flMax_mA == 0) {
          timeStopper = GetMSTick();
          flMax_mA = 1;
        }
        if (GetMSTick() - timeStopper > timeControl) {
          ad -> tm_mv_stop = GetMSTick() - timeMove - timeControl;
          adrv_set_out(ad, ADRIVE_MOVE_STOP);
          timeWorkMax = 0;
          timeStartCurrent = 0;
          timeMove = 0;
          timeStopper = 0;
          flMax_mA = 0;
          flagTime = 0;
          stateMove = START_MOVE;
          return OK;
        }
        break;
      }
    } else {
      flMax_mA = 0;
    }
  } else if (timeWorkMax < GetMSTick()) {

    adrv_set_out(ad, ADRIVE_MOVE_STOP);
    timeWorkMax = 0;
    timeStartCurrent = 0;
    timeMove = 0;
    timeStopper = 0;
    flMax_mA = 0;
    flagTime = 0;
    stateMove = START_MOVE;
    return ERR;
  }

  return MOVE;

}

/**
 * @brief  Функция отвечает за движения привода в режиме калибровки. Принимает решение продолжать ли дальше работу калибровки или нет,
 * после возврата состояния из  aDriveMoveColibration().
 * Вызывается в 10 ms Timer.
 *
 *  @param *ad указатель на структуру асимметрии
 *  @param wayMove - в какую сторону переместить привод
 *  @param timeControl - время в течение которого должно быть превышение по току
 *  @param I_stop - ток  при котором считаем, что достигли упора
 *   @param state -  Переход к следующиму состоянию через автомат состояний.
 *   stateClb - глобальная переменная состояний автомата  при калибровке.
 *
 */
void aDriveMover(tAsymerticDrive * ad, int8_t wayMove, uint32_t timeControl, int I_stop, stateClb_t state) {
  int STATE = aDriveMoveColibration(ad, wayMove, timeControl, I_stop);

  switch (STATE) {
  case ERR:
    stateClb = CLB_ERROR;
    adrv_move(ad, ADRIVE_MOVE_STOP);
    sleep(1);
    break;
  case MOVE:
    break;
  case OK:
    stateClb = state;
    adrv_move(ad, ADRIVE_MOVE_STOP);
    sleep(1);

    break;
  }
}

/**
 * @brief Калибровка привода асимметрии. Калибровка происходит следующим способом. Есть автомат состояний
 *   switch (stateClb){} который вызывает функцию aDriveMover(), отвечающию за движение влево или вправо и переключает состояния
 *   после движения привода. Если детектирование крайних положений прошло успешно в функции  aDriveMoveColibration().
 *   То автомат switch (stateClb) переключится на следующее состояние и продолжит калибровку. Иначе он выдаст калибровка неуспешна.
 *   Так же калибровка неуспешно может произойти по нескольким причинам: Машина не заведена или напряжение ниже 26В. Включен пескач.
 *   Скорость больше 0. А так же, если при калибровке время получилось меньше в одну сторону 1000 мс.
 *  @param *ad указатель на структуру асимметрии
 */
void asymerticDriveCalibration(tAsymerticDrive * ad) {
  static uint8_t count = 0;
  tControl * ctrl_ = CtrlGet();
  tParamData * parameter = GetParameter();
  static uint16_t timeRight = 0;
  static uint16_t timeLeft = 0;
  float R = 0;
  float L = 0;
  static int fl = 0;
  if (fl == 0) {
    fl = 1;
  }
  if ((ctrl_ -> top.WinterTop.SpreadConv.on == 0) && (TopGetSpeed() == 0) && ((GSeMCM_GetVoltage(MCM250_4) > LIMIT_VOLTAGE) || (ctrl_ -> engine.rpm > 0))) {

    switch (stateClb) {
    case CLB_PRE_RIGHT:
      if (count == 2) {
        //stateClb переключается внутри  aDriveMover на CLB_LEFT, если ошибка то на CLB_ERROR
        aDriveMover(ad, ADRIVE_MOVE_RIGHT, ad -> timeCntrlMoveCalibrationStop, ad -> I_rightStop, CLB_LEFT);
        if (stateClb == CLB_LEFT) {
          count = 0;
        }
      } else {
        //stateClb переключается внутри  aDriveMover на CLB_PRE_LEFT, если ошибка то на CLB_ERROR
        aDriveMover(ad, ADRIVE_MOVE_RIGHT, ad -> timeCntrlMoveCalibrationStop, ad -> I_rightStop, CLB_PRE_LEFT);
      }
      break;

    case CLB_PRE_LEFT:
      //stateClb переключается внутри  aDriveMover на CLB_PRE_RIGHT, если ошибка то на CLB_ERROR
      aDriveMover(ad, ADRIVE_MOVE_LEFT, ad -> timeCntrlMoveCalibrationStop, ad -> I_leftStop, CLB_PRE_RIGHT);
      if (stateClb == CLB_PRE_RIGHT) {
        count++;
      }
      break;

    case CLB_LEFT:
      //stateClb переключается внутри  aDriveMover на CLB_RIGHT, если ошибка то на CLB_ERROR
      aDriveMover(ad, ADRIVE_MOVE_LEFT, ad -> timeCntrlMoveCalibrationStop, ad -> I_leftStop, CLB_RIGHT);
      timeLeft = ad -> tm_mv_stop;
      break;

    case CLB_RIGHT:
      //stateClb переключается внутри  aDriveMover наCLB_STOP, если ошибка то на CLB_ERROR
      aDriveMover(ad, ADRIVE_MOVE_RIGHT, ad -> timeCntrlMoveCalibrationStop, ad -> I_rightStop, CLB_STOP);
      timeRight = ad -> tm_mv_stop;
      break;

    case CLB_ERROR:
      adrv_move(ad, ADRIVE_MOVE_STOP);
      ctrl_->top.WinterTop.AsymCalibrStatus = 2;
      ctrl_->top.WinterTop.AsymCalibrOn = 0;
      count = 0;
      fl = 0;
      stateClb = CLB_PRE_RIGHT; // вернуть автомат в изначальное состояние
      break;

    case CLB_STOP:

      timeRight = (timeRight / 2);
      timeLeft = (timeLeft / 2);
      R = (20 * ceil((float) timeRight / 20.)) - 100;
      L = (20 * ceil((float) timeLeft / 20.)) - 100;
      if ((L < 1000) || (R < 1000)) {
        stateClb = CLB_ERROR;
        return;

      }
      parameter -> AsymDrive.timeFullRight = ad -> timeRight = (uint32_t) R * 2;
      parameter -> AsymDrive.timeFullLeft = ad -> timeLeft = (uint32_t) L * 2;
      ad -> timeLeft /= (-2);
      ad -> timeRight /= 2;
      writeParameters();
      ad -> tm_pos_act = ad -> tm_pos = ad -> tm_pos_act = ad -> timeRight;
      ad -> idx_pos = ad -> idx_pos_act = 5;
      ctrl_->top.WinterTop.AsymCalibrOn = 0;
      count = 0;
      ctrl_->top.WinterTop.AsymCalibrStatus = ON;
      stateClb = CLB_PRE_RIGHT; // вернуть автомат в изначальное состояние
      fl = 0;
      flagStart = 1;
      break;
    }
  } else {
    ctrl_->top.WinterTop.AsymCalibrStatus = 2;
    ctrl_->top.WinterTop.AsymCalibrOn = 0;
    count = 0;
    fl = 0;
    stateClb = CLB_PRE_RIGHT; // вернуть автомат в изначальное состояние
  }
}

/**
 * @brief Активирование контрвключения и выключение автомата для движения приводом.
 *  @param *ad указатель на структуру асимметрии
 *  @param stateMove указатель состояния автомата
 */
void ControlDawn(tAsymerticDrive * ad, move_t * stateMove) {
  static int flag = 0;
  static uint64_t timer = 0;
  if (!flag) {
    timer = GetMSTick();
    flag = 1;
  }

  GSeMCM_SetDigitalOut(ad -> pol_dev, ad -> pol_out, ad -> dir_old == ADRIVE_MOVE_RIGHT ? 0 : 1);
  if (GetMSTick() - timer > ad -> timeReversEnable) {
    GSeMCM_SetDigitalOut(ad -> pol_dev, ad -> pol_out, ad -> dir_old == ADRIVE_MOVE_RIGHT ? 1 : 0);
    flagAdriveStep = 0; // выключение движения привода
    flag = 0;
    timer = 0;
    * stateMove = MOVE_START;
  }
}

/**
 * @brief Включение привода асимметрии. Если это первое включение после обесточивания будет отработан,  AdriveStartPos()
 * при условии, что не было калибровки, иначе отработает  asymerticDrivePosition().
 * Каждый раз после выключения пескача, когда производится включение привод асимметрии уводится в центр через 10 ms Timer
 *
 *  @param *ad указатель на структуру асимметрии
 *  @param  pos В какую  позицию двигать привод при первом включении после обесточения для AdriveStartPos()
 */
void adrv_on(tAsymerticDrive * ad, int32_t pos) {
  if (!ad -> on) {
    ad -> on = 1;

    if ((!flagFirstEnable) && (flagStart)) {
      asymerticDrivePosition(ad, 2);
      flagFirstEnable = 1;
    }
  }
  if (!flagStart) {
    AdriveStartPos(ad, ADRIVE_MOVE_RIGHT);
  }
}

/**
 * @brief Выключение привода асимметрии. Куда переместить его согласно выбору из меню.
 *  @param *ad указатель на структуру асимметрии
 *  @param  pos В какую  позицию двигать привод после выключения пескача
 */
void adrv_off(tAsymerticDrive * ad_, int32_t pos_) { //(ПЕРЕДЕЛАННАЯ ФУНКЦИЯ)
  if (ad_ -> on) {
    ad_ -> on = 0;
    if (flagFirstEnable == 1) {
      asymerticDrivePosition(ad_, pos_);
      flagFirstEnable = 0;
    }
  }
}

/**
 * @brief Опрос кнопки джойстика для выбора положения в какую позицию переместить привод асимметрии.
 * Запуск движения производится, как истечет время timeReaction
 * Пока время не истекло задается положение в которое, необходимо перевести привод
 *  Работает только, когда привод не движется и пескач включен
 *  @param *ad указатель на структуру асимметрии
 *  @param  move  сместить на одну позицию влево или вправо
 */
void AdriveCommand(tAsymerticDrive * ad, int32_t move) {

  static uint32_t tim = 0;
  static int8_t count = 0;
  static int flagMove = 0;

  if ((ad -> on) && (!flagAdriveStep)) {

    switch (move) {
    case 1:
      if (ad -> idx_pos != ADRIVE_POS_LEFT) {
        ad -> idx_pos--;
        count--;
        tim = GetMSTick();
        flagMove = 1;
      }
      break;
    case -1:
      if (ad -> idx_pos != ADRIVE_POS_RIGHT) {
        ad -> idx_pos++;
        count++;
        tim = GetMSTick();
        flagMove = 1;
      }
      break;

    case 0:
      if ((GetMSTick() - tim >= ad -> timeReaction) && (flagMove == 1)) {
        ad -> idx_step_move = count;
        AdriveStep(ad);
        count = 0;
        tim = GetMSTick();
        flagMove = 0;
        flagAdriveStep = 1; // запуск привода
      }
      break;
    }
  }
}

/**
 * @brief Функция, где задается на сколько позиций необходимо сместить привод асимметрии влево или вправо.
 * @param *ad указатель на структуру асимметрии
 */
void AdriveStep(tAsymerticDrive * ad) {

  switch (ad -> idx_step_move) {
  case LEFT_ONE_MOVE:
    ad -> tm_pos = ad -> tm_pos + ad -> timeLeft / 2;
    break;
  case LEFT_TWO_MOVE:
    ad -> tm_pos = ad -> tm_pos + ad -> timeLeft;
    break;
  case LEFT_THREE_MOVE:
    ad -> tm_pos = ad -> tm_pos + ad -> timeLeft / 2 + ad -> timeLeft;
    break;
  case LEFT_FOUR_MOVE:
    ad -> tm_pos = ad -> tm_pos + 2 * ad -> timeLeft;
    break;
  case NO_MOVE:
    flagAdriveStep = 0;
    break;
  case RIGHT_ONE_MOVE:
    ad -> tm_pos = ad -> tm_pos + ad -> timeRight / 2;
    break;
  case RIGHT_TWO_MOVE:
    ad -> tm_pos = ad -> tm_pos + ad -> timeRight;
    break;
  case RIGHT_THREE_MOVE:
    ad -> tm_pos = ad -> tm_pos + ad -> timeRight / 2 + ad -> timeRight;
    break;
  case RIGHT_FOUR_MOVE:
    ad -> tm_pos = ad -> tm_pos + 2 * ad -> timeRight;
    break;

  }
  if (ad -> tm_pos < ad -> timeLeft) {
    ad -> tm_pos = ad -> timeLeft;
  }
  if (ad -> tm_pos > ad -> timeRight) {
    ad -> tm_pos = ad -> timeRight;
  }
}

/**
 * @brief Функция, где начинается движение влево или вправо .
 * @param *ad указатель на структуру асимметрии
 */
void AdriveStepMove(tAsymerticDrive * ad) {

  switch (ad -> idx_step_move) {
  case LEFT_ONE_MOVE:

    AdriveMove(ad, ADRIVE_MOVE_LEFT);
    break;
  case LEFT_TWO_MOVE:

    AdriveMove(ad, ADRIVE_MOVE_LEFT);
    break;
  case LEFT_THREE_MOVE:

    AdriveMove(ad, ADRIVE_MOVE_LEFT);
    break;
  case LEFT_FOUR_MOVE:

    AdriveMove(ad, ADRIVE_MOVE_LEFT);
    break;
  case NO_MOVE:
    flagAdriveStep = 0;
    break;
  case RIGHT_ONE_MOVE:

    AdriveMove(ad, ADRIVE_MOVE_RIGHT);
    break;
  case RIGHT_TWO_MOVE:

    AdriveMove(ad, ADRIVE_MOVE_RIGHT);
    break;
  case RIGHT_THREE_MOVE:

    AdriveMove(ad, ADRIVE_MOVE_RIGHT);
    break;
  case RIGHT_FOUR_MOVE:

    AdriveMove(ad, ADRIVE_MOVE_RIGHT);
    break;
  }
}

/**
 * @brief Автомат движения привода ассиметрии. У него есть  состояния MOVE_START, MOVE_STOP,MOVE_DAWN,MOVE_STOP_CONTROL.
 * Если движение привода указывается не в крайнию левую или правую позицию. То автомат отработает MOVE_START-> MOVE_STOP -> MOVE_DAWN.
 * и выключится после достижения заданной позиции то есть это переход ко всем позициям кроме крайних. MOVE_DAWN отвечает за контрвключение.
 * Если же переход происходит к крайней позиции, то автомат отработает следующим образом MOVE_START-> MOVE_STOP_CONTROL и выключится после контроля тока
 * или по времени ошибки. При условии, что активирован режим контроля крайних положений по току.
 * @param *ad указатель на структуру асимметрии
 * @param wayMove направление движения, куда двигаться
 */
void AdriveMove(tAsymerticDrive * ad, int8_t wayMove) {
  static move_t stateMove = MOVE_START;
  static uint32_t timer = 0;
  static uint32_t timerPause = 0;
  uint8_t flagEnd = 0;
  static uint8_t flagTime = 0;
  static uint8_t flTim = 0;
  if (flagTime == 0) {
    timer = GetMSTick();
    flagTime = 1;
  }
  if (stateMove != MOVE_STOP) {
    flTim = 0;
    timerPause = 0;
  }

  switch (stateMove) {
  case MOVE_START:
    adrv_move(ad, wayMove);
    if (GetMSTick() - timer > 20) {
      timer = GetMSTick();
      // если включен режим контроля крайнего положения и переход происходит к крайнему положению, то переходим к контролю по току
      // иначе  контроль только по времени
      if (((ad -> idx_pos == ADRIVE_POS_RIGHT) || (ad -> idx_pos == ADRIVE_POS_LEFT)) && (ad -> cntrlMove == 1)) {
        stateMove = MOVE_STOP_CONTROL;
        flagTime = 0;
      } else {
        stateMove = MOVE_STOP;
        flagTime = 0;
      }
    }

    break;

  case MOVE_STOP:
    if (flTim == 0) {
      timerPause = GetMSTick();
      flTim = 1;
    }

    if ((wayMove == ADRIVE_MOVE_LEFT) && (ad -> tm_pos_act > ad -> tm_pos)) {
      ad -> tm_pos_act = ad -> tm_pos_act - 10;
      I_STOP = ad -> I_leftStop;
      ad -> dir_old = ADRIVE_MOVE_LEFT;
    } else if ((wayMove == ADRIVE_MOVE_RIGHT) && (ad -> tm_pos_act < ad -> tm_pos)) {
      ad -> tm_pos_act = ad -> tm_pos_act + 10;
      I_STOP = ad -> I_rightStop;
      ad -> dir_old = ADRIVE_MOVE_RIGHT;
    }

    if (ad -> tm_pos_act == ad -> tm_pos) {
      adrv_move(ad, ADRIVE_MOVE_STOP);
      stateMove = MOVE_DAWN;
      timerPause = GetMSTick();
      flTim = 0;
    }

    if (GetMSTick() - timerPause > ADRV_TM_SLEEP_MOVE) {
      ADriveICntrl(ad, I_STOP, wayMove, & stateMove);
    }
    break;

  case MOVE_DAWN:
    ControlDawn(ad, & stateMove);
    break;

  case MOVE_STOP_CONTROL:
    //функция контроля крайнего положения
    if (wayMove == ADRIVE_MOVE_LEFT) {
      if (ad -> tm_pos_act > ad -> tm_pos) {
        ad -> tm_pos_act = ad -> tm_pos_act - 10;
      }
      AdriveStopControlMove(ad, ad -> timeLeft, & flagEnd, wayMove, ad -> I_leftStop);
    } else if (wayMove == ADRIVE_MOVE_RIGHT) {
      if (ad -> tm_pos_act < ad -> tm_pos) {
        ad -> tm_pos_act = ad -> tm_pos_act + 10;
      }
      AdriveStopControlMove(ad, ad -> timeRight, & flagEnd, wayMove, ad -> I_rightStop);
    }
    if (flagEnd == 1) {
      stateMove = MOVE_START;
      flagEnd = 0;
      flagAdriveStep = 0;
    }
    break;
  }
}

/**
 *
 * @brief Автомат движения привода ассиметрии. При крайних положениях. У автомата есть 2 состояния CURRENT_CAPTURE и  CURRENT_CHECK.
 * Расчитывается приблизительное время движения привода в 1042 или 1047 строках. В зависимости от направления.
 * После чего начинается контроль по току, как только превышение будет
 * в течение времени заданного в timeStopMove. То привод остановится и скажет,
 * что он достиг крайнего положения с контролем по току.
 * Если же нет превышения по току, то привод все равно остановится
 * через время расчитанное в 1042 или 1047 + timeErrorStop.
 * Это защита в случае не срабатывания контроля по току крайних положений.
 * @param *ad указатель на структуру асимметрии
 * @param pos значение, какое необходимо установить при завершении функции, когда flagEnd = 1
 * @param flagEnd - признак, что функция контроля по току завершила исполнение
 * @param wayMove направление движения, куда двигаться
 * @param I_stop  ток при котором, считаем, что достигли упора
 *
 */
void AdriveStopControlMove(tAsymerticDrive * ad, int32_t pos, uint8_t * flagEnd, uint8_t wayMove, int I_stop) {
  static uint32_t timeMoveMax = 0;
  static uint32_t timeControl = 0;
  static uint8_t flagCntrlTime = 0;
  static uint8_t flagContTime = 0;
  int32_t mA;
  static eCurrent_t state = CURRENT_CAPTURE;
  mA = getCurentAsymerticDrive(ad);
  if ((wayMove == ADRIVE_MOVE_LEFT) && (flagCntrlTime == 0)) {
    timeControl = GetMSTick() + (ad -> timeLeft / 2 * ad -> idx_step_move) + ad -> timeCntrlErrorStop;
    flagCntrlTime = 1;
  } else if ((wayMove == ADRIVE_MOVE_RIGHT) && (flagCntrlTime == 0)) {
    timeControl = GetMSTick() + (ad -> timeRight / 2 * ad -> idx_step_move) + ad -> timeCntrlErrorStop;
    flagCntrlTime = 1;
  }
  // Контроль по току
  if (mA > I_stop) {
    switch (state) {

    case CURRENT_CAPTURE:

      if ((wayMove == ADRIVE_MOVE_LEFT) && (flagContTime == 0)) {
        timeMoveMax = GetMSTick() + (ad -> timeLeft / 2 * ad -> idx_step_move) + ad -> timeCntrlMoveStop;
        flagContTime = 1;
      } else if ((wayMove == ADRIVE_MOVE_RIGHT) && (flagContTime == 0)) {
        timeMoveMax = GetMSTick() + (ad -> timeRight / 2 * ad -> idx_step_move) + ad -> timeCntrlMoveStop;
        flagContTime = 1;
      }
      if (GetMSTick() - timeControl > 150) {
        state = CURRENT_CHECK;
      }
      break;

    case CURRENT_CHECK:
      if (timeMoveMax <= GetMSTick()) {

        adrv_move(ad, ADRIVE_MOVE_STOP);
        timeControl = 0;
        * flagEnd = 1;
        flagCntrlTime = 0;
        flagContTime = 0;
        state = CURRENT_CAPTURE;
        ad -> tm_pos_act = ad -> tm_pos = pos;
        return;
      }
      break;

    }
  } else {
    state = CURRENT_CAPTURE;
    timeMoveMax = 0;
  }
  // Защита, если не сработает контроль по току
  if (timeControl <= GetMSTick()) {
    adrv_move(ad, ADRIVE_MOVE_STOP);
    * flagEnd = 1;
    timeControl = 0;
    flagCntrlTime = 0;
    flagContTime = 0;
    ad -> tm_pos_act = ad -> tm_pos = pos;
    state = CURRENT_CAPTURE;

  }
}

void ADriveICntrl(tAsymerticDrive * ad_, int32_t I_stop, uint8_t wayMove, move_t * stateMove) {
  int16_t mA_ = GSeMCM_GetAnalogIn(ad_ -> eng_dev, ad_ -> eng_out + 4);
  static uint32_t timer = 0;
  static uint8_t flag = 0;
  if (mA_ > I_stop) {
    if (!flag) {
      timer = GetMSTick();
      flag = 1;
    }
    if (GetMSTick() - timer > ADRV_TM_CHK) {
      adrv_move(ad_, ADRIVE_MOVE_STOP);
      flag = 0;
      timer = 0;
      flagAdriveStep = 0;
      * stateMove = MOVE_START;
      if (wayMove == ADRIVE_MOVE_RIGHT) {
        ad_ -> tm_pos = ad_ -> tm_pos_act = ad_ -> timeRight;
        ad_ -> idx_pos = ad_ -> idx_pos_act = 5;
      } else if (wayMove == ADRIVE_MOVE_LEFT) {
        ad_ -> tm_pos = ad_ -> tm_pos_act = ad_ -> timeLeft;
        ad_ -> idx_pos = ad_ -> idx_pos_act = 1;
      }
    }
  } else {
    flag = 0;
    timer = 0;
  }
}

/**
 * @brief Функция старта калибровки привода асимметрии .
 * @param *ad указатель на структуру асимметрии
 */
void AdriveStartCallibration(tAsymerticDrive * ad) {
  tControl * ctrl_ = CtrlGet();
  if (ctrl_->top.WinterTop.AsymCalibrOn) {
    asymerticDriveCalibration(ad);

  }
}

/**
 * @brief Функция, где происходит движении привода асимметрии AdriveStepMove() при его обычной работе.
 * Где вызывается старт калибровки привода асимметрии AdriveStartCallibration().
 * А так же расчет, где сейчас находится физически привод при движении.
 * @param *ad указатель на структуру асимметрии
 */
void ADrive_Timer_10ms(tAsymerticDrive * ad) {
  static int32_t count = 0;
  int dpos = 0;

  AdriveStartCallibration(ad);

  if (ad -> on) {
    if (flagAdriveStep == 1) {
      AdriveStepMove(ad);
      count++;
      dpos = (ad -> timeRight - ad -> timeLeft) / 4 / 2;
      ad -> idx_pos_act = 1 + (ad -> tm_pos_act - ad -> timeLeft + dpos) * 4 / (ad -> timeRight - ad -> timeLeft);
    }

  } else if (!ad -> on) {
    if (flagAdriveStep == 1) {
      AdriveStepMove(ad);
      dpos = (ad -> timeRight - ad -> timeLeft) / 4 / 2;
      ad -> idx_pos_act = 1 + (ad -> tm_pos_act - ad -> timeLeft + dpos) * 4 / (ad -> timeRight - ad -> timeLeft);
    }

  }
}

/**
 * @brief Функция отображения графики для привода асимметрии.
 * @param *ad указатель на структуру асимметрии
 */
void ADrive_Draw(const tAsymerticDrive * ad_) {
  //Spreading Sector,
  tControl * ctrl_ = CtrlGet();
  static int tm = 0;

  if (ad_ -> on) {
    if (getFlagUnloading()) {
      SetVarIndexed(IDX_SPREADINGSECTOR, 0);
      SetVarIndexed(IDX_SPREADINGSECTOR_ACT, 0);
    } else {
      SetVarIndexed(IDX_SPREADINGSECTOR, ad_ -> idx_pos); // зелёное
      SetVarIndexed(IDX_SPREADINGSECTOR_ACT, ad_ -> idx_pos_act);
    }
    // жёлтое
  } else { //no colour, if  conveyor off.
    if (ad_ -> idx_pos != ad_ -> idx_pos_act) {
      SetVarIndexed(IDX_SPREADINGSECTOR, ad_ -> idx_pos);
      SetVarIndexed(IDX_SPREADINGSECTOR_ACT, ad_ -> idx_pos_act);
      tm = GetMSTick();
    } else {
      SetVarIndexed(IDX_SPREADINGSECTOR, ad_ -> idx_pos);
      SetVarIndexed(IDX_SPREADINGSECTOR_ACT, ad_ -> idx_pos_act);
      if (GetMSTick() - tm > 1000) {
        SetVarIndexed(IDX_SPREADINGSECTOR, 0);
        SetVarIndexed(IDX_SPREADINGSECTOR_ACT, 0);
      }

    }

  }
  SetVar(HDL_ADRV_POS, ad_ -> tm_pos);
  SetVar(HDL_ADRV_TM_POS_ACT, ad_ -> tm_pos_act);
  SetVar(HDL_ADRV_INDX_POS, ad_ -> idx_pos);

  if (2 == ctrl_ -> top.WinterTop.AsymCalibrStatus) {
    ctrl_ -> top.WinterTop.AsymCalibrStatus = 0;
    MsgBoxOk(RCTextGetText(RCTEXT_T_WARNING, GetVarIndexed(IDX_SYS_LANGUAGE)), RCTextGetText(RCTEXT_T_ADRV_CALBR_ERR, GetVarIndexed(IDX_SYS_LANGUAGE)), NULL, NULL);
  } else if (ON == ctrl_ -> top.WinterTop.AsymCalibrStatus) {
    ctrl_ -> top.WinterTop.AsymCalibrStatus = 0;
    MsgBoxOk(RCTextGetText(RCTEXT_T_WARNING, GetVarIndexed(IDX_SYS_LANGUAGE)), RCTextGetText(RCTEXT_T_ADRV_CALBR_OK, GetVarIndexed(IDX_SYS_LANGUAGE)), NULL, NULL);
  }
}
