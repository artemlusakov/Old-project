/****************************************************************************
*
* File:         CTRL_LIGHT.c
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
#include <UserCEvents.h> /* Events send to the Cycle function              */
#include <UserCAPI.h>    /* API-Function declarations                      */

#include "vartab.h" /* Variable Table definition                      */
/* include this file in every C-Source to access  */
/* the variable table of your project             */
/* vartab.h is generated automatically            */
#include "objtab.h" /* Object ID definition                           */
					/* include this file in every C-Source to access  */
					/* the object ID's of the visual objects          */
					/* objtab.h is generated automatically            */
#include "control.h"
#include "ctrl_brush.h"
#include "profile.h"
#include "RCText.h"
#include "ctrl_top_summer_attachment.h"

/****************************************************************************/

/* macro definitions ********************************************************/

/* type definitions *********************************************************/

/* prototypes ***************************************************************/

/* global constants *********************************************************/

/* global variables *********************************************************/

/* local function prototypes*************************************************/

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
/***********************************
* Lights
* *********************************/
uint8_t Prev_alarm = 0;
extern uint8_t flagOKB600_Init;

// Not used
void Ctrl_Light_Worklight_AutoOn(tLight * Light) {
  const tControl * Ctrl = CtrlGet();
  static int32_t AutoWorklight_OKB600 = 0;
  if(flagOKB600_Init) {
    if(Brush_Active( & Ctrl -> ABC[EQUIP_A].ABC_Attach)) {
      if(!AutoWorklight_OKB600) {
        Light -> On.OKB600_WorkRear = 1;
        AutoWorklight_OKB600 = 1;
      }
    } else {
      AutoWorklight_OKB600 = 0;
    }
  }
}

int32_t LightAlarm_Off = 0;
void Ctrl_Light_Alarm_On(tLight * Light) {
  if (!LightAlarm_Off) {
    Light -> On.FrontBeaconCab = 1;
    Light -> On.RTR_RearBeacon = 1;
    Light -> On.LRS_RearBeacon = 1;
    LightAlarm_Off = 1; //prevent from switching on again, automatically.
  }
}

void Ctrl_Light_AutoOn(tLight * Light) {
  const tControl * Ctrl = CtrlGet();
  const tProfile * Prof = Profile_GetCurrentProfile();
  int32_t AlarmOn = 0;

  static uint8_t InterLightAutoOn = 0;
  uint8_t InterLightOn = 0;
  // Подсветка межосевого оборудования
  if (GetVarIndexed(IDX_CHECK_INTERLIGHT)) {
    if (Ctrl -> cmd_ABC[EQUIP_B].Float) {
      if (Ctrl -> sens.DI.Ignition) {
        if(!InterLightAutoOn) {
          Light -> On.BrushLight = 1;
          InterLightAutoOn = 1;
        }
        InterLightOn = 1;
      }
    }
  }
  if(InterLightOn == 0) {
    InterLightAutoOn = 0;
  }


  static uint8_t SideLightAutoOn = 0;
  uint8_t SideLightOn = 0;
  // Подсветка бокового отвала
  if (GetVarIndexed(IDX_CHECK_SIDELIGHT)) {
    if (Ctrl -> cmd_ABC[EQUIP_C].LeftRight != 0 || Ctrl -> cmd_ABC[EQUIP_C].Float == FLOAT_ON) {
      if (Ctrl -> sens.DI.Ignition) {
        if(!SideLightAutoOn) {
          Light -> On.WorkSide = 1;
          Ctrl_Light_Alarm_On(Light);
          AlarmOn = 1;
          SideLightAutoOn = 1;
        }
        SideLightOn = 1;
      }
    }
  }
  if(SideLightOn == 0) {
    SideLightAutoOn = 0;
  }


  static uint8_t RTRLightAutoOn = 0;
  uint8_t RTRLightOn = 0;
  // Подсветка ПС
  if (Ctrl -> top.WinterTop.SpreadConv.on) {
    if (Ctrl -> sens.DI.Ignition) {
      if(!RTRLightAutoOn) {
        Light -> On.RTR_WorkRear = 1;
        RTRLightAutoOn = 1;
      }
      RTRLightOn = 1;
    }
  }
  if(RTRLightOn == 0) {
    RTRLightAutoOn = 0;
  }


  static uint8_t CRLightAutoOn = 0;
  uint8_t CRLightOn = 0;
  // Подсветка рабочей зоны CR
  if (Prof -> module[0] == RCTEXT_LI_TOPATTACHMENT_ATTACHMENT_BUCHER_CR) {
    if (WaterPump_IsOn( & Ctrl -> top.SummerTop.Pump) > 0) {
      if (Ctrl -> sens.DI.Ignition) {
        if(!CRLightAutoOn) {
          Light -> On.LRS_WorkRear = 1;
          Light -> On.RTR_WorkRear = 1;
          CRLightAutoOn = 1;
        }
        CRLightOn = 1;
      }
    }
    if(CRLightOn == 0) {
      CRLightAutoOn = 0;

    }
  }

  // Включение маяков
  for (int32_t i = 0; i < 3; i++) {
    if (Prof -> module[i + 1] != 0) {
      if (Ctrl -> cmd_ABC[i].Float) {
        Ctrl_Light_Alarm_On(Light);
        AlarmOn = 1;
        return;
      }
    }
  }
  if (WaterPump_IsOn( & Ctrl -> top.SummerTop.Pump)) {
    AlarmOn = 1;
    Ctrl_Light_Alarm_On(Light);
    return;
  } else if (MF500Bar_IsOn(&Ctrl->top.SummerTop.MF500Bar)) {
    AlarmOn = 1;
    Ctrl_Light_Alarm_On(Light);
  } else if (Ctrl -> top.WinterTop.SpreadConv.on) {
    AlarmOn = 1;
    Ctrl_Light_Alarm_On(Light);
  }
  if (0 == AlarmOn) {
    LightAlarm_Off = 0;
  }
}


