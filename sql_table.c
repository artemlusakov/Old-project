#include <stdint.h>
#include <UserCEvents.h>
#include <UserCAPI.h>
#include "vartab.h"
#include "objtab.h"
#include "sqlite3.h"
#include "sql_table.h"
#include "gsToVisu.h"
#include "gselist.h"
#include "param.h"
#include "io_types.h"
#include "visu.h"
#include "visu_material.h"
#include "control.h"
#include "gse_msgbox.h"
#include "RCText.h"

typedef int( * SQL_Exec_cb)(void * , int, char ** , char ** );

tVisu_SQL_Table VisuTable;

tGSList TableList = {
  NULL,
  NULL,
  NULL,
  0
};
tSQLTable * ActualTable;

void SQL_fillTableList_fromLocalFile(void);
void SQL_saveTableList_locally(void);

/**
 * @brief This function will interpolate a value between two given values in a table.
 * the first and the last line of the column y will give the maximum and minimum return value.
 *
 * @param Table Pointer to the table.
 * @param ci_x column of the x value
 * @param ci_y column of the y value
 * @param x
 * @return int32_t y = mx + b;
 */
int32_t TableGetLSEValue(tSQLTable * Table, uint32_t ci_x, uint32_t ci_y, int32_t x) {
  //Max and Min:
  if (NULL == Table) {
    return -1;
  }
  //* В пределах
  if (x <= Table -> Cell[0][ci_x]) {
    return Table -> Cell[0][ci_y];
  }
  if (x > Table -> Cell[10][ci_x]) {
    return Table -> Cell[10][ci_y];
  }

  for (int32_t i = 1; i <= 10; i++) {
    if (x <= Table -> Cell[i][ci_x]) {
      if (Table -> Cell[i][ci_x] == Table -> Cell[i - 1][ci_x]) //would be division through 0
        return -1;
      double m = (double)(Table -> Cell[i][ci_y] - Table -> Cell[i - 1][ci_y]) /
        (double)(Table -> Cell[i][ci_x] - Table -> Cell[i - 1][ci_x]);

      double b = (double)(Table -> Cell[i][ci_y]) - m * (double) Table -> Cell[i][ci_x];
      return m * x + b;
    }
  }
  return 0;
}

/**
 * @brief This Function will check if the Table already exists in the SQL-Table.
 *
 * @param Table
 * @return tSQLTable*
 */
tSQLTable * SQL_Tabel_Init(tSQLTable * Table) {
  // чек сохраненных данных в памяти пульта
  if (TableList.first == NULL) {
    if (FileSize(PATH_SQL_FLASH) > 0) {
      SQL_fillTableList_fromLocalFile();
    }
  }

  // Проверка, нет ли такой таблицы
  // Если есть, вернуть указатель на нее для работы
  tSQLTable * tmp_table = GSList_GetFirstData( & TableList);
  while (tmp_table != NULL) {
    if (strcmp(tmp_table -> TableName, Table -> TableName) == 0) {
      return tmp_table;
    }
    tmp_table = GSList_GetNextData( & TableList);
  }

  // Добавление переданной таблицы в список
  GSList_AddData( & TableList, Table, sizeof(tSQLTable));
  SQL_saveTableList_locally();

  return Table;
}

/**
 * @brief This Function adds a Line to the Table. The Table has 4 rows.
 *
 * @param TableView
 * @param line Number of the line.
 * @param Obj_0 Object ID of the table entry, to make it visible
 * @param Hdl_0 Hdl of the Variable, to show the Table Value
 * @param Obj_1 Object ID of the table entry, to make it visible
 * @param Hdl_1 Hdl of the Variable, to show the Table Value
 * @param Obj_2 Object ID of the table entry, to make it visible
 * @param Hdl_2 Hdl of the Variable, to show the Table Value
 * @param Obj_3 Object ID of the table entry, to make it visible
 * @param Hdl_3 Hdl of the Variable, to show the Table Value
 */
void SetTableView_Line(tVisu_SQL_Table * TableView, uint32_t line, uint32_t Obj_0, uint32_t Hdl_0, uint32_t Obj_1, uint32_t Hdl_1, uint32_t Obj_2, uint32_t Hdl_2, uint32_t Obj_3, uint32_t Hdl_3) {
  TableView -> ObjId[line][0] = Obj_0;
  TableView -> Hdl[line][0] = Hdl_0;
  TableView -> ObjId[line][1] = Obj_1;
  TableView -> Hdl[line][1] = Hdl_1;
  TableView -> ObjId[line][2] = Obj_2;
  TableView -> Hdl[line][2] = Hdl_2;
  TableView -> ObjId[line][3] = Obj_3;
  TableView -> Hdl[line][3] = Hdl_3;
}

