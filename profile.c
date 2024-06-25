#include <stdint.h>
#include <UserCEvents.h>
#include <UserCAPI.h>
#include "vartab.h"
#include "objtab.h"
#include "profile.h"
#include "gsToVisu.h"
#include "RCColor.h"
#include "RCText.h"
#include "gselist.h"
#include "control.h"
#include "pinout.h"
#include "ctrl_front_attach.h"
#include "ctrl_inter_attach.h"
#include "ctrl_side_attach.h"
#include "ctrl_top_attachment_cr.h"
#include "ctrl_top_attachment_cr600.h"
#include "ctrl_abc_attach.h"
#include "ctrl_manipulator.h"
#include "okb600.h"
#include "top_summer_attachment.h"
#include "rtr_uh.h"
#include "param.h"


#define INACTIVE INT32_MAX
#define ARROW_INVISIBLE 0
#define ARROW_WHITE     1
#define ARROW_GREEN_0   2
#define ARROW_GREEN_1   3

tGSList      ProfileList;

tProfileModule Module_A[RCTEXT_CNT_LT_A_ATTACHMENT] = {
	//Name of the Attachment                                        //Idx of Attachments.png    //Container showning
	{.RC_Name = RCTEXT_LI_A_ATTACHMENT_ATTACHMENT_NONE,				.Icon = ATPIC_NONE,			.Cnt = CNT_EMPTY,			.Init_cb = NULL,					.Cycle_cb = NULL,								.Timer_cb = NULL},
	{.RC_Name = RCTEXT_LI_A_ATTACHMENT_ATTACHMENT_BUCHERFKPLOUGH,	.Icon = ATPIC_PLOUGH_FK,	.Cnt = CNT_A_PLOUGH_FK,		.Init_cb = Ctrl_Front_FK_Init,		.Cycle_cb = Ctrl_Front_Plough_Standard_Cycle,	.Timer_cb = Ctrl_Front_Plough_Standard_Timer_10ms},
	{.RC_Name = RCTEXT_LI_A_ATTACHMENT_ATTACHMENT_BUCHERTN34,		.Icon = ATPIC_PLOUGH_NT,	.Cnt = CNT_A_PLOUGH_TN,		.Init_cb = Ctrl_Front_TN34_Init,	.Cycle_cb = Ctrl_Front_Plough_Standard_Cycle,	.Timer_cb = Ctrl_Front_Plough_Standard_Timer_10ms},
	{.RC_Name = RCTEXT_LI_A_ATTACHMENT_ATTACHMENT_MN,				.Icon = ATPIC_PLOUGH_NT,	.Cnt = CNT_A_PLOUGH_MN,		.Init_cb = Ctrl_Front_MN_Init,		.Cycle_cb = Ctrl_Front_Plough_Standard_Cycle,	.Timer_cb = Ctrl_Front_Plough_Standard_Timer_10ms},
	{.RC_Name = RCTEXT_LI_A_ATTACHMENT_ATTACHMENT_OFF_OPF_245,		.Icon = ATPIC_OPF,			.Cnt = CNT_A_BRUSH_OPF245,	.Init_cb = Ctrl_Front_OFF_OPF_245,	.Cycle_cb = Ctrl_Front_Brush_Standard_Cycle,	.Timer_cb = Ctrl_Front_Brush_Timer_10ms},
	{.RC_Name = RCTEXT_LI_A_ATTACHMENT_ATTACHMENT_OMB1,				.Icon = ATPIC_OMB,			.Cnt = CNT_A_BRUSH_OMB1,	.Init_cb = Ctrl_Front_OMB1,			.Cycle_cb = Ctrl_Front_Brush_Standard_Cycle,	.Timer_cb = Ctrl_Front_Brush_Timer_10ms},
	{.RC_Name = RCTEXT_LI_A_ATTACHMENT_ATTACHMENT_OMT1,				.Icon = ATPIC_OMT,			.Cnt = CNT_A_BRUSH_OMT1,	.Init_cb = Ctrl_Front_OMT1,			.Cycle_cb = Ctrl_Front_Brush_Standard_Cycle,	.Timer_cb = Ctrl_Front_Brush_Timer_10ms},
	{.RC_Name = RCTEXT_LI_A_ATTACHMENT_ATTACHMENT_MF300,			.Icon = ATPIC_MF300_OUT,	.Cnt = CNT_A_WASHER_MF300,	.Init_cb = Ctrl_Front_MF300,		.Cycle_cb = Ctrl_Front_Brush_Standard_Cycle,	.Timer_cb = Ctrl_Front_Brush_Timer_10ms},
	{.RC_Name = RCTEXT_LI_A_ATTACHMENT_ATTACHMENT_MF500,			.Icon = ATPIC_MF300_OUT,	.Cnt = CNT_A_WASHER_MF500,	.Init_cb = Ctrl_Front_MF500,		.Cycle_cb = Ctrl_Front_Brush_Standard_Cycle,	.Timer_cb = Ctrl_Front_Brush_Timer_10ms},
	{.RC_Name = RCTEXT_LI_A_ATTACHMENT_ATTACHMENT_MSPN,				.Icon = ATPIC_PLOUGH_NT,	.Cnt = CNT_A_PLOUGH_TN,		.Init_cb = Ctrl_Front_MSPN_A,		.Cycle_cb = Ctrl_Front_Plough_2_Sections_MSPN_Cycle,	.Timer_cb = Ctrl_Front_Brush_Timer_10ms},
	{.RC_Name = RCTEXT_LI_A_ATTACHMENT_ATTACHMENT_FMM600,				.Icon = ATPIC_PLOUGH_NT,	.Cnt = CNT_FMM600,		.Init_cb = Ctrl_Manipulator_FMM600_Init,		.Cycle_cb = Ctrl_Manipulator_Standard_Cycle,	.Timer_cb = Ctrl_Front_Brush_Timer_10ms},
	{.RC_Name = RCTEXT_LI_A_ATTACHMENT_ATTACHMENT_OKB600,				.Icon = ATPIC_NONE,	.Cnt = CNT_OKB600,		.Init_cb = Ctrl_Front_OKB600_Init,		.Cycle_cb = Ctrl_Front_OKB600_Cycle,	.Timer_cb = Ctrl_Front_OKB600_Timer_10ms},
};

