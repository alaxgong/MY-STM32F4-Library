#ifndef MYSTM32F4_WWDG_H
#define MYSTM32F4_WWDG_H
#include "stm32f4xx_wwdg.h"
void myWWDG_DeInit(void);
void myWWDG_SetPrescaler(uint32_t WWDG_Prescaler);
void myWWDG_SetWindowValue(uint8_t WindowValue);
void myWWDG_EnableIT(void);
void myWWDG_SetCounter(uint8_t Counter);
void myWWDG_Enable(uint8_t Counter);
FlagStatus myWWDG_GetFlagStatus(void);
void myWWDG_ClearFlag(void);
void myWWDG_Init(uint32_t prescaler, uint8_t window, uint8_t counter, uint8_t enableEWI);
#endif