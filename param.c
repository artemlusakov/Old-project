#include <stdint.h>
#include <UserCEvents.h>  /* Events send to the Cycle function              */
#include <UserCAPI.h>     /* API-Function declarations                      */

#include "gs_easy_config.h"

#include "param.h"


tParamData Parameter;

/**
* @brief This array defines the parameter file. All entries will be saved in a file according to the tag.
*/
tEasyConfigEntry ParamArray[] = {
	// Engine
	{.CType = EASY_CONFIG_TYPE_INT, .Entry = &Parameter.Engine.PTOatEngine, .Default = "1", .Tag = "Engine.PTO"},

	// VIN
  {.CType = EASY_CONFIG_TYPE_STRING, .Entry = &Parameter.VIN.VIN, .Default = "0", .Tag = "VIN.VIN"},
	{.CType = EASY_CONFIG_TYPE_STRING, .Entry = &Parameter.VIN.Equip1, .Default = "0", .Tag = "VIN.Equip1"},
	{.CType = EASY_CONFIG_TYPE_STRING, .Entry = &Parameter.VIN.Equip2, .Default = "0", .Tag = "VIN.Equip2"},
	{.CType = EASY_CONFIG_TYPE_INT, .Entry = &Parameter.VIN.ActualHydDistributor, .Default = "0", .Tag = "VIN.ActualHydDistributor"},

	// Speedometer
  {.CType = EASY_CONFIG_TYPE_INT, .Entry = &Parameter.Speedometer.TypeSpeedMsg, .Default = "0", .Tag = "Speedometer.TypeSpeedMsg"},
  {.CType = EASY_CONFIG_TYPE_INT, .Entry = &Parameter.Speedometer.Impulses, .Default = "0", .Tag = "Speedometer.Impulses"},

	// OMFB
	{.CType = EASY_CONFIG_TYPE_INT, .Entry = &Parameter.OMFB.Invert_UpDown_A, .Default = "0", .Tag = "OMFB.Invert_UpDown_A"},
	{.CType = EASY_CONFIG_TYPE_INT, .Entry = &Parameter.OMFB.Invert_UpDown_B, .Default = "0", .Tag = "OMFB.Invert_UpDown_B"},
	{.CType = EASY_CONFIG_TYPE_INT, .Entry = &Parameter.OMFB.Invert_UpDown_C, .Default = "0", .Tag = "OMFB.Invert_UpDown_C"},

	{.CType = EASY_CONFIG_TYPE_INT, .Entry = &Parameter.OMFB.PWM_UpDown_A, .Default = "380", .Tag = "OMFB.PWM_UpDown_A"},
	{.CType = EASY_CONFIG_TYPE_INT, .Entry = &Parameter.OMFB.PWM_UpDown_B, .Default = "380", .Tag = "OMFB.PWM_UpDown_B"},
	{.CType = EASY_CONFIG_TYPE_INT, .Entry = &Parameter.OMFB.PWM_UpDown_C, .Default = "380", .Tag = "OMFB.PWM_UpDown_C"},

	{.CType = EASY_CONFIG_TYPE_INT, .Entry = &Parameter.OMFB.PWM_RTR, .Default = "450", .Tag = "OMFB.PWM_RTR"},
	{.CType = EASY_CONFIG_TYPE_INT, .Entry = &Parameter.OMFB.PWM_C610H, .Default = "500", .Tag = "OMFB.PWM_C610H"},
	{.CType = EASY_CONFIG_TYPE_INT, .Entry = &Parameter.OMFB.PWM_BP300, .Default = "500", .Tag = "OMFB.PWM_BP300"},

	{.CType = EASY_CONFIG_TYPE_INT, .Entry = &Parameter.OMFB.PWM_Rotate_OMB1, .Default = "500", .Tag = "OMFB.PWM_Rotate_OMB1"},
	{.CType = EASY_CONFIG_TYPE_INT, .Entry = &Parameter.OMFB.PWM_Rotate_OMT1, .Default = "500", .Tag = "OMFB.PWM_Rotate_OMT1"},
	{.CType = EASY_CONFIG_TYPE_INT, .Entry = &Parameter.OMFB.PWM_Rotate_OPF245, .Default = "500", .Tag = "OMFB.PWM_Rotate_OPF245"},
	{.CType = EASY_CONFIG_TYPE_INT, .Entry = &Parameter.OMFB.PWM_Rotate_OMP, .Default = "500", .Tag = "OMFB.PWM_Rotate_OMP"},
	{.CType = EASY_CONFIG_TYPE_INT, .Entry = &Parameter.OMFB.PWM_Rotate_OZP231, .Default = "500", .Tag = "OMFB.PWM_Rotate_OZP231"},

	// EquipTimers
  {.CType = EASY_CONFIG_TYPE_INT, .Entry = &Parameter.EquipTimers.InterUp_timer, .Default = "1500", .Tag = "EquipTimers.InterUp_timer"},
  {.CType = EASY_CONFIG_TYPE_INT, .Entry = &Parameter.EquipTimers.InterDown_timer, .Default = "2000", .Tag = "EquipTimers.InterDown_timer"},
  {.CType = EASY_CONFIG_TYPE_INT, .Entry = &Parameter.EquipTimers.DelayInterRotate_timer, .Default = "1000", .Tag = "EquipTimers.DelayInterRotate_timer"},

	// SideAutoPark
  {.CType = EASY_CONFIG_TYPE_INT, .Entry = &Parameter.SideAutoPark.SideAutoParkGlobalTimer, .Default = "16000", .Tag = "SideAutoPark.SideAutoParkGlobalTimer"},
	{.CType = EASY_CONFIG_TYPE_INT, .Entry = &Parameter.SideAutoPark.TimerUp, .Default = "5000", .Tag = "SideAutoPark.TimerUp"},
	{.CType = EASY_CONFIG_TYPE_INT, .Entry = &Parameter.SideAutoPark.TimerTiltUp, .Default = "5000", .Tag = "SideAutoPark.TimerTiltUp"},
	{.CType = EASY_CONFIG_TYPE_INT, .Entry = &Parameter.SideAutoPark.TimerLeft, .Default = "5000", .Tag = "SideAutoPark.TimerLeft"},

	// Sensors
	{.CType = EASY_CONFIG_TYPE_INT, .Entry = &Parameter.Sensors.OilTemp.x1, .Default = "400", .Tag = "Sensors.OilTemperature.mA_min"},
	{.CType = EASY_CONFIG_TYPE_INT, .Entry = &Parameter.Sensors.OilTemp.x2, .Default = "2000", .Tag = "Sensors.OilTemperature.mA_max"},
	{.CType = EASY_CONFIG_TYPE_INT, .Entry = &Parameter.Sensors.OilTemp.y1, .Default = "0", .Tag = "Sensors.OilTemperature.T_min"},
	{.CType = EASY_CONFIG_TYPE_INT, .Entry = &Parameter.Sensors.OilTemp.y2, .Default = "200", .Tag = "Sensors.OilTemperature.T_max"},
	{.CType = EASY_CONFIG_TYPE_INT, .Entry = &Parameter.Sensors.OilPressure.x1, .Default = "400", .Tag = "Sensors.OilPressure.V_min"},
	{.CType = EASY_CONFIG_TYPE_INT, .Entry = &Parameter.Sensors.OilPressure.x2, .Default = "2000", .Tag = "Sensors.OilPressure.V_max"},
	{.CType = EASY_CONFIG_TYPE_INT, .Entry = &Parameter.Sensors.OilPressure.y1, .Default = "0", .Tag = "Sensors.OilPressure.mBar_min"},
	{.CType = EASY_CONFIG_TYPE_INT, .Entry = &Parameter.Sensors.OilPressure.y2, .Default = "250", .Tag = "Sensors.OilPressure.mBar_max"},
	{.CType = EASY_CONFIG_TYPE_INT, .Entry = &Parameter.Sensors.UsSens_1.x1, .Default = "400", .Tag = "Sensors.UsSens_1.mA_min"},
	{.CType = EASY_CONFIG_TYPE_INT, .Entry = &Parameter.Sensors.UsSens_1.x2, .Default = "2000", .Tag = "Sensors.UsSens_1.mA_max"},
	{.CType = EASY_CONFIG_TYPE_INT, .Entry = &Parameter.Sensors.UsSens_1.y1, .Default = "200", .Tag = "Sensors.UsSens_1.Dist_min"},
	{.CType = EASY_CONFIG_TYPE_INT, .Entry = &Parameter.Sensors.UsSens_1.y2, .Default = "1500", .Tag = "Sensors.UsSens_1.Dist_max"},
	{.CType = EASY_CONFIG_TYPE_INT, .Entry = &Parameter.Sensors.UsSens_2.x1, .Default = "400", .Tag = "Sensors.UsSens_2.mA_min"},
	{.CType = EASY_CONFIG_TYPE_INT, .Entry = &Parameter.Sensors.UsSens_2.x2, .Default = "2000", .Tag = "Sensors.UsSens_2.mA_max"},
	{.CType = EASY_CONFIG_TYPE_INT, .Entry = &Parameter.Sensors.UsSens_2.y1, .Default = "200", .Tag = "Sensors.UsSens_2.Dist_min"},
	{.CType = EASY_CONFIG_TYPE_INT, .Entry = &Parameter.Sensors.UsSens_2.y2, .Default = "1500", .Tag = "Sensors.UsSens_2.Dist_max"},
	{.CType = EASY_CONFIG_TYPE_INT, .Entry = &Parameter.Sensors.NormOpen, .Default = "1", .Tag = "Sensors.LiquidPresence.NormOpen"},
	{.CType = EASY_CONFIG_TYPE_INT, .Entry = &Parameter.Sensors.NormClosed, .Default = "0", .Tag = "Sensors.LiquidPresence.NormClosed"},

	// RTR
	{.CType = EASY_CONFIG_TYPE_INT, .Entry = &Parameter.RTR.TypeRTR, .Default = "2", .Tag = "RTR.TypeRTR"},
	{.CType = EASY_CONFIG_TYPE_INT, .Entry = &Parameter.RTR.AsymType, .Default = "1", .Tag = "RTR.AsymType"},
	{.CType = EASY_CONFIG_TYPE_INT, .Entry = &Parameter.RTR.HumidType, .Default = "1", .Tag = "RTR.HumidType"},

	// Joystick
	{.CType = EASY_CONFIG_TYPE_INT, .Entry = &Parameter.Joystick.Id, .Default = "0x3", .Tag = "Joystick.ID"},
  {.CType = EASY_CONFIG_TYPE_INT, .Entry = &Parameter.Joystick.Id2, .Default = "0x1", .Tag = "Joystick.ID2"},

	// AsymDrive
	{.CType = EASY_CONFIG_TYPE_INT, .Entry = &Parameter.AsymDrive.I_rightStop, .Default = "2300", .Tag = "AsymetricDriveElCur.I_rightStop"},
  {.CType = EASY_CONFIG_TYPE_INT, .Entry = &Parameter.AsymDrive.I_leftStop, .Default = "2300",  .Tag = "AsymetricDriveElCur.I_leftStop"},
  {.CType = EASY_CONFIG_TYPE_INT, .Entry = &Parameter.AsymDrive.timeFullRight,.Default = "3920", .Tag = "AsymetricDrive.timeFullRight"},
  {.CType = EASY_CONFIG_TYPE_INT, .Entry = &Parameter.AsymDrive.timeFullLeft, .Default =  "4000",.Tag = "AsymetricDrive.timeFullLeft"},
  {.CType = EASY_CONFIG_TYPE_INT, .Entry = &Parameter.AsymDrive.timeCntrlMoveCalibrationStop, .Default =  "250",.Tag = "AsymetricDrive.timeCntrlMoveCalibrationStop"},
  {.CType = EASY_CONFIG_TYPE_INT, .Entry = &Parameter.AsymDrive.timeCalibrationAside, .Default =  "5000",.Tag = "AsymetricDrive.timeCalibrationAside"},
  {.CType = EASY_CONFIG_TYPE_INT, .Entry = &Parameter.AsymDrive.timeCntrlMoveStop,    .Default =  "120",.Tag = "AsymetricDrive.timeCntrlMoveStop"},
  {.CType = EASY_CONFIG_TYPE_INT, .Entry = &Parameter.AsymDrive.timeCntrlErrorStop, .Default =  "300",.Tag = "AsymetricDrive.timeCntrlErrorStop"},
  {.CType = EASY_CONFIG_TYPE_INT, .Entry = &Parameter.AsymDrive.cntrlMove, .Default =  "1",.Tag = "AsymetricDrive.cntrlMove"},
  {.CType = EASY_CONFIG_TYPE_INT, .Entry = &Parameter.AsymDrive.timeReversEnable, .Default =  "50",.Tag = "AsymetricDrive.timeReversEnable"},
  {.CType = EASY_CONFIG_TYPE_INT, .Entry = &Parameter.AsymDrive.timeReaction, .Default =  "500",.Tag = "AsymetricDrive.timeReaction"},

	// AsymDriveFeedback
  {.CType = EASY_CONFIG_TYPE_INT, .Entry = &Parameter.AsymDriveFeedback.I_rightStop, .Default =  "30",.Tag = "AsymDriveFeedback.I_rightStop"},
  {.CType = EASY_CONFIG_TYPE_INT, .Entry = &Parameter.AsymDriveFeedback.I_leftStop, .Default =  "30",.Tag = "AsymDriveFeedback.I_leftStop"},
  {.CType = EASY_CONFIG_TYPE_INT, .Entry = &Parameter.AsymDriveFeedback.RightAmperage, .Default =  "30",.Tag = "AsymDriveFeedback.RightAmperage"},
  {.CType = EASY_CONFIG_TYPE_INT, .Entry = &Parameter.AsymDriveFeedback.LeftAmperage, .Default =  "800",.Tag = "AsymDriveFeedback.LeftAmperage"},
  {.CType = EASY_CONFIG_TYPE_INT, .Entry = &Parameter.AsymDriveFeedback.AsymAccuracy, .Default =  "30",.Tag = "AsymDriveFeedback.AsymAccuracy"},

	// FrontBrush
	{.CType = EASY_CONFIG_TYPE_INT, .Entry = &Parameter.FrontBrush.NoHumid, .Default = "0", .Tag = "FrontBrush.NoHumid"},
	{.CType = EASY_CONFIG_TYPE_INT, .Entry = &Parameter.FrontBrush.HumidFromRoration, .Default = "0", .Tag = "FrontBrush.HumidFromRoration"},

};

