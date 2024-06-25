/************************************************************************
*
* File:        gseMCM.h
* Project:		EasyLibs
* Author(s):	Marc Schartel
* Date:		16.01.2020
* Version:     0.2
*
* Description:
*
*
*
*
*
************************************************************************/
#include <stdint.h>
#include <UserCEvents.h>  /* Events send to the Cycle function              */
#include <UserCAPI.h>     /* API-Function declarations                      */
#include "vartab.h"

#include "gsemcm.h"
#include "gsThread.h"
#include "gsMsgFiFo.h"
#include "visu_mcm_config.h"

 /****************************************************************************/

 /* macro definitions ********************************************************/

 /* type definitions *********************************************************/
#define HANDLE_TEMP 0xFFF7
#define HANDLE_VOLT 0xFFF4
#define HANDLE_ROTENCODER_0 5000
#define HANDLE_ROTENCODER_1 5001
/* prototypes ***************************************************************/

/* global constants *********************************************************/

/* global variables *********************************************************/

/* local function prototypes*************************************************/
uint32_t	_numMCM = 0;
tGS_MCM* _pMCM = NULL;

tGsThread	_Thread = NULL;
tGsMutex	_Mutex = NULL;
int32_t		_Thread_Running = 0;

uint32_t	_FiFo;

uint32_t	_emcy;

/****************************************************************************/

/* function code ************************************************************/

void SendLed( tGS_MCM* MCM, eGSeMCM_Led_Color Color );
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

int32_t GSeMCM_SendDigitalOut( tGS_MCM* MCM ) {
	tGSeMCM_DO* DO;
	uint32_t DO_Hdl;

	switch( MCM->Type ) {
	case MCMTYPE_MCM200:
		DO_Hdl = MCM->MCM.MCM200.digOutHnd;
		DO = &MCM->MCM.MCM200.digOut;
		break;
	case MCMTYPE_MCM250:
		DO_Hdl = MCM->MCM.MCM250.digOutHnd;
		DO = &MCM->MCM.MCM250.digOut;
		break;
	case MCMTYPE_MCM210:
		DO_Hdl = 2010;
		DO = &MCM->MCM.MCM210.digOut;
		break;

	default:
		return -1;
		break;
	}
	//if Emergency is active, all Outputs are 0
	//if( _emcy )
	//	DO->val = 0;

	if( 50 > (GetMSTick() - DO->time) ) {
		return 0;
	}
	if( (DO->val != DO->val_old)//Send at changes
		|| (1000 < (GetMSTick() - DO->time)) ) //Send at least every second
	{
		DO->val_old = DO->val;
		DO->time = GetMSTick();
		tCanMessage tx_msg;
		tx_msg.channel = MCM->CAN;
		tx_msg.ext = MCM->Ext;
		tx_msg.id = MCM->RX_Id;
		tx_msg.len = 8;
		tx_msg.res = 0;
		tx_msg.data.u8[0] = 0x02;
		tx_msg.data.u8[1] = 0xFF & DO_Hdl;
		tx_msg.data.u8[2] = 0xFF & (DO_Hdl >> 8);
		tx_msg.data.u8[3] = 0;
		tx_msg.data.u32[1] = DO->val;
		CANSendMsg( &tx_msg );
	}
	return 1;
}

int32_t _GSeMCM_SetDigitalOut( tGS_MCM* MCM, uint32_t pos, int32_t val ) {
	tGS_MCM* this = MCM;
	if( MCMTYPE_MCM200 == this->Type ) {
		if( val )
			this->MCM.MCM200.digOut.val = this->MCM.MCM200.digOut.val | (0x1 << pos);
		else
			this->MCM.MCM200.digOut.val = this->MCM.MCM200.digOut.val & (~(0x1 << pos));
	}
	else if( MCMTYPE_MCM210 == this->Type ) {
		uint8_t DO_Low = this->MCM.MCM210.digOut.val & 0xFF;
		uint8_t DO_High = (this->MCM.MCM210.digOut.val >> 8) & 0xFF;
		if( val )//1 9r -1
			DO_Low = DO_Low | (0x1 << pos);
		else
			DO_Low = DO_Low & (~(0x1 << pos));
		if( 1 == val )
			DO_High = DO_High | (0x1 << pos);
		else
			DO_High = DO_High & (~(0x1 << pos));
		this->MCM.MCM210.digOut.val = DO_Low | (DO_High << 8);

	}
	else if( MCMTYPE_MCM250 == this->Type ) {
		if( val )
			this->MCM.MCM250.digOut.val = this->MCM.MCM250.digOut.val | (0x1 << pos);
		else
			this->MCM.MCM250.digOut.val = this->MCM.MCM250.digOut.val & (~(0x1 << pos));
	}
	else return -1;
	return 0;
}

