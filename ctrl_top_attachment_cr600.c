#include <stdint.h>
#include <UserCEvents.h>
#include <UserCAPI.h>
#include "vartab.h"
#include "objtab.h"
#include "RCText.h"
#include "control.h"
#include "io.h"
#include "gsemcm.h"
#include "gsToVisu.h"
#include "ctrl_light.h"
#include "gse_msgbox.h"
#include "RCColor.h"
#include "param.h"
#include "ctrl_top_adrive.h"
#include "visu_material.h"
#include	"profile.h"
#include "ctrl_top_attachment_cr600.h"
#include "ctrl_top_attachment_cr.h"
#include "top_summer_attachment.h"
#include	"usr_tools.h"
#include	"math.h"
#include	"pinout.h"
#include "calibr_omfb.h"

enum CR_CONST {
  TMT_AIR_DRY = 5000,
  TMT_AIR_DRY_PUMP = 15000,
  RAIL1 = 0x01,
  RAIL2 = 0x02,
  RAIL3 = 0x04,
  DIRL = 0x10,
  DIRC = 0x20,
  DIRR = 0x40,
  VOL_STEPS = 20,
  VOL_REV = 1170,
  DISPENS_MIN = 450,
  DISPENS_MAX = 1900,
  DISPENS_ACCU = 15,
  RECYCL_OFF = 1,
  RECYCL_ON = 0,
  SET_DOZ_MIN = 820,
  SET_DOZ_MAX = 1600,
};

static int32_t tmAirDry = 0;
static int32_t tmAirDry_dispens = 0;
static int stateRailAirDry = 0;
static int startRailOpen = 0;
static uint32_t timer_press = 0;

//Переменная состояния работы установки. = 1 - включена; = 0 - не работает
static uint32_t STATE_CR600 = 0;
static int rail_side_state = 0;

int recycle_state = 0;
int Out_ = 0;

//Переменная отрисовки секторов. Например при dispensSectors = 0x110 отрисовываются активными левый и центральный сектора
int32_t dispensSectors = 0;
CR_ctrl_t ctrl_CR600;
CR_DO_t cr_do;
CR_dispens_pos_t dispens;

// Флаг работы шланга
static int hoseWorking = 0;

typedef struct tagCRValve {
  uint8_t * DO;
  uint8_t Device;
  uint8_t idx;
}
tCRValve;

tCRValve CR600Valve[] = {
	{.DO = &cr_do.vavl_hose,				.Device = MCM250_2,		.idx = 1},

	{.DO = &cr_do.valv_l_rail1,				.Device = MCM250_6,		.idx = 1},
	{.DO = &cr_do.valv_l_rail2,				.Device = MCM250_6,		.idx = 3},
	{.DO = &cr_do.valv_l_rail3,				.Device = MCM250_6,		.idx = 5},

  {.DO = &cr_do.valv_c_rail1,				.Device = MCM250_6,		.idx = 0},
	{.DO = &cr_do.valv_c_rail2,				.Device = MCM250_6,		.idx = 8},
	{.DO = &cr_do.valv_c_rail3,				.Device = MCM250_6,		.idx = 2},

  {.DO = &cr_do.valv_r_rail1,				.Device = MCM250_6,		.idx = 6},
	{.DO = &cr_do.valv_r_rail2,				.Device = MCM250_6,		.idx = 4},
	{.DO = &cr_do.valv_r_rail3,				.Device = MCM250_6,		.idx = 7},

	{.DO = &cr_do.recycl,				      .Device = MCM250_5,		.idx = 4},
	{.DO = &cr_do.dispens_on,			    .Device = MCM250_5,		.idx = 1},
	{.DO = &cr_do.dispens_off,			  .Device = MCM250_5,		.idx = 3},
	{.DO = &cr_do.air_dry,			      .Device = MCM250_5,		.idx = 5},
	{.DO = &cr_do.drive_heating,			.Device = MCM250_5,		.idx = 8},
};

