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
#include	"usr_tools.h"
#include	"ctrl_top_attachment_cr.h"
#include	"ctrl_top_attachment_cr600.h"
#include "ctrl_top_summer_attachment.h"
#include "calibr_omfb.h"

#define FN_CONV_TBL		"/gs/data/uh_conv_tbl.cfg"
#define FN_CONV_TBL_USB	"/gs/usb/uh_conv_tbl.cfg"
#define		SPEED_LIM_CR	(60.1*1000/3600)

CR_VALVE_t crValve;

typedef struct tagCRValve {
	uint8_t* DO;
	uint8_t Device;
	uint8_t idx;
} tCRValve;

tCRValve CRValve[] = {
	{.DO = &crValve.valv_oil,			.Device = MCM250_5,		.idx = 1},
	{.DO = &crValve.valv_l,				.Device = MCM250_5,		.idx = 3},
	{.DO = &crValve.valv_c,				.Device = MCM250_5,		.idx = 5},
	{.DO = &crValve.valv_r,				.Device = MCM250_5,		.idx = 7},
	{.DO = &crValve.recycl,				.Device = MCM250_6,		.idx = 0},
	{.DO = &crValve.rail1,				.Device = MCM250_6,		.idx = 2},
	{.DO = &crValve.rail2,				.Device = MCM250_6,		.idx = 4},
	{.DO = &crValve.rail3,				.Device = MCM250_6,		.idx = 6},
	{.DO = &crValve.dispens_on,		.Device = MCM250_6,		.idx = 1},
	{.DO = &crValve.dispens_off,	.Device = MCM250_6,		.idx = 3},
};

enum CR_CONST {
#define	FN_CR_TBL		"/gs/data/cr_table_liq.cfg"
#define	FN_CR_TBL_USB	"/gs/usb/cr_table_liq.cfg"
	CR_NUM_RANGE = 6,
	CR_NUM_W = 3,
	NX_CR = 32,
	NY_CR = CR_NUM_RANGE * 2 * CR_NUM_W,
	STEP_DENS = 5,
	TMT_CLBR_POS = 8000,
	TMT_WAIT_LIQ = 3000,
	TMT_WATER_ERROR = 8000,
	TMT_CHG_SPEED = 1000,
	NUM_STEPS_CLBR = 25,
	RAIL1 = 0x01,
	RAIL2 = 0x02,
	RAIL3 = 0x04,
	DIRL = 0x10,
	DIRC = 0x20,
	DIRR = 0x40,
	VOL_STEPS = 20,
	VOL_REV = 200,
	VOL_REV_UP = 210,
	VOL_REV_DN = 190,
	DISPENS_MIN = 200,
	DISPENS_MAX = 900,
	DISPENS_PREC = 50,
	DISPENS_ACCU = 25,
	CHG_BOOST = 0x01,
	CHG_DENS = 0x02,
	CHG_WIDHT = 0x04,
	CHG_ROLL = 0x08,
	CHG_SPEED = 0x10,
	CHG_DISPENS = 0x20,
	RECYCL_OFF = 0,
	RECYCL_ON = 1,
};


typedef struct tCR_tbl_dispens {
	float		pot[NX_CR];
	float		vol[NX_CR];
} CR_tbl_dispans;


int32_t f_adrv_calibr = 0; // флаг включения калибровки 1 - вкл; 0 - выкл
int8_t			flagCRDump;
CR_ctrl_t			ctrl_CR;
CR_dispens_pos_t	dispens;

const int		lvl_rails[CR_NUM_RANGE] = { RAIL1,  RAIL2,  RAIL3,  RAIL1 | RAIL3,  RAIL2 | RAIL3,  RAIL1 | RAIL2 | RAIL3 };
const float		widths[3] = { 3.5,  7.0, 10.5 };

static uint32_t			tmt_sens_liq;
static int32_t			prec_disp = DISPENS_PREC;
static uint32_t			clbr_set_up[NUM_STEPS_CLBR + 1];
static uint32_t			clbr_pot_up[NUM_STEPS_CLBR + 1];
static uint32_t			clbr_vol_up[NUM_STEPS_CLBR + 1];
static uint32_t			clbr_set_dn[NUM_STEPS_CLBR + 1];
static uint32_t			clbr_pot_dn[NUM_STEPS_CLBR + 1];
static uint32_t			clbr_vol_dn[NUM_STEPS_CLBR + 1];