int32_t GSeMCM_SendPWMOut( tGS_MCM* MCM, uint32_t pos ) {
	tGSeMCM_PWM* PWM = NULL;
	uint32_t   PWM_Hdl = 0;
	switch( MCM->Type ) {
	case MCMTYPE_MCM250:
		if( pos < GS_ARRAYELEMENTS( MCM->MCM.MCM250.pwm ) ) {
			PWM = MCM->MCM.MCM250.pwm;
			PWM_Hdl = MCM->MCM.MCM250.pwmHnd + 2 * pos;
		}
		break;
	case MCMTYPE_MCM210:
		if( pos < GS_ARRAYELEMENTS( MCM->MCM.MCM210.pwm ) ) {
			PWM = MCM->MCM.MCM210.pwm;
		}
		break;

	default:
		return -1;
		break;
	}

	if( 50 > ( GetMSTick() - PWM[pos].time ) ) //don't send faster than 50 ms;
	{
		return 0;
	}
	if( (PWM[pos].duty != PWM[pos].duty_old)//Send at changes
		|| (PWM[pos].freq != PWM[pos].freq_old)
		|| (PWM[pos].freq && (1000 < (GetMSTick() - PWM[pos].time))) ) //Send at least every second
	{
		switch( MCM->Type ) {
		case MCMTYPE_MCM250: {
			PWM[pos].time = GetMSTick();
			PWM[pos].duty_old = PWM[pos].duty;
			PWM[pos].freq_old = PWM[pos].freq;
			tCanMessage msg;
			msg.channel = MCM->CAN;
			msg.ext = MCM->Ext;
			msg.id = MCM->RX_Id;
			msg.len = 6;
			msg.res = 0;
			//Send duty
			msg.data.u8[0] = 0x02;
			msg.data.u8[1] = PWM_Hdl & 0xFF;
			msg.data.u8[2] = (PWM_Hdl >> 8) & 0xFF;
			msg.data.u8[3] = 0x00;
			msg.data.u32[1] = PWM[pos].duty;
			CANSendMsg( &msg );
			//send Freq
			PWM_Hdl++;
			msg.data.u8[1] = PWM_Hdl & 0xFF;
			msg.data.u8[2] = (PWM_Hdl >> 8) & 0xFF;
			msg.data.u32[1] = PWM[pos].freq;
			CANSendMsg( &msg );
			return 1;
		}
		/* code */
		break;

		case MCMTYPE_MCM210: {
			int32_t duty;
			PWM[pos].time = GetMSTick();
			if( ((PWM[pos].duty_old < 0) && (PWM[pos].duty > 0))
				|| ((PWM[pos].duty_old > 0) && (PWM[pos].duty < 0)) ) {
				duty = 0;
				_GSeMCM_SetDigitalOut( MCM, pos * 2 + 1, 0 );
				GSeMCM_SendDigitalOut( MCM );
				PWM[pos].duty_old = duty;
			}
			else if( PWM[pos].duty < 0 ) {
				duty = -PWM[pos].duty;
				PWM[pos].duty_old = PWM[pos].duty;
				_GSeMCM_SetDigitalOut( MCM, pos * 2 + 1, -1 );
				GSeMCM_SendDigitalOut( MCM );
			}
			else if( PWM[pos].duty > 0 ) {
				duty = PWM[pos].duty;
				PWM[pos].duty_old = PWM[pos].duty;
				_GSeMCM_SetDigitalOut( MCM, pos * 2 + 1, 1 );
				GSeMCM_SendDigitalOut( MCM );
			}
			else {
				duty = 0;
				PWM[pos].duty_old = 0;
				_GSeMCM_SetDigitalOut( MCM, pos * 2 + 1, 0 );
				GSeMCM_SendDigitalOut( MCM );
			}

			PWM[pos].freq_old = PWM[pos].freq;

			tCanMessage msg;
			msg.channel = MCM->CAN;
			msg.ext = MCM->Ext;
			msg.id = MCM->RX_Id;
			msg.len = 8;
			msg.res = 0;
			msg.data.u8[0] = 0x15;
			msg.data.u8[1] = 0x21;
			msg.data.u8[2] = 0x00;
			msg.data.u8[3] = pos;
			msg.data.u16[2] = PWM[pos].freq;
			msg.data.u16[3] = PWM[pos].duty;
			CANSendMsg( &msg );
		}
		break;

		default:
			break;
		}

	}
	return 0;
}

int32_t GSeMCM_SetPWMOut( uint32_t Hdl_MCM, uint8_t pos, uint16_t pulswidth, uint16_t frequency, uint8_t enabled ) {
	if( Hdl_MCM >= _numMCM ) {
		return -1;
	}
	tGS_MCM* this = &_pMCM[Hdl_MCM];
	switch( this->Type ) {
	case MCMTYPE_MCM250:
		if( pos < GS_ARRAYELEMENTS( this->MCM.MCM250.pwm ) ) {
			this->MCM.MCM250.pwm[pos].duty = pulswidth;
			this->MCM.MCM250.pwm[pos].freq = frequency;
			return 0;
		}
		break;//???
	case MCMTYPE_MCM210:
		if( pos < GS_ARRAYELEMENTS( this->MCM.MCM210.pwm ) ) {
			this->MCM.MCM210.pwm[pos].duty = pulswidth;
			this->MCM.MCM210.pwm[pos].freq = frequency;
			return 0;
		}
		break;
	default:
		return -1;
		break;
	}
	return -1;
}

int32_t GSeMCM_GetPWMOut( uint32_t Hdl_MCM, uint8_t pos, uint16_t* pulswidth, uint16_t* frequency ) {
	if( Hdl_MCM >= _numMCM ) {
		return -1;
	}
	tGS_MCM* this = &_pMCM[Hdl_MCM];
	switch( this->Type ) {
	case MCMTYPE_MCM250:
		if( pos < GS_ARRAYELEMENTS( this->MCM.MCM250.pwm ) ) {
			*pulswidth = this->MCM.MCM250.pwm[pos].duty;
			*frequency = this->MCM.MCM250.pwm[pos].freq;
			return 0;
		}

		break;

	default:
		return -1;
		break;
	}
	return -1;
}

extern runreconf;
void * GSeMCM_Thread(void * arg) {
  _Thread_Running = 1;
  while (_Thread_Running) {
    static uint32_t time_old = 0;
    int32_t mstick = GetMSTick();
    for (uint32_t i = 0; i < _numMCM; i++) {
      tGS_MCM * this = & _pMCM[i];
      if (this -> polling_ms) {
        if (this -> polling_ms < mstick - this -> polling_t_old) {
          this -> polling_t_old = mstick;
          tCanMessage tx_msg;
          tx_msg.channel = this -> CAN;
          tx_msg.ext = this -> Ext;
          tx_msg.id = this -> RX_Id;
          tx_msg.len = 8;
          tx_msg.res = 0;
          tx_msg.data.u8[0] = 0x1C;
          tx_msg.data.u8[1] = 0xFF & HANDLE_TEMP;
          tx_msg.data.u8[2] = 0xFF & (HANDLE_TEMP >> 8);
          tx_msg.data.u8[3] = 0;
          tx_msg.data.u32[1] = 0;
          CANSendMsg( & tx_msg);
          usleep(1000);
          tx_msg.data.u8[1] = 0xFF & HANDLE_VOLT;
          tx_msg.data.u8[2] = 0xFF & (HANDLE_VOLT >> 8);
          CANSendMsg( & tx_msg);
          usleep(1000);
          switch (this -> Type) {
          case MCMTYPE_MCM200:

            break;

          case MCMTYPE_MCM250:

            break;

          case MCMTYPE_GSK_K2_4x3:
          case MCMTYPE_K1:
          case MCMTYPE_BT20:
            SendLed(this, GSeMCM_LED_WHITE);
            SendLed(this, GSeMCM_LED_RED);
            break;

          default:
            break;
          }

        }
        int32_t timeout = 5000;
        if (this -> polling_ms) {
          timeout = 2 * this -> polling_ms;
        }
        if (timeout < (int32_t)(GetMSTick() - this -> Last_Msg_ms)) {
          this -> state = 0;
        } else this -> state = 1;
      }
      //cycle part
      switch (this -> Type) {
      case MCMTYPE_MCM200:
        GSeMCM_SendDigitalOut(this);
        break;

      case MCMTYPE_MCM250:
        if (runreconf == 0) {
          GSeMCM_SendDigitalOut(this);
          for (int32_t j = 0; j < 4; j++) {
            GSeMCM_SendPWMOut(this, j);
          }
        } else {
          McmConfup();
        }
        break;

      default:
        break;
      }

    }

    //Zyklus 50 ms;
    int32_t usleeptime = 50000 - (GetMSTick() - time_old) * 1000;
    if (usleeptime < 0)
      usleeptime = 0;
    time_old = GetMSTick();
    usleep(usleeptime);
  }
  _Thread_Running = -1;
  _numMCM = 0;
  gsThreadExit(NULL);
  return NULL;
}

