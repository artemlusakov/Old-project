/****************************************************************************
*
* File:         VISU_SENSORS.c
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
#include "visu.h"
#include "visu_sensors.h"
#include "control.h"
#include "gsToVisu.h"
/****************************************************************************/

/* macro definitions ********************************************************/

#define SENSORS_FN    "/gs/data/sensors_uh.cfg"
#define PATH_SENSORS_USB "/gs/usb/BACKUP_LAST/sensors.cfg"

/* type definitions *********************************************************/

struct _SSensors {
	uint32_t	humid_sens;
	uint32_t	spread_sens;
	uint32_t	tail_sens;
	uint32_t	humid_rot;
	uint32_t	conv_rot;
	uint32_t	temp_sens;
};

/* prototypes ***************************************************************/

/* global constants *********************************************************/
static struct _SSensors		sens;

/* global variables *********************************************************/

/* local function prototypes*************************************************/

static int8_t		sens_wr_file( void );
static int8_t		sens_rd_file( void );

/****************************************************************************/

/* function code ************************************************************/
/**	@brief sens_wr_file
**	@return					**/
static int8_t		sens_wr_file( void ) {
	tGsFile*	fp_ = FileOpen( SENSORS_FN, "wb" );
	uint32_t	rc_;
	if( NULL == fp_ )		{
  //db_out( "sensors write error open\r\n" );
  return -1;}
	rc_ = FileWrite( &sens, 1, sizeof(sens), fp_ );
	if( sizeof(sens) != rc_ )		{
  //db_out( "sensors write error:%u\r\n", rc_ );
  return -2;}
	FileSync( fp_ );
	FileClose( fp_ );
	//db_out( "sensors write ok\r\n" );
	return 0;
}

/**	@brief sens_rd_file
**	@return		**/
static int8_t		sens_rd_file( void ) {
	tGsFile*	fp_ = FileOpen( SENSORS_FN, "rb" );
	uint32_t	rc_;
	if( NULL == fp_ )		{//db_out( "sensors read error open file\r\n" );
  return -1;}
	rc_ = FileRead( &sens, 1, sizeof(sens), fp_ );
	if( sizeof(sens) != rc_ )		{//db_out( "sensors read error:%u\r\n", rc_ );
  return -2;}
	FileClose( fp_ );
	// db_out( "sensors read ok\r\n" );
	return 0;
}

void Visu_Sensor_Init( const tVisuData* Data ) {
	if( 0 == sens_rd_file() ) {
		SetVar( HDL_SW_HUMID_SENS, sens.humid_sens );
		SetVar( HDL_SW_SPREAD_SENS, sens.spread_sens );
		SetVar( HDL_SW_TAIL_POS, sens.tail_sens );
		SetVar( HDL_SW_HUMID_ROT, sens.humid_rot );
		SetVar( HDL_SW_CONV_ROT, sens.conv_rot );
		SetVar( HDL_SW_TEMP, sens.temp_sens );
	}	else {
		SetVar( HDL_SW_HUMID_SENS, 1 );
		SetVar( HDL_SW_SPREAD_SENS, 1 );
		SetVar( HDL_SW_TAIL_POS, 1 );
		SetVar( HDL_SW_HUMID_ROT, 1 );
		SetVar( HDL_SW_CONV_ROT, 1 );
		SetVar( HDL_SW_TEMP, 1 );
	}
}

void Visu_Sensor_Open( const tVisuData* VData ) {
	PrioMaskOn( MSK_ACTIVESENSORS );

	if( 0 == sens_rd_file() ) {
		SetVar( HDL_SW_HUMID_SENS, sens.humid_sens );
		SetVar( HDL_SW_SPREAD_SENS, sens.spread_sens );
		SetVar( HDL_SW_TAIL_POS, sens.tail_sens );
		SetVar( HDL_SW_HUMID_ROT, sens.humid_rot );
		SetVar( HDL_SW_CONV_ROT, sens.conv_rot );
		SetVar( HDL_SW_TEMP, sens.temp_sens );
	}	else {
		SetVar( HDL_SW_HUMID_SENS, 1 );
		SetVar( HDL_SW_SPREAD_SENS, 1 );
		SetVar( HDL_SW_TAIL_POS, 1 );
		SetVar( HDL_SW_HUMID_ROT, 1 );
		SetVar( HDL_SW_CONV_ROT, 1 );
		SetVar( HDL_SW_TEMP, 1 );
	}
}

void Visu_Sensor_SaveLocally( void ) {
	sens.humid_sens = GetVar( HDL_SW_HUMID_SENS );
	sens.spread_sens = GetVar( HDL_SW_SPREAD_SENS );
	sens.tail_sens = GetVar( HDL_SW_TAIL_POS );
	sens.humid_rot = GetVar( HDL_SW_HUMID_ROT );
	sens.conv_rot = GetVar( HDL_SW_CONV_ROT );
	sens.temp_sens = GetVar( HDL_SW_TEMP );
	sens_wr_file();
}

