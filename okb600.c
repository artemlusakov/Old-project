#include <stdint.h>
#include <UserCEvents.h>
#include <UserCAPI.h>
#include <math.h>
#include "vartab.h"
#include "objtab.h"
#include "RCText.h"
#include "control.h"
#include "io.h"
#include "ctrl_abc_attach.h"
#include "ctrl_brush.h"
#include "hydvalvectrl.h"
#include "gseerrorlist.h"
#include "gsemcm.h"
#include "gse_msgbox.h"
#include "gsToVisu.h"
#include "profile.h"
#include "ctrl_light.h"
#include "ctrl_top_summer_attachment.h"


const uint16_t minSetDistanceCopy = 280; // Минимальное значение установки для копирования (мм)
const uint16_t maxSetDistanceCopy = 800; // Максимальное значение установки для копирования (мм)

const uint16_t minSaveDistanceCopy = 250; // Минимальный предел безопасности (мм)
const uint16_t maxSaveDistanceCopy = 1400; // Максимальный предел безопасности (мм)

const uint16_t stepSetDistanceCopy = 20; // Шаг изменения значения установки для вертикального копирования (мм)
const uint16_t stepPercentDischarge = 5; // Шаг изменения процента разгрузки для горизонтального копирования (мм)

const uint16_t UsSens1_Out = 0; // Выход первого датчика (нижний по дефолту)
const uint16_t UsSens2_Out = 1; // Выход второго датчика (верхний по дефолту)
const uint16_t minMASens = 350; // Минимальный ток для проверки наличия УЗ датчиков

const uint16_t LinearTolerance = 20; // Линейный допуск (мм)
const uint16_t TiltTolerance = 20; // Наклонный допуск (мм)

const uint16_t tmrOutLimitDistance_Lower = 2500; // Время диагностики потери нижнего датчика (мс)
const uint16_t tmrOutLimitDistance_Upper = 2500; // Время диагностики потери верхнего датчика (мс)
const uint16_t tmrSecurityProcessed = 3000; // Время смещения каретки (мс)

const uint16_t PWMTilt = 380; // Шим управления наклоном
const uint16_t PWMBogie = 450; // Шим управления смещения каретки

const uint16_t SideWorkRight = 0; // Правая сторона работы
const uint16_t SideWorkLeft = 1; // Левая сторона работы

/* global variables *********************************************************/

static uint16_t modeWork = MODE_INIT; // Режим работы OKB600

static uint16_t flagTiltCopy = 1; // Флаг активации режима копирования наклона (0 - выкл, 1 - вкл)
static uint16_t TypeCopy = 0; // Горизонтальное или вертикальное копирование (0 - горизонтальное, 1 - вертикальное)
static uint16_t flagCopyOn = 0; // Флаг активации режима копирования (0 - выкл, 1 - вкл)

static int16_t mA_UsSensLower, mA_UsSensUpper = 0; // Напряжение от датчиков (мА)
static uint8_t Power_UsSensLower, Power_UsSensUpper = 0; // Питание датчика (0 - нет датчика, 1 - датчик активен)
static int16_t Distance_UsSensLower, Distance_UsSensUpper = 0; // Дистанция от датчиков (мм)
static int16_t FilterDistance_UsSensLower, FilterDistance_UsSensUpper = 0; // Отфильтрованная дистанция от датчиков (мм)

static uint16_t SetDistanceCopy = 0; // Хранит значения расстояния для копирования
static int16_t ValueDischarge = 0; // Хранит значения процента разгрузки

static uint8_t ErrorSecurityProcessed = 0; // Флаг смещения каретки при ошибке (0 - каретка не изменяла положения, 1 - каретка сдвинулась от экрана)
static uint8_t ErrorSecurityFlag = 0; // Флаг отработки ошибки вертикального копирования
static uint8_t ErrorSecurityDist_Lower = 0; // Ошибка безопасности для нижнего датчика
static uint8_t ErrorSecurityDist_Upper = 0; // Ошибка безопасности для верхнего датчика


/* local function prototypes ************************************************/

void Ctrl_OKB600_HorizontalCopy(tOKB600Valve * Valves, uint16_t PercentDischarge);
void Ctrl_OKB600_VerticalCopy(tOKB600Valve * Valves, uint16_t DistanceCopy);
void Ctrl_OKB600_SecurityError(tOKB600Valve * Valves);
void Check_UsSens(void);

void OKB600_Ctrl_Joystick(const tControl * Ctrl);
void OKB600_Crawl(tOKB600Valve * Valves);
void Draw_OKB600(void);

