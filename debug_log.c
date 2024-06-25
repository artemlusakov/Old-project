#include <stdint.h>
#include <UserCEvents.h>
#include <UserCAPI.h>
#include <gsDebug.h>
#include "vartab.h"
#include "objtab.h"
#include "debug_log.h"

#define DEBUGLOG_PATH "/gs/data/dbg_log.txt"
#define DEBUGLOG_PATH_USB "/gs/usb/dbg_log.txt"

void DebugLog_Init(eDebugLog_LineMode mode) {
  static uint8_t flagInit = 0;
  if (!flagInit) {
    gsDebugStreamsAddFile(DEBUGLOG_PATH);
    gsDebugStreamsInit(GS_DBG_STREAM_FILE);
    gsDebugStreamsSetLineMode(mode);
    flagInit = 1;
  }
}

void DebugLog_Export(void) {
  gsDebugStreamsDeInit();
  CopyFile(DEBUGLOG_PATH, DEBUGLOG_PATH_USB, NULL);
}