static CR_tbl_dispans	dispens1[CR_NUM_RANGE] = {
	{	{163,	166,	168,	170,	175,	180,	190,	200,	220,	250,	300},
		{0.0,	8.1,	10.2,	11.8,	17.3,	24.9,	37.6,	47.2,	55.2,	59.1,	59.8}	},
	{	{204,	210,	220,	230,	240,	250,	260,	270,	285,	300},
		{59.8,	67.2,	72.5,	78.6,	82.4,	83.1,	84.3,	84.9,	85.4,	85.9}	},
	{	{224,	225,	230,	235,	240,	250,	260,	270,	300,	350},
		{85.9,	86.3,	91.8,	93.3,	95.5,	98.3,	100.5,	103.0,	104.6,	105.6}	},
	{	{223,	225,	230,	235,	245,	255,	265,	280,	300,	350},
		{105.6,	108.5,	115.4,	121.8,	130.3,	134.9,	140.6,	145.8,	151.4,	156.9}	},
	{	{244,	250,	260,	270,	280,	285,	290,	300,	350,	400},
		{156.9,	163.2,	172.6,	180.2,	186.2,	187.2,	187.5,	191.5,	201.0,	204.2}	},
	{	{263,	265,	270,	280,	290,	300,	310,	330,	350,	400},
		{204.2,	209.3,	218.2,	222.3,	235.4,	240.8,	244.6,	248.5,	253.1,	259.2}	}
};
static CR_tbl_dispans	dispens2[CR_NUM_RANGE] = {
	{	{169,	172,	175,	180,	190,	200,	210,	220,	240,	260,	300},
		{0.0,	15.2,	19.4,	28.0,	40.3,	57.7,	73.8,	86.8,	99.7,	105.2,	109.4}	},
	{	{226,	230,	235,	240,	250,	260,	280,	300,	320,	350},
		{109.4,	113.8,	120.0,	123.9,	131.6,	137.1,	144.9,	147.7,	148.5,	152.2}	},
	{	{269,	270,	275,	280,	285,	290,	300,	320,	350},
		{152.2,	152.5,	154.0,	155.5,	159.5,	160.8,	164.6,	168.5,	172.8}	},
	{	{240,	245,	250,	260,	280,	300,	330,	350,	400,	450},
		{172.8,	184.2,	199.4,	211.0,	245.3,	258.9,	193.5,	276.9,	285.4,	290.0}	},
	{	{302,	305,	310,	315,	320,	330,	340,	360,	400,	500},
		{290.0,	290.8,	298.5,	304.1,	312.9,	316.3,	318.7,	323.7,	327.8,	335.6}	},
	{	{301,	305,	310,	330,	350,	400,	450,	500,	600,	650},
		{335.6,	345.8,	348.0,	363.3,	372.5,	389.4,	405.9,	412.8,	422.3,	427.2}	}
};
static CR_tbl_dispans	dispens3[CR_NUM_RANGE] = {
	{	{177,	180,	185,	190,	195,	200,	210,	220,	240,	260,	300},
		{0.0,	23.9,	32.5,	42.0,	51.7,	62.3,	80.6,	100.0,	121.8,	134.0,	146.5}	},
	{	{242,	245,	250,	260,	270,	280,	300,	320,	350,	400},
		{146.5,	150.9,	156.7,	165.5,	172.8,	179.1,	189.2,	194.2,	196.6,	205.8}	},
	{	{269,	270,	275,	280,	295,	315,	330,	350,	400,	450},
		{205.8,	206.5,	212.7,	217.3,	228.7,	236.8,	242.5,	245.6,	248.9,	256.4}	},
	{	{266,	270,	280,	290,	320,	350,	400,	450,	500,	550},
		{256.4,	268.8,	289.1,	297.4,	325.1,	343.8,	352.2,	365.5,	372.2,	376.6}	},
	{	{341,	350,	360,	380,	400,	450,	500,	550,	600},
		{376.6,	382.9,	389.8,	396.5,	409.8,	417.5,	430.6,	434.8,	440.3}	},
	{	{350,	370,	390,	410,	430,	450,	500,	550,	600,	650},
		{440.3,	455.8,	473.5,	476.9,	489.3,	491.5,	498.5,	522.5,	530.5,	530.8}	}
};
static float		koef_multi = 0.0;


void Ctrl_CR_Dump( void );

void Ctrl_top_LRS_CR_Init(tControl * ctrl_) {
  //Pump switch on/off by long press E
  WaterPump_Init( & ctrl_ -> top.SummerTop, WATERPUMP_TYPE_C610H);
  WaterPump_SetMaxLiter( 65000 );
  SendToVisuObj(OBJ_WATERPUMP, GS_TO_VISU_SET_ATTR_VISIBLE, 1);
  //@@@ Spreader settings
  SendToVisuObj(OBJ_IMPORT_TABLE, GS_TO_VISU_SET_ATTR_VISIBLE, 1);
  SendToVisuObj(OBJ_ATRESTTTEXT98, GS_TO_VISU_SET_ATTR_VISIBLE, 1);
  SendToVisuObj(OBJ_TRUETYPEVARIABLE100, GS_TO_VISU_SET_ATTR_VISIBLE, 1);

  crValve.valv_l = crValve.valv_c = crValve.valv_r = 0;
  crValve.recycl = crValve.rail1 = crValve.rail2 = crValve.rail3 = 0;
  crValve.dispens_on = crValve.dispens_off = 0;
  memset( & ctrl_CR, 0, sizeof(ctrl_CR));
  {
    float ** tbl_ = new_arr_float(NX_CR, NY_CR);
    int * len_ = calloc(NY_CR, sizeof(int));
    //SetVarIndexed( IDX_CR_GET_CFG, 99 );
    if (0 == rd_file_txt_float(FN_CR_TBL, tbl_, len_, NX_CR, NY_CR)) {
      memset( & dispens1, 0, sizeof(dispens1));
      memset( & dispens2, 0, sizeof(dispens2));
      memset( & dispens3, 0, sizeof(dispens3));
      for (int y_ = 0; y_ < CR_NUM_RANGE; ++y_) {
        PrintToDebug("----CR-:good init");
        for (int x_ = 0; x_ < len_[y_ * 2]; ++x_) {
          dispens1[y_].pot[x_] = tbl_[y_ * 2][x_];
          dispens1[y_].vol[x_] = tbl_[y_ * 2 + 1][x_];
        }
        PrintToDebug("[%f->%f]", dispens1[y_].pot[len_[y_] - 1], dispens1[y_].vol[len_[y_] - 1]);
        for (int x_ = 0; x_ < len_[CR_NUM_RANGE * 2 + y_ * 2]; ++x_) {
          dispens2[y_].pot[x_] = tbl_[CR_NUM_RANGE * 2 + y_ * 2][x_];
          dispens2[y_].vol[x_] = tbl_[CR_NUM_RANGE * 2 + y_ * 2 + 1][x_];
        }
        PrintToDebug("[%f->%f]", dispens2[y_].pot[len_[y_] - 1], dispens2[y_].vol[len_[y_] - 1]);
        for (int x_ = 0; x_ < len_[y_ * 2 + CR_NUM_RANGE * 4]; ++x_) {
          dispens3[y_].pot[x_] = tbl_[CR_NUM_RANGE * 4 + y_ * 2][x_];
          dispens3[y_].vol[x_] = tbl_[CR_NUM_RANGE * 4 + y_ * 2 + 1][x_];
        }
        PrintToDebug("[%f->%f]\r\n", dispens3[y_].pot[len_[y_] - 1], dispens3[y_].vol[len_[y_] - 1]);
      }
    } else {
      PrintToDebug("----CR-:no init\r\n");
    }
    free(len_);
    del_arr_float(tbl_, NY_CR);
  }
  ctrl_CR.md = MD_INI;
  dispens.min = DISPENS_MIN;
  dispens.max = DISPENS_MAX;
  for (int y_ = 0; y_ < CR_NUM_RANGE; ++y_) {
    for (int x_ = 0; x_ < NX_CR; ++x_) {
      if (0 < dispens1[y_].pot[x_]) {
        dispens.vol_max[0] = maxf(dispens.vol_max[0], dispens1[y_].vol[x_]);
      }
      if (0 < dispens2[y_].pot[x_]) {
        dispens.vol_max[1] = maxf(dispens.vol_max[1], dispens2[y_].vol[x_]);
      }
      if (0 < dispens3[y_].pot[x_]) {
        dispens.vol_max[2] = maxf(dispens.vol_max[2], dispens3[y_].vol[x_]);
      }
    }
  }
  dispens.dens_max = dispens.vol_max[0] * 1000 * 3600 / (widths[0] * 30 * 1000. * 60);
  dispens.dens_stp = dispens.dens_max / VOL_STEPS;
  IOT_NormAI_Init( & ctrl_ -> sens.AI.cr_liqmeter, 400, 2000, 0, 700, 0, 0);
  koef_multi = GetVarFloat(HDL_KOEF_MULTI);
  if (koef_multi <= 0.0) {
    koef_multi = 1.0;
    SetVarFloat(HDL_KOEF_MULTI, koef_multi);
  }
  ctrl_CR.vol_rev = VOL_REV;
  SetVar(HDL_MCM250_6_INIT, 1);
}

