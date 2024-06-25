#ifndef PARAM_H
#define PARAM_H

#include	<stdint.h>
#include "gs_easy_config.h"


#define PATH_PARAM_FLASH "/gs/data/param.cfg"
#define PATH_PARAM_USB   "/gs/usb/param.cfg"
#define PATH_PARAM_USB_BACKUP   "/gs/usb/BACKUP_LAST/param.cfg"

typedef struct tagParamEngine {
	uint8_t  PTOatEngine;
}tParamEngine;

typedef struct tagSpeedometer {
	uint32_t TypeSpeedMsg; // 0 - FEF1, 1 - FE6C
	uint32_t Impulses;
}tSpeedometer;

typedef struct tagParamJoystick {
	uint32_t Id;          //J1939-Id of the Joystick
  uint32_t Id2;          //J1939-Id2 of the Joystick
}tParamJoystick;

typedef struct tagParamAI {
	int32_t x1, x2;
	int32_t y1, y2;
}tParamAI;

typedef struct tagParamSensor {
	tParamAI OilTemp;
	tParamAI UsSens_1;
	tParamAI UsSens_2;
	tParamAI OilPressure;

	// LiquidPresence
	uint32_t NormOpen;
	uint32_t NormClosed;
}tParamSensor;

typedef struct tagParamAsymDrive {

  uint32_t    I_leftStop;
  uint32_t    I_rightStop;
  uint32_t    timeFullLeft;
  uint32_t    timeFullRight;
  uint32_t    timeCntrlMoveCalibrationStop;
  uint32_t    timeCalibrationAside;
  uint32_t    timeCntrlMoveStop;
  uint32_t    timeCntrlErrorStop;
  uint8_t     cntrlMove;
  uint32_t    timeReversEnable;
  uint32_t    timeReaction;

}tParamAsymDrive;

typedef struct tagParamAsymDriveFeedback {
  uint32_t    I_leftStop;
  uint32_t    I_rightStop;
  uint32_t    RightAmperage;
  uint32_t    LeftAmperage;
  uint32_t    AsymAccuracy;
}tParamAsymDriveFeedback;

typedef struct tagParamEquipTimers {
	int32_t InterUp_timer;
	int32_t InterDown_timer;
	int32_t DelayInterRotate_timer;
}tParamEquipTimers;

typedef struct tagParamSideAutoPark {
	int32_t SideAutoParkGlobalTimer;
	int32_t TimerUp;
	int32_t TimerTiltUp;
	int32_t TimerLeft;
}tParamSideAutoPark;

typedef struct tagParamVIN {
	char VIN[18];
	char Equip1[11];
	char Equip2[11];
	int32_t ActualHydDistributor;
} tParamVIN;

typedef struct tagParamOMFB {
	int32_t Invert_UpDown_A;
	int32_t Invert_UpDown_B;
	int32_t Invert_UpDown_C;
	int32_t PWM_UpDown_A;
	int32_t PWM_UpDown_B;
	int32_t PWM_UpDown_C;
	int32_t PWM_RTR;
	int32_t PWM_C610H;
	int32_t PWM_BP300;
	// PWM Brush
	int32_t PWM_Rotate_OPF245;
	int32_t PWM_Rotate_OMB1;
	int32_t PWM_Rotate_OMT1;
	int32_t PWM_Rotate_OMP;
	int32_t PWM_Rotate_OZP231;
}tParamOMFB;

typedef struct tagFrontBrush {
	int32_t NoHumid;
	int32_t HumidFromRoration;
}tFrontBrush;

typedef struct tagRTR {
	int32_t AsymType; // 0 - нет асим, 1 - KH/SH, 2 - Соргаз
	int32_t HumidType; // 0 - нет увлажнения, 1 - KH/SH, 2 - Соргаз

	int32_t TypeRTR; // 1 - KH, 2 - SH, 3 - Соргаз, 4 - UH3000, 5 - UH5000
}tRTR;

/**
* @brief Structure with all parameters saved in "/gs/data/param.cfg". You can
* access to a variable of this structure by ParameterGet() in the programm code.
* Use this structure to save parameters permanently.
* In the C-File you can find the Array ParamArray. With this array you can define
* how the informations of this structure are saved in the parameter file.
*/
typedef struct tagParamData {
	tParamEngine     Engine;
	tParamVIN        VIN;
  tSpeedometer     Speedometer;
  tParamOMFB OMFB;
  tParamEquipTimers EquipTimers;
  tParamSideAutoPark SideAutoPark;
	tParamSensor     Sensors;
	tRTR 						 RTR;
	tParamJoystick   Joystick;
	tParamAsymDrive  AsymDrive;
	tParamAsymDriveFeedback  AsymDriveFeedback;
	tFrontBrush FrontBrush;
}tParamData;

/**
* @brief Reads the config file from"/gs/data/param.cfg" and fills the Parameters
* defined in ParamArray. If an Entry is not found, default values will be used.
* If the parameter file has not been found, a new one will be generated.
*/
void InitParam( void );

void writeParameters(void);
/**
* @brief returns a pointer to the filled parameter structure.
* @return const tParamData*
*/
const tParamData* ParameterGet( void );
tParamData* GetParameter( void );
tEasyConfigEntry * Param_GetECE( void );
int Param_GetECE_Size( void );
char* translateParam(char* Src);

/*************************************************************************/
/* Protection against multiple includes ends here ! Stop editing here !  */
/*************************************************************************/
#endif  // #ifndef PARAM_H
