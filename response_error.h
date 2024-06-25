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
#ifndef RESPONSE_ERORR_H
#define RESPONSE_ERORR_H
#define PATH_ERRORREACT_FLASH "/gs/data/errorreact.cfg"
#define PATH_ERRORREACT_USB "/gs/usb/errorreact.cfg"
#define PATH_ERRORREACT_USB_BACKUP "/gs/usb/BACKUP_LAST/errorreact.cfg"
#include "visu.h"
/* macro definitions ********************************************************/

/* type definitions *********************************************************/

/* prototypes ***************************************************************/

/* global constants *********************************************************/

/* global variables *********************************************************/

/* global function prototypes ***********************************************/
void Error_Signal_Init( const tVisuData* VData );
void Error_Signal_Cycle( const tVisuData* VData, uint32_t evtc, tUserCEvt* evtv );
void Error_Signal_Open( const tVisuData* VData );
char * ReactTxt(int i);
int ReactSignalInError(int i);
int ReactDoingInError(int i);
int SizeOfReact( void );
void ErrorReaction_Import_Backup(void);
void ErrorReaction_Export_Backup( void );
void WrInFileReact( void );

/****************************************************************************/
/* Protection against multiple includes ends here ! Stop editing here !     */
/****************************************************************************/
#endif  // #ifndef RESPONSE_ERORR_H
