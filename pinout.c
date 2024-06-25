/****************************************************************************
 *
 * Project:   name
 *
 * @file      filename.c
 * @author    author
 * @date      [Creation date in format %02d.%02d.20%02d]
 *
 * @brief     description
 *
 ****************************************************************************/

#include <stdint.h>
#include <UserCEvents.h>  /* Events send to the Cycle function              */
#include <UserCAPI.h>     /* API-Function declarations                      */

#include "vartab.h"       /* Variable Table definitions:                    */
                          /* include this file in every C-Source to access  */
                          /* the variable table of your project.            */
                          /* vartab.h is generated automatically.           */

#include "objtab.h"       /* Object ID definitions:                         */
                          /* include this file in every C-Source to access  */
                          /* the object ID's of the visual objects.         */
                          /* objtab.h is generated automatically.           */
#include "visu_pinout.h"
#include "pinout.h"
#include "param.h"
#include "config/libconfig.h"
#include "gse_msgbox.h"
#include "gselist.h"
#include "RCText.h"

/* macro definitions ********************************************************/

/* type definitions *********************************************************/

#define MCM_None 0
#define MCM250_1 1
#define MCM250_2 2
#define MCM250_3 3
#define MCM250_4 4
#define MCM250_5 5

/* prototypes ***************************************************************/

/* global constants *********************************************************/

/* global variables *********************************************************/

/* local function prototypes ************************************************/

/* functions ****************************************************************/

int DigitalOut_PinIDX[] = { 0,1,2,3,4,5,6,7,8,9,10};
int DigitalOut_Out[]    = {-1,0,2,1,3,5,7,4,6,8,9};

/**
 * @brief Актуальная распиновка
 * // PinOut_RG _ PinOut_OMFB _ PinOut_Danfoss _ AllPinOut _ PinOut_OMFBv2
 */
tPinOut AllPinOut[] = {
  //Front
  {.NameAction = "Front_UpDown",    .MCM_A = MCM250_2, .Pin_A = Pin_5, .MCM_B = MCM250_2, .Pin_B = Pin_3, .ValveType = Valve_PVEH_OMFB}, // 0
  {.NameAction = "Front_LeftRight", .MCM_A = MCM250_1, .Pin_A = Pin_28, .MCM_B = MCM250_1, .Pin_B = Pin_29, .ValveType = Valve_PVEO_2},
  {.NameAction = "Front_Float",     .MCM_A = MCM_None, .Pin_A = Pin_None, .MCM_B = MCM_None, .Pin_B = Pin_None, .ValveType = Valve_PVEO_1},
  {.NameAction = "Front_Lock",      .MCM_A = MCM250_1, .Pin_A = Pin_4, .MCM_B = MCM_None, .Pin_B = Pin_None, .ValveType = Valve_PVEO_1},
  {.NameAction = "Front_SteelRuber",.MCM_A = MCM250_2, .Pin_A = Pin_28, .MCM_B = MCM250_2, .Pin_B = Pin_29, .ValveType = Valve_PVEO_2},
  {.NameAction = "Front_Unload",    .MCM_A = MCM_None, .Pin_A = Pin_None, .MCM_B = MCM_None, .Pin_B = Pin_None, .ValveType = Valve_PVEH_16},
  {.NameAction = "Front_Press",     .MCM_A = MCM_None, .Pin_A = Pin_None, .MCM_B = MCM_None, .Pin_B = Pin_None, .ValveType = Valve_PVEH_16},
  {.NameAction = "Front_Rotate",    .MCM_A = MCM250_2, .Pin_A = Pin_8, .MCM_B = MCM_None, .Pin_B = Pin_None, .ValveType = Valve_PVEH_OMFB},
  {.NameAction = "Front_Tilt",      .MCM_A = MCM_None, .Pin_A = Pin_None, .MCM_B = MCM_None, .Pin_B = Pin_None, .ValveType = Valve_PVEO_1}, //8
  //Inter
  {.NameAction = "Inter_UpDown",    .MCM_A = MCM250_2, .Pin_A = Pin_6, .MCM_B = MCM250_2, .Pin_B = Pin_4, .ValveType = Valve_PVEH_OMFB},
  {.NameAction = "Inter_LeftRight", .MCM_A = MCM_None, .Pin_A = Pin_None, .MCM_B = MCM_None, .Pin_B = Pin_None, .ValveType = Valve_PVEO_2},
  {.NameAction = "Inter_Float",     .MCM_A = MCM_None, .Pin_A = Pin_None, .MCM_B = MCM_None, .Pin_B = Pin_None, .ValveType = Valve_PVEO_1},
  {.NameAction = "Inter_Lock",      .MCM_A = MCM250_1, .Pin_A = Pin_3, .MCM_B = MCM_None, .Pin_B = Pin_None, .ValveType = Valve_PVEO_1},
  {.NameAction = "Inter_SteelRuber",.MCM_A = MCM_None, .Pin_A = Pin_None, .MCM_B = MCM_None, .Pin_B = Pin_None, .ValveType = Valve_PVEO_2},
  {.NameAction = "Inter_Unload",    .MCM_A = MCM_None, .Pin_A = Pin_None, .MCM_B = MCM_None, .Pin_B = Pin_None, .ValveType = Valve_PVEH_16},
  {.NameAction = "Inter_Press",     .MCM_A = MCM_None, .Pin_A = Pin_None, .MCM_B = MCM_None, .Pin_B = Pin_None, .ValveType = Valve_PVEH_16},
  {.NameAction = "Inter_Rotate",    .MCM_A = MCM250_2, .Pin_A = Pin_7, .MCM_B = MCM_None, .Pin_B = Pin_None, .ValveType = Valve_PVEH_OMFB},
  {.NameAction = "Inter_Tilt",      .MCM_A = MCM_None, .Pin_A = Pin_None, .MCM_B = MCM_None, .Pin_B = Pin_None, .ValveType = Valve_PVEH_16},
  //Side
  {.NameAction = "Side_UpDown",    .MCM_A = MCM250_1, .Pin_A = Pin_5, .MCM_B = MCM250_1, .Pin_B = Pin_10, .ValveType = Valve_PVEH_OMFB},
  {.NameAction = "Side_LeftRight", .MCM_A = MCM250_2, .Pin_A = Pin_9, .MCM_B = MCM250_2, .Pin_B = Pin_10, .ValveType = Valve_PVEO_2},
  {.NameAction = "Side_Float",     .MCM_A = MCM_None, .Pin_A = Pin_None, .MCM_B = MCM_None, .Pin_B = Pin_None, .ValveType = Valve_PVEO_1},
  {.NameAction = "Side_Lock",      .MCM_A = MCM250_1, .Pin_A = Pin_9, .MCM_B = MCM_None, .Pin_B = Pin_None, .ValveType = Valve_PVEO_1},
  {.NameAction = "Side_SteelRuber",.MCM_A = MCM_None, .Pin_A = Pin_None, .MCM_B = MCM_None, .Pin_B = Pin_None, .ValveType = Valve_PVEO_1},
  {.NameAction = "Side_Unload",    .MCM_A = MCM_None, .Pin_A = Pin_None, .MCM_B = MCM_None, .Pin_B = Pin_None, .ValveType = Valve_PVEO_1},
  {.NameAction = "Side_Press",     .MCM_A = MCM_None, .Pin_A = Pin_None, .MCM_B = MCM_None, .Pin_B = Pin_None, .ValveType = Valve_PVEO_1},
  {.NameAction = "Side_Rotate",    .MCM_A = MCM_None, .Pin_A = Pin_None, .MCM_B = MCM_None, .Pin_B = Pin_None, .ValveType = Valve_PVEO_1},
  {.NameAction = "Side_Tilt",      .MCM_A = MCM_None, .Pin_A = Pin_None, .MCM_B = MCM_None, .Pin_B = Pin_None, .ValveType = Valve_PVEH_OMFB},
  //Top
  {.NameAction = "RTR",       .MCM_A = MCM250_1, .Pin_A = Pin_8, .MCM_B = MCM_None, .Pin_B = Pin_None, .ValveType = Valve_PVEH_OMFB},
  {.NameAction = "C610H",     .MCM_A = MCM250_1, .Pin_A = Pin_8, .MCM_B = MCM_None, .Pin_B = Pin_None, .ValveType = Valve_PVEH_OMFB},
  {.NameAction = "BP300",     .MCM_A = MCM250_1, .Pin_A = Pin_7, .MCM_B = MCM_None, .Pin_B = Pin_None, .ValveType = Valve_PVEH_OMFB},
  {.NameAction = "MF500Bar",  .MCM_A = MCM250_3, .Pin_A = Pin_7, .MCM_B = MCM_None, .Pin_B = Pin_None, .ValveType = Valve_PVEO_1},
  {.NameAction = "Top_NULL",      .MCM_A = MCM_None, .Pin_A = Pin_None, .MCM_B = MCM_None, .Pin_B = Pin_None, .ValveType = Valve_PVEH_16},
  {.NameAction = "Top_NULL",      .MCM_A = MCM_None, .Pin_A = Pin_None, .MCM_B = MCM_None, .Pin_B = Pin_None, .ValveType = Valve_PVEH_16},
  {.NameAction = "Top_NULL",      .MCM_A = MCM_None, .Pin_A = Pin_None, .MCM_B = MCM_None, .Pin_B = Pin_None, .ValveType = Valve_PVEH_16},
  {.NameAction = "Top_NULL",      .MCM_A = MCM_None, .Pin_A = Pin_None, .MCM_B = MCM_None, .Pin_B = Pin_None, .ValveType = Valve_PVEH_16},
  {.NameAction = "Top_NULL",      .MCM_A = MCM_None, .Pin_A = Pin_None, .MCM_B = MCM_None, .Pin_B = Pin_None, .ValveType = Valve_PVEH_16},
};

