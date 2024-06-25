#ifndef VISU_PROFILE_H
#define VISU_PROFILE_H

#include "visu.h"


void Visu_Profile_Open( const tVisuData* VData );
void Visu_Profile_Cycle( const tVisuData* VData, uint32_t evtc, tUserCEvt* evtv );
void Visu_ProfileEdit_Open( const tVisuData* VData );
void Visu_ProfileEdit_Cycle( const tVisuData* VData, uint32_t evtc, tUserCEvt* evtv );

void Visu_RTR_Edit_Open( const tVisuData* VData );
void Visu_RTR_Edit_Cycle( const tVisuData* VData, uint32_t evtc, tUserCEvt* evtv );

#endif  // #ifndef VISU_PROFILE_H
