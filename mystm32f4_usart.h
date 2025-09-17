
#ifndef __MYSTM32F4_USART_H
#define __MYSTM32F4_USART_H
#include "stm32f4xx_usart.h"
#include "stm32f4xx_rcc.h"
/* USART 外设初始化与配置函数 */
void myUSART_DeInit(USART_TypeDef* USARTx);                              // 复位USART外设，恢复默认设置
void myUSART_Init(USART_TypeDef* USARTx, USART_InitTypeDef* USART_InitStruct); // 初始化USART外设，配置波特率、数据位等
void myUSART_StructInit(USART_InitTypeDef* USART_InitStruct);            // 初始化USART配置结构体为默认值
void myUSART_ClockInit(USART_TypeDef* USARTx, USART_ClockInitTypeDef* USART_ClockInitStruct); // 配置USART时钟源
void myUSART_ClockStructInit(USART_ClockInitTypeDef* USART_ClockInitStruct); // 初始化USART时钟配置结构体为默认值
void myUSART_Cmd(USART_TypeDef* USARTx, FunctionalState NewState);       // 启用或禁用USART外设
void myUSART_SetPrescaler(USART_TypeDef* USARTx, uint8_t USART_Prescaler); // 设置USART的预分频器
void myUSART_OverSampling8Cmd(USART_TypeDef* USARTx, FunctionalState NewState); // 启用/禁用8倍采样
void myUSART_OneBitMethodCmd(USART_TypeDef* USARTx, FunctionalState NewState); // 启用/禁用单比特方法
void myUSART_SendData(USART_TypeDef* USARTx, uint16_t Data);            // 发送一个字节数据
uint16_t myUSART_ReceiveData(USART_TypeDef* USARTx);                    // 接收一个字节数据
void myUSART_SetAddress(USART_TypeDef* USARTx, uint8_t USART_Address);  // 设置USART的地址（用于多设备通信）
void myUSART_ReceiverWakeUpCmd(USART_TypeDef* USARTx, FunctionalState NewState); // 启用/禁用接收器唤醒功能
void myUSART_WakeUpConfig(USART_TypeDef* USARTx, uint16_t USART_WakeUp); // 配置USART唤醒源
void myUSART_LINBreakDetectLengthConfig(USART_TypeDef* USARTx, uint16_t USART_LINBreakDetectLength); // 配置LIN中断检测长度
void myUSART_LINCmd(USART_TypeDef* USARTx, FunctionalState NewState);    // 启用/禁用LIN模式
void myUSART_SendBreak(USART_TypeDef* USARTx);                          // 发送Break信号
void myUSART_HalfDuplexCmd(USART_TypeDef* USARTx, FunctionalState NewState); // 启用/禁用半双工模式
void myUSART_SetGuardTime(USART_TypeDef* USARTx, uint8_t USART_GuardTime); // 设置USART接收防护时间
void myUSART_SmartCardCmd(USART_TypeDef* USARTx, FunctionalState NewState); // 启用/禁用智能卡功能
void myUSART_IrDAConfig(USART_TypeDef* USARTx, uint16_t USART_IrDAMode); // 配置红外功能模式
void myUSART_SmartCardNACKCmd(USART_TypeDef* USARTx, FunctionalState NewState); // 启用/禁用智能卡NACK响应
void myUSART_IrDACmd(USART_TypeDef* USARTx, FunctionalState NewState);   // 启用/禁用红外功能
void myUSART_DMACmd(USART_TypeDef* USARTx, uint16_t USART_DMAReq, FunctionalState NewState); // 启用/禁用DMA传输
void myUSART_ITConfig(USART_TypeDef* USARTx, uint16_t USART_IT, FunctionalState NewState); // 启用/禁用USART中断
void myUSART_ClearFlag(USART_TypeDef* USARTx, uint16_t USART_FLAG);      // 清除USART标志
FlagStatus myUSART_GetFlagStatus(USART_TypeDef* USARTx, uint16_t USART_FLAG); // 获取USART标志状态
ITStatus myUSART_GetITStatus(USART_TypeDef* USARTx, uint16_t USART_IT); // 获取USART中断状态
void myUSART_ClearITPendingBit(USART_TypeDef* USARTx, uint16_t USART_IT);  // 清除USART中断挂起位



#endif