/* functions ****************************************************************/

// Главный клапан
void OKB600_MainValve_Init(tOKB600Valve * Valves, uint32_t Device1, uint32_t Pos1) {
  ValvDisk_Init( & Valves -> MainValve, Device1, Pos1);
}
// Ходоуменьшитель
void OKB600_Crawl_Init(tOKB600Valve * Valves, uint32_t Device1, uint32_t Pos1) {
  ValvDisk_Init( & Valves -> ValveCrawl, Device1, Pos1);
}
// КОМ
void OKB600_PTO_Init(tOKB600Valve * Valves, uint32_t Device1, uint32_t Pos1) {
  ValvDisk_Init( & Valves -> ValvePTO, Device1, Pos1);
}
// Клапаны управления 1 секцией вверх/вниз
void OKB600_fSectionUp_Init(tOKB600Valve * Valves, uint32_t Device1, uint32_t Pos1) {
  ValvDisk_Init( & Valves -> fSectionUp, Device1, Pos1);
}
void OKB600_fSectionDown_Init(tOKB600Valve * Valves, uint32_t Device1, uint32_t Pos1) {
  ValvDisk_Init( & Valves -> fSectionDown, Device1, Pos1);
}
// Разгрузочный клапан
void OKB600_ValveDischarge_Init(tOKB600Valve * Valves, uint32_t Device1, uint32_t Pos1) {
  ValveProp_NoPort_Init( & Valves -> ValveDischarge.Valve.PVEH, Device1, Pos1);
}
// Клапаны управления 2 секцией вверх/вниз
void OKB600_sSectionUp_Init(tOKB600Valve * Valves, uint32_t Device1, uint32_t Pos1) {
  ValveProp_NoPort_Init( & Valves -> sSectionUp.Valve.PVEH, Device1, Pos1);
}
void OKB600_sSectionDown_Init(tOKB600Valve * Valves, uint32_t Device1, uint32_t Pos1) {
  ValveProp_NoPort_Init( & Valves -> sSectionDown.Valve.PVEH, Device1, Pos1);
}
// Клапаны управления кареткой влево/вправо
void OKB600_Zoom_Init(tOKB600Valve * Valves, uint32_t Device1, uint32_t Pos1) {
  ValveProp_NoPort_Init( & Valves -> ValveZoom.Valve.PVEH, Device1, Pos1);
}
void OKB600_Decrease_Init(tOKB600Valve * Valves, uint32_t Device1, uint32_t Pos1) {
  ValveProp_NoPort_Init( & Valves -> ValveDecrease.Valve.PVEH, Device1, Pos1);
}
// Клапаны управления поворотом стрелы влево/вправо
void OKB600_TurnRight_Init(tOKB600Valve * Valves, uint32_t Device1, uint32_t Pos1) {
  ValvDisk_Init( & Valves -> ValveTurnRight, Device1, Pos1);
}
void OKB600_TurnLeft_Init(tOKB600Valve * Valves, uint32_t Device1, uint32_t Pos1) {
  ValvDisk_Init( & Valves -> ValveTurnLeft, Device1, Pos1);
}
// Клапаны управления наклоном
void OKB600_TiltClockwise_Init(tOKB600Valve * Valves, uint32_t Device1, uint32_t Pos1) {
  ValveProp_NoPort_Init( & Valves -> ValveTiltClockwise.Valve.PVEH, Device1, Pos1);
}
void OKB600_TiltCounterclockwise_Init(tOKB600Valve * Valves, uint32_t Device1, uint32_t Pos1) {
  ValveProp_NoPort_Init( & Valves -> ValveTiltCounterClockwise.Valve.PVEH, Device1, Pos1);
}

/**
 * @brief Функция фильтрации сигнала "бегущее среднее".
 * Функция не универсальная, сделана на 2 датчика
 * @param newVal Показания с датчика
 * @param k Коэффициент фильтрации, 0.0-1.0
 * @param idx Индекс передаваемого датчика
 * @return int
 */
int expRunningAverage(float newVal, uint8_t idx) {
  static float filVal[2] = {0};
  static float newValArr[2] = {0};
  newValArr[idx] = newVal;
  filVal[idx] += (newValArr[idx] - filVal[idx]) * 0.2; // k
  return filVal[idx];
}