void Visu_Sensor_Cycle( const tVisuData* VData, uint32_t evtc, tUserCEvt* evtv ) {
	const tControl* Ctrl = CtrlGet();
	static int32_t timer = 0;
	if( 333 < (GetMSTick() - timer) ) {
		timer = GetMSTick();
		SetVarIndexed( IDX_IO_VIEW_DI_00, Ctrl->sens.DI.Spreading );
		SetVarIndexed( IDX_IO_VIEW_DI_01, Ctrl->sens.DI.Humidifier );
		SetVarIndexed( IDX_IO_VIEW_DI_02, Ctrl->sens.DI.MaterialPresense );
		SetVarIndexed( IDX_IO_VIEW_DI_03, Ctrl->sens.DI.Ignition );
		SetVarIndexed( IDX_IO_VIEW_DI_04, Ctrl->emcy.val );
		SetVarIndexed( IDX_IO_VIEW_DI_05, Ctrl->sens.DI.OilFilter );
		SetVarIndexed( IDX_IO_VIEW_DI_06, Ctrl->sens.DI.UpperOilLevel );
		SetVarIndexed( IDX_IO_VIEW_DI_07, Ctrl->sens.DI.LowerOilLevel );
		SetVarIndexed( IDX_IO_VIEW_DI_08, Ctrl->sens.DI.CheckOilLevelSensor );
		SetVarIndexed( IDX_IO_VIEW_DI_09, Ctrl->sens.DI.ClutchSensor );

		SetVarIndexed( IDX_IO_VIEW_AI_00, Ctrl->sens.Freq.ConveryRotation ); //AK NOT TESTED div by reductor ratio returned 0. How to make it showing decimals

		SetVarIndexed( IDX_IO_VIEW_AI_01, (IOT_NormAI( &Ctrl->sens.AI.OilTemp ) + 5) / 10 * 10 );
		SetVarIndexed( IDX_IO_VIEW_AI_02, IOT_NormAI(&Ctrl->sens.AI.OilPressure));
	}
	if( IsKeyPressedNew( 10 ) ) {
		Visu_Sensor_SaveLocally();
		Visu_OpenScreen( SCREEN_MAIN );
	}

	Visu_HomeKeyPressed();
}

typedef struct tagSensorsState {
	char name[64];
	int * val;
}tSens;

#define PATH_SENSOR_BACKUP_USB "/gs/usb/BACKUP_LAST/sensor.cfg"
#define SENSOR_BACKUPCOUNT_HEADER "Число сохраненных сенсоров = "
tSens sensors[] = {
	{.name = "Распределение ", 								.val = &sens.spread_sens},
	{.name = "Отсутствие жидкости в баках ", 	.val = &sens.humid_sens},
	{.name = "Конвейер ", 										.val = &sens.conv_rot},
	{.name = "Насос увлажнения ", 						.val = &sens.humid_rot},
	{.name = "Группа распределения опущена ", .val = &sens.tail_sens},
	{.name = "Температура ", 									.val = &sens.temp_sens},
};

void Sensors_SaveBacup( void ) {
	Visu_Sensor_SaveLocally();

	tGsFile* fp = FileOpen(PATH_SENSOR_BACKUP_USB, "w");
  if(fp == NULL) {
    return;
  }

	FilePrintf(fp, "%s%d\r\n", SENSOR_BACKUPCOUNT_HEADER, GS_ARRAYELEMENTS(sensors));
	for(int i = 0; i < GS_ARRAYELEMENTS(sensors); i++) {
		FilePrintf(fp, "%s%d\r\n", sensors[i].name, *sensors[i].val);
	}

	FileClose(fp);
}

char * Sensors_GetValPtrByHeader(const char * buf, const char * header) {
  if(buf == NULL || header == NULL) {
    return NULL;
  }

  char * pos = strstr(buf, header);
  if(pos) {
    return pos + strlen(header);
  }

  return NULL;
}

#define READ_BUFFER_SIZE 128
void Sensors_LoadBacup( void ) {
	for(int i = 0; i < GS_ARRAYELEMENTS(sensors); i++) {
		*sensors[i].val = 1;
	}

	tGsFile* fp = FileOpen(PATH_SENSOR_BACKUP_USB, "r");
  if(fp == NULL) {
    return;
  }

	char buffer[READ_BUFFER_SIZE];
  FileGets(buffer, READ_BUFFER_SIZE, fp);
	int size = atoi(Sensors_GetValPtrByHeader(buffer, SENSOR_BACKUPCOUNT_HEADER));

	for(int i = 0; i < size; i++) {
		if(!FileGets(buffer, READ_BUFFER_SIZE, fp)) {
      break;
    }

		for(int j = 0; j < GS_ARRAYELEMENTS(sensors); j++) {
			char * pos = Sensors_GetValPtrByHeader(buffer, sensors[j].name);

			if(pos) {
				*sensors[j].val = atoi(pos);
				break;
			}
		}
	}

	sens_wr_file();
}
