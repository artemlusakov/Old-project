#ifndef SQL_TABLE_H
#define SQL_TABLE_H
#include "visu.h"

#define PATH_SQL_FLASH "/gs/data/SQLList.cfg"
#define PATH_SQL_USB "/gs/usb/BACKUP_LAST/SQLList.cfg"

typedef struct tagSQL_Table {
	const char* TableName;     //name of the table
	const char* ColumnNames[4];//name of the columns
	int32_t     Cell[11][4];   //Cells with the values
	int32_t     idx;
}tSQLTable;

typedef struct Visu_SQL_Table {
	uint32_t ObjId[11][4];
	uint32_t Hdl[11][4];
}tVisu_SQL_Table;

/**
* @brief Initialises the Objects for the Mask MSK_SQL_LIST.
*
* @param Data not used
*/
void Visu_SQL_Table_Init( const tVisuData* Data );

/**
* @brief Opens the Mask MSK_SQL_LIST and shows the data of the first initalised
* table by the function SQL_Tabel_Init
*
* @param Data
*/
void Visu_SQL_Table_Open( const tVisuData* Data );

/**
* @brief Cycle function for MSK_SQL_LIST.
* Key 1: shows previeus table
* Key 2: shows next table
* Key 3: saves the changes done on the mask
* Key 4: Values of column 3 will be recalculated. the values between the first and the last
* row will be linerised.
* Example:
* 0  -> 0
* 5  -> 2
* 8  -> 4
* 5  -> 6
* 9  -> 8
* 0  ->10
* 2  ->12
* 8  ->14
* 8  ->16
* 9  ->18
* 20 ->20
* @param Data
* @param evtc
* @param evtv
*/
void Visu_SQL_Table_Cycle( const tVisuData* Data, uint32_t evtc, tUserCEvt* evtv );

/**
* @brief Initalises a SQL-Table. If a table is initialized the first time on a device,
* the Data will be safed in a SQLite-File on the flash of the device. the Data for this
* table has to be defined in the given parameter table. If the table already exists,
* the data will be read out of the flash.
*
* @param Table filled structure of a table. The values entered in the structure will be
* used as default values.
* @return tSQLTable* Pointer to a Table in the flash, with the actual values out of the
* SQL-File. this Pointer does not show on the same data like the parameter Table.
*/
tSQLTable* SQL_Tabel_Init( tSQLTable* Table );

/**
* @brief This function will interpolate a value between two given values in a table.
* the first and the last line of the column y will give the maximum and minimum return value.
*
* @param Table Pointer to the table.
* @param ci_x column of the x value
* @param ci_y column of the y value
* @param x
* @return int32_t y = mx + b;
*/
int32_t TableGetLSEValue( tSQLTable* Table, uint32_t ci_x, uint32_t ci_y, int32_t x );

/**
* @brief Closes the used SQL-Table. Use this in the end of the Init
*
*/
void Visu_SQL_Vin_Equip (void);
void SQL_Close( void );
void RpSQL(void);
void WrSQL(void);
/*************************************************************************/
/* Protection against multiple includes ends here ! Stop editing here !  */
/*************************************************************************/
#endif  // #ifndef SQL_TABLE_H
