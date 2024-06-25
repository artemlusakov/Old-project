/****************************************************************************
*
* File:         ERRORLIST.c
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
#include "objtab.h"
#include "vartab.h"
#include "gseerrorlist.h"
#include "RCText.h"
#include "gsToVisu.h"
#include "gsLkfCodes.h"
#include "gselist.h"
#include "gsescrolllist.h"
#include "gs_easy_config.h"
#include "param.h"
/****************************************************************************/

/* macro definitions ********************************************************/
/**
* @brief Structure to define the objects of an error list.
*  if you don't use one of the objects, use the Object ID 0;
*/
typedef struct tagErrDraw_Line {
	uint32_t TextObj_Num; //!< Textobject. The error ID will be written as an hex value.
	uint32_t TextObj_Text; //!< Textobject the actual error text will be written into.
	uint32_t Checkbox_Hdl; //!< Handle of the checkbox showing
} tErrDraw_Line;

/* type definitions *********************************************************/
typedef struct tagEList {
	tGSList List;
	int32_t time_hide;
	int8_t change;
} tEList;
/* prototypes ***************************************************************/

/* global constants *********************************************************/

/* global variables *********************************************************/
tEList EList = {{.first = NULL, .last = NULL, .act = NULL, .numItems = 0}, .time_hide = 3000};
/* local function prototypes*************************************************/
typedef struct tagErrDraw_Table {
	uint32_t numLines;
	tErrDraw_Line* Line;
	uint32_t ShownLines;
	struct {
		uint32_t ActCheckdErr;
		uint32_t ActUnCheckdErr;
		uint32_t InActCheckdErr;
		uint32_t InActUnCheckdErr;
	}	Color;
	tGSeScrollList* sl;
} tErrDraw_Table;
extern tParamData		Parameter;
tErrDraw_Table Table;

tGsErrOutCallback fpErrOut = NULL;
tGsPrintErrOutCallback fpPrintErrOut = NULL;
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
void EList_SetErrorOut( tGsErrOutCallback ErrOutCb ) {
	fpErrOut = ErrOutCb;
}
void EList_SetPrintErrorOut( tGsPrintErrOutCallback ErrOutCb ) {
	fpPrintErrOut = ErrOutCb;
}

void ErrOut( tGsError* Error ) {
	if( NULL != fpErrOut ) {
		fpErrOut( Error );
	}

	if( NULL != fpPrintErrOut ) {
		char string[196];
		char ErrorType[32];
		char ErrorState[32];
		switch( Error->State ) {
		case ES_UNCHECKED_ERROR_ACTIVE:
			sprintf( ErrorState, "Set" );
			break;

		case ES_UNCHECKED_ERROR_INACTIVE:
			sprintf( ErrorState, "Reset" );
			break;

		case ES_CHECKED_ERROR_ACTIVE:
			sprintf( ErrorState, "Set checked" );
			break;

		case ES_CHECKED_ERROR_INACTIVE:
			sprintf( ErrorState, "Reset checked" );
			break;
		default: string[0] = '\0';
			break;
		}

		switch( Error->Level ) {
		case VBL_ERROR_CRITICAL:
			sprintf( ErrorType, "Critical Error" );
			break;

		case VBL_ERROR:
			sprintf( ErrorType, "Error" );
			break;

		case VBL_WARNING:
			sprintf( ErrorType, "Warning" );
			break;

		case VBL_INFO:
			sprintf( ErrorType, "Info" );
			break;
		default: break;
		}
		snprintf( string, sizeof( string ), "%s %s 0x%X: %s\r\n",
			ErrorState,
			ErrorType,
			Error->id,
			Error->text
		);
		fpPrintErrOut( string );
	}
}

tGsError* EList_SearchId( int32_t id ) {
	tGsError* actErr = GSList_GetFirstData( &EList.List );
	while( NULL != actErr ) {
		if( actErr->id == id )
			break;
		actErr = GSList_GetNextData( &EList.List );
	}
	return actErr;
}