typedef struct _Translate{
	char* src;
  char* trans;
} Translate;

const Translate TransParam[] = {
	// Общее
	{"Type", "Тип"},

	// Engine
  {"Engine", "Двигатель"},
  {"PTO", "Тип отбора мощности"},

	// VIN
	{"VIN", "VIN"},
	{"Equip1","VIN установки №1"},
  {"Equip2","VIN установки №2"},
  {"ActualHydDistributor","ID гидрораспределителя"},

	//Speedometer
	{"Speedometer", "Спидометр"},
	{"TypeSpeedMsg", "Тип кан-сообщения"},
	{"Impulses", "Импульсов на км/ч"},

	// Joystick
  {"Joystick", "Джойстик"},
  {"ID", "ID"},
  {"ID2", "ID2"},

	// Sensors
  {"Sensors", "Датчики"},
  {"OilTemperature", "Датчик температуры масла"},
  {"OilPressure", "Датчик давления масла"},
  {"UsSens_1", "Датчик ультразвуковой №1"},
  {"UsSens_2", "Датчик ультразвуковой №2"},
  {"LiquidPresence", "Датчик наличия жидкости"},
  {"NormOpen", "Нормально открытый"},
  {"NormClosed", "Нормально закрытый"},

  {"mA_min", "Минимальный ток, мА"},
  {"mA_max", "Максимальный ток, мА"},
  {"T_min", "Минимальная температура, °C"},
  {"T_max", "Максимальная температура, °C"},
  {"V_min", "Минимальное напряжение, V"},
  {"V_max", "Максимальное напряжение, V"},
  {"mBar_min", "Минимальное давление, Bar"},
  {"mBar_max", "Максимальное давление, Bar"},
  {"Dist_min", "Минимальная дистанция, мм"},
  {"Dist_max", "Максимальная дистанция, мм"},

	// RTR
	{"RTR", "Распределитель твердых реагентов"},
  {"AsymType", "Тип асимметрии"},
  {"HumidType", "Тип увлажнения"},
  {"TypeRTR", "Тип ПС"},

	// AsymDrive
  {"AsymetricDrive", "Асимметрия SH/UH_Точная настройка"},
  {"AsymetricDriveElCur", "Асимметрия SH/UH_Токи упора"},

  {"I_rightStop", "Ток упора справа"},
  {"I_leftStop", "Ток упора слева"},
  {"timeFullRight", "Время движения с левого к правому крайнему положению"},
  {"timeFullLeft", "Время движения с правого к левому крайнему положению"},
  {"timeCntrlMoveCalibrationStop", "Калибровка. Время проверки превышения по току"},
  {"timeCalibrationAside", "Калибровка. Макс. время движение по крайним положениям"},
  {"timeCntrlMoveStop", "Время проверки превышения по току"},
  {"timeCntrlErrorStop", "Макс. время движение привода"},
  {"cntrlMove", "Режим работы привода асимметрии"},
  {"timeReversEnable", "Время контрвключения"},
  {"timeReaction", "Задержка движения привода"},

	// AsymDriveFeedback
	{"AsymDriveFeedback", "Асимметрия Соргаз"},
	{"RightAmperage", "Напряжение тока при правом упоре"},
	{"LeftAmperage", "Напряжение тока при левом упоре"},
	{"AsymAccuracy", "Погрешность положения штока"},

	// EquipTimers
	{"EquipTimers", "Таймеры"},
	{"InterUp_timer", "Подъем межосевой щетки, мс"},
	{"InterDown_timer", "CH2800 опускание вниз при плавании, мс"},
	{"DelayInterRotate_timer", "Задержка вкл. вращения межосевой щетки, мс"},

	// SideAutoPark
	{"SideAutoPark", "Автопарковка бокового отвала"},
	{"SideAutoParkGlobalTimer", "Общее время автопарковки, мс"},
	{"TimerUp", "Время подъема, мс"},
	{"TimerTiltUp", "Время подъема наклона, мс"},
	{"TimerLeft", "Время складывания, мс"},

	// OMFB
	{"OMFB", "Гидрораспределитель OMFB"},
	{"Invert_UpDown_A", "Инверсия подъема/опускания переднего оборудования"},
	{"Invert_UpDown_B", "Инверсия подъема/опускания межосевого оборудования"},
	{"Invert_UpDown_C", "Инверсия подъема/опускания бокового оборудования"},
	{"PWM_UpDown_A", "ШИМ подъема/опускания переднего оборудования"},
	{"PWM_UpDown_B", "ШИМ подъема/опускания межосевого оборудования"},
	{"PWM_UpDown_C", "ШИМ подъема/опускания бокового оборудования"},
	{"PWM_RTR", "ШИМ ПС"},
	{"PWM_C610H", "ШИМ C610H"},
	{"PWM_BP300", "ШИМ BP300"},
	{"PWM_Rotate_OPF245", "ШИМ вращения OPF245"},
	{"PWM_Rotate_OMB1", "ШИМ вращения OMB1"},
	{"PWM_Rotate_OMT1", "ШИМ вращения OMT1"},
	{"PWM_Rotate_OMP", "ШИМ вращения OMP"},
	{"PWM_Rotate_OZP231", "ШИМ вращения OZP231"},

	// FrontBrush
	{"FrontBrush", "Передняя щётка"},
	{"NoHumid", "Отсутствие увлажнения"},
	{"HumidFromRoration", "Увлажнение от вращения"},

};