int32_t  MCM200_Callback( tCanMessage* msg ) {
	for( uint32_t i = 0; i < _numMCM; i++ ) {
		tGS_MCM* this = &_pMCM[i];
		if( msg->id == this->TX_Id ) {
			this->Last_Msg_ms = GetMSTick();
			if( (0x02 == msg->data.u8[0])
				|| (0x03 == msg->data.u8[0]) ) {
				int32_t Handle = msg->data.u8[1] + (msg->data.u8[2] << 8);
				int32_t Data = msg->data.u32[1];
				if( Handle == this->MCM.MCM200.digInHnd )
					this->MCM.MCM200.digIn = Data;

				else if( Handle == this->MCM.MCM200.digOutHnd )
					this->MCM.MCM200.digOut.val = Data;

				else if( (Handle >= this->MCM.MCM200.anaInHnd)
					&& (Handle < this->MCM.MCM200.anaInHnd + 4) )
					this->MCM.MCM200.anaIn[Handle - this->MCM.MCM200.anaInHnd] = Data;

				else if( Handle == HANDLE_VOLT ) {
					this->Volt = Data;
				}
				else if( Handle == HANDLE_TEMP ) {
					this->Temp = Data;
				}
			}
		}
	}
	return 0;
}

int32_t  MCM250_Callback( tCanMessage* msg ) {
	for( uint32_t i = 0; i < _numMCM; i++ ) {
		tGS_MCM* this = &_pMCM[i];
		if( msg->id == this->TX_Id ) {
			this->Last_Msg_ms = GetMSTick();
			if( (0x02 == msg->data.u8[0])
				|| (0x03 == msg->data.u8[0]) ) {
				int32_t Handle = msg->data.u8[1] + (msg->data.u8[2] << 8);
				int32_t Data = msg->data.u32[1];
				if( Handle == this->MCM.MCM250.digInHnd )
					this->MCM.MCM250.digIn = Data;

				else if( Handle == this->MCM.MCM250.digOutHnd )
					this->MCM.MCM250.digOut.val = Data;

				else if( (Handle >= this->MCM.MCM250.anaInHnd)
						&& (Handle < this->MCM.MCM250.anaInHnd + 12) )
					this->MCM.MCM250.anaIn[Handle - this->MCM.MCM250.anaInHnd] = Data;

				else if( (Handle >= this->MCM.MCM250.cntHnd) // 3000 - 3031
					&& (Handle < this->MCM.MCM250.cntHnd + 32) ) {
					Handle = Handle - this->MCM.MCM250.cntHnd;
					if( 0 == ((Handle) % 8) )//Counter for the 3000,3008,3016
					{
						Handle = Handle / 8;
						this->MCM.MCM250.counter[Handle] = Data;
					}
					else if( 4 == ((Handle) % 8) ) //Counter for the 3004,3012,3020..
					{
						Handle = (Handle - 4) / 8;
						this->MCM.MCM250.frequency[Handle] = Data;
					}
					this->MCM.MCM250.anaIn[Handle - this->MCM.MCM250.anaInHnd] = Data;
				}
				else if( Handle == HANDLE_VOLT ) {
					this->Volt = Data;
				}
				else if( Handle == HANDLE_TEMP ) {
					this->Temp = Data;
				}
			}
		}
	}
	return 0;
}

int32_t  BT20_Callback( tCanMessage* msg ) {
	for( uint32_t i = 0; i < _numMCM; i++ ) {
		tGS_MCM* this = &_pMCM[i];
		if( msg->id == this->TX_Id ) {
			this->Last_Msg_ms = GetMSTick();
			if( (0x02 == msg->data.u8[0])
				|| (0x03 == msg->data.u8[0]) ) {
				int32_t Handle = msg->data.u8[1] + (msg->data.u8[2] << 8);
				int32_t Data = msg->data.u32[1];

				if( Handle == HANDLE_VOLT ) {
					this->Volt = Data;
				}
				else if( Handle == HANDLE_TEMP ) {
					this->Temp = Data;
				}
			}
			else if( 0x17 == msg->data.u8[0] ) {
				gsMsgFiFoWrite( this->MCM.BT20.msgFiFo, &msg->data.u8[0], sizeof( msg->data ), 5 );
			}
		}
	}
	return 0;
}

int32_t  K2_4x3_Callback( tCanMessage* msg ) {
	for( uint32_t i = 0; i < _numMCM; i++ ) {
		tGS_MCM* this = &_pMCM[i];
		if( msg->id == this->TX_Id ) {
			this->Last_Msg_ms = GetMSTick();
			if( (0x02 == msg->data.u8[0])
				|| (0x03 == msg->data.u8[0]) ) {
				int32_t Handle = msg->data.u8[1] + (msg->data.u8[2] << 8);
				int32_t Data = msg->data.u32[1];

				if( Handle == HANDLE_VOLT ) {
					this->Volt = Data;
				}
				else if( Handle == HANDLE_TEMP ) {
					this->Temp = Data;
				}
			}
			else if( 0x17 == msg->data.u8[0] ) {
				gsMsgFiFoWrite( this->MCM.K2_4x3.msgFiFo, &msg->data.u8[0], sizeof( msg->data ), 5 );
			}
		}
	}
	return 0;
}

