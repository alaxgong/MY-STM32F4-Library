#ifndef __MYSTM32F4_RTC_H
#define __MYSTM32F4_RTC_H
#include "stm32f4xx_rtc.h"
ErrorStatus myRTC_DeInit(void);                                // 复位 RTC 外设
ErrorStatus myRTC_Init(RTC_InitTypeDef* RTC_InitStruct);       // 初始化 RTC
void myRTC_StructInit(RTC_InitTypeDef* RTC_InitStruct);        // 初始化 RTC_InitTypeDef 结构体
void myRTC_WriteProtectionCmd(FunctionalState NewState);      // 启用或禁用 RTC 写保护
ErrorStatus myRTC_EnterInitMode(void);                         // 进入 RTC 初始化模式
void myRTC_ExitInitMode(void);                                 // 退出 RTC 初始化模式
ErrorStatus myRTC_WaitForSynchro(void);                        // 等待 RTC 寄存器同步
ErrorStatus myRTC_RefClockCmd(FunctionalState NewState);       // 启用或禁用参考时钟检测
ErrorStatus myRTC_SetTime(uint32_t RTC_Format, RTC_TimeTypeDef* RTC_TimeStruct); // 设置 RTC 时间
void myRTC_TimeStructInit(RTC_TimeTypeDef* RTC_TimeStruct);   // 初始化 RTC_TimeTypeDef 结构体
void myRTC_GetTime(uint32_t RTC_Format, RTC_TimeTypeDef* RTC_TimeStruct);       // 获取 RTC 时间
uint32_t myRTC_GetSubSecond(void);                             // 获取 RTC 秒下分数
ErrorStatus myRTC_SetDate(uint32_t RTC_Format, RTC_DateTypeDef* RTC_DateStruct); // 设置 RTC 日期
void myRTC_DateStructInit(RTC_DateTypeDef* RTC_DateStruct);   // 初始化 RTC_DateTypeDef 结构体
void myRTC_GetDate(uint32_t RTC_Format, RTC_DateTypeDef* RTC_DateStruct);       // 获取 RTC 日期
void myRTC_AlarmStructInit(RTC_AlarmTypeDef* RTC_AlarmStruct);// 初始化 RTC_AlarmTypeDef 结构体
void myRTC_GetAlarm(uint32_t RTC_Format, uint32_t RTC_Alarm, RTC_AlarmTypeDef* RTC_AlarmStruct); // 获取闹钟设置
ErrorStatus myRTC_AlarmCmd(uint32_t RTC_Alarm, FunctionalState NewState);       // 启用或禁用闹钟
void myRTC_AlarmSubSecondConfig(uint32_t RTC_Alarm, uint32_t RTC_AlarmSubSecondValue, uint32_t RTC_AlarmSubSecondMask); // 配置闹钟子秒
uint32_t myRTC_GetAlarmSubSecond(uint32_t RTC_Alarm);           // 获取闹钟子秒值
void myRTC_WakeUpClockConfig(uint32_t RTC_WakeUpClock);        // 配置唤醒时钟
void myRTC_SetWakeUpCounter(uint32_t RTC_WakeUpCounter);       // 设置唤醒计数器
uint32_t myRTC_GetWakeUpCounter(void);                         // 获取唤醒计数器
ErrorStatus myRTC_WakeUpCmd(FunctionalState NewState);         // 启用或禁用唤醒功能
void myRTC_DayLightSavingConfig(uint32_t RTC_DayLightSaving, uint32_t RTC_StoreOperation); // 夏令时配置
uint32_t myRTC_GetStoreOperation(void);                        // 获取存储操作状态
void myRTC_OutputConfig(uint32_t RTC_Output, uint32_t RTC_OutputPolarity); // 配置 RTC 输出
ErrorStatus myRTC_CoarseCalibConfig(uint32_t RTC_CalibSign, uint32_t Value); // 配置粗调校
ErrorStatus myRTC_CoarseCalibCmd(FunctionalState NewState);    // 启用或禁用粗调校
void myRTC_CalibOutputCmd(FunctionalState NewState);           // 启用或禁用校准输出
void myRTC_CalibOutputConfig(uint32_t RTC_CalibOutput);        // 配置校准输出频率
ErrorStatus myRTC_SmoothCalibConfig(uint32_t RTC_SmoothCalibPeriod, uint32_t RTC_SmoothCalibPlusPulses, uint32_t RTC_SmouthCalibMinusPulsesValue); // 配置平滑校准
void myRTC_TimeStampCmd(uint32_t RTC_TimeStampEdge, FunctionalState NewState); // 启用或禁用时间戳功能
void myRTC_GetTimeStamp(uint32_t RTC_Format, RTC_TimeTypeDef* RTC_StampTimeStruct, RTC_DateTypeDef* RTC_StampDateStruct); // 获取时间戳
uint32_t myRTC_GetTimeStampSubSecond(void);                   // 获取时间戳子秒
void myRTC_TamperTriggerConfig(uint32_t RTC_Tamper, uint32_t RTC_TamperTrigger); // 配置防篡改触发边沿
void myRTC_TamperCmd(uint32_t RTC_Tamper, FunctionalState NewState); // 启用或禁用防篡改
void myRTC_TamperFilterConfig(uint32_t RTC_TamperFilter);      // 配置防篡改滤波
void myRTC_TamperSamplingFreqConfig(uint32_t RTC_TamperSamplingFreq); // 配置防篡改采样频率
void myRTC_TamperPinsPrechargeDuration(uint32_t RTC_TamperPrechargeDuration); // 配置防篡改预充电时间
void myRTC_TimeStampOnTamperDetectionCmd(FunctionalState NewState); // 防篡改检测时启用时间戳
void myRTC_TamperPullUpCmd(FunctionalState NewState);           // 启用或禁用防篡改上拉
void myRTC_WriteBackupRegister(uint32_t RTC_BKP_DR, uint32_t Data); // 写备份寄存器
uint32_t myRTC_ReadBackupRegister(uint32_t RTC_BKP_DR);          // 读备份寄存器
void myRTC_TamperPinSelection(uint32_t RTC_TamperPin);           // 选择防篡改引脚
void myRTC_TimeStampPinSelection(uint32_t RTC_TimeStampPin);     // 选择时间戳引脚
void myRTC_OutputTypeConfig(uint32_t RTC_OutputType);            // 配置 RTC 输出类型
ErrorStatus myRTC_SynchroShiftConfig(uint32_t RTC_ShiftAdd1S, uint32_t RTC_ShiftSubFS); // 配置同步移位
void myRTC_ITConfig(uint32_t RTC_IT, FunctionalState NewState);  // 配置 RTC 中断
FlagStatus myRTC_GetFlagStatus(uint32_t RTC_FLAG);               // 获取 RTC 标志状态
void myRTC_ClearFlag(uint32_t RTC_FLAG);                         // 清除 RTC 标志
ITStatus myRTC_GetITStatus(uint32_t RTC_IT);                     // 获取 RTC 中断状态
void myRTC_ClearITPendingBit(uint32_t RTC_IT);                   // 清除 RTC 中断挂起位

				

#endif