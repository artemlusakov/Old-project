/*******************************************************************************
*
* File:         ERRORLIST.h
* Project:      Easy Libs
* Author(s):    Marc Schartel
* Date:         02.07.2019
* Versio:       0.1.2
* Description:  This libery helps to create an error list.
* You can add errors which will be saved in a doubly linked list with all
* important data.An error is filled with the following data:
*  -Unique ID to identify the error
*  -Error Text
*  -Error State
*  -timestamp
*  -Error-Level
*
* The Error state shows if an error is still active or allready inactive.
* Additional to that an error can be marked as checked or removed. Checked
* inactive errors can be removed from the list automatically.
*
* To visulize the list, you can use
* -a text object showing the unique ID
* -a text object showing the error message
* -a index bitmap to show the actual state of the error.
* Additional to that you can change the text color depending on the error state.
*
* The distance between two lines has to be fix.
* To show the actually selected line a SimpleBitmap can be used as frame and
* will be drawn above the selected line.
*
* The lists can be handled by the standard key functions up, down and enter or
* by touch.
*
************************************************************************/

 /************************************************************************/
 /* Protection against multiple includes.                                */
 /* Do not code anything outside the following defines (except comments) */

#ifndef GSE_ERRORLIST_H
#define GSE_ERRORLIST_H
#include <UserCAPI.h>
#include "stdint.h"
#include "gselist.h"
#include "visu_statistik.h"
/*************************************************************************/

/* macro definitions *****************************************************/
#define PATH_ERROR_FLASH "/gs/data/ErorrList.cfg"
#define PATH_ERROR_USB "/gs/usb/ErorrList.cfg"

/* type definitions ******************************************************/

/* prototypes ************************************************************/

/* global constants ******************************************************/

/* global variables ******************************************************/

/* global function prototypes ********************************************/
/**
* @brief Different states a error can have.
*
*/
typedef enum eagErrorState {
	ES_NOERROR = 0,               //!< default state
	ES_UNCHECKED_ERROR_ACTIVE = (1 << 0),    //!< New Errors will be unchecked and active
	ES_UNCHECKED_ERROR_INACTIVE = (1 << 1),  //!< If the Error isn't active anymore, an active unchecked error will jump to this state
	ES_CHECKED_ERROR_ACTIVE = (1 << 2),      //!< Active Error which is checked by the user
	ES_CHECKED_ERROR_INACTIVE = (1 << 3)    //!< Inactive Error which is checked by the error. Inactive errors can be removed by  EList_AutoErrorRemove()
}eErrorState;

/**
* @brief Different levels, a Error can have.
*
*/
typedef enum eagErrLevel {
	VBL_ERROR_CRITICAL,
	VBL_ERROR,
	VBL_WARNING,
	VBL_INFO,
}eErrLevel;

typedef struct tagGsError tGsError;

/**
* @brief Structure of a Error
*
*/
struct tagGsError {
	int32_t       id;         //!< unic id of a error. Errors are defined by this id.
	char* text;       //!< Text belonging to an error
	eErrorState   State;      //!< Actual state of an error
	tSysTime      Time;       //!< Actual time an error occured the last time.
	tSysDate      Date;       //!< Actual date an error occured the last time.
	eErrLevel     Level;      //!< Error level
	uint32_t      timeold;    //!< internal used for AutoRemove of an error.
	int32_t       newErr;

	int8_t FlagCheckError; // Значение 0 - ошибка не чекнута, 1 - ошибка чекнута
	int8_t FlagCheckResetCheckError;
	int8_t FlagCheckResetNoCheckError;
	int8_t ControlWriteNewError;
};

/**
* @brief Callback function. Will be called, if a state of an error has changed.
* @param Error structur of the error.
*/
typedef void(*tGsErrOutCallback)(tGsError* Error);

/**
* @brief Callback function. Will be called, if a state of an error has changed.
* @param string Error message will be written into this string. The maximul stringsize is 196 Byte.
*/
typedef void(*tGsPrintErrOutCallback)(const char* string);

/**
* @brief Sets an new error. If the id allready exists, the error message will be overwritten.
* The level of the error will be VBL_ERROR.
*
* @param id unique error id. Errors will be identifyed by this id.
* @param text Error text of the error.
* @return int32_t returns the id of the error
*/
int32_t EList_SetErrByText( int32_t id, const char* text );