int32_t  K1_Callback( tCanMessage* msg ) {
	for( uint32_t i = 0; i < _numMCM; i++ ) {
		tGS_MCM* this = &_pMCM[i];
		if( msg->id == this->TX_Id ) {
			this->Last_Msg_ms = GetMSTick();
			if( (0x02 == msg->data.u8[0])
				|| (0x03 == msg->data.u8[0]) ) {
				int32_t Handle = msg->data.u8[1] + (msg->data.u8[2] << 8);
				int32_t Data = msg->data.u32[1];

				if( Handle == HANDLE_VOLT ) {
					this->Volt = Data;
				}
				else if( Handle == HANDLE_TEMP ) {
					this->Temp = Data;
				}
				else if( Handle == HANDLE_ROTENCODER_0 ) {
					this->MCM.K1.Rot[0] = Data;
				}
				else if( Handle == HANDLE_ROTENCODER_1 ) {
					this->MCM.K1.Rot[1] = Data;
				}
			}
			else if( 0x17 == msg->data.u8[0] ) {
				gsMsgFiFoWrite( this->MCM.K1.msgFiFo, &msg->data.u8[0], sizeof( msg->data ), 5 );
			}
		}
	}
	return 0;
}

void GSeMCM_Cycle_BT20( tGS_MCM* pMCM ) {
	tCanMessage msg;
	tGS_BT20* BT20;
	BT20 = &pMCM->MCM.BT20;
	BT20->KeyPressedNew = 0;
	BT20->KeyReleasedNew = 0;
	if( gsMsgFiFoRead( BT20->msgFiFo, &msg.data.u8[0], sizeof( msg.data ), 0 ) ) {
		if( 0x17 == msg.data.u8[0] ) {
			if( 1 == msg.data.u8[1] ) {
				BT20->Key_Act[0] = msg.data.u32[1];
			}
			else if( 2 == msg.data.u8[1] ) {
				BT20->Key_Act[1] = msg.data.u32[1];
			}
			if( msg.data.u8[2] ) {
				if( msg.data.u8[2] & 0x80 ) {
					BT20->KeyReleasedNew = msg.data.u8[2] & 0x7F;
				}
				else {
					BT20->KeyPressedNew = msg.data.u8[2] & 0x7F;
				}
			}
		}
	}
	for( int32_t i = 0; i < 3; i++ ) {
		if( BT20->Led.Led[i] != BT20->Led.Led_Old[i] ) {
			BT20->Led.Led_Old[i] = BT20->Led.Led[i];
			SendLed( pMCM, i );
		}
	}
}

void GSeMCM_Cycle_K2_4x3( tGS_MCM* pMCM ) {
	tCanMessage msg;
	tGS_GSK_K2_4x3* K2_4x3;
	K2_4x3 = &pMCM->MCM.K2_4x3;
	K2_4x3->KeyPressedNew = 0;
	K2_4x3->KeyReleasedNew = 0;
	if( gsMsgFiFoRead( K2_4x3->msgFiFo, &msg.data.u8[0], sizeof( msg.data ), 0 ) ) {
		if( 0x17 == msg.data.u8[0] ) {
			if( 1 == msg.data.u8[1] ) {
				K2_4x3->Key_Act = msg.data.u32[1];
			}

			if( msg.data.u8[2] ) {
				if( msg.data.u8[2] & 0x80 ) {
					K2_4x3->KeyReleasedNew = msg.data.u8[2] & 0x7F;
				}
				else {
					K2_4x3->KeyPressedNew = msg.data.u8[2] & 0x7F;
				}
			}
		}
	}

	for( int32_t i = 0; i < 3; i++ ) {
		if( K2_4x3->Led.Led[i] != K2_4x3->Led.Led_Old[i] ) {
			K2_4x3->Led.Led_Old[i] = K2_4x3->Led.Led[i];
			SendLed( pMCM, i );
		}
	}
}

void GSeMCM_Cycle_K1( tGS_MCM* pMCM ) {
	tCanMessage msg;
	tGS_K1* K1;
	K1 = &pMCM->MCM.K1;
	K1->KeyPressedNew = 0;
	K1->KeyReleasedNew = 0;
	if( gsMsgFiFoRead( K1->msgFiFo, &msg.data.u8[0], sizeof( msg.data ), 0 ) ) {
		if( 0x17 == msg.data.u8[0] ) {
			if( 1 == msg.data.u8[1] ) {
				K1->Key_Act = msg.data.u32[1];
			}

			if( msg.data.u8[2] ) {
				if( msg.data.u8[2] & 0x80 ) {
					K1->KeyReleasedNew = msg.data.u8[2] & 0x7F;
				}
				else {
					K1->KeyPressedNew = msg.data.u8[2] & 0x7F;
				}
			}
		}
	}

	for( int32_t i = 0; i < 3; i++ ) {
		if( K1->Led.Led[i] != K1->Led.Led_Old[i] ) {
			K1->Led.Led_Old[i] = K1->Led.Led[i];
			SendLed( pMCM, i );
		}
	}
}

void GSeMCM_Cycle( void ) {
	for( uint32_t i = 0; i < _numMCM; i++ ) {
		switch( _pMCM[i].Type ) {
		case MCMTYPE_BT20:        GSeMCM_Cycle_BT20( &_pMCM[i] );   break;
		case MCMTYPE_GSK_K2_4x3:  GSeMCM_Cycle_K2_4x3( &_pMCM[i] ); break;
		case MCMTYPE_K1:          GSeMCM_Cycle_K1( &_pMCM[i] );     break;
		default:       break;
		}
	}
}