/**
 * @brief Распиновка для распределителя OMFB
 * // PinOut_RG _ PinOut_OMFB _ PinOut_Danfoss _ AllPinOut
 */
tPinOut PinOut_OMFBv2[] = {
  //Front
  {.NameAction = "Front_UpDown",    .MCM_A = MCM250_2, .Pin_A = Pin_3, .MCM_B = MCM250_2, .Pin_B = Pin_5, .ValveType = Valve_PVEO_2}, // 0
  {.NameAction = "Front_LeftRight", .MCM_A = MCM250_1, .Pin_A = Pin_28, .MCM_B = MCM250_1, .Pin_B = Pin_29, .ValveType = Valve_PVEO_2},
  {.NameAction = "Front_Float",     .MCM_A = MCM_None, .Pin_A = Pin_None, .MCM_B = MCM_None, .Pin_B = Pin_None, .ValveType = Valve_PVEO_1},
  {.NameAction = "Front_Lock",      .MCM_A = MCM250_1, .Pin_A = Pin_4, .MCM_B = MCM_None, .Pin_B = Pin_None, .ValveType = Valve_PVEO_1},
  {.NameAction = "Front_SteelRuber",.MCM_A = MCM250_2, .Pin_A = Pin_28, .MCM_B = MCM250_2, .Pin_B = Pin_29, .ValveType = Valve_PVEO_2},
  {.NameAction = "Front_Unload",    .MCM_A = MCM_None, .Pin_A = Pin_None, .MCM_B = MCM_None, .Pin_B = Pin_None, .ValveType = Valve_PVEH_16},
  {.NameAction = "Front_Press",     .MCM_A = MCM_None, .Pin_A = Pin_None, .MCM_B = MCM_None, .Pin_B = Pin_None, .ValveType = Valve_PVEH_16},
  {.NameAction = "Front_Rotate",    .MCM_A = MCM250_2, .Pin_A = Pin_8, .MCM_B = MCM_None, .Pin_B = Pin_None, .ValveType = Valve_PVEH_OMFB},
  {.NameAction = "Front_Tilt",      .MCM_A = MCM_None, .Pin_A = Pin_None, .MCM_B = MCM_None, .Pin_B = Pin_None, .ValveType = Valve_PVEO_1}, //8
  //Inter
  {.NameAction = "Inter_UpDown",    .MCM_A = MCM250_2, .Pin_A = Pin_4, .MCM_B = MCM250_2, .Pin_B = Pin_6, .ValveType = Valve_PVEO_2},
  {.NameAction = "Inter_LeftRight", .MCM_A = MCM_None, .Pin_A = Pin_None, .MCM_B = MCM_None, .Pin_B = Pin_None, .ValveType = Valve_PVEO_2},
  {.NameAction = "Inter_Float",     .MCM_A = MCM_None, .Pin_A = Pin_None, .MCM_B = MCM_None, .Pin_B = Pin_None, .ValveType = Valve_PVEO_1},
  {.NameAction = "Inter_Lock",      .MCM_A = MCM250_1, .Pin_A = Pin_3, .MCM_B = MCM_None, .Pin_B = Pin_None, .ValveType = Valve_PVEO_1},
  {.NameAction = "Inter_SteelRuber",.MCM_A = MCM_None, .Pin_A = Pin_None, .MCM_B = MCM_None, .Pin_B = Pin_None, .ValveType = Valve_PVEO_2},
  {.NameAction = "Inter_Unload",    .MCM_A = MCM_None, .Pin_A = Pin_None, .MCM_B = MCM_None, .Pin_B = Pin_None, .ValveType = Valve_PVEH_16},
  {.NameAction = "Inter_Press",     .MCM_A = MCM_None, .Pin_A = Pin_None, .MCM_B = MCM_None, .Pin_B = Pin_None, .ValveType = Valve_PVEH_16},
  {.NameAction = "Inter_Rotate",    .MCM_A = MCM250_2, .Pin_A = Pin_7, .MCM_B = MCM_None, .Pin_B = Pin_None, .ValveType = Valve_PVEH_OMFB},
  {.NameAction = "Inter_Tilt",      .MCM_A = MCM_None, .Pin_A = Pin_None, .MCM_B = MCM_None, .Pin_B = Pin_None, .ValveType = Valve_PVEH_16},
  //Side
  {.NameAction = "Side_UpDown",    .MCM_A = MCM250_1, .Pin_A = Pin_10, .MCM_B = MCM250_1, .Pin_B = Pin_5, .ValveType = Valve_PVEO_2},
  {.NameAction = "Side_LeftRight", .MCM_A = MCM250_2, .Pin_A = Pin_9, .MCM_B = MCM250_2, .Pin_B = Pin_10, .ValveType = Valve_PVEO_2},
  {.NameAction = "Side_Float",     .MCM_A = Pin_None, .Pin_A = MCM_None, .MCM_B = MCM_None, .Pin_B = Pin_None, .ValveType = Valve_PVEO_1},
  {.NameAction = "Side_Lock",      .MCM_A = MCM250_1, .Pin_A = Pin_9, .MCM_B = MCM_None, .Pin_B = Pin_None, .ValveType = Valve_PVEO_1},
  {.NameAction = "Side_SteelRuber",.MCM_A = MCM_None, .Pin_A = Pin_None, .MCM_B = MCM_None, .Pin_B = Pin_None, .ValveType = Valve_PVEO_1},
  {.NameAction = "Side_Unload",    .MCM_A = MCM_None, .Pin_A = Pin_None, .MCM_B = MCM_None, .Pin_B = Pin_None, .ValveType = Valve_PVEO_1},
  {.NameAction = "Side_Press",     .MCM_A = MCM_None, .Pin_A = Pin_None, .MCM_B = MCM_None, .Pin_B = Pin_None, .ValveType = Valve_PVEO_1},
  {.NameAction = "Side_Rotate",    .MCM_A = MCM_None, .Pin_A = Pin_None, .MCM_B = MCM_None, .Pin_B = Pin_None, .ValveType = Valve_PVEO_1},
  {.NameAction = "Side_Tilt",      .MCM_A = MCM_None, .Pin_A = Pin_None, .MCM_B = MCM_None, .Pin_B = Pin_None, .ValveType = Valve_PVEO_2},
  //Top
  {.NameAction = "RTR",       .MCM_A = MCM250_1, .Pin_A = Pin_8, .MCM_B = MCM_None, .Pin_B = Pin_None, .ValveType = Valve_PVEH_OMFB},
  {.NameAction = "C610H",     .MCM_A = MCM250_1, .Pin_A = Pin_8, .MCM_B = MCM_None, .Pin_B = Pin_None, .ValveType = Valve_PVEH_OMFB},
  {.NameAction = "BP300",     .MCM_A = MCM250_1, .Pin_A = Pin_7, .MCM_B = MCM_None, .Pin_B = Pin_None, .ValveType = Valve_PVEH_OMFB},
  {.NameAction = "MF500Bar",  .MCM_A = MCM250_3, .Pin_A = Pin_7, .MCM_B = MCM_None, .Pin_B = Pin_None, .ValveType = Valve_PVEO_1},
  {.NameAction = "Top_NULL",      .MCM_A = MCM_None, .Pin_A = Pin_None, .MCM_B = MCM_None, .Pin_B = Pin_None, .ValveType = Valve_PVEH_16},
  {.NameAction = "Top_NULL",      .MCM_A = MCM_None, .Pin_A = Pin_None, .MCM_B = MCM_None, .Pin_B = Pin_None, .ValveType = Valve_PVEH_16},
  {.NameAction = "Top_NULL",      .MCM_A = MCM_None, .Pin_A = Pin_None, .MCM_B = MCM_None, .Pin_B = Pin_None, .ValveType = Valve_PVEH_16},
  {.NameAction = "Top_NULL",      .MCM_A = MCM_None, .Pin_A = Pin_None, .MCM_B = MCM_None, .Pin_B = Pin_None, .ValveType = Valve_PVEH_16},
  {.NameAction = "Top_NULL",      .MCM_A = MCM_None, .Pin_A = Pin_None, .MCM_B = MCM_None, .Pin_B = Pin_None, .ValveType = Valve_PVEH_16},
};

