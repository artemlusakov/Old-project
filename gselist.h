/************************************************************************
*
* File:         GS_LIST.h
* Project:
* Author(s):
* Date:
*
* Description: Lib for doulbe linked list.
* This lib is not thread save. This lib is for devices not supporting cpp
* with the STL.
*
*
*
*
*
************************************************************************/

 /************************************************************************/
 /* Protection against multiple includes.                                */
 /* Do not code anything outside the following defines (except comments) */
#ifndef GSELIST_H
#define GSELIST_H
#include "stdint.h"
/*************************************************************************/

/* macro definitions *****************************************************/

/* type definitions ******************************************************/

/* prototypes ************************************************************/

/* global constants ******************************************************/

/* global variables ******************************************************/

/* global function prototypes ********************************************/
typedef struct tagGSList_Item tGSList_Item;

struct tagGSList_Item {
	void* data; //pointer to the saved data
	size_t        size; //sizeo of the saved data
	tGSList_Item* next; //pointer to the next item in the list, NULL if this is the last item
	tGSList_Item* prev; //pointer to the previeous item, NULL if this is the first item
};

typedef struct tagGSList {
	tGSList_Item* first; //pointer to the first item
	tGSList_Item* last;  //pointer to the last item
	tGSList_Item* act;   //pointer to the actual selected item.
	uint32_t      numItems; //Number of items in the list
}tGSList;

/**
* @brief Callback function for a search data function. This function will be
* called by GSList_SearchItem(). In the callback function you can check for
* the parameter arg in the data containing the different items.
*
* @param data  Pointer to data saved in the item of a list.
* @param arg   This should be the data you are looking for in the list items.
* return 0 if the function found.
*
*/
typedef int32_t( *tListSearchData_cb )(void* data, void* arg);

typedef int32_t( *tListSortData_cb )(void* data_0, void* data_1);

/**
* @brief Initalizes a List.
*
* @param List
*/
void GSList_Init( tGSList* List );

/**
* @brief Saves a Copy of Data in the list.
*
* @param List pointer to the list
* @param data pointer to the data to be saved in the list. Memory for the new
* element will be allocated automatically
* @param size Size of the new element.
* @return void* returns a pointer to the saved data.
*/
void* GSList_AddData( tGSList* List, void* data, size_t size );

void* GSList_AddDataEnd( tGSList* List, void* data, size_t size );

/**
* @brief Searches for the Pointer of Data and removes it from the list.
* The data is not freed.
* @param List Pointer to a list
* @param Data Pointer to the data, which shall be removed from the list
* @return void* returns the pointer to the data, that has been removed from the list
* or NULL if the data has not been found
*/
void* GSList_RemoveData( tGSList* List, void* Data );

/**
* @brief Removes Data from the list and frees it.
*
* @param List Pointer to a list
* @param Data Pointer to the data, which shall be removed from the list
*/
void           GSList_DeleteData( tGSList* List, void* Data );

/**
* @brief Returns the first data saved in the list
*
* @param List Pointer to a list
* @return void* Pointer to the first Data in the list,
* NULL if no data is saved
*/
void* GSList_GetFirstData( tGSList* List );

/**
* @brief Returns the last data saved in the list
*
* @param List Pointer to a list
* @return void* Pointer to the first Data in the list,
* NULL if no data is saved
*/
void* GSList_GetLastData( tGSList* List );

/**
* @brief Returns the next data saved in the list
*
* @param List Pointer to a list
* @return void* Pointer to the next Data in the list,
* NULL if there is no next data.
*/
void* GSList_GetNextData( tGSList* List );

/**
* @brief Returns the previous data saved in the list
*
* @param List Pointer to a list
* @return void* Pointer to the previous Data in the list,
* NULL if there is no previous data.
*/
void* GSList_GetPrevData( tGSList* List );

/**
* @brief searches for data in the list.
*
* @param List Pointer to a list
* @param arg  if SearchData_cb == NULL, arg has to be the same type
* like the data saved in the list. Else it is the argument for SearchData_cb
* @param SearchData_cb Callback funktion for searching a object
* @return void* data which has been found or NULL if nothing has been found
*/
void* GSList_SearchData( tGSList* List, void* arg, tListSearchData_cb SearchData_cb );

void          GSList_SortData( tGSList* List, tListSortData_cb SearchData_cb );

/**
* @brief Destroys a list and deletes all frees in it.
*
* @param List
*/
void           GSList_Destroy( tGSList* List );

/*************************************************************************/
/* Protection against multiple includes ends here ! Stop editing here !  */
/*************************************************************************/
#endif  // #ifndef GSELIST_H