tProfileModule Module_B[RCTEXT_CNT_LT_B_ATTACHMENT] = {
	{.RC_Name = RCTEXT_LI_B_ATTACHMENT_ATTACHMENT_NONE,		.Icon = ATPIC_NONE,				.Cnt = CNT_EMPTY,			.Init_cb = NULL,					.Cycle_cb = NULL,								.Timer_cb = NULL},
	{.RC_Name = RCTEXT_LI_B_ATTACHMENT_ATTACHMENT_OMP220,	.Icon = ATPIC_OMP253,			.Cnt = CNT_B_BRUSH_OMP253,	.Init_cb = Ctrl_Inter_OMP220_Init,	.Cycle_cb = Ctrl_Inter_Brush_Standard_Cycle,	.Timer_cb = Ctrl_Inter_Brush_Timer_10ms},
	{.RC_Name = RCTEXT_LI_B_ATTACHMENT_ATTACHMENT_CH2600,	.Icon = ATPIC_PLOUGH_CH2600,	.Cnt = CNT_B_GRADER_CH2600,	.Init_cb = Ctrl_Inter_CH2600_Init,	.Cycle_cb = Ctrl_Inter_Plough_Standard_Cycle,	.Timer_cb = Ctrl_Inter_Plough_Timer_10ms},
	{.RC_Name = RCTEXT_LI_B_ATTACHMENT_ATTACHMENT_OZP231,	.Icon = ATPIC_OMP253,			.Cnt = CNT_B_BRUSH_OZP231,	.Init_cb = Ctrl_Inter_OZP231_Init,	.Cycle_cb = Ctrl_Inter_Brush_Standard_Cycle,	.Timer_cb = Ctrl_Inter_Brush_Timer_10ms},
	{.RC_Name = RCTEXT_LI_B_ATTACHMENT_ATTACHMENT_MSPN,		.Icon = ATPIC_PLOUGH_NT,		.Cnt = CNT_EMPTY,			.Init_cb = Ctrl_Front_MSPN_B,       .Cycle_cb =  NULL,								.Timer_cb = Ctrl_Inter_Plough_Timer_10ms},
	{.RC_Name = RCTEXT_LI_B_ATTACHMENT_ATTACHMENT_CH2800,	.Icon = ATPIC_PLOUGH_CH2600,	.Cnt = CNT_B_GRADER_CH2600,	.Init_cb = Ctrl_Inter_CH2800_Init,	.Cycle_cb = Ctrl_Inter_CH2800_Standard_Cycle,	.Timer_cb = Ctrl_Inter_Plough_Timer_10ms},
	{.RC_Name = RCTEXT_LI_B_ATTACHMENT_ATTACHMENT_OMP220_HUMID,	.Icon = ATPIC_OMP253,			.Cnt = CNT_B_BRUSH_OMP253,	.Init_cb = Ctrl_Inter_OMP220_Humid_Init,	.Cycle_cb = Ctrl_Inter_Brush_Standard_Cycle,	.Timer_cb = Ctrl_Inter_Brush_Timer_10ms},
};

tProfileModule Module_C[RCTEXT_CNT_LT_C_ATTACHMENT] = {
	{.RC_Name = RCTEXT_LI_C_ATTACHMENT_ATTACHMENT_NONE,			.Icon = ATPIC_NONE,			.Cnt = CNT_EMPTY,		.Init_cb = NULL,						.Cycle_cb = NULL,					.Timer_cb = NULL},
	{.RC_Name = RCTEXT_LI_C_ATTACHMENT_ATTACHMENT_AA_AC,		.Icon = ATPIC_PLOUGH_SIDE,	.Cnt = CNT_C_PLOUGH_AA,	.Init_cb = Ctrl_Side_AA_AC_Init,		.Cycle_cb = Ctrl_Side_Plough_Cycle,	.Timer_cb = Ctrl_Side_Plough_Timer_10ms},
	{.RC_Name = RCTEXT_LI_C_ATTACHMENT_ATTACHMENT_BUCHER_AK,	.Icon = ATPIC_PLOUGH_SIDE,	.Cnt = CNT_C_PLOUGH_AK,	.Init_cb = Ctrl_Side_BUCHER_AK_Init,	.Cycle_cb = Ctrl_Side_AK_Cycle,		.Timer_cb = Ctrl_Side_Plough_Timer_10ms},
	{.RC_Name = RCTEXT_LI_C_ATTACHMENT_ATTACHMENT_MSN37,		.Icon = ATPIC_PLOUGH_SIDE,	.Cnt = CNT_C_PLOUGH_AK,	.Init_cb = Ctrl_Side_MSN37_Init,		.Cycle_cb = Ctrl_Side_Plough_Cycle,	.Timer_cb = Ctrl_Side_Plough_Timer_10ms},
	{.RC_Name = RCTEXT_LI_C_ATTACHMENT_ATTACHMENT_SL,		.Icon = ATPIC_PLOUGH_SIDE,	.Cnt = CNT_C_PLOUGH_AK,	.Init_cb = Ctrl_Side_BUCHER_AK_Init,		.Cycle_cb = Ctrl_Side_AK_Cycle,					.Timer_cb = Ctrl_Side_Plough_Timer_10ms},
};

