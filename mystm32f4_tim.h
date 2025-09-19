#ifndef mystm32f4_tim_h
#define mystm32f4_tim_h

#include "stm32f4xx_tim.h"

void myTIM_DeInit(TIM_TypeDef* TIMx);                                // 复位 TIM 外设到默认状态
void myTIM_TimeBaseInit(TIM_TypeDef* TIMx, TIM_TimeBaseInitTypeDef* TIM_TimeBaseInitStruct); // 初始化基本定时器参数
void myTIM_TimeBaseStructInit(TIM_TimeBaseInitTypeDef* TIM_TimeBaseInitStruct); // 初始化 TIM_TimeBaseInitTypeDef 结构体默认值
void myTIM_PrescalerConfig(TIM_TypeDef* TIMx, uint16_t Prescaler, uint16_t TIM_PSCReloadMode); // 配置定时器预分频
void myTIM_CounterModeConfig(TIM_TypeDef* TIMx, uint16_t TIM_CounterMode); // 配置计数模式（向上/向下/中心）
void myTIM_SetCounter(TIM_TypeDef* TIMx, uint32_t Counter);           // 设置计数器当前值
void myTIM_SetAutoreload(TIM_TypeDef* TIMx, uint32_t Autoreload);    // 设置自动重载值
uint32_t myTIM_GetCounter(TIM_TypeDef* TIMx);                         // 获取计数器当前值
uint16_t myTIM_GetPrescaler(TIM_TypeDef* TIMx);                       // 获取预分频值
void myTIM_UpdateDisableConfig(TIM_TypeDef* TIMx, FunctionalState NewState); // 使能/禁止更新事件
void myTIM_UpdateRequestConfig(TIM_TypeDef* TIMx, uint16_t TIM_UpdateSource); // 配置更新事件源
void myTIM_ARRPreloadConfig(TIM_TypeDef* TIMx, FunctionalState NewState);    // 使能/禁止自动重载寄存器预装载
void myTIM_SelectOnePulseMode(TIM_TypeDef* TIMx, uint16_t TIM_OPMode);       // 配置单脉冲模式
void myTIM_SetClockDivision(TIM_TypeDef* TIMx, uint16_t TIM_CKD);            // 配置时钟分频
void myTIM_Cmd(TIM_TypeDef* TIMx, FunctionalState NewState);                 // 启动或停止定时器

void myTIM_OCxInit(TIM_TypeDef* TIMx, uint8_t channel, TIM_OCInitTypeDef* TIM_OCInitStruct); // 配置指定通道输出比较
void myTIM_OC2Init(TIM_TypeDef* TIMx, TIM_OCInitTypeDef* TIM_OCInitStruct); // 配置通道 2
void myTIM_OC3Init(TIM_TypeDef* TIMx, TIM_OCInitTypeDef* TIM_OCInitStruct); // 配置通道 3
void myTIM_OC4Init(TIM_TypeDef* TIMx, TIM_OCInitTypeDef* TIM_OCInitStruct); // 配置通道 4
void myTIM_OCStructInit(TIM_OCInitTypeDef* TIM_OCInitStruct);               // 初始化 TIM_OCInitTypeDef 默认值
void myTIM_SelectOCxM(TIM_TypeDef* TIMx, uint16_t TIM_Channel, uint16_t TIM_OCMode); // 配置输出比较模式
void myTIM_SetCompare(TIM_TypeDef* TIMx, uint8_t channel, uint32_t Compare);         // 设置比较值
void myTIM_ForcedOCConfig(TIM_TypeDef* TIMx, uint8_t channel, uint16_t TIM_ForcedAction); // 强制输出状态
void myTIM_OCPreloadConfig(TIM_TypeDef* TIMx, uint8_t Channel, uint16_t TIM_OCPreload); // 配置比较寄存器预装载
void myTIM_OCFastConfig(TIM_TypeDef* TIMx, uint8_t Channel, uint16_t TIM_OCFast);       // 配置快速响应
void myTIM_OCClearConfig(TIM_TypeDef* TIMx, uint8_t Channel, uint16_t TIM_OCClear);     // 配置输出清零功能
void myTIM_OCPolarityConfig(TIM_TypeDef* TIMx, uint8_t Channel, uint16_t Polarity);    // 配置输出极性
void myTIM_CCxCmd(TIM_TypeDef* TIMx, uint8_t Channel, uint16_t Cmd);                   // 使能/禁用指定通道输出

void myTIM_ICStructInit(TIM_ICInitTypeDef* TIM_ICInitStruct);                         // 初始化输入捕获结构体
void myTIM_ICInit(TIM_TypeDef* TIMx, TIM_ICInitTypeDef* TIM_ICInitStruct);            // 初始化输入捕获
void myTIM_PWMIConfig(TIM_TypeDef* TIMx, TIM_ICInitTypeDef* TIM_ICInitStruct);       // 配置脉宽测量模式
uint32_t myTIM_GetCapture(TIM_TypeDef* TIMx, uint8_t Channel);                        // 获取输入捕获值
void myTIM_SetICPrescaler(TIM_TypeDef* TIMx, uint8_t Channel, uint16_t Prescaler);   // 配置输入捕获预分频