uint8_t flagOKB600_Init = 0;
void Ctrl_OKB600_Init(tOKB600Valve * Valves) {
  tControl * Ctrl = CtrlGet();
  OKB600_MainValve_Init(Valves, MCM250_8, 3);
  OKB600_fSectionUp_Init(Valves, MCM250_7, 4);
  OKB600_fSectionDown_Init(Valves, MCM250_7, 6);
  OKB600_ValveDischarge_Init(Valves, MCM250_8, 1);
  OKB600_sSectionUp_Init(Valves, MCM250_8, 7);
  OKB600_sSectionDown_Init(Valves, MCM250_8, 5);
  OKB600_TurnRight_Init(Valves, MCM250_8, 2);
  OKB600_TurnLeft_Init(Valves, MCM250_8, 0);
  Brush_Rotate_Init( & Ctrl -> ABC[EQUIP_A].BrushValve, MCM250_7, 2, 0);
  Brush_A_B( & Ctrl -> ABC[EQUIP_A].BrushValve, PVE_PORT_B);
  OKB600_TiltClockwise_Init(Valves, MCM250_7, 5);
  OKB600_TiltCounterclockwise_Init(Valves, MCM250_7, 7);
  OKB600_Crawl_Init(Valves, MCM250_3, 2);
  OKB600_PTO_Init(Valves, MCM250_3, 5);

  if (GetVarIndexed(IDX_SIDEWORK) == SideWorkRight) {
    OKB600_Zoom_Init(Valves, MCM250_7, 3);
    OKB600_Decrease_Init(Valves, MCM250_7, 1);
  }
  // else {
  //   OKB600_Zoom_Init(Valves, MCM250_7, 1);
  //   OKB600_Decrease_Init(Valves, MCM250_7, 3);
  // }

  WaterPump_ChangePinout( & Ctrl -> top.SummerTop.Pump, MCM250_3, 3);
  WaterPump_SetMaxLiter(65000);

  TypeCopy = GetVarIndexed(IDX_TYPECOPY);
  flagTiltCopy = GetVarIndexed(IDX_TYPETILT);
  SetDistanceCopy = GetVarIndexed(IDX_SETDISTANCECOPY);
  ValueDischarge = GetVarIndexed(IDX_VALUEDISCHARGE);
  SetVarIndexed(IDX_JOYSTICKSTATE, 1);

  SetTop_CntEmpty();
  // Spreader settings - выбор стороны копирования
  SendToVisuObj(OBJ_ATRESTTTEXT907, GS_TO_VISU_SET_ATTR_VISIBLE, 1);
  SendToVisuObj(OBJ_ATRESTTTEXTLIST914, GS_TO_VISU_SET_ATTR_VISIBLE, 1);

  flagOKB600_Init = 1;
}