int32_t GSeMCM_InitDevice( eGSeMCM_TYPE Type, int32_t CAN, int32_t ext, int32_t GSeMCM_TX, int32_t GSeMCM_RX ) {
	int32_t idx = _numMCM;

	//tGS_MCM * pNewMCM = realloc(_pMCM,sizeof(tGS_MCM) * (idx + 1));
	if( NULL == _pMCM ) {
		tGS_MCM* pNewMCM = malloc( sizeof( tGS_MCM ) * (10) );
		if( NULL == pNewMCM ) {
			return -1;
		}
		_pMCM = pNewMCM;
	}

	memset( &_pMCM[idx], 0, sizeof( tGS_MCM ) );
	tGS_MCM* this = &_pMCM[idx];
	this->RX_Id = GSeMCM_RX;
	this->TX_Id = GSeMCM_TX;
	this->Ext = ext;
	this->CAN = CAN;
	this->Last_Msg_ms = GetMSTick() + 10000;
	this->Type = Type;
	_numMCM++;					//Index erst erhöhen, wenn MCM-Daten initialisiert sind.

	if( NULL == _Thread ) {
		//     gsThreadMutexInit(&_Mutex,NULL);
		gsThreadCreate( &_Thread, NULL, GSeMCM_Thread, NULL );
		_FiFo = CANCreateFiFo( 256 );
	}
	switch( this->Type ) {
	case MCMTYPE_MCM200:
		this->MCM.MCM200.anaInHnd = 2016;
		this->MCM.MCM200.digInHnd = 2000;
		this->MCM.MCM200.digOutHnd = 2008;
		CANAddRxFilterCallBack( this->CAN, this->TX_Id, 0x7FF, this->Ext, MCM200_Callback );
		break;

	case MCMTYPE_MCM250:
		this->MCM.MCM250.anaInHnd = 2016;
		this->MCM.MCM250.digInHnd = 2000;
		this->MCM.MCM250.digOutHnd = 2008;
		this->MCM.MCM250.pwmHnd = 2500;
		this->MCM.MCM250.cntHnd = 3000;
		this->polling_ms = 2000;

		CANAddRxFilterCallBack( this->CAN, this->TX_Id, 0x7FF, this->Ext, MCM250_Callback );
		break;

	case MCMTYPE_BT20:
		CANAddRxFilterCallBack( this->CAN, this->TX_Id, 0x7FF, this->Ext, BT20_Callback );
		this->polling_ms = 2000;
		this->MCM.BT20.msgFiFo = gsMsgFiFoCreate( 256 );
		break;

	case MCMTYPE_GSK_K2_4x3:
		CANAddRxFilterCallBack( this->CAN, this->TX_Id, 0x7FF, this->Ext, K2_4x3_Callback );
		this->polling_ms = 2000;
		this->MCM.K2_4x3.msgFiFo = gsMsgFiFoCreate( 256 );
		break;

	case MCMTYPE_K1:
		CANAddRxFilterCallBack( this->CAN, this->TX_Id, 0x7FF, this->Ext, K1_Callback );
		this->polling_ms = 2000;
		this->MCM.K1.msgFiFo = gsMsgFiFoCreate( 256 );
		break;

	default:
		return -1;
		break;
	}
	return idx;
}

void GSeMCM_DeInit( void ) {
	if( NULL != _Thread ) {
		_Thread_Running = 0;
		while( -1 != _Thread_Running ) {
			usleep( 1000 );
		}
		gsThreadJoin( _Thread, NULL );
		//gsThreadMutexDestroy(_Mutex);
		_Thread = NULL;
		_numMCM = 0;
		free( _pMCM );
		_pMCM = NULL;
	}
}
int32_t GSeMCM_SetPolling( uint32_t Hdl_MCM, uint32_t Polling_ms ) {
	if( Hdl_MCM < _numMCM ) {
		_pMCM[Hdl_MCM].polling_ms = Polling_ms;
		return 0;
	}
	else return -1;
}

int32_t GSeMCM_GetActiveStatus( uint32_t Hdl_MCM ) {
	if( Hdl_MCM < _numMCM ) {
		return _pMCM[Hdl_MCM].state;
	}
	else return -1;
}

int32_t GSeMCM_GetDigitalIn( uint32_t Hdl_MCM, uint32_t pos ) {
	if( Hdl_MCM < _numMCM ) {
		if( MCMTYPE_MCM200 == _pMCM[Hdl_MCM].Type ) {
			return 0x01 & (_pMCM[Hdl_MCM].MCM.MCM200.digIn >> pos);
		}
		else if( MCMTYPE_MCM250 == _pMCM[Hdl_MCM].Type ) {
			return 0x01 & (_pMCM[Hdl_MCM].MCM.MCM250.digIn >> pos);
		}
		else if( MCMTYPE_GSK_K2_4x3 == _pMCM[Hdl_MCM].Type ) {
			return 0x01 & (_pMCM[Hdl_MCM].MCM.K2_4x3.Key_Act >> pos);
		}
		else return -1;
	}
	return -1;
}

int32_t GSeMCM_GetAnalogIn( uint32_t Hdl_MCM, uint32_t pos ) {
	if( Hdl_MCM < _numMCM ) {
		if( MCMTYPE_MCM200 == _pMCM[Hdl_MCM].Type ) {
			if( pos < 4 ) {
				return _pMCM[Hdl_MCM].MCM.MCM200.anaIn[pos];
			}
			else return -1;
		}
		else if( MCMTYPE_MCM250 == _pMCM[Hdl_MCM].Type ) {
			if( pos < GS_ARRAYELEMENTS( _pMCM[Hdl_MCM].MCM.MCM250.anaIn ) ) {
				return _pMCM[Hdl_MCM].MCM.MCM250.anaIn[pos];
			}
			else return -1;
		}
		else return -1;
	}
	return -1;
}

int32_t  GSeMCM_GetTemperature( uint32_t Hdl_MCM ) {
	if( Hdl_MCM < _numMCM ) {
		return _pMCM[Hdl_MCM].Temp;
	}
	else return -1;
}

int32_t GSeMCM_GetVoltage( uint32_t Hdl_MCM ) {
	if( Hdl_MCM < _numMCM ) {
		return _pMCM[Hdl_MCM].Volt;
	}
	else return -1;
}

int32_t GSeMCM_SetDigitalOut( uint32_t Hdl_MCM, uint32_t pos, int32_t val ) {
	if( Hdl_MCM < _numMCM ) {
		tGS_MCM* this = &_pMCM[Hdl_MCM];
		_GSeMCM_SetDigitalOut( this, pos, val );
	}
	return 0;
}

int32_t SetDigitalOut(uint32_t Hdl_MCM, uint32_t pos, int32_t val) {
	tGS_MCM * this = & _pMCM[Hdl_MCM];
  if (GetVar(HDL_IO_MODECTRL)) {
    // Пустое условие необходимо для того, чтобы в ручном режиме
    // не отправлялись сигналы на МСМ.
  } else if (GetVarIndexed(IDX_OILLEVEL) > 0 && !GetVarIndexed(IDX_EMCY_ON)) {
    if (Hdl_MCM < _numMCM) {
      _GSeMCM_SetDigitalOut(this, pos, val);
    }
  } else {
    _GSeMCM_SetDigitalOut(this, pos, 0);
  }
  return 0;
}

