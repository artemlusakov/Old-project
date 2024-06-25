/****************************************************************************
*
* File:         USERJ1939.c
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

#include "userj1939.h"

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

static _j1939_hdl J1939_Hdl = NULL;
static uint8_t Can;
static uint32_t timestamp_rx_msg = 0;

typedef struct tagECU {
	_j1939_name name;
	int8_t AddPGs;
	int32_t numPGs;
	tPG** pPGs; /* J1939-stack PG handle */
	uint32_t timer;
} tECU;

int32_t ThisECUHdl = 0;
int32_t ThisPrefAddr = 0;

tECU* ECUs = NULL;
int32_t numECUs = 0;

static char _cgm_pgnToken[4] = "PT";
static char _cgm_pgnName[4] = "PN";
//static char _cgm_spnName[4]  = "SN";

static void _cgf_ErrorCb_0( void* arg, _j1939_err_type type, const char* text ) {

	char buffer[128];
	switch( type ) {
	case J1939_ERR_FATAL: snprintf( buffer, sizeof( buffer ), "J1939 Fatal Error: %s", text ); break;
	case J1939_ERR_ERROR: snprintf( buffer, sizeof( buffer ), "J1939 Error: %s", text ); break;
		//case J1939_ERR_WARNING : snprintf(buffer, sizeof(buffer), "J1939 Warning: %s", text); break;
	case J1939_ERR_WARNING: return; break;
	default: buffer[0] = '\0'; return;
	}
//	db_out( buffer );

}

int32_t UserJ1939_Add_ECU( tECUName* pECUName ) {
	tECU* newECUs = realloc( ECUs, sizeof( tECU ) * (numECUs + 1) );
	if( NULL != newECUs ) {
		ECUs = newECUs;
		int32_t idx = numECUs;
		numECUs++;
		ECUs[idx].name = j1939_name( pECUName->ArbAdrCap, /* arbitrary address capable */
			pECUName->IndGroup, /* industry group */
			pECUName->VehSysInst, /* vehicle system instance */
			pECUName->VehSys, /* vehicle system */
			pECUName->Func, /* function */
			pECUName->FuncInst, /* function instance */
			pECUName->ECUInst, /* ECU instance */
			pECUName->ManufCode, /* Manufacturer code */
			pECUName->IdNumber ); /* identity number */

		ECUs[idx].AddPGs = 0;
		ECUs[idx].numPGs = 0;
		ECUs[idx].pPGs = NULL;
		return idx;
	}
	else {
//		db_out( "Error allocating space for ECU" );
	}
	return -1;
}

int32_t UserJ1939_Add_ECU_Fix_Address( int32_t FixAddress ) {
	if( NULL != J1939_Hdl ) {

		_j1939_name* LookUpName = NULL;
		j1939_lkup_name( J1939_Hdl, Can, FixAddress, LookUpName );

		uint8_t can;
		_j1939_addr addr;
		for( int32_t i = 0; i < numECUs; i++ ) {
			int ret = j1939_lkup_addr( J1939_Hdl, ECUs[i].name, &can, &addr );
			if( ret == 1 ) {
				if( addr == FixAddress ) {
					return i;
				}
			}
		}
		return -1;
	}
	return -1;
}

static void _cgf_NwmevCb_0( _j1939_nwm_event event, void* event_data, void* arg ) {
	//Neuer Address-Claim:
	//if(NULL ==ECUs) return;
	switch( event ) {
	case J1939_NWM_COMMANDED_ADDRESS: {
		// _j1939_nwm_commanded_address* evtca = event_data;
//		db_out( "Commanded Address:%d, CAN %d, Source = 0x%X, Dest = 0x%X\r\n", evtca->commaddr, evtca->controller, evtca->srcaddr, evtca->destname );
	}
									break;
	case J1939_NWM_ADDRESS_CLAIMED: {
		_j1939_nwm_address_claimed* evtac = (_j1939_nwm_address_claimed*)event_data;
		uint64_t claimedName = (evtac->name & ~0x00000000001FFFFFull);
		for( int32_t i = 0; i < numECUs; i++ ) {
			if( claimedName == ECUs[i].name ) {
				ECUs[i].AddPGs = 1;
				break;
			}
		}
	}
								  break;
	case J1939_NWM_CANNOT_CLAIM_ADDRESS: {
		// _j1939_nwm_cannot_claim_address* evtcca = event_data;
//		db_out( "Cannot Claim Address 0x%X, Name 0x%X\r\n", evtcca->controller, evtcca->name );

	}
									   break;
	case J1939_NWM_LOOKUP_ADDR_FAILED: {
		// _j1939_nwm_lookup_addr_failed* evtcca = event_data;
//		db_out( "Lookup Address Failed , Name 0x%X\r\n", evtcca->name );
	}
									 break;
	case J1939_NWM_LOOKUP_NAME_FAILED: {
		// _j1939_nwm_lookup_name_failed* evtcca = event_data;
//		db_out( "Lookup Name Failed. Address: 0x%X CAN: %d\r\n", evtcca->addr, evtcca->controller );
		tECUName ECUName;

		ECUName.ArbAdrCap = 0;
		ECUName.IndGroup = 7;
		ECUName.VehSysInst = 15;
		ECUName.VehSys = 127;
		ECUName.Func = 255;
		ECUName.FuncInst = 31;
		ECUName.ECUInst = 7;
		ECUName.ManufCode = 338;
		// ECUName.IdNumber = evtcca->addr;
		int32_t ECUHdl;
//		db_out( "Addres already exists.\r\n" );

		if( -1 == (ECUHdl = UserJ1939_Add_ECU( &ECUName )) )
			return;

		// if( 0 != j1939_add_static_addr( J1939_Hdl, Can, ECUs[ECUHdl].name, evtcca->addr ) ) {
			//db_out("Error UserJ1939_Add_ECU_Fix_Address: Can't set fix address");
		// }

	}
									 break;
	default:
		break;
	}
}