void Ctrl_OKB600_Cycle(tOKB600Valve * Valves, tJSCommands * Cmd) {
  const tControl * Ctrl = CtrlGet();
  Draw_OKB600();
  OKB600_Ctrl_Joystick(Ctrl);

  // Обнуление всех управляющих переменных
  Valves -> MainValveValue = 1; // Открытие главного клапана (Открыт всегда)
  Valves -> fSectionUpValue = 0;
  Valves -> fSectionDownValue = 0;
  Valves -> sSectionUpValue = 0;
  Valves -> sSectionDownValue = 0;
  Valves -> ValveTurnRightValue = 0;
  Valves -> ValveTurnLeftValue = 0;
  Valves -> ValveZoomValue = 0;
  Valves -> ValveDecreaseValue = 0;
  Valves -> ValveTiltClockwiseValue = 0;
  Valves -> ValveTiltCounterClockwiseValue = 0;
  Valves -> ValveDischargeValue = 0;


  // Ручное управление
  int16_t JstUpDown = IOT_AnaJoy_GetInPercent( & Ctrl -> joystick.UpDown) * 10;
  int16_t JstLeftRight = IOT_AnaJoy_GetInPercent( & Ctrl -> joystick.LeftRight) * 10;
  int16_t JstX = IOT_AnaJoy_GetInPercent( & Ctrl -> joystick.X) * 10;
  int16_t JstZ = IOT_AnaJoy_GetInPercent( & Ctrl -> joystick.Z) * 10;

  if (!GetVarIndexed(IDX_JOYSTICKSTATE)) {

    // Управление вверх/вниз первой секцией
    if (JstUpDown > 0) {
      Valves -> fSectionDownValue = 1;
    } else if (JstUpDown < 0) {
      Valves -> fSectionUpValue = 1;
      Valves -> ValveDischargeValue = 1000;
    }

    // Управление вверх/вниз второй секцией
    if (!IOT_Button_IsDown( & Ctrl -> joystick.Button[0])) { // Кнопка А не нажата
      if (JstLeftRight > 0) {
        Valves -> sSectionUpValue = JstLeftRight;
      } else if (JstLeftRight < 0) {
        Valves -> sSectionDownValue = -JstLeftRight;
      }
    }

    // Сдвиг каретки влево/вправо
    if (JstZ > 0) { // Отдалиться
      Valves -> ValveDecreaseValue = JstZ;
    } else if (JstZ < 0) { // Приблизиться
      Valves -> ValveZoomValue = -JstZ;
    }

    // Поворот агрегата по/против часовой стрелки
    if (JstX > 0) {
      Valves -> ValveTiltCounterClockwiseValue = JstX;
    } else if (JstX < 0) {
      Valves -> ValveTiltClockwiseValue = -JstX;
    }

    // Поворот стрелы влево/вправо
    if (IOT_Button_IsDown( & Ctrl -> joystick.Button[0])) { // Кнопка А нажата
      if (JstLeftRight > 0) {
        Valves -> ValveTurnRightValue = JstLeftRight;
      } else if (JstLeftRight < 0) {
        Valves -> ValveTurnLeftValue = -JstLeftRight;
      }
    }

    // Управление щеткой
    Brush_Cycle( & Ctrl -> ABC[EQUIP_A].BrushValve, & Ctrl -> cmd_ABC[EQUIP_A], EQUIP_A);
  }

  OKB600_Crawl(Valves);


  // Проверка условий для активации режима копирования
  if(TypeCopy && flagCopyOn) {
    modeWork = MODE_VERTICAL_COPY;
  } else if(!TypeCopy && flagCopyOn) {
    modeWork = MODE_HORIZONTAL_COPY;
  } else if(ErrorSecurityDist_Lower || ErrorSecurityDist_Upper) {
    modeWork = MODE_SECURITY_ERROR;
  } else if(!flagCopyOn && !ErrorSecurityProcessed) {
    modeWork = MODE_INIT;
  }

  if(!ErrorSecurityDist_Lower && !ErrorSecurityDist_Upper && !ErrorSecurityProcessed) {
    ErrorSecurityFlag = 0;
  }

  // Режим работы
  switch (modeWork) {
  case MODE_INIT:
    break;

  case MODE_VERTICAL_COPY:
    Ctrl_OKB600_VerticalCopy(Valves, SetDistanceCopy);

    // Отключение копирования при попытке управления джойстиком
    if(flagTiltCopy) {
        if (JstZ || JstX) {
        flagCopyOn = 0;
        SetBuzzer(1, 30, 12, 3);
      }
    } else {
      if (JstZ) {
        flagCopyOn = 0;
        SetBuzzer(1, 30, 12, 3);
      }
    }
    break;

  case MODE_SECURITY_ERROR:
    Ctrl_OKB600_SecurityError(Valves);
    break;

  case MODE_HORIZONTAL_COPY:
    Ctrl_OKB600_HorizontalCopy(Valves, ValueDischarge);

    // Отключение копирования при попытке управления джойстиком
    if (JstUpDown) {
      flagCopyOn = 0;
      SetBuzzer(1, 30, 12, 3);
    }
    break;
  }



  // Обработка нажатия кнопок на пульте
  {
    // Выбор режима копирования (горизонтальное/вертикальное)
    if (!IsMsgContainerOn(CNT_LIGHTKEYS) && !IsMsgContainerOn(CNT_LIGHTKEYS_2) && IsInfoContainerOn(CNT_OKB600)) {
      if (!flagCopyOn) {
        if (IsKeyPressedNew(1)) {
          TypeCopy = 1 - TypeCopy;
        }
      }
      SetVarIndexed(IDX_TYPECOPY, TypeCopy);

      // Выбор режима вертикального копирования (с наклоном/без наклона)
      if(TypeCopy) {
        if (IsKeyPressedNew(2)) {
          flagTiltCopy = 1 - flagTiltCopy;
        }
      }
      SetVarIndexed(IDX_TYPETILT, flagTiltCopy);

      // Активация/деактивация джойстика
      if (IsKeyPressedNew(5)) {
        if (!flagCopyOn) {
          SetVarIndexed(IDX_JOYSTICKSTATE, 1 - GetVarIndexed(IDX_JOYSTICKSTATE));
        }
      }
    }
  }

  // Ограничение переменных
  {
    if (SetDistanceCopy < minSetDistanceCopy) {
      SetDistanceCopy = minSetDistanceCopy;
    } else if (SetDistanceCopy > maxSetDistanceCopy) {
      SetDistanceCopy = maxSetDistanceCopy;
    }
    if (ValueDischarge < 0) {
      ValueDischarge = 0; // %
    } else if (ValueDischarge > 100) {
      ValueDischarge = 100; // %
    }
  }
}