// Добре
int32_t TableToVisu(tSQLTable * Table) {
  SetVisObjData(OBJ_TABLE, Table -> TableName, strlen(Table -> TableName) + 1);
  for (int32_t i = 0; i < 4; i++) {

    //don't Draw Column, if the Word "EMPTY" is part of the columnname.
    if (NULL == strstr(Table -> ColumnNames[i], "EMPTY")) {

      if (Table -> ColumnNames[i]) {
        SetVisObjData(OBJ_DESCRIPTION_0 + i, Table -> ColumnNames[i], strlen(Table -> ColumnNames[i]) + 1);
      }
      for (int32_t j = 0; j < 11; j++) {
        SendToVisuObj(VisuTable.ObjId[j][i], GS_TO_VISU_SET_ATTR_ALL, ATTR_VISIBLE | ATTR_NOMINAL | ATTR_SMOOTH);
        SetVar(VisuTable.Hdl[j][i], Table -> Cell[j][i]);
      }
    } else {
      //write empty string into column name.
      if (Table -> ColumnNames[i]) {
        SetVisObjData(OBJ_DESCRIPTION_0 + i, " ", strlen(" ") + 1);
      }
      //make Table Entries of this row invisivble
      for (int32_t j = 0; j < 11; j++) {
        SendToVisuObj(VisuTable.ObjId[j][i], GS_TO_VISU_SET_ATTR_ALL, 0);
      }
    }
  }
  return 0;
}

/**
 * @brief Reads the Values from the visu and writes them into the table structure.
 *
 * @param Table Values from visu will be written into this table.
 * @return int32_t
 */
int32_t TableFromVisu(tSQLTable * Table) {
  // for each column
  for (int32_t i = 0; i < 4; i++) {
    if (NULL == strstr(Table -> ColumnNames[i], "EMPTY")) {
      //each cell in the column
      for (int32_t j = 0; j < 11; j++) {
        Table -> Cell[j][i] = GetVar(VisuTable.Hdl[j][i]);
      }
    }
  }
  return 0;
}

void Visu_SQL_Vin_Equip(void) {
  const tParamData * Parameter = ParameterGet();
  SetVisObjData(OBJ_TRUETYPETEXT455, Parameter -> VIN.VIN, strlen(Parameter -> VIN.VIN) + 1);
  SetVisObjData(OBJ_TRUETYPETEXT456, Parameter -> VIN.Equip1, strlen(Parameter -> VIN.Equip1) + 1);
  SetVisObjData(OBJ_TRUETYPETEXT508, Parameter -> VIN.Equip2, strlen(Parameter -> VIN.Equip2) + 1);
}