void Ctrl_Light(tLight * Light, uint32_t evtc, tUserCEvt * evtv) {
  const tControl * Ctrl = CtrlGet();
  //Toggle Lights
  if (IsMaskOn(MSK_MAIN) != 1) {
    MsgContainerOff(CNT_LIGHTKEYS);
    MsgContainerOff(CNT_LIGHTKEYS_2);
  } else {
    if (IsMsgContainerOn(CNT_LIGHTKEYS)) {
      if (IsKeyPressedNew(1)) {
        Light -> On.RTR_RearBeacon = 1 - Light -> On.RTR_RearBeacon;
        Light -> On.LRS_RearBeacon = 1 - Light -> On.LRS_RearBeacon;
        Light -> On.FrontBeaconCab = 1 - Light -> On.FrontBeaconCab;
      }
      if (Ctrl -> sens.DI.Ignition) {
        if (IsKeyPressedNew(2)) {
          Light -> On.LRS_WorkRear = 1 - Light -> On.LRS_WorkRear;
          Light -> On.RTR_WorkRear = 1 - Light -> On.RTR_WorkRear;
        }
      }
      if ((GetVarIndexed(IDX_ROADSIGN_LEFT) != 0) || GetVarIndexed(IDX_ROADSIGN_RIGHT) != 0) {
        if (IsKeyPressedNew(3) && GetVarIndexed(IDX_ROADSIGN_LEFT) != 0) {
          Light -> On.RTR_LeftRoadSign = 1 - Light -> On.RTR_LeftRoadSign;
          Light -> On.LRS_LeftRoadSign = 1 - Light -> On.LRS_LeftRoadSign;
        }

        if (IsKeyPressedNew(4) && GetVarIndexed(IDX_ROADSIGN_RIGHT) != 0) {
          Light -> On.RTR_RightRoadSign = 1 - Light -> On.RTR_RightRoadSign;
          Light -> On.LRS_RightRoadSign = 1 - Light -> On.LRS_RightRoadSign;
        }
      } else {
        if (GetVarIndexed(IDX_CHECK_SIDELIGHT) && GetVarIndexed(IDX_CHECK_INTERLIGHT) && (GetVarIndexed(IDX_CHECK_HOPPERLIGHT) || flagOKB600_Init)) {
          if (Ctrl -> sens.DI.Ignition) {
            if (IsKeyPressedNew(3)) {
              Light -> On.WorkSide = 1 - Light -> On.WorkSide;
            }
            if (IsKeyPressedNew(4)) {
              Light -> On.BrushLight = 1 - Light -> On.BrushLight;
            }
          }
          if (GetVarIndexed(IDX_CHECK_HOPPERLIGHT)) {
            if (IsKeyPressedNew(5)) {
              Light -> On.RTR_HopperTop = 1 - Light -> On.RTR_HopperTop;
            }
          }
          if (flagOKB600_Init) {
            if (IsKeyPressedNew(5)) {
              Light -> On.OKB600_WorkRear = 1 - Light -> On.OKB600_WorkRear;
            }
          }
        }
        if (!GetVarIndexed(IDX_CHECK_SIDELIGHT) && GetVarIndexed(IDX_CHECK_INTERLIGHT) && (GetVarIndexed(IDX_CHECK_HOPPERLIGHT) || flagOKB600_Init)) {
          if (Ctrl -> sens.DI.Ignition) {
            if (IsKeyPressedNew(3)) {
              Light -> On.BrushLight = 1 - Light -> On.BrushLight;
            }
          }
          if (GetVarIndexed(IDX_CHECK_HOPPERLIGHT)) {
            if (IsKeyPressedNew(4)) {
              Light -> On.RTR_HopperTop = 1 - Light -> On.RTR_HopperTop;
            }
          }
          if (flagOKB600_Init) {
            if (IsKeyPressedNew(4)) {
              Light -> On.OKB600_WorkRear = 1 - Light -> On.OKB600_WorkRear;
            }
          }
        }
        if (!GetVarIndexed(IDX_CHECK_SIDELIGHT) && GetVarIndexed(IDX_CHECK_INTERLIGHT) && !GetVarIndexed(IDX_CHECK_HOPPERLIGHT)) {
          if (Ctrl -> sens.DI.Ignition) {
            if (IsKeyPressedNew(3)) {
              Light -> On.BrushLight = 1 - Light -> On.BrushLight;
            }
          }
        }
        if (!GetVarIndexed(IDX_CHECK_SIDELIGHT) && !GetVarIndexed(IDX_CHECK_INTERLIGHT) && (GetVarIndexed(IDX_CHECK_HOPPERLIGHT) || flagOKB600_Init)) {
          if (GetVarIndexed(IDX_CHECK_HOPPERLIGHT)) {
            if (IsKeyPressedNew(3)) {
              Light -> On.RTR_HopperTop = 1 - Light -> On.RTR_HopperTop;
            }
          }
          if (flagOKB600_Init) {
            if (IsKeyPressedNew(3)) {
              Light -> On.OKB600_WorkRear = 1 - Light -> On.OKB600_WorkRear;
            }
          }
        }
        if (GetVarIndexed(IDX_CHECK_SIDELIGHT) && GetVarIndexed(IDX_CHECK_INTERLIGHT) && !GetVarIndexed(IDX_CHECK_HOPPERLIGHT)) {
          if (Ctrl -> sens.DI.Ignition) {
            if (IsKeyPressedNew(3)) {
              Light -> On.WorkSide = 1 - Light -> On.WorkSide;
            }
            if (IsKeyPressedNew(4)) {
              Light -> On.BrushLight = 1 - Light -> On.BrushLight;
            }
          }
        }
        if (GetVarIndexed(IDX_CHECK_SIDELIGHT) && !GetVarIndexed(IDX_CHECK_INTERLIGHT) && (GetVarIndexed(IDX_CHECK_HOPPERLIGHT) || flagOKB600_Init)) {
          if (Ctrl -> sens.DI.Ignition) {
            if (IsKeyPressedNew(3)) {
              Light -> On.WorkSide = 1 - Light -> On.WorkSide;
            }
          }
          if (GetVarIndexed(IDX_CHECK_HOPPERLIGHT)) {
            if (IsKeyPressedNew(4)) {
              Light -> On.RTR_HopperTop = 1 - Light -> On.RTR_HopperTop;
            }
          }
          if (flagOKB600_Init) {
            if (IsKeyPressedNew(4)) {
              Light -> On.OKB600_WorkRear = 1 - Light -> On.OKB600_WorkRear;
            }
          }
        }
        if (GetVarIndexed(IDX_CHECK_SIDELIGHT) && !GetVarIndexed(IDX_CHECK_INTERLIGHT) && !GetVarIndexed(IDX_CHECK_HOPPERLIGHT)) {
          if (Ctrl -> sens.DI.Ignition) {
            if (IsKeyPressedNew(3)) {
              Light -> On.WorkSide = 1 - Light -> On.WorkSide;
            }
          }
        }
      }
    } else if (IsMsgContainerOn(CNT_LIGHTKEYS_2)) {
      if (GetVarIndexed(IDX_CHECK_SIDELIGHT) && GetVarIndexed(IDX_CHECK_INTERLIGHT) && (GetVarIndexed(IDX_CHECK_HOPPERLIGHT) || flagOKB600_Init)) {
        if (Ctrl -> sens.DI.Ignition) {
          if (IsKeyPressedNew(1)) {
            Light -> On.WorkSide = 1 - Light -> On.WorkSide;
          }
          if (IsKeyPressedNew(2)) {
            Light -> On.BrushLight = 1 - Light -> On.BrushLight;
          }
        }
        if (GetVarIndexed(IDX_CHECK_HOPPERLIGHT)) {
          if (IsKeyPressedNew(3)) {
            Light -> On.RTR_HopperTop = 1 - Light -> On.RTR_HopperTop;
          }
        }
        if (flagOKB600_Init) {
          if (IsKeyPressedNew(3)) {
            Light -> On.OKB600_WorkRear = 1 - Light -> On.OKB600_WorkRear;
          }
        }
      }
      if (!GetVarIndexed(IDX_CHECK_SIDELIGHT) && GetVarIndexed(IDX_CHECK_INTERLIGHT) && (GetVarIndexed(IDX_CHECK_HOPPERLIGHT) || flagOKB600_Init)) {
        if (Ctrl -> sens.DI.Ignition) {
          if (IsKeyPressedNew(1)) {
            Light -> On.BrushLight = 1 - Light -> On.BrushLight;
          }
        }
        if (GetVarIndexed(IDX_CHECK_HOPPERLIGHT)) {
          if (IsKeyPressedNew(2)) {
            Light -> On.RTR_HopperTop = 1 - Light -> On.RTR_HopperTop;
          }
        }
        if (flagOKB600_Init) {
          if (IsKeyPressedNew(2)) {
            Light -> On.OKB600_WorkRear = 1 - Light -> On.OKB600_WorkRear;
          }
        }
      }
      if (!GetVarIndexed(IDX_CHECK_SIDELIGHT) && GetVarIndexed(IDX_CHECK_INTERLIGHT) && !GetVarIndexed(IDX_CHECK_HOPPERLIGHT)) {
        if (Ctrl -> sens.DI.Ignition) {
          if (IsKeyPressedNew(1)) {
            Light -> On.BrushLight = 1 - Light -> On.BrushLight;
          }
        }
      }
      if (!GetVarIndexed(IDX_CHECK_SIDELIGHT) && !GetVarIndexed(IDX_CHECK_INTERLIGHT) && (GetVarIndexed(IDX_CHECK_HOPPERLIGHT) || flagOKB600_Init)) {
        if (GetVarIndexed(IDX_CHECK_HOPPERLIGHT)) {
          if (IsKeyPressedNew(1)) {
            Light -> On.RTR_HopperTop = 1 - Light -> On.RTR_HopperTop;
          }
        }
        if (flagOKB600_Init) {
          if (IsKeyPressedNew(1)) {
            Light -> On.OKB600_WorkRear = 1 - Light -> On.OKB600_WorkRear;
          }
        }
      }
      if (GetVarIndexed(IDX_CHECK_SIDELIGHT) && GetVarIndexed(IDX_CHECK_INTERLIGHT) && !GetVarIndexed(IDX_CHECK_HOPPERLIGHT)) {
        if (Ctrl -> sens.DI.Ignition) {
          if (IsKeyPressedNew(1)) {
            Light -> On.WorkSide = 1 - Light -> On.WorkSide;
          }
          if (IsKeyPressedNew(2)) {
            Light -> On.BrushLight = 1 - Light -> On.BrushLight;
          }
        }
      }
      if (GetVarIndexed(IDX_CHECK_SIDELIGHT) && !GetVarIndexed(IDX_CHECK_INTERLIGHT) && (GetVarIndexed(IDX_CHECK_HOPPERLIGHT) || flagOKB600_Init)) {
        if (Ctrl -> sens.DI.Ignition) {
          if (IsKeyPressedNew(1)) {
            Light -> On.WorkSide = 1 - Light -> On.WorkSide;
          }
        }
        if (GetVarIndexed(IDX_CHECK_HOPPERLIGHT)) {
          if (IsKeyPressedNew(2)) {
            Light -> On.RTR_HopperTop = 1 - Light -> On.RTR_HopperTop;
          }
        }
        if (flagOKB600_Init) {
          if (IsKeyPressedNew(2)) {
            Light -> On.OKB600_WorkRear = 1 - Light -> On.OKB600_WorkRear;
          }
        }
      }
      if (GetVarIndexed(IDX_CHECK_SIDELIGHT) && !GetVarIndexed(IDX_CHECK_INTERLIGHT) && !GetVarIndexed(IDX_CHECK_HOPPERLIGHT)) {
        if (Ctrl -> sens.DI.Ignition) {
          if (IsKeyPressedNew(1)) {
            Light -> On.WorkSide = 1 - Light -> On.WorkSide;
          }
        }
      }
    }
  }

  if ((IsMaskOn(MSK_MAIN)) == 1 && (GetVarIndexed(IDX_LIGHT_ALARM) != Prev_alarm)) {
    Prev_alarm = GetVarIndexed(IDX_LIGHT_ALARM); //Check if more light are off or on
    uint32_t LightCount = 0;
    if (Light -> On.LRS_LeftRoadSign)
      LightCount++;
    if (Light -> On.LRS_RightRoadSign)
      LightCount++;
    if (Light -> On.LRS_RearBeacon)
      LightCount++;
    if (Light -> On.FrontBeaconCab)
      LightCount++;
    if (Light -> On.LRS_WorkRear)
      LightCount++;
    if (Light -> On.WorkSide)
      LightCount++;
    if (Light -> On.BrushLight)
      LightCount++;
      if (Light -> On.OKB600_WorkRear)
      LightCount++;

    if (LightCount > 0) { //most lights are on
      Light -> On.RTR_LeftRoadSign = 0;
      Light -> On.LRS_LeftRoadSign = 0;
      Light -> On.RTR_RightRoadSign = 0;
      Light -> On.LRS_RightRoadSign = 0;
      Light -> On.RTR_RearBeacon = 0;
      Light -> On.LRS_RearBeacon = 0;
      Light -> On.FrontBeaconCab = 0;
      Light -> On.RTR_WorkRear = 0;
      Light -> On.LRS_WorkRear = 0;
      Light -> On.PloughHead[0] = 0;
      Light -> On.PloughHead[1] = 0;
      Light -> On.WorkSide = 0;
      Light -> On.BrushLight = 0;
      Light -> On.OKB600_WorkRear = 0;
    } else { // most lights are off.
      if (GetVarIndexed(IDX_ROADSIGN_LEFT) != 0) {
        Light -> On.RTR_LeftRoadSign = 1;
        Light -> On.LRS_LeftRoadSign = 1;
      }
      if (GetVarIndexed(IDX_ROADSIGN_RIGHT) != 0) {
        Light -> On.RTR_RightRoadSign = 1;
        Light -> On.LRS_RightRoadSign = 1;
      }
      Light -> On.RTR_RearBeacon = 1;
      Light -> On.LRS_RearBeacon = 1;
      Light -> On.FrontBeaconCab = 1;
      Light -> On.PloughHead[0] = 1;
      Light -> On.PloughHead[1] = 1;
      if (Ctrl -> sens.DI.Ignition) {
        if (GetVarIndexed(IDX_CHECK_SIDELIGHT) != 0) {
          Light -> On.WorkSide = 1;
        }
        Light -> On.RTR_WorkRear = 1;
        Light -> On.LRS_WorkRear = 1;
        if (GetVarIndexed(IDX_CHECK_INTERLIGHT) != 0) {
          Light -> On.BrushLight = 1;
        }
      }
    }
  }

  if(!Ctrl -> sens.DI.Ignition) {
    Light -> On.LRS_WorkRear = 0;
    Light -> On.RTR_WorkRear = 0;
    Light -> On.WorkSide = 0;
    Light -> On.BrushLight = 0;
  }

  if(TopGetSpeed() >= 5) {
    Light -> On.RTR_HopperTop = 0;
  }
  // Ctrl_Light_Worklight_AutoOn(Light);
  Ctrl_Light_AutoOn(Light);
}

