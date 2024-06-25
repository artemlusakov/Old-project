#include <stdint.h>
#include <UserCEvents.h>
#include <UserCAPI.h>
#include "vartab.h"
#include "objtab.h"
#include "control.h"
#include "param.h"
#include "RCText.h"
#include "gse_msgbox.h"
#include "gsToVisu.h"
#include "ctrl_top_conveyor.h"
#include "rtr_uh.h"
#include "visu_material.h"
#include "sql_table.h"


double convWidth = 0;
double convLengthPerRot = 0;

tSQLTable ConveyorTable = { .TableName = "Conveyor" ,
  .ColumnNames = {
    "Conv_RPM", "PWM Up", "PWM Down", "EMPTY"
  },
  .Cell = {
    {800, 108, 109, 30},
    {1870, 126, 127, 31},
    {2940, 146, 147, 32},
    {4010, 165, 166, 33},
    {5080, 184, 184, 34},
    {6150, 203, 204, 35},
    {7220, 220, 221, 36},
    {8290, 240, 241, 37},
    {9360, 258, 259, 38},
    {10430, 278, 279, 39},
    {11500, 296, 297, 40},
  }
};

tSQLTable SorgazHumid = { .TableName = "Sorgaz Humid" ,
	.ColumnNames = {
		"Val",	"PWM",	"EMPTY_1",	"EMPTY_3"
	},
	.Cell = {
		{7,		  230,		0,		0},
		{8,		  240,		0,		0},
		{10,		250,		0,		0},
		{20,		300,		0,		0},
		{28,		350,		0,		0},
		{39,		400,		0,		0},
		{48,		450,		0,		0},
		{57,		500,		0,		0},
		{65,		550,		0,		0},
		{70,		600,		0,		0},
		{75,		650,		0,		0},
	}
};

tSQLTable * pSorgazHumidTable;

tTemperatureTable * SetTemperature(tConveyor * Conveyor, uint32_t Idx_Reagent, uint32_t Idx_Temperature, int8_t Temperature, tSQLTable * Table) {
  if (Idx_Reagent >= NUMREAGENTS) {
    return NULL;
  }
  if (Idx_Temperature >= TEMPERATURE_PER_REAGENT) {
    return NULL;
  }
  Conveyor -> TempTables[Idx_Temperature].Table = SQL_Tabel_Init(Table);
  Conveyor -> TempTables[Idx_Temperature].temp = Temperature;
  return & Conveyor -> TempTables[Idx_Temperature];
}

void ConveyorCreate(tConveyor * Conveyor) {
  SetTemperature(Conveyor, 0, 0, 15, & ConveyorTable);
}

void ConveyorSet_Width_LenghtPerRot(double * pConvWidth, double * pConvLengthPerRot) {
  * pConvWidth = 0.65;
  * pConvLengthPerRot = 0.4075;
}

void ConveyorInit(tConveyor * Conveyor, uint32_t Device, uint32_t Pos) {
  ValveProp12V_Init( & Conveyor -> Valve, Device, Pos, 1000, 5);

  ConveyorSet_Width_LenghtPerRot( & convWidth, & convLengthPerRot);
  pSorgazHumidTable = SQL_Tabel_Init( & SorgazHumid);
}

int32_t ConveyorGetMaxDensity(tConveyor * Conveyor) {
  uint32_t val = 0;
  const tReagent * Reagent = GetActualReagent();
  if (NULL != Reagent) {
    val = Reagent -> SpreadingDensity_Max;
  }
  return val;
}

static double HumidMass = 0;
void CalcHumidMass(tControl * Ctrl, double MassAll) {
  if (Ctrl -> top.WinterTop.RTRHumid.ManualMode) { // Автоматический режим
    HumidMass = MassAll / (double) 100 * (double) GetVar(HDL_RTR_HUMIDPERCENT);
  } else { // Ручной режим
    HumidMass = MassAll * 0.2;
  }
}
uint32_t GetPWM_RTRHumid(void) {
  return TableGetLSEValue(pSorgazHumidTable, 0, 1, HumidMass);
}