int32_t  GSeMCM_GetDigitalOut( uint32_t Hdl_MCM, uint32_t pos ) {
	if( Hdl_MCM < _numMCM ) {
		tGS_MCM* this = &_pMCM[Hdl_MCM];
		if( MCMTYPE_MCM200 == this->Type ) {
			return (this->MCM.MCM200.digOut.val >> pos) & 0x1;
		}
		else if( MCMTYPE_MCM250 == this->Type ) {
			return (this->MCM.MCM250.digOut.val >> pos) & 0x1;
		}
		else return -1;
	}
	return -1;
}

int32_t  GSeMCM_GetFrequencyInput( uint32_t Hdl_MCM, uint32_t pos ) {
	if( Hdl_MCM < _numMCM ) {
		tGS_MCM* this = &_pMCM[Hdl_MCM];
		if( MCMTYPE_MCM250 == this->Type ) {
			if( pos < 4 )
				return this->MCM.MCM250.frequency[pos];
		}
		else return -1;
	}
	return -1;
}

int32_t GSeMCM_Get_CounterValue( uint32_t Hdl_MCM, uint32_t pos, int32_t* val1, int32_t* val2 ) {
	if( Hdl_MCM < _numMCM ) {
		tGS_MCM* this = &_pMCM[Hdl_MCM];
		if( MCMTYPE_MCM250 == this->Type ) {
			if( pos < 4 ) {
				if( val1 != NULL )
					*val1 = this->MCM.MCM250.counter[pos];
				if( val2 != NULL )
					*val2 = this->MCM.MCM250.overflow[pos];
				return 1;
			}
		}
	}
	return -1;
}

int32_t GSeMCM_IsKeyPressedNew( uint32_t GSeMCM_Hdl, int32_t Key ) {
	if( GSeMCM_Hdl < _numMCM ) {
		switch( _pMCM[GSeMCM_Hdl].Type ) {
		case MCMTYPE_BT20:
			if( Key == _pMCM[GSeMCM_Hdl].MCM.BT20.KeyPressedNew )
				return 1;
			else return 0;
			break;

		case MCMTYPE_GSK_K2_4x3:
			if( Key == _pMCM[GSeMCM_Hdl].MCM.K2_4x3.KeyPressedNew )
				return 1;
			else return 0;
			break;

		case MCMTYPE_K1:
			if( Key == _pMCM[GSeMCM_Hdl].MCM.K1.KeyPressedNew )
				return 1;
			else return 0;
			break;

		default:
			return -1;
			break;
		}
	}
	return -1;
}

int32_t GSeMCM_IsKeyReleasedNew( uint32_t GSeMCM_Hdl, int32_t Key ) {
	if( GSeMCM_Hdl < _numMCM ) {
		switch( _pMCM[GSeMCM_Hdl].Type ) {
		case MCMTYPE_BT20:
			if( Key == _pMCM[GSeMCM_Hdl].MCM.BT20.KeyReleasedNew )
				return 1;
			else return 0;
			break;

		case MCMTYPE_GSK_K2_4x3:
			if( Key == _pMCM[GSeMCM_Hdl].MCM.K2_4x3.KeyReleasedNew )
				return 1;
			else return 0;
			break;

		case MCMTYPE_K1:
			if( Key == _pMCM[GSeMCM_Hdl].MCM.K1.KeyReleasedNew )
				return 1;
			else return 0;
			break;

		default:
			return -1;
			break;
		}
	}
	return -1;
}

int32_t GSeMCM_IsKeyDown( uint32_t GSeMCM_Hdl, int32_t Key ) {
	if( GSeMCM_Hdl < _numMCM ) {
		switch( _pMCM[GSeMCM_Hdl].Type ) {
		case MCMTYPE_BT20:
			if( Key <= 32 )
				return (_pMCM[GSeMCM_Hdl].MCM.BT20.Key_Act[0] >> (Key - 1)) & 0x01;

			else if( Key <= 64 )
				return (_pMCM[GSeMCM_Hdl].MCM.BT20.Key_Act[1] >> (Key - 33)) & 0x01;
			break;
		case MCMTYPE_GSK_K2_4x3:
			if( Key <= 32 )
				return (_pMCM[GSeMCM_Hdl].MCM.K2_4x3.Key_Act >> (Key - 1)) & 0x01;
			break;

		case MCMTYPE_K1:
			if( Key <= 32 )
				return (_pMCM[GSeMCM_Hdl].MCM.K1.Key_Act >> (Key - 1)) & 0x01;
			break;
		default:
			return -1;
			break;
		}
	}
	return -1;
}

int32_t GSeMCM_IsAnyKeyDown( uint32_t GSeMCM_Hdl ) {
	if( GSeMCM_Hdl < _numMCM ) {
		switch( _pMCM[GSeMCM_Hdl].Type ) {
		case MCMTYPE_BT20:
			for( int32_t i = 0; i < 64; i++ ) {
				if( i < 32 ) {
					if( (_pMCM[GSeMCM_Hdl].MCM.BT20.Key_Act[0] >> (i)) & 0x01 )
						return i + 1;
				}
				else if( i < 64 ) {
					if( (_pMCM[GSeMCM_Hdl].MCM.BT20.Key_Act[1] >> (i - 32)) & 0x01 )
						return i + 1;
				}
			}
			break;

		case MCMTYPE_GSK_K2_4x3:
			for( int32_t i = 0; i < 12; i++ ) {
				if( (_pMCM[GSeMCM_Hdl].MCM.K2_4x3.Key_Act >> (i)) & 0x01 )
					return i + 1;
			}
			break;

		case MCMTYPE_K1:
			for( int32_t i = 0; i < 12; i++ ) {
				if( (_pMCM[GSeMCM_Hdl].MCM.K1.Key_Act >> (i)) & 0x01 )
					return i + 1;
			}
			break;

		default:
			return -1;
			break;
		}
	}
	return -1;
}