/**
 * @brief Распиновка для распределителя Danfoss
 * // PinOut_RG _ PinOut_OMFB _ PinOut_Danfoss _ AllPinOut
 */
tPinOut PinOut_Danfoss[] = {
  //Front
  {.NameAction = "Front_UpDown",    .MCM_A = MCM250_1, .Pin_A = Pin_5, .MCM_B = MCM_None, .Pin_B = Pin_None, .ValveType = Valve_PVEH_16}, // 0
  {.NameAction = "Front_LeftRight", .MCM_A = MCM250_1, .Pin_A = Pin_29, .MCM_B = MCM250_1, .Pin_B = Pin_28, .ValveType = Valve_PVEO_2},
  {.NameAction = "Front_Float",     .MCM_A = MCM250_1, .Pin_A = Pin_3, .MCM_B = MCM_None, .Pin_B = Pin_None, .ValveType = Valve_PVEO_1},
  {.NameAction = "Front_Lock",      .MCM_A = MCM250_1, .Pin_A = Pin_4, .MCM_B = MCM_None, .Pin_B = Pin_None, .ValveType = Valve_PVEO_1},
  {.NameAction = "Front_SteelRuber",.MCM_A = MCM250_1, .Pin_A = Pin_8, .MCM_B = MCM250_1, .Pin_B = Pin_7, .ValveType = Valve_PVEO_2},
  {.NameAction = "Front_Unload",    .MCM_A = MCM_None, .Pin_A = Pin_None, .MCM_B = MCM_None, .Pin_B = Pin_None, .ValveType = Valve_PVEH_16},
  {.NameAction = "Front_Press",     .MCM_A = MCM_None, .Pin_A = Pin_None, .MCM_B = MCM_None, .Pin_B = Pin_None, .ValveType = Valve_PVEH_16},
  {.NameAction = "Front_Rotate",    .MCM_A = MCM250_1, .Pin_A = Pin_6, .MCM_B = MCM_None, .Pin_B = Pin_None, .ValveType = Valve_PVEH_16},
  {.NameAction = "Front_Tilt",      .MCM_A = MCM_None, .Pin_A = Pin_None, .MCM_B = MCM_None, .Pin_B = Pin_None, .ValveType = Valve_PVEO_1}, //8
  //Inter
  {.NameAction = "Inter_UpDown",    .MCM_A = MCM250_2, .Pin_A = Pin_5, .MCM_B = MCM_None, .Pin_B = Pin_None, .ValveType = Valve_PVEH_16},
  {.NameAction = "Inter_LeftRight", .MCM_A = MCM_None, .Pin_A = Pin_None, .MCM_B = MCM_None, .Pin_B = Pin_None, .ValveType = Valve_PVEO_2},
  {.NameAction = "Inter_Float",     .MCM_A = MCM250_2, .Pin_A = Pin_28, .MCM_B = MCM_None, .Pin_B = Pin_None, .ValveType = Valve_PVEO_1},
  {.NameAction = "Inter_Lock",      .MCM_A = MCM250_2, .Pin_A = Pin_4, .MCM_B = MCM_None, .Pin_B = Pin_None, .ValveType = Valve_PVEO_1},
  {.NameAction = "Inter_SteelRuber",.MCM_A = MCM_None, .Pin_A = Pin_None, .MCM_B = MCM_None, .Pin_B = Pin_None, .ValveType = Valve_PVEO_2},
  {.NameAction = "Inter_Unload",    .MCM_A = MCM_None, .Pin_A = Pin_None, .MCM_B = MCM_None, .Pin_B = Pin_None, .ValveType = Valve_PVEH_16},
  {.NameAction = "Inter_Press",     .MCM_A = MCM_None, .Pin_A = Pin_None, .MCM_B = MCM_None, .Pin_B = Pin_None, .ValveType = Valve_PVEH_16},
  {.NameAction = "Inter_Rotate",    .MCM_A = MCM250_1, .Pin_A = Pin_6, .MCM_B = MCM_None, .Pin_B = Pin_None, .ValveType = Valve_PVEH_16},
  {.NameAction = "Inter_Tilt",      .MCM_A = MCM_None, .Pin_A = Pin_None, .MCM_B = MCM_None, .Pin_B = Pin_None, .ValveType = Valve_PVEH_16},
  //Side
  {.NameAction = "Side_UpDown",    .MCM_A = MCM250_2, .Pin_A = Pin_7, .MCM_B = MCM_None, .Pin_B = Pin_None, .ValveType = Valve_PVEH_16},
  {.NameAction = "Side_LeftRight", .MCM_A = MCM250_2, .Pin_A = Pin_8, .MCM_B = MCM250_2, .Pin_B = Pin_9, .ValveType = Valve_PVEO_2},
  {.NameAction = "Side_Float",     .MCM_A = MCM250_2, .Pin_A = Pin_10, .MCM_B = MCM_None, .Pin_B = Pin_None, .ValveType = Valve_PVEO_1},
  {.NameAction = "Side_Lock",      .MCM_A = MCM_None, .Pin_A = Pin_None, .MCM_B = MCM_None, .Pin_B = Pin_None, .ValveType = Valve_PVEO_1},
  {.NameAction = "Side_SteelRuber",.MCM_A = MCM_None, .Pin_A = Pin_None, .MCM_B = MCM_None, .Pin_B = Pin_None, .ValveType = Valve_PVEO_1},
  {.NameAction = "Side_Unload",    .MCM_A = MCM_None, .Pin_A = Pin_None, .MCM_B = MCM_None, .Pin_B = Pin_None, .ValveType = Valve_PVEO_1},
  {.NameAction = "Side_Press",     .MCM_A = MCM_None, .Pin_A = Pin_None, .MCM_B = MCM_None, .Pin_B = Pin_None, .ValveType = Valve_PVEO_1},
  {.NameAction = "Side_Rotate",    .MCM_A = MCM_None, .Pin_A = Pin_None, .MCM_B = MCM_None, .Pin_B = Pin_None, .ValveType = Valve_PVEO_1},
  {.NameAction = "Side_Tilt",      .MCM_A = MCM_None, .Pin_A = Pin_None, .MCM_B = MCM_None, .Pin_B = Pin_None, .ValveType = Valve_PVEO_1},
  //Top
  {.NameAction = "RTR",       .MCM_A = MCM250_2, .Pin_A = Pin_6, .MCM_B = MCM_None, .Pin_B = Pin_None, .ValveType = Valve_PVEH_16},
  {.NameAction = "C610H",     .MCM_A = MCM250_2, .Pin_A = Pin_6, .MCM_B = MCM_None, .Pin_B = Pin_None, .ValveType = Valve_PVEH_16},
  {.NameAction = "BP300",     .MCM_A = MCM250_2, .Pin_A = Pin_6, .MCM_B = MCM_None, .Pin_B = Pin_None, .ValveType = Valve_PVEH_16},
  {.NameAction = "MF500Bar",  .MCM_A = MCM250_3, .Pin_A = Pin_7, .MCM_B = MCM_None, .Pin_B = Pin_None, .ValveType = Valve_PVEO_1},
  {.NameAction = "Top_NULL",      .MCM_A = MCM_None, .Pin_A = Pin_None, .MCM_B = MCM_None, .Pin_B = Pin_None, .ValveType = Valve_PVEH_16},
  {.NameAction = "Top_NULL",      .MCM_A = MCM_None, .Pin_A = Pin_None, .MCM_B = MCM_None, .Pin_B = Pin_None, .ValveType = Valve_PVEH_16},
  {.NameAction = "Top_NULL",      .MCM_A = MCM_None, .Pin_A = Pin_None, .MCM_B = MCM_None, .Pin_B = Pin_None, .ValveType = Valve_PVEH_16},
  {.NameAction = "Top_NULL",      .MCM_A = MCM_None, .Pin_A = Pin_None, .MCM_B = MCM_None, .Pin_B = Pin_None, .ValveType = Valve_PVEH_16},
  {.NameAction = "Top_NULL",      .MCM_A = MCM_None, .Pin_A = Pin_None, .MCM_B = MCM_None, .Pin_B = Pin_None, .ValveType = Valve_PVEH_16},
};