void Ctrl_OKB600_Timer_10ms(tOKB600Valve * Valves) {
  const tControl * Ctrl = CtrlGet();
  Brush_Timer_10ms( & Ctrl -> ABC[EQUIP_A].BrushValve);
  Valves -> tmrOKB600++;
  if (0 == (Valves -> tmrOKB600 % 10)) {
    // Управляющие сигналы
    Valv_Set( & Valves -> MainValve, Valves -> MainValveValue);
    Valv_Set( & Valves -> fSectionUp, Valves -> fSectionUpValue);
    Valv_Set( & Valves -> fSectionDown, Valves -> fSectionDownValue);
    ValvProp_Set( & Valves -> sSectionUp.Valve.PVEH, Valves -> sSectionUpValue);
    ValvProp_Set( & Valves -> sSectionDown.Valve.PVEH, Valves -> sSectionDownValue);
    Valv_Set( & Valves -> ValveTurnRight, Valves -> ValveTurnRightValue);
    Valv_Set( & Valves -> ValveTurnLeft, Valves -> ValveTurnLeftValue);
    ValvProp_Set( & Valves -> ValveZoom.Valve.PVEH, Valves -> ValveZoomValue);
    ValvProp_Set( & Valves -> ValveDecrease.Valve.PVEH, Valves -> ValveDecreaseValue);
    ValvProp_Set( & Valves -> ValveTiltClockwise.Valve.PVEH, Valves -> ValveTiltClockwiseValue);
    ValvProp_Set( & Valves -> ValveTiltCounterClockwise.Valve.PVEH, Valves -> ValveTiltCounterClockwiseValue);
    ValvProp_Set( & Valves -> ValveDischarge.Valve.PVEH, Valves -> ValveDischargeValue);
    Valv_Set( & Valves -> ValveCrawl, Valves -> ValveCrawlValue);
    Valv_Set( & Valves -> ValvePTO, Valves -> ValvePTOValue);
  }

  if (TypeCopy) {
    Check_UsSens();
  }

}

// Обработка нажатия кнопок на джойстике
void OKB600_Ctrl_Joystick(const tControl * Ctrl) {
  if (!GetVarIndexed(IDX_JOYSTICKSTATE)) {

    if (IOT_Button_IsReleasedShortNew( & Ctrl -> joystick.Button[3])) { // Кнопка D
      flagCopyOn = 1 - flagCopyOn;
    }

    // Управление рокером X
    static uint32_t tmr, tmr1 = 0;
    static uint8_t flagTmr, flagTmr1 = 0;
    int16_t JstY = IOT_AnaJoy_GetInPercent( & Ctrl -> joystick.Y) * 10;
    // Задание установки дистанции при вертикальном копировании
    if (GetVarIndexed(IDX_TYPECOPY)) {
      if (JstY > 500) {
        flagTmr1 = 0;
        if (!flagTmr) {
          tmr = GetMSTick();
          flagTmr = 1;
        }
        if (GetMSTick() - tmr > 300) {
          tmr = GetMSTick();
          SetDistanceCopy = SetDistanceCopy + stepSetDistanceCopy;
        }
      } else if (JstY < -500) {
        flagTmr = 0;
        if (!flagTmr1) {
          tmr1 = GetMSTick();
          flagTmr1 = 1;
        }
        if (GetMSTick() - tmr1 > 300) {
          tmr1 = GetMSTick();
          SetDistanceCopy = SetDistanceCopy - stepSetDistanceCopy;
        }
      }
    } else {
      // Задание процента разгрузки при горизонтальном копировании
      if (JstY > 500) {
        flagTmr1 = 0;
        if (!flagTmr) {
          tmr = GetMSTick();
          flagTmr = 1;
        }
        if (GetMSTick() - tmr > 300) {
          tmr = GetMSTick();
          ValueDischarge = ValueDischarge + stepPercentDischarge;
        }
      } else if (JstY < -500) {
        flagTmr = 0;
        if (!flagTmr1) {
          tmr = GetMSTick();
          flagTmr1 = 1;
        }
        if (GetMSTick() - tmr1 > 300) {
          tmr1 = GetMSTick();
          ValueDischarge = ValueDischarge - stepPercentDischarge;
        }
      }
    }
  }
}

/**
 * @brief Функция горизонтального копирования. Работает без датчиков расстояния!
 *
 * @param Valves структура клапанов;
 * @param PercentDischarge процент разгрузки.
 */