// Ладно
void Visu_SQL_Table_Init(tVisuData
  const * Data) {
  SetTableView_Line( & VisuTable, 0, OBJ_COL_1_0, HDL_SQL_ZELL_00, OBJ_COL_1_10, HDL_SQL_ZELL_10, OBJ_COL_1_20, HDL_SQL_ZELL_20, OBJ_COL_1_30, HDL_SQL_ZELL_30);
  SetTableView_Line( & VisuTable, 1, OBJ_COL_1_1, HDL_SQL_ZELL_01, OBJ_COL_1_11, HDL_SQL_ZELL_11, OBJ_COL_1_21, HDL_SQL_ZELL_21, OBJ_COL_1_31, HDL_SQL_ZELL_31);
  SetTableView_Line( & VisuTable, 2, OBJ_COL_1_2, HDL_SQL_ZELL_02, OBJ_COL_1_12, HDL_SQL_ZELL_12, OBJ_COL_1_22, HDL_SQL_ZELL_22, OBJ_COL_1_32, HDL_SQL_ZELL_32);
  SetTableView_Line( & VisuTable, 3, OBJ_COL_1_3, HDL_SQL_ZELL_03, OBJ_COL_1_13, HDL_SQL_ZELL_13, OBJ_COL_1_23, HDL_SQL_ZELL_23, OBJ_COL_1_33, HDL_SQL_ZELL_33);
  SetTableView_Line( & VisuTable, 4, OBJ_COL_1_4, HDL_SQL_ZELL_04, OBJ_COL_1_14, HDL_SQL_ZELL_14, OBJ_COL_1_24, HDL_SQL_ZELL_24, OBJ_COL_1_34, HDL_SQL_ZELL_34);
  SetTableView_Line( & VisuTable, 5, OBJ_COL_1_5, HDL_SQL_ZELL_05, OBJ_COL_1_15, HDL_SQL_ZELL_15, OBJ_COL_1_25, HDL_SQL_ZELL_25, OBJ_COL_1_35, HDL_SQL_ZELL_35);
  SetTableView_Line( & VisuTable, 6, OBJ_COL_1_6, HDL_SQL_ZELL_06, OBJ_COL_1_16, HDL_SQL_ZELL_16, OBJ_COL_1_26, HDL_SQL_ZELL_26, OBJ_COL_1_36, HDL_SQL_ZELL_36);
  SetTableView_Line( & VisuTable, 7, OBJ_COL_1_7, HDL_SQL_ZELL_07, OBJ_COL_1_17, HDL_SQL_ZELL_17, OBJ_COL_1_27, HDL_SQL_ZELL_27, OBJ_COL_1_37, HDL_SQL_ZELL_37);
  SetTableView_Line( & VisuTable, 8, OBJ_COL_1_8, HDL_SQL_ZELL_08, OBJ_COL_1_18, HDL_SQL_ZELL_18, OBJ_COL_1_28, HDL_SQL_ZELL_28, OBJ_COL_1_38, HDL_SQL_ZELL_38);
  SetTableView_Line( & VisuTable, 9, OBJ_COL_1_9, HDL_SQL_ZELL_09, OBJ_COL_1_19, HDL_SQL_ZELL_19, OBJ_COL_1_29, HDL_SQL_ZELL_29, OBJ_COL_1_39, HDL_SQL_ZELL_39);
  SetTableView_Line( & VisuTable, 10, OBJ_COL_1_A, HDL_SQL_ZELL_0A, OBJ_COL_1_1A, HDL_SQL_ZELL_1A, OBJ_COL_1_2A, HDL_SQL_ZELL_2A, OBJ_COL_1_3A, HDL_SQL_ZELL_3A);

  Visu_SQL_Vin_Equip();
}

void Visu_SQL_Table_Open(tVisuData
  const * Data) {
  PrioMaskOn(MSK_SQL_LIST);
  ActualTable = GSList_GetFirstData( & TableList);

  if (NULL != ActualTable) {
    TableToVisu(ActualTable);
  }
}

void Table_CalibrateLinear(tSQLTable * Table, uint32_t column) {
  //y = mx + b;
  //m = (y1-y2)/(x1-x2)
  //b = y1-mx1
  double m = (double)(Table -> Cell[0][column] - (Table -> Cell[10][column])) / -10.0;
  double b = (double)(Table -> Cell[0][column]);
  for (int32_t i = 1; i < 11; i++) {
    Table -> Cell[i][column] = (double)(m * (double) i + b);
  }
}

void RTR_Calibration(uint32_t evtc, tUserCEvt * evtv) {
  tControl * Ctrl = CtrlGet();
  tConveyor * Conv = & Ctrl -> top.WinterTop.SpreadConv;
  tWinterTop * wTop = & Ctrl -> top.WinterTop;
  static uint8_t flagShowCtrlArrow = 0;

  uint32_t SpreadingDensity_Step = 0;
  const tReagent * Reag = GetActualReagent();
  if (NULL != Reag) {
    SpreadingDensity_Step = Reag -> SpreadingDensity_Step;
  }

  if (strstr(ActualTable -> TableName, "Conveyor") ||
    strstr(ActualTable -> TableName, "Spreader Disk")) {

    if (IsKeyPressedNew(6)) {
      Ctrl -> cmd_no_key.E_S = 1 - Ctrl -> cmd_no_key.E_S;
    }

    for (uint32_t i = 0; i < evtc; i++) {
      if (CEVT_MENU_INDEX == evtv[i].Type) {
        int16_t obj = evtv[i].Content.mMenuIndex.ObjID;
        if (OBJ_DENSITYUP == obj) {
          wTop -> SpreadDensity = wTop -> SpreadDensity + (1 * SpreadingDensity_Step);
        }
        if (OBJ_DENSITYDOWN == obj) {
          wTop -> SpreadDensity = wTop -> SpreadDensity + (-1 * SpreadingDensity_Step);
        }
        if (OBJ_WIDTHUP == obj) {
          wTop -> SpreadWidth = wTop -> SpreadWidth + 1;
        }
        if (OBJ_WIDTHDOWN == obj) {
          wTop -> SpreadWidth = wTop -> SpreadWidth - 1;
        }
      }
    }
    // Visu
    if (!flagShowCtrlArrow) {
      SendToVisuObj(OBJ_DENSITYUP, GS_TO_VISU_SET_ATTR_VISIBLE, 1);
      SendToVisuObj(OBJ_DENSITYDOWN, GS_TO_VISU_SET_ATTR_VISIBLE, 1);
      SendToVisuObj(OBJ_WIDTHUP, GS_TO_VISU_SET_ATTR_VISIBLE, 1);
      SendToVisuObj(OBJ_WIDTHDOWN, GS_TO_VISU_SET_ATTR_VISIBLE, 1);
      flagShowCtrlArrow = 1;
    }
  } else {
    // Visu
    if (flagShowCtrlArrow) {
      SendToVisuObj(OBJ_DENSITYUP, GS_TO_VISU_SET_ATTR_VISIBLE, 0);
      SendToVisuObj(OBJ_DENSITYDOWN, GS_TO_VISU_SET_ATTR_VISIBLE, 0);
      SendToVisuObj(OBJ_WIDTHUP, GS_TO_VISU_SET_ATTR_VISIBLE, 0);
      SendToVisuObj(OBJ_WIDTHDOWN, GS_TO_VISU_SET_ATTR_VISIBLE, 0);
      flagShowCtrlArrow = 0;
    }
  }

  SetVarIndexed(IDX_RTR_ON, Ctrl -> cmd_no_key.E_S);
  CheckRange((int32_t * )( & (Ctrl -> top.WinterTop.SpreadWidth)), 2, 12);
  CheckRange((int32_t * ) & wTop -> SpreadDensity, 0, ConveyorGetMaxDensity(Conv));
}