int32_t			Import_table_cr = 0;
void Ctrl_top_LRS_CR_Cycle(tControl * ctrl_, uint32_t evtc, tUserCEvt * evtv) {
  static uint32_t _dbg_vw_;
  static uint8_t cr_clb = 0;
  static uint16_t cr_pot_min = 165;
  static uint16_t cr_pot_max = 650;
  uint32_t Password = (uint32_t) GetVarIndexed(IDX_PASSWORD_ENTER);
  if ((Password == (uint32_t) GetVar(HDL_PASSWORD_MERKATOR) || Password == 32546) && cr_clb == 0) {
    SendToVisuObj(OBJ_BTN_CR_CALIBR, GS_TO_VISU_SET_ATTR_VISIBLE, 1);
    cr_clb = 1;
  }
  koef_multi = GetVarFloat(HDL_KOEF_MULTI);
  {
    if (0 < cr_pot_min) {
      dispens.min = cr_pot_min;
    }
    if (0 < cr_pot_max) {
      dispens.max = cr_pot_max;
    }
  }
  if (Import_table_cr) {
    Import_table_cr = 0;
    float ** tbl_ = new_arr_float(NX_CR, NY_CR);
    int * len_ = calloc(NY_CR, sizeof(int));
    if (0 == rd_file_txt_float(FN_CR_TBL_USB, tbl_, len_, NX_CR, NY_CR)) {
      memset( & dispens1, 0, sizeof(dispens1));
      memset( & dispens2, 0, sizeof(dispens2));
      memset( & dispens3, 0, sizeof(dispens3));
      for (int y_ = 0; y_ < CR_NUM_RANGE; ++y_) {
        for (int x_ = 0; x_ < len_[y_ * 2]; ++x_) {
          dispens1[y_].pot[x_] = tbl_[y_ * 2][x_];
          dispens1[y_].vol[x_] = tbl_[y_ * 2 + 1][x_];
        }
        for (int x_ = 0; x_ < len_[CR_NUM_RANGE * 2 + y_ * 2]; ++x_) {
          dispens2[y_].pot[x_] = tbl_[CR_NUM_RANGE * 2 + y_ * 2][x_];
          dispens2[y_].vol[x_] = tbl_[CR_NUM_RANGE * 2 + y_ * 2 + 1][x_];
        }
        for (int x_ = 0; x_ < len_[y_ * 2 + CR_NUM_RANGE * 4]; ++x_) {
          dispens3[y_].pot[x_] = tbl_[CR_NUM_RANGE * 4 + y_ * 2][x_];
          dispens3[y_].vol[x_] = tbl_[CR_NUM_RANGE * 4 + y_ * 2 + 1][x_];
        }
        PrintToDebug("----CR-:[%d][%d][%d]\r\n", len_[y_ * 2], len_[CR_NUM_RANGE * 2 + y_ * 2], len_[CR_NUM_RANGE * 4 + y_ * 2]);
      }
      FileUnlink(FN_CR_TBL);
      CopyFile(FN_CR_TBL_USB, FN_CR_TBL, NULL);
      PrintToDebug("----CR-:cycle ok read usb\r\n");
      MsgBoxOk(RCTextGetText(RCTEXT_T_WARNING, GetVarIndexed(IDX_SYS_LANGUAGE)), RCTextGetText(RCTEXT_T_COPYFROMUSBSUCCESS, GetVarIndexed(IDX_SYS_LANGUAGE)), NULL, NULL);
    } else {
      PrintToDebug("----CR-:cycle error read usb\r\n");
      MsgBoxOk(RCTextGetText(RCTEXT_T_WARNING, GetVarIndexed(IDX_SYS_LANGUAGE)), RCTextGetText(RCTEXT_T_COPYFROMUSBERROR, GetVarIndexed(IDX_SYS_LANGUAGE)), NULL, NULL);
    }
    char fn_[1024];
    strcpy(fn_, FN_CR_TBL_USB ".res");
    FileUnlink(fn_);
    tGsFile * fl_ = FileOpen(fn_, "w");
    for (int y_ = 0; y_ < CR_NUM_RANGE; ++y_) {
      for (int x_ = 0; x_ < NX_CR; ++x_) {
        FilePrintf(fl_, "%9f ", dispens1[y_].pot[x_]);
      }
      FilePuts("\n", fl_);
      for (int x_ = 0; x_ < NX_CR; ++x_) {
        FilePrintf(fl_, "%9f ", dispens1[y_].vol[x_]);
      }
      FilePuts("\n", fl_);
    }
    for (int y_ = 0; y_ < CR_NUM_RANGE; ++y_) {
      for (int x_ = 0; x_ < NX_CR; ++x_) {
        FilePrintf(fl_, "%9f ", dispens2[y_].pot[x_]);
      }
      FilePuts("\n", fl_);
      for (int x_ = 0; x_ < NX_CR; ++x_) {
        FilePrintf(fl_, "%9f ", dispens2[y_].vol[x_]);
      }
      FilePuts("\n", fl_);
    }
    for (int y_ = 0; y_ < CR_NUM_RANGE; ++y_) {
      for (int x_ = 0; x_ < NX_CR; ++x_) {
        FilePrintf(fl_, "%9f ", dispens3[y_].pot[x_]);
      }
      FilePuts("\n", fl_);
      for (int x_ = 0; x_ < NX_CR; ++x_) {
        FilePrintf(fl_, "%9f ", dispens3[y_].vol[x_]);
      }
      FilePuts("\n", fl_);
    }
    FileClose(fl_);
    free(len_);
    del_arr_float(tbl_, NY_CR);
  }
  Ctrl_CR_Dump();
  Top_Boost_Cycle(&ctrl_->top);
  static uint32_t timer_press = 0;
  if (ctrl_ -> cmd_no_key.E_S) {
    ctrl_CR.md = MD_WRK;
    if (timer_press == 0 && DI_SENSOR_OFF == ctrl_ -> sens.DI.cr_pressure) {
      timer_press = GetMSTick();
    }
    if (TopGetSpeed() >= GetVar(HDL_WORKSPEED)) {
      ctrl_CR.q_valv_oil = ON;
      if(!GetFlagTopCalibr()) {
        Ctrl_Valve_SetLiter( & ctrl_->top.SummerTop.Pump.Valv.Valve.PVEH, 65000);
      }
      SetVarIndexed(IDX_ICON_PUMP, 1);
    } else {
      ctrl_CR.q_valv_oil = OFF;
    }
  } else {
    if (MD_WRK == ctrl_CR.md || MD_DUMP == ctrl_CR.md) {
      ctrl_CR.md = MD_CLR;
    }
    timer_press = 0;
    ctrl_CR.q_valv_oil = OFF;
    ctrl_CR.rail_dir = 0;
    crValve.recycl = RECYCL_ON;
		Ctrl_Valve_SetLiter( & ctrl_->top.SummerTop.Pump.Valv.Valve.PVEH, 0);
    SetVarIndexed(IDX_ICON_PUMP, 0);
  } {
    if (0 == GetVar(HDL_SW_HUMID_SENS)) {
      ctrl_ -> sens.DI.cr_pressure = DI_SENSOR_INACTIVE;
    } else if (DI_SENSOR_INACTIVE == ctrl_ -> sens.DI.cr_pressure) {
      ctrl_ -> sens.DI.cr_pressure = DI_SENSOR_UNKNOWN;
    }
    if (DI_SENSOR_ON == ctrl_ -> sens.DI.cr_pressure) {
      ctrl_CR.press_liq = ON;
      timer_press = 0;
    } else if (DI_SENSOR_OFF == ctrl_ -> sens.DI.cr_pressure) {
      ctrl_CR.press_liq = OFF;
    } else {
      ctrl_CR.press_liq = UNK;
      timer_press = 0;
    }
    if (MD_WRK == ctrl_CR.md && (OFF == ctrl_CR.press_liq && GetMSTick() >= timer_press + TMT_WATER_ERROR) && TopGetSpeed() >= GetVar(HDL_WORKSPEED)) {
      ctrl_ -> cmd_no_key.E_S = 0;
      MsgBoxOk(RCTextGetText(RCTEXT_T_WARNING, GetVarIndexed(IDX_SYS_LANGUAGE)), RCTextGetText(RCTEXT_T_WATERSENSORNOTAVAILABLE, GetVarIndexed(IDX_SYS_LANGUAGE)), NULL, NULL);
    }
  }
  if (MD_SET == ctrl_CR.md) {
    if (tmt_sens_liq < GetMSTick() || ON == ctrl_CR.press_liq) {
      ctrl_CR.md = MD_WRK;
      dispens.set = dispens.min;
      ctrl_CR.chg |= CHG_DENS | CHG_WIDHT | CHG_ROLL;
    }
  }
  static uint8_t flagInfoBoxDump = 0;
  if (MD_WRK == ctrl_CR.md && ON == flagCRDump) {
    if (0 == TopGetSpeed()) {
      if (!flagInfoBoxDump) {
        MsgBoxOk(RCTextGetText(RCTEXT_T_INFO, GetVarIndexed(IDX_SYS_LANGUAGE)),
          RCTextGetText(RCTEXT_T_CR_DUMP, GetVarIndexed(IDX_SYS_LANGUAGE)), NULL, NULL);
        flagInfoBoxDump = 1;
      }
      ctrl_CR.md = MD_DUMP;
    } else {
      flagCRDump = OFF;
    }
  }
  if (OFF == flagCRDump) {
    flagInfoBoxDump = 0;
  }
  if (MD_DUMP == ctrl_CR.md) {
    if (0 < TopGetSpeed()) {
      flagCRDump = OFF;
    }
    if (OFF == flagCRDump) {
      ctrl_CR.md = MD_INI;
    }
  } {
    if( ctrl_->top.BoostOn == 1 ) {
			ctrl_CR.boost = ON;
			ctrl_CR.chg |= CHG_BOOST;
		} else {
			if( ON == ctrl_CR.boost ) {
				ctrl_CR.boost = OFF;
				ctrl_CR.chg |= CHG_BOOST;
			}
		}

    if (ctrl_ -> cmd_no_key.X_Change) {
      ctrl_CR.density += ctrl_ -> cmd_no_key.X_Change * dispens.dens_stp;
      ctrl_CR.chg |= CHG_DENS;
    }
    ctrl_CR.density = (0 < ctrl_CR.density) ? ((ctrl_CR.density < dispens.dens_max) ? ctrl_CR.density : dispens.dens_max) : 0;
    if (ctrl_ -> cmd_no_key.Y_Change) {
      ctrl_CR.width += ctrl_ -> cmd_no_key.Y_Change;
      ctrl_CR.chg |= CHG_WIDHT;
    }
    if (ctrl_CR.width < 1) {
      ctrl_CR.width = 1;
    } else if (3 < ctrl_CR.width) {
      ctrl_CR.width = 3;
    }
    if (ctrl_ -> cmd_no_key.Z_Change) {
      ctrl_CR.roll -= ctrl_ -> cmd_no_key.Z_Change * ctrl_CR.width;
      ctrl_CR.chg |= CHG_ROLL;
    }
    if (ctrl_CR.roll < -1) {
      ctrl_CR.roll = -1;
    } else if (1 < ctrl_CR.roll) {
      ctrl_CR.roll = 1;
    }
    static int16_t _speed_old_ = 0;
    static uint32_t _tmt_speed_ = 0;
    static uint32_t _tmt_dispens_ = 0;
    if (_tmt_speed_ < GetMSTick()) {
      if (_speed_old_ != ctrl_ -> engine.speed) {
        ctrl_CR.chg |= CHG_SPEED;
        _speed_old_ = ctrl_ -> engine.speed;
      }
      _tmt_speed_ = GetMSTick() + TMT_CHG_SPEED;
    }
    if (_tmt_dispens_ < GetMSTick()) {
      if (DISPENS_ACCU < absint(dispens.cur - dispens.set)) {
        ctrl_CR.chg |= CHG_DISPENS;
      }
      _tmt_dispens_ = GetMSTick() + TMT_CHG_SPEED;
    }
  }
  dispens.cur = ctrl_ -> sens.AI.cr_dispens_pos.value;

  static float _vol_ = 0.;
  float dens_ = ctrl_CR.density;
  if (1 == f_adrv_calibr) {
    enum {
      GO_BEG,
      GO_OPEN,
      GO_CLOSE,
      GO_CYCL_WAIT,
      GO_CYCL_GET,
      GO_CYCL_RET,
    };
    static int _stt_calibr_ = UNK;
    static uint8_t _q_step_ = GO_BEG;
    static uint32_t _tm_;
    static int8_t _lvl_;
    static int8_t _idx_disp_;
    static float _step_disp_;
    static tGsFile * _fl_;
    static int _dir_rev_;

    if (MD_WRK != ctrl_CR.md) {
      _stt_calibr_ = OFF;
      _q_step_ = GO_BEG;
    } else {
      switch (_q_step_) {
      case GO_BEG: {
        ctrl_CR.q_valv_oil = OFF;
        _fl_ = FileOpen("/gs/usb/cr_clbr.txt", "w");
        _stt_calibr_ = UNK;
        _tm_ = GetMSTick() + TMT_CLBR_POS;
        _q_step_ = GO_OPEN;
      }
      break;
      case GO_OPEN: {
        if (_tm_ < GetMSTick()) {
          dispens.max = ctrl_ -> sens.AI.cr_dispens_pos.value;
          _tm_ = GetMSTick() + TMT_CLBR_POS;
          _q_step_ = GO_CLOSE;
        } else {
          ctrl_CR.dir_disp = 1;
        }
      }
      break;
      case GO_CLOSE: {
        if (_tm_ < GetMSTick()) {
          _lvl_ = 0;
          _idx_disp_ = 0;
          _dir_rev_ = 1;
          dispens.min = ctrl_ -> sens.AI.cr_dispens_pos.value;
          dispens.min += (dispens.max - dispens.min) / 10;
          dispens.max -= (dispens.max - dispens.min) / 10;
          dispens.set = dispens.min;
          _step_disp_ = (dispens.max - dispens.min) / NUM_STEPS_CLBR;

          if (DISPENS_PREC * 2 < _step_disp_) {
            prec_disp = DISPENS_PREC;
          } else {
            prec_disp = _step_disp_ / 2;
          }
          ctrl_CR.dir_disp = 0;
          ctrl_CR.width = 1;
          ctrl_CR.roll = 0;
          ctrl_CR.rail_dir &= ~(DIRL | DIRC | DIRR);
          ctrl_CR.rail_dir |= DIRC;
          ctrl_CR.rail_dir &= ~(RAIL1 | RAIL2 | RAIL3);
          ctrl_CR.rail_dir |= lvl_rails[_lvl_];
          _q_step_ = GO_CYCL_WAIT;
        } else {
          ctrl_CR.dir_disp = -1;
        }
      }
      break;
      case GO_CYCL_WAIT: {
        if (absint(dispens.set - dispens.cur) <= prec_disp) {
          ctrl_CR.dir_disp = 0;
          ctrl_CR.q_valv_oil = ON;
          if (dispens.set < 700) {
            _tm_ = GetMSTick() + TMT_WAIT_LIQ * 2;
          } else {
            _tm_ = GetMSTick() + TMT_WAIT_LIQ;
          }
          _q_step_ = GO_CYCL_GET;
        } else {
          if (dispens.set < dispens.cur) {
            ctrl_CR.dir_disp = -1;
          } else {
            ctrl_CR.dir_disp = 1;
          }
        }
      }
      break;
      case GO_CYCL_GET: {
        if (_tm_ < GetMSTick()) {
          if (1 == _dir_rev_) {
            clbr_set_up[_idx_disp_] = dispens.set;
            clbr_pot_up[_idx_disp_] = dispens.cur;
            clbr_vol_up[_idx_disp_] = IOT_NormAI( & ctrl_ -> sens.AI.cr_liqmeter);
            if (RECYCL_OFF == crValve.recycl) {
              _dir_rev_ = -1;
              dispens.set = dispens.max;
              _idx_disp_ = NUM_STEPS_CLBR;
            } else {
              if (++_idx_disp_ <= NUM_STEPS_CLBR) {
                dispens.set += _step_disp_;
                ctrl_CR.dir_disp = 1;
                _q_step_ = GO_CYCL_WAIT;
              } else {
                _q_step_ = GO_CYCL_RET;
              }
            }
          } else if (-1 == _dir_rev_) {
            clbr_set_dn[_idx_disp_] = dispens.set;
            clbr_pot_dn[_idx_disp_] = dispens.cur;
            clbr_vol_dn[_idx_disp_] = IOT_NormAI( & ctrl_ -> sens.AI.cr_liqmeter);
            if (RECYCL_ON == crValve.recycl) {
              _q_step_ = GO_CYCL_RET;
            } else {
              if (0 <= --_idx_disp_) {
                dispens.set -= _step_disp_;
                ctrl_CR.dir_disp = -1;
                _q_step_ = GO_CYCL_WAIT;
              } else {
                _q_step_ = GO_CYCL_RET;
              }
            }
          } else {
            ctrl_CR.dir_disp = 0;
            _stt_calibr_ = OFF;
            _q_step_ = GO_BEG;
          }
        }
      }
      break;
      case GO_CYCL_RET: {
        ctrl_CR.dir_disp = 0;
        for (int x_ = 0; x_ <= NUM_STEPS_CLBR; ++x_) {
          FilePrintf(_fl_, "%5d", clbr_set_up[x_]);
        }
        FilePuts("\n", _fl_);
        for (int x_ = 0; x_ <= NUM_STEPS_CLBR; ++x_) {
          FilePrintf(_fl_, "%5d", clbr_pot_up[x_]);
        }
        FilePuts("\n", _fl_);
        for (int x_ = 0; x_ <= NUM_STEPS_CLBR; ++x_) {
          FilePrintf(_fl_, "%5d", clbr_vol_up[x_]);
        }
        FilePuts("\n\n", _fl_);
        for (int x_ = 0; x_ <= NUM_STEPS_CLBR; ++x_) {
          FilePrintf(_fl_, "%5d", clbr_set_dn[x_]);
        }
        FilePuts("\n", _fl_);
        for (int x_ = 0; x_ <= NUM_STEPS_CLBR; ++x_) {
          FilePrintf(_fl_, "%5d", clbr_pot_dn[x_]);
        }
        FilePuts("\n", _fl_);
        for (int x_ = 0; x_ <= NUM_STEPS_CLBR; ++x_) {
          FilePrintf(_fl_, "%5d", clbr_vol_dn[x_]);
        }
        FilePuts("\n\n", _fl_);
        if (++_lvl_ < CR_NUM_RANGE) {
          ctrl_CR.rail_dir &= ~(RAIL1 | RAIL2 | RAIL3);
          ctrl_CR.rail_dir |= lvl_rails[_lvl_];
          _q_step_ = GO_CYCL_WAIT;
        } else {
          if (++ctrl_CR.width <= 3) {
            _lvl_ = 0;
            ctrl_CR.rail_dir &= ~(DIRL | DIRC | DIRR);
            if (1 <= ctrl_CR.width) {
              ctrl_CR.rail_dir |= DIRC;
            }
            if (2 <= ctrl_CR.width) {
              ctrl_CR.rail_dir |= DIRL;
            }
            if (3 <= ctrl_CR.width) {
              ctrl_CR.rail_dir |= DIRR;
            }
            ctrl_CR.rail_dir &= ~(RAIL1 | RAIL2 | RAIL3);
            ctrl_CR.rail_dir |= lvl_rails[_lvl_];
            _q_step_ = GO_CYCL_WAIT;
          } else {
            _stt_calibr_ = ON;
            _q_step_ = GO_BEG;
          }
        }
        memset(clbr_set_up, 0, sizeof(clbr_set_up));
        memset(clbr_pot_up, 0, sizeof(clbr_pot_up));
        memset(clbr_vol_up, 0, sizeof(clbr_vol_up));
        memset(clbr_set_dn, 0, sizeof(clbr_set_dn));
        memset(clbr_pot_dn, 0, sizeof(clbr_pot_dn));
        memset(clbr_vol_dn, 0, sizeof(clbr_vol_dn));

        _dir_rev_ = 1;
        _idx_disp_ = 0;
        ctrl_CR.dir_disp = -1;
        ctrl_CR.q_valv_oil = OFF;
        dispens.set = dispens.min;
      }
      break;
      default:
        ctrl_CR.dir_disp = 0;
        _stt_calibr_ = OFF;
        _q_step_ = GO_BEG;
      }
    }
    _dbg_vw_ = ctrl_CR.width * 0x100000 + _lvl_ * 0x10000 + _q_step_ * 0x1000 + _idx_disp_ * 0x10;
    _vol_ = ctrl_CR.vol = IOT_NormAI( & ctrl_ -> sens.AI.cr_liqmeter);
    if (OFF == _stt_calibr_) {
      FileClose(_fl_);
      MsgBoxOk(RCTextGetText(RCTEXT_T_WARNING, GetVarIndexed(IDX_SYS_LANGUAGE)), RCTextGetText(RCTEXT_T_ADRV_CALBR_ERR, GetVarIndexed(IDX_SYS_LANGUAGE)), NULL, NULL);
    } else if (ON == _stt_calibr_) {
      FileClose(_fl_);
      MsgBoxOk(RCTextGetText(RCTEXT_T_WARNING, GetVarIndexed(IDX_SYS_LANGUAGE)), RCTextGetText(RCTEXT_T_ADRV_CALBR_OK, GetVarIndexed(IDX_SYS_LANGUAGE)), NULL, NULL);
    }
    if (UNK != _stt_calibr_) {
      f_adrv_calibr = 0;
    }

    if (0 == f_adrv_calibr) {

      for (int y_ = 0; y_ < CR_NUM_RANGE; ++y_) {
        for (int x_ = 0; x_ < NX_CR; ++x_) {
          if (0 < dispens1[y_].pot[x_]) {
            dispens.vol_max[0] = maxf(dispens.vol_max[0], dispens1[y_].vol[x_]);
          }
          if (0 < dispens2[y_].pot[x_]) {
            dispens.vol_max[1] = maxf(dispens.vol_max[1], dispens2[y_].vol[x_]);
          }
          if (0 < dispens3[y_].pot[x_]) {
            dispens.vol_max[2] = maxf(dispens.vol_max[2], dispens3[y_].vol[x_]);
          }
        }
      }
    }
  }

  else if (MD_WRK == ctrl_CR.md) {
    CR_tbl_dispans * tbl_;

    float koef_boost_ = 2.;
    float dens_mat_ = 1000;
    float speed_ = ctrl_ -> engine.speed * 1000. / 3600.;

    if (fabs(dens_mat_) < 0.001) {
      dens_mat_ = 1100;
    }

    dispens.dens_max = dispens.vol_max[ctrl_CR.width - 1] * dens_mat_ * 3600 / (widths[ctrl_CR.width - 1] * 30 * 1000. * 60);
    dispens.dens_stp = dispens.dens_max / VOL_STEPS;
    if (ctrl_CR.chg & CHG_BOOST && ON == ctrl_CR.boost) {
      koef_boost_ = (float) GetVarFloat(HDL_COEFBOOST);
      if (koef_boost_ < 1.) {
        koef_boost_ = 2.;
      }
    } else {
      koef_boost_ = 1.;
    }
    if (ctrl_CR.chg & CHG_ROLL) {}
    if (ctrl_CR.chg & (CHG_DENS | CHG_WIDHT | CHG_SPEED | CHG_DISPENS | CHG_BOOST)) {
      ctrl_CR.vol = ctrl_CR.density * widths[ctrl_CR.width - 1] * speed_ * 60 / (dens_mat_);
      _vol_ = ctrl_CR.vol * koef_boost_;
      if (dispens.vol_max[ctrl_CR.width - 1] < ctrl_CR.vol) {
        ctrl_CR.vol = dispens.vol_max[ctrl_CR.width - 1];
        if (0 < speed_) {
          ctrl_CR.density = ctrl_CR.vol * dens_mat_ / (widths[ctrl_CR.width - 1] * speed_ * 60);
        }
      }
      if (dispens.vol_max[ctrl_CR.width - 1] < _vol_) {
        _vol_ = dispens.vol_max[ctrl_CR.width - 1];
      }
      if ((VOL_REV - 1) < ctrl_CR.vol && ctrl_CR.vol < VOL_REV) {
        ctrl_CR.vol = VOL_REV - 1;
      }
      if (0. == speed_) {
        dens_ = ctrl_CR.density;
      } else {
        dens_ = _vol_ * dens_mat_ / (widths[ctrl_CR.width - 1] * speed_ * 60);
      }
      ctrl_CR.rail_dir &= ~(RAIL1 | RAIL2 | RAIL3);
      switch (ctrl_CR.width) {
      case 1: {
        tbl_ = dispens1;
      }
      break;
      case 2: {
        tbl_ = dispens2;
      }
      break;
      case 3: {
        tbl_ = dispens3;
      }
      break;
      default:
        return;
      }
      int y_ = 0, x_ = 0;
      int q_ = 0;
      for (; y_ < CR_NUM_RANGE && !q_; ++y_) {
        for (x_ = 1; x_ < NX_CR && 0 < tbl_[y_].pot[x_]; ++x_) {
          if (tbl_[y_].vol[x_] < _vol_) {
            continue;
          }
          dispens.set = ceil(liner_interpret2(tbl_[y_].vol[x_ - 1], tbl_[y_].vol[x_], tbl_[y_].pot[x_ - 1], tbl_[y_].pot[x_], _vol_));
          ctrl_CR.rail_dir |= lvl_rails[y_];
          q_ = 1;
          break;
        }
      }
      if (dispens.tmt_set < GetMSTick()) {
        dispens.tmt_set = GetMSTick() + 1000;
      }
    }
    if (-1 == ctrl_CR.roll || 3 == ctrl_CR.width) {
      ctrl_CR.rail_dir |= DIRL;
    } else {
      ctrl_CR.rail_dir &= ~DIRL;
    }
    if (1 == ctrl_CR.roll || 3 == ctrl_CR.width) {
      ctrl_CR.rail_dir |= DIRR;
    } else {
      ctrl_CR.rail_dir &= ~DIRR;
    }
    if (0 == ctrl_CR.roll || 1 < ctrl_CR.width) {
      ctrl_CR.rail_dir |= DIRC;
    } else {
      ctrl_CR.rail_dir &= ~DIRC;
    }
    if (0 == ctrl_CR.roll && 2 == ctrl_CR.width) {
      ctrl_CR.rail_dir |= DIRL;
    }
    if (absint(dispens.set - dispens.cur) <= DISPENS_ACCU) {
      ctrl_CR.dir_disp = 0;
    } else {
      if (dispens.set < dispens.cur) {
        ctrl_CR.dir_disp = -1;
      } else {
        ctrl_CR.dir_disp = 1;
      }
    }
    if (0 < ctrl_CR.dir_disp) {
      if (dispens.set <= dispens.cur) {
        ctrl_CR.dir_disp = 0;
      }
    }
    if (0 > ctrl_CR.dir_disp) {
      if (dispens.cur <= dispens.set) {
        ctrl_CR.dir_disp = 0;
      }
    }
  } else if (MD_CLR == ctrl_CR.md) {
    dispens.set = 0;
    ctrl_CR.dir_disp = -1;
    if (dispens.cur < dispens.min) {
      ctrl_CR.md = MD_INI;
    }
  } else if (MD_DUMP == ctrl_CR.md) {
    ctrl_CR.rail_dir = 0;
    _vol_ = dispens.vol_max[3 - 1];
    dispens.set = DISPENS_MAX;
    if (absint(dispens.set - dispens.cur) <= DISPENS_ACCU) {
      ctrl_CR.dir_disp = 0;
    } else {
      if (dispens.set < dispens.cur) {
        ctrl_CR.dir_disp = -1;
      } else {
        ctrl_CR.dir_disp = 1;
      }
    }
    if (0 < ctrl_CR.dir_disp) {
      if (dispens.set <= dispens.cur) {
        ctrl_CR.dir_disp = 0;
      }
    }
    if (0 > ctrl_CR.dir_disp) {
      if (dispens.cur <= dispens.set) {
        ctrl_CR.dir_disp = 0;
      }
    }
  }
  ctrl_CR.chg = 0;

  if (MD_INI != ctrl_CR.md) {
    if (ON == ctrl_CR.q_valv_oil) {
      crValve.valv_oil = 1;
    } else {
      crValve.valv_oil = 0;
    }

    if (0 < ctrl_CR.dir_disp) {
      crValve.dispens_off = 0;
      crValve.dispens_on = 1;
    }
    if (ctrl_CR.dir_disp < 0) {
      crValve.dispens_on = 0;
      crValve.dispens_off = 1;
    }
    if (0 == ctrl_CR.dir_disp) {
      crValve.dispens_off = 0;
      crValve.dispens_on = 0;
    }

    if (MD_DUMP == ctrl_CR.md) {
      crValve.recycl = RECYCL_OFF;
    }
    else if (_vol_ < VOL_REV && MD_WRK == ctrl_CR.md) {
      crValve.recycl = RECYCL_ON;
    }
    else {
      crValve.recycl = RECYCL_OFF;
    }
    crValve.valv_l = ctrl_CR.rail_dir & DIRL ? 1 : 0;
    crValve.valv_c = ctrl_CR.rail_dir & DIRC ? 1 : 0;
    crValve.valv_r = ctrl_CR.rail_dir & DIRR ? 1 : 0;

    if (TopGetSpeed() > GetVar(HDL_WORKSPEED)) {
      crValve.rail1 = ctrl_CR.rail_dir & RAIL1 ? 1 : 0;
      crValve.rail2 = ctrl_CR.rail_dir & RAIL2 ? 1 : 0;
      crValve.rail3 = ctrl_CR.rail_dir & RAIL3 ? 1 : 0;
    } else {
      crValve.rail1 = 0;
      crValve.rail2 = 0;
      crValve.rail3 = 0;
    }
  } else {}


  {
    float dens_mat_ = 1000.;
    float speed_lim_ = dispens.vol_max[ctrl_CR.width - 1] * dens_mat_ / (ctrl_CR.density * widths[ctrl_CR.width - 1] * 60);
    if (speed_lim_ > SPEED_LIM_CR) {
      speed_lim_ = SPEED_LIM_CR;
    }
    ctrl_CR.speed_lim = speed_lim_ * 3600 / 1000;
  }
  if (MD_WRK == ctrl_CR.md) {
    SetVarIndexed(IDX_ROADLINEL, ctrl_CR.rail_dir & DIRL ? 1 : 0);
    SetVarIndexed(IDX_ROADLINER, ctrl_CR.rail_dir & DIRR ? 1 : 0);
    SetVarIndexed(IDX_ROADLINEM, ctrl_CR.rail_dir & DIRC ? 1 : 0);

    Ctrl_SetMaxSpeed(ctrl_CR.speed_lim);
  } else if (MD_SET == ctrl_CR.md) {
    SetVarIndexed(IDX_ROADLINEL, 2);
    SetVarIndexed(IDX_ROADLINER, 2);
    SetVarIndexed(IDX_ROADLINEM, 2);
  } else {
    SetVarIndexed(IDX_ROADLINEM, 0);
    SetVarIndexed(IDX_ROADLINEL, 0);
    SetVarIndexed(IDX_ROADLINER, 0);
  }

  for (uint32_t i = 0; i < GS_ARRAYELEMENTS(CRValve); i++) {
    if (GetVar(HDL_IO_MODECTRL)) {
    } else if (GetVarIndexed(IDX_OILLEVEL) > 0 && !GetVarIndexed(IDX_EMCY_ON)) {
      GSeMCM_SetDigitalOut(CRValve[i].Device, CRValve[i].idx, * CRValve[i].DO);
    } else {
      GSeMCM_SetDigitalOut(CRValve[i].Device, CRValve[i].idx, 0);
    }
  }

  Light_Draw( & ctrl_ -> light);
  SetVar(HDL_DENSITY, 5 * floor(dens_ / 5.));
  SetVar(HDL_TESTVAR_0, _vol_);
  SetVar(HDL_ACT_VOL, _vol_);
  SetVar(HDL_CR_GET_CFG, dispens.cur);
  SetVar(HDL_CR_WIDHT, dispens.set);
  SetVar(HDL_SPREADERWIDTH, dispens.set);
  SetVar(HDL_CR_ROLL, IOT_NormAI( & ctrl_ -> sens.AI.cr_liqmeter));
  SetVar(HDL_CR_BOOST, _dbg_vw_ + crValve.recycl);
}

