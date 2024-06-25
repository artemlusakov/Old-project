#include <stdint.h>
#include <UserCEvents.h>
#include <UserCAPI.h>
#include "vartab.h"
#include "objtab.h"
#include "gsToVisu.h"
#include "drawfunction.h"
#include "RCColor.h"

void PaintText_Init(tPaintText * pt, uint32_t colourActive, uint32_t colourInactive, uint32_t obj) {
  pt -> colourActive = RCColorGetColor(GetVarIndexed(IDX_SYS_COLOR), colourActive);
  pt -> colourInactive = RCColorGetColor(GetVarIndexed(IDX_SYS_COLOR), colourInactive);
  pt -> Obj = obj;
}

void PaintText_Cycle(tPaintText * pt, int8_t active) {
  if (pt -> oldVal != (uint8_t) active) {
    pt -> oldVal = active;
    if (active) {
      SendToVisuObj(pt -> Obj, GS_TO_VISU_SET_FG_COLOR, pt -> colourActive);
    } else {
      SendToVisuObj(pt -> Obj, GS_TO_VISU_SET_FG_COLOR, pt -> colourInactive);
    }
  }
}