void Visu_RTR_PWM(void) {
  const tControl * Ctrl_ = CtrlGet();
  static uint8_t flagShowRTRInfo = 0;
  static uint8_t flagHideRTRInfo = 0;

  SetVar(HDL_RTRINFO, Ctrl_ -> top.WinterTop.SpreadConv.g_minvisu);
  SetVarIndexed(IDX_CONVFREG, Ctrl_ -> sens.Freq.ConveryRotation);

  if (strstr(ActualTable -> TableName, "Conveyor")) {
    SetVar(HDL_SHIMRTR, GetVar(HDL_SHIMCONVEYOR));
    if (flagShowRTRInfo == 0 || flagShowRTRInfo == 2 || flagHideRTRInfo == 1) {
      SendToVisuObj(OBJ_TRUETYPETEXT755, GS_TO_VISU_SET_ATTR_VISIBLE, 1); // PWM
      SendToVisuObj(OBJ_TRUETYPETEXT787, GS_TO_VISU_SET_ATTR_VISIBLE, 1); // Сonv freg
      SendToVisuObj(OBJ_TRUETYPETEXT780, GS_TO_VISU_SET_ATTR_VISIBLE, 1); // Width
      SendToVisuObj(OBJ_TRUETYPETEXT428, GS_TO_VISU_SET_ATTR_VISIBLE, 1); // Density
      SendToVisuObj(OBJ_TRUETYPETEXT784, GS_TO_VISU_SET_ATTR_VISIBLE, 1); // Info
      SendToVisuObj(OBJ_TRUETYPEVARIABLE788, GS_TO_VISU_SET_ATTR_VISIBLE, 1); // PWM val
      SendToVisuObj(OBJ_TRUETYPEVARIABLE756, GS_TO_VISU_SET_ATTR_VISIBLE, 1); // Сonv freg val
      SendToVisuObj(OBJ_TRUETYPEVARIABLE811, GS_TO_VISU_SET_ATTR_VISIBLE, 1); // Width val
      SendToVisuObj(OBJ_TRUETYPEVARIABLE433, GS_TO_VISU_SET_ATTR_VISIBLE, 1); // Density val
      SendToVisuObj(OBJ_TRUETYPEVARIABLE792, GS_TO_VISU_SET_ATTR_VISIBLE, 1); // Info val

      flagHideRTRInfo = 0;
      flagShowRTRInfo = 1;
    }
  } else if (strstr(ActualTable -> TableName, "Spreader Disk")) {
    SetVar(HDL_SHIMRTR, GetVar(HDL_SHIMSPREADERDISK));
    if (flagShowRTRInfo == 1 || flagHideRTRInfo == 1) {
      SendToVisuObj(OBJ_TRUETYPETEXT755, GS_TO_VISU_SET_ATTR_VISIBLE, 1); // PWM
      SendToVisuObj(OBJ_TRUETYPETEXT787, GS_TO_VISU_SET_ATTR_VISIBLE, 0); // Сonv freg
      SendToVisuObj(OBJ_TRUETYPETEXT780, GS_TO_VISU_SET_ATTR_VISIBLE, 1); // Width
      SendToVisuObj(OBJ_TRUETYPETEXT428, GS_TO_VISU_SET_ATTR_VISIBLE, 1); // Density
      SendToVisuObj(OBJ_TRUETYPETEXT784, GS_TO_VISU_SET_ATTR_VISIBLE, 0); // Info
      SendToVisuObj(OBJ_TRUETYPEVARIABLE788, GS_TO_VISU_SET_ATTR_VISIBLE, 1); // PWM val
      SendToVisuObj(OBJ_TRUETYPEVARIABLE756, GS_TO_VISU_SET_ATTR_VISIBLE, 0); // Сonv freg val
      SendToVisuObj(OBJ_TRUETYPEVARIABLE811, GS_TO_VISU_SET_ATTR_VISIBLE, 1); // Width val
      SendToVisuObj(OBJ_TRUETYPEVARIABLE433, GS_TO_VISU_SET_ATTR_VISIBLE, 1); // Density val
      SendToVisuObj(OBJ_TRUETYPEVARIABLE792, GS_TO_VISU_SET_ATTR_VISIBLE, 0); // Info val

      flagHideRTRInfo = 0;
      flagShowRTRInfo = 2;
    }
  } else {
    if (flagHideRTRInfo == 0) {
      SendToVisuObj(OBJ_TRUETYPETEXT755, GS_TO_VISU_SET_ATTR_VISIBLE, 0); // PWM
      SendToVisuObj(OBJ_TRUETYPETEXT787, GS_TO_VISU_SET_ATTR_VISIBLE, 0); // Сonv freg
      SendToVisuObj(OBJ_TRUETYPETEXT780, GS_TO_VISU_SET_ATTR_VISIBLE, 0); // Width
      SendToVisuObj(OBJ_TRUETYPETEXT428, GS_TO_VISU_SET_ATTR_VISIBLE, 0); // Density
      SendToVisuObj(OBJ_TRUETYPETEXT784, GS_TO_VISU_SET_ATTR_VISIBLE, 0); // Info
      SendToVisuObj(OBJ_TRUETYPEVARIABLE788, GS_TO_VISU_SET_ATTR_VISIBLE, 0); // PWM val
      SendToVisuObj(OBJ_TRUETYPEVARIABLE756, GS_TO_VISU_SET_ATTR_VISIBLE, 0); // Сonv freg val
      SendToVisuObj(OBJ_TRUETYPEVARIABLE811, GS_TO_VISU_SET_ATTR_VISIBLE, 0); // Width val
      SendToVisuObj(OBJ_TRUETYPEVARIABLE433, GS_TO_VISU_SET_ATTR_VISIBLE, 0); // Density val
      SendToVisuObj(OBJ_TRUETYPEVARIABLE792, GS_TO_VISU_SET_ATTR_VISIBLE, 0); // Info val
      flagHideRTRInfo = 1;
    }
  }

  // Скрытие VIN, EQUIP на вкладке PVE_Table
  static uint8_t flagShowVin = 0;
  if (strstr(ActualTable -> TableName, "PVE_Valves") || strstr(ActualTable -> TableName, "RotateML")) {
    if (!flagShowVin) {
      SendToVisuObj(OBJ_TRUETYPETEXT457, GS_TO_VISU_SET_ATTR_VISIBLE, 0); // VIN
      SendToVisuObj(OBJ_TRUETYPETEXT458, GS_TO_VISU_SET_ATTR_VISIBLE, 0); // Equip_1
      SendToVisuObj(OBJ_TRUETYPETEXT510, GS_TO_VISU_SET_ATTR_VISIBLE, 0); // Equip_2
      SendToVisuObj(OBJ_TRUETYPETEXT455, GS_TO_VISU_SET_ATTR_VISIBLE, 0); // VIN valv
      SendToVisuObj(OBJ_TRUETYPETEXT456, GS_TO_VISU_SET_ATTR_VISIBLE, 0); // Equip_1 valv
      SendToVisuObj(OBJ_TRUETYPETEXT508, GS_TO_VISU_SET_ATTR_VISIBLE, 0); // Equip_2 valv
      flagShowVin = 1;
    }
  } else {
    if (flagShowVin) {
      SendToVisuObj(OBJ_TRUETYPETEXT457, GS_TO_VISU_SET_ATTR_VISIBLE, 1); // VIN
      SendToVisuObj(OBJ_TRUETYPETEXT458, GS_TO_VISU_SET_ATTR_VISIBLE, 1); // Equip_1
      SendToVisuObj(OBJ_TRUETYPETEXT510, GS_TO_VISU_SET_ATTR_VISIBLE, 1); // Equip_2
      SendToVisuObj(OBJ_TRUETYPETEXT455, GS_TO_VISU_SET_ATTR_VISIBLE, 1); // VIN valv
      SendToVisuObj(OBJ_TRUETYPETEXT456, GS_TO_VISU_SET_ATTR_VISIBLE, 1); // Equip_1 valv
      SendToVisuObj(OBJ_TRUETYPETEXT508, GS_TO_VISU_SET_ATTR_VISIBLE, 1); // Equip_2 valv
      flagShowVin = 0;
    }
  }
}

