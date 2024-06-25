/************************************************************************
*
* File:         IO_TYPES.h
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
************************************************************************/

 /************************************************************************/
 /* Protection against multiple includes.                                */
 /* Do not code anything outside the following defines (except comments) */
#ifndef IO_TYPES_H
#define IO_TYPES_H

#include	<stdint.h>
/*************************************************************************/

/* macro definitions *****************************************************/

/* type definitions ******************************************************/
typedef struct tagIOT_Button {
	uint32_t		tm_short;
	uint32_t		tm_long;
	uint32_t		tm_press;
	uint32_t		tm_release;
	uint32_t		cnt_chg;
	uint32_t		long_press;
	uint8_t			val;
} tIOT_Button;

/* prototypes ************************************************************/

/* global constants ******************************************************/

/* global variables ******************************************************/

/* global function prototypes ********************************************/

/**
* @brief This has to be called at the beginning of every C-Cycle
*
*/
void IOT_Cycle( void );

/**
* @brief Initalises a IOT-Button.
*
* @param Button Pointer to a Structure of the Type tIOT_Button
* @param time_short_ms from this time on a short key press will be detected.
* @param time_long_ms  from this timeon a long key press will be detected.
*/
void IOT_Button_Init( tIOT_Button* Button, uint32_t time_short_ms, uint32_t time_long_ms );

/**
* @brief Use this Function to change the value of a button. this value may come from a digital Input or a key
*
* @param Button Pointer to a Structure of the Type tIOT_Button
* @param val    Value of the Input
*/
void IOT_Button_SetDI( tIOT_Button* Button, uint8_t val );

/**
* @brief returns 1 if a key is pressed longer then time_short_ms and shorter then
* time_long_ms defined in IOT_Button_Init
*
* @param Button Pointer to a Structure of the Type tIOT_Button
* @return int32_t 1 if pressed time is between time_short_ms and time_long_ms. Else 0
*/
int32_t IOT_Button_IsPressedShort( tIOT_Button* Button );

/**
* @brief returns 1 if a key is pressed longer then time_long_ms defined in IOT_Button_Init
*
* @param Button Pointer to a Structure of the Type tIOT_Button
* @return int32_t 1 if pressed time is longer then time_long_ms. Else 0
*/
int32_t IOT_Button_IsPressedLong( const tIOT_Button* Button );

/**
* @brief returns 1 in the first UserCCylce when the pressed time is longer then.
* Call this funciton only in the UserCCycle.
*
* @param Button Pointer to a Structure of the Type tIOT_Button
* @return int32_t 1 for one cycle if pressed time is longer then time_long_ms. Else 0
*/
int32_t IOT_Button_IsPressedLongNew( const tIOT_Button* Button );

/**
* @brief returns 1 in the first UserCCylce when a short press has been released.
*
* @param Button Pointer to a Structure of the Type tIOT_Button
* @return int32_t 1 after release for one cycle if pressed time is shorter then time_long_ms. Else 0
*/
int32_t IOT_Button_IsReleasedShortNew( const tIOT_Button* Button );

/**
* @brief returns 1 in the first UserCCylce when a long press has been released.
*
* @param Button Pointer to a Structure of the Type tIOT_Button
* @return int32_t 1 for one cycle if pressed time is longer then time_long_ms. Else 0
*/
int32_t IOT_Button_IsReleasedLongNew( tIOT_Button* Button );

/**
* @brief like IsKeyPressedNew
*
* @param Button Pointer to a Structure of the Type tIOT_Button
* @return Like IsKeyPressedNew
*/
int32_t IOT_Button_IsPressedNew( const tIOT_Button* Button );

int32_t IOT_Button_IsReleasedNew( tIOT_Button* Button );

int32_t IOT_Button_IsDown( const tIOT_Button* Button );

typedef struct tagAct_AnaJoy {
	int32_t raw;
	int32_t middle;
	int32_t ThresZero;			///< Дрейф нуля, допустимое отклонение
	int32_t min;
	int32_t max;
	uint32_t d_count_change;	///< номер цикла измерений
	int32_t d;					///< направление положения: 1 0 -1
	int32_t a;					///< Относительное положение (%): 100 <-> 0 <-> -100
}tIOT_AnaJoy;

/**
* @brief Initalizes a analog Joystick input. It will calculate the raw value to
*  a value between -100 and 100. It is also posible to set a Threshold for the zero position.
*
* @param AnaJoy
* @param min minimum raw value. values <= min will return -100
* @param middle raw value for middle position. the Joystick will return 0 at this position.
* @param max maximum raw value. Values >= max will return 100.
* @param ThresHoldZero Threshold area around the Zero postion. Example
* ThresHoldZero = 10, min = 0 -> Values between -10 and 10 will return 0.
*/
void     IOT_AnaJoy_Init( tIOT_AnaJoy* AnaJoy, const int32_t min, const int32_t middle, const int32_t max, const  int32_t  ThresHoldZero );

/**
* @brief returns the positon of the Joystick in (-100) - 0 - (+100)
*
* @param AnaJoy
* @return int32_t range between -100 and 100
*/
int32_t  IOT_AnaJoy_GetInPercent( const tIOT_AnaJoy* AnaJoy );

/**
* @brief Use this function to actualize the function of the Joystick
*
* @param AnaJoy
* @param value analouge raw value of the Joystick
*/
void     IOT_AnaJoy_Set( tIOT_AnaJoy* AnaJoy, int32_t value );
void IOT_AnaJoy_Set_Rocker( tIOT_AnaJoy* AnaJoy, int32_t value_pos,int32_t value_percent);
void IOT_AnaJoy_Set_JoyPos( tIOT_AnaJoy* AnaJoy, int32_t value_pos,int32_t value_percent);

/**
* @brief Checks if the Joystick changed the position between neutral positive and negative position.
*
* @param AnaJoy
* @return uint32_t 1 if changed to Postion > 0%,
*                 -1 if changed to Postion < 0%,
*                  0 if nothing changed or changed to 0
*/
int32_t IOT_AnaJoy_PressedNew( const tIOT_AnaJoy* AnaJoy );

typedef struct tagIOT_AI {
	int32_t value;
	int32_t off, off_max;
	char  nom;
	float m;
	float b;
	float value_nom;

}tIOT_AI;

//Set parameters for linear function to calculate analog value into real world value
int32_t IOT_NormAI_Init( tIOT_AI* pAi, int32_t x1, int32_t x2, float y1, float y2, int32_t offset, int32_t offset_xmax );
//Calculate value
void IOT_NormAI_Calc( tIOT_AI* pAi, uint32_t value );
//read value
float IOT_NormAI( const tIOT_AI* pAi );

/*************************************************************************/
/* Protection against multiple includes ends here ! Stop editing here !  */
/*************************************************************************/
#endif  // #ifndef IO_TYPES_H