double ShieldFactors[] = {
  0.07,
  0.104,
  0.13,
  0.157,
  0.182,
  0.2
};

double Conveyor_GetShieldFactor(void) {
  uint32_t val = GetVarIndexed(IDX_SHIELDPOSITION);
  if (val < GS_ARRAYELEMENTS(ShieldFactors)) {
    return ShieldFactors[val];
  }
  return 0;
}

void ConveyerTrueDensityGrapthics(double a, float Density) {
  double result = (double) Density * a;
  static int flag = 0;
  if (Density == 0) {
    if (flag == 1) {
      SendToVisuObj(OBJ_ATRESTTTEXT164, GS_TO_VISU_SET_ATTR_VISIBLE, 0);
      SendToVisuObj(OBJ_RTR_TRUE_DENSITY, GS_TO_VISU_SET_ATTR_VISIBLE, 0);
      flag = 0;
    }
  }
  if (result >= 10.7 && Density != 0) {
    if (flag == 0) {
      MsgBoxOk(RCTextGetText(RCTEXT_T_WARNING, GetVarIndexed(IDX_SYS_LANGUAGE)), RCTextGetText(RCTEXT_T_TRUEDENSITY, GetVarIndexed(IDX_SYS_LANGUAGE)), NULL, NULL);
      SendToVisuObj(OBJ_ATRESTTTEXT164, GS_TO_VISU_SET_ATTR_VISIBLE, 1);
      SendToVisuObj(OBJ_RTR_TRUE_DENSITY, GS_TO_VISU_SET_ATTR_VISIBLE, 1);
      flag = 1;
    }
  } else {
    if (flag == 1) {
      SendToVisuObj(OBJ_ATRESTTTEXT164, GS_TO_VISU_SET_ATTR_VISIBLE, 0);
      SendToVisuObj(OBJ_RTR_TRUE_DENSITY, GS_TO_VISU_SET_ATTR_VISIBLE, 0);
      flag = 0;
    }
  }
}

double ConveyorTruePermissionDensity(double a, double Density, double numberA) {
  if (Density == 0) return 0;
  if (numberA == 10.7) {
    for (double i = 0; i <= Density; i++) {
      double resultTruePermission1 = (double)(Density - (double) i) * a;
      if (resultTruePermission1 < 10.7) {
        return Density - i;
      }
    }
    return 0;
  }
  return 0;
}