void Ctrl_top_LRS_CR600_Init(void) {
  //клапаны установки, рейки, дозирование, рециркуляция
  //насос, датчик
  tControl * ctrl_ = CtrlGet();

  SendToVisuObj(OBJ_WATERPUMP, GS_TO_VISU_SET_ATTR_VISIBLE, 1);
  CtrlI_StandardDOs_SetRearWorkLight_ForCR600();
  WaterPump_SetMaxLiter(65000);

  SendToVisuObj(OBJ_SIMPLEBITMAP1341, GS_TO_VISU_SET_ATTR_VISIBLE, !GetVarIndexed(IDX_CHEK_HOSE)); // иконка чекбокса активации шланга
  //@@@ Spreader settings
  SetVar(HDL_DENSITY, 0);
  SendToVisuObj(OBJ_ATRESTTTEXT926, GS_TO_VISU_SET_ATTR_VISIBLE, 1);
  SendToVisuObj(OBJ_ATRESTTTEXT925, GS_TO_VISU_SET_ATTR_VISIBLE, 1);
  SendToVisuObj(OBJ_INDEXBITMAP689, GS_TO_VISU_SET_ATTR_VISIBLE, 1);
  SendToVisuObj(OBJ_ATRESTTTEXTLIST60, GS_TO_VISU_SET_ATTR_VISIBLE, 1);

  // Инициализация аттрибутов для корректного отображения в режимах работы на шланг и рейки
  SetVarIndexed(IDX_GRAMMPERMETER_TEXTATTR, 0x38);
  SetVarIndexed(IDX_LITERPERMIN_TEXTATTR, 0x30);

  cr_do.valv_l_rail1 = cr_do.valv_c_rail1 = cr_do.valv_r_rail1 =
    cr_do.valv_l_rail2 = cr_do.valv_c_rail2 = cr_do.valv_r_rail2 =
    cr_do.valv_l_rail3 = cr_do.valv_c_rail3 = cr_do.valv_r_rail3 = 1;

  cr_do.recycl = 0;
  cr_do.rail1 = cr_do.rail2 = cr_do.rail3 = 0;
  cr_do.dispens_on = cr_do.dispens_off = 0;

  ctrl_CR600.md = MD_INI;

  dispens.dens_stp = dispens.dens_max / VOL_STEPS;
  IOT_NormAI_Init( & ctrl_ -> sens.AI.cr_liqmeter, 400, 2000, 0, 700, 0, 0);
  SetVar(HDL_MCM250_6_INIT, 1);
}

void HoseModeSwitch(void);
void HoseWorking(void);
void DispensSectorsCheck(void);

void Ctrl_top_LRS_CR600_Cycle(uint32_t evtc, tUserCEvt * evtv) {
  tControl * ctrl_ = CtrlGet();
  rail_side_state = (DIRL * GetVarIndexed(IDX_ROADLINEL)) | (DIRC * GetVarIndexed(IDX_ROADLINEM)) | (DIRR * GetVarIndexed(IDX_ROADLINER));
  Ctrl_CR_Dump();
  Top_Boost_Cycle( & ctrl_ -> top);
  Light_Draw( & ctrl_ -> light);
  //был ли запущен насос нажатием боковой кнопки джойстика
  if (LiqPressureCheck(ctrl_)) {
    PumpWorking();
  } else {
    Pump_NOT_Working();
  }

  //Текущий расход жидкости верхнего бака
  static int32_t CurrentLiterPerMin = 0;
  if (TimerCheck(LITER_SET_TIMER, 1000)) {
    CurrentLiterPerMin = CheckLiterOnSpeed(3);
  } // Изменения по времени
  if (GetVar(HDL_DUMP_REAGENT) == 1 || ctrl_ -> top.BoostOn) {
    CurrentLiterPerMin = MAX_LITER_PER_MIN;
    rail_side_state = DIRL | DIRC | DIRR;
    if (GetVar(HDL_DUMP_REAGENT) == 1) {
      ctrl_CR600.md = MD_DUMP;
    }
  }

  if (ctrl_CR600.md == MD_AIRDRY || ctrl_CR600.md == MD_AIRDRY_PUMP) {
    AirDry_Cycle();
  } else {
    if (ctrl_ -> sens.DI.pump_airdry == 0) {
      Out_ = 0;
    }

    dispens.set = SetDozParameter(CurrentLiterPerMin, STATE_CR600);

    if (IsMainMaskCR() && IsKeyPressedNew(4) && !GetVarIndexed(IDX_CHEK_HOSE)) { //включение с кнопки 4 иконки шланг
      HoseModeSwitch();
    }

    if (cr_do.vavl_hose = hoseWorking) {
      HoseWorking();
    } else {
      DispensSectorsCheck();
      SetSaidOpen(STATE_CR600);
      RailOpen_Close(STATE_CR600);
    }

    DicpensOpenClose(STATE_CR600);
    SetRecycleState();
  }

  cr_do.recycl = recycle_state;
  SetValveHeating();
  SendToMCM();
}