tProfileModule Module_Top[RCTEXT_CNT_LT_TOPATTACHMENT] = {
	{.RC_Name = RCTEXT_LI_TOPATTACHMENT_ATTACHMENT_NONE,					.Icon = ATPIC_NONE,	.Cnt = CNT_EMPTY,			.Init_cb = NULL,							.Cycle_cb = NULL,								.Timer_cb = NULL},
	{.RC_Name = RCTEXT_LI_TOPATTACHMENT_ATTACHMENT_TOP_RTR,					.Icon = ATPIC_NONE,	.Cnt = CNT_TOP_RTR,			.Init_cb = Ctrl_RTR_Init,			.Cycle_cb = Ctrl_RTR_Cycle,				.Timer_cb = Ctrl_RTR_Timer_10ms},
	{.RC_Name = RCTEXT_LI_TOPATTACHMENT_ATTACHMENT_BUCHER_CR,					.Icon = ATPIC_NONE,	.Cnt = CNT_TOP_CK,			.Init_cb = Ctrl_top_LRS_CR_Init,			.Cycle_cb = Ctrl_top_LRS_CR_Cycle,				.Timer_cb = Ctrl_top_LRS_CR_Timer_10ms},
	{.RC_Name = RCTEXT_LI_TOPATTACHMENT_ATTACHMENT_CP600A,						.Icon = ATPIC_NONE,	.Cnt = CNT_TOP_CR_600A,			.Init_cb = Ctrl_top_LRS_CR600_300_Init,			.Cycle_cb = Ctrl_top_CR600_BP300,				.Timer_cb = Ctrl_top_LRS_CR_Timer_10ms},
	{.RC_Name = RCTEXT_LI_TOPATTACHMENT_ATTACHMENT_E2000_BP300,				.Icon = ATPIC_NONE,	.Cnt = CNT_TOP_E2000,		.Init_cb = Ctrl_top_E2000_BP300_Init,		.Cycle_cb = Ctrl_top_E2000_BP300_Cycle,			.Timer_cb = NULL},
	{.RC_Name = RCTEXT_LI_TOPATTACHMENT_ATTACHMENT_E2000_C610H,				.Icon = ATPIC_NONE,	.Cnt = CNT_TOP_E2000,		.Init_cb = Ctrl_top_E2000_C610H_Init,		.Cycle_cb = Ctrl_top_E2000_C610H_Cycle,			.Timer_cb = NULL},
	{.RC_Name = RCTEXT_LI_TOPATTACHMENT_ATTACHMENT_E2000_C610H_C610H,		.Icon = ATPIC_NONE,	.Cnt = CNT_TOP_E2000,		.Init_cb = Ctrl_top_E2000_C610H_C610H_Init,	.Cycle_cb = Ctrl_top_E2000_C610H_C610H_Cycle,	.Timer_cb = NULL},
	{.RC_Name = RCTEXT_LI_TOPATTACHMENT_ATTACHMENT_E2000_BP300_C610H,		.Icon = ATPIC_NONE,	.Cnt = CNT_TOP_E2000,		.Init_cb = Ctrl_top_E2000_BP300_C610H_Init,	.Cycle_cb = Ctrl_top_E2000_BP300_C610H_Cycle,	.Timer_cb = NULL},
	{.RC_Name = RCTEXT_LI_TOPATTACHMENT_ATTACHMENT_E2000_C610H_RR300,		.Icon = ATPIC_NONE,	.Cnt = CNT_TOP_RR300_E2000,	.Init_cb = Ctrl_top_E2000_C610H_RR300_Init,		.Cycle_cb = Ctrl_top_E2000_C610H_RR300_Cycle,	.Timer_cb = NULL},
	{.RC_Name = RCTEXT_LI_TOPATTACHMENT_ATTACHMENT_E2000_C610H_RR400,		.Icon = ATPIC_NONE,	.Cnt = CNT_TOP_RR300_E2000,	.Init_cb = Ctrl_top_E2000_C610H_RR400_Init,		.Cycle_cb = Ctrl_top_E2000_C610H_RR400_Cycle,	.Timer_cb = NULL},
	{.RC_Name = RCTEXT_LI_TOPATTACHMENT_ATTACHMENT_E2000_C610H_BP300_RR400, .Icon = ATPIC_NONE, .Cnt = CNT_TOP_RR300_E2000, .Init_cb = Ctrl_top_E2000_C610H_BP300_RR400_Init,	.Cycle_cb = Ctrl_top_E2000_C610H_BP300_RR400_Cycle, .Timer_cb = NULL},
	{.RC_Name = RCTEXT_LI_TOPATTACHMENT_ATTACHMENT_E2000_C610H_BP300_RR300, .Icon = ATPIC_NONE, .Cnt = CNT_TOP_RR300_E2000, .Init_cb = Ctrl_top_E2000_C610H_BP300_RR300_Init,	.Cycle_cb = Ctrl_top_E2000_C610H_BP300_RR300_Cycle, .Timer_cb = NULL},
	{.RC_Name = RCTEXT_LI_TOPATTACHMENT_ATTACHMENT_EKM_10,			.Icon = ATPIC_NONE,	.Cnt = CNT_TOP_E2000,	.Init_cb = Ctrl_top_EKM10_Init,		.Cycle_cb = Ctrl_top_EKM10_Cycle,					.Timer_cb = NULL},
};

