/****************************************************************************
*
* File:         IO_TYPES.c
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
#include "io_types.h"
/****************************************************************************/

/* macro definitions ********************************************************/

/* type definitions *********************************************************/

/* prototypes ***************************************************************/

/* global constants *********************************************************/

/* global variables *********************************************************/

/* local function prototypes*************************************************/

/****************************************************************************/

/* function code ************************************************************/
uint32_t CycleCount;

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

/**
* @brief This has to be called at the beginning of every C-Cycle
*
*/
void IOT_Cycle( void ) {
	CycleCount++;
}

/**
* @brief Initalises a IOT-Button.
*
* @param Button Pointer to a Structure of the Type tIOT_Button
* @param time_short_ms from this time on a short key press will be detected.
* @param time_long_ms  from this timeon a long key press will be detected.
*/
void IOT_Button_Init( tIOT_Button* Button, uint32_t time_short_ms, uint32_t time_long_ms ) {
	memset( Button, 0, sizeof( tIOT_Button ) );
	Button->tm_long = time_long_ms;
	Button->tm_short = time_short_ms;
}

/**
* @brief Use this Function to change the value of a button. this value may come from a digital Input or a key
*
* @param Button Pointer to a Structure of the Type tIOT_Button
* @param val    Value of the Input
*/
void IOT_Button_SetDI( tIOT_Button* Button, uint8_t val ) {
	if( val != Button->val ) {
		Button->val = val;
		Button->cnt_chg = CycleCount;
		if( val ) {
			Button->tm_press = GetMSTick();
			Button->tm_release = 0;
		}
		else {
			Button->tm_release = GetMSTick();
			Button->long_press = 0;
		}
	}
	if( Button->val ) {
		if( 0 == Button->long_press ) {
			uint32_t dt = GetMSTick() - Button->tm_press;
			if( dt >= Button->tm_long ) {
				Button->long_press = CycleCount; //button pressed long new.
			}
		}
	}
}

/**
* @brief returns 1 if a key is pressed longer then time_short_ms and shorter then
* time_long_ms defined in IOT_Button_Init
*
* @param Button Pointer to a Structure of the Type tIOT_Button
* @return int32_t 1 if pressed time is between time_short_ms and time_long_ms. Else 0
*/
int32_t IOT_Button_IsPressedShort( tIOT_Button* Button ) {
	if( Button->val ) {
		uint32_t dt = GetMSTick() - Button->tm_press;
		if( (dt >= Button->tm_short) && (dt < Button->tm_long) ) 	{return 1;}
	}
	return 0;
}

/**
* @brief returns 1 if a key is pressed longer then time_long_ms defined in IOT_Button_Init
*
* @param Button Pointer to a Structure of the Type tIOT_Button
* @return int32_t 1 if pressed time is longer then time_long_ms. Else 0
*/
int32_t IOT_Button_IsPressedLong( const tIOT_Button* Button ) {
	if( Button->long_press == CycleCount )
		return 1;
	else
		return 0;
}

/**
* @brief returns 1 in the first UserCCylce when the pressed time is longer then.
* Call this funciton only in the UserCCycle.
*
* @param Button Pointer to a Structure of the Type tIOT_Button
* @return int32_t 1 for one cycle if pressed time is longer then time_long_ms. Else 0
*/
int32_t IOT_Button_IsPressedLongNew( const tIOT_Button* Button ) {
	if( IOT_Button_IsPressedLong( Button ) && (CycleCount == Button->long_press) ) {
		return 1;
	}
	return 0;
}

/**
* @brief returns 1 in the first UserCCylce when a long press has been released.
*
* @param Button Pointer to a Structure of the Type tIOT_Button
* @return int32_t 1 for one cycle if pressed time is longer then time_long_ms. Else 0
*/
int32_t IOT_Button_IsReleasedLongNew( tIOT_Button* Button ) {
	if( (Button->val == 0) && (Button->cnt_chg == CycleCount) ) {
		if( (Button->tm_release - Button->tm_press) >= Button->tm_long ) {
			return 1;
		}
	}
	return 0;
}

/**
* @brief like IsKeyPressedNew
*
* @param Button Pointer to a Structure of the Type tIOT_Button
* @return Like IsKeyPressedNew
*/
int32_t IOT_Button_IsPressedNew( const tIOT_Button* Button ) {
	if( (Button->val) && (Button->cnt_chg == CycleCount) ) {
		return 1;
	}
	return 0;
}

int32_t IOT_Button_IsReleasedShortNew( const tIOT_Button* Button ) {
	if( (Button->val == 0) && (Button->cnt_chg == CycleCount) ) {
		if( (Button->tm_release - Button->tm_press) < Button->tm_long ) {
			return 1;
		}
	}
	return 0;
}

int32_t IOT_Button_IsReleasedNew( tIOT_Button* Button ) {
	if( (!Button->val) && (Button->cnt_chg == CycleCount) ) {
		return 1;
	}
	return 0;
}

int32_t IOT_Button_IsDown( const tIOT_Button* Button ) {
	if( NULL == Button )	{return 0;}
	else if( Button->val ) {
		int32_t		tm_ = GetMSTick() - Button->tm_press;
		if( tm_ == 0 )			tm_ = 1;
		return tm_;
	}
	else return 0;
}

