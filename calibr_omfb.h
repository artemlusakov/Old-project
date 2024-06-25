#ifndef CALIBR_OMFB_H
#define CALIBR_OMFB_H

void Visu_CalibrBrush_Open(const tVisuData * VData);
void Visu_CalibrBrush_Close(const tVisuData * VData);
void Visu_CalibrBrush_Cycle(const tVisuData * VData);

void Visu_CalibrTop_Open(const tVisuData * VData);
void Visu_CalibrTop_Close(const tVisuData * VData);
void Visu_CalibrTop_Cycle(const tVisuData * VData);

uint8_t GetFlagBrushCalibr(void);
uint8_t GetFlagTopCalibr(void);

#endif  // #ifndef CALIBR_OMFB_H