tModules ProfModules[] = {
	{Module_Top,	GS_ARRAYELEMENTS( Module_Top ),	RCTEXT_L_TOPATTACHMENT},
	{Module_A,		GS_ARRAYELEMENTS( Module_A ),	  RCTEXT_L_A_ATTACHMENT},
	{Module_B,		GS_ARRAYELEMENTS( Module_B ),  	RCTEXT_L_B_ATTACHMENT},
	{Module_C,		GS_ARRAYELEMENTS( Module_C ), 	RCTEXT_L_C_ATTACHMENT},
};


tProfile StandardProfiles[] = {
	{.name = "Зима",
	.module = {
		RCTEXT_LI_TOPATTACHMENT_ATTACHMENT_TOP_RTR,
		RCTEXT_LI_A_ATTACHMENT_ATTACHMENT_BUCHERTN34,
		RCTEXT_LI_B_ATTACHMENT_ATTACHMENT_OMP220,
		RCTEXT_LI_C_ATTACHMENT_ATTACHMENT_NONE}
	},
	{.name = "Зима щётка",
	.module = {
		RCTEXT_LI_TOPATTACHMENT_ATTACHMENT_TOP_RTR,
		RCTEXT_LI_A_ATTACHMENT_ATTACHMENT_OFF_OPF_245,
		RCTEXT_LI_B_ATTACHMENT_ATTACHMENT_OMP220,
		RCTEXT_LI_C_ATTACHMENT_ATTACHMENT_NONE}
	},
	{.name = "Лето щётка",
	.module = {
		RCTEXT_LI_TOPATTACHMENT_ATTACHMENT_E2000_BP300,
		RCTEXT_LI_A_ATTACHMENT_ATTACHMENT_OMT1,
		RCTEXT_LI_B_ATTACHMENT_ATTACHMENT_NONE,
		RCTEXT_LI_C_ATTACHMENT_ATTACHMENT_NONE}
	},
	{.name = "Лето мойка",
	.module = {
		RCTEXT_LI_TOPATTACHMENT_ATTACHMENT_E2000_BP300,
		RCTEXT_LI_A_ATTACHMENT_ATTACHMENT_MF300,
		RCTEXT_LI_B_ATTACHMENT_ATTACHMENT_NONE,
		RCTEXT_LI_C_ATTACHMENT_ATTACHMENT_NONE}
	},
};

static tProfile* _cur_profile = NULL;
uint32_t		q_chg_profile	= 0;
void Prof_AddStandardProfiles( void ) {
	for( uint32_t i = 0; i < GS_ARRAYELEMENTS( StandardProfiles ); i++ ) {
		GSList_AddData( &ProfileList, &StandardProfiles[i], sizeof( tProfile ) );
	}
}

tProfileModule* GetCurrentModule( uint32_t idx ) {
	return &ProfModules[idx].p[_cur_profile->module[idx]];
}

void SetRTR_CntUH(void) {
	Module_Top[1].Cnt = CNT_TOP_UH;
}

void SetTop_CntEmpty(void) {
	ProfModules[0].p[_cur_profile->module[0]].Cnt = CNT_EMPTY;
}

const char* Prof_GetModuleName( uint32_t ModuleIdx, uint32_t idx, uint32_t language ) {
	if( ModuleIdx >= GS_ARRAYELEMENTS( ProfModules ) ) {
		return "Prof_GetModuleName: Invalid Module Index\r\n";
	}
	if( idx >= ProfModules[ModuleIdx].p_num ) {
		return "Prof_GetModuleName: Invalid Listindex in Module \r\n";
	}
	return RCTextGetListElement( ProfModules[ModuleIdx].RCtListOffset, ProfModules[ModuleIdx].p[idx].RC_Name, language );;
}

void Prof_Write( tProfile* Profile, tGsFile** fp ) {
	FilePrintf( *fp, "PROFNAME=\"%s\"\r\n", Profile->name );
	for( uint32_t i = 0; i < GS_ARRAYELEMENTS( Profile->module ); i++ ) {
		FilePrintf( *fp, "MODULE%u=\"%s\"\r\n", i + 1, Prof_GetModuleName( i, Profile->module[i], 0 ) );
	}
}

int Profile_GetModuleN(tProfileModule * moduleABC, int moduleSize, int RCTEXT) {
	for(int i = 0; i < moduleSize; i++) {
		if(moduleABC[i].RC_Name == RCTEXT) {
			return i;
		}
	}
	return -1;
}

typedef struct tagRTR_Config {
	char * oldName;
	int type;
	int asym;
	int humid;
}tRTR_Config;