/**
 * @brief Распиновка для распределителя OMFB
 * // PinOut_RG _ PinOut_OMFB _ PinOut_Danfoss _ AllPinOut
 */
tPinOut PinOut_OMFB[] = {
  //Front
  {.NameAction = "Front_UpDown",    .MCM_A = MCM250_2, .Pin_A = Pin_5, .MCM_B = MCM250_2, .Pin_B = Pin_3, .ValveType = Valve_PVEH_OMFB}, // 0
  {.NameAction = "Front_LeftRight", .MCM_A = MCM250_1, .Pin_A = Pin_28, .MCM_B = MCM250_1, .Pin_B = Pin_29, .ValveType = Valve_PVEO_2},
  {.NameAction = "Front_Float",     .MCM_A = MCM_None, .Pin_A = Pin_None, .MCM_B = MCM_None, .Pin_B = Pin_None, .ValveType = Valve_PVEO_1},
  {.NameAction = "Front_Lock",      .MCM_A = MCM250_1, .Pin_A = Pin_4, .MCM_B = MCM_None, .Pin_B = Pin_None, .ValveType = Valve_PVEO_1},
  {.NameAction = "Front_SteelRuber",.MCM_A = MCM250_2, .Pin_A = Pin_28, .MCM_B = MCM250_2, .Pin_B = Pin_29, .ValveType = Valve_PVEO_2},
  {.NameAction = "Front_Unload",    .MCM_A = MCM_None, .Pin_A = Pin_None, .MCM_B = MCM_None, .Pin_B = Pin_None, .ValveType = Valve_PVEH_16},
  {.NameAction = "Front_Press",     .MCM_A = MCM_None, .Pin_A = Pin_None, .MCM_B = MCM_None, .Pin_B = Pin_None, .ValveType = Valve_PVEH_16},
  {.NameAction = "Front_Rotate",    .MCM_A = MCM250_2, .Pin_A = Pin_8, .MCM_B = MCM_None, .Pin_B = Pin_None, .ValveType = Valve_PVEH_OMFB},
  {.NameAction = "Front_Tilt",      .MCM_A = MCM_None, .Pin_A = Pin_None, .MCM_B = MCM_None, .Pin_B = Pin_None, .ValveType = Valve_PVEO_1}, //8
  //Inter
  {.NameAction = "Inter_UpDown",    .MCM_A = MCM250_2, .Pin_A = Pin_6, .MCM_B = MCM250_2, .Pin_B = Pin_4, .ValveType = Valve_PVEH_OMFB},
  {.NameAction = "Inter_LeftRight", .MCM_A = MCM_None, .Pin_A = Pin_None, .MCM_B = MCM_None, .Pin_B = Pin_None, .ValveType = Valve_PVEO_2},
  {.NameAction = "Inter_Float",     .MCM_A = MCM_None, .Pin_A = Pin_None, .MCM_B = MCM_None, .Pin_B = Pin_None, .ValveType = Valve_PVEO_1},
  {.NameAction = "Inter_Lock",      .MCM_A = MCM250_1, .Pin_A = Pin_3, .MCM_B = MCM_None, .Pin_B = Pin_None, .ValveType = Valve_PVEO_1},
  {.NameAction = "Inter_SteelRuber",.MCM_A = MCM_None, .Pin_A = Pin_None, .MCM_B = MCM_None, .Pin_B = Pin_None, .ValveType = Valve_PVEO_2},
  {.NameAction = "Inter_Unload",    .MCM_A = MCM_None, .Pin_A = Pin_None, .MCM_B = MCM_None, .Pin_B = Pin_None, .ValveType = Valve_PVEH_16},
  {.NameAction = "Inter_Press",     .MCM_A = MCM_None, .Pin_A = Pin_None, .MCM_B = MCM_None, .Pin_B = Pin_None, .ValveType = Valve_PVEH_16},
  {.NameAction = "Inter_Rotate",    .MCM_A = MCM250_2, .Pin_A = Pin_7, .MCM_B = MCM_None, .Pin_B = Pin_None, .ValveType = Valve_PVEH_OMFB},
  {.NameAction = "Inter_Tilt",      .MCM_A = MCM_None, .Pin_A = Pin_None, .MCM_B = MCM_None, .Pin_B = Pin_None, .ValveType = Valve_PVEH_16},
  //Side
  {.NameAction = "Side_UpDown",    .MCM_A = MCM250_1, .Pin_A = Pin_5, .MCM_B = MCM250_1, .Pin_B = Pin_10, .ValveType = Valve_PVEH_OMFB},
  {.NameAction = "Side_LeftRight", .MCM_A = MCM250_2, .Pin_A = Pin_9, .MCM_B = MCM250_2, .Pin_B = Pin_10, .ValveType = Valve_PVEO_2},
  {.NameAction = "Side_Float",     .MCM_A = MCM250_3, .Pin_A = Pin_10, .MCM_B = MCM_None, .Pin_B = Pin_None, .ValveType = Valve_PVEO_1},
  {.NameAction = "Side_Lock",      .MCM_A = MCM250_1, .Pin_A = Pin_9, .MCM_B = MCM_None, .Pin_B = Pin_None, .ValveType = Valve_PVEO_1},
  {.NameAction = "Side_SteelRuber",.MCM_A = MCM_None, .Pin_A = Pin_None, .MCM_B = MCM_None, .Pin_B = Pin_None, .ValveType = Valve_PVEO_1},
  {.NameAction = "Side_Unload",    .MCM_A = MCM_None, .Pin_A = Pin_None, .MCM_B = MCM_None, .Pin_B = Pin_None, .ValveType = Valve_PVEO_1},
  {.NameAction = "Side_Press",     .MCM_A = MCM_None, .Pin_A = Pin_None, .MCM_B = MCM_None, .Pin_B = Pin_None, .ValveType = Valve_PVEO_1},
  {.NameAction = "Side_Rotate",    .MCM_A = MCM_None, .Pin_A = Pin_None, .MCM_B = MCM_None, .Pin_B = Pin_None, .ValveType = Valve_PVEO_1},
  {.NameAction = "Side_Tilt",      .MCM_A = MCM_None, .Pin_A = Pin_None, .MCM_B = MCM_None, .Pin_B = Pin_None, .ValveType = Valve_PVEH_OMFB},
  //Top
  {.NameAction = "RTR",       .MCM_A = MCM250_1, .Pin_A = Pin_8, .MCM_B = MCM_None, .Pin_B = Pin_None, .ValveType = Valve_PVEH_OMFB},
  {.NameAction = "C610H",     .MCM_A = MCM250_1, .Pin_A = Pin_8, .MCM_B = MCM_None, .Pin_B = Pin_None, .ValveType = Valve_PVEH_OMFB},
  {.NameAction = "BP300",     .MCM_A = MCM250_1, .Pin_A = Pin_7, .MCM_B = MCM_None, .Pin_B = Pin_None, .ValveType = Valve_PVEH_OMFB},
  {.NameAction = "MF500Bar",  .MCM_A = MCM250_3, .Pin_A = Pin_7, .MCM_B = MCM_None, .Pin_B = Pin_None, .ValveType = Valve_PVEO_1},
  {.NameAction = "Top_NULL",      .MCM_A = MCM_None, .Pin_A = Pin_None, .MCM_B = MCM_None, .Pin_B = Pin_None, .ValveType = Valve_PVEH_16},
  {.NameAction = "Top_NULL",      .MCM_A = MCM_None, .Pin_A = Pin_None, .MCM_B = MCM_None, .Pin_B = Pin_None, .ValveType = Valve_PVEH_16},
  {.NameAction = "Top_NULL",      .MCM_A = MCM_None, .Pin_A = Pin_None, .MCM_B = MCM_None, .Pin_B = Pin_None, .ValveType = Valve_PVEH_16},
  {.NameAction = "Top_NULL",      .MCM_A = MCM_None, .Pin_A = Pin_None, .MCM_B = MCM_None, .Pin_B = Pin_None, .ValveType = Valve_PVEH_16},
  {.NameAction = "Top_NULL",      .MCM_A = MCM_None, .Pin_A = Pin_None, .MCM_B = MCM_None, .Pin_B = Pin_None, .ValveType = Valve_PVEH_16},
};

