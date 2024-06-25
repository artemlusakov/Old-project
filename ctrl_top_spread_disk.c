#include <stdint.h>
#include <UserCEvents.h>
#include <UserCAPI.h>
#include "vartab.h"
#include "objtab.h"
#include "ctrl_top_spread_disk.h"
#include "sql_table.h"
#include "control.h"

tSQLTable DiskTable = {
  .TableName = "Spreader Disk",
  .ColumnNames = {"Width", "PWM Up", "PWM Down", "EMPTY"},
  .Cell = {
  {2, 102, 111, 0},
  {3, 113, 122, 0},
  {4, 125, 133, 0},
  {5, 136, 144, 0},
  {6, 148, 155, 0},
  {7, 160, 166, 0},
  {8, 171, 177, 0},
  {9, 183, 188, 0},
  {10, 194, 199, 0},
  {11, 206, 211, 0},
  {12, 218, 222, 0},
  }
};


void SpreaderDiskCreate(tSpreaderDisk * Disk) {
  memset(Disk, 0, sizeof(tSpreaderDisk));
  Disk -> Table = SQL_Tabel_Init( & DiskTable);
}

void SpreaderDiskInit(tSpreaderDisk * Disk, uint32_t Device, uint32_t idx) {
  ValveProp_Init( & Disk -> Valve, Device, idx, 100, 10);
  SetVar(HDL_SPREADERDELAY, 2);
}

void SpreaderDiskCycle(tSpreaderDisk * Disk, uint32_t setWidth) {
  const tControl * Ctrl = CtrlGet();
  static uint8_t q_stop_ = 1;
  static int tm_old_ = 0;
  uint8_t wrk_speed_ = GetVar(HDL_WORKSPEED);

  if (Disk -> on) {
    if (setWidth > Disk -> Width || q_stop_) {
      Disk -> Width = setWidth;
      Disk -> PWM = TableGetLSEValue(Disk -> Table, 0, 1, setWidth);
      q_stop_ = 0;
    } else if (setWidth < Disk -> Width) {
      Disk -> Width = setWidth;
      Disk -> PWM = TableGetLSEValue(Disk -> Table, 0, 2, setWidth);
      q_stop_ = 0;
    }

    OperatingMode mode_ = Ctrl -> automatic.on;
    DiskModeSettingAuto md_auto_set_ = GetVar(HDL_WORKMODEDISK);

    switch (mode_) {
    case MANUAL_MODE_30km: {
      tm_old_ = GetMSTick();
    }
    break;
    case AUTO_MODE: {
      switch (md_auto_set_) {
      case STOP_DISK: {
        if (TopGetSpeed() <= wrk_speed_) {
          Disk -> PWM = 0;
          q_stop_ = 1;
        }
      }
      break;
      case DELAY_DISK: {
        if (TopGetSpeed() <= wrk_speed_) {
          uint32_t delay_ = 1000 * GetVar(HDL_SPREADERDELAY);
          if (delay_ < GetMSTick() - tm_old_) {
            Disk -> PWM = 0;
          }
          q_stop_ = 1;
        } else {
          tm_old_ = GetMSTick();
        }
      }
      break;
      case SLOW_DISK: {
        if (TopGetSpeed() <= wrk_speed_) {
          Disk -> PWM = TableGetLSEValue(Disk -> Table, 0, 2, 2);
          q_stop_ = 1;
        }
      }
      break;
      }
    }
    break;
    }
  } else {
    q_stop_ = 1;
    Disk -> PWM = 0;
  }
  ValvProp_Set( & Disk -> Valve, Disk -> PWM);
}

void SpreaderDisk_Switch(tSpreaderDisk * Disk, uint32_t value) {
  Disk -> on = value;
}

void SpreaderDiskTimer(tSpreaderDisk * Disk) {
  ValvProp_Timer_100ms( & Disk -> Valve);
}