int32_t UserJ1939_Add_This_ECU( tECUName* pECUName, int32_t FixAddress ) {
	ThisECUHdl = UserJ1939_Add_ECU( pECUName );
	ThisPrefAddr = FixAddress;
	return ThisECUHdl;
}

void UserJ1939_Start( void ) {
	if( NULL != J1939_Hdl ) {
		if( 0 == j1939_claim_name( J1939_Hdl, ThisECUHdl, ThisPrefAddr ) ) {
			/* Error */
//			db_out( "Error UserJ1939_Start: Can't claim Name" );
		}
	}
}

static int UserJ1939_req_cb( _j1939_pg_hdl pg_hdl, _j1939_name source, void* arg ) {
	if( NULL != J1939_Hdl ) {
		for( int32_t i = 0; i < numECUs; i++ ) {
			if( ECUs[i].name == source ) {

				for( int32_t j = 0; j < ECUs[i].numPGs; j++ ) {
					if( pg_hdl == ECUs[i].pPGs[j]->pgHdl ) {
						tPG* pg = arg;
						if( 0 == j1939_wr_pg( J1939_Hdl, pg->pgHdl, pg->data, pg->size, 0, 0 ) ) {
							; /* error */
						}
						return 1;
					}
				}
			}
		}
		return 0;
	}
	return 0;
}

static int UserJ1939_rcv_cb( _j1939_pg_hdl pg_hdl, _j1939_name source, uint8_t* data, int len, void* arg ) {
	if( NULL != J1939_Hdl ) {
		timestamp_rx_msg = GetMSTick();
		for( int32_t i = 0; i < numECUs; i++ ) {
			if( ECUs[i].name == source ) {

				ECUs[i].timer = GetMSTick();
				for( int32_t j = 0; j < ECUs[i].numPGs; j++ ) {
					if( pg_hdl == ECUs[i].pPGs[j]->pgHdl ) {
						tPG* pg = arg;
						memcpy( pg->data, data, len );
						return 1;
					}
				}
			}
		}
		return 0;
	}
	else return 0;
}

int32_t UserJ1939_Add_SendPGN( int32_t pgn, int32_t ECUHdl_Dest, tPG* pg, int32_t broadcast, int32_t prio, int32_t rate_ms, size_t DataSize ) {
	if( NULL != J1939_Hdl ) {
		pg->size = DataSize;
		pg->data = malloc( DataSize );
		memset( pg->data, 0xFF, DataSize );
		pg->pg.len = DataSize;
		pg->pg.num_sps = 0;
		pg->pg.sps = NULL;
		pg->pg.prio = prio;
		pg->pg.rate = rate_ms;
		pg->pg.token = _cgm_pgnToken;
		pg->pg.name = _cgm_pgnName;
		pg->pg.rcv_cb = UserJ1939_rcv_cb;
		pg->pg.req_cb = UserJ1939_req_cb;
		pg->pg.rcv_cb_arg = pg;
		pg->pg.req_cb_arg = pg;
		pg->pg.pgn = pgn;
		if( broadcast ) {
			pg->pg.target = 0xFFFFFFFFFFFFFFFFull;
		}
		else {
			pg->pg.target = ECUs[ECUHdl_Dest].name;
		}
		pg->pg.source = ECUs[ThisECUHdl].name;
		pg->pgHdl = j1939_add_pg( J1939_Hdl, &(pg->pg) );
		j1939_wr_pg( J1939_Hdl, pg->pgHdl, pg->data, pg->pg.len, 0, NULL );
		tPG** newPGs;
		newPGs = (tPG**)realloc( ECUs[ECUHdl_Dest].pPGs, sizeof( tPG* ) * (ECUs[ECUHdl_Dest].numPGs + 1) );
		if( NULL != newPGs ) {
			ECUs[ECUHdl_Dest].pPGs = newPGs;
			ECUs[ECUHdl_Dest].pPGs[ECUs[ECUHdl_Dest].numPGs] = pg;
			ECUs[ECUHdl_Dest].numPGs++;
		}
		return 1;
	}
	else return 0;
}