tRTR_Config RTR_TransOldProf[] = {
	{.oldName = "KH_3000_4000", 					.type = 1, .asym = 0, .humid = 1},
	{.oldName = "SH_3000_4000", 					.type = 2, .asym = 1, .humid = 1},
	{.oldName = "KH_5000", 								.type = 1, .asym = 0, .humid = 1},
	{.oldName = "SH_5000", 								.type = 2, .asym = 1, .humid = 1},
	{.oldName = "UH_3000_4000", 					.type = 4, .asym = 0, .humid = 0},
	{.oldName = "UH_5000", 								.type = 5, .asym = 0, .humid = 0},
	{.oldName = "Соргаз", 								.type = 3, .asym = 2, .humid = 2},
	{.oldName = "Соргаз без увлажнения",	.type = 3, .asym = 2, .humid = 0},
};

void Profile_BackupLoad_CheckOldNames(tProfile * ActProfile, eProfileType idx, char * ptr) {
	switch(idx) {
		case P_TYPE_TOP: {
			// Old RTR check
			int rtr = Profile_GetModuleN(Module_Top, GS_ARRAYELEMENTS(Module_Top), RCTEXT_LI_TOPATTACHMENT_ATTACHMENT_TOP_RTR);
			if (rtr != -1) {
				for (int i = 0; i < GS_ARRAYELEMENTS(RTR_TransOldProf); i++) {
					if (strcmp(ptr, RTR_TransOldProf[i].oldName) == 0) {
						tParamData * param = GetParameter();

						ActProfile -> module[idx] = rtr;
						param -> RTR.TypeRTR = RTR_TransOldProf[i].type;
						param -> RTR.AsymType = RTR_TransOldProf[i].asym;
						param -> RTR.HumidType = RTR_TransOldProf[i].humid;
						break;
					}
				}
			}
			break;
		}
		case P_TYPE_A: {
			int mf300 = Profile_GetModuleN(Module_A, GS_ARRAYELEMENTS(Module_A), RCTEXT_LI_A_ATTACHMENT_ATTACHMENT_MF300);
			if (mf300 != -1) {
				if ((strcmp(ptr, "ФМО 300") == 0)) {
					ActProfile -> module[idx] = mf300;
				}
			}
			break;
		}
		case P_TYPE_B: {
			int omp = Profile_GetModuleN(Module_B, GS_ARRAYELEMENTS(Module_B), RCTEXT_LI_B_ATTACHMENT_ATTACHMENT_OMP220);
			if (omp != -1) {
				if ((strcmp(ptr, "ОМП 253") == 0) || (strcmp(ptr, "МПО 285") == 0) || (strcmp(ptr, "ОМП 220") == 0)) {
					ActProfile -> module[idx] = omp;
				}
			}
			break;
		}
		case P_TYPE_C: {

			break;
		}
	}
}

int32_t Prof_Load(void) {
  tGsFile * fp = FileOpen(PROFILE_PATH, "r");
  if (fp == NULL) {
    return -1;
  }
  tProfile * ActProfile = NULL;
  char buffer[128];
  // Destroy existing Profile List
  GSList_Destroy( & ProfileList);

  while (FileGets(buffer, sizeof(buffer), fp)) {
    // check for new Profile
    if (strstr(buffer, "PROFNAME")) {
      tProfile Profile;
      memset( & Profile, 0, sizeof(tProfile));
      char * ptr;
      char delimeter[] = "=\r\n\"";
      // Get profile name
      ptr = strtok(buffer, delimeter);
      ptr = strtok(NULL, delimeter);
      strncpy(Profile.name, ptr, sizeof(Profile.name));
      // Save Profile in the List
      ActProfile = GSList_AddData( & ProfileList, & Profile, sizeof(Profile));
    }
    // Check for Module Entries
    else if (strstr(buffer, "MODULE")) {
      // Check Number of Module
      if (NULL != ActProfile) {
        eProfileType idx;
        if (strstr(buffer, "MODULE1")) {
          idx = P_TYPE_TOP;
        } else if (strstr(buffer, "MODULE2")) {
          idx = P_TYPE_A;
        } else if (strstr(buffer, "MODULE3")) {
          idx = P_TYPE_B;
        } else if (strstr(buffer, "MODULE4")) {
          idx = P_TYPE_C;
        } else return -1;

        char * ptr;
        char delimeter[] = "=\r\n\"";
        // Get module name
        ptr = strtok(buffer, delimeter);
        ptr = strtok(NULL, delimeter);
        if (NULL == ptr) {}

        uint32_t i = 0;
        for (; i < ProfModules[idx].p_num; i++) {
          if (0 == strcmp(ptr, Prof_GetModuleName(idx, i, 0))) {
            ActProfile -> module[idx] = i;
            break;
          }
        }

        // if name not found
        if (i == ProfModules[idx].p_num) {
					Profile_BackupLoad_CheckOldNames(ActProfile, idx, ptr);
        }
      }
    }
  }

  FileClose(fp);
  // rewrite local profile file
  Prof_Save();
  writeParameters();
  return 0;
}

