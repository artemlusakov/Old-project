#include <stdint.h>
#include <UserCEvents.h>
#include <UserCAPI.h>
#include "vartab.h"
#include "objtab.h"
#include "rtr_uh.h"
#include "RCText.h"
#include "gsemcm.h"
#include "io.h"
#include "gse_msgbox.h"
#include "errorlist.h"
#include "gsToVisu.h"
#include "visu_material.h"
#include "ctrl_top_conveyor.h"
#include "param.h"
#include "profile.h"
#include "pinout.h"
#include "control.h"
#include "RCColor.h"
#include "drawfunction.h"
#include "calibr_omfb.h"

void HumidPump_init(tHumidPump * pump_, uint8_t stt_, uint8_t type_);
void HumidPump_set(tHumidPump * pump_, uint16_t val_);
void HumidPump_cycle(tHumidPump * pump_);
void HumidPump_timer(tHumidPump * pump_);
uint16_t HumidPump_state(const tHumidPump * pump_);
int uniqa_init(void);
int uniqa_cmd(void);
int uniqa_ctrl(void);

const float kf_r2r_humid = 1.25; ///< Пересчёт скорости вала
const float kf_p2p_humid = 1.25; ///< Пересчёт ШИМ

#define FN_CONV_TBL "/gs/data/uh_conv_tbl.cfg"
#define FN_CONV_TBL_USB "/gs/usb/uh_conv_tbl.cfg"

static int8_t wr_file(const char * fn_);
static int8_t rd_file(const char * fn_);

SUniqa_t uh;

static uint32_t _keyDump_ = 0;

unsigned uh_humid_len = 0;
float uh_humid_vol[UH_LEN_TBL_MAX];
float uh_humid_rot[UH_LEN_TBL_MAX];
float uh_humid_pwm[UH_LEN_TBL_MAX];
unsigned uh_conv_len = 0;
float uh_conv_rot[UH_LEN_TBL_MAX];
float uh_conv_vol[UH_LEN_TBL_MAX];
float uh_conv_pwm_up[UH_LEN_TBL_MAX];
float uh_conv_pwm_down[UH_LEN_TBL_MAX];
static int a_speed[10]; ///< Массив накопленных скоростей
static int cur_speed = 0; ///< Текущее положение в массиве
static int num_speed = 5;
static uint32_t tm_speed = 0;
static int speed_max = 0;
static int speed_mid = 0;


float liner_interpret(float * x_, float * y_, int len_, float x0_) {
  if (x0_ < x_[0]) return 0.;
  for (int i_ = 1; i_ < len_; ++i_) {
    if (x0_ > x_[i_]) continue;
    if (x0_ == x_[i_ - 1]) return y_[i_ - 1];
    return liner_interpret2(x_[i_ - 1], x_[i_], y_[i_ - 1], y_[i_], x0_);
  }
  return y_[len_ - 1];
}

void sw_sens_state(uint32_t hndl_, eDISensorState * sns_) {
  if (0 == GetVar(hndl_)) {
    * sns_ = DI_SENSOR_INACTIVE;
  } else if (DI_SENSOR_INACTIVE == * sns_) {
    * sns_ = DI_SENSOR_UNKNOWN;
  }
}

/** @brief: Расчёт средней и максимальной скорости за последнии %num_speed секунд **/
void uniqa_set_speed(int speed_) {
  if (tm_speed > GetMSTick()) return;
  tm_speed = GetMSTick() + 1000;
  speed_mid = speed_max = 0;
  a_speed[cur_speed++] = speed_;
  if (cur_speed >= num_speed) cur_speed = 0;
  for (int i_ = 0; i_ < num_speed; ++i_) {
    speed_mid += a_speed[i_];
    if (speed_max < a_speed[i_]) speed_max = a_speed[i_];
  }
  speed_mid /= num_speed;
}