int32_t EList_SetErrByText( int32_t id, const char* text ) {
	return EList_SetErrLevByText( VBL_ERROR, id, text );
}

uint32_t EList_GetNumUncheckedErrors( void ) {
	int32_t count = 0;
	tGsError* actErr = GSList_GetFirstData( &EList.List );
	while( NULL != actErr ) {
		if( (actErr->State == ES_UNCHECKED_ERROR_ACTIVE)
			|| (actErr->State == ES_UNCHECKED_ERROR_INACTIVE) ) {
			count++;
		}
		actErr = GSList_GetNextData( &EList.List );
	}
	return count;
}

/********************
* Логирование ошибок
********************/

int8_t FlagWriteVinEquip;
int8_t RecordingConditionErrors;

char newpowerday[15];
char newpowermonth[15];
char newpoweryear[15];

int32_t day;
int32_t month;
int32_t year;

void ErrorsWriteFile( tGsError* Error, tGsFile** fp ) {
	char ErrorState[100];
	const char* LineState = NULL;
	const char* FileDate = RCTextGetText( RCTEXT_T_FILECREATEDATE, GetVarIndexed( IDX_SYS_LANGUAGE ) );
	switch( Error->State ) {
	case ES_UNCHECKED_ERROR_ACTIVE: LineState = RCTextGetText( RCTEXT_T_ACTIVE, GetVarIndexed( IDX_SYS_LANGUAGE ) ); break;
	case ES_UNCHECKED_ERROR_INACTIVE: LineState = RCTextGetText( RCTEXT_T_NOACTIVE, GetVarIndexed( IDX_SYS_LANGUAGE ) ); break;
	case ES_CHECKED_ERROR_ACTIVE: LineState = RCTextGetText( RCTEXT_T_MARKEDACTIVE, GetVarIndexed( IDX_SYS_LANGUAGE ) ); break;
	case ES_CHECKED_ERROR_INACTIVE: LineState = RCTextGetText( RCTEXT_T_MARKEDNOACTIVE, GetVarIndexed( IDX_SYS_LANGUAGE ) ); break;
	default: sprintf( LineState, "Error" ); break;
	}
	sprintf( ErrorState, LineState );
	if( FileSize( PATH_ERROR_FLASH ) > 0 && GetVar( HDL_KONTROLWRITE ) == 0 ) {// добавлено для 2.0
		SetVar( HDL_KONTROLWRITE, 1 );
	}

	if( GetVar( HDL_KONTROLWRITE ) == 0 ) {
		FilePrintf( *fp, "                                      \n" );
		FilePrintf( *fp, "                                      \n" );
		FilePrintf( *fp, "                                      \n" );
		//FilePrintf( *fp, "");
		FilePrintf( *fp, "\n" );
		RTCGetTime( &Error->Time );
		snprintf( newpowerday, GS_ARRAYELEMENTS( newpowerday ), "%02d", Error->Date.Day );
		snprintf( newpowermonth, GS_ARRAYELEMENTS( newpowermonth ), "%02d", Error->Date.Month );
		snprintf( newpoweryear, GS_ARRAYELEMENTS( newpoweryear ), "%02d", Error->Date.Year );
		day = atoi( newpowerday );
		month = atoi( newpowermonth );
		year = atoi( newpoweryear );
		SetVar( HDL_KOLTIMEDAY, day );
		SetVar( HDL_KOLTIMEMONTH, month );
		SetVar( HDL_KOLTIMEYEAR, year );
		FilePrintf( *fp, " %s = %02d.%02d.%02d |\n", FileDate, day, month, year );
		SetVar( HDL_KONTROLWRITE, 1 );
	}

	char ErrorTime[12];
	char string[12];
	RTCGetDate( &Error->Date );
	RTCGetTime( &Error->Time );
	snprintf( string, GS_ARRAYELEMENTS( string ), "%02d.%02d.%02d", Error->Date.Day, Error->Date.Month, Error->Date.Year );
	snprintf( ErrorTime, GS_ARRAYELEMENTS( ErrorTime ), "%02d:%02d:%02d", Error->Time.Hours, Error->Time.Minutes, Error->Time.Seconds );
	FilePrintf( *fp, " %s |", string );
	FilePrintf( *fp, " %s |", ErrorTime );
	FilePrintf( *fp, " %s |", Error->text );
	FilePrintf( *fp, " %s |\n", ErrorState );
}