void myTIM_BDTRConfig(TIM_TypeDef* TIMx, TIM_BDTRInitTypeDef* TIM_BDTRInitStruct);   // 配置死区和刹车功能
void myTIM_BDTRStructInit(TIM_BDTRInitTypeDef* TIM_BDTRInitStruct);                  // 初始化 BDTR 结构体默认值
void myTIM_CtrlPWMOutputs(TIM_TypeDef* TIMx, FunctionalState NewState);             // 控制 PWM 输出使能
void myTIM_SelectCOM(TIM_TypeDef* TIMx, FunctionalState NewState);                  // 配置通用输出使能
void myTIM_CCPreloadControl(TIM_TypeDef* TIMx, FunctionalState NewState);           // 配置捕获/比较预装载

void myTIM_ITConfig(TIM_TypeDef* TIMx, uint16_t TIM_IT, FunctionalState NewState);  // 配置中断
void myTIM_GenerateEvent(TIM_TypeDef* TIMx, uint16_t TIM_EventSource);              // 产生事件
FlagStatus myTIM_GetFlagStatus(TIM_TypeDef* TIMx, uint16_t TIM_FLAG);               // 获取标志位状态
void myTIM_DMACmd(TIM_TypeDef* TIMx, uint16_t TIM_DMASource, FunctionalState NewState); // 配置 DMA 使能
void myTIM_DMAConfig(TIM_TypeDef* TIMx, uint16_t TIM_DMABase, uint16_t TIM_DMABurstLength); // 配置 DMA 传输
void myTIM_ClearITPendingBit(TIM_TypeDef* TIMx, uint16_t TIM_IT);                   // 清中断标志
ITStatus myTIM_GetITStatus(TIM_TypeDef* TIMx, uint16_t TIM_IT);                      // 获取中断状态
void myTIM_ClearFlag(TIM_TypeDef* TIMx, uint16_t TIM_FLAG);                          // 清标志位
void myTIM_SelectCCDMA(TIM_TypeDef* TIMx, FunctionalState NewState);                 // 配置捕获/比较 DMA 请求

void myTIM_TIxExternalClockConfig(TIM_TypeDef* TIMx, uint16_t TIM_TIxExternalCLKSource, // 外部时钟模式配置
    uint16_t TIM_ICPolarity, uint16_t ICFilter);
void myTIM_ITRxExternalClockConfig(TIM_TypeDef* TIMx, uint16_t TIM_InputTriggerSource); // 内部触发外部时钟
void myTIM_InternalClockConfig(TIM_TypeDef* TIMx);                                     // 配置内部时钟
void TIM_ETRClockMode2Config(TIM_TypeDef* TIMx, uint16_t TIM_ExtTRGPrescaler,          // 外部触发时钟模式2
    uint16_t TIM_ExtTRGPolarity, uint16_t ExtTRGFilter);
void TIM_ETRClockMode1Config(TIM_TypeDef* TIMx, uint16_t TIM_ExtTRGPrescaler,          // 外部触发时钟模式1
    uint16_t TIM_ExtTRGPolarity, uint16_t ExtTRGFilter);

void myTIM_ETRConfig(TIM_TypeDef* TIMx, uint16_t TIM_ExtTRGPrescaler,                  // 配置外部触发器
    uint16_t TIM_ExtTRGPolarity, uint16_t ExtTRGFilter);
void myTIM_SelectMasterSlaveMode(TIM_TypeDef* TIMx, uint16_t TIM_MasterSlaveMode);     // 配置主从模式
void myTIM_SelectSlaveMode(TIM_TypeDef* TIMx, uint16_t TIM_SlaveMode);                 // 配置从模式
void myTIM_SelectOutputTrigger(TIM_TypeDef* TIMx, uint16_t TIM_TRGOSource);            // 配置触发输出源
void myTIM_SelectInputTrigger(TIM_TypeDef* TIMx, uint16_t TIM_InputTriggerSource);     // 配置输入触发源
void myTIM_SelectHallSensor(TIM_TypeDef* TIMx, FunctionalState NewState);             // 使能/禁止霍尔传感器接口
void TIM_EncoderInterfaceConfig(TIM_TypeDef* TIMx, uint16_t TIM_EncoderMode,          // 配置编码器接口
    uint16_t TIM_IC1Polarity, uint16_t TIM_IC2Polarity);
void myTIM_RemapConfig(TIM_TypeDef* TIMx, uint16_t TIM_Remap);                         // 配置定时器重映射

static void myTIx_Config(TIM_TypeDef* TIMx, uint8_t channel,                              // 配置通道 TI1~TI4 输入
    uint16_t TIM_ICPolarity, uint16_t TIM_ICSelection,
    uint16_t TIM_ICFilter);

#endif