/** @brief УНИКА, проверка вращения конвйеера и наличия материала **/
void mat_spread_UH_cycle(void) {
  static uint32_t _tmt_spread_ = 0; ///<
  static int _old_sens_ = -1; ///< старое значене сенсора для отображения.
  int q_sens_; ///< текущее расчётное значение для отображения
  int f_blink_; ///< моргаем если датчики отключены
  //? состояние конвеера по датчикам
  if (OFF == uh.stt.conv) {
    q_sens_ = MAT_SENSOR_NO_ROT;
    f_blink_ = 0;
  } //= не вращается
  else if (ON == uh.stt.conv) { //= вращается
    if (ON == uh.stt.spread_sens) { //= есть материала
      q_sens_ = MAT_SENSOR_OK;
      f_blink_ = 0;
    } else if (OFF == uh.stt.spread_sens) { //= нет материал
      q_sens_ = MAT_SENSOR_EMPTY;
      f_blink_ = 0;
    } else { //= материал не определяется
      q_sens_ = MAT_SENSOR_OK;
      f_blink_ = 1;
    }
  } else if (uh.wr.conv_pwm) { //= конвейер не оперделён, выставлен ШИМ
    f_blink_ = 1;
    if (ON == uh.stt.spread_sens) { //= есть материала
      q_sens_ = MAT_SENSOR_OK;
    } else if (OFF == uh.stt.spread_sens) { //= нет материал
      q_sens_ = MAT_SENSOR_EMPTY;
    } else { //= материал не определяется
      q_sens_ = MAT_SENSOR_OK;
    }
  } else { //= ШИМ сброшен в 0
    q_sens_ = MAT_SENSOR_NO_ROT;
    f_blink_ = 0;
  }
  //*
  if (_old_sens_ != q_sens_) {
    _old_sens_ = q_sens_;
    if (MAT_SENSOR_EMPTY == q_sens_) {
      _tmt_spread_ = GetMSTick() + TMT_SPREAD_SENS;
    } else {
      _tmt_spread_ = 0;
    }
  }
  if (_tmt_spread_ && _tmt_spread_ < GetMSTick() && uh.md != MD_DUMP) {
    MsgBoxOk(RCTextGetText(RCTEXT_T_WARNING, GetVarIndexed(IDX_SYS_LANGUAGE)), RCTextGetText(RCTEXT_T_NOMATERIAL, GetVarIndexed(IDX_SYS_LANGUAGE)), NULL, NULL);
    _tmt_spread_ = 0;
  }
  //*
  SendToVisuObj(OBJ_IND_CONV_SPREAD, GS_TO_VISU_SET_ATTR_BLINK, f_blink_);
  SetVar(HDL_ICON_DENSITY, uh.md == MD_DUMP ? MAT_SENSOR_OK : q_sens_);
}

void HumidPump_init(tHumidPump * pump_, uint8_t stt_, uint8_t type_) {
  uniqa_init();
  pump_ -> stt = stt_;
  pump_ -> type = type_;
  ValveProp_Init( & pump_ -> val, MCM250_4, UH_PIN_HUMID_PUMP, 1000, 10);
  SendToVisuObj(OBJ_WATERPUMP, GS_TO_VISU_SET_ATTR_VISIBLE, 1);
}

void HumidPump_set(tHumidPump * pump_, uint16_t val_) {
  ValvProp_Set( & pump_ -> val, val_);
}

void HumidPump_cycle(tHumidPump * pump_) {
  uniqa_cmd();
  uniqa_ctrl();
}

void HumidPump_timer(tHumidPump * pump_) {
  if (pump_ -> stt) {
    ValvProp_Timer_100ms( & pump_ -> val);
  } else {
    ValvProp_Set( & pump_ -> val, 0);
  }
}

uint16_t HumidPump_state(const tHumidPump * pump_) {
  return pump_ -> stt;
}