int32_t Prof_Save( void ) {
	tGsFile* fp = FileOpen( PROFILE_PATH, "w" );
	if( fp == NULL ) {
		return -1;
	}
	tProfile* ActProf = GSList_GetFirstData( &ProfileList );
	while( ActProf != NULL ) {
		Prof_Write( ActProf, &fp );
		ActProf = GSList_GetNextData( &ProfileList );
	}

	FileClose( fp );
	int32_t ProfIdx = Profile_GetIndex( Profile_GetCurrentProfile() );
	if( -1 < ProfIdx )
		SetVarIndexed( IDX_CURRENTPROFILE, ProfIdx ); //saves the idx of the current profile as remanent variable

	return 0;
}

int32_t Prof_CopyToUsb( void ) {
	return CopyFile( PROFILE_PATH, PROFILE_PATHUSB, NULL );
}

int32_t Prof_LoadFromUsb( void ) {
	return CopyFile( PROFILE_PATHUSB, PROFILE_PATH, NULL );
}

/**	@brief Initalises the profiles. Loads existing Profiles from the flash
**	or generates standard profiles, if no profiles exist, jet.
**	This will also load find the profile, which had been active the last time and
**	will select it as current
**/
void Prof_Init( void ) {
  PinOut_Change_Valv();
	PinOut_Read();
	PinOut_Front_Init();
	PinOut_Inter_Init();
	PinOut_Side_Init();

//	db_out( "Start init profile" );
	GSList_Init( &ProfileList );
//	db_out_time( "ProfileList initialized\r\n" );
	if( -1 == Prof_Load() ) {
//		db_out_time( "Load Standard Profile List\r\n" );
		Prof_AddStandardProfiles();
		Prof_Save();
	}
	//Get current Profile
	_cur_profile = GSList_GetFirstData( &ProfileList );
	for( int32_t i = 0; i < GetVarIndexed( IDX_CURRENTPROFILE ); i++ ) {
		_cur_profile = GSList_GetNextData( &ProfileList );
	}
	if( _cur_profile == NULL ) {
		_cur_profile = GSList_GetFirstData( &ProfileList );
	}
}
/**	@brief This Functions draws the container, showing the actual movement of the attachments.
**	In the center of the container the actual selected equipment is shown (depending on the pressed Joystick Button A, B or C)
**	Now it is checked, which valves are in use for this attachment. If a Valve is used, the arrows to this valve function will be shown.
**
**	@example:
**	A snow Plough is used. Its picture will be shown in the middle, if Key A is pressed. (A for front Attachment)
**	The Plough can move up and down. So there is a Valve for the movment UpDown. If no movmemnt the arrows for up and down are shown in white.
**	If the Plough moves up, the Arrow for up will become green.
**/
extern tManipulatorTM		ManipulatorTM;
void Prof_DrawMove(void) {
  eAttachmentPicIdx Pic = 0;
  int32_t updown = INACTIVE;
  int32_t leftright = INACTIVE;
  int32_t extract = INACTIVE;
  int32_t steelblade = INACTIVE;
  int32_t tilt = INACTIVE;
  int32_t work = INACTIVE;
  int32_t sideupdown = INACTIVE;
  const tControl * Ctrl = CtrlGet();
  //Check updown
  tProfileModule * Module;

  int32_t Attachment = EQUIP_A;
  if (1 == Ctrl -> pressed_ABC && ManipulatorTM.Set == 0) //only A XOR B XOR C
  {
    for (int32_t i = 0; i <= EQUIP_C; i++) {
      //check if Button A,B or C is down
      Attachment = i;
      if (IOT_Button_IsDown( & Ctrl -> joystick.Button[i])) {
        //check for movement depending on the connected valve.
        if ((Module = GetCurrentModule(i + 1))) {
          Pic = Module -> Icon;
          if (!Pic) continue;

          if (Ctrl -> ABC[i].ABC_Attach.ValveUpDown.Type != VALVE_TYPE_NONE) {
            updown = Ctrl -> cmd_ABC[i].Updown;
          }
					if (Ctrl -> ABC[i].BrushValve.VUpDown.Type != VALVE_TYPE_NONE) {
            updown = Ctrl -> cmd_ABC[i].Updown;
          }

					if (Ctrl -> ABC[i].ABC_Attach.ValveLeftRight.Type != VALVE_TYPE_NONE) {
            leftright = Ctrl -> cmd_ABC[i].LeftRight;
          }
					if (Ctrl -> ABC[i].BrushValve.VLeftRight.Type != VALVE_TYPE_NONE) {
            leftright = Ctrl -> cmd_ABC[i].LeftRight;
          }

          if (Ctrl -> ABC[i].ABC_Attach.ValveExRetract.Type != VALVE_TYPE_NONE) {
            extract = Ctrl -> cmd_ABC[i].Z;
          }
					if (Ctrl -> ABC[i].BrushValve.VOptional.Type != VALVE_TYPE_NONE) {
            extract = Ctrl -> cmd_ABC[i].Z;
          }

					if (Ctrl -> ABC[i].ABC_Attach.ValveSteelUpDown.Type != VALVE_TYPE_NONE) {
            steelblade = Ctrl -> cmd_ABC[i].Z;
            if (steelblade > 0)
              Pic = ATPIC_STEELBLADE_DOWN;
            else if (steelblade < 0)
              Pic = ATPIC_STEELBLADE_UP;
          }
          if (Ctrl -> ABC[i].ABC_Attach.ValveTilt.Type != VALVE_TYPE_NONE) {
            tilt = Ctrl -> cmd_ABC[i].Z;
          }
          if (Ctrl -> ABC[i].ABC_Attach.ValveWorkTransport.Type != VALVE_TYPE_NONE) {
            work = Ctrl -> cmd_ABC[i].LeftRight;
          }

          if (Ctrl -> ABC[i].BrushValve.VSideSec.Type != VALVE_TYPE_NONE) {
            sideupdown = Ctrl -> cmd_ABC[i].Z;
            if (Pic == ATPIC_MF300_OUT) {
              if (sideupdown < 0) {
                Pic = ATPIC_MF300_IN;
              }
            }
          }


        }
      }
      if (Pic) {
        break;
      }
    }
  }
  //draw the arrows, if a picture is shown.
  if (Pic) {
    if (!IsInfoContainerOn(CNT_ATTACHMOVE)) {
      InfoContainerOn(CNT_ATTACHMOVE);
    }
    //____________________
    // Draw Arrows for possible, and active functions into the container.
    //Up / Down
    if (updown == INACTIVE) SetVarIndexed(IDX_ATTACHMENT_UP, ARROW_INVISIBLE);
    else if (updown > 0) SetVarIndexed(IDX_ATTACHMENT_UP, ARROW_GREEN_0);
    else SetVarIndexed(IDX_ATTACHMENT_UP, ARROW_WHITE);
    if (updown == INACTIVE) SetVarIndexed(IDX_ATTACHMENT_DOWN, ARROW_INVISIBLE);
    else if (updown < 0) SetVarIndexed(IDX_ATTACHMENT_DOWN, ARROW_GREEN_0);
    else SetVarIndexed(IDX_ATTACHMENT_DOWN, ARROW_WHITE);
    //Left /Right
    if (leftright == INACTIVE) SetVarIndexed(IDX_ATTACHMENT_RIGHT, ARROW_INVISIBLE);
    else if (leftright > 0) SetVarIndexed(IDX_ATTACHMENT_RIGHT, ARROW_GREEN_0);
    else SetVarIndexed(IDX_ATTACHMENT_RIGHT, ARROW_WHITE);
    if (leftright == INACTIVE) SetVarIndexed(IDX_ATTACHMENT_LEFT, ARROW_INVISIBLE);
    else if (leftright < 0) SetVarIndexed(IDX_ATTACHMENT_LEFT, ARROW_GREEN_0);
    else SetVarIndexed(IDX_ATTACHMENT_LEFT, ARROW_WHITE);
    //Extract /Retract
    if (steelblade == INACTIVE) SetVarIndexed(IDX_ATTACHMENT_STEELUPDOWN, ARROW_INVISIBLE);
    else if (steelblade > 0) SetVarIndexed(IDX_ATTACHMENT_STEELUPDOWN, ARROW_GREEN_0);
    else if (steelblade < 0) SetVarIndexed(IDX_ATTACHMENT_STEELUPDOWN, ARROW_GREEN_1);
    else SetVarIndexed(IDX_ATTACHMENT_STEELUPDOWN, ARROW_WHITE);
    // Tilt
    if (tilt == INACTIVE) SetVarIndexed(IDX_ATTACHMENT_SIDETILT, ARROW_INVISIBLE);
    else if (tilt > 0) SetVarIndexed(IDX_ATTACHMENT_SIDETILT, ARROW_GREEN_0);
    else if (tilt < 0) SetVarIndexed(IDX_ATTACHMENT_SIDETILT, ARROW_GREEN_1);
    else SetVarIndexed(IDX_ATTACHMENT_SIDETILT, ARROW_WHITE);
    //Workposition
    if (work == INACTIVE) SetVarIndexed(IDX_ATTACHMENT_WORK, ARROW_INVISIBLE);
    else if (work > 0) SetVarIndexed(IDX_ATTACHMENT_WORK, ARROW_GREEN_0);
    else if (work < 0) SetVarIndexed(IDX_ATTACHMENT_WORK, ARROW_GREEN_1);
    else SetVarIndexed(IDX_ATTACHMENT_WORK, ARROW_WHITE);
    //Steelblade
    if (extract == INACTIVE) SetVarIndexed(IDX_ATTACHMENT_EXTRACT, ARROW_INVISIBLE);
    else if (extract == 0) SetVarIndexed(IDX_ATTACHMENT_EXTRACT, ARROW_WHITE);
    else if (Attachment == EQUIP_A) {
      if (extract > 0) SetVarIndexed(IDX_ATTACHMENT_EXTRACT, ARROW_GREEN_0);
      else if (extract < 0) SetVarIndexed(IDX_ATTACHMENT_EXTRACT, ARROW_GREEN_1);
    } else if (Attachment == EQUIP_C) {
      if (extract > 0) SetVarIndexed(IDX_ATTACHMENT_EXTRACT, ARROW_GREEN_1);
      else if (extract < 0) SetVarIndexed(IDX_ATTACHMENT_EXTRACT, ARROW_GREEN_0);
    } else SetVarIndexed(IDX_ATTACHMENT_EXTRACT, ARROW_WHITE);

    if (sideupdown == INACTIVE) SetVarIndexed(IDX_ATTACHMENT_SIDEUPDOWN, ARROW_INVISIBLE);
    else if (sideupdown < 0) SetVarIndexed(IDX_ATTACHMENT_SIDEUPDOWN, ARROW_GREEN_1);
    else if (sideupdown > 0) SetVarIndexed(IDX_ATTACHMENT_SIDEUPDOWN, ARROW_GREEN_0);
    else SetVarIndexed(IDX_ATTACHMENT_SIDEUPDOWN, ARROW_WHITE);
    SetVarIndexed(IDX_ATTACHPIC, Pic);
  } else //switch off container, if no key is pressed.
  {
    if (IsInfoContainerOn(CNT_ATTACHMOVE)) {
      InfoContainerOff(CNT_ATTACHMOVE);
    }
  }
}