int32_t ErrorCheckState( void ) {
	tGsError* actErr = GSList_GetFirstData( &EList.List );
	while( NULL != actErr ) {
		if( actErr->FlagCheckError != 1 ) {
			if( ES_CHECKED_ERROR_ACTIVE == actErr->State ) {
				if( ES_UNCHECKED_ERROR_INACTIVE != actErr->State ) {
					if( ES_CHECKED_ERROR_INACTIVE != actErr->State ) {
          tGsFile* fp = FileOpen( PATH_ERROR_FLASH, "a+" );
						ErrorsWriteFile( actErr, &fp );
						actErr->FlagCheckError = 1;
            FileClose( fp );
            SetVarIndexed(IDX_ERRORSSIZE, FileSize(PATH_ERROR_FLASH));
					}
				}

			}
		}
		actErr = GSList_GetNextData( &EList.List );
	}

	return 0;
}

int32_t WriteLastError( void ) {
	tGsError* actErr = GSList_GetFirstData( &EList.List );
	while( NULL != actErr ) {
		if( ES_UNCHECKED_ERROR_INACTIVE != actErr->State ) {
			if( ES_CHECKED_ERROR_INACTIVE != actErr->State ) {
				if( actErr->ControlWriteNewError != 1 ) {
        	tGsFile* fp = FileOpen( PATH_ERROR_FLASH, "a+" );
					ErrorsWriteFile( actErr, &fp );
					actErr->ControlWriteNewError = 1;//Дбавлено для 1.9
					actErr->FlagCheckResetNoCheckError = 0;//Дбавлено для 1.9
					actErr->FlagCheckResetCheckError = 0;//Дбавлено для 1.9
          	FileClose( fp );
            SetVarIndexed(IDX_ERRORSSIZE, FileSize(PATH_ERROR_FLASH));
				}
			}
		}
		actErr = GSList_GetNextData( &EList.List );
	}
	return 0;
}

void AutomaticFormattingFile(tVisuData * VData, tGsFile ** fp) {
  int32_t NUMBERAUTO = GetVar(HDL_NUMBERAUTO) + 1;
  int32_t NUMBERFORM = GetVar(HDL_NUMBERFORM);
  char string[12];
  const char * FileDate = RCTextGetText(RCTEXT_T_FILECREATEDATE, GetVarIndexed(IDX_SYS_LANGUAGE));
  RTCGetDate( & VData -> Date);
  snprintf(string, GS_ARRAYELEMENTS(string), "%02d.%02d.%02d", VData -> Date.Day, VData -> Date.Month, VData -> Date.Year);
  FilePrintf( * fp, "                                      \n");
  FilePrintf( * fp, "                                      \n");
  FilePrintf( * fp, "                                      \n");
  FilePrintf( * fp, "                                                                                                       ");
  FilePrintf( * fp, "\nАвтоматическое форматирование|");
  int32_t day1 = GetVar(HDL_KOLTIMEDAY);
  int32_t month1 = GetVar(HDL_KOLTIMEMONTH);
  int32_t year1 = GetVar(HDL_KOLTIMEYEAR);

  FilePrintf( * fp, "\n %s - %02d.%02d.%02d |\n",FileDate, day1, month1, year1);
  FilePrintf( * fp, "%s |", string);
  FilePrintf( * fp, "Количество автоматических форматирований: %d |", NUMBERAUTO);
  FilePrintf( * fp, "Количество сервисных форматирований: %d |\n", NUMBERFORM);
  SetVar(HDL_NUMBERAUTO, NUMBERAUTO);
}