/**
* @brief Sets an new error. If the id allready exists, the error message will be overwritten.
* The level of the error will be VBL_ERROR. The text written into the Error depends of the acutal language.
*
* @param id unique error id. Errors will be identifyed by this ID. If the ID 0 is used, a Error ID will be generated out of the Ressource Test ID.
* @param RCText Offset of the Ressourcetext you want to use. The ressouretext-ID has to be used for it. It
* can be found in "RCText.h"  begining with "RCTEXT_T_". The text ressource option "transfer all texts" should be activated.
* @return int32_t returns the id of the error. If the ID 0 is used, a Error ID will be generated out of RCText
*/
int32_t EList_SetErrByRCText( int32_t id, uint32_t RCText );

/**
* @brief Sets an new error. If the id allready exists, the error message will be overwritten.
* The level of the error will be VBL_ERROR.
*
* @param id                unique error id. Errors will be identifyed by this ID. If the ID 0 is used, a Error ID will be generated out of the TestList ID.
* If you use id = 0,  every text list can genreate only one error.
* @param RCText_List       List the text to be shown is in. Look for the define RCTEXT_L_<Listname> in "RCText.h"
* @param RCText_ListIndex  Listindex of the text which has to be shown in the error. Look for the define RCTEXT_LI_HEADLINE_<Listname>_<TEXT NAME> in "RCText.h"
* @return int32_t returns the id of the error. If the ID 0 is used, a Error ID will be generated out of the RCText_List
*/
int32_t EList_SetErrByRCTextList( int32_t id, uint32_t RCText_List, uint32_t RCText_ListIndex );

/**
* @brief like EList_SetErrByText, but you can choose the error level and use a formated string with parameters.
*
* @param lev       Error level
* @param id        id unique error id. Errors will be identifyed by this id.
* @param fmt       formated string like printf. The string will be written into the error.
* @param ...       parameters like in printf.
* @return int32_t  returns the error id.
*/
int32_t EList_SetErrLevByText( eErrLevel lev, int32_t id, const char* fmt, ... );

/**
* @brief
*
* @param lev       Error level
* @param id        id unique error id. Errors will be identifyed by this id.
* @param RCText    Offset of the Ressourcetext you want to use. The ressouretext-ID has to be used for it. It
*                  can be found in "RCText.h"  begining with "RCTEXT_T_". The text ressource option "transfer all texts" should be activated.
* @param ...       The Ressource test can be used as an formated string. In this case you can use parameters like in printf here.
* @return int32_t returns the error id.
*/
int32_t EList_SetErrLevByRCText( eErrLevel lev, int32_t id, uint32_t RCText, ... );

/**
* @brief Sets an new error. If the id allready exists, the error message will be overwritten.
* The level of the error will be VBL_ERROR.
*
* @param lev               Error level
* @param id                unique error id. Errors will be identifyed by this ID. If the ID 0 is used, a Error ID will be generated out of the TestList ID.
* If you use id = 0,       every text list can genreate only one error.
* @param RCText_List       List the text to be shown is in. Look for the define RCTEXT_L_<Listname> in "RCText.h"
* @param RCText_ListIndex  Listindex of the text which has to be shown in the error. Look for the define RCTEXT_LI_HEADLINE_<Listname>_<TEXT NAME> in "RCText.h"
* @return int32_t          returns the id of the error. If the ID 0 is used, a Error ID will be generated out of the RCText_List
*/
int32_t EList_SetErrLevByRCTextList( eErrLevel lev, int32_t id, uint32_t RCText_List, uint32_t RCText_ListIndex );

/**
* @brief Sets the callback function. The data of an error will be given to this function. It will be called if the state of an error is changing.
*
* @param ErrOutCb Callback Function of the Type void(*tGsErrOutCallback)(tGsError * Error);
*/
void    EList_SetErrorOut( tGsErrOutCallback ErrOutCb );

/**
* @brief Sets the callback function. An error message will be given to the function.
*
* @param ErrOutCb Callback Function of the Type void(*tGsPrintErrOutCallback)(const char* string);
*/
void    EList_SetPrintErrorOut( tGsPrintErrOutCallback ErrOutCb );

/**
* @brief Resets an error. Call this function if an error is not active anymore.
* The acutal state of the error will change like this:
* ES_UNCHECKED_ERROR_ACTIVE --> ES_UNCHECKED_ERROR_INACTIVE
* ES_CHECKED_ERROR_ACTIVE   --> ES_CHECKED_ERROR_INACTIVE
* @param id ID of the error
*/
void    EList_ResetErr( int32_t id );