void ConveyorCycle(tConveyor * Conveyor, int32_t temp) {
  const tControl * Ctrl = CtrlGet();
  const tParamData * Param = ParameterGet();
  tTemperatureTable * TempTables = Conveyor -> TempTables;

  float Density = Ctrl -> top.WinterTop.SpreadDensity;
  float CoefBoost = GetVarFloat(HDL_COEFBOOST);

  if (Ctrl -> top.BoostOn) {
    if (Density * CoefBoost >= ConveyorGetMaxDensity(Conveyor)) {
      Density = ConveyorGetMaxDensity(Conveyor);
      SendToVisuObj(OBJ_MSGBOX_MAXSPREAD, GS_TO_VISU_SET_ATTR_VISIBLE, 1);
    } else {
      Density = Density * CoefBoost;
      SendToVisuObj(OBJ_MSGBOX_MAXSPREAD, GS_TO_VISU_SET_ATTR_VISIBLE, 0);
    }
    SetVarIndexed(IDX_DENSITY, Density);
  } else {
    SetVarIndexed(IDX_DENSITY, Ctrl -> top.WinterTop.SpreadDensity);
  }

  const tReagent * Reagent = GetActualReagent();
  uint8_t workSpeed = GetVar(HDL_WORKSPEED);
  if (Reagent == NULL) {
    return;
  }

  if (Ctrl -> top.WinterTop.SpreadConv.on) {
    double speed = 0;
    speed = TopGetSpeed();
    double mDensity = Reagent -> MaterialDensity;

    if (fabs(mDensity) < 0.001) {
      mDensity = 1;
    }
    double shield_factor = 0.0;
    shield_factor = Conveyor_GetShieldFactor();

    static uint32_t g_min = 0;
    if (Param -> RTR.TypeRTR == 3) { // Для Соргаза
      double M = mDensity * convLengthPerRot * convWidth * shield_factor * 0.70; // 1200 * 0.52 * 0.60 * 0.07 * 0.70
      g_min = (((double) Ctrl -> top.WinterTop.SpreadWidth * (speed * 16.67) * (Density / 1000.)) / M) * 1000.;
      double g_true = g_min / 1000 / Density;
      ConveyerTrueDensityGrapthics(g_true, Density);
      if ((double) Density * g_true > 10.7) {
        SetVar(HDL_TRUEDENSITY, ConveyorTruePermissionDensity(g_true, Density, 10.7));
      }
      CalcHumidMass(Ctrl, M * g_min / 1000);
    } else {
      double a = speed * (double) Ctrl -> top.WinterTop.SpreadWidth / mDensity / shield_factor / convWidth / convLengthPerRot / 60.0;
      ConveyerTrueDensityGrapthics(a, Density);
      if ((double) Density * a > 10.7) {
        SetVar(HDL_TRUEDENSITY, ConveyorTruePermissionDensity(a, Density, 10.7));
      }
      g_min = (double) Density * a * 1000;
    }

    if (g_min > Conveyor -> g_min) {
      Conveyor -> g_minvisu = g_min;
      Conveyor -> g_min = g_min;

      //temp is colder then the table with the coldest temperature-> take table with the coldest temperature
      if (temp <= TempTables[0].temp) {
        if ((Density == 0) || (speed == 0) || (workSpeed >= speed)) //AK STOPS the conveyor if speed=0 or density = 0.
        { //AK
          Conveyor -> pwm = 0; //AK
        } //AK
        else {
          Conveyor -> pwm = TableGetLSEValue(TempTables[0].Table, 0, 1, Conveyor -> g_min);
          Conveyor -> g_min_max = TempTables[0].Table -> Cell[10][0];
        }
      }
    } else if (g_min < Conveyor -> g_min) {
      Conveyor -> g_minvisu = g_min;
      Conveyor -> g_min = g_min;

      //temp is colder then the table with the coldest temperature-> take table with the coldest temperature
      if (temp <= TempTables[0].temp) {
        if ((Density == 0) || (speed == 0) || (workSpeed >= speed)) //AK STOPS the conveyor if speed=0 or density = 0.
        { //AK
          Conveyor -> pwm = 0; //AK
        } //AK
        else {
          Conveyor -> pwm = TableGetLSEValue(TempTables[0].Table, 0, 2, Conveyor -> g_min);
          Conveyor -> g_min_max = TempTables[0].Table -> Cell[10][0];
        }
      }
    }
    // Разгрузка
    if (RTRDump_IsOn( & Ctrl -> top.WinterTop)) {
      Conveyor -> pwm = TableGetLSEValue(TempTables[0].Table, 0, 2, TempTables[0].Table -> Cell[10][0]);
    }
  } else {
    Conveyor -> pwm = 0;
    Conveyor -> g_minvisu = -1;
    Conveyor -> g_min = -1;
  }

  // Выключение конвейера (Шим = 0) при изменении индикатора
  // ICON_DENSITY на состояние конвейер не вращается
  if (GetVarIndexed(IDX_ICON_DENSITY) == 2) { // MAT_SENSOR_NO_ROT
    Conveyor -> pwm = 0;
    Conveyor -> g_minvisu = -1;
    Conveyor -> g_min = -1;
  }
  ValvProp_Set( & Conveyor -> Valve.Valve.PVEH, Conveyor -> pwm);
}