void Ctrl_top_LRS_CR600_Timer_10ms(void) {
  tControl * ctrl_ = CtrlGet();
  dispens.cur = GSeMCM_GetAnalogIn(MCM250_5, 2);

  if (0 != ctrl_CR600.dir_disp && 0 != dispens.set && absint(dispens.set - dispens.cur) <= 200) {
    ctrl_CR600.dir_disp = 0;
    cr_do.dispens_off = 0;
    cr_do.dispens_on = 0;
  }

  LRS_Timer_10ms( & ctrl_ -> top.SummerTop.LRS);
}

/***********************************************************************************************************************/

void RailOpen_Close(int IsOpen) {
  int Rail1_is_on = ctrl_CR600.rail_dir & RAIL1 ? 1 : 0;
  int Rail2_is_on = ctrl_CR600.rail_dir & RAIL2 ? 1 : 0;
  int Rail3_is_on = ctrl_CR600.rail_dir & RAIL3 ? 1 : 0;

  cr_do.valv_l_rail1 = Rail1_is_on && (ctrl_CR600.rail_dir & DIRL) ? IsOpen : 0;
  cr_do.valv_c_rail1 = Rail1_is_on && (ctrl_CR600.rail_dir & DIRC) ? IsOpen : 0;
  cr_do.valv_r_rail1 = Rail1_is_on && (ctrl_CR600.rail_dir & DIRR) ? IsOpen : 0;

  cr_do.valv_l_rail2 = Rail2_is_on && (ctrl_CR600.rail_dir & DIRL) ? IsOpen : 0;
  cr_do.valv_c_rail2 = Rail2_is_on && (ctrl_CR600.rail_dir & DIRC) ? IsOpen : 0;
  cr_do.valv_r_rail2 = Rail2_is_on && (ctrl_CR600.rail_dir & DIRR) ? IsOpen : 0;

  cr_do.valv_l_rail3 = Rail3_is_on && (ctrl_CR600.rail_dir & DIRL) ? IsOpen : 0;
  cr_do.valv_c_rail3 = Rail3_is_on && (ctrl_CR600.rail_dir & DIRC) ? IsOpen : 0;
  cr_do.valv_r_rail3 = Rail3_is_on && (ctrl_CR600.rail_dir & DIRR) ? IsOpen : 0;
}

void SetRailOpen(int CurrentRailOpen) {

  switch (CurrentRailOpen) {
  case 0:
    ctrl_CR600.rail_dir = 0;
    break;
  case 100:
    ctrl_CR600.rail_dir = rail_side_state | RAIL1;
    break;
  case 200:
    ctrl_CR600.rail_dir = rail_side_state | RAIL2;
    break;
  case 300:
    ctrl_CR600.rail_dir = rail_side_state | RAIL3;
    break;
  case 400:
    ctrl_CR600.rail_dir = rail_side_state | RAIL3 | RAIL1;
    break;
  case 500:
    ctrl_CR600.rail_dir = rail_side_state | RAIL3 | RAIL2;
    break;
  case 600:
    ctrl_CR600.rail_dir = rail_side_state | RAIL3 | RAIL2 | RAIL1;
    break;
  }
}

double CurrCalc(int par) {
  return SET_DOZ_MIN + (SET_DOZ_MAX - SET_DOZ_MIN) / 100. * par / 6.;
}

