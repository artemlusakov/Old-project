/************************************************************************
*
* File:         USERJ1939.h
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
#ifndef USERJ1939_H
#define USERJ1939_H

#include <j1939/lib-j1939.h>

/*************************************************************************/

/* macro definitions *****************************************************/

/* type definitions ******************************************************/

/* prototypes ************************************************************/

/* global constants ******************************************************/

/* global variables ******************************************************/

/* global function prototypes ********************************************/
typedef struct tagECUName {
	uint8_t  ArbAdrCap;      /* arbitrary address capable */
	uint8_t  IndGroup;       /* industry group */
	uint8_t  VehSysInst;     /* vehicle system instance */
	uint8_t  VehSys;         /* vehicle system */
	uint8_t  Func;           /* function */
	uint8_t  FuncInst;       /* function instance */
	uint8_t  ECUInst;        /* ECU instance */
	uint16_t ManufCode;      /* Manufacturer code */
	uint8_t  IdNumber;       /* identity number */

}tECUName;

typedef struct  tagPG {
	_j1939_pg_hdl	pgHdl;
	_j1939_pg		pg;
	uint8_t*		data;
	size_t			size;
}tPG;

void    UserJ1939_Create( uint8_t CAN );
void    UserJ1939_Destroy( void );
int32_t UserJ1939_Add_ECU( tECUName* pECUName );
int32_t UserJ1939_Add_ECU_Fix_Address( int32_t FixAddress );
int32_t UserJ1939_Add_This_ECU( tECUName* pECUName, int32_t FixAddress );
void    UserJ1939_Start( void );

int32_t UserJ1939_Add_SendPGN( int32_t pgn, int32_t ECUHdl_Dest, tPG* pg, int32_t broadcast, int32_t prio, int32_t rate_ms, size_t DataSize );
int32_t UserJ1939_Add_RecvPGN( int32_t pgn, int32_t ECUHdl_Source, tPG* pg, int32_t broadcast, int32_t prio, int32_t rate_ms, size_t DataSize );

int32_t UserJ1939_PGN_Req( tPG* pg );
int32_t UserJ1939_PGN_Send( tPG* pg );
void UserJ1939_PGN_GetData( tPG* pg, uint8_t* data, size_t len );
void UserJ1939_PGN_SetData( tPG* pg, uint8_t* data, size_t len );
uint32_t UserJ1939_GetTimestamp( void );
uint32_t UserJ1939_ECU_Active( uint32_t ECU_Hdl );

/*************************************************************************/
/* Protection against multiple includes ends here ! Stop editing here !  */
/*************************************************************************/
#endif  // #ifndef USERJ1939_H