void ManualFormattingFile(tVisuData * VData, tGsFile ** fp) {
  int32_t NUMBERFORM = GetVar(HDL_NUMBERFORM) + 1;
  char string[12];
  const char * FileDate = RCTextGetText(RCTEXT_T_FILECREATEDATE, GetVarIndexed(IDX_SYS_LANGUAGE));
  RTCGetDate( & VData -> Date);
  snprintf(string, GS_ARRAYELEMENTS(string), "%02d.%02d.%02d", VData -> Date.Day, VData -> Date.Month, VData -> Date.Year);
  FilePrintf( * fp, "                                      \n");
  FilePrintf( * fp, "                                      \n");
  FilePrintf( * fp, "                                      \n");
  FilePrintf( * fp, "                                                                                                       ");
  FilePrintf( * fp, "\nСервисное форматирование|");
  int32_t day1 = GetVar(HDL_KOLTIMEDAY);
  int32_t month1 = GetVar(HDL_KOLTIMEMONTH);
  int32_t year1 = GetVar(HDL_KOLTIMEYEAR);

  FilePrintf( * fp, "\n %s - %02d.%02d.%02d |\n",FileDate, day1, month1, year1);
  FilePrintf( * fp, " %s |", string);
  FilePrintf( * fp, "Количество сервисных форматирований: %d |\n", NUMBERFORM);
  SetVar(HDL_NUMBERFORM, NUMBERFORM);
}

int32_t FormattingErrorFile(tVisuData * VData) {
  uint32_t Password = (uint32_t) GetVarIndexed(IDX_PASSWORD_ENTER);
  if (Password == (uint32_t) GetVar(HDL_PASSWORD_MERKATOR) || Password == 32546) {
    tGsFile * fp = FileOpen(PATH_ERROR_FLASH, "w+");
    //tGsError * actErr;
    ManualFormattingFile(VData, & fp);
    SetVarIndexed(IDX_ERRORSSIZE, FileSize(PATH_ERROR_FLASH));
    FileClose(fp);
  }
  return 0;
}

int32_t UpdateErrorFile(tVisuData * VData) {
  if (GetVarIndexed(IDX_ERRORSSIZE) > 1000000) { // Тут задается размер файла в байтах.
    tGsFile * fp = FileOpen(PATH_ERROR_FLASH, "w+");
    //tGsError * actErr = GSList_GetLastData( & EList.List);
    AutomaticFormattingFile(VData, & fp);
    SetVarIndexed(IDX_ERRORSSIZE, FileSize(PATH_ERROR_FLASH));
    FileClose(fp);
  }
  return 0;
}

int32_t Reset( void ) {
	tGsError* actErr = GSList_GetFirstData( &EList.List );
	while( NULL != actErr ) {
		if( actErr->FlagCheckResetNoCheckError != 1 ) {
			if( ES_UNCHECKED_ERROR_INACTIVE == actErr->State ) {
      tGsFile* fp = FileOpen( PATH_ERROR_FLASH, "a+" );
				ErrorsWriteFile( actErr, &fp );
				actErr->FlagCheckResetNoCheckError = 1;
				actErr->ControlWriteNewError = 0;//Дбавлено для 1.9
        FileClose( fp );
        SetVarIndexed(IDX_ERRORSSIZE, FileSize(PATH_ERROR_FLASH));
			}
		}
		if( actErr->FlagCheckResetCheckError != 1 ) {
			if( ES_CHECKED_ERROR_INACTIVE == actErr->State ) {
      tGsFile* fp = FileOpen( PATH_ERROR_FLASH, "a+" );
				ErrorsWriteFile( actErr, &fp );
				actErr->FlagCheckResetCheckError = 1;
				actErr->ControlWriteNewError = 0;//Дбавлено для 1.9
        FileClose( fp );
        SetVarIndexed(IDX_ERRORSSIZE, FileSize(PATH_ERROR_FLASH));
			}
		}
		actErr = GSList_GetNextData( &EList.List );
	}
	return 0;
}

