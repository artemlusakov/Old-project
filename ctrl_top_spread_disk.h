#ifndef CTRL_TOP_SPREAD_DISK_H
#define CTRL_TOP_SPREAD_DISK_H
#include "sql_table.h"
#include "hydvalvectrl.h"
#include "drawfunction.h"

typedef enum {
	AUTO_MODE = 0,
	MANUAL_MODE_30km,
	MANUAL_MODE_20km,
	MANUAL_MODE_10km,
} OperatingMode;

typedef enum {
	STOP_DISK,
	DELAY_DISK,
	SLOW_DISK
} DiskModeSettingAuto;

typedef struct tagSpreaderDisk {
	uint8_t on;
	uint32_t Width;
	uint16_t PWM;
	tSQLTable* Table;
	tValveProp Valve;
	tPaintText ColourWidth;
} tSpreaderDisk;

void SpreaderDiskCreate( tSpreaderDisk* Disk );
void SpreaderDiskInit( tSpreaderDisk* Disk, uint32_t Device, uint32_t idx );
void SpreaderDiskCycle( tSpreaderDisk* Disk, uint32_t width );
void SpreaderDisk_Switch( tSpreaderDisk* Disk, uint32_t value );
void SpreaderDiskTimer( tSpreaderDisk* Disk );

#endif  // #ifndef CTRL_TOP_SPREAD_DISK_H