void Prof_Draw( void ) {
	//SetIcons
	for( int32_t i = 0; i < MODULES_NUM; i++ ) {
		tProfileModule* ThisModule = &ProfModules[i].p[_cur_profile->module[i]];
		if( ThisModule != NULL && !ManipulatorTM.Set) {
			if( CNT_EMPTY != ThisModule->Cnt ) {
				if( !IsInfoContainerOn( ThisModule->Cnt ) ) {
					InfoContainerOn( ThisModule->Cnt );
				}
			}
		}
	}
  if(ManipulatorTM.Set){
        if( !IsInfoContainerOn( ManipulatorTM.Set ) ) {
					InfoContainerOn( ManipulatorTM.Set );
				}
  }
}

void ProfHide( void ) {
	//Close Container
	for( int32_t i = 0; i < MODULES_NUM; i++ ) {
		tProfileModule* ThisModule = &ProfModules[i].p[_cur_profile->module[i]];
		InfoContainerOff( ThisModule->Cnt );
	}
}

/**	@brief This function will generate a default name out of a configuration.
**	@param prof
*/
void Prof_GenerateName( tProfile* prof ) {
	memset( prof->name, 0, sizeof( prof->name ) );
	for( uint32_t i = 0; i < GS_ARRAYELEMENTS( ProfModules ); i++ ) {
		uint32_t idx = prof->module[i];
		if( 0 < idx ) {
			if( idx < ProfModules[i].p_num ) {
				const char* name = RCTextGetListElement( ProfModules[i].RCtListOffset, ProfModules[i].p[idx].RC_Name, GetVarIndexed( IDX_SYS_LANGUAGE ) );
				strncat( prof->name, name, sizeof( prof->name ) - strlen( prof->name ) );
			}
		}
	}
}