int uniqa_init(void) {
  const tParamData * Param = ParameterGet();
  uh.md = MD_INI;
  uh.md_humid = MD_UNDEF;
  uh.md_conv = MD_UNDEF;
  uh.md_tail = MD_UNDEF;
  uh.conv_vol = 0.;
  uh.humid_vol = 0.;
  uh.humid_prcnt = 0;
  uh.stt.humid_sens = UNK;
  uh.stt.humid_pump = UNK;
  uh.stt.spread_sens = UNK;
  uh.stt.conv = UNK;
  uh.stt.tail_pos = UNK;
  uh.stt.adrv_calibr = UNK;
  { //+ Иництализация таблиц увлажнителя
    uh_humid_len = UH_HUMID_ROT_MAX - UH_HUMID_ROT_MIN + 1;
    float stp_pwm_ = (float)(UH_HUMID_PWM_MAX - UH_HUMID_PWM_MIN) / (UH_HUMID_ROT_MAX - UH_HUMID_ROT_MIN);
    float stp_vol_ = (float)(UH_HUMID_VOL_MAX - UH_HUMID_VOL_MIN) / (UH_HUMID_ROT_MAX - UH_HUMID_ROT_MIN);
    for (unsigned r_ = UH_HUMID_ROT_MIN, i_ = 0; r_ <= UH_HUMID_ROT_MAX; ++r_, ++i_) {
      uh_humid_rot[i_] = r_;
      uh_humid_vol[i_] = UH_HUMID_VOL_MIN + i_ * stp_vol_;
      uh_humid_pwm[i_] = ceil(UH_HUMID_PWM_MIN + i_ * stp_pwm_);
    }
  } //.
  //* � нициализация таблиц конвейера

  if (Param -> RTR.TypeRTR == 5) { // UH 5000
    uh_conv_len = UH_CONV_ROT_MAX - UH_CONV_ROT_MIN + 1;
    float stp_pwm_ = (float)(UH_CONV_PWM_MAX - UH_CONV_PWM_MIN) / (UH_CONV_ROT_MAX - UH_CONV_ROT_MIN);
    float stp_vol_ = (float)(UH_CONV_VOL_MAX_5000 - UH_CONV_VOL_MIN_5000) / (UH_CONV_ROT_MAX - UH_CONV_ROT_MIN);
    for (unsigned r_ = UH_CONV_ROT_MIN, i_ = 0; r_ <= UH_CONV_ROT_MAX; ++r_, ++i_) {
      uh_conv_rot[i_] = r_;
      uh_conv_vol[i_] = UH_CONV_VOL_MIN_5000 + i_ * stp_vol_;
      uh_conv_pwm_up[i_] = ceil(UH_CONV_PWM_MIN + i_ * stp_pwm_);
      uh_conv_pwm_down[i_] = ceil(UH_CONV_PWM_MIN + i_ * stp_pwm_);
    }
  } else if (Param -> RTR.TypeRTR == 4) { // UH 3000
    uh_conv_len = UH_CONV_ROT_MAX - UH_CONV_ROT_MIN + 1;
    float stp_pwm_ = (float)(UH_CONV_PWM_MAX - UH_CONV_PWM_MIN) / (UH_CONV_ROT_MAX - UH_CONV_ROT_MIN);
    float stp_vol_ = (float)(UH_CONV_VOL_MAX_3000 - UH_CONV_VOL_MIN_3000) / (UH_CONV_ROT_MAX - UH_CONV_ROT_MIN);
    for (unsigned r_ = UH_CONV_ROT_MIN, i_ = 0; r_ <= UH_CONV_ROT_MAX; ++r_, ++i_) {
      uh_conv_rot[i_] = r_;
      uh_conv_vol[i_] = UH_CONV_VOL_MIN_3000 + i_ * stp_vol_;
      uh_conv_pwm_up[i_] = ceil(UH_CONV_PWM_MIN + i_ * stp_pwm_);
      uh_conv_pwm_down[i_] = ceil(UH_CONV_PWM_MIN + i_ * stp_pwm_);
    }
  }
  return OK;
}

int chk_bound(const int v_,
  const int min_,
    const int max_) {
  if (v_ < min_) return min_;
  if (v_ > max_) return max_;
  return v_;
}

int uniqa_cmd(void) {
  static uint32_t _tm_dump_ = 0;
  tControl * ctrl_ = ctrl_get();
  uniqa_set_speed(TopGetSpeed());
  //? Не ABC режим
  static uint64_t tmr, tmr1 = 0;
  static uint8_t flagTmr, flagTmr1 = 0;
  static int8_t Percent = 0;
  if ((!IOT_Button_IsDown( & ctrl_ -> joystick.Button[JSB_A])) &&
    (!IOT_Button_IsDown( & ctrl_ -> joystick.Button[JSB_B])) &&
    (!IOT_Button_IsDown( & ctrl_ -> joystick.Button[JSB_C]))) {
    if (IOT_Button_IsDown( & ctrl_ -> joystick.Button[JSB_F])) {
      if ((IOT_AnaJoy_GetInPercent( & ctrl_ -> joystick.X) * 10) > 500) {
        flagTmr1 = 0;
        if (!flagTmr) {
          tmr1 = GetMSTick();
          flagTmr = 1;
        }
        if (GetMSTick() - tmr > 250) {
          tmr = GetMSTick();
          Percent = 10;
        } else {
          Percent = 0;
        }
      } else if ((IOT_AnaJoy_GetInPercent( & ctrl_ -> joystick.X) * 10) < -500) {
        flagTmr = 0;
        if (!flagTmr1) {
          tmr = GetMSTick();
          flagTmr1 = 1;
        }
        if (GetMSTick() - tmr1 > 250) {
          tmr1 = GetMSTick();
          Percent = -10;
        } else {
          Percent = 0;
        }
      } else {
        Percent = 0;
      }
      uh.humid_prcnt = uh.humid_prcnt + Percent;
      CheckRange((int32_t * ) & uh.humid_prcnt, 0, 40);
      PaintText_Cycle( & uh.humid_pump.ColourUHPercent, IOT_AnaJoy_GetInPercent( & ctrl_ -> joystick.X));
    }
  } else {
    PaintText_Cycle( & uh.humid_pump.ColourUHPercent, 0);
  }

  //* Проверка на разгрузка
  if (IsKeyReleasedNew(5)) { //= кнопка отпущена
    if (UNK == _keyDump_ && GetMSTick() >= _tm_dump_) {
      _keyDump_ = ON;
    } else {
      _keyDump_ = OFF;
    }
  }
  if (IsKeyPressedNew(5)) {
    if (OFF == _keyDump_) {
      _keyDump_ = UNK;
      _tm_dump_ = GetMSTick() + 2000;
    }
  }

  //* Отображение разгрузки
  if (OFF == _keyDump_) {
    SendToVisuObj(OBJ_IND_DUMP_REAG, GS_TO_VISU_SET_ATTR_BLINK, 0);
    SetVar(HDL_DUMP_REAGENT, 0);
  } else if (UNK == _keyDump_) {
    SendToVisuObj(OBJ_IND_DUMP_REAG, GS_TO_VISU_SET_ATTR_BLINK, 1);
    if (_tm_dump_ < GetMSTick()) {
      SetVar(HDL_DUMP_REAGENT, 1);
      SendToVisuObj(OBJ_IND_DUMP_REAG, GS_TO_VISU_SET_ATTR_BLINK, 0);
    } else {
      SetVar(HDL_DUMP_REAGENT, 0);
    }
  } else if (ON == _keyDump_) {
    SendToVisuObj(OBJ_IND_DUMP_REAG, GS_TO_VISU_SET_ATTR_BLINK, 0);
    SetVar(HDL_DUMP_REAGENT, 1);
  }
  sw_sens_state(HDL_SW_HUMID_SENS, & ctrl_ -> sens.DI.Humidifier);
  sw_sens_state(HDL_SW_SPREAD_SENS, & ctrl_ -> sens.DI.Spreading);
  sw_sens_state(HDL_SW_TAIL_POS, & ctrl_ -> sens.DI.tail_pos);
  return OK;
}