void SendLed( tGS_MCM* MCM, eGSeMCM_Led_Color Color ) {
	tGSeMCM_Led* Led = NULL;
	//uint32_t num_leds;
	switch( MCM->Type ) {
	case MCMTYPE_BT20:
		Led = &MCM->MCM.BT20.Led;
		//num_leds = 64;
		break;

	case MCMTYPE_GSK_K2_4x3:
		Led = &MCM->MCM.K2_4x3.Led;
		//num_leds = 12;
		break;

	case MCMTYPE_K1:
		Led = &MCM->MCM.K1.Led;
		//num_leds = 16;
		break;
	default:
		return;
		break;
	}
	tCanMessage msg;
	msg.id = MCM->RX_Id;
	msg.ext = MCM->Ext;
	msg.len = 8;
	msg.res = 0;
	msg.channel = MCM->CAN;
	msg.data.u8[0] = 0x16;
	msg.data.u8[1] = 0x03;
	msg.data.u16[1] = 0;
	msg.data.u32[1] = 0;
	switch( MCM->Type ) {
	case MCMTYPE_BT20:
	case MCMTYPE_K1:
		switch( Color ) {
		case GSeMCM_LED_RED:
		case GSeMCM_LED_GREEN:
			msg.data.u8[2] = 0;
			for( int32_t i = 0; i < 20; i++ ) {
				int8_t val_r = (Led->Led[GSeMCM_LED_RED] >> i) & 0x01;
				int8_t val_g = (Led->Led[GSeMCM_LED_GREEN] >> i) & 0x01;
				uint8_t* data = &msg.data.u8[3 + i / 4];
				if( val_r )
					*data = *data | (val_r << ((2 * i) % 8));
				else
					*data = *data & ~(val_r << ((2 * i) % 8));
				if( val_g )
					*data = *data | (val_g << ((2 * i + 1) % 8));
				else
					*data = *data & ~(val_g << ((2 * i + 1) % 8));
			}
			CANSendMsg( &msg );
			msg.data.u8[2] = 5;
			msg.data.u8[3] = 0;
			msg.data.u32[1] = 0;
			for( int32_t i = 20; i < 40; i++ ) {
				int8_t val_r = (Led->Led[GSeMCM_LED_RED] >> i) & 0x01;
				int8_t val_g = (Led->Led[GSeMCM_LED_GREEN] >> i) & 0x01;
				uint8_t* data = &msg.data.u8[3 + i / 4 - 5];
				if( val_r )
					*data = *data | (val_r << ((2 * i) % 8));
				else
					*data = *data & ~(val_r << ((2 * i) % 8));
				if( val_g )
					*data = *data | (val_g << ((2 * i + 1) % 8));
				else
					*data = *data & ~(val_g << ((2 * i + 1) % 8));
			}
			CANSendMsg( &msg );
			break;

		case GSeMCM_LED_WHITE:
			msg.data.u8[2] = 12;
			for( int32_t i = 0; i < 40; i++ ) {
				int8_t val = (Led->Led[GSeMCM_LED_WHITE] >> i) & 0x01;
				uint8_t* data = &msg.data.u8[3 + (i + 4) / 8];
				if( val )
					*data = *data | (val << ((i + 4) % 8));
				else
					*data = *data & (~(val << ((i + 4) % 8)));
			}
			CANSendMsg( &msg );
			msg.data.u8[2] = 17;
			for( int32_t i = 40; i < 64; i++ ) {
				int8_t val = (Led->Led[GSeMCM_LED_WHITE] >> i) & 0x01;
				uint8_t* data = &msg.data.u8[3 + i / 8 - 5];
				if( val )
					*data = *data | (val << ((i + 4) % 8));
				else
					*data = *data & (~(val << ((i + 4) % 8)));
			}
			CANSendMsg( &msg );
			break;
		default: break;

			break;
		}

		break;

	case MCMTYPE_GSK_K2_4x3: {
		msg.data.u8[2] = 0;
		for( int32_t i = 0; i < 40; i++ ) {
			int8_t val = (Led->Led[GSeMCM_LED_WHITE] >> i) & 0x01;
			uint8_t* data = &msg.data.u8[3 + (i) / 8];
			if( val )
				*data = *data | (val << ((i) % 8));
			else
				*data = *data & (~(val << ((i) % 8)));
		}
		CANSendMsg( &msg );
		msg.data.u8[2] = 5;
		for( int32_t i = 40; i < 64; i++ ) {
			int8_t val = (Led->Led[GSeMCM_LED_WHITE] >> i) & 0x01;
			uint8_t* data = &msg.data.u8[3 + i / 8 - 5];
			if( val )
				*data = *data | (val << ((i) % 8));
			else
				*data = *data & (~(val << ((i) % 8)));
		}
		CANSendMsg( &msg );
	}

	break;
	default:

		break;

	}

}

void SetLed64( uint32_t GSeMCM_Hdl, tGSeMCM_Led* Led, uint8_t Key, eGSeMCM_Led_Color Color, uint8_t val ) {
	uint64_t* data = &Led->Led[Color];
	if( 0 == Key ) {
		if( val )
			*data = 0xFFFFFFFFFFFFFFFF;
		else
			*data = 0;
	}
	else {
		if( val ) {
			*data = *data | ((uint64_t)1 << (Key - 1));
		}
		else {
			*data = *data & (~((uint64_t)1 << (Key - 1)));
		}
	}
}

int32_t GSeMCM_SetKeyBacklight_White( uint32_t GSeMCM_Hdl, uint32_t Key, uint8_t val ) {
	if( GSeMCM_Hdl < _numMCM ) {
		switch( _pMCM[GSeMCM_Hdl].Type ) {
		case MCMTYPE_BT20:           SetLed64( GSeMCM_Hdl, &_pMCM[GSeMCM_Hdl].MCM.BT20.Led, Key, GSeMCM_LED_WHITE, val );   break;
		case MCMTYPE_K1:             SetLed64( GSeMCM_Hdl, &_pMCM[GSeMCM_Hdl].MCM.K1.Led, Key, GSeMCM_LED_WHITE, val );   break;
		case MCMTYPE_GSK_K2_4x3:     SetLed64( GSeMCM_Hdl, &_pMCM[GSeMCM_Hdl].MCM.K2_4x3.Led, Key, GSeMCM_LED_WHITE, val );   break;

		default:  break;
		}
	}
	return 0;
}

