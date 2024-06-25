/************************************************************************
*
* File:         CTRL_PLOUGH.h
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
#ifndef CTRL_PLOUGH_H
#define CTRL_PLOUGH_H
#include "hydvalvectrl.h"
#include "io_types.h"
#include "gsemaint_runtime.h"
#include "ctrl_abc_attach.h"
#include "commands.h"
#include "errorlist.h"
/*************************************************************************/

/* macro definitions *****************************************************/

/* type definitions ******************************************************/

/* prototypes ************************************************************/

/* global constants ******************************************************/

/* global variables ******************************************************/

/* global function prototypes ********************************************/

/**
* @brief Set Valve to go Up and down.
*
* @param Plough
* @param Device
* @param Pos
*/
void Plough_Maint_Create( tABC_Attach* ABC_Attach, tMaint* Maint, eErrorID ID, uint32_t RCText_Warn, uint32_t RCText_Service, uint32_t HdlRemVar_Warn, uint32_t Hdl_RemVar_Service );
void Plough_Maint_Create_Reset( tABC_Attach* ABC_Attach, tMaint* Maint, eErrorID ID, uint32_t RCText_Warn, uint32_t RCText_Service, uint32_t HdlRemVar_Warn, uint32_t Hdl_RemVar_Service );
void Plough_Lock_Init( tABC_Attach* Plough, uint32_t Device1, uint32_t Pos1 );
void Plough_TiltLock_Init( tABC_Attach* Plough, uint32_t Device1, uint32_t Pos1 );
void Plough_UpDown_Init( tABC_Attach* Plough, uint32_t Device1, uint32_t Pos1, uint32_t Device2, uint32_t Pos2, uint32_t ValveType );
void OMFB_UpDownSecond_Init( tABC_Attach* Plough, uint32_t Device1, uint32_t Pos1, uint32_t Device2, uint32_t Pos2, uint32_t ValveType );

void Plough_UpDown_Second_Init( tABC_Attach* Plough, uint32_t Device1, uint32_t Pos1, uint32_t Device2, uint32_t Pos2, int32_t ValveType );

/**
* @brief sets the valve to move between working position and drive position
* If Device1 != -1 Device2 == -1     , an propotional valve will be initialized
* If Device1 != -1 Device2 != -1     , two discret valves will be initialized
* If Device1 == -1 Device2 == -1     , no valves will be initialized
* @param Plough
* @param Device1
* @param Pos1
* @param Device2
* @param Pos2
*/
void Plough_WorkTransport( tABC_Attach* Plough, uint32_t Device1, uint32_t Pos1, uint32_t Device2, uint32_t Pos2, uint32_t ValveType );

/**
* @brief sets the valve to move between working position and drive position
* If Device1 != -1 Device2 == -1     , an propotional valve will be initialized
* If Device1 != -1 Device2 != -1     , two discret valves will be initialized
* If Device1 == -1 Device2 == -1     , no valves will be initialized
* @param Plough
* @param Device1
* @param Pos1
* @param Device2
* @param Pos2
*/
void Plough_TiltUpDown( tABC_Attach* Plough, uint32_t Device1, uint32_t Pos1, uint32_t Device2, uint32_t Pos2, uint32_t ValveType );

/**
* @brief sets the Float ventil and activates float functionality.
* Use 0 for device and pos to deactivate floating.
*
* @param Plough
* @param Device MCM-Module
* @param Pos    Pos of Output
*/
void Plough_Float_Init( tABC_Attach* Plough, int32_t Device, int32_t Pos );

/**
* @brief sets the valve to turn the plough left and right. Use 0 for DeviceTurnLeft and PosTurnLeft to deactivate this function
  * If Device1 != -1 Device2 == -1     , an propotional valve will be initialized
* If Device1 != -1 Device2 != -1     , two discret valves will be initialized
* If Device1 == -1 Device2 == -1     , no valves will be initialized
* @param Plough
* @param DeviceTurnLeft MCM-Module
* @param PosTurnLeft Pos of Output
* @param DeviceTurnRightMCM-Module
* @param PosTurnRight Pos of Output
*/
void Plough_LeftRight_Init( tABC_Attach* Plough, uint32_t DeviceTurnLeft, uint32_t PosTurnLeft, uint32_t DeviceTurnRight, uint32_t PosTurnRight, uint32_t ValveType );

/**
* @brief sets the valve to lift and lower the steel sheeld. Use 0 for DeviceSteelUp and PosSteelUp to deactivate this function
* If Device1 != -1 Device2 == -1     , an propotional valve will be initialized
* If Device1 != -1 Device2 != -1     , two discret valves will be initialized
* If Device1 == -1 Device2 == -1     , no valves will be initialized
* @param Plough
* @param DeviceTurnLeft MCM-Module
* @param PosTurnLeft Pos of Output
* @param DeviceTurnRightMCM-Module
* @param PosTurnRight Pos of Output
*/
void Plough_SteelRubber_Init( tABC_Attach* Plough, uint32_t DeviceSteelUp, uint32_t PosSteelUp, uint32_t DeviceSteelDown, uint32_t PosSteelDown );

/**
* @brief sets the valve for retacting extracting
* If Device1 != -1 Device2 == -1     , an propotional valve will be initialized
* If Device1 != -1 Device2 != -1     , two discret valves will be initialized
* If Device1 == -1 Device2 == -1     , no valves will be initialized *
*
* @param Plough
* @param DeviceRetract MCM-Module
* @param PosRetract Pos of Output
* @param DeviceExtract MCM-Module
* @param PosExtract Pos of Output

*/
void Plough_ExRetract_Init( tABC_Attach* Plough, uint32_t DeviceRetract, uint32_t PosRetract, uint32_t DeviceExtract, uint32_t PosExtract );

/**
* @brief
*
* @param Plough
* @param DeviceRightSection MCM-Module Ventil to change between left and right section
* @param PosRightSection Pos of Output
*/
void Plough_ExRetract_RightSection( tABC_Attach* Attachment, uint32_t DeviceRightSection, uint32_t PosRightSection );

/**
* @brief This will let wobble the valve of the front plough a little bit, to prevent
* it getting stuck.
*
* @param Plough
*/
void Plough_UpDown_Timer_10ms( tABC_Attach* Plough );

/**
* @brief cycle function for Ploughs. Used to lift / lower plough
*
* @param Plough
*/
void Plough_Cycle( tABC_Attach* Plough, tJSCommands* Cmd, uint8_t Equip );
void Ctrl_Inter_Plough_CH2800_Cycle( tABC_Attach* Plough, tJSCommands* Cmd );
void Ctrl_Inter_Plough_CH2800_OMFB_Cycle( tABC_Attach* Plough, tJSCommands* Cmd );

/**
* @brief Cycle function if you use a plough with two sections, controled by Key A and C.
* All functions can be controled, if Key A or C is pressed. If Key C is pressed, the Valve for picking the right section is active.
*
*/
void Plough_2_Section_MSPN_Cycle(tABC_Attach *Plough_A, tJSCommands *Cmd_A, tABC_Attach *Plough_B, tJSCommands *Cmd_B);

/*************************************************************************/
/* Protection against multiple includes ends here ! Stop editing here !  */
/*************************************************************************/
#endif // #ifndef CTRL_PLOUGH_H
