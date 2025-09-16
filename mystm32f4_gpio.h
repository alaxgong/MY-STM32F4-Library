#ifndef __mystm32f4_gpio_H
#define __mystm32f4_gpio_H
#include "stm32f4xx_gpio.h"  // 官方头文件，提供结构体和寄存器定义
#ifdef __cplusplus
extern "C" {
#endif

/* GPIO 外设函数声明（右侧加简短功能说明） */

void myGPIO_DeInit(GPIO_TypeDef* GPIOx);                           // 将指定 GPIO 端口寄存器复位到默认状态
void myGPIO_Init(GPIO_TypeDef* GPIOx, GPIO_InitTypeDef* GPIO_InitStruct);  // 根据配置结构体初始化指定 GPIO 端口
void myGPIO_StructInit(GPIO_InitTypeDef* GPIO_InitStruct);        // 将 GPIO 初始化结构体填充为默认值
void myGPIO_PinLockConfig(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin); // 锁定指定 GPIO 引脚配置，防止修改
uint8_t myGPIO_ReadInputDataBit(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);  // 读取指定 GPIO 输入引脚的电平（高/低）
uint16_t myGPIO_ReadInputData(GPIO_TypeDef* GPIOx);                // 读取指定 GPIO 端口所有输入引脚的电平
uint8_t myGPIO_ReadOutputDataBit(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin); // 读取指定 GPIO 输出引脚的状态（高/低）
uint16_t myGPIO_ReadOutputData(GPIO_TypeDef* GPIOx);               // 读取指定 GPIO 端口所有输出引脚的状态
void myGPIO_SetBits(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);       // 将指定 GPIO 引脚输出置高电平
void myGPIO_ResetBits(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);     // 将指定 GPIO 引脚输出置低电平
void myGPIO_WriteBit(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, BitAction BitVal); // 设置指定 GPIO 引脚输出高或低
void myGPIO_Write(GPIO_TypeDef* GPIOx, uint16_t PortVal);          // 写入整个 GPIO 端口输出数据
void myGPIO_ToggleBits(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);    // 切换指定 GPIO 引脚的输出电平
void myGPIO_PinAFConfig(GPIO_TypeDef* GPIOx, uint16_t GPIO_PinSource, uint8_t GPIO_AF); // 配置指定 GPIO 引脚的复用功能
#ifdef __cplusplus
}
#endif

#endif