#define SQL_TEXT_SIZE 64

tSQLTable * SQL_NewTable( void ) {
  tSQLTable * tmp = calloc(sizeof(tSQLTable), 1);
  if (tmp == NULL) {
    return NULL;
  }

  tmp -> TableName = calloc(sizeof(char), SQL_TEXT_SIZE);
  if (tmp -> TableName == NULL) {
    free(tmp);
    return NULL;
  }

  for (int i = 0; i < 4; i++) {
    tmp -> ColumnNames[i] = calloc(sizeof(char), SQL_TEXT_SIZE);

    if (tmp -> ColumnNames[i] == NULL) {
      while (--i >= 0) {
        free(tmp -> ColumnNames[i]);
      }
      free(tmp -> TableName);
      free(tmp);
      return NULL;
    }
  }

  return tmp;
}

void SQL_DestroyTable(tSQLTable * table) {
  if (table == NULL) {
    return;
  }
  if (table -> TableName != NULL) {
    free(table -> TableName);
  }
  if (table -> ColumnNames != NULL) {
    for (int i = 0; i < GS_ARRAYELEMENTS(table -> ColumnNames); i++) {
      free(table -> ColumnNames[i]);
    }
  }

  free(table);
}

enum {
  SQL_HEADER_TABLENAME = 0,
    SQL_HEADER_ROWC,
    SQL_HEADER_COLUMNC,
    SQL_HEADER_COLUMNNAME,
    SQL_HEADER_TABLECOUNT,
    SQL_HEADER_EMPTYCOLUMNNAME,
};
// Заголовки для облегчения чтения файла бэкапа человеком и машиной
char sql_headers[][SQL_TEXT_SIZE] = {
  {
    "Table_name = "
  },
  {
    "Rows = "
  },
  {
    "Columns = "
  },
  {
    "Column_names = "
  },
  {
    "Table_count = "
  },
  {
    "EMPTY"
  }
};

