#ifndef MYSTM32F4_SYSCFG_H
#define MYSTM32F4_SYSCFG_H

#include "mystm32f4xx_syscfg.h"

void mySYSCFG_DeInit(void);                                       // 复位 SYSCFG 外设寄存器
void mySYSCFG_MemoryRemapConfig(uint8_t SYSCFG_MemoryRemap);      // 配置存储器重映射
void mySYSCFG_MemorySwappingBank(FunctionalState NewState);       // 切换内部 Flash Bank 映射
void mySYSCFG_EXTILineConfig(uint8_t EXTI_PortSourceGPIOx,uint8_t EXTI_PinSourcex); // 将 GPIO 引脚映射到 EXTI 线路
void mySYSCFG_CompensationCellCmd(FunctionalState NewState);      // 使能/关闭 I/O 补偿单元
void mySYSCFG_ETH_MediaInterfaceConfig(uint32_t SYSCFG_ETH_MediaInterface); // 配置以太网接口模式 (MII/RMII)
FlagStatus mySYSCFG_GetCompensationCellStatus(void);                // 获取补偿单元是否就绪

#endif