void Light_Draw(const tLight * Light) {
  if ((Light -> On.RTR_LeftRoadSign && GetVarIndexed(IDX_ROADSIGN_LEFT) != 0) ||
    (Light -> On.LRS_LeftRoadSign && GetVarIndexed(IDX_ROADSIGN_LEFT) != 0) ||
    (Light -> On.RTR_RightRoadSign && GetVarIndexed(IDX_ROADSIGN_RIGHT) != 0) ||
    (Light -> On.LRS_RightRoadSign && GetVarIndexed(IDX_ROADSIGN_RIGHT) != 0) ||
    Light -> On.RTR_RearBeacon ||
    Light -> On.LRS_RearBeacon ||
    Light -> On.FrontBeaconCab ||
    Light -> On.RTR_WorkRear ||
    Light -> On.RTR_HopperTop ||
    Light -> On.OKB600_WorkRear ||
    Light -> On.BrushLight ||
    Light -> On.LRS_WorkRear ||
    Light -> On.WorkSide) {
    SetVarIndexed(IDX_LIGHT_ON, 1);
  } else {
    SetVarIndexed(IDX_LIGHT_ON, 0);
  }
  //Blink
  static int32_t count = 0;
  if (500 < (GetMSTick() - count)) {
    count = GetMSTick();
    //Back beacon blinking
    if (Light -> On.RTR_RearBeacon) {
      SetVarIndexed(IDX_LIGHT_REARBEACON_0, 1 - GetVarIndexed(IDX_LIGHT_REARBEACON_0));
      SetVarIndexed(IDX_LIGHT_REARBEACON_1, 1 - GetVarIndexed(IDX_LIGHT_REARBEACON_0));
    } else {
      SetVarIndexed(IDX_LIGHT_REARBEACON_0, 0);
      SetVarIndexed(IDX_LIGHT_REARBEACON_1, 0);
    }

    //Front beacon blinking
    if (Light -> On.FrontBeaconCab) {
      SetVarIndexed(IDX_LIGHT_FRONTBEACON, 1 - GetVarIndexed(IDX_LIGHT_FRONTBEACON));
    } else {
      SetVarIndexed(IDX_LIGHT_FRONTBEACON, 0);
    }
    if (Light -> On.LRS_LeftRoadSign) {
      SetVarIndexed(IDX_ROADSIGN_LEFT_BLINK, GetVarIndexed(IDX_ROADSIGN_LEFT));
    } else {
      SetVarIndexed(IDX_ROADSIGN_LEFT_BLINK, 0);
    }

    if (Light -> On.LRS_RightRoadSign) {
      SetVarIndexed(IDX_ROADSIGN_RIGHT_BLINK, GetVarIndexed(IDX_ROADSIGN_RIGHT));
    } else {
      SetVarIndexed(IDX_ROADSIGN_RIGHT_BLINK, 0);
    }
  }

  if (Light -> On.RTR_WorkRear) {
    SetVarIndexed(IDX_LIGHT_REAR, 1);
  } else {
    SetVarIndexed(IDX_LIGHT_REAR, 0);
  }

  if (Light -> On.WorkSide) {
    SetVarIndexed(IDX_LIGHT_SIDE, 1);
  } else {
    SetVarIndexed(IDX_LIGHT_SIDE, 0);
  }

  if (Light -> On.BrushLight) {
    SetVarIndexed(IDX_LIGHT_INTER, 1);
  } else {
    SetVarIndexed(IDX_LIGHT_INTER, 0);
  }

  if (Light -> On.RTR_HopperTop) {
    SetVarIndexed(IDX_LIGHT_HOPPERTOP, 1);
  } else {
    SetVarIndexed(IDX_LIGHT_HOPPERTOP, 0);
  }

  if (Light -> On.OKB600_WorkRear) {
    SetVarIndexed(IDX_LIGHT_OKB600, 1);
  } else {
    SetVarIndexed(IDX_LIGHT_OKB600, 0);
  }

  if (Light -> On.FrontBeaconCab) {
    SetVarIndexed(IDX_LIGHT_BEACONON, 1);
  } else {
    SetVarIndexed(IDX_LIGHT_BEACONON, 0);
  }

  if (Light -> On.LRS_LeftRoadSign) {
    SetVarIndexed(IDX_ROADSIGN_LEFTON, GetVarIndexed(IDX_ROADSIGN_LEFT));
    SetVar(HDL_ATT_LEFTSIGNON, ATTR_VISIBLE);
  } else {
    SetVarIndexed(IDX_ROADSIGN_LEFTON, 0);
    SetVar(HDL_ATT_LEFTSIGNON, 0);
  }

  if (Light -> On.LRS_RightRoadSign) {
    SetVarIndexed(IDX_ROADSIGN_RIGHTON, GetVarIndexed(IDX_ROADSIGN_RIGHT));
    SetVar(HDL_ATT_RIGHTSIGNON, ATTR_VISIBLE);
  } else {
    SetVarIndexed(IDX_ROADSIGN_RIGHTON, 0);
    SetVar(HDL_ATT_RIGHTSIGNON, 0);
  }
}