int SQL_NonEmptyColumnsCount(tSQLTable * table) {
  if (table == NULL) {
    return 0;
  }

  int colN = 0;
  for (; colN < 4; colN++) {
    if (strstr(table -> ColumnNames[colN], sql_headers[SQL_HEADER_EMPTYCOLUMNNAME])) {
      break;
    }
  }

  return colN;
}

void SQL_WriteTableToFile(tGsFile * fp, tSQLTable * table) {
  int colN = SQL_NonEmptyColumnsCount(table);

  FilePrintf(fp, "%s%s, ", sql_headers[SQL_HEADER_TABLENAME], table -> TableName);
  FilePrintf(fp, "%s%d, ", sql_headers[SQL_HEADER_ROWC], GS_ARRAYELEMENTS(table -> Cell));
  FilePrintf(fp, "%s%d\r\n", sql_headers[SQL_HEADER_COLUMNC], colN);
  FilePrintf(fp, "%s[%s,%s,%s,%s]\n", sql_headers[SQL_HEADER_COLUMNNAME], table -> ColumnNames[0], table -> ColumnNames[1], table -> ColumnNames[2], table -> ColumnNames[3]);

  for (int i = 0; i < GS_ARRAYELEMENTS(table -> Cell); i++) {
    for (int j = 0; j < colN; j++) {
      FilePrintf(fp, "%d ", table -> Cell[i][j]);
    }
    FilePrintf(fp, "\n");

  }
  FilePrintf(fp, "\n");
}

/**
 * @brief Сохранение списка актуальных таблиц в локальную память
 *
 */
