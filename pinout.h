/****************************************************************************
 *
 * Project:   name
 *
 * @file      filename.h
 * @author    author
 * @date      [Creation date in format %02d.%02d.20%02d]
 *
 * @brief     description
 *
 ****************************************************************************/

/* Protection against multiple includes.                                    */
/* Do not code anything outside the following defines (except comments)     */
#ifndef PINOUT_H
#define PINOUT_H

#define PATH_PINOUT "/gs/data/pinout.cfg"
#define PATH_PINOUT_USB "/gs/usb/pinout.cfg"
#define PATH_PINOUT_BACKUP "/gs/usb/BACKUP_LAST/pinout.cfg"

#define Pin_None 0
#define Pin_3 1
#define Pin_4 2
#define Pin_5 3
#define Pin_6 4
#define Pin_7 5
#define Pin_8 6
#define Pin_9 7
#define Pin_10 8
#define Pin_28 9
#define Pin_29 10

#define Valve_PVEO_1 0
#define Valve_PVEO_2 1
#define Valve_PVEH_16 2
#define Valve_PVEH_32 3
#define Valve_RG 4
#define Valve_PVEH_OMFB 5

#define HydDistributor_Actual 0
#define HydDistributor_Danfoss 1
#define HydDistributor_OMFB 2
#define HydDistributor_OMFBv2 3
#define HydDistributor_RG 4


/* macro definitions ********************************************************/

/* type definitions *********************************************************/

/* prototypes ***************************************************************/

/* global constants *********************************************************/

/* global variables *********************************************************/

/* global function prototypes ***********************************************/

typedef struct tagPinOut{
  char NameAction[20];
  int32_t MCM_A;
  int32_t MCM_B;
  int32_t Pin_A;
  int32_t Pin_B;
  uint32_t ValveType;
}tPinOut;

void PinOut_Load(void);
void PinOut_Write(void);
void PinOut_Read(void);

int16_t PinOut_Get_MCM_A (uint8_t Action, uint8_t Equip);
int16_t PinOut_Get_MCM_B (uint8_t Action, uint8_t Equip);
int16_t PinOut_Get_Pin_A (uint8_t Action, uint8_t Equip);
int16_t PinOut_Get_Pin_B (uint8_t Action, uint8_t Equip);
uint16_t PinOut_Get_ValveType (uint8_t Action, uint8_t Equip);
void PinOut_Write_Struct(void);
void PinOut_Change_Valv( void );

int PinOut_WriteAllToFile(const char * path);
int PinOut_ReadBackup(const char * path);
/****************************************************************************/
/* Protection against multiple includes ends here ! Stop editing here !     */
/****************************************************************************/
#endif  // #ifndef PINOUT_H