void VinEquipWrite( tGsError* Error, tGsFile** fp ) {
	FilePrintf( *fp, "VIN-%s         \n", Parameter.VIN.VIN );
	FilePrintf( *fp, "EQUIP_1-%s         \n", Parameter.VIN.Equip1 );
	FilePrintf( *fp, "EQUIP_2-%s         \n", Parameter.VIN.Equip2 );
}

int32_t VinEquipUpdate( void ) {
	tGsFile* fp = FileOpen( PATH_ERROR_FLASH, "r+" );
	tGsError* actErr = GSList_GetFirstData( &EList.List );
	FileSeek( fp, 0, GS_FILE_SEEK_SET );
	VinEquipWrite( actErr, &fp );
	FileClose( fp );
	return 0;
}
/*************/

uint32_t EList_GetNumCheckedErrors( void ) {
	int32_t count = 0;
	tGsError* actErr = GSList_GetFirstData( &EList.List );
	while( NULL != actErr ) {
		if( (actErr->State == ES_CHECKED_ERROR_ACTIVE)
			|| (actErr->State == ES_CHECKED_ERROR_INACTIVE) ) {
			count++;
		}
		actErr = GSList_GetNextData( &EList.List );
	}
	return count;
}

uint32_t EList_GetErrors( uint32_t ErrorStates ) {
	int32_t count = 0;
	tGsError* actErr = GSList_GetFirstData( &EList.List );
	while( NULL != actErr ) {
		if( actErr->State & ErrorStates ) {
			count++;
		}
		actErr = GSList_GetNextData( &EList.List );
	}
	return count;
}

int32_t EList_SetErrByRCText( int32_t id, uint32_t RCText ) {
	return EList_SetErrLevByRCText( VBL_ERROR, id, RCText );
}

int32_t EList_SetErrByRCTextList( int32_t id, uint32_t RCText_List, uint32_t RCText_ListIndex ) {
	return EList_SetErrLevByRCTextList( VBL_ERROR, id, RCText_List, RCText_ListIndex );
}

int32_t EList_SetErrLevByRCText( eErrLevel lev, int32_t id, uint32_t RCText, ... ) {
	if( 0 > id )
		id = RCText + 10000;

	const char* text = RCTextGetText( RCText, GetVarIndexed( IDX_SYS_LANGUAGE ) );
	if( NULL == text )
		return -1;
	char buffer[256];
	va_list args;
	va_start( args, RCText );
	vsnprintf( buffer, 256, text, args );
	va_end( args );

	return EList_SetErrLevByText( lev, id, buffer );
}

int32_t EList_SetErrLevByRCTextList( eErrLevel lev, int32_t id, uint32_t RCText_List, uint32_t RCText_ListIndex ) {
	if( 0 > id )
		id = RCText_List * 1000 + RCText_ListIndex;
	const char* text = RCTextGetListElement( RCText_List, RCText_ListIndex, GetVarIndexed( IDX_SYS_LANGUAGE ) );
	return EList_SetErrLevByText( lev, id, text );
}

int32_t CheckID( void* data, void* arg ) {
	tGsError* Error_Check = data;
	tGsError* Error_Data = arg;
	if( Error_Check->id > Error_Data->id )
		return -1;
	else
		return 0;
}