void SQL_saveTableList_locally(void) {
  tGsFile * fp = FileOpen(PATH_SQL_FLASH, "w");

  FilePrintf(fp, "%s%d\r\n", sql_headers[SQL_HEADER_TABLECOUNT], TableList.numItems);

  tSQLTable * tmp_t = GSList_GetFirstData( & TableList);
  while (tmp_t != NULL) {
    SQL_WriteTableToFile(fp, tmp_t);
    tmp_t = GSList_GetNextData( & TableList);
  }

  FileSync(fp);
  FileClose(fp);
}

void WrSQL( void ) {
  SQL_saveTableList_locally();

  CopyFile(PATH_SQL_FLASH, PATH_SQL_USB, NULL);
}

char * SQL_Backup_GetValPtrByHeader(const char * buf,
  const char * header) {
  if (buf == NULL || header == NULL) {
    return NULL;
  }

  char * pos = strstr(buf, header);
  if (pos) {
    return pos + strlen(header);
  }

  return NULL;
}

int SQL_Backup_GetNameSize(const char * buf) {
  if (buf == NULL) {
    return 0;
  }

  char * pos = SQL_Backup_GetValPtrByHeader(buf, sql_headers[SQL_HEADER_TABLENAME]);
  if (pos) {
    return strchr(pos, ',') - pos;
  }

  return 0;
}

int SQL_Backup_GetColNameSize(const char * buf) {
  if (buf == NULL) {
    return 0;
  }

  char * last = strchr(buf, ',');
  if (last) {
    return last - buf;
  }

  last = strchr(buf, ']');
  if (last) {
    return last - buf;
  }

  return 0;
}

#define READ_BUFFER_SIZE 128

int SQL_Backup_ReadTableCount(tGsFile * fp) {
  if (fp == NULL) {
    return -1;
  }

  char buffer[READ_BUFFER_SIZE];
  FileGets(buffer, READ_BUFFER_SIZE, fp);

  char * val = SQL_Backup_GetValPtrByHeader(buffer, sql_headers[SQL_HEADER_TABLECOUNT]);
  if (val == NULL) {
    return -1;
  }

  return atoi(val);
}

tSQLTable * SQL_Backup_ReadTable(tGsFile * fp) {
  if (fp == NULL) {
    return NULL;
  }

  tSQLTable * tmp_t = SQL_NewTable();
  if (tmp_t == NULL) {
    return NULL;
  }

  char buffer[READ_BUFFER_SIZE];
  int rowC = 0;
  int columnC = 0;

  // Жесткий парс имени таблицы и ее размеров
  if (FileGets(buffer, READ_BUFFER_SIZE, fp)) {
    // Чек валидности
    if (strlen(buffer) < strlen(sql_headers[SQL_HEADER_TABLENAME])) {
      if (!FileGets(buffer, READ_BUFFER_SIZE, fp)) {
        SQL_DestroyTable(tmp_t);
        return NULL;
      }
    }

    // Копируем все хорошее, предварительно помолившись
    memcpy(tmp_t -> TableName, SQL_Backup_GetValPtrByHeader(buffer, sql_headers[SQL_HEADER_TABLENAME]), SQL_Backup_GetNameSize(buffer));
    rowC = atoi(SQL_Backup_GetValPtrByHeader(buffer, sql_headers[SQL_HEADER_ROWC]));
    columnC = atoi(SQL_Backup_GetValPtrByHeader(buffer, sql_headers[SQL_HEADER_COLUMNC]));

    // Костыль для пустого имени
    if (strlen(tmp_t -> TableName) < 2) {
      SQL_DestroyTable(tmp_t);
      return NULL;
    }
  }

  // Душный парс имен столбцов таблицы
  if (FileGets(buffer, READ_BUFFER_SIZE, fp)) {
    // Мощный чек валидности
    if (strlen(buffer) < strlen(sql_headers[SQL_HEADER_COLUMNNAME])) {
      SQL_DestroyTable(tmp_t);
      return NULL;
    }

    // Чек + поиск = МОЩНИЩЩЕ
    char * tmp_c = SQL_Backup_GetValPtrByHeader(buffer, sql_headers[SQL_HEADER_COLUMNNAME]);
    if (tmp_c == NULL) {
      SQL_DestroyTable(tmp_t);
      return NULL;
    }

    // Скромный ++ из-за открывающей квадратной скобки и чтение имен столбцов
    tmp_c++;
    int i = 0;
    for (; i < columnC; i++) {
      int colNamesize = SQL_Backup_GetColNameSize(tmp_c);
      memcpy(tmp_t -> ColumnNames[i], tmp_c, colNamesize);
      tmp_c += colNamesize + 1;
    }
    for (; i < GS_ARRAYELEMENTS(tmp_t -> ColumnNames); i++) {
      memcpy(tmp_t -> ColumnNames[i], sql_headers[SQL_HEADER_EMPTYCOLUMNNAME], strlen(sql_headers[SQL_HEADER_EMPTYCOLUMNNAME]));
    }
  }

  for (int i = 0; i < rowC; i++) {
    if (!FileGets(buffer, READ_BUFFER_SIZE, fp)) {
      break;
    }
    if (strlen(buffer) < columnC) {
      break;
    }

    char * tmp_c = buffer;
    for (int j = 0; j < columnC; j++) {
      tmp_t -> Cell[i][j] = atoi(tmp_c);
      tmp_c = strchr(tmp_c + 1, ' ');
      if (tmp_c == NULL) {
        break;
      }
    }
  }

  return tmp_t;
}

