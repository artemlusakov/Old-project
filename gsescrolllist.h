/************************************************************************
*
* File:         GSESCROLLLIST.h
* Project:
* Author(s):
* Date:
*
* Description: This lib helps to visualize a list in the GSe-VISU. It will
* draw a frame above your list, which shows, which line is selected. By
* moving the frame by touch or Keyboard Input, you can scroll through the
* list. The Lib can tell you, offset and Position of selected lines and trigger
* actions for selecting or entering a line.
* The drawing of the list NOT be done by this lib.
*

************************************************************************/

 /************************************************************************/
 /* Protection against multiple includes.                                */
 /* Do not code anything outside the following defines (except comments) */
#ifndef GSESCROLLLIST_H
#define GSESCROLLLIST_H
/*************************************************************************/

/* macro definitions *****************************************************/

/* type definitions ******************************************************/

/* prototypes ************************************************************/

/* global constants ******************************************************/

/* global variables ******************************************************/

/* global function prototypes ********************************************/
typedef void (*ScrList_cb)(void* arg);
typedef void (*ScrListEnter_cb)(void* arg, int32_t Selection, int32_t Offset);

typedef struct tagLines {
	int32_t num;
	int32_t selected;
	int32_t offset;
	int32_t offset_old;
	uint32_t first_ypos;
	uint32_t dy;
}tLines;

typedef struct tagVek {
	int32_t x;
	int32_t y;
}tVek2;

typedef struct tagGSeSquare {
	uint32_t id;
	int32_t x;
	int32_t y;
	uint32_t dx;
	uint32_t dy;
}tGSeSquare;

typedef struct tagGSeScrollList {
	tGSeSquare      Container;
	tGSeSquare      Frame;
	tLines          Line;
	tVek2           TouchDown;
	void* arg;
	uint32_t        ListLines;
	ScrList_cb      Select_cb;
	ScrList_cb      Scroll_cb;
	ScrListEnter_cb Enter_cb;
}tGSeScrollList;

/**
* @brief Creates a Scrolllist. Returns the Pointer to it.
*
* @param arg Parameter given to Callback Functions ScrListEnter_cb and ScrList_cb
* @return tGSeScrollList*
*/
tGSeScrollList* ScrList_Create( void* arg );

/**
* @brief If the List is used in a contianer, use this to set the offset. This is
* needed for Use by touch.
*
* @param sl Pointer to the List
* @param PosX X-Position of the container
* @param PosY Y-Position of the container
*/
void ScrList_Init_SetContainer( tGSeScrollList* sl, uint32_t PosX, uint32_t PosY );

/**
* @brief Sets the Size of a selection frame. For the selection  a Object is
* used which will be moved to the positon of the selected text. Use a Bitmap
* as Frame Object.
*
* @param sl
* @param Height Height of the Object, which is used as frame
* @param Width Width of the Object, which is used as frame.
*/
void ScrList_Init_SetFrame( tGSeScrollList* sl, uint32_t Height, uint32_t Width );

/**
* @brief Sets the Object ID of the Frame object.
*
* @param sl
* @param ObjID
*/
void ScrList_Init_SetFrameObjId( tGSeScrollList* sl, int32_t ObjID );

/**
* @brief Sets the Position of the Text-Objects in the Scrolllist. All List Entries
* need the same y-distance.
*
* @param sl
* @param y_pos_FirstLine Y-Positon of the first line in the Visu
* @param dy_Lines        distance between two Lines in the visu.
*/
void ScrList_Init_SetLinePositions( tGSeScrollList* sl, uint32_t  y_pos_FirstLine, uint32_t dy_Lines );

/**
* @brief Sets the number of shown Lines in the list.
*
* @param sl
* @param num
*/
void ScrList_Init_SetNumShownLines( tGSeScrollList* sl, uint32_t num );

/**
* @brief Sets the Callback function for Enter. This is called if a User presses Enter
* and a Line of the List is selected.
*
* @param sl
* @param cb
*/
void ScrList_Init_SetEnter_cb( tGSeScrollList* sl, ScrListEnter_cb cb );
void ScrList_Init_SetSelect_cb( tGSeScrollList* sl, ScrList_cb cb );

/**
* @brief returns the actual selected line in the visu. You have to add the offset
* to get the selected Entry in your list.
*
* @param sl
* @return int32_t
*/
int32_t  ScrList_GetSelLine( tGSeScrollList* sl );

/**
* @brief returns the offset of the list.
*
* @param sl
* @return int32_t
*/
int32_t  ScrList_GetOffset( tGSeScrollList* sl );

/**
* @brief Sets the offset of the scrolllist.
*
* @param sl
* @param offset
*/
void ScrList_SetOffset( tGSeScrollList* sl, uint32_t offset );

/**
* @brief Draws the frame of the list
*
* @param sl
* @return int32_t
*/
int32_t ScrList_Draw( tGSeScrollList* sl );

/**
* @brief Deselects a Line. The Scroll Bar will go to a negative position.
*
* @param sl
*/
void ScrList_DeSelect( tGSeScrollList* sl );
void ScrList_Select( tGSeScrollList* sl, int32_t NewSelection );

/**
* @brief Reads actual events to move the frame by touch and standard keys.
*
* @param sl
* @param evtc
* @param evtv
* @param numListLines Number of Entries in the List
*/
void ScrList_Cycle( tGSeScrollList* sl, uint32_t evtc, tUserCEvt* evtv, int32_t numListLines );

/*************************************************************************/
/* Protection against multiple includes ends here ! Stop editing here !  */
/*************************************************************************/
#endif  // #ifndef GSESCROLLLIST_H