// const char * Light_GetSignName(int sign_state) {
//   if(sign_state > 3 || sign_state < 0) {
//     return "biba i boba prishli v buhgalteriyu no tam vse pereputali";
//   }

//   return RCTextGetListElement(RCTEXT_L_ROADSIGNS, sign_state, GetVar(HDL_SYS_LANGUAGE));
// }

// int Light_GetStateIdx(const char * name) {
//   for(int i = 0; i < 4; i++) {
//     if(strcmp(Light_GetSignName(i), name)) {
//       return i;
//     }
//   }
//   return 0;
// }

#define PATH_LIGHT_BACKUP "/gs/usb/BACKUP_LAST/Light.cfg"

int Light_SaveToBackup( void ) {
  tGsFile* fp = FileOpen( PATH_LIGHT_BACKUP, "w" );
  if( fp == NULL ) {
		return -1;
	}

  FilePrintf(fp, "LEFT SIGN=\"%d\"\r\n", GetVar(HDL_ROADSIGN_LEFT));
  FilePrintf(fp, "RIGHT SIGN=\"%d\"\r\n", GetVar(HDL_ROADSIGN_RIGHT));
  FilePrintf(fp, "INTER LIGHT=\"%d\"\r\n", GetVar(HDL_CHECK_INTERLIGHT));
  FilePrintf(fp, "SIDE LIGHT=\"%d\"\r\n", GetVar(HDL_CHECK_SIDELIGHT));
  FilePrintf(fp, "HOPPER LIGHT=\"%d\"\r\n", GetVar(HDL_CHECK_HOPPERLIGHT));

  FileClose( fp );

  return 0;
}