/** **/
int uniqa_ctrl( void ) {
  static uint32_t _pwm_ = 0;

  tControl * ctrl_ = ctrl_get();
  tHumidPump * pump_ = & uh.humid_pump;
  float dens_mat_ = 0.;

  switch (uh.md) {
  case MD_INI: {
    uh.md = MD_WRK;
    ctrl_ -> top.WinterTop.SpreadDensity = 0;
    ctrl_ -> top.WinterTop.SpreadWidth = 2;
    uh.md_humid = MD_UNDEF;
    uh.humid_prcnt = 0;
  }
  break;
  case MD_WRK: {
    if (ON == _keyDump_) {
      if (0 == TopGetSpeed()) {
        uh.md = MD_DUMP;
      } else {
        _keyDump_ = OFF;
      }
    }
  }
  break;
  case MD_DUMP: {
    if (5 < TopGetSpeed()) {
      _keyDump_ = OFF;
    }
    if (OFF == _keyDump_) {
      uh.md = MD_INI;
    }
  }
  break;
  default:
    break;
  }

  uh.stt.conv = 0 == GetVar(HDL_SW_CONV_ROT) ? UNK : (0 == ctrl_ -> sens.Freq.ConveryRotation ? OFF : ON);
  uh.stt.humid_pump = 0 == GetVar(HDL_SW_HUMID_ROT) ? UNK : (0 == ctrl_ -> sens.Freq.humid_pump_rot ? OFF : ON);

  uh.rd.humid_rot = ctrl_ -> sens.Freq.humid_pump_rot;
  uh.rd.conv_rot = ctrl_ -> sens.Freq.ConveryRotation;
  uh.wr.humid_pwm = uh.humid_pump.val.duty;
  uh.wr.conv_pwm = ctrl_ -> top.WinterTop.SpreadConv.pwm;
  //*
  SetVar(HDL_VAR_HUMID_PERCENT, uh.humid_prcnt);
  SetVar(HDL_VAR_CONV_ROT, uh.rd.conv_rot);
  SetVar(HDL_VAR_HUMID_PUMP_ROT, uh.rd.humid_rot);
  SetVar(HDL_VAR_TAIL_POS, ctrl_ -> sens.DI.tail_pos);
  //* состояние насоса увлажнения
  if (ON == uh.stt.humid_pump || (UNK == uh.stt.humid_pump && 0 != uh.wr.humid_pwm)) {
    SetVar(HDL_ICON_PUMP, 1);
  } else {
    SetVar(HDL_ICON_PUMP, 0);
  }
  //* состояние датчика наличия жидкости
  if (UNK == uh.stt.humid_sens || 0 == uh.wr.humid_pwm) {
    SetVar(HDL_WETTINGSENSORSTATE, 0);
  } else if (ON == uh.stt.humid_sens) {
    SetVar(HDL_WETTINGSENSORSTATE, 1);
  } else if (OFF == uh.stt.humid_sens) {
    SetVar(HDL_WETTINGSENSORSTATE, 2);
  }
  //* состояние группы распределения (хвоста)
  if (OFF == uh.stt.tail_pos) {
    if (MD_INI != uh.md_tail) {
      if (MD_CLR == uh.md_tail) {
        uh.md_tail = MD_INI;
      } else {
        uh.md_tail = MD_CLR;
        MsgBoxOk(RCTextGetText(RCTEXT_T_WARNING, GetVarIndexed(IDX_SYS_LANGUAGE)), RCTextGetText(RCTEXT_T_TAILPOS, GetVarIndexed(IDX_SYS_LANGUAGE)), NULL, NULL);
      }
    }
  } else {
    if (MD_WRK != uh.md_tail) {
      if (MD_SET == uh.md_tail) {
        uh.md_tail = MD_WRK;
      } else {
        uh.md_tail = MD_SET;
      }
    }
  }

  //* расчитаем
  static uint32_t _tm_ctrl_;
  if (MD_DUMP == uh.md) {
    uh.md_humid = MD_UNDEF;
    uh.humid_prcnt = 0;
    if (ON == uh.stt.tail_pos) {
      uh.md = MD_INI;
      _keyDump_ = OFF;
      MsgBoxOk(RCTextGetText(RCTEXT_T_WARNING, GetVarIndexed(IDX_SYS_LANGUAGE)), RCTextGetText(RCTEXT_T_TAILPOSDOWN, GetVarIndexed(IDX_SYS_LANGUAGE)), NULL, NULL);
    }
  } else if (MD_UNDEF == uh.md_humid &&
    OFF != uh.stt.tail_pos &&
    0 != uh.humid_prcnt &&
    0 < uh.conv_vol) {
    uh.md_humid = MD_INI;
  }

  switch (uh.md_humid) {
  case MD_INI: {
    uh.md_humid = MD_CHK;
    _tm_ctrl_ = GetMSTick() + TM_HUMID_PUMP_CTRL;
    _pwm_ = 1.1 * uh_humid_pwm[0];
    pump_ -> stt = ON;
  }
  break;
  case MD_CHK: {
    if (_tm_ctrl_ < GetMSTick()) {
      uh.md_humid = MD_CTRL;
    }
  }
  break;
  case MD_CTRL:
  case MD_SET:
  case MD_WRK: {
    if (OFF != uh.stt.humid_sens && 0 != uh.humid_prcnt && 0 < uh.conv_vol) {
      const tReagent * Reagent = GetActualReagent();
      if (Reagent == NULL) {
        dens_mat_ = 1.;
      } else {
        dens_mat_ = Reagent -> MaterialDensity;
      }
      if (fabs(dens_mat_) < 0.001) {
        dens_mat_ = 1.;
      }
      //* Расчёт необходимого объёма жидкости и соответсвующая установка насоса
      float vol_ = uh.conv_vol * dens_mat_ / 1000. * 60. * uh.humid_prcnt / 100.;
      SetVarFloat(HDL_HUMID_VOL, vol_);
      //? объём меньше возможного
      if (vol_ < UH_HUMID_VOL_MIN) { //= надо что-то лить
        vol_ = UH_HUMID_VOL_MIN;
      }
      //? именился объём
      if (vol_ != uh.humid_vol) { //= расчётный и текущий объём не совпадают
        uh.wr.humid_rot = liner_interpret(uh_humid_vol, uh_humid_rot, uh_humid_len, vol_);
        _pwm_ = liner_interpret(uh_humid_vol, uh_humid_pwm, uh_humid_len, vol_);

        uh.md_humid = MD_SET;
        uh.humid_vol = vol_;
        uh.tm_humid = GetMSTick() + TM_HUMID_SET;
        if (_pwm_ < UH_HUMID_PWM_MIN) {
          _pwm_ = UH_HUMID_PWM_MIN;
        } else if (_pwm_ > UH_HUMID_PWM_MAX) {
          _pwm_ = UH_HUMID_PWM_MAX;
        }
      } else if (MD_SET == uh.md_humid && GetMSTick() >= uh.tm_humid) {
        if (uh.wr.humid_rot == uh.rd.humid_rot) { //= обороты нужные
          uh.md_humid = MD_WRK;
          { //+ корректируем таблички
            unsigned idx_ = 0;
            for (unsigned i_ = 0; i_ < uh_humid_len; ++i_) {
              if (uh.rd.humid_rot == uh_humid_rot[i_]) {
                idx_ = i_;
                break;
              }
            }
            uh_humid_pwm[idx_] = uh.wr.humid_pwm;
            for (unsigned i_ = idx_ + 1; i_ < uh_humid_len; ++i_) {
              if (uh_humid_pwm[i_] <= uh_humid_pwm[i_ - 1]) {
                uh_humid_pwm[i_] = uh_humid_pwm[i_ - 1] + 1;
              }
            }
            for (int i_ = idx_ - 1; i_ >= 0; --i_) {
              if (uh_humid_pwm[i_] >= uh_humid_pwm[i_ + 1]) {
                uh_humid_pwm[i_] = uh_humid_pwm[i_ + 1] - 1;
              }
            }
          }
        } else if (OFF == uh.stt.humid_pump) {
          uh.humid_prcnt = 0;
          MsgBoxOk(RCTextGetText(RCTEXT_T_WARNING, GetVarIndexed(IDX_SYS_LANGUAGE)), RCTextGetText(RCTEXT_T_HUMIDPUMPROT, GetVarIndexed(IDX_SYS_LANGUAGE)), NULL, NULL);
        } else if (ON != uh.stt.humid_pump) { //= пофиг на обороты
          uh.md_humid = MD_WRK;
          _pwm_ = liner_interpret(uh_humid_vol, uh_humid_pwm, uh_humid_len, vol_);
          uh.wr.humid_rot = liner_interpret(uh_humid_vol, uh_humid_rot, uh_humid_len, vol_);
        } else { //= пересчёт ШИМ для нужных оборотов
          if (uh.rd.humid_rot == UH_HUMID_ROT_MIN) {
            _pwm_ = uh.wr.humid_pwm + (float)(uh.wr.humid_pwm - UH_HUMID_PWM_MIN) * (uh.wr.humid_rot - uh.rd.humid_rot);
          } else if (uh.wr.humid_pwm == UH_HUMID_PWM_MIN) {
            _pwm_ = uh.wr.humid_pwm + (float)(UH_HUMID_PWM_MAX - UH_HUMID_PWM_MIN) / (float)(UH_HUMID_ROT_MAX - UH_HUMID_ROT_MIN) * (uh.wr.humid_rot - uh.rd.humid_rot);
          } else {
            _pwm_ = uh.wr.humid_pwm + (float)(uh.wr.humid_pwm - UH_HUMID_PWM_MIN) / fabs((float)(uh.rd.humid_rot - UH_HUMID_ROT_MIN)) * (uh.wr.humid_rot - uh.rd.humid_rot);
          }
          if (_pwm_ < UH_HUMID_PWM_MIN) {
            _pwm_ = UH_HUMID_PWM_MIN;
          } else if (_pwm_ > UH_HUMID_PWM_MAX) {
            _pwm_ = UH_HUMID_PWM_MAX;
          }
          uh.tm_humid = GetMSTick() + TM_HUMID_SET;
        }
      }
      if (MD_WRK == uh.md_humid) {
        if (ON == uh.stt.humid_pump && uh.wr.humid_rot != uh.rd.humid_rot) {
          uh.md_humid = MD_WRK;
          uh.tm_humid = GetMSTick() + TM_HUMID_SET;
        }
      }
    } else {
      uh.md_humid = MD_UNDEF;
      if (OFF == uh.stt.humid_sens && 0 != uh.humid_prcnt) {
        MsgBoxOk(RCTextGetText(RCTEXT_T_WARNING, GetVarIndexed(IDX_SYS_LANGUAGE)), RCTextGetText(RCTEXT_T_HUMIDIFIER, GetVarIndexed(IDX_SYS_LANGUAGE)), NULL, NULL);
        uh.humid_prcnt = 0;
      }
      _pwm_ = 0;
      uh.wr.humid_rot = 0;
    }
  }
  break;
  case MD_UNDEF: {
    pump_ -> stt = OFF;
    _pwm_ = 0;
    uh.humid_vol = 0;
    uh.wr.humid_rot = 0;
  }
  break;
  }

  SetVarFloat(HDL_ACT_VOL, _pwm_);
  SetVarFloat(HDL_CONV_VOL, uh.wr.humid_rot);
  ValvProp_Set( & pump_ -> val, _pwm_);
  return OK;
}

