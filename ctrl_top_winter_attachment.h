#ifndef CTRL_TOP_WINTER_ATTACHMENT_H
#define CTRL_TOP_WINTER_ATTACHMENT_H

#include "ctrl_top_adrive.h"
#include "ctrl_top_adrive_feedback.h"
#include "ctrl_top_spread_disk.h"
#include "ctrl_top_conveyor.h"
#include "drawfunction.h"


#define IDXBITMAP_WETTINGSENSOR_NO_SENSOR 0
#define IDXBITMAP_WETTINGSENSOR_FULL 1
#define IDXBITMAP_WETTINGSENSOR_EMPTY 2


#define MAT_SENSOR_OK 0
#define MAT_SENSOR_EMPTY 1
#define MAT_SENSOR_NO_ROT 2

#define TMT_SPREAD_SENS 5000 // задержка времени на наличие материала


typedef struct tagRTRHumid{
  uint8_t     HumidAvailable;
  uint8_t     on;
  uint8_t     ManualMode; // 0 - автоматический, 1 - ручной режим
  int32_t     HumidPercent;
	tValveProp	ValvHumid;
  tPaintText  ColourPercent;
} tRTRHumid;

typedef struct tagWinterTop {
  uint8_t     on;
  tValv       ValveMain;

  uint32_t		SpreadWidth;
	uint32_t		SpreadDensity;

  uint8_t     DumpOn;

  // Рабочие элементы установки
  tSpreaderDisk SpreadDisk; // Диск
  tConveyor     SpreadConv; // Конвейер

  uint8_t     AsymCalibrOn; // Флаг включения калибровки
  int8_t     AsymCalibrStatus; // Статус калибровки: 1 - успешно, 2 - не удачно
  tAsymerticDrive Adrive; // Асимметрия без обратной связи
	tAsymerticDriveFeedback AdriveFB; // Асимметрия с обратной связи (FeedBack)

  tRTRHumid RTRHumid;

} tWinterTop;


void RTR_Humid_Switch(tRTRHumid * Humid, int32_t value);
void RTR_Humid_ManualMode_Switch(tRTRHumid * Humid, int32_t value);

void RTR_Command(tWinterTop * wTop);
void RTR_Draw( tWinterTop * wTop );
void Ctrl_RTR_Dump(tWinterTop * wTop);
uint8_t RTRDump_IsOn(tWinterTop * wTop);

// Локальные функции
void Ctrl_RTR_Default_Init(tWinterTop * wTop);
void Ctrl_RTR_Default_Cycle(tWinterTop * wTop, uint32_t evtc, tUserCEvt * evtv);

void RTR_Humid_Init(tRTRHumid * Pump);
void RTR_Humid_Cycle(tWinterTop * wTop);
void RTR_Humid_Percent(tWinterTop * wTop);

void AsymDrive_Init(tWinterTop * wTop);
void AsymDriveFB_Init(tWinterTop * wTop);
void DrawNoAsym(void);

void HumidAvailable_Switch( tRTRHumid * Humid, int32_t value );
void Material_Spreading_Sensor_Cycle(void);


#endif  // #ifndef CTRL_TOP_WINTER_ATTACHMENT_H