uint8_t flagUnloading = 0;
uint8_t getFlagUnloading(void) {
  return flagUnloading;
}
void setFlagUnloading(uint8_t flag) {
  flagUnloading = flag;
}

void ConveyorCycle_UH(tConveyor * conv_, int32_t temp) {
  tControl * ctrl_ = ctrl_get();
  uint8_t wrk_speed_ = GetVar(HDL_WORKSPEED);
  float coef_boost = GetVarFloat(HDL_COEFBOOST);
  static int _f_conv_err_ = 0; ///< 0 = не было ошибки, 1 - была ошибка
  static int _conv_dir_ = 0; ///< 0 = ровно, -1 = вниз, 1 = вверх
  float dens_spdr_ = (float) ctrl_ -> top.WinterTop.SpreadDensity; ///< плотность посыпания, г/м2
  //? Режим разгрузки
  if (ctrl_ -> top.WinterTop.SpreadConv.on && ON != uh.stt.tail_pos && MD_DUMP == uh.md) {
    //* максимальная скорость конвеера
    conv_ -> pwm = UH_CONV_PWM_MAX;
    setFlagUnloading(1);
  }
  //? Рабочий режим
  else if (ctrl_ -> top.WinterTop.SpreadConv.on && OFF != uh.stt.tail_pos) {
    const tReagent * rgnt_ = GetActualReagent();

    float dens_mat_ = rgnt_ -> MaterialDensity; ///< плотность реагента, кг/м3 (г/литр)
    float speed_ = TopGetSpeed() * 1000. / 3600; ///< скорость, м/сек
    setFlagUnloading(0);
    if (rgnt_ == NULL) {
      return;
    }
    if (ctrl_ -> top.BoostOn) {
      if (dens_spdr_ * coef_boost >= ConveyorGetMaxDensity(conv_)) {
        dens_spdr_ = ConveyorGetMaxDensity(conv_);
        SendToVisuObj(OBJ_MSGBOX_MAXSPREAD, GS_TO_VISU_SET_ATTR_VISIBLE, 1);
      } else {
        dens_spdr_ = dens_spdr_ * coef_boost;
        SendToVisuObj(OBJ_MSGBOX_MAXSPREAD, GS_TO_VISU_SET_ATTR_VISIBLE, 0);
      }
    }
    if (fabs(dens_mat_) < 0.001) {
      dens_mat_ = 1000;
    }
    /// объём реагента в литр/сек: g/m2 * m * m/sec / g/litr
    float vol_ = dens_spdr_ * (float) ctrl_ -> top.WinterTop.SpreadWidth * speed_ / dens_mat_;
    if (vol_ > uh_conv_vol[uh_conv_len - 1]) {
      uh.f_conv_vol_lim = ON;
      vol_ = uh_conv_vol[uh_conv_len - 1];
    } else {
      uh.f_conv_vol_lim = OFF;
    }
    if (vol_ > 0 && vol_ < uh_conv_vol[0]) {
      vol_ = uh_conv_vol[0];
    }
    /// предельная скорость, м/сек : литр/сек * г/литр / г/м2 / м
    float speed_lim_ = uh_conv_vol[uh_conv_len - 1] * dens_mat_ / dens_spdr_ / (float) ctrl_ -> top.WinterTop.SpreadWidth;
    if (speed_lim_ > SPEED_LIM) {
      speed_lim_ = SPEED_LIM;
    }
    uh.conv_speed_lim = speed_lim_ * 3600 / 1000; // км/ч
    Ctrl_SetMaxSpeed(uh.conv_speed_lim);

    if ((dens_spdr_ == 0) || (wrk_speed_ >= TopGetSpeed())) {
      conv_ -> pwm = 0;
      uh.wr.conv_rot = 0;
      uh.conv_vol = 0;
      uh.md_conv = MD_INI;
    } else if (vol_ != uh.conv_vol && (MD_SET != uh.md_conv || 0 == uh.rd.conv_rot)) {
      if (vol_ < uh.conv_vol) { //= уменьшение
        conv_ -> pwm = liner_interpret(uh_conv_vol, uh_conv_pwm_down, uh_conv_len, vol_);
        _conv_dir_ = -1;
      } else { //= увеличение
        conv_ -> pwm = liner_interpret(uh_conv_vol, uh_conv_pwm_up, uh_conv_len, vol_);
        _conv_dir_ = 1;
      }
      if (0 == uh.conv_vol) { //= с нуля стартуем, выставим Ш� М побольше
        conv_ -> pwm *= 1.2;
      }
      uh.wr.conv_rot = liner_interpret(uh_conv_vol, uh_conv_rot, uh_conv_len, vol_);
      uh.md_conv = MD_SET;
      uh.conv_vol = vol_;
      uh.tm_conv = GetMSTick() + TM_CONV_SET;
    } else if (vol_ > 0) { //= объём тот что надо и больше 0
      if (MD_SET == uh.md_conv && GetMSTick() >= uh.tm_conv) { //= Время установки вышло
        conv_ -> g_minvisu = (uint32_t) vol_ * 1000;
        if (uh.wr.conv_rot == uh.rd.conv_rot) { //= достигли рабочего состояния
          uh.md_conv = MD_WRK;
          conv_ -> g_min = (uint32_t) vol_ * 1000;
          //* корректировка таблицы Ш� М
          unsigned idx_ = 0;
          for (unsigned i_ = 0; i_ < uh_conv_len; ++i_) {
            if (uh.rd.conv_rot == uh_conv_rot[i_]) {
              idx_ = i_;
              break;
            }
          }
          if (1 == _conv_dir_) {
            uh_conv_pwm_up[idx_] = uh.wr.conv_pwm;
            for (unsigned i_ = idx_ + 1; i_ < uh_conv_len; ++i_) {
              if (uh_conv_pwm_up[i_] <= uh_conv_pwm_up[i_ - 1]) {
                uh_conv_pwm_up[i_] = uh_conv_pwm_up[i_ - 1] + 1;
              }
            }
            for (int i_ = idx_ - 1; i_ >= 0; --i_) {
              if (uh_conv_pwm_up[i_] >= uh_conv_pwm_up[i_ + 1]) {
                uh_conv_pwm_up[i_] = uh_conv_pwm_up[i_ + 1] - 1;
              }
            }
          } else if (-1 == _conv_dir_) {
            uh_conv_pwm_down[idx_] = uh.wr.conv_pwm;
            for (unsigned i_ = idx_ + 1; i_ < uh_conv_len; ++i_) {
              if (uh_conv_pwm_down[i_] <= uh_conv_pwm_down[i_ - 1]) {
                uh_conv_pwm_down[i_] = uh_conv_pwm_down[i_ - 1] + 1;
              }
            }
            for (int i_ = idx_ - 1; i_ >= 0; --i_) {
              if (uh_conv_pwm_down[i_] >= uh_conv_pwm_down[i_ + 1]) {
                uh_conv_pwm_down[i_] = uh_conv_pwm_down[i_ + 1] - 1;
              }
            }
          }
          _f_conv_err_ = 0;
        } else if (OFF == uh.stt.conv && 0 == _f_conv_err_) {
          static uint32_t tmrError = 0;
          static uint8_t flagTmr = 0;
          if (!flagTmr) {
            tmrError = GetMSTick() + 5000;
            flagTmr = 1;
          }
          if (GetMSTick() >= tmrError) {
            uh.md = MD_INI;
            uh.md_conv = MD_INI;
            conv_ -> pwm = 0;
            _f_conv_err_ = 1;
            flagTmr = 0;
            MsgBoxOk(RCTextGetText(RCTEXT_T_WARNING, GetVarIndexed(IDX_SYS_LANGUAGE)), RCTextGetText(RCTEXT_T_CONVSENSPROBLEM, GetVarIndexed(IDX_SYS_LANGUAGE)), NULL, NULL);
          }
        } else if (ON != uh.stt.conv || uh.wr.conv_pwm >= uh_conv_pwm_up[uh_conv_len - 1]) {
          uh.md_conv = MD_WRK;
          conv_ -> pwm = liner_interpret(uh_conv_vol, uh_conv_pwm_up, uh_conv_len, vol_);
          uh.wr.conv_rot = liner_interpret(uh_conv_vol, uh_conv_rot, uh_conv_len, vol_);
        } else {
          unsigned pwm_;
          if (uh.wr.conv_rot < uh.rd.conv_rot) { //= вниз
            _conv_dir_ = -1;
          } else { //= вверх
            _conv_dir_ = 1;
          }
          if (uh.rd.conv_rot == UH_CONV_ROT_MIN) {
            pwm_ = uh.wr.conv_pwm + (float)(uh.wr.conv_pwm - UH_CONV_PWM_MIN) * (uh.wr.conv_rot - uh.rd.conv_rot);
          } else if (uh.wr.conv_pwm == UH_CONV_PWM_MIN) {
            pwm_ = uh.wr.conv_pwm + (float)(UH_CONV_PWM_MAX - UH_CONV_PWM_MIN) / (float)(UH_CONV_ROT_MAX - UH_CONV_ROT_MIN) * (uh.wr.conv_rot - uh.rd.conv_rot);
          } else {
            pwm_ = uh.wr.conv_pwm + (float)(uh.wr.conv_rot - uh.rd.conv_rot) * (uh.wr.conv_pwm - UH_CONV_PWM_MIN) / fabs((float)(uh.rd.conv_rot - UH_CONV_ROT_MIN));
          }
          if (pwm_ < UH_CONV_PWM_MIN) {
            pwm_ = UH_CONV_PWM_MIN;
          } else if (pwm_ > UH_CONV_PWM_MAX) {
            pwm_ = UH_CONV_PWM_MAX;
          }
          conv_ -> pwm = pwm_;
          uh.tm_conv = GetMSTick() + TM_CONV_SET;
        }
      } else if (MD_WRK == uh.md_conv) {
        if (ON == uh.stt.conv && uh.wr.conv_rot != uh.rd.conv_rot) {
          uh.md_conv = MD_SET;
          uh.tm_conv = GetMSTick() + TM_CONV_SET;
        }
      }
    } else { //= оьъём 0
      uh.md_conv = MD_INI;
    }
  } else {
    conv_ -> g_minvisu = -1;
    conv_ -> g_min = -1;
    conv_ -> pwm = 0;
    uh.conv_vol = 0;
    uh.md_conv = MD_INI;
    uh.wr.conv_rot = 0;
    _f_conv_err_ = 0;
  }
  SetVarIndexed(IDX_DENSITY, dens_spdr_);
  ValvProp_Set( & conv_ -> Valve.Valve.PVEH, conv_ -> pwm);
}

void Conveyor_Switch(tConveyor * Conveyor, uint32_t value) {
  Conveyor -> on = value;
}

void ConveyorTimer(tConveyor * Conveyor) {
  ValvProp_Timer_100ms( & Conveyor -> Valve.Valve.PVEH);
}

void ConveyorDraw(const tConveyor * Conveyor) {
  static int32_t msk_old = -1;
  if (GetCurrentMaskShown() != msk_old) {
    msk_old = GetCurrentMaskShown();
    const tReagent * ActualReagent = GetActualReagent();
    if (NULL != ActualReagent) {
      if (NULL != ActualReagent -> Name) {
        SetVisObjData(OBJ_RTR_MATERIAL, ActualReagent -> Name, strlen(ActualReagent -> Name) + 1);
        SetVisObjData(OBJ_MATERIAL_UH, ActualReagent -> Name, strlen(ActualReagent -> Name) + 1);
      }
    }
  }
}