void Ctrl_RTR_UH_init(tControl * ctrl_) {
  const tParamData * Param = ParameterGet();
  ctrl_->top.TopType = TOP_WINTER;
  SetRTR_CntUH();

  adrv_init( & ctrl_ -> top.WinterTop.Adrive, MCM250_4, UH_PIN_ASYM_DRV_ON, MCM250_4, UH_PIN_ASYM_DRV_REV);
  ADrive_TimeCtrl_Init( & ctrl_ -> top.WinterTop.Adrive, Param -> AsymDrive.timeFullLeft, Param -> AsymDrive.timeFullRight,
    Param -> AsymDrive.timeCntrlMoveStop, Param -> AsymDrive.timeCntrlMoveCalibrationStop,
    Param -> AsymDrive.timeCalibrationAside, Param -> AsymDrive.timeCntrlErrorStop,
    Param -> AsymDrive.timeReversEnable, Param -> AsymDrive.timeReaction);
  ADrive_ICtrl_Init( & ctrl_ -> top.WinterTop.Adrive, Param -> AsymDrive.I_leftStop, Param -> AsymDrive.I_rightStop);
  ADrive_MoveCntrl_Init( & ctrl_ -> top.WinterTop.Adrive, Param -> AsymDrive.cntrlMove);
  ConveyorInit( & ctrl_ -> top.WinterTop.SpreadConv, MCM250_4, UH_PIN_CONV);
  SpreaderDiskInit( & ctrl_ -> top.WinterTop.SpreadDisk, MCM250_4, UH_PIN_DISK_ROT);
  HumidPump_init( & uh.humid_pump, 0, 1);

  //Main Valve
  int RTR[3] = {
    PinOut_Get_MCM_A(0, 3),
    PinOut_Get_Pin_A(0, 3),
    PinOut_Get_ValveType(0, 3)
  };

  switch (RTR[2]) {
  case Valve_PVEO_1:
  case Valve_RG:
    ValvDisk_Init( & ctrl_ -> top.WinterTop.ValveMain, RTR[0], RTR[1]);
    break;
  case Valve_PVEH_16:
    Valv_Init( & ctrl_ -> top.WinterTop.ValveMain, RTR[0], RTR[1], -1, -1);
    break;
  case Valve_PVEH_32:
    Valv_PVG32_Init( & ctrl_ -> top.WinterTop.ValveMain, RTR[0], RTR[1], -1, -1);
    break;
  case Valve_PVEH_OMFB:
    Valv_OMFB_Init( & ctrl_ -> top.WinterTop.ValveMain, RTR[0], RTR[1], -1, -1);
    break;
  default:
    break;
  }

  //Read
  ctrl_ -> top.WinterTop.SpreadDensity = GetVarIndexed(IDX_DENSITY);
  ctrl_ -> top.WinterTop.SpreadWidth = GetVarIndexed(IDX_SPREADERWIDTH);

  PaintText_Init( & ctrl_ -> top.WinterTop.SpreadConv.ColourDensity, RCCOLOR_NAME_SELECT, RCCOLOR_NAME_FRONT, OBJ_RTR_DENSITY_UH);
  PaintText_Init( & ctrl_ -> top.WinterTop.SpreadDisk.ColourWidth, RCCOLOR_NAME_SELECT, RCCOLOR_NAME_FRONT, OBJ_RTR_WIDTH_UH);
  PaintText_Init( & uh.humid_pump.ColourUHPercent, RCCOLOR_NAME_SELECT, RCCOLOR_NAME_FRONT, OBJ_RTR_WETTING_UH);

  int rc_ = rd_file(FN_CONV_TBL);
  if (0 != rc_) {
    uniqa_init();
  }
}

