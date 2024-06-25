/************************************************************************
*
* File:        gseMCM.h
* Project:		EasyLibs
* Author(s):	Marc Schartel
* Date:		16.01.2020
* Version:     0.2
*
* Description:
*
*
*
*
*
************************************************************************/

 /************************************************************************/
 /* Protection against multiple includes.                                */
 /* Do not code anything outside the following defines (except comments) */
#ifndef gseMCM200_H
#define gseMCM200_H

/*************************************************************************/
#include	<stdint.h>

/* macro definitions *****************************************************/

/* type definitions ******************************************************/

/* prototypes ************************************************************/

/* global constants ******************************************************/

/* global variables ******************************************************/

/* global function prototypes ********************************************/

typedef enum eagGSeMCM_TYPE {
	MCMTYPE_MCM200,
	MCMTYPE_MCM210,
	MCMTYPE_MCM250,
	MCMTYPE_BT20,
	MCMTYPE_GSK_K2_4x3,
	MCMTYPE_K1
}eGSeMCM_TYPE;

typedef enum eagGSeMCM_Led_Color {
	GSeMCM_LED_WHITE,
	GSeMCM_LED_RED,
	GSeMCM_LED_GREEN,
}eGSeMCM_Led_Color;

typedef struct tagGSeMCM_Led {
	uint64_t Led[3];
	uint64_t Led_Old[3];
}tGSeMCM_Led;

typedef struct tagGSeMCM_PWM {
	uint16_t freq;
	uint16_t freq_old;
	int16_t duty;
	int16_t duty_old;
	uint32_t time;
}tGSeMCM_PWM;

typedef struct tagGSeMCM_DO {
	uint32_t val;
	uint32_t val_old;
	uint32_t time;

}tGSeMCM_DO;

typedef struct tagGS_MCM200 {
	uint32_t  digIn;     /* the digital input value  */
	tGSeMCM_DO   digOut;    /* the digital output structure */

	uint16_t  anaIn[4];  /* the analog Input value */

	uint16_t  digInHnd;  /* the digital input handle */
	uint16_t  digOutHnd; /* the digital output handle */
	uint16_t  anaInHnd;  /* the analog input handle */
	uint16_t  cntHnd;    /* the counter handle */

} tGS_MCM200;

typedef struct tagGS_BT20 {

	uint32_t Key_Act[2];
	uint32_t Key_Old[2];
	uint8_t  KeyPressedNew;
	uint8_t  KeyReleasedNew;
	tGSeMCM_Led Led;
	void* msgFiFo;

}tGS_BT20;

typedef struct tagGS_GSK_K2_4x3 {

	uint32_t Key_Act;
	uint32_t Key_Old;
	uint8_t  KeyPressedNew;
	uint8_t  KeyReleasedNew;
	tGSeMCM_Led Led;
	void* msgFiFo;

}tGS_GSK_K2_4x3;

typedef struct tagGS_K1 {
	uint32_t Key_Act;
	uint32_t Key_Old;
	uint8_t  KeyPressedNew;
	uint8_t  KeyReleasedNew;
	tGSeMCM_Led Led;
	uint8_t  Rot[2];
	int8_t  RotChange[2];
	int8_t  RotChangeOld[2];
	void* msgFiFo;
}tGS_K1;

typedef struct tagGS_MCM250 {
	uint32_t  digIn;     /* the digital input value  */
	tGSeMCM_DO   digOut;    /* the digital output structure */
	uint16_t  anaIn[12];  /* the analog Input value */
	uint32_t  counter[4];
	uint32_t  overflow[4];
	uint32_t  frequency[4];
	tGSeMCM_PWM  pwm[4];

	uint16_t  digInHnd;  /* the digital input handle */
	uint16_t  digOutHnd; /* the digital output handle */
	uint16_t  anaInHnd;  /* the analog input handle */
	uint16_t  cntHnd;    /* the counter handle */
	uint16_t  pwmHnd;
}tGS_MCM250;

typedef struct tagGS_MCM210 {
	uint32_t  digIn;     /* the digital input value  */
	tGSeMCM_DO   digOut;    /* the digital output structure */
	uint16_t  anaIn[12];  /* the analog Input value */
	uint32_t  counter[4];
	uint32_t  overflow[4];
	uint32_t  frequency[4];
	tGSeMCM_PWM  pwm[4];

	uint16_t  digInHnd;  /* the digital input handle */
	uint16_t  digOutHnd; /* the digital output handle */
	uint16_t  anaInHnd;  /* the analog input handle */
	uint16_t  cntHnd;    /* the counter handle */
	uint16_t  pwmHnd;
}tGS_MCM210;