void InitParam( void ) {
	//Read parameters.
	if( 0 != gsEasyConfig_Read( ParamArray, GS_ARRAYELEMENTS( ParamArray ), PATH_PARAM_FLASH ) ) {
		//if reading parameters failed, write parameters and use the default value, if a parameter doesn't exist.
		gsEasyConfig_Write( ParamArray, GS_ARRAYELEMENTS( ParamArray ), PATH_PARAM_FLASH );
	}
}

const tParamData* ParameterGet( void ) {
	return &Parameter;
}

 tParamData* GetParameter( void ) {
    return &Parameter;
}

tEasyConfigEntry * Param_GetECE( void ) {
	return ParamArray;
}
int Param_GetECE_Size( void ) {
	return GS_ARRAYELEMENTS( ParamArray );
}

void writeParameters(void) {
  gsEasyConfig_Write( ParamArray, GS_ARRAYELEMENTS( ParamArray ), PATH_PARAM_FLASH );
}

char * translateParam(char * Src) {
  int l = GS_ARRAYELEMENTS(TransParam);
  for (size_t i = 0; i < l; i++) {
    if ( * Src == * TransParam[i].src) {
      if (strncmp(Src, TransParam[i].src, strlen(Src)) == 0) {
				return TransParam[i].trans;
      }
    }
  }
	return Src;
}