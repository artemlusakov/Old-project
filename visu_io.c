/****************************************************************************
*
* File:         VISO_IO.c
* Project:
* Author(s):
* Date:
*
* Description:
*
*
*
*
*
****************************************************************************/
#include <stdint.h>
#include <UserCEvents.h>  /* Events send to the Cycle function              */
#include <UserCAPI.h>     /* API-Function declarations                      */

#include "vartab.h"       /* Variable Table definition                      */
 /* include this file in every C-Source to access  */
 /* the variable table of your project             */
 /* vartab.h is generated automatically            */
#include "objtab.h"       /* Object ID definition                           */
						  /* include this file in every C-Source to access  */
						  /* the object ID's of the visual objects          */
						  /* objtab.h is generated automatically            */
#include "visu.h"
#include "io.h"
#include "visu_io.h"
#include "gsemcm.h"
#include "control.h"
#include "gse_msgbox.h"
#include "RCText.h"
#include "gsToVisu.h"

/****************************************************************************/

/* macro definitions ********************************************************/

#define DEVICE_TYPE_UNKNOWN -1
#define DEVICE_TYPE_MCM250   0

/* type definitions *********************************************************/

/* prototypes ***************************************************************/

/* global constants *********************************************************/

/* global variables *********************************************************/

uint32_t CurrentDeviceType = DEVICE_TYPE_UNKNOWN;

/* local function prototypes*************************************************/

/****************************************************************************/

/* function code ************************************************************/

/****************************************************************************
**
**    Function      :
**
**    Description   :
**
**
**
**    Returnvalues  : none
**
*****************************************************************************/

void Visu_IO_Init( const tVisuData* Data ) {
	VarEnableEvent( HDL_ACTUALIODEVICE );
	SetVarIndexed( IDX_ACTUALIODEVICE, 1 );
}

void Visu_IO_DrawMCM250( uint32_t CurrentDevice ) {
	InfoContainerOn( CNT_IO_MCM250 );

	char string[32];
	sprintf( string, "MCM250_%u", CurrentDevice + 1 );
	SetVisObjData( OBJ_DEVICENAME, string, strlen( string ) + 1 );
	for( int32_t i = 0; i < 10; i++ ) {
		SetVar( HDL_IO_VIEW_DO_00 + i, GSeMCM_GetDigitalOut( CurrentDevice, i ) );
	}
	for( int32_t i = 0; i < 4; i++ ) {
		uint16_t duty;
		uint16_t frequency;
		GSeMCM_GetPWMOut( CurrentDevice, i, &duty, &frequency );
		SetVar( HDL_IO_VIEW_PWM_0 + i, duty );
	}
}

void Visu_IO_DrawJoystick( void ) {
	char string[32];
	sprintf( string, "Joystick" );
	SetVisObjData( OBJ_DEVICENAME, string, strlen( string ) + 1 );
	InfoContainerOn( CNT_IO_JOYSTICK );
}

void Visu_IO_Engine( uint32_t CurrentDevice ) {
	char string[32];
	sprintf( string, "Engine" );
	SetVisObjData( OBJ_DEVICENAME, string, strlen( string ) + 1 );
	InfoContainerOn( CNT_ENGINE );
}

void Visu_IO_Draw( void ) {
	InfoContainerOff( CNT_IO_MCM250 );
	InfoContainerOff( CNT_IO_JOYSTICK );
	uint32_t CurrentDevice = GetVarIndexed( IDX_ACTUALIODEVICE ) - 1;
	if( CurrentDevice < JOYSTICK ) {
		Visu_IO_DrawMCM250( CurrentDevice );
  } else if (CurrentDevice == JOYSTICK) {
    Visu_IO_DrawJoystick();
  } else {
    Visu_IO_Engine(CurrentDevice);
  }
}

/**
 * @brief Функция переключения режимов управления клапанами в I/O
 */
void Visu_IO_ChangeMode( void ) {
  SetVar(HDL_IO_MODECTRL, 1 - GetVar(HDL_IO_MODECTRL));
}

/**
 * @brief Фильтр "Оптимальное бегущее среднее арифметическое"
 *
 * @param newVal
 * @param i
 * @return float
 */
float Filter_RunMiddleArifmOptim(float newVal, int i) {
  static int t[10] = {0};
  static float vals[30][10] = {0};
  static float average[10] = {0};

  if (++t[i] >= 30) t[i] = 0; // перемотка t
  average[i] -= vals[t[i]][i]; // вычитаем старое
  average[i] += newVal; // прибавляем новое
  vals[t[i]][i] = newVal; // запоминаем в массив
  return ((float) average[i] / 30);
}

/**
 * @brief Функция ручного управления клапанами.
 * Доступно управление цифровыми выходами - дискретными и пропорциональными.
 *
 * @param CurrentDevice Номер блока MCM.
 */