typedef struct tagGS_MCM {
	uint32_t   polling_ms;
	uint32_t   polling_t_old;
	uint32_t   Ext;
	uint32_t   CAN;
	uint32_t   RX_Id;
	uint32_t   TX_Id;
	uint32_t   FiFo;
	int32_t   Temp;
	uint32_t   Volt;
	uint32_t   Last_Msg_ms;
	uint32_t   state;
	char       DeviceName[32];
	eGSeMCM_TYPE Type;
	union {
		tGS_MCM200     MCM200;
		tGS_MCM210     MCM210;
		tGS_MCM250     MCM250;
		tGS_BT20       BT20;
		tGS_GSK_K2_4x3 K2_4x3;
		tGS_K1         K1;
	}MCM;
}tGS_MCM;

/**
* @brief Creates a MCM-Module. The returned handle can be used as Parameter for other MCM-Funcitons.
*
* @param Type type of the MCM-Module
* @param CAN which CAN
* @param ext 0 for 11 Bit, 1 for 29 Bit
* @param MCMTX TX-ID of the device
* @param MCMRX RX-ID of the device
* @return int32_t returns the Handle of the Device, which has to be used in the other MCM-Functions.
*/
int32_t GSeMCM_InitDevice( eGSeMCM_TYPE Type, int32_t CAN, int32_t ext, int32_t MCMTX, int32_t MCMRX );

/**
* @brief Call this function once in a UserCCycle
*
*/
void GSeMCM_Cycle( void );

/**
* @brief Sets the Polling time. The Lib requests the states of Temperature, Voltage,.
* The polling time is needed to check if the device is active.
* @param Hdl_MCM
* @param Polling_ms Polling time in ms
* @return int32_t -1 if invalid handle, else 0
*/
int32_t GSeMCM_SetPolling( uint32_t Hdl_MCM, uint32_t Polling_ms );

/**
* @brief returns, if the MCM-Device is active. If no massage has been received
* for more then 2 x Polling_ms (see GSeMCM_SetPolling), the function will return 0.
* else 1.
* For the first 10 seconds after  devicestart this function will return 1, to prevent
* problems while startup.
*
* @param Hdl_MCM
* @return int32_t
*/
int32_t GSeMCM_GetActiveStatus( uint32_t Hdl_MCM );

/**
* @brief returns the state of a digital input
*
* @param Hdl_MCM
* @param pos position of the digital input
* @return int32_t 1 if Input high, 0, if input low, -1 if device has no inputs or handle is invalid
*/
int32_t GSeMCM_GetDigitalIn( uint32_t Hdl_MCM, uint32_t pos );

/**
* @brief returns the value of and analouge input.
*
* @param Hdl_MCM
* @param pos
* @return -1 at an error, else <=0. Range depends on the type of the MCM-Module
*/
int32_t GSeMCM_GetAnalogIn( uint32_t Hdl_MCM, uint32_t pos );

/**
* @brief returns the temperature of the device.
*
* @param Hdl_MCM
* @return INT32_MIN if an error occured. Else temperature  in 0.1°C-steps
*/
int32_t GSeMCM_GetTemperature( uint32_t Hdl_MCM );

/**
* @brief returns the voltage of a device.
*
* @param Hdl_MCM
* @return int32_t -1 if an error occured, else Voltage in 0.1V-steps
*/
int32_t GSeMCM_GetVoltage( uint32_t Hdl_MCM );

/**
* @brief sets a digital output. the message will be send to the can at a value change.
* The time difference between two messages will be between 50 and 1000ms.
*
* @param Hdl_MCM
* @param pos Postion of the digital output
* @param val 1 or 0.
* @return int32_t -1 if invalid handle, else 0.
*/
int32_t GSeMCM_SetDigitalOut( uint32_t Hdl_MCM, uint32_t pos, int32_t val );
int32_t SetDigitalOut( uint32_t Hdl_MCM, uint32_t pos, int32_t val );

/**
* @brief returns the state of an digital output. This value will be written to the can.
* So it might not be the actual state on the MCM-Module.
*
* @param Hdl_MCM
* @param pos
* @return int32_t  -1 if invalid handle, else value of the digital output.
*/
int32_t GSeMCM_GetDigitalOut( uint32_t Hdl_MCM, uint32_t pos );

/**
* @brief Sets a PWM-Output. The message will be send to the can at a value change.
* The time difference between two messages will be between 50 and 1000ms.
*
* @param i MCM-Hld
* @param pos Index of the MCM-Output
* @param pulswidth Pulswidth from 0-1000
* @param frequency Frequency, Minimum and maximum depending on the device.
* @param enabled enable / disable MCM-Module
* @return int32_t  -1 if invalid handle, else 0.
*/
int32_t GSeMCM_SetPWMOut( uint32_t Hdl_MCM, uint8_t pos, uint16_t pulswidth, uint16_t frequency, uint8_t enabled );