/**
 * @brief Распиновка для распределителя RG
 * // PinOut_RG _ PinOut_OMFB _ PinOut_Danfoss _ AllPinOut
 */
tPinOut PinOut_RG[] = {
  //Front
  {.NameAction = "Front_UpDown",    .MCM_A = MCM250_1, .Pin_A = Pin_3, .MCM_B = MCM250_1, .Pin_B = Pin_5, .ValveType = Valve_RG}, // 0
  {.NameAction = "Front_LeftRight", .MCM_A = MCM250_1, .Pin_A = Pin_28, .MCM_B = MCM250_1, .Pin_B = Pin_29, .ValveType = Valve_PVEO_2},
  {.NameAction = "Front_Float",     .MCM_A = MCM250_1, .Pin_A = Pin_3, .MCM_B = MCM_None, .Pin_B = Pin_None, .ValveType = Valve_PVEO_1},
  {.NameAction = "Front_Lock",      .MCM_A = MCM250_1, .Pin_A = Pin_4, .MCM_B = MCM_None, .Pin_B = Pin_None, .ValveType = Valve_PVEO_1},
  {.NameAction = "Front_SteelRuber",.MCM_A = MCM250_1, .Pin_A = Pin_7, .MCM_B = MCM250_1, .Pin_B = Pin_8, .ValveType = Valve_PVEO_2},
  {.NameAction = "Front_Unload",    .MCM_A = MCM_None, .Pin_A = Pin_None, .MCM_B = MCM_None, .Pin_B = Pin_None, .ValveType = Valve_PVEH_16},
  {.NameAction = "Front_Press",     .MCM_A = MCM_None, .Pin_A = Pin_None, .MCM_B = MCM_None, .Pin_B = Pin_None, .ValveType = Valve_PVEH_16},
  {.NameAction = "Front_Rotate",    .MCM_A = MCM250_1, .Pin_A = Pin_6, .MCM_B = MCM_None, .Pin_B = Pin_None, .ValveType = Valve_RG},
  {.NameAction = "Front_Tilt",      .MCM_A = MCM_None, .Pin_A = Pin_None, .MCM_B = MCM_None, .Pin_B = Pin_None, .ValveType = Valve_PVEO_1}, //8
  //Inter
  {.NameAction = "Inter_UpDown",    .MCM_A = MCM250_2, .Pin_A = Pin_9, .MCM_B = MCM250_2, .Pin_B = Pin_10, .ValveType = Valve_RG},
  {.NameAction = "Inter_LeftRight", .MCM_A = MCM_None, .Pin_A = Pin_None, .MCM_B = MCM_None, .Pin_B = Pin_None, .ValveType = Valve_PVEO_2},
  {.NameAction = "Inter_Float",     .MCM_A = MCM250_2, .Pin_A = Pin_28, .MCM_B = MCM_None, .Pin_B = Pin_None, .ValveType = Valve_PVEO_1},
  {.NameAction = "Inter_Lock",      .MCM_A = MCM250_2, .Pin_A = Pin_4, .MCM_B = MCM_None, .Pin_B = Pin_None, .ValveType = Valve_PVEO_1},
  {.NameAction = "Inter_SteelRuber",.MCM_A = MCM_None, .Pin_A = Pin_None, .MCM_B = MCM_None, .Pin_B = Pin_None, .ValveType = Valve_PVEO_2},
  {.NameAction = "Inter_Unload",    .MCM_A = MCM_None, .Pin_A = Pin_None, .MCM_B = MCM_None, .Pin_B = Pin_None, .ValveType = Valve_PVEH_16},
  {.NameAction = "Inter_Press",     .MCM_A = MCM_None, .Pin_A = Pin_None, .MCM_B = MCM_None, .Pin_B = Pin_None, .ValveType = Valve_PVEH_16},
  {.NameAction = "Inter_Rotate",    .MCM_A = MCM250_2, .Pin_A = Pin_8, .MCM_B = MCM_None, .Pin_B = Pin_None, .ValveType = Valve_RG},
  {.NameAction = "Inter_Tilt",      .MCM_A = MCM_None, .Pin_A = Pin_None, .MCM_B = MCM_None, .Pin_B = Pin_None, .ValveType = Valve_PVEH_16},
  //Side
  {.NameAction = "Side_UpDown",    .MCM_A = MCM250_2, .Pin_A = Pin_7, .MCM_B = MCM_None, .Pin_B = Pin_None, .ValveType = Valve_RG},
  {.NameAction = "Side_LeftRight", .MCM_A = MCM250_2, .Pin_A = Pin_8, .MCM_B = MCM250_2, .Pin_B = Pin_9, .ValveType = Valve_PVEO_2},
  {.NameAction = "Side_Float",     .MCM_A = MCM250_2, .Pin_A = Pin_10, .MCM_B = MCM_None, .Pin_B = Pin_None, .ValveType = Valve_PVEO_1},
  {.NameAction = "Side_Lock",      .MCM_A = MCM_None, .Pin_A = Pin_None, .MCM_B = MCM_None, .Pin_B = Pin_None, .ValveType = Valve_PVEO_1},
  {.NameAction = "Side_SteelRuber",.MCM_A = MCM_None, .Pin_A = Pin_None, .MCM_B = MCM_None, .Pin_B = Pin_None, .ValveType = Valve_PVEO_1},
  {.NameAction = "Side_Unload",    .MCM_A = MCM_None, .Pin_A = Pin_None, .MCM_B = MCM_None, .Pin_B = Pin_None, .ValveType = Valve_PVEO_1},
  {.NameAction = "Side_Press",     .MCM_A = MCM_None, .Pin_A = Pin_None, .MCM_B = MCM_None, .Pin_B = Pin_None, .ValveType = Valve_PVEO_1},
  {.NameAction = "Side_Rotate",    .MCM_A = MCM_None, .Pin_A = Pin_None, .MCM_B = MCM_None, .Pin_B = Pin_None, .ValveType = Valve_PVEO_1},
  {.NameAction = "Side_Tilt",      .MCM_A = MCM_None, .Pin_A = Pin_None, .MCM_B = MCM_None, .Pin_B = Pin_None, .ValveType = Valve_PVEO_1},
  //Top
  {.NameAction = "Top_RTR",       .MCM_A = MCM250_2, .Pin_A = Pin_6, .MCM_B = MCM_None, .Pin_B = Pin_None, .ValveType = Valve_RG},
  {.NameAction = "Top_C610H",     .MCM_A = MCM250_2, .Pin_A = Pin_6, .MCM_B = MCM_None, .Pin_B = Pin_None, .ValveType = Valve_RG},
  {.NameAction = "Top_BP300",     .MCM_A = MCM250_2, .Pin_A = Pin_7, .MCM_B = MCM_None, .Pin_B = Pin_None, .ValveType = Valve_RG},
  {.NameAction = "Top_MF500Bar",  .MCM_A = MCM250_3, .Pin_A = Pin_7, .MCM_B = MCM_None, .Pin_B = Pin_None, .ValveType = Valve_PVEO_1},
  {.NameAction = "Top_NULL",      .MCM_A = MCM_None, .Pin_A = Pin_None, .MCM_B = MCM_None, .Pin_B = Pin_None, .ValveType = Valve_PVEH_16},
  {.NameAction = "Top_NULL",      .MCM_A = MCM_None, .Pin_A = Pin_None, .MCM_B = MCM_None, .Pin_B = Pin_None, .ValveType = Valve_PVEH_16},
  {.NameAction = "Top_NULL",      .MCM_A = MCM_None, .Pin_A = Pin_None, .MCM_B = MCM_None, .Pin_B = Pin_None, .ValveType = Valve_PVEH_16},
  {.NameAction = "Top_NULL",      .MCM_A = MCM_None, .Pin_A = Pin_None, .MCM_B = MCM_None, .Pin_B = Pin_None, .ValveType = Valve_PVEH_16},
  {.NameAction = "Top_NULL",      .MCM_A = MCM_None, .Pin_A = Pin_None, .MCM_B = MCM_None, .Pin_B = Pin_None, .ValveType = Valve_PVEH_16},
};