void Visu_IO_ManualCtrl(uint32_t CurrentDevice) {
  static uint32_t oldCurrentDevice = 0;

  // При смене МСМ получение реального состояния клапанов
  if (oldCurrentDevice != CurrentDevice) {
    for (int32_t i = 0; i < 10; i++) {
      SetVar(HDL_IO_VIEW_DO_00 + i, GSeMCM_GetDigitalOut(CurrentDevice, i));
    }
    for (int32_t i = 0; i < 4; i++) {
      uint16_t duty;
      uint16_t frequency;
      GSeMCM_GetPWMOut(CurrentDevice, i, & duty, & frequency);
      SetVar(HDL_IO_VIEW_PWM_0 + i, duty);
    }
    oldCurrentDevice = CurrentDevice;
  }

  // Управление digital out
  for (int32_t i = 0; i < 10; i++) {
    GSeMCM_SetDigitalOut(CurrentDevice, i, GetVar(HDL_IO_VIEW_DO_00 + i));
  }
  // Управление PWM out
  for (int32_t i = 0; i < 4; i++) {
    GSeMCM_SetPWMOut(CurrentDevice, i, GetVar(HDL_IO_VIEW_PWM_0 + i), 100, 1);
  }

  // Получение состояния входов
  for (int32_t i = 0; i < 6; i++) {
    SetVar(HDL_IO_VIEW_DI_00 + i, GSeMCM_GetDigitalIn(CurrentDevice, i));
  }
  // for (int32_t i = 0; i < 12; i++) {
  //   SetVar(HDL_IO_VIEW_AI_00 + i, GSeMCM_GetAnalogIn(CurrentDevice, i));
  // }
  for (int32_t i = 0; i < 12; i++) {
    if (i >= 5 && i % 2 != 0) {
      SetVar(HDL_IO_VIEW_AI_00 + i, Filter_RunMiddleArifmOptim(GSeMCM_GetAnalogIn(CurrentDevice, i), i));
    } else {
      SetVar(HDL_IO_VIEW_AI_00 + i, GSeMCM_GetAnalogIn(CurrentDevice, i));
    }
  }

  for (int32_t i = 0; i < 4; i++) {
    SetVar(HDL_IO_VIEW_FREQ_0 + i, GSeMCM_GetFrequencyInput(CurrentDevice, i));
  }
}

/**
 * @brief Функция получения состояние входов/выходов МСМ.
 * Отвечает только за графическое отображение.
 *
 * @param CurrentDevice Номер блока MCM
 */
void Visu_IO_AutoCtrl(uint32_t CurrentDevice) {
  for (int32_t i = 0; i < 6; i++) {
    SetVar(HDL_IO_VIEW_DI_00 + i, GSeMCM_GetDigitalIn(CurrentDevice, i));
  }
  for (int32_t i = 0; i < 10; i++) {
    SetVar(HDL_IO_VIEW_DO_00 + i, GSeMCM_GetDigitalOut(CurrentDevice, i));
  }
  // for (int32_t i = 0; i < 12; i++) {
  //   SetVar(HDL_IO_VIEW_AI_00 + i, GSeMCM_GetAnalogIn(CurrentDevice, i));
  // }
  for (int32_t i = 0; i < 12; i++) {
    if (i >= 5 && i % 2 != 0) {
      SetVar(HDL_IO_VIEW_AI_00 + i, Filter_RunMiddleArifmOptim(GSeMCM_GetAnalogIn(CurrentDevice, i), i));
    } else {
      SetVar(HDL_IO_VIEW_AI_00 + i, GSeMCM_GetAnalogIn(CurrentDevice, i));
    }
  }

  for (int32_t i = 0; i < 4; i++) {
    SetVar(HDL_IO_VIEW_FREQ_0 + i, GSeMCM_GetFrequencyInput(CurrentDevice, i));
  }
  for (int32_t i = 0; i < 4; i++) {
    uint16_t duty;
    uint16_t frequency;
    GSeMCM_GetPWMOut(CurrentDevice, i, & duty, & frequency);
    SetVar(HDL_IO_VIEW_PWM_0 + i, duty);
  }
}

/**
 * @brief Массив графических объектов текста ШИМ выходов
 */
int OBJ_PWMOutText[] = {
  OBJ_TRUETYPEVARIABLE513,
  OBJ_TRUETYPEVARIABLE516,
  OBJ_TRUETYPEVARIABLE519,
  OBJ_TRUETYPEVARIABLE520,
};

/**
 * @brief Массив графических объектов дискретных выходов
 */
