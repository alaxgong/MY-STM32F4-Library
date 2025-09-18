#ifndef mystm32f4_exti_H
#define mystm32f4_exti_H




#include "stm32f4xx_exti.h"
/* EXTI 外部中断驱动函数声明 */

void myEXTI_DeInit(void);                       // 复位 EXTI 外设寄存器到默认状态
void myEXTI_Init(EXTI_InitTypeDef* EXTI_InitStruct); // 根据结构体参数初始化 EXTI 线路
void myEXTI_StructInit(EXTI_InitTypeDef* EXTI_InitStruct); // 将 EXTI_InitTypeDef 结构体初始化为默认值
void myEXTI_GenerateSWInterrupt(uint32_t EXTI_Line); // 软件触发指定 EXTI 线路中断
FlagStatus myEXTI_GetFlagStatus(uint32_t EXTI_Line); // 获取指定 EXTI 线路的挂起标志状态
void myXTI_ClearFlag(uint32_t EXTI_Line);      // 清除指定 EXTI 线路的挂起标志位
ITStatus myEXTI_GetITStatus(uint32_t EXTI_Line); // 获取指定 EXTI 线路中断状态（是否有效触发）
void myEXTI_ClearITPendingBit(uint32_t EXTI_Line); // 清除指定 EXTI 线路的中断挂起位（Pending Bit）


#endif // !mystm32f4_exti_H


