/************************************************************************
*
* File:         GSE_MSGBOX.h
* Project:
* Author(s):
* Date:
*
* Description: This libary provides the possiblilty to use a Container as a
* pop up Message box. The message box can has between 1 and 3 buttons.
* You can open several Messageboxes in your c code. They will be saved into a
* fifo and shown one after a nother. As soon as a button of a message box is
* pressed, a callback function will be called to trigger an action. If there is
* another Messagebox saved in the fifo, this one will be shown. Otherwise the
* container for the messageboxes will close.
*
************************************************************************/

 /************************************************************************/
 /* Protection against multiple includes.                                */
 /* Do not code anything outside the following defines (except comments) */
#ifndef GSE_MSGBOX_H
#define GSE_MSGBOX_H
#include "UserCEvents.h"

/*************************************************************************/

/* macro definitions *****************************************************/

/* type definitions ******************************************************/

/* prototypes ************************************************************/

/* global constants ******************************************************/

/* global variables ******************************************************/

/* global function prototypes ********************************************/

/**
* @brief Callback function for a messge box. This Function will be called if the
* corresponding Icon in the messagebox has been pressed.
*/
typedef void (*MsgBox_cb)(void* arg);

/**
* @brief Use this Fuction to define a container as messagebox. The messagebox can has between 1 and 3 buttons to confirm.
* In the container 3 MenuBitmaps are needed as buttons. In each button a ressurce text is needed. Best use the alignment "Align center".
* Additonal to that you can add a Textobject for the headline and one as a textmessage.
*
* @param MsgCnt               Container to be opened
* @param Obj_Headline         Headline of the messagebox shown at top Use a text object for it.
* @param Obj_Text             Object ID of the textmessage. Use the option WordWrap to write a text over several lines
* @param Obj_MenuButton_0     Object ID of the first menu button
* @param Obj_MenuButton_1     Object ID of the second menu button
* @param Obj_MenuButton_2     Object ID of the third menu button
* @param Obj_MenuButtonText_0 Object ID of the first text in the MenuButton
* @param Obj_MenuButtonText_1 Object ID of the second text in the MenuButton
* @param Obj_MenuButtonText_2 Object ID of the third text in the MenuButton
* @return int32_t
*/
int32_t MsgBoxInit( int32_t MsgCnt,
	int32_t Obj_Headline,
	int32_t Obj_Text,
	int32_t Obj_MenuButton_0,
	int32_t Obj_MenuButton_1,
	int32_t Obj_MenuButton_2,
	int32_t Obj_MenuButtonText_0,
	int32_t Obj_MenuButtonText_1,
	int32_t Obj_MenuButtonText_2 );
  
  typedef struct tagReactBuzzer {
	uint32_t ReactBuzzer;
	int64_t ReactTimer;
	int16_t ReactDoing;
} tReactBuzzer;

/**
* @brief Gernerates a Messagebox and saves it into the message box fifo.
* The Box will have the Buttons "OK" and "Cancel"
*
* @param headline      Text of the headline
* @param text          Text of the message
* @param ok_cb         function called if you press OK
* @param ok_arg        argument of ok_cb
* @param cancel_cb     function called if you press Cancel
* @param cancel_arg    argument of cancel_cb
* @return int32_t currently only 0
*/
int32_t MsgBoxOkCancel( const char* headline, const char* text, MsgBox_cb ok_cb, void* ok_arg, MsgBox_cb cancel_cb, void* cancel_arg );

/**
* @brief Gernerates a Messagebox and saves it into the message box fifo.
* The Box will have the Button "OK"
*
* @param headline      Text of the headline
* @param text          Text of the message
* @param cb            function called if you press OK
* @param arg           argument of cb

* @return int32_t currently only 0
*/
int32_t MsgBoxOk( const char* headline, const char* text, MsgBox_cb cb, void* arg );

/**
* @brief
*
* @param Headline_RCText Text_ID of the Headline text of the type RCTEXT_T_...
* @param Text_RCText     Text_ID of the Message text of the type RCTEXT_T_...
* @param ok_cb           function called if you press OK
* @param ok_arg          argument of ok_cb
* @param cancel_cb       function called if you press Cancel
* @param cancel_arg      argument of cancel_cb
* @return int32_t currently only 0
*/
int32_t MsgBoxOKCancel_RCText( uint32_t Headline_RCText, uint32_t Text_RCText, MsgBox_cb ok_cb, void* ok_arg, MsgBox_cb cancel_cb, void* cancel_arg );

/**
* @brief
*
* @param Headline_RCText Text_ID of the Headline text of the type RCTEXT_T_...
* @param Text_RCText     Text_ID of the Message text of the type RCTEXT_T_...
* @param ok_cb           function called if you press OK
* @param ok_arg          argument of ok_cb
* @param cb              function called if you press OK
* @param arg             argument of cb
* @return int32_t currently only 0
*/
int32_t MsgBoxOK_RCText( uint32_t Headline_RCText, uint32_t Text_RCText, MsgBox_cb cb, void* arg );

/**
* @brief Call this function once every UserCCycle. This function will open a new messagebox if
* none is shown and a new one is in the Messagebox Stack.
* If a Messagebox is open, this will close it, if a Button is pressed and call the
* corresponding CallBack-Functions
*
* @param evtc
* @param evtv
* @return int32_t
*/
int32_t MsgBoxCycle( uint32_t evtc, tUserCEvt* evtv );

/**
* @brief Checks if a message box is shown on the screen.
*
* @return int32_t 1 if any message box is shown. Otherwise 0
*/
int32_t MsgBox_IsAnyBoxActive( void );
/*************************************************************************/
/* Protection against multiple includes ends here ! Stop editing here !  */
/*************************************************************************/
#endif  // #ifndef GSE_MSGBOX_H
