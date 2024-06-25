#include <stdint.h>
#include <UserCEvents.h>
#include <UserCAPI.h>
#include "vartab.h"
#include "objtab.h"
#include "visu.h"
#include "io.h"
#include "profile.h"

#include "control.h"
#include "param.h"
#include "visu_ctrl.h"
#include "gsUsbHid.h"

#define PASSWORD_ADMIN_STANDARD    1
#define PASSWORD_MERKATOR_STANDARD 4848
#define PASSWORD_MASTER_STANDARD 32546

tVisuData VisuData;
int32_t init = 0;

int UserCInit(uint32_t initFlags) {
  // First device start.
  // SetDebugWindow(1);
  if (GS_INIT_FLAG_FIRST_PROJECT_START & initFlags) {

    //Set Standard Passwords at first device start.
    if (0 == GetVar(HDL_PASSWORD_MERKATOR)) {
      SetVar(HDL_PASSWORD_MERKATOR, PASSWORD_MERKATOR_STANDARD);
      SetVar(HDL_PASSWORD_ADMIN, PASSWORD_ADMIN_STANDARD);
      SetVar(HDL_PASSWORD_MASTER, PASSWORD_MASTER_STANDARD);
    }
    //Set Brightness of Displaybacklight
    SetVar(HDL_BRIGHTNESS, 100);
    SetVar(HDL_MATERIALDENSITY, 1000);
  }
  /* The return value is the interval of UserC-Timer in steps of 10 ms */
  /* Returning 0 disables the function UserCTimer */
  return 1;
}

/****************************************************************************
  UserC cyclic part
****************************************************************************/
void UserCCycle(uint32_t evtc, tUserCEvt * evtv) {
  //The UserCInit is limited by a timeout. That's why the system initialisation will be done in the first cycle.
  if (!init) {
    // SetDebugWindow(1); //Use this to show debug output on the screen

    //Read Parameters
    InitParam();

    Prof_Init();
    Ctrl_Init();
    Visu_Init( & VisuData);
    init = 1;
    gsUsbHidSetLayout(0, GS_USB_HID_KEY_LAYOUT_US_INTERNATIONAL);

  } else {
    Visu_Ctrl_Cycle(evtc, evtv);
    Visu_Cycle(evtc, evtv);
    Ctrl_Cycle(evtc, evtv);
  }
}

void UserCTimer(void) {
  /* Enter your time critical code here ... */
  /* Be aware that the execution time must not exceed the timer interval */

  //Call timer functions only if init is complete
  if (init == 1) {
    Visu_Timer();
    Ctrl_Timer_MS();
  }
}

void UserCDeInit(void) {
  Ctrl_DeInit();
}