int OBJ_DigitalOut[] = {
  //OBJ_INDEXBITMAP81,
  OBJ_INDEXBITMAP82,
  OBJ_INDEXBITMAP83,
  //OBJ_INDEXBITMAP84,
  OBJ_INDEXBITMAP91,
  //OBJ_INDEXBITMAP92,
  OBJ_INDEXBITMAP99,
  //OBJ_INDEXBITMAP100,
  OBJ_INDEXBITMAP107,
  OBJ_INDEXBITMAP108,
};

/**
 * @brief Функция изменяющая параметр объектов на редактирование.
 *
 * @param nominalFlag 1 - редактирование доступно, 2 - редактирование недоступно.
 */
void Visu_IO_NominalFlag(uint8_t nominalFlag) {
  static uint8_t oldNominalFlag = 0;
  uint16_t countPWMObj = sizeof(OBJ_PWMOutText) / sizeof(* OBJ_PWMOutText);
  uint16_t countDigitalObj = sizeof(OBJ_DigitalOut) / sizeof(* OBJ_DigitalOut);

  if(oldNominalFlag != nominalFlag) {
    for (uint32_t i = 0; i < countPWMObj; i++) {
      SendToVisuObj( OBJ_PWMOutText[i], GS_TO_VISU_SET_ATTR_NOMINALFLAG, nominalFlag);
    }
    for (uint32_t i = 0; i < countDigitalObj; i++) {
      SendToVisuObj( OBJ_DigitalOut[i], GS_TO_VISU_SET_ATTR_NOMINALFLAG, nominalFlag);
    }
    oldNominalFlag = nominalFlag;
  }
}

/**
 * @brief Сброс состояния клапанов при переключении с ручного режима на программный.
 *
 * @param CurrentDevice Номер блока MCM
 * @param pFlagChangeMode Флаг мода
 */
void CloseAllOut(uint8_t * pFlagChangeMode) {
  if (*pFlagChangeMode == 1) {
    for(uint32_t i = 0; i <= 7; i++) {
      for (int32_t j = 0; j < 10; j++) {
        GSeMCM_SetDigitalOut(i, j, 0);
      }

      for (int32_t k = 0; k < 4; k++) {
        GSeMCM_SetPWMOut(i, k, 0, 500, 1);
      }
    }
    *pFlagChangeMode = 0;
  }
}

void Visu_IO_MCM250_Cycle(uint32_t CurrentDevice, uint32_t evtc, tUserCEvt * evtv) {
  static uint8_t flagMode = 0;
  SetVar(HDL_IO_VIEW_DEVICE_STATE, GSeMCM_GetActiveStatus(CurrentDevice));
  SetVar(HDL_MCM_VOLTAGE, GSeMCM_GetVoltage(CurrentDevice));

  if (GetVar(HDL_IO_MODECTRL)) {
    SetVisObjData(OBJ_TRUETYPETEXT1100, "       Ручной", strlen("       Ручной") + 1);
    Visu_IO_NominalFlag(1);
    Visu_IO_ManualCtrl(CurrentDevice);
    flagMode = 1;
  } else {
    SetVisObjData(OBJ_TRUETYPETEXT1100, "По умолчанию", strlen("По умолчанию") + 1);
    CloseAllOut(&flagMode);
    Visu_IO_NominalFlag(0);
    Visu_IO_AutoCtrl(CurrentDevice);
  }

  // Переключение режимов
	if (IsKeyPressedNew(6)) {
		MsgBoxOkCancel(RCTextGetText(RCTEXT_T_INFO, GetVarIndexed(IDX_SYS_LANGUAGE)), RCTextGetText(RCTEXT_T_IO_CHANGEMODE, GetVarIndexed(IDX_SYS_LANGUAGE)), Visu_IO_ChangeMode, NULL, NULL, NULL);
  }
}

void Viso_IO_Joystick_Cycle( void ) {
	const tControl* Ctrl = CtrlGet();
	SetVar( HDL_IO_VIEW_DI_00, IOT_Button_IsDown( &Ctrl->joystick.Button[JSB_A] ) ? 1 : 0 );
	SetVar( HDL_IO_VIEW_DI_01, IOT_Button_IsDown( &Ctrl->joystick.Button[JSB_B] ) ? 1 : 0 );
	SetVar( HDL_IO_VIEW_DI_02, IOT_Button_IsDown( &Ctrl->joystick.Button[JSB_C] ) ? 1 : 0 );
	SetVar( HDL_IO_VIEW_DI_03, IOT_Button_IsDown( &Ctrl->joystick.Button[JSB_D] ) ? 1 : 0 );
	SetVar( HDL_IO_VIEW_DI_04, IOT_Button_IsDown( &Ctrl->joystick.Button[JSB_E] ) ? 1 : 0 );
	SetVar( HDL_IO_VIEW_DI_05, IOT_Button_IsDown( &Ctrl->joystick.Button[JSB_F] ) ? 1 : 0 );
	SetVar( HDL_IO_VIEW_AI_00, IOT_AnaJoy_GetInPercent( &Ctrl->joystick.UpDown ) );
	SetVar( HDL_IO_VIEW_AI_01, IOT_AnaJoy_GetInPercent( &Ctrl->joystick.LeftRight ) );
	SetVar( HDL_IO_VIEW_AI_02, IOT_AnaJoy_GetInPercent( &Ctrl->joystick.X ) );
	SetVar( HDL_IO_VIEW_AI_03, IOT_AnaJoy_GetInPercent( &Ctrl->joystick.Y ) );
	SetVar( HDL_IO_VIEW_AI_04, IOT_AnaJoy_GetInPercent( &Ctrl->joystick.Z ) );
	SetVar( HDL_IO_VIEW_DEVICE_STATE, IO_Joystick_IsActive( &Ctrl->joystick ) );
}