double CurrentByLiters(int CurrentLiterPerMin) {
  if (CurrentLiterPerMin <= 0) {
    SetRailOpen(0);
    return SET_DOZ_MIN;
  }
  if (CurrentLiterPerMin >= 600) {
    SetRailOpen(600);
    return SET_DOZ_MAX;
  }
  for (int i = 0; i <= 600; i = i + 100) {
    if (CurrentLiterPerMin < i) {
      SetRailOpen(i);
      return CurrCalc(CurrentLiterPerMin); // Вывод Тока для Нужного дозирования
    }
    if (CurrentLiterPerMin == i) {
      SetRailOpen(i);
      return SET_DOZ_MAX;
    }
  }
}

double SetDozParameter(int CurrentLiterPerMin, int IsOpen) {
  if (IsOpen == 1) {
    return CurrentByLiters(CurrentLiterPerMin);
  }
  return SET_DOZ_MIN;
}

void Ctrl_DispensOpenClose(void) {
  dispens.cur = GSeMCM_GetAnalogIn(MCM250_5, 2);

  if (0 < ctrl_CR600.dir_disp && dispens.cur < DISPENS_MAX) {
    cr_do.dispens_on = 1;
    cr_do.dispens_off = 0;
  } else if (ctrl_CR600.dir_disp < 0 && dispens.cur > DISPENS_MIN) {
    cr_do.dispens_on = 0;
    cr_do.dispens_off = 1;
  } else {
    cr_do.dispens_off = 0;
    cr_do.dispens_on = 0;
  }
}

void Ctrl_CR600_DispensOpenClose(int isOpen) {
  dispens.cur = GSeMCM_GetAnalogIn(MCM250_5, 2);

  if (absint(dispens.set - dispens.cur) <= DISPENS_ACCU) {
    ctrl_CR600.dir_disp = 0;
  } else {
    if (dispens.set < dispens.cur) {
      ctrl_CR600.dir_disp = -1;
    } else {
      ctrl_CR600.dir_disp = 1;
    }
  }
  if (isOpen == 0) {
    ctrl_CR600.dir_disp = -1;
  }
}

void DicpensOpenClose(int IsOpen) {
  Ctrl_CR600_DispensOpenClose(IsOpen);
  Ctrl_DispensOpenClose();
}

int IsMainMaskCR(void) {
  if (!IsMsgContainerOn(CNT_LIGHTKEYS) && !IsMsgContainerOn(CNT_LIGHTKEYS_2) &&
    IsInfoContainerOn(CNT_TOP_CR_600A)) {
    return 1; // На главном экране
  }
  return 0;
}

void SetRecycleState(void) {
  static int32_t tmt_cr600_recycle = 0;

  if (MD_DUMP == ctrl_CR600.md) {
    recycle_state = RECYCL_OFF;
  } else {
    if (GSeMCM_GetAnalogIn(MCM250_5, 3) > VOL_REV && MD_DUMP != ctrl_CR600.md) {
      recycle_state = RECYCL_ON;
      tmt_cr600_recycle = GetMSTick() + 1000;
    }
    else {
      if (tmt_cr600_recycle < GetMSTick() || tmt_cr600_recycle == 0) {
        recycle_state = RECYCL_OFF;
      }
    }
  }
  SetVar(HDL_PRESSUREVOLTAGE, (double) GSeMCM_GetAnalogIn(MCM250_5, 3) / (double) 100);
  SetVar(HDL_PRESSUREBAR, ((double) GSeMCM_GetAnalogIn(MCM250_5, 3) / (double) 100) - (double) 4);
}

void SetValveHeating(void) {
  if (CtrlGet() -> engine.AmbientAirTemp < 5) {
    cr_do.drive_heating = 1;
  } else {
    cr_do.drive_heating = 0;
  }
}

void WorkModeSwitch(int isWorking) {
  ctrl_CR600.md = MD_CLR * !isWorking + MD_WRK * isWorking;
  STATE_CR600 = isWorking;
  ctrl_CR600.q_valv_oil = isWorking;
  if (!GetFlagTopCalibr()) {
    Ctrl_Valve_SetLiter( & CtrlGet() -> top.SummerTop.Pump.Valv.Valve.PVEH, 65000 * isWorking);
  }
  SetVarIndexed(IDX_ICON_PUMP, 1);
  timer_press *= isWorking;
}

void DispensSectorsCheck(void) {
  if (dispensSectors == 0) {
    dispensSectors = 0x010;
  }
}