void Ctrl_OKB600_HorizontalCopy(tOKB600Valve * Valves, uint16_t PercentDischarge) {
  Valves -> fSectionUpValue = 1;
  Valves -> ValveDischargeValue = PercentDischarge * 10;
}

void Check_UsSens(void) {
  const tControl * Ctrl = CtrlGet();
  // Получение напряжения на датчиках
  mA_UsSensLower = GSeMCM_GetAnalogIn(MCM250_7, UsSens1_Out);
  mA_UsSensUpper = GSeMCM_GetAnalogIn(MCM250_7, UsSens2_Out);

  if (mA_UsSensLower > minMASens) {
    Distance_UsSensLower = IOT_NormAI( & Ctrl -> sens.AI.UsSens_1);
    Power_UsSensLower = 1;
  } else {
    Power_UsSensLower = 0;
    Distance_UsSensLower = 0;
  }
  if (mA_UsSensUpper > minMASens) {
    Distance_UsSensUpper = IOT_NormAI( & Ctrl -> sens.AI.UsSens_2);
    Power_UsSensUpper = 1;
  } else {
    Distance_UsSensUpper = 0;
    Power_UsSensUpper = 0;
  }

  FilterDistance_UsSensLower = expRunningAverage((float) Distance_UsSensLower, 0);
  FilterDistance_UsSensUpper = expRunningAverage((float) Distance_UsSensUpper, 1);
  // FilterDistance_UsSensUpper = Distance_UsSensUpper;
  // FilterDistance_UsSensUpper = Distance_UsSensUpper;

  static uint8_t flagErrUpper, flagErrLower = 0;
  if (Power_UsSensLower) {
    EList_ResetErr(ERRID_CHECKUSSENS1);
    flagErrLower = 0;
  } else {
    flagCopyOn = 0;
    if (!flagErrLower) {
      EList_SetErrLevByRCText(VBL_WARNING, ERRID_CHECKUSSENS1, RCTEXT_T_USSENS1_OFFLINE);
      flagErrLower = 1;
    }
  }

  if (Power_UsSensUpper) {
    EList_ResetErr(ERRID_CHECKUSSENS2);
    flagErrUpper = 0;
  } else {
    if (!flagErrUpper) {
      EList_SetErrLevByRCText(VBL_WARNING, ERRID_CHECKUSSENS2, RCTEXT_T_USSENS2_OFFLINE);
      flagCopyOn = 0;
      flagTiltCopy = 0;
      flagErrUpper = 1;
    }
  }

  // Пределы безопасности
  static uint32_t tmrNormalWork_Lower, tmrNormalWork_Upper = 0;
  static uint8_t flagBuzzer_Lower, flagBuzzer_Upper = 0;

  // Нижний датчик работает в пределах расстояния
  if (FilterDistance_UsSensLower > minSaveDistanceCopy && FilterDistance_UsSensLower < maxSaveDistanceCopy) {
    tmrNormalWork_Lower = GetMSTick();
    ErrorSecurityDist_Lower = 0;
    flagBuzzer_Lower = 0;
  }
  // Верхний датчик работает в пределах расстояния
  if (FilterDistance_UsSensUpper > minSaveDistanceCopy && FilterDistance_UsSensUpper < maxSaveDistanceCopy) {
    tmrNormalWork_Upper = GetMSTick();
    ErrorSecurityDist_Upper = 0;
    flagBuzzer_Upper = 0;
  }

  // Нижний датчик вышел за пределы диапозона видимости
  if ((GetMSTick() - tmrNormalWork_Lower > tmrOutLimitDistance_Lower) && flagCopyOn) {
    flagCopyOn = 0;
    ErrorSecurityDist_Lower = 1;
    if (!flagBuzzer_Lower) {
      SetBuzzer(1, 30, 12, 3);
      flagBuzzer_Lower = 1;
    }
  }
  // Верхний датчик вышел за пределы диапозона видимости
  if ((GetMSTick() - tmrNormalWork_Upper > tmrOutLimitDistance_Upper) && flagCopyOn) {
    if (flagTiltCopy) {
      flagCopyOn = 0;
      ErrorSecurityDist_Upper = 1;
    }
    if (!flagBuzzer_Upper) {
      flagCopyOn = 0;
      SetBuzzer(1, 30, 12, 3);
      flagBuzzer_Upper = 1;
    }
  }
  if(!flagTiltCopy) {
    ErrorSecurityDist_Upper = 0;
  }
}

