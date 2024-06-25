/****************************************************************************
*
* File:         gseList.c
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
#include<UserCAPI.h>
#include "gselist.h"
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
*****************************************************************************/#
void GSList_Init( tGSList* List ) {
	memset( List, 0, sizeof( tGSList ) );
}

tGSList_Item* GSList_AddItem( tGSList* List, void* data, size_t size ) {
	tGSList_Item* Item = malloc( sizeof( tGSList_Item ) );
	if( NULL == Item ) {
		return NULL;
	}
	memset( Item, 0, sizeof( tGSList_Item ) );
	Item->data = malloc( size );
	Item->size = size;
	memcpy( Item->data, data, size );
	//in Liste eingliedern
	if( NULL == List->first ) {
		List->first = Item;
		List->last = Item;
		List->act = Item;
		List->numItems = 1;
	}
	else {
//@		tGSList_Item* actItem = List->act;
		tGSList_Item* actItem = List->last;
		tGSList_Item* NextItem = actItem->next;
		if( NULL != NextItem ) {
			NextItem->prev = Item;
			Item->next = NextItem;
		}

		actItem->next = Item;
		Item->prev = actItem;
		List->numItems++;
		if( NULL == Item->next ) {
			List->last = Item;
		}
		if( NULL == Item->prev ) {
			List->first = Item;
		}
	}
	return Item;
}

tGSList_Item* GSList_RemoveItem( tGSList* List, tGSList_Item* Item ) {

	if( Item != NULL ) {
		if( NULL != Item->next ) {
			Item->next->prev = Item->prev;
		}
		if( NULL != Item->prev ) {
			Item->prev->next = Item->next;
		}
		if( Item == List->first ) {
			List->first = Item->next;
		}
		if( Item == List->act ) {
			List->act = Item->next;
		}
		if( Item == List->last ) {
			List->last = Item->prev;
		}

		List->numItems--;
	}
	return Item;
}

void GSList_DeleteItem( tGSList* List, tGSList_Item* Item ) {
	Item = GSList_RemoveItem( List, Item );
	free( Item->data );
	free( Item );
	Item = NULL;
}

tGSList_Item* GSList_GetFirstItem( tGSList* List ) {
	List->act = List->first;
	return List->first;
}

tGSList_Item* GSList_GetLastItem( tGSList* List ) {
	List->act = List->last;
	return List->last;
}

tGSList_Item* GSList_GetNextItem( tGSList* List ) {
	if( NULL != List->act ) {
		if( NULL != List->act->next ) {
			List->act = List->act->next;
		}
		else return NULL;
	}
	return List->act;
}

tGSList_Item* GSList_GetPrevItem( tGSList* List ) {
	if( NULL != List->act ) {
		if( NULL != List->act->prev ) {
			List->act = List->act->prev;
		}
		else return NULL;
	}
	return List->act;
}

void* GSList_GetData( tGSList_Item* Item ) {
	if( NULL != Item ) {
		return Item->data;
	}
	else return NULL;
}

tGSList_Item* GSList_SearchItem( tGSList* List, void* arg, tListSearchData_cb SearchData_cb ) {
	register tGSList_Item* Item = GSList_GetFirstItem( List );
	while( Item ) {
		if( 0 == SearchData_cb( Item->data, arg ) ) {
			return Item;
		}
		else Item = GSList_GetNextItem( List );
	}
	return NULL;
}

void* GSList_AddData( tGSList* List, void* data, size_t size ) {
	return GSList_GetData( GSList_AddItem( List, data, size ) );
}

void* GSList_AddDataEnd( tGSList* List, void* data, size_t size ) {
	List->act = List->last;
	return GSList_GetData( GSList_AddItem( List, data, size ) );
}

void* GSList_SearchData( tGSList* List, void* data, tListSearchData_cb SearchData_cb ) {
	register tGSList_Item* Item = GSList_GetFirstItem( List );
	if( NULL == SearchData_cb ) {
		while( NULL != Item ) {
			List->act = Item;
			if( Item->data == data ) {
				return Item->data;
			}
			Item = Item->next;
		}
	}
	else {
		while( NULL != Item ) {
			List->act = Item;
			if( 0 == SearchData_cb( data, Item->data ) ) {
				return Item->data;
			}
			Item = Item->next;
		}
	}
	return NULL;
}

void* GSList_RemoveData( tGSList* List, void* data ) {
	if( NULL != GSList_SearchData( List, data, NULL ) ) {
		GSList_RemoveItem( List, List->act );
		return data;
	}
	return NULL;
}

void GSList_DeleteData( tGSList* List, void* data ) {
	if( NULL != GSList_SearchData( List, data, NULL ) ) {
		GSList_DeleteItem( List, List->act );
	}
}

void* GSList_GetFirstData( tGSList* List ) {
	return GSList_GetData( GSList_GetFirstItem( List ) );
}

void* GSList_GetLastData( tGSList* List ) {
	return GSList_GetData( GSList_GetLastItem( List ) );
}

void* GSList_GetNextData( tGSList* List ) {
	return GSList_GetData( GSList_GetNextItem( List ) );
}

void* GSList_GetPrevData( tGSList* List ) {
	return GSList_GetData( GSList_GetPrevItem( List ) );
}

void GSList_SwapData( tGSList_Item* item_a, tGSList_Item* item_b ) {
	void* data = item_a->data;
	item_a->data = item_b->data;
	item_b->data = data;
}
void GSList_SortData( tGSList* List, tListSortData_cb SearchData_cb ) {
	tGSList_Item* Item = List->first;
	while( Item != NULL ) {
		tGSList_Item* SortItem = Item;
		if( SortItem->next != NULL ) {
			while( 0 > SearchData_cb( SortItem->data, SortItem->next->data ) ) {
				GSList_SwapData( SortItem, SortItem->next );
				SortItem = SortItem->prev;
				if( NULL == SortItem ) {
					break;
				}
			}
		}
		Item = Item->next;
	}
}

void GSList_Destroy( tGSList* List ) {
	while( NULL != List->first ) {
		GSList_DeleteItem( List, List->first );
	}
}