void PumpWorking(void) {
  int isWorking = (TopGetSpeed() >= GetVar(HDL_WORKSPEED) && GetVar(HDL_DENSITY) > 0);
  WorkModeSwitch(isWorking);

  if (timer_press == 0) {
    timer_press = GetMSTick();
  }
  if ((IsKeyPressedNew(2) && IsMainMaskCR()) || CtrlGet() -> sens.DI.pump_airdry == 1) {
    MsgBoxOk(RCTextGetText(RCTEXT_T_WARNING, GetVarIndexed(IDX_SYS_LANGUAGE)), RCTextGetText(RCTEXT_T_OFFTOP, GetVarIndexed(IDX_SYS_LANGUAGE)), NULL, NULL);
  }
}

void Pump_NOT_Working(void) {
  if (MD_WRK == ctrl_CR600.md) {
    ctrl_CR600.md = MD_CLR;
  }
  if (IsKeyPressedNew(2) && IsMainMaskCR()) {
    if (ctrl_CR600.md != MD_AIRDRY && ctrl_CR600.md != MD_AIRDRY_PUMP)
      ctrl_CR600.md = MD_AIRDRY;
  }
  if (CtrlGet() -> sens.DI.pump_airdry == 1 && IsMainMaskCR() && ctrl_CR600.md != MD_AIRDRY_PUMP && ctrl_CR600.md != MD_AIRDRY && Out_ == 0) {
    ctrl_CR600.md = MD_AIRDRY_PUMP;
  }

  // ValvProp_Set( & CtrlGet() -> top.SummerTop.MainValve.Valve.PVEH, 0);
  Ctrl_Valve_SetLiter( & CtrlGet() -> top.SummerTop.Pump.Valv.Valve.PVEH, 0);
  SetVarIndexed(IDX_ICON_PUMP, 0);
  STATE_CR600 = OFF;
  timer_press = 0;
}

void SetDensity(int min, int max, uint8_t step) {
  tControl * ctrl_ = CtrlGet();

  //проверка задержки изменения распределения
  if (ctrl_ -> cmd_no_key.X > 50 && GetVar(HDL_DENSITY) < max) {
    if (TimerCheck(DENSIT_TIMER, DENSIT_TIMER_MS)) {
      SetVar(HDL_DENSITY, GetVar(HDL_DENSITY) + step);
    }
  }
  if (ctrl_ -> cmd_no_key.X < -50 && GetVar(HDL_DENSITY) > min) {
    if (TimerCheck(DENSIT_TIMER, DENSIT_TIMER_MS)) {
      SetVar(HDL_DENSITY, GetVar(HDL_DENSITY) - step);
    }
  }
}

void AddSector() {
  if (dispensSectors == 0x110 || dispensSectors == 0x011 || dispensSectors == 0x111) {
    dispensSectors = 0x111;
    return;
  }
  if (dispensSectors == 0x010 || dispensSectors == 0x100) {
    dispensSectors = 0x110;
    return;
  }
  dispensSectors = 0x011;
}

void RemoveSector() {
  if (dispensSectors == 0x111) {
    dispensSectors = 0x110;
    return;
  }
  if (dispensSectors == 0x110 || dispensSectors == 0x011) {
    dispensSectors = 0x010;
    return;
  }
}

void SetLinesToDraw() {
  tControl * Ctrl = CtrlGet();

  //смещение зоны распределения по команде с рокера z
  if (0 > IOT_AnaJoy_PressedNew( & Ctrl -> joystick.Z)) {
    if (!(dispensSectors & 0x001)) {
      dispensSectors >>= 4;
    }
  } else
  if (0 < IOT_AnaJoy_PressedNew( & Ctrl -> joystick.Z)) {
    if (!(dispensSectors & 0x100)) {
      dispensSectors <<= 4;
    }
  }

  //изменение ширины зоны распределения по команде с рокера Y
  if (0 < IOT_AnaJoy_PressedNew( & Ctrl -> joystick.Y)) {
    AddSector();
  } else if (0 > IOT_AnaJoy_PressedNew( & Ctrl -> joystick.Y)) {
    RemoveSector();
  }
}

