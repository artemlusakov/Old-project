#ifndef RTR_UH_H
#define RTR_UH_H

#include "control.h"
#include "drawfunction.h"

#define		TM_CONV_SET		1000
#define		TM_HUMID_SET	1000
#define		SPEED_LIM	(60.1*1000/3600)
#define		TM_HUMID_PUMP_CTRL	5000

#define		UH_HUMID_VOL_MIN		8.89
#define		UH_HUMID_VOL_MAX		120.
#define		UH_HUMID_ROT_MIN		8
#define		UH_HUMID_ROT_MAX		105
#define		UH_HUMID_PWM_MIN		190
#define		UH_HUMID_PWM_MAX		355

#define		UH_CONV_VOL_MIN_3000		0.274
#define		UH_CONV_VOL_MAX_3000		3.556
#define		UH_CONV_VOL_MIN_5000		(0.274*1.378)
#define		UH_CONV_VOL_MAX_5000		(3.556*1.378)
#define		UH_CONV_ROT_MIN		2
#define		UH_CONV_ROT_MAX		26
#define		UH_CONV_PWM_MIN		200
#define		UH_CONV_PWM_MAX		420
#define		UH_LEN_TBL_MAX		256

enum {
	UH_PIN_ROAD_SIGN = 0,	   ///< 3	->
	UH_PIN_CONV = 1,		   ///< 5	|> spreading density
	UH_PIN_DISK_ROT = 3,	   ///< 6	|>
	UH_PIN_ASYM_DRV_ON = 5,	   ///< 7	->
	UH_PIN_ASYM_DRV_REV = 8,   ///< 28	->
	UH_PIN_REAR_WRK_LIGHT = 4, ///< 9	->
	UH_PIN_REAR_BEACONS = 6,   ///< 10	->
	UH_PIN_HUMID_PUMP = 7,	   ///< 8	|>
	//UH_PIN_MAIN_VALVE		= 4,		///< 9	->
	UH_PIN_SPREAD = 0,		   ///< 24	<-
	UH_PIN_NUMID_PUMP_ROT = 1, ///< 25	<=
	UH_PIN_TAIL_POS = 2,	   ///< 26	<-
	UH_PIN_CONV_ROT = 3,	   ///< 27	<=
	UH_PIN_HUMID = 9,		   ///< 29	<-
};



extern unsigned	uh_conv_len;
extern float	uh_conv_vol[UH_LEN_TBL_MAX];
extern float	uh_conv_rot[UH_LEN_TBL_MAX];
extern float	uh_conv_pwm_up[UH_LEN_TBL_MAX];
extern float	uh_conv_pwm_down[UH_LEN_TBL_MAX];
extern unsigned		uh_humid_len;
extern float		uh_humid_vol[UH_LEN_TBL_MAX];
extern float		uh_humid_rot[UH_LEN_TBL_MAX];
extern float		uh_humid_pwm[UH_LEN_TBL_MAX];

typedef struct {
	uint8_t		stt;
	uint8_t		type;
	tValveProp	val;
	uint16_t	rot;
	tPaintText ColourUHPercent;
} tHumidPump;


typedef struct SUniqa {
  tHumidPump		humid_pump;
	int8_t			md;							///< Общий режим: WRK, DUMP
	int8_t			md_conv;					///< Режим конвейера: MD_INI = начальное, MD_SET = усановка значения, MD_WRK = рабочее значение
	int8_t			md_humid;					///< Режим увлажнителя: UNDEF - неопределённо, INI - готов, CHK - ждём насос, SET - идёт подстрйка насоса, WRK - рабочий режим
	int8_t			md_tail;
	uint32_t		tm_conv;					///< время ожидания события, мсек
	float			conv_vol;					///< л/сек, установленный объём реагента
	int8_t			f_conv_vol_lim;				///< превышен предел объёма реагента
	int8_t			conv_speed_lim;				///< км/ч, предел скорости для этого объёма
	uint32_t		tm_humid;					///< время ожидания события, мсек
	int32_t		humid_prcnt;				///< Влажность в процентах
	float			humid_vol;					///< л/сек, установленный объём жидкости
	struct SSTT{
		int8_t			humid_sens;			///< Наличие жидкости:  UNK = не контролируется, ON = вкл., OFF = выкл.
		int8_t			spread_sens;
		int8_t			tail_pos;
		int8_t			adrv_calibr;
		int8_t			humid_pump;			///< Состояние насоса: UNK = не контролируется, ON = вкл., OFF = выкл.
		int8_t			conv;
	}		stt;
	struct SGet {
		uint16_t		humid_rot;
		uint16_t		conv_rot;
	}		rd;
	struct SSet {
		uint16_t		humid_rot;				///< Вращение насоса
		uint32_t		humid_pwm;				///< ШИМ насоса
		uint16_t		conv_rot;				///< Вращение конвеера
		uint32_t		conv_pwm;				///< ШИМ конвеера
	}		wr;
} SUniqa_t;

extern SUniqa_t	uh;

void	Ctrl_RTR_UH_init( tControl* ctrl_ );
void	Ctrl_RTR_UH_cycle( tControl* ctrl_, uint32_t evtc_, tUserCEvt* evtv_ );
void	ctrl_top_RTR_UH_timer10ms( tControl* ctrl_ );
float		liner_interpret( float* x_, float* y_, int len_, float x0_ );

#endif  // #ifndef RTR_UH_H