void PinOut_WriteAction(tPinOut * source, tGsFile * fp) {
  if(source == NULL || fp == NULL) {
    return;
  }

  FilePrintf(fp, "Name = %s, ", source->NameAction);
  FilePrintf(fp, "MCM_A = %d, ", source->MCM_A);
  FilePrintf(fp, "pin_A = %d, ", source->Pin_A);
  FilePrintf(fp, "MCM_B = %d, ", source->MCM_B);
  FilePrintf(fp, "pin_B = %d, ", source->Pin_B);
  FilePrintf(fp, "ValveType = %d\r\n", source->ValveType);
}

void PinOut_WritaActions(tPinOut * pins, int size, tGsFile * fp) {
  for(int i = 0; i < size; i++) {
    PinOut_WriteAction(pins + i, fp);
  }
}

int PinOut_WriteAllToFile(const char * path) {
  tGsFile* fp = FileOpen( path, "w" );
  if(fp == NULL) {
    return 0;
  }

  PinOut_WritaActions(AllPinOut, GS_ARRAYELEMENTS(AllPinOut), fp);

  FileClose( fp );

  return 1;
}

void PinOut_FindNSet(int32_t * val, const char * header, const char * buf) {
  char * pos = strstr(buf, header);
  if(pos) {
    *val = atoi(pos + strlen(header));
  }
  else {
    *val = 0;
  }
}