int32_t GSeMCM_SetKeyBacklight_Green( uint32_t GSeMCM_Hdl, uint32_t Key, uint8_t val ) {
	if( GSeMCM_Hdl < _numMCM ) {
		switch( _pMCM[GSeMCM_Hdl].Type ) {
		case MCMTYPE_BT20:           SetLed64( GSeMCM_Hdl, &_pMCM[GSeMCM_Hdl].MCM.BT20.Led, Key, GSeMCM_LED_GREEN, val );   break;
		case MCMTYPE_K1:             SetLed64( GSeMCM_Hdl, &_pMCM[GSeMCM_Hdl].MCM.K1.Led, Key, GSeMCM_LED_GREEN, val );   break;
		case MCMTYPE_GSK_K2_4x3:     SetLed64( GSeMCM_Hdl, &_pMCM[GSeMCM_Hdl].MCM.K2_4x3.Led, Key, GSeMCM_LED_GREEN, val );   break;
		default:  break;
		}
	}
	return 0;
}

int32_t GSeMCM_SetKeyBacklight_Red( uint32_t GSeMCM_Hdl, uint32_t Key, uint8_t val ) {
	if( GSeMCM_Hdl < _numMCM ) {
		switch( _pMCM[GSeMCM_Hdl].Type ) {
		case MCMTYPE_BT20:           SetLed64( GSeMCM_Hdl, &_pMCM[GSeMCM_Hdl].MCM.BT20.Led, Key, GSeMCM_LED_RED, val );   break;
		case MCMTYPE_K1:             SetLed64( GSeMCM_Hdl, &_pMCM[GSeMCM_Hdl].MCM.K1.Led, Key, GSeMCM_LED_RED, val );   break;
		case MCMTYPE_GSK_K2_4x3:     SetLed64( GSeMCM_Hdl, &_pMCM[GSeMCM_Hdl].MCM.K2_4x3.Led, Key, GSeMCM_LED_RED, val );   break;
		default:  break;
		}
	}
	return 0;
}

int32_t GSeMCM_SetKeyBacklight_Brightness( uint32_t GSeMCM_Hdl, eGSeMCM_Led_Color Color, uint32_t Birghtness ) {
	if( GSeMCM_Hdl < _numMCM ) {
		tGS_MCM* MCM = &_pMCM[GSeMCM_Hdl];
		switch( MCM->Type ) {
		case MCMTYPE_GSK_K2_4x3:
		case MCMTYPE_K1: {
			tCanMessage msg;
			msg.id = MCM->RX_Id;
			msg.ext = MCM->Ext;
			msg.len = 4;
			msg.res = 0;
			msg.channel = MCM->CAN;
			msg.data.u8[0] = 0x16;
			msg.data.u8[1] = 0x06;
			msg.data.u16[1] = 0;
			msg.data.u32[1] = 0;
			msg.data.u8[2] = Color;
			msg.data.u8[3] = Birghtness;
			return CANSendMsg( &msg );
		}

		break;

		default: break;
		}
	}
	return -1;
}

void GSeMCM_Emcy_Set( uint32_t val ) {
	if( val ) {
		_emcy = 1;
	}
	else {
		_emcy = 0;
	}
}

uint32_t GSeMCM_Emcy_Get( void ) {
	return _emcy;
}

int32_t GSeMCM_SetBuzzer( uint32_t GSeMCM_Hdl, uint32_t t_on_ms, uint32_t t_off_ms, uint32_t count ) {
	tGS_MCM* GSeMCM = &_pMCM[GSeMCM_Hdl];
	tCanMessage msg;
	msg.id = GSeMCM->RX_Id;
	msg.ext = GSeMCM->Ext;
	msg.len = 6;
	msg.res = 0;
	msg.channel = GSeMCM->CAN;

	msg.data.u16[0] = 0x0F15;
	msg.data.u8[2] = 0x01;
	msg.data.u8[3] = t_on_ms / 10;
	msg.data.u8[4] = t_off_ms / 10;
	msg.data.u8[5] = 1;
	return CANSendMsg( &msg );
}

int32_t GSeMCM_SetBuzzerFreq( uint32_t GSeMCM_Hdl, uint32_t Freq ) {
	tGS_MCM* GSeMCM = &_pMCM[GSeMCM_Hdl];
	tCanMessage msg;
	msg.id = GSeMCM->RX_Id;
	msg.ext = GSeMCM->Ext;
	msg.len = 8;
	msg.res = 0;
	msg.channel = GSeMCM->CAN;

	msg.data.u16[0] = 0x1F15;
	msg.data.u16[1] = Freq;
	msg.data.u16[2] = 0;
	msg.data.u8[6] = 100;
	msg.data.u8[7] = 0;
	return CANSendMsg( &msg );
}

int32_t GSeMCM_GetRotaryEncoderPos( uint32_t GSeMCM_Hdl, uint32_t Knob ) {
	if( GSeMCM_Hdl < _numMCM ) {
		switch( _pMCM[GSeMCM_Hdl].Type ) {
		case MCMTYPE_K1: {
			tGS_K1* K1 = &_pMCM[GSeMCM_Hdl].MCM.K1;
			if( Knob < 2 ) {
				return K1->Rot[Knob];
			}
		}
		break;

		default:

			break;
		}
	}
	return -1;
}

int32_t GSeMCM_SetRotaryEncoder( uint32_t GSeMCM_Hdl, int32_t idx, int32_t position ) {
	if( GSeMCM_Hdl < _numMCM ) {
		tGS_MCM* MCM = &_pMCM[GSeMCM_Hdl];
		tCanMessage tx_msg;
		tx_msg.channel = MCM->CAN;
		tx_msg.ext = MCM->Ext;
		tx_msg.id = MCM->RX_Id;
		tx_msg.len = 8;
		tx_msg.res = 0;

		switch( MCM->Type ) {
		case MCMTYPE_K1:
			tx_msg.data.u8[0] = 0x02;
			tx_msg.data.u8[1] = 0xFF & (HANDLE_ROTENCODER_0 + position);
			tx_msg.data.u8[2] = 0xFF & ((HANDLE_ROTENCODER_0 + position) >> 8);
			tx_msg.data.u8[3] = 0;
			tx_msg.data.u32[1] = position;
			CANSendMsg( &tx_msg );
			MCM->MCM.K1.Rot[idx] = position;
			break;

		default:
			return -1;
			break;
		}
		return 1;
	}
	else return -1;
}