void IOT_AnaJoy_Init( tIOT_AnaJoy* AnaJoy, const int32_t min, const int32_t middle, const int32_t max, const int32_t ThresHoldZero ) {
	memset( AnaJoy, 0, sizeof( tIOT_AnaJoy ) );
	AnaJoy->max = max;
	AnaJoy->middle = middle;
	AnaJoy->min = min;
	AnaJoy->ThresZero = ThresHoldZero;
	AnaJoy->raw = middle;
}

void IOT_AnaJoy_Set( tIOT_AnaJoy* AnaJoy, int32_t value ) {
	int32_t d;
	//? do the calculation only at value change
	if( AnaJoy->raw == value )	{return;}
	AnaJoy->raw = value;

	int32_t dx = (AnaJoy->max) - (AnaJoy->middle) - (AnaJoy->ThresZero);
	if( dx == 0 )	{return;}
	//? Больше большего
	if( AnaJoy->raw >= AnaJoy->max )	{AnaJoy->a = 100; d = 1;}
	//? В каком диапазоне
	else if( AnaJoy->raw > (AnaJoy->middle) + (AnaJoy->ThresZero) ) {//= верхняя половина
		AnaJoy->a = (AnaJoy->raw - (AnaJoy->middle) - AnaJoy->ThresZero) * 100 / (dx);
		d = 1;
	}
	else if( AnaJoy->raw > AnaJoy->middle - AnaJoy->ThresZero ) {//= середина
		AnaJoy->a = 0;
		d = 0;
	}
	else if( AnaJoy->raw > (AnaJoy->min) ) {//= Нижняя половина
		AnaJoy->a = (AnaJoy->raw - (AnaJoy->middle) + (AnaJoy->ThresZero)) * 100 / (dx);
		d = -1;
	}
	else {//= меньше меньшего
		AnaJoy->a = -100;
		d = -1;
	}
	//? положение поменялось
	if( d != AnaJoy->d ) {
		AnaJoy->d = d;
		AnaJoy->d_count_change = CycleCount;
	}
}
void IOT_AnaJoy_Set_Rocker( tIOT_AnaJoy* AnaJoy, int32_t value_pos,int32_t value_percent) {
	//? do the calculation only at value change
	//if( AnaJoy->raw == value_pos)	{return;}
	AnaJoy->raw = value_pos;
  //if(value_percent>0xF0){AnaJoy->a = 0;}
	//else{
		AnaJoy->a = (value_percent/2.4)*value_pos;
	//}
	//? положение поменялось
	if( value_pos != AnaJoy->d ) {
		AnaJoy->d = value_pos;
		AnaJoy->d_count_change = CycleCount;

	}
}
void IOT_AnaJoy_Set_JoyPos( tIOT_AnaJoy* AnaJoy, int32_t value_pos,int32_t value_percent) {
	//? do the calculation only at value change
	//if( AnaJoy->raw == value_pos)	{return;}
	AnaJoy->raw = value_pos;
  //if(value_percent>0xF0){AnaJoy->a = 0;}
	//else{
		AnaJoy->a = (value_percent/2.55)*value_pos;
	//}
	//? положение поменялось
	if( value_pos != AnaJoy->d ) {
		AnaJoy->d = value_pos;
		AnaJoy->d_count_change = CycleCount;

	}
}

/**	@brief IOT_AnaJoy_GetInPercent : значение в процентах
**	@param AnaJoy
**	@return **/
int32_t IOT_AnaJoy_GetInPercent( const tIOT_AnaJoy* AnaJoy ) {
	return AnaJoy->a;
}

/**	@brief IOT_AnaJoy_PressedNew - было ли изменение положения в текущем цикле
**	@param AnaJoy
**	@return новое положение **/
int32_t IOT_AnaJoy_PressedNew( const tIOT_AnaJoy* AnaJoy ) {
	if( AnaJoy->d_count_change == CycleCount ) {
		return AnaJoy->d;
	}
	else return 0;
}

int32_t IOT_NormAI_Init( tIOT_AI* pAi, int32_t x1, int32_t x2, float y1, float y2, int32_t offset, int32_t offset_xmax ) {
	if( 0 != (x2 - x1) ) {
		if( NULL != pAi ) {
			pAi->off = offset;
			pAi->off_max = offset_xmax;
			if( x2 < pAi->off_max ) x2 = x2 + pAi->off;
			pAi->m = (y2 - y1) / (x2 - x1);
			pAi->b = y1 - pAi->m * x1;
			pAi->nom = 1;
			return 0;
		}
		else return -1;
	}
	else return -1;
}

void IOT_NormAI_Calc( tIOT_AI* pAi, uint32_t value ) {
	pAi->value = value;
	if( pAi->nom ) {
		if( pAi->value <= pAi->off_max ) {
			pAi->value_nom = pAi->m * pAi->value + pAi->b + pAi->off;
		}
		else {
			pAi->value_nom = pAi->m * pAi->value + pAi->b;
		}
	}
}

float IOT_NormAI( const tIOT_AI* pAi ) {
	return pAi->value_nom;
}