void		Ctrl_top_LRS_CR_Timer_10ms( tControl* ctrl_ ) {
	dispens.cur = ctrl_->sens.AI.cr_dispens_pos.value;
	if( 1 == f_adrv_calibr && 0 != ctrl_CR.dir_disp && 0 != dispens.set ) {
		if( absint( dispens.set - dispens.cur ) <= prec_disp
			|| (0 < ctrl_CR.dir_disp && dispens.set <= dispens.cur)
			|| (0 > ctrl_CR.dir_disp && dispens.cur <= dispens.set)
		) {
			ctrl_CR.dir_disp = 0;
			crValve.dispens_off = 0;
			crValve.dispens_on = 0;
		}
	}

	LRS_Timer_10ms( &ctrl_->top.SummerTop.LRS );
}

void Ctrl_CR_Dump( void ) {
	static uint32_t _tm_dump_ = 0;
	if( IsInfoContainerOn( CNT_TOP_CK ) || IsInfoContainerOn( CNT_TOP_CR_600A )) {
		if( (IsMaskOn( MSK_MAIN ) == 1) && (IsMsgContainerOn( CNT_LIGHTKEYS ) == 0) && (IsMsgContainerOn( CNT_LIGHTKEYS_2 ) == 0) ) {
			if( IsKeyReleasedNew( 5 ) ) {
				if( UNK == flagCRDump && GetMSTick() >= _tm_dump_ ) {
					flagCRDump = ON;
				}
				else {
					flagCRDump = OFF;
				}
			}
			if( TopGetSpeed() == 0 ) {
				if( IsKeyPressedNew( 5 ) ) {
					if( OFF == flagCRDump ) {
						flagCRDump = UNK;
						_tm_dump_ = GetMSTick() + 2000;
					}
				}
			}
		}
	}

	if( OFF == flagCRDump ) {
		SendToVisuObj( OBJ_CK_DUMP, GS_TO_VISU_SET_ATTR_BLINK, 0 );
		SendToVisuObj( OBJ_CK_DUMP_1, GS_TO_VISU_SET_ATTR_BLINK, 0 );
		SetVar( HDL_DUMP_REAGENT, 0 );
	}
	else if( UNK == flagCRDump ) {
		SendToVisuObj( OBJ_CK_DUMP, GS_TO_VISU_SET_ATTR_BLINK, 1 );
		SendToVisuObj( OBJ_CK_DUMP_1, GS_TO_VISU_SET_ATTR_BLINK, 1 );
		if( _tm_dump_ < GetMSTick() ) {
			SetVar( HDL_DUMP_REAGENT, 1 );
			SendToVisuObj( OBJ_CK_DUMP, GS_TO_VISU_SET_ATTR_BLINK, 0 );
			SendToVisuObj( OBJ_CK_DUMP_1, GS_TO_VISU_SET_ATTR_BLINK, 0 );
		}
		else {
			SetVar( HDL_DUMP_REAGENT, 0 );
		}
	}
	else if( ON == flagCRDump ) {
		SendToVisuObj( OBJ_CK_DUMP, GS_TO_VISU_SET_ATTR_BLINK, 0 );
		SendToVisuObj( OBJ_CK_DUMP_1, GS_TO_VISU_SET_ATTR_BLINK, 0 );
		SetVar( HDL_DUMP_REAGENT, 1 );
	}
}