int32_t EList_SetErrLevByText( eErrLevel lev, int32_t id, const char* fmt, ... ) {
	tGsError* Error = EList_SearchId( id );
	int32_t TextChange = 0;
	if( NULL != Error ) //Fehler ist schon in der Liste vorhanden
	{
		Error->newErr = 0;
		if( 0 != strcmp( fmt, Error->text ) ) // String hat sich  geändert.
		{

//			db_out( "Text changed\r\n" );
			free( Error->text );
			char buffer[256];
			va_list args;
			va_start( args, fmt );
			vsnprintf( buffer, 256, fmt, args );
			va_end( args );
			Error->text = strdup( buffer );
			EList.change = 1;
			TextChange = 1;
		}
		//db_out("ErrorState = %d\r\n");
		if( (ES_CHECKED_ERROR_ACTIVE == Error->State)
			|| (ES_UNCHECKED_ERROR_ACTIVE == Error->State) ) {
			if( TextChange ) {
				ErrOut( Error );
			}
		}
		else if( ES_CHECKED_ERROR_INACTIVE == Error->State ) // Inaktiver Fehler wieder auf Fehler gesetzt
		{
			EList.change = 1;
			Error->State = ES_CHECKED_ERROR_ACTIVE;
			Error->newErr = 1;
			ErrOut( Error );
		}
		else {
			EList.change = 1;
			Error->State = ES_UNCHECKED_ERROR_ACTIVE;
			Error->newErr = 1;
			ErrOut( Error );
		}
		return Error->id;
	}
	else {
		tGsError NewError;
		memset( &NewError, 0, sizeof( tGsError ) );
		RTCGetDate( &NewError.Date );
		RTCGetTime( &NewError.Time );
		NewError.id = id;
		NewError.Level = lev;
		NewError.newErr = 1;
		char buffer[256];
		//db_out("Errid = %d\r\n", id);
		// db_out("Errorstring: %s", fmt);

		va_list args;
		va_start( args, fmt );
		vsnprintf( buffer, 256, fmt, args );
		va_end( args );
		NewError.text = strdup( buffer );
		NewError.State = ES_UNCHECKED_ERROR_ACTIVE;

		//db_out("ErrOut(&NewError);");
		ErrOut( &NewError );
		//In Liste Eingliedern

		Error = GSList_SearchData( &EList.List, &NewError, CheckID );

		EList.change = 1;
		//db_out("Add Data");
		GSList_AddData( &EList.List, &NewError, sizeof( tGsError ) );
		return NewError.id;
	}
	return id;
}

void EList_ResetErr( int32_t id ) {
	tGsError* Error = EList_SearchId( id );
	if( Error != NULL ) {
		if( (ES_UNCHECKED_ERROR_ACTIVE == Error->State)
			|| (ES_CHECKED_ERROR_ACTIVE == Error->State) ) {
			EList.change = 1;
			if( ES_UNCHECKED_ERROR_ACTIVE == Error->State ) {
				Error->State = ES_UNCHECKED_ERROR_INACTIVE;
				ErrOut( Error );
			}
			else {
				Error->State = ES_CHECKED_ERROR_INACTIVE;
				ErrOut( Error );
			}
		}
	}
}

void EList_CheckErr( int32_t id ) {
	tGsError* Error = EList_SearchId( id );
	if( Error != NULL ) {
		if( ES_UNCHECKED_ERROR_ACTIVE == Error->State ) {
			EList.change = 1;
			Error->State = ES_UNCHECKED_ERROR_INACTIVE;
			ErrOut( Error );
		}
		else if( ES_CHECKED_ERROR_ACTIVE == Error->State ) {
			EList.change = 1;
			Error->State = ES_CHECKED_ERROR_INACTIVE;
			ErrOut( Error );
		}
	}
}

int32_t CheckEqID( void* data, void* arg ) {
	tGsError* Error_Check = data;
	tGsError* Error_Data = arg;
	if( Error_Check->id < Error_Data->id )
		return -1;
	else if( Error_Check->id > Error_Data->id )
		return 1;
	else
		return 0;
}

void EList_DeleteErr( int32_t id ) {
	EList.change = 1;
	tGsError ErrorToSearch;
	ErrorToSearch.id = id;
	tGsError* Error = GSList_SearchData( &EList.List, &ErrorToSearch, CheckEqID );
	GSList_DeleteData( &EList.List, &Error );
}

int32_t EList_Search_id_cb( void* data, void* arg ) {
	tGsError* Error = data;
	int32_t id = (int32_t)arg;
	if( id == Error->id )
		return 0;
	else return -1;
}