/**
* @brief returns the frequency of an counter, or -1 if the device or position are invalid.
*
* @param Hdl_MCM
* @param pos Index of the MCM-Output
* @return int32_t Frequency or -1 at invalid / not suppored Parameters
*/
int32_t GSeMCM_GetFrequencyInput( uint32_t Hdl_MCM, uint32_t pos );

/**
* @brief writes the value of a counter in val1 and val2.
*
* @param Hdl_MCM
* @param pos  Index of the counter
* @param val1 value of the counter
* @param val2 value of the overflow counter
* @return int32_t 0 or -1 at invalid / not suppored Parameters
*/
int32_t GSeMCM_Get_CounterValue( uint32_t Hdl_MCM, uint32_t pos, int32_t* val1, int32_t* val2 );

/**
* @brief Sets the white key backlight.
*
* @param GSeMCM_Hdl
* @param Key Number of the Backlight to be set, 0 for all white backlights
* @param val 1 for on , 0 for off
* @return int32_t 0 or -1 at invalid / not suppored Parameters
*/
int32_t GSeMCM_SetKeyBacklight_White( uint32_t GSeMCM_Hdl, uint32_t Key, uint8_t val );

/**
* @brief Sets the red  status light.
*
* @param GSeMCM_Hdl
* @param Key Number of the Backlight to be set, 0 for all red backlights
* @param val 1 for on , 0 for off
* @return int32_t 0 or -1 at invalid / not suppored Parameters
*/
int32_t GSeMCM_SetKeyBacklight_Red( uint32_t GSeMCM_Hdl, uint32_t Key, uint8_t val );

/**
* @brief Sets the green status light.
*
* @param GSeMCM_Hdl
* @param Key Number of the Backlight to be set, 0 for all green backlights
* @param val 1 for on , 0 for off
* @return int32_t 0 or -1 at invalid / not suppored Parameters
*/
int32_t GSeMCM_SetKeyBacklight_Green( uint32_t GSeMCM_Hdl, uint32_t Key, uint8_t val );

/**
* @brief Sets the brightness of the keybacklight. With this function the brightness of one color
* can be set completely.
*
* @param GSeMCM_Hdl
* @param Color Color to beset according to eGSeMCM_Led_Color
* @param Birghtness Brightness from 0 - 100
* @return int32_t
*/
int32_t GSeMCM_SetKeyBacklight_Brightness( uint32_t GSeMCM_Hdl, eGSeMCM_Led_Color Color, uint32_t Birghtness );

/**
* @brief returns the number of the lowest pressed key.
*
* @param GSeMCM_Hdl
* @return int32_t numnber of the pressed key or 0.
*/
int32_t GSeMCM_IsAnyKeyDown( uint32_t GSeMCM_Hdl );

int32_t GSeMCM_IsKeyDown( uint32_t GSeMCM_Hdl, int32_t Key );
int32_t GSeMCM_IsKeyReleasedNew( uint32_t GSeMCM_Hdl, int32_t Key );
int32_t GSeMCM_IsKeyPressedNew( uint32_t GSeMCM_Hdl, int32_t Key );

int32_t GSeMCM_SetBuzzer( uint32_t GSeMCM_Hdl, uint32_t t_on_ms, uint32_t t_off_ms, uint32_t count );

int32_t GSeMCM_SetBuzzerFreq( uint32_t GSeMCM_Hdl, uint32_t Freq );
/**
* @brief If emcy is set, all digital outouts off all devices  will bes switched off.
*
* @param val
*/
void GSeMCM_Emcy_Set( uint32_t val );

/**
* @brief returns the state of emergency
*
* @return uint32_t 1 if emergency is active. else 0
*/
uint32_t GSeMCM_Emcy_Get( void );

/**
* @brief deinitalises the mcm lib and stops all opened threads. this is needed to shut down the device by software
* (for example at a software reboot)
*
*/
void    GSeMCM_DeInit( void );

int32_t GSeMCM_GetRotaryEncoderPos( uint32_t GSeMCM_Hdl, uint32_t Knob );
int32_t GSeMCM_SetRotaryEncoder( uint32_t GSeMCM_Hdl, int32_t idx, int32_t position );

int32_t GSeMCM_GetPWMOut( uint32_t Hdl_MCM, uint8_t pos, uint16_t* pulswidth, uint16_t* frequency );

/*************************************************************************/
/* Protection against multiple includes ends here ! Stop editing here !  */
/*************************************************************************/
#endif  // #ifndefgseMCM200_H