/**
 * @brief Очистка списка актульных таблиц
 *
 */
void SQL_freeTableList(void) {
  tSQLTable * tmp_table = GSList_GetFirstData( & TableList);
  while (tmp_table != NULL) {
    GSList_RemoveData( & TableList, tmp_table);
    tmp_table = GSList_GetFirstData( & TableList);
  }
  TableList.act = TableList.first = TableList.last = NULL;
  TableList.numItems = 0;
}

/**
 * @brief Заполнение спика актуальных таблиц из локального файла
 *
 */
void SQL_fillTableList_fromLocalFile(void) {
  tGsFile * fp = FileOpen(PATH_SQL_FLASH, "r");
  if (fp == NULL) {
    return;
  }

  int table_count = SQL_Backup_ReadTableCount(fp);
  for (int i = 0; i < table_count; i++) {
    tSQLTable * tmp_table = SQL_Backup_ReadTable(fp);
    if (tmp_table == NULL) {
      break;
    }

    GSList_AddData( & TableList, tmp_table, sizeof(tSQLTable));
  }

  FileClose(fp);
}

void RpSQL( void ) {
  if (FileSize(PATH_SQL_USB) > 0) {
    CopyFile(PATH_SQL_USB, PATH_SQL_FLASH, NULL);

    SQL_freeTableList();
    SQL_fillTableList_fromLocalFile();

    // переписывание локального файла на случай, если что нелегальное прошло
    if (TableList.first != NULL) {
      SQL_saveTableList_locally();
    }
  } else {
    char stringinf[100];
    snprintf(stringinf, GS_ARRAYELEMENTS(stringinf), "%s %s", RCTextGetText(RCTEXT_T_FILENOTFOUND, GetVarIndexed(IDX_SYS_LANGUAGE)), "SQLList.cfg");
    MsgBoxOk(RCTextGetText(RCTEXT_T_INFO, GetVarIndexed(IDX_SYS_LANGUAGE)), stringinf, NULL, NULL);
  }
}

void Visu_SQL_Table_Cycle(const tVisuData * Data, uint32_t evtc, tUserCEvt * evtv) {
  Visu_RTR_PWM();
  RTR_Calibration(evtc, evtv);

  //previous table
  if (IsKeyPressedNew(1)) {
    tSQLTable * NewTable = GSList_GetPrevData( & TableList);
    if (NULL == NewTable) {
      NewTable = GSList_GetLastData( & TableList);
    }
    ActualTable = NewTable;
    TableToVisu(ActualTable);
  }
  //NextTable
  if (IsKeyPressedNew(2)) {
    tSQLTable * NewTable = GSList_GetNextData( & TableList);
    if (NULL == NewTable) {
      NewTable = GSList_GetFirstData( & TableList);
    }
    ActualTable = NewTable;
    TableToVisu(ActualTable);
  }
  //Save
  if (IsKeyPressedNew(3)) {
    TableFromVisu(ActualTable);
    SQL_saveTableList_locally();

    tSQLTable * NewTable = GSList_GetFirstData( & TableList);
    while (NewTable != NULL) {
      if (NewTable == ActualTable) {
        break;
      }
      NewTable = GSList_GetNextData( & TableList);
    }
  }
  //linear calibration
  if (IsKeyPressedNew(4)) {
    TableFromVisu(ActualTable);
    Table_CalibrateLinear(ActualTable, 1);

    TableToVisu(ActualTable);
  }
  if (IsKeyPressedNew(5)) {
    TableFromVisu(ActualTable);
    Table_CalibrateLinear(ActualTable, 2);

    TableToVisu(ActualTable);
  }

  Visu_HomeKeyPressed();

}