void DrawRoadLines(int IsOpen) {
  SetVarIndexed(IDX_ROADLINER, (dispensSectors & 0x1) * IsOpen);
  SetVarIndexed(IDX_ROADLINEM, ((dispensSectors & 0x10) >> 4) * IsOpen);
  SetVarIndexed(IDX_ROADLINEL, ((dispensSectors & 0x100) >> 8) * IsOpen);
}

void SetSaidOpen(int IsOpen) {
  SetDensity(0, 190, 10);
  if (IsOpen) SetLinesToDraw();
  DrawRoadLines(IsOpen);
}

void DispensControl() {
  dispens.cur = GSeMCM_GetAnalogIn(MCM250_5, 2);

  if (startRailOpen == 2) {
    cr_do.air_dry = 1; // открытие КОМ продувки после полного открытия дозирующего
    if (GetMSTick() >= TMT_AIR_DRY + tmAirDry_dispens) {
      cr_do.air_dry = 0; // закрытие КОМ продувки после полного открытия дозирующего
      startRailOpen = 3;
    }
  }
  if (startRailOpen == 0 || startRailOpen == 1) {
    if (dispens.cur <= DISPENS_MAX) {
      cr_do.dispens_on = 1;
      cr_do.dispens_off = 0;
      startRailOpen = 1;
    } else {
      startRailOpen = 2;
      tmAirDry_dispens = GetMSTick();
      cr_do.dispens_off = 0;
      cr_do.dispens_on = 0;
    }
  } else if (startRailOpen == 3) {
    if (dispens.cur > DISPENS_MIN) {
      cr_do.dispens_on = 0;
      cr_do.dispens_off = 1;
    } else {
      startRailOpen = 4;
      cr_do.dispens_off = 0;
      cr_do.dispens_on = 0;
    }
  }
}

void AirDry() {
  if (GetMSTick() >= TMT_AIR_DRY + tmAirDry && startRailOpen == 4) {
    stateRailAirDry++;
    tmAirDry = GetMSTick();
  }

  if (IsKeyPressedNew(2) && IsMainMaskCR() && startRailOpen != 0) { // выключение продувки при повторном нажатии кнопки 4 и если было начало открытия клапанов
    stateRailAirDry = 13;
  }

  DispensControl();

  switch (stateRailAirDry) {
  case 0:
    SetVarIndexed(IDX_AIRDRY, 1); // графическое отображение активации продувки
    ctrl_CR600.rail_dir = 0;
    recycle_state = RECYCL_ON;
    break;
  case 1:
    cr_do.air_dry = 1; // открытие КОМ продувки после полного открытия дозирующего
    ctrl_CR600.rail_dir = DIRL | DIRC | DIRR | RAIL1;
    recycle_state = RECYCL_OFF;
    break;
  case 2:
    ctrl_CR600.rail_dir = DIRL | DIRC | DIRR | RAIL2;
    recycle_state = RECYCL_OFF;
    break;
  case 3:
    ctrl_CR600.rail_dir = DIRL | DIRC | DIRR | RAIL3;
    recycle_state = RECYCL_OFF;
    break;
  case 4:
    ctrl_CR600.rail_dir = DIRL | RAIL1;
    recycle_state = RECYCL_OFF;
    break;
  case 5:
    ctrl_CR600.rail_dir = DIRC | RAIL1;
    recycle_state = RECYCL_OFF;
    break;
  case 6:
    ctrl_CR600.rail_dir = DIRR | RAIL1;
    recycle_state = RECYCL_OFF;
    break;
  case 7:
    ctrl_CR600.rail_dir = DIRL | RAIL2;
    recycle_state = RECYCL_OFF;
    break;
  case 8:
    ctrl_CR600.rail_dir = DIRC | RAIL2;
    recycle_state = RECYCL_OFF;
    break;
  case 9:
    ctrl_CR600.rail_dir = DIRR | RAIL2;
    recycle_state = RECYCL_OFF;
    break;
  case 10:
    ctrl_CR600.rail_dir = DIRL | RAIL3;
    recycle_state = RECYCL_OFF;
    break;
  case 11:
    ctrl_CR600.rail_dir = DIRC | RAIL3;
    recycle_state = RECYCL_OFF;
    break;
  case 12:
    ctrl_CR600.rail_dir = DIRR | RAIL3;
    recycle_state = RECYCL_OFF;
    break;
  default:
    stateRailAirDry = 0;
    tmAirDry = 0;
    cr_do.air_dry = 0;
    startRailOpen = 0;
    ctrl_CR600.md = MD_WRK;
    SetVarIndexed(IDX_AIRDRY, 0); //графическое отображение деактивации продувки
    break;
  }
}