void Ctrl_OKB600_VerticalCopy(tOKB600Valve * Valves, uint16_t DistanceCopy) {
  int16_t DifPos_Lower = DistanceCopy - FilterDistance_UsSensLower; // Разница установки и показаний нижнего датчика
  int16_t DifPos_Upper = DistanceCopy - FilterDistance_UsSensUpper; // Разница установки и показаний верхнего датчика

  // Движение каретки вправо/влево
  if (fabs(DifPos_Lower) > LinearTolerance) {
    if (DifPos_Lower > 0) {
      Valves -> ValveDecreaseValue = PWMBogie;
    } else if (DifPos_Lower < 0) {
      Valves -> ValveZoomValue = PWMBogie;
    }
  }

  // Управление наклоном
  int16_t DifPosTilt = DifPos_Lower - DifPos_Upper;
  if (flagTiltCopy) {
    if (fabs(DifPosTilt) > TiltTolerance) {
      if (DifPosTilt > 0) {
        Valves -> ValveTiltClockwiseValue = PWMTilt;
      } else if (DifPosTilt < 0) {
        Valves -> ValveTiltCounterClockwiseValue = PWMTilt;
      }
    }
  }

  if(FilterDistance_UsSensLower < minSaveDistanceCopy || FilterDistance_UsSensLower > maxSaveDistanceCopy) {
    Valves -> ValveDecreaseValue = 0;
    Valves -> ValveZoomValue = 0;
    Valves -> ValveTiltClockwiseValue = 0;
    Valves -> ValveTiltCounterClockwiseValue = 0;
  }
  if(FilterDistance_UsSensUpper < minSaveDistanceCopy || FilterDistance_UsSensUpper > maxSaveDistanceCopy) {
    if(flagTiltCopy) {
      Valves -> ValveDecreaseValue = 0;
      Valves -> ValveZoomValue = 0;
      Valves -> ValveTiltClockwiseValue = 0;
      Valves -> ValveTiltCounterClockwiseValue = 0;
    }
  }
}

void Ctrl_OKB600_SecurityError(tOKB600Valve * Valves) {
  static uint32_t tmrProcessed = 0;

  if(!ErrorSecurityFlag) {
    ErrorSecurityProcessed = 1;
    if(!tmrProcessed) {
      tmrProcessed = GetMSTick() + tmrSecurityProcessed;
      Valves -> ValveTiltClockwiseValue = 0;
      Valves -> ValveTiltCounterClockwiseValue = 0;
    }
    if(tmrProcessed > GetMSTick()) {
      // SetBuzzer(1, 30, 12, 3);
      Valves -> ValveDecreaseValue = 750;
    } else {
      tmrProcessed = 0;
      ErrorSecurityProcessed = 0;
      ErrorSecurityFlag = 1;
    }
  }
}


void Ctrl_MCM7n8_Check(void) {
  if (flagOKB600_Init) {
    if (!GSeMCM_GetActiveStatus(MCM250_7)) {
      EList_SetErrLevByRCText(VBL_ERROR_CRITICAL, ERRID_MCM250_7_OFFLINE, RCTEXT_T_MCM250_OFFLINE, MCM250_7 + 1);
    } else {
      EList_ResetErr(ERRID_MCM250_7_OFFLINE);
    }
    if (!GSeMCM_GetActiveStatus(MCM250_8)) {
      EList_SetErrLevByRCText(VBL_ERROR_CRITICAL, ERRID_MCM250_8_OFFLINE, RCTEXT_T_MCM250_OFFLINE, MCM250_8 + 1);
    } else {
      EList_ResetErr(ERRID_MCM250_8_OFFLINE);
    }
  }
}

int HorizontalCopyObj[] = {
  OBJ_INDEXBITMAP1308,
  OBJ_ATRESTTTEXT1273,
  OBJ_BARGRAPH1272,
  OBJ_TRUETYPETEXT1276,
  OBJ_TRUETYPEVARIABLE1271,
};

int VerticalCopyObj[] = {
  OBJ_INDEXBITMAP1284,
  OBJ_INDEXBITMAP1285,
  OBJ_ATRESTTTEXTLIST1296,
  OBJ_ATRESTTTEXTLIST1295,
  OBJ_ATRESTTTEXT1287,
  OBJ_ATRESTTTEXT1286,
  OBJ_ATRESTTTEXT1299,
  OBJ_ATRESTTTEXT1298,
  OBJ_ATRESTTTEXT1297,
  OBJ_TRUETYPEVARIABLE1292,
  OBJ_BARGRAPH1290,
  OBJ_ATRESTTTEXT1291,
  OBJ_TRUETYPEVARIABLE1289,
  OBJ_TRUETYPEVARIABLE1288,
};

