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
#ifndef BACKUP_H
#define BACKUP_H

/* macro definitions ********************************************************/

/* type definitions *********************************************************/

/* prototypes ***************************************************************/

/* global constants *********************************************************/

/* global variables *********************************************************/

/* global function prototypes ***********************************************/

void Backup_Open(void);
void Save_Repair_Cycle(const tVisuData * VData,uint32_t evtc, tUserCEvt * evtv);
int protect_files(const char* in,int reed,const char* name);

/****************************************************************************/
/* Protection against multiple includes ends here ! Stop editing here !     */
/****************************************************************************/
#endif  // #ifndef BACKUP_H