tGsError* EList_GetErr( int32_t idx ) {
	tGsError* Error = GSList_GetFirstData( &EList.List );
	for( int32_t i = 0; i < idx; i++ ) {
		Error = GSList_GetNextData( &EList.List );
	}
	return Error;
}

void EList_AutoErrorRemove( void ) {
	tGsError* Error = GSList_GetFirstData( &EList.List );
	while( NULL != Error ) {
		if( ES_CHECKED_ERROR_INACTIVE == Error->State ) {
			if( 3000 < GetMSTick() - Error->timeold ) {
				GSList_DeleteData( &EList.List, Error );
				Error = GSList_GetNextData( &EList.List );
				EList.change = 1;
				continue;
			}
		}
		else {
			Error->timeold = GetMSTick();
		}
		Error = GSList_GetNextData( &EList.List );
	}
}

void ErrDraw_Enter_cb( void* arg, int32_t Selection, int32_t Offset ) {
	int32_t ErrIdx = Selection + Offset;
	tGsError* Error = EList_GetErr( ErrIdx );
	if( NULL != Error ) {
		if( Error->State == ES_UNCHECKED_ERROR_ACTIVE ) {
			Error->State = ES_CHECKED_ERROR_ACTIVE;
			ErrOut( Error );
		}
		else if( Error->State == ES_UNCHECKED_ERROR_INACTIVE ) {
			Error->State = ES_CHECKED_ERROR_INACTIVE;
			ErrOut( Error );
		}
	}
	EList.change = 1;
}

void ErrDraw_Select_cb( void* arg ) {
	EList.change = 1;
	ErrDraw_Draw();
}

void _ErrDraw_Init_( void ) {
	if( NULL == Table.sl ) {
		Table.sl = ScrList_Create( &Table );
		ScrList_Init_SetEnter_cb( Table.sl, ErrDraw_Enter_cb );
		ScrList_Init_SetSelect_cb( Table.sl, ErrDraw_Select_cb );
	}
}

int32_t ErrDraw_AddLine( uint32_t TextObj_Num, uint32_t TextObj_Text, uint32_t Checkbox_Hdl ) {
	_ErrDraw_Init_();
	tErrDraw_Line* newLines = realloc( Table.Line, sizeof( tErrDraw_Line ) * (Table.numLines + 1) );
	if( NULL == newLines ) {
		return -1;
	}
	Table.Line = newLines;
	int32_t idx = Table.numLines;
	Table.numLines++;
	tErrDraw_Line* newLine = &newLines[idx];
	newLine->Checkbox_Hdl = Checkbox_Hdl;
	newLine->TextObj_Num = TextObj_Num;
	newLine->TextObj_Text = TextObj_Text;
	EList.change = 1;
	ScrList_Init_SetNumShownLines( Table.sl, Table.numLines );

	return idx;
}

void ErrDraw_InitErrorFrame( int32_t y_PosMin, int32_t PosStep, int32_t Pos_x, int32_t Width ) {
	_ErrDraw_Init_();
	ScrList_Init_SetFrame( Table.sl, PosStep, Width );
	ScrList_Init_SetLinePositions( Table.sl, y_PosMin, PosStep );

}

void ErrDraw_InitSetFrameObj( int32_t ObjID ) {
	_ErrDraw_Init_();
	ScrList_Init_SetFrameObjId( Table.sl, ObjID );
}

void ErrDraw_SetContainerOffset( int32_t CntPosX, int32_t CntPosY ) {
	_ErrDraw_Init_();
	ScrList_Init_SetContainer( Table.sl, CntPosX, CntPosY );

}

