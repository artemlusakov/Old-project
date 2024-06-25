/****************************************************************************
*
* File:         GSE_MSGBOX.c
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

#include "gsToVisu.h"
#include "objtab.h"
#include "gsMsgFiFo.h"
#include "gse_msgbox.h"
#include "vartab.h"
#include "RCText.h"
#include "response_error.h"
#include "gseerrorlist.h"
#include "control.h"
 /****************************************************************************/

 /* macro definitions ********************************************************/

 /* type definitions *********************************************************/

 /* prototypes ***************************************************************/

 /* global constants *********************************************************/

 /* global variables *********************************************************/

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

typedef enum eagGSe_MsgBoxType {
	MSB_BOX_OWN,
	MSG_BOX_TYPE_OK,
	MSG_BOX_TYPE_CANCEL,
	MSG_BOX_TYPE_OK_CANCEL,
}		eagGSe_MsgBoxType;

typedef struct tagGse_MsgBoxButton {
	char* Text;
	MsgBox_cb cb;
	void* cb_arg;
}		tGse_MsgBoxButton;

typedef struct tagGSe_MsgBox {
	char* Text;
	char* Headline;
	tGse_MsgBoxButton Buttons[3];
	uint32_t numButtons;
}		tGSe_MsgBox;

typedef struct tagGSe_MsgBoxPriv {
	int32_t Cnt;
	int32_t Obj_Text;
	int32_t Obj_Headline;
	int32_t Obj_Button[3];
	int32_t Obj_ButtonText[3];
	tGsMsgFiFo Fifo;
	tGSe_MsgBox* Actual;
}		tGSe_MsgBoxPriv;

tGSe_MsgBoxPriv msb;

int32_t MsgBoxInit( int32_t MsgCnt, int32_t Obj_Headline, int32_t Obj_Text,
	int32_t Obj_MenuButton_0, int32_t Obj_MenuButton_1, int32_t Obj_MenuButton_2,
	int32_t Obj_MenuButtonText_0, int32_t Obj_MenuButtonText_1, int32_t Obj_MenuButtonText_2 ) {
	memset( &msb, 0, sizeof( tGSe_MsgBoxPriv ) );
	msb.Cnt = MsgCnt;
	msb.Obj_Text = Obj_Text;
	msb.Obj_Headline = Obj_Headline;
	msb.Obj_Button[0] = Obj_MenuButton_0;
	msb.Obj_Button[1] = Obj_MenuButton_1;
	msb.Obj_Button[2] = Obj_MenuButton_2;
	msb.Obj_ButtonText[0] = Obj_MenuButtonText_0;
	msb.Obj_ButtonText[1] = Obj_MenuButtonText_1;
	msb.Obj_ButtonText[2] = Obj_MenuButtonText_2;
	msb.Fifo = gsMsgFiFoCreate( sizeof( tGSe_MsgBox ) * 16 );
	return 0;
}

void MsgBoxButtonDestroy( tGse_MsgBoxButton* Button ) {
	if( NULL != Button->Text )
		free( Button->Text );
}