void Ctrl_RTR_UH_cycle(tControl * ctrl_, uint32_t evtc_, tUserCEvt * evtv_) {
  tWinterTop * wTop = & ctrl_ -> top.WinterTop;
  const tParamData * Param = ParameterGet();
  static int _f_tail_ = 0;
  static int _f_wr_ = 0;
  RTR_Draw(wTop);

  if (ctrl_ -> cmd_no_key.E_S) {
    if (!_f_tail_ && OFF == uh.stt.tail_pos) {
      _f_tail_ = 1;
      MsgBoxOk(RCTextGetText(RCTEXT_T_WARNING, GetVarIndexed(IDX_SYS_LANGUAGE)), RCTextGetText(RCTEXT_T_TAILPOS, GetVarIndexed(IDX_SYS_LANGUAGE)), NULL, NULL);
    }
    Conveyor_Switch( & ctrl_ -> top.WinterTop.SpreadConv, 1);
    if (MD_DUMP == uh.md) {
      SpreaderDisk_Switch( & ctrl_ -> top.WinterTop.SpreadDisk, 0);
      adrv_off( & ctrl_ -> top.WinterTop.Adrive, GetVar(HDL_MODEASSIMETRY));
    } else {
      SpreaderDisk_Switch( & ctrl_ -> top.WinterTop.SpreadDisk, 1);
      adrv_on( & ctrl_ -> top.WinterTop.Adrive, GetVar(HDL_MODEASSIMETRY));
    }

    switch (ctrl_ -> top.WinterTop.ValveMain.Type) {
    case VALVE_TYPE_DISK:
      Valv_Set( & ctrl_ -> top.WinterTop.ValveMain, 1);
      break;
    case VALVE_TYPE_PVE_AHS:
      ValvPropPVEP_SetLiter( & ctrl_ -> top.WinterTop.ValveMain.Valve.PVEH, 65000);
      break;
    case VALVE_TYPE_PVE_AHS32:
    case VALVE_TYPE_OMFB:
      if(!GetFlagTopCalibr()) {
        Valv_Set( & ctrl_ -> top.WinterTop.ValveMain, Param->OMFB.PWM_RTR);
      }
      break;
    default:
      break;
    }
    _f_wr_ = 1;
  } else {
    _f_tail_ = 0;
    Conveyor_Switch( & ctrl_ -> top.WinterTop.SpreadConv, 0);
    SpreaderDisk_Switch( & ctrl_ -> top.WinterTop.SpreadDisk, 0);
    adrv_off( & ctrl_ -> top.WinterTop.Adrive, GetVar(HDL_MODEASSIMETRY));
    Valv_Set( & ctrl_ -> top.WinterTop.ValveMain, 0);

    if (_f_wr_) {
      wr_file(FN_CONV_TBL);
      _f_wr_ = 0;
    }
  }
  Top_Boost_Cycle( & ctrl_ -> top);

  RTR_Command(wTop);
  ConveyorCycle_UH( & ctrl_ -> top.WinterTop.SpreadConv, IOT_NormAI( & ctrl_ -> sens.AI.OilTemp));
  SpreaderDiskCycle( & ctrl_ -> top.WinterTop.SpreadDisk, ctrl_ -> top.WinterTop.SpreadWidth);
  AdriveCommand( & ctrl_ -> top.WinterTop.Adrive, ctrl_ -> cmd_no_key.Z_Change);
  ADrive_Draw( & ctrl_ -> top.WinterTop.Adrive);
  mat_spread_UH_cycle();
  HumidPump_cycle( & uh.humid_pump);

  Light_Draw( & ctrl_ -> light);

}