void PinOut_ParseNWrite(tPinOut * pin, char * buf) {
  if(pin == NULL || buf == NULL) {
    return;
  }

  PinOut_FindNSet(&pin->MCM_A, "MCM_A = ", buf);
  PinOut_FindNSet(&pin->Pin_A, "pin_A = ", buf);
  PinOut_FindNSet(&pin->MCM_B, "MCM_B = ", buf);
  PinOut_FindNSet(&pin->Pin_B, "pin_B = ", buf);
  PinOut_FindNSet(&pin->ValveType, "ValveType = ", buf);
}

char * PinOut_ParsePinName(const char * backup_str) {
  if(backup_str == NULL) {
    return NULL;
  }

  char * pos = strstr(backup_str, "Name = ");
  if(pos) {
    pos += 7;
  }

  return pos;
}

tPinOut * PinOut_FindPinByName(const char * name) {
  for(int i = 0; i < GS_ARRAYELEMENTS(AllPinOut); i++) {
    if(strncmp(name, AllPinOut[i].NameAction, strlen(AllPinOut[i].NameAction)) == 0) {
      return AllPinOut + i;
    }
  }

  return NULL;
}

#define READ_BUFFER_SIZE 256
int PinOut_ReadBackup(const char * path) {
  tGsFile* fp = FileOpen( path, "r" );
  if(fp == NULL) {
    return 0;
  }

  char buffer[READ_BUFFER_SIZE];
  while(FileGets(buffer, READ_BUFFER_SIZE, fp)) {
    tPinOut * pin = PinOut_FindPinByName(PinOut_ParsePinName(buffer));

    if(pin != NULL) {
      PinOut_ParseNWrite(pin, buffer);
    }
  }

  FileClose( fp );
  PinOut_Write_Struct();

  return 1;
}


int16_t PinOut_Get_MCM_A(uint8_t Action, uint8_t Equip) {
  return AllPinOut[Action + (Equip * 9)].MCM_A - 1;
}
int16_t PinOut_Get_MCM_B(uint8_t Action, uint8_t Equip) {
  return AllPinOut[Action + (Equip * 9)].MCM_B - 1;
}
int16_t PinOut_Get_Pin_A(uint8_t Action, uint8_t Equip) {
  uint32_t pin = AllPinOut[Action + (Equip * 9)].Pin_A;
  static uint32_t num = 0;
  for (uint16_t i = 0; i < GS_ARRAYELEMENTS(DigitalOut_PinIDX); i++) {
    if (pin == DigitalOut_PinIDX[i]) {
      num = i;
      break;
    }
  }
  return DigitalOut_Out[num];
}
int16_t PinOut_Get_Pin_B(uint8_t Action, uint8_t Equip) {
  uint32_t pin = AllPinOut[Action + (Equip * 9)].Pin_B;
  static uint32_t num = 0;
  for (uint16_t i = 0; i < GS_ARRAYELEMENTS(DigitalOut_PinIDX); i++) {
    if (pin == DigitalOut_PinIDX[i]) {
      num = i;
      break;
    }
  }
  return DigitalOut_Out[num];
}
uint16_t PinOut_Get_ValveType(uint8_t Action, uint8_t Equip) {
  return AllPinOut[Action + (Equip * 9)].ValveType;
}

/**
 * @brief Графическое отображение распиновки из структуры
 */
void PinOut_Load(void) {
  static int change = -1;
  if (GetVarIndexed(IDX_PIN_ATTACHNAME) != change) {
    for (int i = 0; i < (GS_ARRAYELEMENTS(AllPinOut) / 4); i++) {
      SetVarIndexed(IDX_PINOUT_MCM_A_00 + i, AllPinOut[i + (GetVarIndexed(IDX_PIN_ATTACHNAME) * 9)].MCM_A);
      SetVarIndexed(IDX_PINOUT_PIN_A_00 + i, AllPinOut[i + (GetVarIndexed(IDX_PIN_ATTACHNAME) * 9)].Pin_A);
      SetVarIndexed(IDX_PINOUT_MCM_B_00 + i, AllPinOut[i + (GetVarIndexed(IDX_PIN_ATTACHNAME) * 9)].MCM_B);
      SetVarIndexed(IDX_PINOUT_PIN_B_00 + i, AllPinOut[i + (GetVarIndexed(IDX_PIN_ATTACHNAME) * 9)].Pin_B);
      SetVarIndexed(IDX_PINOUT_VALVETYPE_00 + i, AllPinOut[i + (GetVarIndexed(IDX_PIN_ATTACHNAME) * 9)].ValveType);
    }
  }
  change = GetVar(HDL_PIN_ATTACHNAME);
}