void MsgBoxDestroy( tGSe_MsgBox* MsgBox ) {
	for( uint32_t i = 0; i < MsgBox->numButtons; i++ ) {
		MsgBoxButtonDestroy( &MsgBox->Buttons[i] );
	}

	if( NULL != MsgBox->Text ) {
		free( MsgBox->Text );
	}
	if( NULL != MsgBox->Headline ) {
		free( MsgBox->Headline );
	}
}
extern tReactBuzzer		React;
int32_t MsgBoxCycle(uint32_t evtc, tUserCEvt * evtv) {
  static tGSe_MsgBox ActualBox;
  if (NULL == msb.Actual) {
    if (0 == gsMsgFiFoRead(msb.Fifo, & ActualBox, sizeof(tGSe_MsgBox), 0)) {
      msb.Actual = NULL;
      if (IsMsgContainerOn(msb.Cnt)) {
        MsgContainerOff(msb.Cnt);
      }
      return 0;
    } else msb.Actual = & ActualBox;

    //Anzahl der Buttons berechnen

    //Draw Buttons

    if (1 == msb.Actual -> numButtons) {
      SendToVisuObj(msb.Obj_Button[0], GS_TO_VISU_SET_ATTR_ALL, 0);
      SendToVisuObj(msb.Obj_Button[1], GS_TO_VISU_SET_ATTR_ALL, ATTR_VISIBLE | ATTR_NOMINAL | ATTR_GRAB_FOCUS);
      SendToVisuObj(msb.Obj_Button[2], GS_TO_VISU_SET_ATTR_ALL, 0);
      SetVisObjData(msb.Obj_ButtonText[0], "", strlen("") + 1);
      SetVisObjData(msb.Obj_ButtonText[1], msb.Actual -> Buttons[0].Text, strlen(msb.Actual -> Buttons[0].Text) + 1);
      SetVisObjData(msb.Obj_ButtonText[2], "", strlen("") + 1);
    } else if (2 == msb.Actual -> numButtons) {
      SendToVisuObj(msb.Obj_Button[0], GS_TO_VISU_SET_ATTR_ALL, ATTR_VISIBLE | ATTR_NOMINAL);
      SendToVisuObj(msb.Obj_Button[1], GS_TO_VISU_SET_ATTR_ALL, 0);
      SendToVisuObj(msb.Obj_Button[2], GS_TO_VISU_SET_ATTR_ALL, ATTR_VISIBLE | ATTR_NOMINAL | ATTR_GRAB_FOCUS);
      SetVisObjData(msb.Obj_ButtonText[0], msb.Actual -> Buttons[0].Text, strlen(msb.Actual -> Buttons[0].Text) + 1);
      SetVisObjData(msb.Obj_ButtonText[1], "", strlen("") + 1);
      SetVisObjData(msb.Obj_ButtonText[2], msb.Actual -> Buttons[1].Text, strlen(msb.Actual -> Buttons[1].Text) + 1);
    } else if (3 == msb.Actual -> numButtons) {
      SendToVisuObj(msb.Obj_Button[0], GS_TO_VISU_SET_ATTR_ALL, ATTR_VISIBLE | ATTR_NOMINAL | ATTR_GRAB_FOCUS);
      SendToVisuObj(msb.Obj_Button[1], GS_TO_VISU_SET_ATTR_ALL, ATTR_VISIBLE | ATTR_NOMINAL | ATTR_GRAB_FOCUS);
      SendToVisuObj(msb.Obj_Button[2], GS_TO_VISU_SET_ATTR_ALL, ATTR_VISIBLE | ATTR_NOMINAL | ATTR_GRAB_FOCUS);
      SetVisObjData(msb.Obj_ButtonText[0], msb.Actual -> Buttons[0].Text, strlen(msb.Actual -> Buttons[0].Text) + 1);
      SetVisObjData(msb.Obj_ButtonText[1], msb.Actual -> Buttons[1].Text, strlen(msb.Actual -> Buttons[1].Text) + 1);
      SetVisObjData(msb.Obj_ButtonText[2], msb.Actual -> Buttons[2].Text, strlen(msb.Actual -> Buttons[2].Text) + 1);
    }

    int check = 0;
    int size_react = SizeOfReact();
    for (int i = 0; i < size_react; i++) {
      if (strstr(msb.Actual -> Text, ReactTxt(i))) {
        check = check + 1;
        switch (ReactDoingInError(i)) {
        case 0: //Ignore
          msb.Actual = NULL;
          React.ReactDoing = -1;
          React.ReactTimer = 0;
          break;
        case 1: //NoDo
         // if (React.ReactDoing != 2 && React.ReactDoing != 1) {
            React.ReactDoing = 0;
          //}
          break;

        case 2: //StopAll
          React.ReactDoing = 2;
          break;

        case 3: //StopTop
          //if (React.ReactDoing != 2) {
            React.ReactDoing = 1;
          //}
          break;
        }

        if (React.ReactDoing != -1) { //If no ignor
          switch (ReactSignalInError(i)) {
          case 0: //NoSignal
            React.ReactTimer = 0;
            SetBuzzer(0, 0, 0, 0);
            break;

          case 1: //Short Signal
            React.ReactTimer = 0;
            SetBuzzer(1, 20, 20, 2);
            break;

          case 2: //Long Signal
            React.ReactTimer = -2;
            break;

          case 3: //Repeat signal
            React.ReactTimer = 1;
            break;

          }
        }
        i = size_react - 1;
      }
    }

    if (React.ReactDoing != -1) {
      if (NULL != msb.Actual -> Text) {
        SetVisObjData(msb.Obj_Text, msb.Actual -> Text, strlen(msb.Actual -> Text) + 1);
      } else {
        SetVisObjData(msb.Obj_Text, " ", strlen(" ") + 1);
      }

      if (NULL != msb.Actual -> Headline)
        SetVisObjData(msb.Obj_Headline, msb.Actual -> Headline, strlen(msb.Actual -> Headline) + 1);
      else {
        SetVisObjData(msb.Obj_Headline, " ", strlen(" ") + 1);
      }
    }

    if (check == 0) {
      React.ReactTimer = 0;
      React.ReactDoing = 0;
      SetBuzzer(1, 20, 20, 2);
    }

    React.ReactBuzzer = 0;
    if (React.ReactDoing != -1) {
      MsgContainerOn(msb.Cnt);
    }
  } else {
    for (uint32_t i = 0; i < evtc; i++) {
      if (CEVT_MENU_INDEX == evtv[i].Type) {
        int32_t ObjId = evtv[i].Content.mMenuIndex.ObjID;
        if (NULL == msb.Actual) {}
        for (int32_t j = 0; j < 3; j++) {
          if (ObjId == msb.Obj_Button[j]) {
            if (1 == msb.Actual -> numButtons) {
              if (j == 1) {
                if (NULL != msb.Actual -> Buttons[0].cb) {
                  msb.Actual -> Buttons[0].cb(msb.Actual -> Buttons[0].cb_arg);
                }
              }
            } else if (2 == msb.Actual -> numButtons) {
              if (j == 0) {
                if (NULL != msb.Actual -> Buttons[0].cb) {
                  msb.Actual -> Buttons[0].cb(msb.Actual -> Buttons[0].cb_arg);
                }
              } else if (j == 2) {
                if (NULL != msb.Actual -> Buttons[1].cb) {
                  msb.Actual -> Buttons[1].cb(msb.Actual -> Buttons[1].cb_arg);
                }
              }
            } else {
              if (NULL != msb.Actual -> Buttons[j].cb) {
                msb.Actual -> Buttons[j].cb(msb.Actual -> Buttons[j].cb_arg);
              }
            }
            React.ReactDoing = 0;
            MsgBoxDestroy(msb.Actual);
            msb.Actual = NULL;
            MsgContainerOff(msb.Cnt);
            return 0;
          }
        }
      }
    }
  }

  if (React.ReactTimer == -2) {
    SetBuzzer(2, 20, 20, 2);
  }

  if (React.ReactTimer > 0 && React.ReactTimer + 5000 < GetMSTick()) {
    React.ReactTimer = GetMSTick();
    SetBuzzer(1, 20, 20, 2);
  }
  return 0;
}