tProfile* Profile_GetCurrentProfile( void ) {
	return _cur_profile;
}

void Prof_SetCurrentProfile( tProfile* prof_ ) {
	_cur_profile = prof_;
}

tGSList* Profile_GetProfileList( void ) {
	return &ProfileList;
}

char* Profile_GetName( uint8_t idx ) {
	uint32_t		list = 0;

	switch( idx ) {
	case 1:
		list = RCTEXT_L_A_ATTACHMENT;
		break;
	case 2:
		list = RCTEXT_L_B_ATTACHMENT;
		break;
	case 3:
		list = RCTEXT_L_C_ATTACHMENT;
		break;

	case 0:
	default:
		list = RCTEXT_L_TOPATTACHMENT;
		break;
	}

	const char*		EquipName = RCTextGetListElement( list, _cur_profile->module[idx], GetVarIndexed(IDX_SYS_LANGUAGE) );
	return EquipName;
}

void Prof_Cycle( tControl* Ctrl, uint32_t evtc, tUserCEvt* evtv ) {
	//Call init Functions of Profiles, if the profile changed.
	static tProfile* LastProfile = NULL;
	if( LastProfile != _cur_profile ) {
		LastProfile = _cur_profile;
		if( NULL != _cur_profile ) {
			for( uint32_t i = 0; i < GS_ARRAYELEMENTS( _cur_profile->module ); i++ ) {
				if( ProfModules[i].p[_cur_profile->module[i]].Init_cb ) {
					ProfModules[i].p[_cur_profile->module[i]].Init_cb( Ctrl );
				}
			}
		}
	}

	//Call Cycle functions of the different moduels of the current modules.
	if( NULL != _cur_profile ) {
		for( uint32_t i = 0; i < GS_ARRAYELEMENTS( _cur_profile->module ); i++ ) {
			if( ProfModules[i].p[_cur_profile->module[i]].Cycle_cb ) {
				ProfModules[i].p[_cur_profile->module[i]].Cycle_cb( Ctrl, evtc, evtv );
			}
		}
	}
}

void Prof_Timer_10ms( tControl* Ctrl ) {
	//Call Timer Functions of Modules
	if( NULL != _cur_profile ) {
		for( uint32_t i = 0; i < GS_ARRAYELEMENTS( _cur_profile->module ); i++ ) {
			if( ProfModules[i].p[_cur_profile->module[i]].Timer_cb ) {
				ProfModules[i].p[_cur_profile->module[i]].Timer_cb( Ctrl );
			}
		}
	}
}

/**	@brief Gets idx of the current profile
**	@param ProfSearch, Pointer of the profile to be searched for.
**/
int32_t Profile_GetIndex( tProfile* ProfSearch ) {
	tGSList* ProfList = Profile_GetProfileList();
	if( ProfSearch == GSList_GetFirstData( ProfList ) )
		return 0;
	for( uint32_t i = 1; i < ProfList->numItems; i++ ) {
		if( ProfSearch == GSList_GetNextData( ProfList ) ) {
			return i;
		}
	}
	return -1;
}
