/****************************************************************************
*
* File:         GSESCROLLLIST.c
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
#include "gsescrolllist.h"
#include "gsToVisu.h"
#include "gsLkfCodes.h"
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
tGSeScrollList* ScrList_Create( void* arg ) {
	tGSeScrollList* sl = malloc( sizeof( tGSeScrollList ) );
	if( NULL == sl )
		return NULL;
	memset( sl, 0, sizeof( tGSeScrollList ) );
	sl->Line.dy = 20;
	sl->arg = arg;
	sl->Frame.id = -1;
	return sl;
}

void ScrList_Destroy( tGSeScrollList** sl ) {
	if( NULL != *sl ) {
		free( *sl );
		*sl = NULL;
	}
}

void ScrList_Init_SetContainer( tGSeScrollList* sl, uint32_t PosX, uint32_t PosY ) {
	sl->Container.id = 0;
	sl->Container.x = PosX;
	sl->Container.y = PosY;
}

void ScrList_Init_SetFrame( tGSeScrollList* sl, uint32_t Height, uint32_t Width ) {
	sl->Frame.dy = Height;
	sl->Frame.dx = Width;
}

void ScrList_Init_SetFrameObjId( tGSeScrollList* sl, int32_t ObjID ) {
	if( ObjID >= 0 )
		sl->Frame.id = ObjID;
}

void ScrList_Init_SetLinePositions( tGSeScrollList* sl, uint32_t y_pos_FirstLine, uint32_t dy_Lines ) {
	sl->Line.first_ypos = y_pos_FirstLine;
	sl->Line.dy = dy_Lines;
	if( 0 == sl->Line.dy ) {
		sl->Line.dy = 20;
	}
}

void ScrList_Init_SetNumShownLines( tGSeScrollList* sl, uint32_t num ) {
	sl->Line.num = num;
}

int32_t ScrList_GetSelLine( tGSeScrollList* sl ) {
	return sl->Line.selected;
}

int32_t ScrList_GetOffset( tGSeScrollList* sl ) {
	return sl->Line.offset;
}

void ScrList_Init_SetSelect_cb( tGSeScrollList* sl, ScrList_cb cb ) {
	sl->Select_cb = cb;
}

int32_t ScrList_Draw( tGSeScrollList* sl ) {
	int32_t ypos;
	tLines* Line = &sl->Line;
	if( 0 <= Line->selected ) {
		ypos = Line->first_ypos + Line->selected * Line->dy;

	}
	else
		ypos = -1000;
	SendToVisuObj( sl->Frame.id, GS_TO_VISU_SET_ORIGIN_Y, ypos );
	return ypos;
}

void ScrList_DeSelect( tGSeScrollList* sl ) {
	sl->Line.selected = -1;
	sl->Line.offset = 0;
	sl->Line.offset_old = 0;
}

void ScrList_Select( tGSeScrollList* sl, int32_t NewSelection ) {
	if( NewSelection < 0 ) {
		NewSelection = 0;

		if( sl->Line.offset ) {
			ScrList_SetOffset( sl, ScrList_GetOffset( sl ) - 1 );
		}

	}
	if( NewSelection >= sl->Line.num ) {
		NewSelection = sl->Line.num - 1;
		int32_t offset = sl->Line.offset;
		ScrList_SetOffset( sl, offset + 1 );
	}
	if( NewSelection >= (int32_t)sl->ListLines ) {
		NewSelection = sl->ListLines - 1;
	}
	if( sl->Line.selected != NewSelection ) {
		sl->Line.selected = NewSelection;
		if( NULL != sl->Select_cb )
			sl->Select_cb( sl->arg );
	}
}

void ScrList_SetOffset( tGSeScrollList* sl, uint32_t offset ) {
	sl->Line.offset = offset;
	if( sl->Line.offset > (int32_t)(sl->ListLines - sl->Line.num) ) {
		sl->Line.offset = sl->ListLines - sl->Line.num;
	}
	if( sl->Line.offset < 0 )
		sl->Line.offset = 0;
}

void ScrList_Init_SetEnter_cb( tGSeScrollList* sl, ScrListEnter_cb cb ) {
	sl->Enter_cb = cb;
}

void ScrList_Enter( tGSeScrollList* sl ) {
	if( ScrList_GetSelLine( sl ) >= 0 ) {
		if( NULL != sl->Enter_cb )
			sl->Enter_cb( sl->arg, ScrList_GetSelLine( sl ), ScrList_GetOffset( sl ) );
	}

}

void ScrList_Cycle( tGSeScrollList* sl, uint32_t evtc, tUserCEvt* evtv, int32_t numListLines ) {
	sl->ListLines = numListLines;
	for( uint32_t i = 0; i < evtc; i++ ) {
		if( CEVT_TOUCH == evtv[i].Type ) {
			//Scroll
			if( evtv[i].Content.mTouch.mId == 0 ) {
				evtv[i].Content.mTouch.mX = evtv[i].Content.mTouch.mX - sl->Container.x;
				evtv[i].Content.mTouch.mY = evtv[i].Content.mTouch.mY - sl->Container.y;

				if( evtv[i].Content.mTouch.mState == CEVT_TOUCH_DOWN ) {
					sl->TouchDown.x = evtv[i].Content.mTouch.mX;
					sl->TouchDown.y = evtv[i].Content.mTouch.mY;
				}
				else {
					int32_t dy = sl->TouchDown.y - evtv[i].Content.mTouch.mY;

					if( 0 != sl->Frame.dy )
						dy = dy / (int32_t)sl->Frame.dy;
					else dy = dy / 20;
					if( dy ) //Scroll
					{
						static int32_t dy_old = 0;
						if( dy != dy_old ) {
							dy_old = dy;
							ScrList_SetOffset( sl, (int32_t)(sl->Line.offset_old + dy) );
							sl->Select_cb( sl->arg );
							ScrList_Draw( sl );

						}
						if( evtv[i].Content.mTouch.mState == CEVT_TOUCH_UP ) {
							sl->Line.offset_old = sl->Line.offset;
						}
					}
					else if( evtv[i].Content.mTouch.mState == CEVT_TOUCH_UP ) //Select
					{
						if( (evtv[i].Content.mTouch.mY >= (int32_t)sl->Line.first_ypos)
						&& (evtv[i].Content.mTouch.mY <= (int32_t)(sl->Line.first_ypos + sl->Line.dy * sl->Line.num)) ) {
							int32_t NewSelection = (evtv[i].Content.mTouch.mY - sl->Line.first_ypos) / sl->Line.dy;

							ScrList_Select( sl, NewSelection );
							ScrList_Enter( sl );
							ScrList_Draw( sl );

						}
					}
				}
			}
		}
		if( CEVT_VISU_KEY == evtv[i].Type ) {
			if( CEVT_STATE_PRESS == evtv[i].Content.mVisuKey.mState ) {

				if( (GS_VISU_LKF_MOVELEFT == evtv[i].Content.mVisuKey.mLogicFunc)
				|| (GS_VISU_LKF_MOVEUP == evtv[i].Content.mVisuKey.mLogicFunc) ) {
					ScrList_Select( sl, sl->Line.selected - 1 );
					sl->Select_cb( sl->arg );
					ScrList_Draw( sl );
				}
				else if( (GS_VISU_LKF_MOVERIGHT == evtv[i].Content.mVisuKey.mLogicFunc)
				|| (GS_VISU_LKF_MOVEDOWN == evtv[i].Content.mVisuKey.mLogicFunc) ) {
					ScrList_Select( sl, sl->Line.selected + 1 );
					sl->Select_cb( sl->arg );
					ScrList_Draw( sl );
				}
				else if( GS_VISU_LKF_ENTER == evtv[i].Content.mVisuKey.mLogicFunc ) {
					ScrList_Enter( sl );
					ScrList_Draw( sl );
				}

			}
		}
	}

}