int32_t Msg_Create( tGSe_MsgBox* MsgBox, const char* headline, const char* text ) {
	memset( MsgBox, 0, sizeof( tGSe_MsgBox ) );
	MsgBox->Text = strdup( text );
	MsgBox->Headline = strdup( headline );
	return 0;
}

int32_t Msg_AddButton( tGSe_MsgBox* MsgBox, const char* text, MsgBox_cb cb, void* arg ) {

	if( MsgBox->numButtons < (GS_ARRAYELEMENTS( MsgBox->Buttons ) - 1) ) {
		tGse_MsgBoxButton* ActButton = &MsgBox->Buttons[MsgBox->numButtons];
		ActButton->Text = strdup( text );
		ActButton->cb = cb;
		ActButton->cb_arg = arg;
		(MsgBox->numButtons)++;
		return 0;
	}
	return -1;
}

int32_t Msg_Push( tGSe_MsgBox* MsgBox ) {
	return gsMsgFiFoWrite( msb.Fifo, MsgBox, sizeof( tGSe_MsgBox ), 5 );
}

int32_t MsgBoxOk( const char* headline, const char* text, MsgBox_cb cb, void* arg ) {
	tGSe_MsgBox BoxOK;
	Msg_Create( &BoxOK, headline, text );
	Msg_AddButton( &BoxOK, "OK", cb, arg );
	Msg_Push( &BoxOK );
	return 0;
}

int32_t MsgBoxOK_RCText( uint32_t Headline_RCText, uint32_t Text_RCText, MsgBox_cb cb, void* arg ) {
	uint32_t lng = GetVarIndexed( IDX_SYS_LANGUAGE );
	return MsgBoxOk(
		RCTextGetText( Headline_RCText, lng ),
		RCTextGetText( Text_RCText, lng ),
		cb,
		arg
	);
}

int32_t MsgBoxOkCancel( const char* headline, const char* text, MsgBox_cb ok_cb, void* ok_arg, MsgBox_cb cancel_cb, void* cancel_arg ) {
	tGSe_MsgBox		BoxOKCancel;
	const char*		Cancel = RCTextGetText( RCTEXT_T_CANCEL, GetVarIndexed( IDX_SYS_LANGUAGE ) );
	Msg_Create( &BoxOKCancel, headline, text );
	Msg_AddButton( &BoxOKCancel, "OK", ok_cb, ok_arg );
	Msg_AddButton( &BoxOKCancel, Cancel, cancel_cb, cancel_arg );
	Msg_Push( &BoxOKCancel );
	return 0;
}

int32_t MsgBoxOKCancel_RCText( uint32_t Headline_RCText, uint32_t Text_RCText, MsgBox_cb ok_cb, void* ok_arg, MsgBox_cb cancel_cb, void* cancel_arg ) {
	uint32_t lng = GetVarIndexed( IDX_SYS_LANGUAGE );
	return MsgBoxOkCancel(
		RCTextGetText( Headline_RCText, lng ),
		RCTextGetText( Text_RCText, lng ),
		ok_cb,
		ok_arg,
		cancel_cb,
		cancel_arg
	);
}

int32_t MsgBox_IsAnyBoxActive( void ) {
	if( msb.Actual )
		return 1;
	return 0;
}