void ctrl_top_RTR_UH_timer10ms(tControl * ctrl_) {
  ADrive_Timer_10ms( & ctrl_ -> top.WinterTop.Adrive);
  static int32_t count = 0;
  count++;
  if (0 == count % 10) {
    ConveyorTimer( & ctrl_ -> top.WinterTop.SpreadConv);
    SpreaderDiskTimer( & ctrl_ -> top.WinterTop.SpreadDisk);
    Valv_Timer_100ms( & ctrl_ -> top.WinterTop.ValveMain);
    HumidPump_timer( & uh.humid_pump);
  }
  // uniqa_dbg_tm();
}

static int8_t wr_file(const char * fn_) {
  FileUnlink(fn_);
  tGsFile * fp_ = FileOpen(fn_, "wb");
  uint32_t rc_;
  if (NULL == fp_) {
    return -1;
  }
  //* conveyer
  rc_ = FileWrite( & uh_conv_len, 1, sizeof(uh_conv_len), fp_);
  if (sizeof(uh_conv_len) != rc_) {
    return -2;
  }
  rc_ = FileWrite(uh_conv_pwm_up, 1, sizeof(uh_conv_pwm_up), fp_);
  if (sizeof(uh_conv_pwm_up) != rc_) {
    return -3;
  }
  rc_ = FileWrite(uh_conv_pwm_down, 1, sizeof(uh_conv_pwm_down), fp_);
  if (sizeof(uh_conv_pwm_down) != rc_) {
    return -4;
  }
  //* humid
  rc_ = FileWrite( & uh_humid_len, 1, sizeof(uh_humid_len), fp_);
  if (sizeof(uh_humid_len) != rc_) {
    return -5;
  }
  rc_ = FileWrite(uh_humid_pwm, 1, sizeof(uh_humid_pwm), fp_);
  if (sizeof(uh_humid_pwm) != rc_) {
    return -6;
  }
  FileSync(fp_);
  FileClose(fp_);
  return 0;
}

static int8_t rd_file(const char * fn_) {
  tGsFile * fp_ = FileOpen(fn_, "rb");
  uint32_t rc_;
  if (NULL == fp_) {
    return -1;
  }
  //* conveyer
  rc_ = FileRead( & uh_conv_len, 1, sizeof(uh_conv_len), fp_);
  if (sizeof(uh_conv_len) != rc_) {
    return -2;
  }
  rc_ = FileRead(uh_conv_pwm_up, 1, sizeof(uh_conv_pwm_up), fp_);
  if (sizeof(uh_conv_pwm_up) != rc_) {
    return -3;
  }
  rc_ = FileRead(uh_conv_pwm_down, 1, sizeof(uh_conv_pwm_down), fp_);
  if (sizeof(uh_conv_pwm_down) != rc_) {
    return -4;
  }
  //* humid
  rc_ = FileRead( & uh_humid_len, 1, sizeof(uh_humid_len), fp_);
  if (sizeof(uh_humid_len) != rc_) {
    return -5;
  }
  rc_ = FileRead(uh_humid_pwm, 1, sizeof(uh_humid_pwm), fp_);
  if (sizeof(uh_humid_pwm) != rc_) {
    return -6;
  }
  FileClose(fp_);
  return 0;
}