int Light_LoadFromBackup( void ) {
  SetVar(HDL_ROADSIGN_LEFT, 0);
  SetVar(HDL_ROADSIGN_RIGHT, 0);
  SetVar(HDL_CHECK_INTERLIGHT, 0);
  SetVar(HDL_CHECK_SIDELIGHT, 0);
  SetVar(HDL_CHECK_HOPPERLIGHT, 0);

  tGsFile* fp = FileOpen( PATH_LIGHT_BACKUP, "r" );
  if( fp == NULL ) {
		return -1;
	}

  char delimeter[] = "=\r\n\"";
  char buffer[128];

  while(FileGets(buffer, sizeof(buffer), fp)) {
    if(strstr(buffer, "LEFT SIGN")) {
      char* ptr;

      ptr = strtok( buffer, delimeter );
      ptr = strtok( NULL, delimeter );

      // SetVar(HDL_ROADSIGN_LEFT, Light_GetStateIdx(ptr));
      // SetVisObjData( OBJ_PROFILE_4, ptr, 4 );
      SetVar(HDL_ROADSIGN_LEFT, *ptr - '0');
    }
    else if(strstr(buffer, "RIGHT SIGN")) {
      char* ptr;

      ptr = strtok( buffer, delimeter );
      ptr = strtok( NULL, delimeter );

      // SetVar(HDL_ROADSIGN_RIGHT, Light_GetStateIdx(ptr));
      // SetVisObjData( OBJ_PROFILE_5, ptr, 4 );
      SetVar(HDL_ROADSIGN_RIGHT, *ptr - '0');
    }
    else if(strstr(buffer, "INTER LIGHT")){
      char* ptr;

      ptr = strtok( buffer, delimeter );
      ptr = strtok( NULL, delimeter );

      SetVar(HDL_CHECK_INTERLIGHT, *ptr - '0');
    }
    else if(strstr(buffer, "SIDE LIGHT")) {
      char* ptr;

      ptr = strtok( buffer, delimeter );
      ptr = strtok( NULL, delimeter );

      SetVar(HDL_CHECK_SIDELIGHT, *ptr - '0');
    }
    else if(strstr(buffer, "HOPPER LIGHT")) {
      char* ptr;

      ptr = strtok( buffer, delimeter );
      ptr = strtok( NULL, delimeter );

      SetVar(HDL_CHECK_HOPPERLIGHT, *ptr - '0');
    }
  }

  FileClose( fp );
  return 0;
}
