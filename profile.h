/************************************************************************
*
* File:         PROFILE.h
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
#ifndef PROFILE_H
#define PROFILE_H

/*************************************************************************/

/* macro definitions *****************************************************/

#define MODULES_NUM 4
#define PROFILE_PATH    "/gs/data/profiles.cfg"
#define PROFILE_PATHUSB "/gs/usb/profiles.cfg"
#define PROFILE_PATHUSB_BACKUP "/gs/usb/BACKUP_LAST/profiles.cfg"

/* type definitions ******************************************************/

#include "gselist.h"
#include "control.h"

enum	CMD_CHG_PROFILE {
	PROF_CHG	= 0x10,		///< изменения в профиле
	PROF_EXP	= 0x20,		///< экспорт профиля на флешку
	PROF_IMP	= 0x40,		///< импорт профиля с флешки
};

typedef enum eagProfileType {
	P_TYPE_TOP,
	P_TYPE_A,
	P_TYPE_B,
	P_TYPE_C,
} eProfileType;

typedef enum eagAttachmentPicIdx {
	ATPIC_NONE,
	ATPIC_PLOUGH_NT,
	ATPIC_PLOUGH_FK = 2,
	ATPIC_PLOUGH_SIDE = 3,
	ATPIC_OMP253 = 4,
	ATPIC_OMB = 5,
	ATPIC_OMT = 6,
	ATPIC_OPF = 7,
	ATPIC_MF300_OUT = 8,
	ATPIC_MF300_IN = 9,
	ATPIC_STEELBLADE_DOWN = 10,
	ATPIC_STEELBLADE_UP = 11,
	ATPIC_PLOUGH_CH2600 = 15,
} eAttachmentPicIdx;

typedef void (*tProfile_Init_cb)(tControl* Ctrl);
typedef void (*tProfile_Timer10ms_cb)(tControl* Ctrl);
typedef void (*tProfile_Cycle_cb)(tControl* Ctrl, uint32_t evtc, tUserCEvt* evtv);

typedef struct tProfileModule {
	uint32_t				RC_Name;	///< Textlistentry to define the Name of the module
	eAttachmentPicIdx		Icon;		///< Icon of the module to be shown on the button in the main view.
										///< The Icons can be found in PRFILE_ICONS.PNG. The Position of the Icons are defined in ePrifleIcons
	uint32_t				Cnt;		///< this Container will be shown in the main view, if the Module is active
	tProfile_Init_cb		Init_cb;	///< Will becalled if the module will be activated. (Project start or chanign module)
	tProfile_Cycle_cb		Cycle_cb;	///< This will be called every cycle.
	tProfile_Timer10ms_cb	Timer_cb;	///< This will be called every 10 ms.
} tProfileModule;

/**
* @brief Structure to save a profile
*
*/
typedef struct tagPofile {
	char			name[128];			///< Name of the Profile
	uint32_t		module[4];			///< Index of the  modules in their array of the type tProfileModule.
}	tProfile;

/**
* @brief Structure used to get access to all Modules types.
* By the Array ProfModules it is possible to have access to all Modules.
*
*/
typedef struct tagModules {
	tProfileModule* p;
	uint32_t        p_num;
	uint32_t        RCtListOffset;
}	tModules;

extern uint32_t			q_chg_profile;			///< была команда на смену параметров профиля

/* prototypes ************************************************************/

/* global constants ******************************************************/

/* global variables ******************************************************/

/* global function prototypes ********************************************/
void Prof_Init( void );

/**	@brief returns the currently selected profile
**	@return tProfile*	**/
tProfile* Profile_GetCurrentProfile( void );

/**	@brief returns the list off all Profiles
**	@return tGSList*		**/
tGSList* Profile_GetProfileList( void );

int32_t Prof_Load( void );

/**	@brief Saves the actual loaded profiles and rewrites the
**		file /gs/data/profiles.cfg on the flash new
**	@return int32_t		**/
int32_t Prof_Save( void );

/**	@brief Copies the profiles.cfg-File from USB to flash. The File will be loaded
**		after the next reboot.
**	@return int32_t	**/
int32_t Prof_LoadFromUsb( void );

/**
* @brief Copies the profiles.cfg-File from USB to flash. The File will be loaded
* after the next reboot.
*
* @return int32_t
*/
int32_t Prof_CopyToUsb( void );

void Prof_SetCurrentProfile( tProfile* Prof );

/**#
* @brief this Function shows Container and Buttons of the profiles on the mainscreen.
*
*/
void Prof_Draw( void );

/**
* @brief This Function closes the Container on the Main Screen. This function has
* to be called if you close it.
*
*/
void ProfHide( void );

/**
* @brief This Function will open a container if Attachment A, B or C will be moved. This container shows, in which direction
* the attachment is moving.
*
*/
void Prof_DrawMove( void );
void Profile_Change_Draw( void );
void Pro_SetCurrentProfile( tProfile* Prof );

void Prof_Cycle( tControl* Ctrl, uint32_t evtc, tUserCEvt* evtv );
void Prof_Timer_10ms( tControl* Ctrl );

int32_t Profile_GetIndex( tProfile* ProfSearch );

tProfileModule* GetCurrentModule( uint32_t idx );
void SetRTR_CntUH(void);
void SetTop_CntEmpty(void);

void Ctrl_RTR_Init(tControl * Ctrl);
void Ctrl_RTR_Cycle(tControl * Ctrl, uint32_t evtc, tUserCEvt * evtv);
void Ctrl_RTR_Timer_10ms( tControl * Ctrl );

/**
* @brief Получение наименования оборудования.
* @param idx Индекс оборудования:
* 0 - Основное (top) оборудование;
* 1 - Переднее оборудование;
* 2 - Межосевое оборудование;
* 3 - Боковое оборудование.
* @return char
*/
char* Profile_GetName ( uint8_t idx );

/*************************************************************************/
/* Protection against multiple includes ends here ! Stop editing here !  */
/*************************************************************************/
#endif  // #ifndef PROFILE_H