int32_t UserJ1939_Add_RecvPGN( int32_t pgn, int32_t ECUHdl_Source, tPG* pg, int32_t broadcast, int32_t prio, int32_t rate_ms, size_t DataSize ) {
	if( NULL != J1939_Hdl ) {
		pg->size = DataSize;
		pg->data = malloc( DataSize );
		memset( pg->data, 0xFF, DataSize );
		pg->pg.len = DataSize;
		pg->pg.num_sps = 0;
		pg->pg.sps = NULL;
		pg->pg.prio = prio;
		pg->pg.rate = 0;
		pg->pg.token = _cgm_pgnToken;
		pg->pg.name = _cgm_pgnName;
		pg->pg.rcv_cb = UserJ1939_rcv_cb;
		pg->pg.req_cb = UserJ1939_req_cb;
		pg->pg.rcv_cb_arg = pg;
		pg->pg.req_cb_arg = pg;
		pg->pg.pgn = pgn;
		if( broadcast ) {
			pg->pg.source = 0xFFFFFFFFFFFFFFFFull;
		}
		else {
			pg->pg.source = ECUs[ECUHdl_Source].name;
		}
		pg->pg.target = ECUs[ThisECUHdl].name;
		pg->pgHdl = j1939_add_pg( J1939_Hdl, &(pg->pg) );
		if( 0 == j1939_rd_pg( J1939_Hdl, pg->pgHdl, pg->data, &pg->size, 0, 0 ) ) {
			; /* error */
		}

		tPG** newPGs;
		newPGs = (tPG**)realloc( ECUs[ECUHdl_Source].pPGs, sizeof( tPG* ) * (ECUs[ECUHdl_Source].numPGs + 1) );
		if( NULL != newPGs ) {
			ECUs[ECUHdl_Source].pPGs = newPGs;
			ECUs[ECUHdl_Source].pPGs[ECUs[ECUHdl_Source].numPGs] = pg;
			ECUs[ECUHdl_Source].numPGs++;
		}
		return 1;
	}
	else return 0;
}

int32_t UserJ1939_PGN_Req( tPG* pg ) {
	if( NULL != J1939_Hdl ) {
		if( 0 == j1939_request_pg( J1939_Hdl, pg->pg.source, NULL, pg->pg.pgn, 0 ) ) {
			; /* error */
		}
		return 1;
	}
	else return 0;
}

void UserJ1939_PGN_GetData( tPG* pg, uint8_t* data, size_t len ) {
	if( NULL != J1939_Hdl ) {
		if( pg->size < len ) {
			len = pg->size;
		}
		memcpy( data, pg->data, len );
	}
}

void UserJ1939_PGN_SetData( tPG* pg, uint8_t* data, size_t len ) {
	if( NULL != J1939_Hdl ) {
		if( pg->size < len ) {
			len = pg->size;
		}
		if( 0 != memcmp( pg->data, data, len ) ) {
			memcpy( pg->data, data, len );
			if( 0 == j1939_wr_pg( J1939_Hdl, pg->pgHdl, pg->data, pg->size, 0, 0 ) ) {
				; /* error */
			}
			//j1939_
		}
	}
}

int32_t UserJ1939_PGN_Send( tPG* pg ) {
	if( NULL != J1939_Hdl ) {
		if( 0 == j1939_snd_pg( J1939_Hdl, pg->pgHdl, 0 ) ) {
			; /* error */
		}
	}
	return 1;
}

void UserJ1939_Create( uint8_t CAN ) {
	Can = CAN;
	if( 0 == (J1939_Hdl = j1939_init( &Can, 1 )) ) {
		/* Error */
//		db_out( "UserJ1939_Create: Error init J1939\r\n" );
	}
//	db_out( "Init J1939-Stack\r\n" );
	//Error callback

	if( 0 == j1939_set_err_cb( J1939_Hdl, &_cgf_ErrorCb_0, NULL ) ) {
		/* Error */
//		db_out( "UserJ1939_Create: Error setting debug func J1939\r\n" );
	}
	/* Setting network management event callback */
	if( 0 == j1939_set_nwm_cb( J1939_Hdl, &_cgf_NwmevCb_0, 0 ) ) {
		/* Error */
//		db_out( "UserJ1939_Create: Error setting nwm_cb\r\n" );
	}
//	db_out( "Added Error Streams" );
}

void UserJ1939_Destroy( void ) {
	/*  if(NULL != J1939_Hdl)
	  {
		j1939_exit(J1939_Hdl);
		J1939_Hdl = NULL;
	  }*/
}

uint32_t UserJ1939_GetTimestamp( void ) {
	return timestamp_rx_msg;
}

uint32_t UserJ1939_ECU_Active( uint32_t ECU_Hdl ) {
	if( ECU_Hdl < (uint32_t)numECUs ) {
		if( 2000 > ( GetMSTick() - ECUs[ECU_Hdl].timer ) ) {
			return 1;
		}
		else {
			return 0;
		}

	}
	return -1;
}
