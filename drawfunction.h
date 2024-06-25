#ifndef DRAWFUNCTIONS_H
#define DRAWFUNCTIONS_H

#include	<stdint.h>


typedef struct tagPaintText {
	uint32_t colourActive;
	uint32_t colourInactive;
	uint32_t Obj;
	uint32_t oldVal;
} tPaintText;

void PaintText_Init( tPaintText* pt, uint32_t colourActive, uint32_t colourInactive, uint32_t obj);
void PaintText_Cycle( tPaintText* pt, int8_t active );

#endif // #ifndef DRAWFUNCTIONS_H