/**
* @brief Deletes an error undependent of the actual state.
*
* @param id
*/
void    EList_DeleteErr( int32_t id );

/**
* @brief This function will delete inactive checked errors after 3 seconds automatically.
*
* @param id
*/
void    EList_AutoErrorRemove( void );

/**
* @brief returns the number of unchecked errors.
*
* @return uint32_t Number of unchecked errors.
*/
uint32_t EList_GetNumUncheckedErrors( void );

/**
* @brief returns the number of checked errors.
*
* @return uint32_t Number of checked errors.
*/
uint32_t EList_GetNumCheckedErrors( void );

/**
* @brief returns the number of errors of the given errorstates
*
* @param ErrorStates state of the errors dpending on the given error type.
* You can set different error states as flags combined. e.g.
* EList_GetErrors(ES_UNCHECKED_ERROR_ACTIVE | ES_CHECKED_ERROR_ACTIVE);
* @return uint32_t number of the counted errors.
*/
uint32_t EList_GetErrors( uint32_t ErrorStates );

/**
* @brief Adds a line to the error table where the errors will be shown in. All lines need the same
* distance from each other. Each line exists out of
*  1 textobject showing the actual error number
*  1 textobject showing the error message
*  1 indexbitmap with 5 images showning the acutal error state acording to eErrorState
*
* @param TextObj_Num  Object-ID of the Textobject showing the Errornumber. Use -1 if not needed.
* @param TextObj_Text Object-ID of the Textobject showing the Error message. Use -1 if not needed.
* @param Checkbox_Hdl Object-ID of the Indexbitmap existing out of 5 images, showing the actual error state. Use -1 if not needed.
* @return int32_t
*/
int32_t ErrDraw_AddLine( uint32_t TextObj_Num, uint32_t TextObj_Text, uint32_t Checkbox_Hdl );

/**
* @brief this Errorframe will be drawn above the actual marked line. Best use an transparent Bitmap for this.
*
* @param PosMin   Minimum y-Position of the bar. Use the Y-Position of the bar, when it is lies above the first line
* @param PosStep  The offset between two lines
* @param Pos_x    X-Position of the error frame. Important for Touch
* @param Width    Width of the error frame. Important for Touch
*/
void    ErrDraw_InitErrorFrame( int32_t Pos_y, int32_t PosStep, int32_t Pos_x, int32_t Width );

void    ErrDraw_InitSetFrameObj( int32_t ObjID );
/**
* @brief Use this function if the error list is drawn inside of a container. Otherwise the Touch won't work corectly.
*
* @param CntPosX X-Position of the container.
* @param CntPosY Y-Position of the container.
*/
void    ErrDraw_SetContainerOffset( int32_t CntPosX, int32_t CntPosY );

/**
* @brief The Textcolor of the current state can be set by this function. This way active unchecked, errors can be red, checked errors orange, etc.
*
* @param State Errorstate whose color you want to set
* @param color color in the format 0xFFRRGGBB
*/
void    ErrDraw_SetTextColor( eErrorState State, uint32_t color );

/**
* @brief Call this Function every c-cycle. The standard Key-Functions (Up, down, enter,...) will be used to controle the visu of the error lis.t
*
* @param evtc number of Events given by the function parameters of UserCCycle
* @param evtv Event-Vector given by the function parameters of UserCCycle
* @return int32_t returns the Position of the errorframe. Use this Function to set the position of the error frame by SetVar
*/
int32_t ErrDraw_Cycle( uint32_t evtc, tUserCEvt* evtv );

/**
* @brief Draws the list, if something has changed.
*
*/
void ErrDraw_Draw( void );

/***********************************/
int32_t WriteOverSpeed( void );
int32_t WriteLastError( void );
int32_t WriteStartError( void );
int32_t ErrorCheckState( void );
int32_t UpdateErrorFile( tVisuData * VData );
int32_t FormattingErrorFile( tVisuData * VData );
int32_t Reset( void );
int32_t VinEquipUpdate( void );
int32_t Error_CopyToUsb( void );
/**
* @brief Used for logging errors
*
*/

 /*************************************************************************/
 /* Protection against multiple includes ends here ! Stop editing here !  */
 /*************************************************************************/
#endif  // #ifndef ERRORLIST_H