void AirDryPump(void) {
  /* актуальный статус продувки
  0 - закрыть рециркуляцию и ждать пока полностью откроется дозирующий,
  1 - открыть первую рейку и КОМ,
  2 - открыть вторую рейку,
  3 - открыть третью рейку,
  4 - открыть рециркуляцию,
  в противном случае выход из продувки*/

  static int stateRailAirDryPump = 0;
  tControl * ctrl_ = CtrlGet();

  if (GetMSTick() >= TMT_AIR_DRY_PUMP + tmAirDry && startRailOpen == 4) {
    stateRailAirDryPump++;
    tmAirDry = GetMSTick();
  }
  if (ctrl_ -> sens.DI.pump_airdry == 1 && Out_ == 1 && startRailOpen != 0) { // выключение продувки при повторном нажатии кнопки 4 и если было начало открытия клапанов
    stateRailAirDryPump = 3;
  }

  if (ctrl_ -> sens.DI.pump_airdry == 0) {
    Out_ = 1;
  }

  DispensControl();

  switch (stateRailAirDryPump) {
  case 0:
    SetVarIndexed(IDX_AIRDRY, 1); // графическое отображение активации продувки
    recycle_state = RECYCL_OFF;
    ctrl_CR600.rail_dir = 0;
    break;
  case 1:
    cr_do.air_dry = 1; //открытие КОМ продувки после полного открытия дозирующего
    recycle_state = RECYCL_ON;
    ctrl_CR600.rail_dir = 0;
    break;
  default:
    stateRailAirDryPump = 0;
    tmAirDry = 0;
    cr_do.air_dry = 0;
    startRailOpen = 0;
    ctrl_CR600.md = MD_WRK;
    SetVarIndexed(IDX_AIRDRY, 0); //графическое отображение деактивации продувки
    break;
  }
}

void AirDry_Cycle(void) {
  if (ctrl_CR600.md == MD_AIRDRY) {
    AirDry();
  } else if (ctrl_CR600.md == MD_AIRDRY_PUMP) {
    AirDryPump();
  }
  RailOpen_Close(1);
}

int LiqPressureCheck(tControl * ctrl_) {
  if (GetVarIndexed(IDX_SW_HUMID_SENS)) {
    static int flag = 0;

    //при flag = 2 уже известно, что воды не осталось, поэтому блокируем управление
    if (flag == 2) {
      ctrl_ -> cmd_no_key.E_S = 0;
      flag = 0; // Сбрасываем флаг, чтобы не блокировать воду полностью
      return 0;
    }
    //если на датчике мало давления и не выжато сцепление
    if (GSeMCM_GetAnalogIn(MCM250_5, 3) < 440 && ctrl_ -> sens.DI.ClutchSensor) {
      if (flag) {
        //если мало давления, машина едет, пытается работать и распылять
        //и вышло время, все ломаем
        if (TopGetSpeed() && ctrl_ -> cmd_no_key.E_S && GetVarIndexed(IDX_DENSITY)) {
          if (IsTimerOff(PRESSURE_TIMER)) {
            ctrl_ -> cmd_no_key.E_S = 0;
            MsgBoxOk(RCTextGetText(RCTEXT_T_WARNING, GetVarIndexed(IDX_SYS_LANGUAGE)),
              RCTextGetText(RCTEXT_T_WATERSENSORNOTAVAILABLE, GetVarIndexed(IDX_SYS_LANGUAGE)),
              NULL, NULL);
            flag = 2;
          }
        } else { //если мало давления, но машина или не едет, или не пытается работать/распылять
          flag = 0;
        }
      } else {
        //если мало давления, машина в движении, в режиме работы и не поднят флаг
        //устанавливаем таймер и поднимаем флаг
        if (TopGetSpeed() > 0 && ctrl_ -> cmd_no_key.E_S && !flag && GetVarIndexed(IDX_DENSITY)) {
          SetTimer(PRESSURE_TIMER, PRESSURE_TIMER_MS);
          flag = 1;
        }
      }
    } else { //если есть давление или выжато сцепление, опускаем флаг
      flag = 0;
    }
  }

  return ctrl_ -> cmd_no_key.E_S || hoseWorking || MF500Bar_IsOn( & ctrl_ -> top.SummerTop.MF500Bar);
}