void Viso_IO_Engine_Cycle( void ) {
	const tControl* Ctrl = CtrlGet();
	SetVar( HDL_IO_VIEW_AI_00, Ctrl->engine.rpm );
	SetVar( HDL_IO_VIEW_AI_01, Ctrl->engine.AirPressure );
	SetVar( HDL_IO_VIEW_AI_02, Ctrl->engine.OilTemp );
	SetVar( HDL_IO_VIEW_AI_03, Ctrl->engine.OilLevel );
	SetVar( HDL_IO_VIEW_AI_04, Ctrl->engine.CoolentTemp );
	SetVar( HDL_IO_VIEW_AI_05, Ctrl->engine.CoolentPressure );
	SetVar( HDL_IO_VIEW_AI_06, Ctrl->engine.CoolentLevel );
	SetVar( HDL_IO_VIEW_AI_07, Ctrl->engine.AmbientAirTemp );
	SetVar( HDL_IO_VIEW_AI_08, Ctrl->engine.FuelTemp );
	SetVar( HDL_IO_VIEW_AI_09, Ctrl->engine.speed );
	SetVar( HDL_IO_VIEW_AI_10, Ctrl->engine.PTO_Engaged );
	SetVar( HDL_IO_VIEW_AI_12, Ctrl->sens.DI.SelectedGear );
	SetVar( HDL_IO_VIEW_DEVICE_STATE, Ctrl->engine.Active );
}

void Visu_IO_Open_cb( const tVisuData* Data ) {
	VarEnableEvent( HDL_ACTUALIODEVICE );
	SetVarIndexed( IDX_ACTUALIODEVICE, 1 );

	PrioMaskOn( MSK_IO );
	Visu_IO_Draw();
}

void Visu_IO_Cycle_cb( const tVisuData* Data, uint32_t evtc, tUserCEvt* evtv ) {
	for( uint32_t i = 0; i < evtc; i++ ) {
		if( evtv[i].Type == CEVT_VAR_CHANGE ) {
			if( evtv[i].Content.mVarChange.VarHandle == HDL_ACTUALIODEVICE ) {
				if( (uint32_t)(GetVarIndexed( IDX_ACTUALIODEVICE ) - 1) >= ENGINE ) {
					SetVarIndexed( IDX_ACTUALIODEVICE, ENGINE + 1 );
				}
				Visu_IO_Draw();
			}
		}
	}
	uint32_t CurrentDevice = GetVarIndexed( IDX_ACTUALIODEVICE ) - 1;
	if( CurrentDevice < JOYSTICK ) {
		Visu_IO_MCM250_Cycle( CurrentDevice, evtc, evtv );
  } else if (CurrentDevice == JOYSTICK) {
		Viso_IO_Joystick_Cycle();
  } else {
		Viso_IO_Engine_Cycle();
	}

  if(IsKeyPressedNew(1)) {
    if(GetVarIndexed(IDX_ACTUALIODEVICE) > 1) {
      SetVarIndexed(IDX_ACTUALIODEVICE, GetVarIndexed(IDX_ACTUALIODEVICE) - 1);
    }
  }
  if(IsKeyPressedNew(2)) {
    SetVarIndexed(IDX_ACTUALIODEVICE, GetVarIndexed(IDX_ACTUALIODEVICE) + 1);
  }
	Visu_HomeKeyPressed();
}

void Visu_IO_Timer_cb(const tVisuData * Data) {}

void Visu_IO_Close_cb( const tVisuData* Data ) {
	VarDisableEvent( HDL_ACTUALIODEVICE );
	InfoContainerOff( CNT_IO_MCM250 );
	InfoContainerOff( CNT_IO_JOYSTICK );
	InfoContainerOff( CNT_ENGINE );
}