void Draw_OKB600() {
  // Вывод данных на экран
  static uint8_t flag1, flag2 = 0;
  if (!TypeCopy) {
    if (!flag1) {
      for (uint16_t i = 0; i <= GS_ARRAYELEMENTS(VerticalCopyObj); i++) {
        SendToVisuObj(VerticalCopyObj[i], GS_TO_VISU_SET_ATTR_VISIBLE, 0);
      }
      for (uint16_t i = 0; i <= GS_ARRAYELEMENTS(HorizontalCopyObj); i++) {
        SendToVisuObj(HorizontalCopyObj[i], GS_TO_VISU_SET_ATTR_VISIBLE, 1);
      }
      SendToVisuObj(OBJ_INDEXBITMAP1313, GS_TO_VISU_SET_ATTR_VISIBLE, 0);
      SendToVisuObj(OBJ_SIMPLEBITMAP1280, GS_TO_VISU_SET_ATTR_VISIBLE, 0);
      flag2 = 0;
      flag1 = 1;
    }
  } else {
    if (!flag2) {
      for (uint16_t i = 0; i <= GS_ARRAYELEMENTS(VerticalCopyObj); i++) {
        SendToVisuObj(VerticalCopyObj[i], GS_TO_VISU_SET_ATTR_VISIBLE, 1);
      }
      for (uint16_t i = 0; i <= GS_ARRAYELEMENTS(HorizontalCopyObj); i++) {
        SendToVisuObj(HorizontalCopyObj[i], GS_TO_VISU_SET_ATTR_VISIBLE, 0);
      }
      SendToVisuObj(OBJ_INDEXBITMAP1313, GS_TO_VISU_SET_ATTR_VISIBLE, 1);
      SendToVisuObj(OBJ_SIMPLEBITMAP1280, GS_TO_VISU_SET_ATTR_VISIBLE, 1);
      flag1 = 0;
      flag2 = 1;
    }
  }

  SetVarIndexed(IDX_USSENS_1, FilterDistance_UsSensLower / 10); // Вывод в см
  SetVarIndexed(IDX_USSENS_2, FilterDistance_UsSensUpper / 10); // Вывод в см

  SetVarIndexed(IDX_SETDISTANCECOPY, SetDistanceCopy / 10); // Вывод в см
  SetVarIndexed(IDX_VALUEDISCHARGE, ValueDischarge);

  SetVarIndexed(IDX_FLAGCOPYTILT, flagTiltCopy);

  if (!TypeCopy) {
    SetVarIndexed(IDX_FLAGCOPYON, flagCopyOn);
  } else {
    if (ErrorSecurityDist_Upper || ErrorSecurityDist_Lower) {
      SetVarIndexed(IDX_FLAGCOPYON, 2);
    } else if (!ErrorSecurityDist_Upper && !ErrorSecurityDist_Lower) {
      SetVarIndexed(IDX_FLAGCOPYON, flagCopyOn);
    }
  }

}


//Функция включения ходоуменьшителя и КОМ насоса
void OKB600_Crawl(tOKB600Valve * Valves) {
  const tControl * Ctrl = CtrlGet();
  static uint8_t flagShowMsgBox = 0;
  // Проверка вращения ОКБ и наличия ходоуменьшителя
  if (Brush_Active(&Ctrl->ABC[EQUIP_A].BrushValve) && !Valves->ValveCrawlValue) {
    if (!flagShowMsgBox) {
      MsgBoxOK_RCText(RCTEXT_T_INFO, RCTEXT_T_CRAWL_ROTATE, NULL, NULL);
      flagShowMsgBox = 1;
    }
  } else {
    flagShowMsgBox = 0;
  }

  if (!IsMsgContainerOn(CNT_LIGHTKEYS) && !IsMsgContainerOn(CNT_LIGHTKEYS_2) && IsInfoContainerOn(CNT_OKB600)) {
    if (IsKeyPressedNew(4)) {
      if (Ctrl -> sens.DI.ClutchSensor == 0) { // Выжато ли сцепление
        SetVarIndexed(IDX_CRAWL_PTO, 1 - GetVarIndexed(IDX_CRAWL_PTO));
      } else {
        MsgBoxOK_RCText(RCTEXT_T_INFO, RCTEXT_T_CLUTCH_OFF, NULL, NULL); // Выжми сцепление
      }
    }
  }
  Valves->ValveCrawlValue = GetVarIndexed(IDX_CRAWL_PTO);
  Valves->ValvePTOValue = GetVarIndexed(IDX_CRAWL_PTO);
}