void SendToMCM(void) {
  for (uint32_t i = 0; i < GS_ARRAYELEMENTS(CR600Valve); i++) {
    if (GetVar(HDL_IO_MODECTRL)) {
      // Пустое условие необходимо для того, чтобы в ручном режиме
      // не отправлялись сигналы на МСМ.
    } else if (GetVarIndexed(IDX_OILLEVEL) > 0 && !GetVarIndexed(IDX_EMCY_ON)) {
      GSeMCM_SetDigitalOut(CR600Valve[i].Device, CR600Valve[i].idx, * CR600Valve[i].DO);
    } else {
      GSeMCM_SetDigitalOut(CR600Valve[i].Device, CR600Valve[i].idx, 0);
    }
  }
}

int TimerCheck(int timerHDL, int timeout) {
  if (IsTimerOff(timerHDL)) {
    SetTimer(timerHDL, timeout);
    return 1;
  }

  return 0;
}

int CheckLiterOnSpeed(int step) {
  double Revert_Line = 1.;
  int CountLine = GetVarIndexed(IDX_ROADLINEL) + GetVarIndexed(IDX_ROADLINEM) + GetVarIndexed(IDX_ROADLINER);
  Revert_Line = 3. / CountLine;
  int speed = TopGetSpeed() / step;
  double SpeedMPS = speed * step / 3.6;
  double CountML = GetVar(HDL_DENSITY) * 2.8 * CountLine;
  int32_t CurrentLiterPerMin = SpeedMPS * CountML / 1000 * 60. * Revert_Line;
  return CurrentLiterPerMin > MAX_LITER_PER_MIN ? MAX_LITER_PER_MIN : CurrentLiterPerMin;
}

void HoseModeSwitch(void) {
  hoseWorking ^= 1; // Переключатель режима работы шланга
  SetVarIndexed(IDX_DENSITY, 0); // Обнуление объемов прокачиваемой через насос жидкости при переключении режимов
  SetVarIndexed(IDX_PROFILEICON_4, hoseWorking); // Переключение отрисовки кнопки шланга

  // Переключение отображений г/м2 -- литр/мин
  SetVarIndexed(IDX_GRAMMPERMETER_TEXTATTR, GetVarIndexed(IDX_GRAMMPERMETER_TEXTATTR) ^ ATTR_VISIBLE);
  SetVarIndexed(IDX_LITERPERMIN_TEXTATTR, GetVarIndexed(IDX_LITERPERMIN_TEXTATTR) ^ ATTR_VISIBLE);

  // Обнуление графики сеторов распределения, если включили шланг
  dispensSectors *= !hoseWorking;
  DrawRoadLines(!hoseWorking);
}

void HoseWorking(void) {
  RailOpen_Close(OFF); // Закрытие реек
  SetDensity(0, 600, 50); // Управление литражем шланга
  dispens.set = CurrentByLiters(GetVarIndexed(IDX_DENSITY)); // Установка объемов полива
}

static int current_pump = 0;
void Ctrl_top_LRS_CR600_300_Init() {

  if (!GetVar(HDL_CURRENT_PUMP)) {
    Ctrl_top_LRS_CR600_Init();
    Ctrl_top_E2000_C610H_Init(CtrlGet());
    current_pump = 0;
  } else {
    Ctrl_top_LRS_CR600_Init();
    Ctrl_top_E2000_BP300_Init(CtrlGet());
    current_pump = 1;
  }
}

void Ctrl_top_CR600_BP300(uint32_t evtc, tUserCEvt * evtv) {
  if (!current_pump) {
    Ctrl_top_LRS_CR600_Cycle(evtc, evtv);
  } else {
    Ctrl_top_E2000_BP300_Cycle(CtrlGet(), evtc, evtv);
  }
}