/**
 * @brief Запись в файл из графических переменных
 */
void PinOut_Write(void) {
  for (int i = 0; i < (GS_ARRAYELEMENTS(AllPinOut) / 4); i++) {
    AllPinOut[i + (GetVarIndexed(IDX_PIN_ATTACHNAME) * 9)].MCM_A = GetVarIndexed(IDX_PINOUT_MCM_A_00 + i);
    AllPinOut[i + (GetVarIndexed(IDX_PIN_ATTACHNAME) * 9)].MCM_B = GetVarIndexed(IDX_PINOUT_MCM_B_00 + i);
    AllPinOut[i + (GetVarIndexed(IDX_PIN_ATTACHNAME) * 9)].Pin_A = GetVarIndexed(IDX_PINOUT_PIN_A_00 + i);
    AllPinOut[i + (GetVarIndexed(IDX_PIN_ATTACHNAME) * 9)].Pin_B = GetVarIndexed(IDX_PINOUT_PIN_B_00 + i);
    AllPinOut[i + (GetVarIndexed(IDX_PIN_ATTACHNAME) * 9)].ValveType = GetVarIndexed(IDX_PINOUT_VALVETYPE_00 + i);
  }
  PinOut_Write_Struct();
}

/**
 * @brief Запись в файл cтруктуры с распиновкой
 */
void PinOut_Write_Struct(void) {
  tGsFile * fp = FileOpen(PATH_PINOUT, "w");
  FileWrite( & AllPinOut, sizeof(AllPinOut), 1, fp);
  FileClose(fp);
}

/**
 * @brief Считывание из файла в структуру
 */
void PinOut_Read(void) {
  tGsFile * fp = FileOpen(PATH_PINOUT, "r");
  FileRead( & AllPinOut, sizeof(AllPinOut), 1, fp);
  FileClose(fp);
}

/**
 * @brief Загрузка распиновки для распределителя OMFB
 */
void PinOut_Write_OMFB(void) {
  tGsFile * fp = FileOpen(PATH_PINOUT, "w");
  for (int i = 0; i < (GS_ARRAYELEMENTS(AllPinOut)); i++) {
    AllPinOut[i].MCM_A = PinOut_OMFB[i].MCM_A;
    AllPinOut[i].MCM_B = PinOut_OMFB[i].MCM_B;
    AllPinOut[i].Pin_A = PinOut_OMFB[i].Pin_A;
    AllPinOut[i].Pin_B = PinOut_OMFB[i].Pin_B;
    AllPinOut[i].ValveType = PinOut_OMFB[i].ValveType;
  }
  FileWrite( & AllPinOut, sizeof(AllPinOut), 1, fp);
  FileClose(fp);
  SetVarIndexed(IDX_CHANGE_VALV, 0);
}

/**
 * @brief Загрузка распиновки для распределителя OMFBv2
 */
void PinOut_Write_OMFBv2(void) {
  tGsFile * fp = FileOpen(PATH_PINOUT, "w");
  for (int i = 0; i < (GS_ARRAYELEMENTS(AllPinOut)); i++) {
    AllPinOut[i].MCM_A = PinOut_OMFBv2[i].MCM_A;
    AllPinOut[i].MCM_B = PinOut_OMFBv2[i].MCM_B;
    AllPinOut[i].Pin_A = PinOut_OMFBv2[i].Pin_A;
    AllPinOut[i].Pin_B = PinOut_OMFBv2[i].Pin_B;
    AllPinOut[i].ValveType = PinOut_OMFBv2[i].ValveType;
  }
  FileWrite( & AllPinOut, sizeof(AllPinOut), 1, fp);
  FileClose(fp);
  SetVarIndexed(IDX_CHANGE_VALV, 0);
}

/**
 * @brief Загрузка распиновки для распределителя Danfoss
 */
void PinOut_Write_Danfoss(void) {
  tGsFile * fp = FileOpen(PATH_PINOUT, "w");
  for (int i = 0; i < (GS_ARRAYELEMENTS(AllPinOut)); i++) {
    AllPinOut[i].MCM_A = PinOut_Danfoss[i].MCM_A;
    AllPinOut[i].MCM_B = PinOut_Danfoss[i].MCM_B;
    AllPinOut[i].Pin_A = PinOut_Danfoss[i].Pin_A;
    AllPinOut[i].Pin_B = PinOut_Danfoss[i].Pin_B;
    AllPinOut[i].ValveType = PinOut_Danfoss[i].ValveType;
  }
  FileWrite( & AllPinOut, sizeof(AllPinOut), 1, fp);
  FileClose(fp);
  SetVarIndexed(IDX_CHANGE_VALV, 0);
}

/**
 * @brief Загрузка распиновки для распределителя RG
 */
void PinOut_Write_RG(void) {
  tGsFile * fp = FileOpen(PATH_PINOUT, "w");
  for (int i = 0; i < (GS_ARRAYELEMENTS(AllPinOut)); i++) {
    AllPinOut[i].MCM_A = PinOut_RG[i].MCM_A;
    AllPinOut[i].MCM_B = PinOut_RG[i].MCM_B;
    AllPinOut[i].Pin_A = PinOut_RG[i].Pin_A;
    AllPinOut[i].Pin_B = PinOut_RG[i].Pin_B;
    AllPinOut[i].ValveType = PinOut_RG[i].ValveType;
  }
  FileWrite( & AllPinOut, sizeof(AllPinOut), 1, fp);
  FileClose(fp);
  SetVarIndexed(IDX_CHANGE_VALV, 0);
}

/**
 * @brief Загрузка распиновки в зависимости от выбора распределителя
 */
void PinOut_Change_Valv(void) {
  tParamData * Param = ParameterGet();
  switch (GetVarIndexed(IDX_CHANGE_VALV)) {
  case HydDistributor_Actual:
    if (GetVar(HDL_FIRSTSTART) == 0) {
      Param->VIN.ActualHydDistributor = HydDistributor_OMFB;
      writeParameters();
    }
    break;
  case HydDistributor_Danfoss:
    PinOut_Write_Danfoss();
    Param->VIN.ActualHydDistributor = HydDistributor_Danfoss;
    writeParameters();
    break;
  case HydDistributor_OMFB:
    PinOut_Write_OMFB();
    Param->VIN.ActualHydDistributor = HydDistributor_OMFB;
    writeParameters();
    break;
  case HydDistributor_OMFBv2:
    PinOut_Write_OMFBv2();
    Param->VIN.ActualHydDistributor = HydDistributor_OMFBv2;
    writeParameters();
    break;
  case HydDistributor_RG:
    PinOut_Write_RG();
    Param->VIN.ActualHydDistributor = HydDistributor_RG;
    writeParameters();
    break;
  default:
    break;
  }
}