/*******************************************************************************
void ErrorList_Draw( tErrorList *Table.int32_t lines, tError *Errors, int32_t numErrors)

drawing the error list.
-Fill the lines of the list with active errors.
-set color of the shown errors

*******************************************************************************/
void ErrDraw_Draw( void ) {
	int32_t offset = ScrList_GetOffset( Table.sl );

	if( 0 == EList.change )
		return;
	tGsError* Error = GSList_GetFirstData( &EList.List );
	uint32_t i;
	for( i = 0; i < (uint32_t)offset; i++ ) {
		Error = GSList_GetNextData( &EList.List );

		if( Error == NULL ) {
			break;
		}
	}
	int32_t Lines = 0;
	for( i = 0; i < Table.numLines; i++ ) {
		if( Error == NULL ) {
			break;
		}
		tErrDraw_Line* Line = &Table.Line[i];
		char string[128];
		snprintf( string, 128, "0x%04x", Error->id );
		SetVisObjData( Line->TextObj_Num, string, strlen( string ) + 1 );
		if( NULL != Error->text )
			SetVisObjData( Line->TextObj_Text, Error->text, strlen( Error->text ) + 1 );
		int32_t color = 0;
		switch( Error->State ) {
		case ES_CHECKED_ERROR_ACTIVE: color = Table.Color.ActCheckdErr; break;
		case ES_UNCHECKED_ERROR_ACTIVE: color = Table.Color.ActUnCheckdErr; break;
		case ES_CHECKED_ERROR_INACTIVE: color = Table.Color.InActCheckdErr; break;
		case ES_UNCHECKED_ERROR_INACTIVE: color = Table.Color.InActUnCheckdErr; break;
		default:

			break;
		}
		SendToVisuObj( Line->TextObj_Num, GS_TO_VISU_SET_FG_COLOR, color );
		SendToVisuObj( Line->TextObj_Text, GS_TO_VISU_SET_FG_COLOR, color );
		int32_t icon_idx = 0;
		for( int32_t j = 0; j < 5; j++ ) {
			if( 1 == (Error->State >> j) ) {
				icon_idx = j + 1;
				break;
			}
		}
		SetVar( Line->Checkbox_Hdl, icon_idx );
		SendToVisuObj( Line->TextObj_Num, GS_TO_VISU_SET_ATTR_ALL, ATTR_VISIBLE | ATTR_TRANSPARENT | ATTR_SMOOTH );
		SendToVisuObj( Line->TextObj_Text, GS_TO_VISU_SET_ATTR_ALL, ATTR_VISIBLE | ATTR_TRANSPARENT | ATTR_SMOOTH );
		Error = GSList_GetNextData( &EList.List );

		Lines++;
	}
	Table.ShownLines = Lines;
	for( ; i < Table.numLines; i++ ) {
		tErrDraw_Line* Line = &Table.Line[i];
		SendToVisuObj( Line->TextObj_Num, GS_TO_VISU_SET_ATTR_ALL, 0 );
		SendToVisuObj( Line->TextObj_Text, GS_TO_VISU_SET_ATTR_ALL, 0 );
		SetVar( Line->Checkbox_Hdl, ES_NOERROR );
	}
	ScrList_Draw( Table.sl );
	EList.change = 0;
}

int32_t ErrDraw_Cycle( uint32_t evtc, tUserCEvt* evtv ) {
	ScrList_Cycle( Table.sl, evtc, evtv, EList.List.numItems );
	ErrDraw_Draw();
	return 0;
}

void ErrDraw_SetTextColor( eErrorState State, uint32_t color ) {
	EList.change = 1;
	switch( State ) {
	case ES_UNCHECKED_ERROR_ACTIVE:
		Table.Color.ActUnCheckdErr = color;
		break;

	case ES_UNCHECKED_ERROR_INACTIVE:
		Table.Color.InActUnCheckdErr = color;
		break;

	case ES_CHECKED_ERROR_ACTIVE:
		Table.Color.ActCheckdErr = color;
		break;

	case ES_CHECKED_ERROR_INACTIVE:
		Table.Color.InActCheckdErr = color;
		break;
	default:
		break;
	}
}

int32_t Error_CopyToUsb( void ) {
	return CopyFile( PATH_ERROR_FLASH, PATH_ERROR_USB, NULL );
}
