/**
  ******************************************************************************
  * @file     mystm32f4_tim.c
  * @author   QiangGu
  * @version  V1.0.0
  * @date     2025-09-16
  * @brief    STM32F4系列定时器（TIM）外设驱动库（重写版）
  *
  * @attention
  *
  * 本文件为 STM32F4xx 标准外设库 TIM 模块的自定义重写版本，编写过程中旨在加深对
  * STM32 编程和定时器工作机制的研究理解。
  *
  * 此重写版本的目标：
  * 1. 提供更清晰的代码结构和命名规范
  * 2. 添加更详细的中文注释，便于学习理解
  * 3. 优化部分功能实现，提高代码可读性
  * 4. 保持与官方库函数的兼容性
  *
  * 与官方库的区别：
  * - 使用更现代的C语言特性（如C99标准）
  * - 增加更多的参数检查和错误处理
  * - 提供更详细的函数说明和使用示例
  * - 代码结构更模块化，便于理解和扩展
  *
  * 使用说明：
  * 1. 本库函数设计为与 STM32F4xx 系列微控制器配合使用
  * 2. 在使用任何 TIM 函数前，必须首先使能对应 TIM 外设时钟
  * 3. 使用 TIM_Init() 函数配置计数器模式、自动重载值、预分频、计数方向等
  * 4. 可配置 TIM 中断或 DMA 请求，并在 NVIC 中设置中断优先级
  *
  * 本库提供的功能：
  * - TIM 初始化与配置
  * - TIM 使能/禁用
  * - 更新中断、捕获/比较中断配置
  * - 自动重载值和预分频配置
  * - 支持基本计数模式、PWM 输出、单脉冲模式等
  *
  * 版本历史：
  * V1.0.0 初始版本
  *   - 实现基本的 TIM 初始化、计数器使能功能
  *   - 提供完整的模式和中断配置支持
  *   - 添加详细的错误检查和调试支持
  *
  * 免责声明：
  * 本软件按“原样”提供，不提供任何明示或暗示的担保，包括但不限于对适销性或特定用途适用性的保证。
  * 作者不对因使用本软件而产生的任何直接、间接、偶然、特殊、示例性或后果性损失承担责任。
  * 本软件仅供学习和研究使用，无偿共享，禁止用于商业用途。
  *
  ******************************************************************************
  */


/*
@verbatim
================================================================================
##### 使用本驱动程序的方法 #####
================================================================================
[..]
本驱动提供了用于配置和编程 STM32F4xx 系列设备中 TIM 外设的函数。
这些函数被分为 9 个功能组：

(#) TIM 时间基准管理：此组包含配置 TIM 时间基准单元所需的所有函数：
(++) 设置 / 获取预分频器
(++) 设置 / 获取自动重载寄存器
(++) 计数器模式配置
(++) 设置时钟分频
(++) 选择单脉冲模式
(++) 更新请求配置
(++) 禁用 / 启用更新事件
(++) 自动预装载配置
(++) 启用 / 禁用计数器

(#) TIM 输出比较管理：此组包含配置捕获 / 比较单元用于输出比较模式所需的所有函数：
(++) 独立配置每个通道的输出比较模式
(++) 选择输出比较模式
(++) 选择每个通道的极性
(++) 设置 / 获取捕获 / 比较寄存器值
(++) 选择输出比较快速模式
(++) 选择输出比较强制模式
(++) 输出比较预装载配置
(++) 清除输出比较参考值
(++) 选择 OCREF 清除信号
(++) 启用 / 禁用捕获 / 比较通道

(#) TIM 输入捕获管理：此组包含配置捕获 / 比较单元用于输入捕获模式所需的所有函数：
(++) 配置每个通道为输入捕获模式
(++) 配置通道 1 / 2 为 PWM 输入模式
(++) 设置输入捕获预分频器
(++) 获取捕获 / 比较寄存器值

(#) 高级定时器（TIM1 和 TIM8）特性：
(++) 配置断路输入、死区时间、锁定等级、OSSI、OSSR 状态及自动输出使能（AOE）
(++) 启用 / 禁用 TIM 外设主输出
(++) 选择换相事件
(++) 设置 / 复位捕获比较预装载控制位

(#) TIM 中断、DMA 和标志管理：
(++) 启用 / 禁用中断源
(++) 获取标志状态
(++) 清除标志 / 挂起位
(++) 启用 / 禁用 DMA 请求
(++) 配置 DMA 突发模式
(++) 选择捕获比较 DMA 请求

(#) TIM 时钟管理：此组包含配置时钟控制单元所需的所有函数：
(++) 选择内部 / 外部时钟
(++) 选择外部时钟模式：ETR（模式1 / 模式2）、TIx 或 ITRx

(#) TIM 同步管理：此组包含配置同步单元所需的所有函数：
(++) 选择输入触发
(++) 选择输出触发
(++) 选择主从模式
(++) 外部触发 ETR 配置（当作为外部触发使用时）

(#) TIM 特定接口管理：此组包含使用 TIM 特定接口所需的所有函数：
(++) 编码器接口配置
(++) 选择霍尔传感器

(#) TIM 特定重映射管理：此组包含特定定时器的重映射配置
@endverbatim
*/



/* Includes ------------------------------------------------------------------*/
#include "mystm32f4_tim.h"
#include "stm32f4xx_rcc.h"

/** @addtogroup STM32F4xx_StdPeriph_Driver
  * @{
  */

  /** @defgroup TIM
    * @brief TIM 驱动模块
    * @{
    */

    /* Private typedef -----------------------------------------------------------*/
    /* 私有类型定义区，可用于自定义结构体或枚举类型 */

    /* Private define ------------------------------------------------------------*/
    /* 私有宏定义区，通常定义寄存器位掩码、偏移量等 */

    /* ---------------------- TIM 寄存器位掩码 ------------------------ */
#define SMCR_ETR_MASK      ((uint16_t)0x00FF)  /* 用于过滤 SMCR 寄存器中的 ETR 位 */
#define CCMR_OFFSET        ((uint16_t)0x0018)  /* CCMR 寄存器偏移量 */
#define CCER_CCE_SET       ((uint16_t)0x0001)  /* CCER 寄存器中开启捕获比较通道 1 的位 */
#define	CCER_CCNE_SET      ((uint16_t)0x0004)  /* CCER 寄存器中开启捕获比较通道互补输出的位 */
#define CCMR_OC13M_MASK    ((uint16_t)0xFF8F)  /* CCMR 寄存器中通道1/3 模式掩码，用于清除对应位 */
#define CCMR_OC24M_MASK    ((uint16_t)0x8FFF)  /* CCMR 寄存器中通道2/4 模式掩码，用于清除对应位 */

/* Private macro -------------------------------------------------------------*/
/* 私有宏函数区，可定义操作寄存器的辅助宏 */

/* Private variables ---------------------------------------------------------*/
/* 私有变量区，可定义 TIM 内部使用的静态变量 */

/* Private function prototypes -----------------------------------------------*/
/* 私有函数原型声明区，用于内部函数封装，不对外暴露 */
static void TI1_Config(TIM_TypeDef* TIMx, uint16_t TIM_ICPolarity, uint16_t TIM_ICSelection,
    uint16_t TIM_ICFilter);  /* 配置 TIM 的通道1 输入捕获参数：极性、选择、滤波器 */
static void TI2_Config(TIM_TypeDef* TIMx, uint16_t TIM_ICPolarity, uint16_t TIM_ICSelection,
    uint16_t TIM_ICFilter);  /* 配置 TIM 的通道2 输入捕获参数 */
static void TI3_Config(TIM_TypeDef* TIMx, uint16_t TIM_ICPolarity, uint16_t TIM_ICSelection,
    uint16_t TIM_ICFilter);  /* 配置 TIM 的通道3 输入捕获参数 */
static void TI4_Config(TIM_TypeDef* TIMx, uint16_t TIM_ICPolarity, uint16_t TIM_ICSelection,
    uint16_t TIM_ICFilter);  /* 配置 TIM 的通道4 输入捕获参数 */


/**
  * @brief  将指定的 TIMx 外设寄存器复位为默认值。
  * @param  TIMx: 指定要复位的定时器外设，范围 TIM1~TIM14。
  * @retval None
  *
  * @note
  *  - 本函数通过 RCC_APB1/APB2 外设复位寄存器来复位 TIM。
  *    即先使能复位信号（ENABLE），然后释放复位（DISABLE）。
  *  - 不同 TIM 外设位于不同的 APB 总线上：
  *      TIM1/TIM8/TIM9/TIM10/TIM11 -> APB2
  *      TIM2~TIM7/TIM12~TIM14 -> APB1
  *  - 调用此函数相当于把定时器寄存器全部清零，等同于上电复位。
  *  - 如果定时器正在运行，复位会立即停止计数器。
  *
  * @attention
  *  - 使用前请确保 TIMx 参数有效（TIM1~TIM14）。
  *  - 本函数不会影响其他外设寄存器，只复位指定 TIMx。
  *
  * @example
  *  TIM_DeInit(TIM2);  // 复位 TIM2 外设寄存器
  */
void myTIM_DeInit(TIM_TypeDef* TIMx)
{


    /* TIM 外设与 RCC 复位宏映射表 */
    typedef struct {
        TIM_TypeDef* TIMx;       /* 定时器外设指针 */
        uint32_t RCC_APBPeriph;  /* 对应 RCC APB 外设复位宏 */
        uint8_t  APB;            /* 1 = APB1 总线, 2 = APB2 总线 */
    } TIM_ResetMap;

    const TIM_ResetMap tim_map[] = {
        {TIM1,  RCC_APB2Periph_TIM1, 2},
        {TIM2,  RCC_APB1Periph_TIM2, 1},
        {TIM3,  RCC_APB1Periph_TIM3, 1},
        {TIM4,  RCC_APB1Periph_TIM4, 1},
        {TIM5,  RCC_APB1Periph_TIM5, 1},
        {TIM6,  RCC_APB1Periph_TIM6, 1},
        {TIM7,  RCC_APB1Periph_TIM7, 1},
        {TIM8,  RCC_APB2Periph_TIM8, 2},
        {TIM9,  RCC_APB2Periph_TIM9, 2},
        {TIM10, RCC_APB2Periph_TIM10,2},
        {TIM11, RCC_APB2Periph_TIM11,2},
        {TIM12, RCC_APB1Periph_TIM12,1},
        {TIM13, RCC_APB1Periph_TIM13,1},
        {TIM14, RCC_APB1Periph_TIM14,1}
    };
    /* 检查 TIMx 参数是否有效 */
    assert_param(IS_TIM_ALL_PERIPH(TIMx));
    /* 遍历映射表，找到匹配的 TIMx 并执行复位 */
    for (int i = 0; i < sizeof(tim_map) / sizeof(TIM_ResetMap); i++)
    {
        if (tim_map[i].TIMx == TIMx)
        {
            /* 三目运算符 + 逗号运算符：
               根据 TIM 所在 APB 总线选择 RCC 外设复位函数，
               先 ENABLE 再 DISABLE，实现寄存器复位
            */
            tim_map[i].APB == 1 ?
                (RCC_APB1PeriphResetCmd(tim_map[i].RCC_APBPeriph, ENABLE),
                    RCC_APB1PeriphResetCmd(tim_map[i].RCC_APBPeriph, DISABLE))
                :
                (RCC_APB2PeriphResetCmd(tim_map[i].RCC_APBPeriph, ENABLE),
                    RCC_APB2PeriphResetCmd(tim_map[i].RCC_APBPeriph, DISABLE));

            break; /* 找到匹配 TIMx 后退出循环 */
        }
    }
}



/**
  * @brief  初始化指定 TIMx 的 Time Base 单元
  * @param  TIMx: 指定的定时器外设，范围 TIM1~TIM14
  * @param  TIM_TimeBaseInitStruct: 指向 TIM_TimeBaseInitTypeDef 结构体的指针，
  *         包含定时器初始化参数：
  *           - TIM_Period：自动重装载值
  *           - TIM_Prescaler：预分频器
  *           - TIM_ClockDivision：时钟分频
  *           - TIM_CounterMode：计数模式（向上、向下或中心对齐）
  *           - TIM_RepetitionCounter：重复计数器，仅 TIM1/TIM8 使用
  * @retval None
  *
  * @note
  *  - 本函数配置定时器的基本计数单元，包括计数模式、时钟分频、
  *    预分频器和自动重装载寄存器。
  *  - 对于高级定时器（TIM1/TIM8），会配置重复计数器。
  *  - 对于基本定时器（TIM6/TIM7），不配置计数模式和时钟分频。
  *  - 调用此函数后会生成一次更新事件，立即加载预分频器和重复计数器值。
  *
  * @example
  *  TIM_TimeBaseInitTypeDef TIM_InitStruct;
  *  TIM_InitStruct.TIM_Period = 1000 - 1;
  *  TIM_InitStruct.TIM_Prescaler = 84 - 1;
  *  TIM_InitStruct.TIM_ClockDivision = TIM_CKD_DIV1;
  *  TIM_InitStruct.TIM_CounterMode = TIM_CounterMode_Up;
  *  TIM_InitStruct.TIM_RepetitionCounter = 0;
  *  TIM_TimeBaseInit(TIM1, &TIM_InitStruct);
  */
void myTIM_TimeBaseInit(TIM_TypeDef* TIMx, TIM_TimeBaseInitTypeDef* TIM_TimeBaseInitStruct)
{

    uint16_t tmpcr1 = TIMx->CR1;

    /* 定义定时器类别映射表 */
    typedef struct {
        TIM_TypeDef* TIMx;
        uint8_t isAdvanced;   /* 是否高级定时器 TIM1/TIM8 */
        uint8_t hasCounterMode; /* 是否有计数模式寄存器（非 TIM6/TIM7） */
    } TIM_TypeMap;

    const TIM_TypeMap tim_map[] = {
        {TIM1,  1, 1},
        {TIM2,  0, 1},
        {TIM3,  0, 1},
        {TIM4,  0, 1},
        {TIM5,  0, 1},
        {TIM6,  0, 0},
        {TIM7,  0, 0},
        {TIM8,  1, 1},
        {TIM9,  0, 1},
        {TIM10, 0, 1},
        {TIM11, 0, 1},
        {TIM12, 0, 1},
        {TIM13, 0, 1},
        {TIM14, 0, 1}
    };


    /* 检查参数有效性 */
    assert_param(IS_TIM_ALL_PERIPH(TIMx));
    assert_param(IS_TIM_COUNTER_MODE(TIM_TimeBaseInitStruct->TIM_CounterMode));
    assert_param(IS_TIM_CKD_DIV(TIM_TimeBaseInitStruct->TIM_ClockDivision));
    /* 遍历映射表找到 TIMx 对应类型 */
    uint8_t isAdvanced = 0;
    uint8_t hasCounterMode = 0;
    for (int i = 0; i < sizeof(tim_map) / sizeof(TIM_TypeMap); i++) {
        if (tim_map[i].TIMx == TIMx) {
            isAdvanced = tim_map[i].isAdvanced;
            hasCounterMode = tim_map[i].hasCounterMode;
            break;
        }
    }

    /* 配置计数模式（非 TIM6/TIM7） */
    if (hasCounterMode) {
        tmpcr1 &= ~(TIM_CR1_DIR | TIM_CR1_CMS);  /* 清除计数模式位 */
        tmpcr1 |= TIM_TimeBaseInitStruct->TIM_CounterMode;
    }

    /* 配置时钟分频（非 TIM6/TIM7） */
    if (hasCounterMode) {
        tmpcr1 &= ~TIM_CR1_CKD;  /* 清除分频位 */
        tmpcr1 |= TIM_TimeBaseInitStruct->TIM_ClockDivision;
    }

    TIMx->CR1 = tmpcr1;

    /* 设置自动重装载值 */
    TIMx->ARR = TIM_TimeBaseInitStruct->TIM_Period;

    /* 设置预分频器 */
    TIMx->PSC = TIM_TimeBaseInitStruct->TIM_Prescaler;

    /* 高级定时器设置重复计数器 */
    if (isAdvanced) {
        TIMx->RCR = TIM_TimeBaseInitStruct->TIM_RepetitionCounter;
    }

    /* 立即生成更新事件，重新加载预分频器和重复计数器 */
    TIMx->EGR = TIM_PSCReloadMode_Immediate;
}



/**
  * @brief  用默认值初始化 TIM_TimeBaseInitTypeDef 结构体。
  * @param  TIM_TimeBaseInitStruct : 指向 TIM_TimeBaseInitTypeDef 结构体的指针，
  *         将被填充为默认配置值。
  * @retval None
  *
  * @note
  *  - 本函数不对 TIM 外设寄存器进行任何操作，仅初始化结构体变量。
  *  - 用户在调用 TIM_TimeBaseInit() 前，可先调用此函数获取默认配置，
  *    然后根据需要修改部分成员。
  *
  * @example
  *  TIM_TimeBaseInitTypeDef TIM_InitStruct;
  *  TIM_TimeBaseStructInit(&TIM_InitStruct); // 先获取默认值
  *  TIM_InitStruct.TIM_Period = 1000 - 1;    // 修改自动重装载值
  *  TIM_InitStruct.TIM_Prescaler = 84 - 1;   // 修改预分频器
  *  TIM_TimeBaseInit(TIM2, &TIM_InitStruct); // 初始化 TIM2
  */
void myTIM_TimeBaseStructInit(TIM_TimeBaseInitTypeDef* TIM_TimeBaseInitStruct)
{
    /* 设置结构体成员为默认值 */
    TIM_TimeBaseInitStruct->TIM_Period = 0xFFFFFFFF;          /* 自动重装载寄存器默认最大值 */
    TIM_TimeBaseInitStruct->TIM_Prescaler = 0x0000;           /* 预分频器默认值 = 0，即不分频 */
    TIM_TimeBaseInitStruct->TIM_ClockDivision = TIM_CKD_DIV1; /* 时钟分频默认为 1 */
    TIM_TimeBaseInitStruct->TIM_CounterMode = TIM_CounterMode_Up; /* 计数器模式默认向上计数 */
    TIM_TimeBaseInitStruct->TIM_RepetitionCounter = 0x0000;   /* 重复计数器默认 0，仅高级定时器使用 */
}


/**
  * @brief  配置指定 TIMx 的预分频器（Prescaler）。
  * @param  TIMx: 指定的定时器外设，范围 TIM1~TIM14。
  * @param  Prescaler: 预分频器寄存器值（PSC），计数器时钟 = TIMxCLK / (Prescaler+1)
  * @param  TIM_PSCReloadMode: 指定预分频器重装载模式
  *          取值可以是：
  *            @arg TIM_PSCReloadMode_Update: 在下一个更新事件（UEV）时加载 PSC 值
  *            @arg TIM_PSCReloadMode_Immediate: 立即加载 PSC 值
  * @retval None
  *
  * @note
  *  - PSC 用于将定时器输入时钟进一步分频，以获得计数器时钟 f_CNT。
  *    计数器时钟计算公式：
  *      f_CNT = TIMxCLK / (PSC + 1)
  *  - TIM_PSCReloadMode 决定 PSC 值什么时候生效：
  *      Update 模式：在下一个更新事件时生效
  *      Immediate 模式：立即生效
  *
  * @example
  *  TIM_PrescalerConfig(TIM2, 8399, TIM_PSCReloadMode_Immediate); // TIM2 计数器时钟 = TIM2CLK / 8400
  */
void myTIM_PrescalerConfig(TIM_TypeDef* TIMx, uint16_t Prescaler, uint16_t TIM_PSCReloadMode)
{
    /* 参数检查 */
    assert_param(IS_TIM_ALL_PERIPH(TIMx));
    assert_param(IS_TIM_PRESCALER_RELOAD(TIM_PSCReloadMode));

    /* 设置预分频器寄存器 */
    TIMx->PSC = Prescaler;

    /* 生成更新事件，立即或延迟加载 PSC */
    TIMx->EGR = TIM_PSCReloadMode;
}

/**
  * @brief  配置指定 TIMx 的计数模式（Counter Mode）。
  * @param  TIMx: 指定的定时器外设，可选 TIM1, TIM2, TIM3, TIM4, TIM5 或 TIM8。
  * @param  TIM_CounterMode: 计数模式选择
  *          可选值：
  *            @arg TIM_CounterMode_Up: 向上计数模式
  *            @arg TIM_CounterMode_Down: 向下计数模式
  *            @arg TIM_CounterMode_CenterAligned1: 中心对齐模式1
  *            @arg TIM_CounterMode_CenterAligned2: 中心对齐模式2
  *            @arg TIM_CounterMode_CenterAligned3: 中心对齐模式3
  * @retval None
  *
  * @note
  *  - 计数器模式决定定时器计数的方向和方式：
  *      Up: 计数器从 0 计数到 ARR
  *      Down: 计数器从 ARR 计数到 0
  *      CenterAligned: 计数器上下计数（PWM模式常用）
  *  - 对 TIMx->CR1 寄存器中的 DIR 和 CMS 位进行配置
  *  - 一般在初始化时调用此函数
  *
  * @example
  *  TIM_CounterModeConfig(TIM3, TIM_CounterMode_Up); // TIM3 使用向上计数模式
  */
void myTIM_CounterModeConfig(TIM_TypeDef* TIMx, uint16_t TIM_CounterMode)
{
    uint16_t tmpcr1 = 0;

    /* 参数检查 */
    assert_param(IS_TIM_LIST3_PERIPH(TIMx));
    assert_param(IS_TIM_COUNTER_MODE(TIM_CounterMode));

    tmpcr1 = TIMx->CR1;

    /* 清除 DIR 和 CMS 位 */
    tmpcr1 &= (uint16_t)~(TIM_CR1_DIR | TIM_CR1_CMS);

    /* 设置计数模式 */
    tmpcr1 |= TIM_CounterMode;

    /* 写入 CR1 寄存器 */
    TIMx->CR1 = tmpcr1;
}


/**
  * @brief  设置 TIMx 的计数器寄存器 CNT 的值
  * @param  TIMx: 指定的定时器外设，范围 TIM1~TIM14
  * @param  Counter: 要写入计数器的新值（0~ARR）
  * @retval None
  *
  * @note
  *  - CNT 寄存器记录当前计数值，定时器计数器每个时钟周期自动递增或递减
  *  - 调用此函数可手动修改计数器值
  *  - 对于上/下计数模式，Counter 的值必须在 0~ARR 之间，否则可能导致计数异常
  *
  * @example
  *  TIM_SetCounter(TIM3, 0); // 将 TIM3 计数器清零
  */
void myTIM_SetCounter(TIM_TypeDef* TIMx, uint32_t Counter)
{
    /* 参数检查 */
    assert_param(IS_TIM_ALL_PERIPH(TIMx));

    /* 设置计数器寄存器 CNT */
    TIMx->CNT = Counter;
}

/**
  * @brief  设置 TIMx 的自动重装载寄存器 ARR 的值
  * @param  TIMx: 指定的定时器外设，范围 TIM1~TIM14
  * @param  Autoreload: 自动重装载寄存器的新值
  * @retval None
  *
  * @note
  *  - ARR 决定计数器的周期：计数器计数到 ARR 后会产生更新事件（UEV）
  *  - 上计数模式：CNT 从 0 计数到 ARR
  *  - 下计数模式：CNT 从 ARR 计数到 0
  *  - 中心对齐模式：CNT 从 0 到 ARR 再回到 0
  *  - 调用此函数后计数周期改变，影响定时器输出频率
  *
  * @example
  *  TIM_SetAutoreload(TIM3, 999); // 设置 TIM3 自动重装载值为 999
  */
void myTIM_SetAutoreload(TIM_TypeDef* TIMx, uint32_t Autoreload)
{
    /* 参数检查 */
    assert_param(IS_TIM_ALL_PERIPH(TIMx));

    /* 设置自动重装载寄存器 ARR */
    TIMx->ARR = Autoreload;
}

/**
  * @brief  获取 TIMx 当前计数器 CNT 的值
  * @param  TIMx: 指定的定时器外设，范围 TIM1~TIM14
  * @retval 当前计数器寄存器 CNT 的值
  *
  * @note
  *  - 返回当前计数值，便于读取定时器状态或测量时间
  *
  * @example
  *  uint32_t count = TIM_GetCounter(TIM3); // 获取 TIM3 当前计数值
  */
uint32_t myTIM_GetCounter(TIM_TypeDef* TIMx)
{
    /* 参数检查 */
    assert_param(IS_TIM_ALL_PERIPH(TIMx));

    /* 读取计数器寄存器 CNT */
    return TIMx->CNT;
}

/**
  * @brief  获取 TIMx 的预分频器 PSC 的值
  * @param  TIMx: 指定的定时器外设，范围 TIM1~TIM14
  * @retval 预分频器寄存器 PSC 的值
  *
  * @note
  *  - PSC 决定计数器时钟 f_CNT = TIMxCLK / (PSC + 1)
  *  - 可通过此函数读取当前分频值，用于计算定时器实际计数频率
  *
  * @example
  *  uint16_t psc = TIM_GetPrescaler(TIM3); // 获取 TIM3 预分频器值
  */
uint16_t myTIM_GetPrescaler(TIM_TypeDef* TIMx)
{
    /* 参数检查 */
    assert_param(IS_TIM_ALL_PERIPH(TIMx));

    /* 读取预分频器寄存器 PSC */
    return TIMx->PSC;
}



/**
  * @brief  开关 TIMx 的“更新事件”（Update Event）。
  * @param  TIMx: 要操作的定时器（TIM1~TIM14）。
  * @param  NewState: 新状态，控制更新事件开关
  *         - ENABLE  : 关闭更新事件（计数器溢出时不会触发中断/更新）
  *         - DISABLE : 打开更新事件（计数器溢出时会触发更新）
  * @retval None
  *
  * @note
  *  - 定时器计数器每次达到自动重载值（ARR）或手动触发 UG 时，会产生“更新事件”。
  *    更新事件会刷新计数器、触发中断或 DMA。
  *  - TIM_CR1_UDIS 位用来控制更新事件：
  *      1 = 禁止更新事件（计数器溢出不会触发更新）
  *      0 = 允许更新事件（计数器溢出会触发更新）
  *  - 如果你临时不想让计数器触发中断或更新，可以设置 ENABLE。
  *  - 用 DISABLE 恢复正常行为。
  *
  * @example
  *  TIM_UpdateDisableConfig(TIM2, ENABLE);   // 暂时屏蔽 TIM2 更新事件
  *  TIM_UpdateDisableConfig(TIM2, DISABLE);  // 恢复 TIM2 更新事件
  */
void myTIM_UpdateDisableConfig(TIM_TypeDef* TIMx, FunctionalState NewState)
{
    /* 1. 检查 TIMx 是否有效，以及 NewState 是否是 ENABLE/DISABLE */
    assert_param(IS_TIM_ALL_PERIPH(TIMx));
    assert_param(IS_FUNCTIONAL_STATE(NewState));

    /* 2. 设置或清除 UDIS 位
       小白理解：
       - ENABLE: 关闭更新事件，就像把计数器的“闹钟”关掉
       - DISABLE: 开启更新事件，计数器满了就会响“闹钟”
    */
    NewState != DISABLE ? (TIMx->CR1 |= TIM_CR1_UDIS)    // 置 1：禁止更新事件
        : (TIMx->CR1 &= (uint16_t)~TIM_CR1_UDIS); // 置 0：允许更新事件
}


/**
  * @brief  配置 TIMx 更新请求（Update Event）中断源。
  * @param  TIMx: 要操作的定时器外设，范围 TIM1~TIM14。
  * @param  TIM_UpdateSource: 更新请求源，可以是：
  *          - TIM_UpdateSource_Global  : 更新源包括计数器溢出/下溢、UG 位设置或从模式控制器触发
  *          - TIM_UpdateSource_Regular : 更新源仅限计数器溢出/下溢
  * @retval None
  *
  * @note
  *  - TIM_CR1_URS 位用于控制哪些事件会触发更新中断：
  *      1 = 仅计数器溢出/下溢触发中断
  *      0 = 所有更新事件（包括 UG 位、从模式触发）都可触发中断
  *  - 小白理解：
  *      更新事件不一定进入中断，URS 控制“哪些事件会真正让 CPU 响应中断”
  *
  * @example
  *  TIM_UpdateRequestConfig(TIM2, TIM_UpdateSource_Regular); // 只在溢出/下溢触发中断
  */
void myTIM_UpdateRequestConfig(TIM_TypeDef* TIMx, uint16_t TIM_UpdateSource)
{
    /* 参数检查 */
    assert_param(IS_TIM_ALL_PERIPH(TIMx));
    assert_param(IS_TIM_UPDATE_SOURCE(TIM_UpdateSource));

    /* 三目运算符：设置或清除 URS 位 */
    TIM_UpdateSource != TIM_UpdateSource_Global ? (TIMx->CR1 |= TIM_CR1_URS)   // 只允许溢出/下溢触发
        : (TIMx->CR1 &= (uint16_t)~TIM_CR1_URS); // 全部更新事件触发
}

/**
  * @brief  使能或禁止 TIMx ARR（自动重载寄存器）预装载功能。
  * @param  TIMx: 要操作的定时器外设，范围 TIM1~TIM14。
  * @param  NewState: 新状态，可以是 ENABLE 或 DISABLE
  *         - ENABLE  : 开启 ARR 预装载功能（写入 ARR 后在下一个更新事件才生效）
  *         - DISABLE : 关闭 ARR 预装载功能（写入 ARR 立即生效）
  * @retval None
  *
  * @note
  *  - TIM_CR1_ARPE 位控制 ARR 预装载：
  *      1 = ARR 寄存器为预装载模式，写入 ARR 后计数器下一更新事件才生效
  *      0 = ARR 寄存器立即生效
  *  - 小白理解：
  *      1. 预装载可以保证当前计数周期完整执行，新值不会打断计数，周期稳定。
  *      2. 对 PWM 或其他输出模式特别重要，可以避免输出波形突变。
  *      3. 软件设计方便，可以安全地随时修改 ARR 值。
  *  - 一般情况下都开启 ARPE，即 NewState = ENABLE。
  *
  * @example
  *  TIM_ARRPreloadConfig(TIM2, ENABLE);  // 开启预装载，推荐默认开启
  */
void myTIM_ARRPreloadConfig(TIM_TypeDef* TIMx, FunctionalState NewState)
{
    /* 参数检查 */
    assert_param(IS_TIM_ALL_PERIPH(TIMx));
    assert_param(IS_FUNCTIONAL_STATE(NewState));

    /* 三目运算符：设置或清除 ARPE 位 */
    NewState != DISABLE ? (TIMx->CR1 |= TIM_CR1_ARPE)  // 开启预装载
        : (TIMx->CR1 &= (uint16_t)~TIM_CR1_ARPE); // 关闭预装载
}


/**
  * @brief  选择 TIMx 的单脉冲模式（One Pulse Mode）。
  * @param  TIMx: 要操作的定时器外设，范围 TIM1~TIM14。
  * @param  TIM_OPMode: 指定单脉冲模式，可选值：
  *          - TIM_OPMode_Single    : 单次脉冲模式，计数器到达 ARR 后停止
  *          - TIM_OPMode_Repetitive: 重复模式，计数器到达 ARR 后可继续触发
  * @retval None
  *
  * @note
  *  - TIM_CR1_OPM 位控制 TIM 的单脉冲模式：
  *      1 = 单次模式（Single Pulse）
  *      0 = 重复模式（Repetitive）
  *  - 单脉冲模式通常用于需要精确输出一次脉冲的场景，如触发信号或测距。
  *  - 设置 TIM_OPMode 后，计数器行为将根据选择模式改变。
  *
  * @example
  *  TIM_SelectOnePulseMode(TIM2, TIM_OPMode_Single); // TIM2 输出一次脉冲
  */
void myTIM_SelectOnePulseMode(TIM_TypeDef* TIMx, uint16_t TIM_OPMode)
{
    /* 检查参数有效性 */
    assert_param(IS_TIM_ALL_PERIPH(TIMx));
    assert_param(IS_TIM_OPM_MODE(TIM_OPMode));

    /* 先清除 OPM 位 */
    TIMx->CR1 &= (uint16_t)~TIM_CR1_OPM;

    /* 设置 OPM 模式 */
    TIMx->CR1 |= TIM_OPMode;
}

/**
  * @brief  设置 TIMx 的时钟分频值（Clock Division）。
  * @param  TIMx: 要操作的定时器外设，范围 TIM1~TIM5, TIM8~TIM14（不包括 TIM6 和 TIM7）。
  * @param  TIM_CKD: 指定时钟分频值，可选：
  *          - TIM_CKD_DIV1 : TDTS = Tck_tim，时钟不分频
  *          - TIM_CKD_DIV2 : TDTS = 2*Tck_tim，时钟频率减半
  *          - TIM_CKD_DIV4 : TDTS = 4*Tck_tim，时钟频率减为四分之一
  * @retval None
  *
  * @note
  *  - TIM_CR1_CKD 位控制定时器的采样时钟分频：
  *      - TDTS 是触发/捕获逻辑使用的定时器采样时钟
  *      - 分频会影响输入捕获、输出比较和 PWM 的精度
  *  - 定时器计数时钟 Tck_tim 计算方式：
  *      Tck_tim = Timer_Clock / (PSC + 1)
  *      其中：
  *        - Timer_Clock = TIM 所在 APB 总线时钟
  *        - 如果 APB 总线预分频器 > 1，则 Timer_Clock = APB × 2
  *        - 如果 APB 总线预分频器 = 1，则 Timer_Clock = APB
  *        - PSC = TIM 预分频器寄存器值
  *  - 小白理解：
  *      - TIM_CKD_DIV1：采样最快，输出和捕获最精确
  *      - TIM_CKD_DIV2/4：采样慢一些，但可减少中断频率或减小功耗
  *
  * @example
  *  TIM_SetClockDivision(TIM3, TIM_CKD_DIV2); // TIM3 采样时钟减半
  */
void myTIM_SetClockDivision(TIM_TypeDef* TIMx, uint16_t TIM_CKD)
{
    /* 检查参数有效性 */
    assert_param(IS_TIM_LIST1_PERIPH(TIMx));
    assert_param(IS_TIM_CKD_DIV(TIM_CKD));

    /* 先清除 CKD 位 */
    TIMx->CR1 &= (uint16_t)(~TIM_CR1_CKD);

    /* 设置 CKD 分频值 */
    TIMx->CR1 |= TIM_CKD;
}

/**
  * @brief  使能或禁止指定的 TIM 外设计数器。
  * @param  TIMx: 要操作的定时器外设，范围 TIM1~TIM14。
  * @param  NewState: TIMx 新状态，可以是 ENABLE 或 DISABLE
  *         - ENABLE  : 使能定时器计数器，开始计数
  *         - DISABLE : 禁止定时器计数器，停止计数
  * @retval None
  *
  * @note
  *  - TIM_CR1_CEN 位控制 TIM 计数器：
  *      1 = 计数器使能，开始计数
  *      0 = 计数器禁止，停止计数
  *  - 小白理解：
  *      - ENABLE 相当于启动定时器
  *      - DISABLE 相当于暂停定时器
  *  - 注意：定时器停止后，CNT 寄存器的值保持不变，重新使能后继续计数
  *
  * @example
  *  TIM_Cmd(TIM2, ENABLE);  // 启动 TIM2 计数器
  *  TIM_Cmd(TIM2, DISABLE); // 停止 TIM2 计数器
  */
void myTIM_Cmd(TIM_TypeDef* TIMx, FunctionalState NewState)
{
    /* 检查参数有效性 */
    assert_param(IS_TIM_ALL_PERIPH(TIMx));
    assert_param(IS_FUNCTIONAL_STATE(NewState));

    /* 使用三目运算符开启或关闭计数器 */
    NewState != DISABLE ? (TIMx->CR1 |= TIM_CR1_CEN)   // ENABLE: 启动计数器
        : (TIMx->CR1 &= (uint16_t)~TIM_CR1_CEN); // DISABLE: 停止计数器
}

/** @defgroup TIM_Group2 输出比较（Output Compare）管理函数
  * @brief    定时器输出比较管理函数
  *
@verbatim
 ===============================================================================
              ##### 输出比较管理函数 #####
 ===============================================================================

        ##### TIM 驱动：如何在输出比较模式下使用定时器 #####
 ===============================================================================
    [..]
    使用定时器的输出比较模式，需要按照以下步骤操作：

      (#) 使能 TIM 时钟，调用函数：
          RCC_APBxPeriphClockCmd(RCC_APBxPeriph_TIMx, ENABLE)

      (#) 配置 TIM 对应的引脚，即配置相关 GPIO 引脚

      (#) 配置定时器的时间基准（Time Base）单元，如需要，否则定时器会使用默认配置：
            Autoreload 寄存器（ARR） = 0xFFFF
            预分频器（PSC） = 0x0000
            计数器模式 = 向上计数（Up counting）
            时钟分频 = TIM_CKD_DIV1

      (#) 填写 TIM_OCInitStruct 结构体的配置参数，包括：
            (++) 输出比较模式：TIM_OCMode
            (++) 通道输出使能状态：TIM_OutputState
            (++) 捕获比较寄存器 CCR 值：TIM_Pulse
            (++) 输出比较极性：TIM_OCPolarity

      (#) 调用 TIM_OCxInit(TIMx, &TIM_OCInitStruct) 来配置指定通道

      (#) 调用 TIM_Cmd(ENABLE) 使能 TIM 计数器

      -@- 其他函数可以单独使用，用于修改定时器的特定功能

      -@- 如果使用 PWM 模式，必须调用：
          TIM_OCxPreloadConfig(TIMx, TIM_OCPreload_ENABLE)

      -@- 如果需要中断或 DMA 请求，用户需要：
        (+@) 使能 NVIC（或 DMA）以使用 TIM 中断（或 DMA 请求）
        (+@) 使用函数 TIM_ITConfig(TIMx, TIM_IT_CCx)
             （或 TIM_DMA_Cmd(TIMx, TIM_DMA_CCx）使能对应中断/DMA
@endverbatim
*/


/**
  * @brief  初始化 TIMx 指定通道的输出比较（Output Compare, OC）功能。
  * @param  TIMx: 要操作的定时器外设，范围 TIM1~TIM5, TIM8（不包括 TIM6 和 TIM7）。
  * @param  channel: 要配置的通道，取值 TIM_Channel_1 ~ TIM_Channel_4。
  * @param  TIM_OCInitStruct: 指向 TIM_OCInitTypeDef 结构体的指针，包含输出比较配置参数：
  *         - TIM_OCMode      : 输出比较模式，如 PWM、翻转输出等
  *         - TIM_OutputState : 通道输出是否使能（开启/关闭）
  *         - TIM_Pulse       : 捕获比较寄存器 CCR 的值，即“触发点”
  *         - TIM_OCPolarity  : 输出极性，高电平有效或低电平有效
  *         - 高级定时器 TIM1/TIM8 可设置互补输出、空闲状态等
  * @retval None
  *
  * @note
  *  - 输出比较功能：定时器在计数过程中，当 CNT 寄存器的值等于 CCR 寄存器值时
  *    根据 TIM_OCMode 做出相应操作（如翻转输出、产生 PWM 波形）。
  *  - 高级定时器（TIM1/TIM8）支持：
  *      - 互补输出（N 通道）：在输出端产生反相信号，常用于 H 桥或电机驱动
  *      - 空闲状态：计数器未启动时输出的初始电平
  *  - 小白理解：
  *      1. CCR = 100，计数器 CNT 每次计数到 100 时，就会触发一次输出动作
  *      2. TIM_OCMode 决定输出动作类型，例如 PWM 输出、翻转输出
  *      3. OutputState 决定通道是否有效，如果关闭，计数器到 CCR 也不会输出
  *      4. 高级定时器可以额外控制互补输出和空闲状态，用于复杂驱动
  *
  * @example
  *  TIM_OCInitTypeDef TIM_OCInitStructure;
  *  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
  *  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
  *  TIM_OCInitStructure.TIM_Pulse = 500; // CCR = 500
  *  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
  *  TIM_OCxInit(TIM3, TIM_Channel_1, &TIM_OCInitStructure); // 初始化 TIM3 通道 1 输出比较
  */

void myTIM_OCxInit(TIM_TypeDef* TIMx, uint8_t channel, TIM_OCInitTypeDef* TIM_OCInitStruct)
{


    /* 定义通道寄存器映射 */
    typedef struct {
        volatile uint16_t* CCMR;  // 输出比较模式寄存器
        volatile uint16_t* CCER;  // 输出使能寄存器
        volatile uint16_t* CR2;   // 高级定时器 CR2 寄存器
        volatile uint16_t* CCR;   // 捕获比较寄存器
        uint16_t OCEN_BIT;        // 通道输出使能位
        uint16_t CCPOL_BIT;       // 通道极性位
        uint16_t OCN_BIT;         // 互补输出使能
        uint16_t OCNPOL_BIT;      // 互补输出极性
        uint16_t OIS_BIT;         // 输出空闲状态
        uint16_t OISN_BIT;        // 互补空闲状态
        uint8_t hasComplement;    // 是否有互补通道
    } TIM_ChannelMap;

    TIM_ChannelMap chMap;
    /* 检查参数有效性 */
    assert_param(IS_TIM_LIST1_PERIPH(TIMx));
    assert_param(IS_TIM_OC_MODE(TIM_OCInitStruct->TIM_OCMode));
    assert_param(IS_TIM_OUTPUT_STATE(TIM_OCInitStruct->TIM_OutputState));
    assert_param(IS_TIM_OC_POLARITY(TIM_OCInitStruct->TIM_OCPolarity));
    assert_param(channel >= 1 && channel <= 4);
    /* 根据通道号初始化映射 */
    switch (channel)
    {
    case 1:
        chMap.CCMR = &TIMx->CCMR1;
        chMap.CCER = &TIMx->CCER;
        chMap.CR2 = &TIMx->CR2;
        chMap.CCR = &TIMx->CCR1;
        chMap.OCEN_BIT = TIM_CCER_CC1E;
        chMap.CCPOL_BIT = TIM_CCER_CC1P;
        chMap.OCN_BIT = TIM_CCER_CC1NE;
        chMap.OCNPOL_BIT = TIM_CCER_CC1NP;
        chMap.OIS_BIT = TIM_CR2_OIS1;
        chMap.OISN_BIT = TIM_CR2_OIS1N;
        break;
    case 2:
        chMap.CCMR = &TIMx->CCMR1;
        chMap.CCER = &TIMx->CCER;
        chMap.CR2 = &TIMx->CR2;
        chMap.CCR = &TIMx->CCR2;
        chMap.OCEN_BIT = TIM_CCER_CC2E;
        chMap.CCPOL_BIT = TIM_CCER_CC2P;
        chMap.OCN_BIT = TIM_CCER_CC2NE;
        chMap.OCNPOL_BIT = TIM_CCER_CC2NP;
        chMap.OIS_BIT = TIM_CR2_OIS2;
        chMap.OISN_BIT = TIM_CR2_OIS2N;
        break;
    case 3:
        chMap.CCMR = &TIMx->CCMR2;
        chMap.CCER = &TIMx->CCER;
        chMap.CR2 = &TIMx->CR2;
        chMap.CCR = &TIMx->CCR3;
        chMap.OCEN_BIT = TIM_CCER_CC3E;
        chMap.CCPOL_BIT = TIM_CCER_CC3P;
        chMap.OCN_BIT = TIM_CCER_CC3NE;
        chMap.OCNPOL_BIT = TIM_CCER_CC3NP;
        chMap.OIS_BIT = TIM_CR2_OIS3;
        chMap.OISN_BIT = TIM_CR2_OIS3N;
        break;
    case 4:
        chMap.CCMR = &TIMx->CCMR2;
        chMap.CCER = &TIMx->CCER;
        chMap.CR2 = &TIMx->CR2;
        chMap.CCR = &TIMx->CCR4;
        chMap.OCEN_BIT = TIM_CCER_CC4E;
        chMap.CCPOL_BIT = TIM_CCER_CC4P;
        chMap.OCN_BIT = TIM_CCER_CC4NE;
        chMap.OCNPOL_BIT = TIM_CCER_CC4NP;
        chMap.OIS_BIT = TIM_CR2_OIS4;
        chMap.OISN_BIT = TIM_CR2_OIS4N;
        break;
    }

    chMap.hasComplement = (TIMx == TIM1 || TIMx == TIM8);

    /* 禁用通道输出 */
    *chMap.CCER &= ~(chMap.OCEN_BIT);

    /* 配置输出比较模式（清除原模式再设置） */
    if (channel == 1 || channel == 2)
        *chMap.CCMR &= ~(TIM_CCMR1_OC1M | TIM_CCMR1_CC1S);
    else
        *chMap.CCMR &= ~(TIM_CCMR2_OC3M | TIM_CCMR2_CC3S);
    *chMap.CCMR |= TIM_OCInitStruct->TIM_OCMode;

    /* 配置输出极性和使能 */
    *chMap.CCER &= ~(chMap.CCPOL_BIT);
    *chMap.CCER |= TIM_OCInitStruct->TIM_OCPolarity;
    *chMap.CCER |= TIM_OCInitStruct->TIM_OutputState;

    /* 高级定时器互补输出 */
    if (chMap.hasComplement)
    {
        *chMap.CCER &= ~(chMap.OCN_BIT | chMap.OCNPOL_BIT);
        *chMap.CCER |= TIM_OCInitStruct->TIM_OutputNState | TIM_OCInitStruct->TIM_OCNPolarity;

        *chMap.CR2 &= ~(chMap.OIS_BIT | chMap.OISN_BIT);
        *chMap.CR2 |= TIM_OCInitStruct->TIM_OCIdleState | TIM_OCInitStruct->TIM_OCNIdleState;
    }

    /* 设置比较值 CCR */
    *chMap.CCR = TIM_OCInitStruct->TIM_Pulse;
}



/**
  * @brief  初始化 TIMx 的通道2输出比较（Output Compare）功能。
  * @param  TIMx: 要操作的定时器外设，范围 TIM1, TIM2, TIM3, TIM4, TIM5, TIM8, TIM9, TIM12
  * @param  TIM_OCInitStruct: 指向 TIM_OCInitTypeDef 结构体的指针，包含输出比较配置参数：
  *         - TIM_OCMode      : 输出比较模式（如 PWM1、PWM2、TOGGLE 等）
  *         - TIM_OutputState : 通道输出使能状态（ENABLE 或 DISABLE）
  *         - TIM_Pulse       : CCR 寄存器值，决定输出触发点
  *         - TIM_OCPolarity  : 输出极性（高电平有效或低电平有效）
  *         - TIM_OutputNState、TIM_OCNPolarity 等：仅高级定时器 TIM1/TIM8 支持互补输出和空闲状态
  * @retval None
  *
  * @note
  *  - CCR = TIM_Pulse，当计数器 CNT == CCR 时触发输出动作
  *  - TIMx 通道2对应寄存器：
  *      - CCMR1 高 8 位：OC2M（输出模式）和 CC2S（通道选择）
  *      - CCER：输出使能、极性、互补输出配置
  *      - CCR2：捕获比较寄存器
  *      - CR2：高级定时器空闲状态等
  *  - 小白理解：初始化告诉定时器“当计数到 CCR2 时，输出做什么”
  */
void myTIM_OC2Init(TIM_TypeDef* TIMx, TIM_OCInitTypeDef* TIM_OCInitStruct)
{


    /* 临时寄存器缓存 */
    uint16_t tmpCCMR = TIMx->CCMR1;
    uint16_t tmpCCER = TIMx->CCER;
    uint16_t tmpCR2 = TIMx->CR2;


    /* 检查参数有效性 */
    assert_param(IS_TIM_LIST2_PERIPH(TIMx));
    assert_param(IS_TIM_OC_MODE(TIM_OCInitStruct->TIM_OCMode));
    assert_param(IS_TIM_OUTPUT_STATE(TIM_OCInitStruct->TIM_OutputState));
    assert_param(IS_TIM_OC_POLARITY(TIM_OCInitStruct->TIM_OCPolarity));
    /* 禁用通道2输出，防止配置过程中触发 */
    tmpCCER &= ~TIM_CCER_CC2E;

    /* ----- 配置通道2输出模式和选择 ----- */
    tmpCCMR &= ~(TIM_CCMR1_OC2M | TIM_CCMR1_CC2S);                 // 清除 OC2M/CC2S
    tmpCCMR |= (TIM_OCInitStruct->TIM_OCMode << 8);                // 设置输出模式

    /* ----- 配置通道2输出极性和使能 ----- */
    tmpCCER &= ~TIM_CCER_CC2P;                                     // 清除极性
    tmpCCER |= ((TIM_OCInitStruct->TIM_OCPolarity | TIM_OCInitStruct->TIM_OutputState) << 4);

    /* 高级定时器 TIM1/TIM8 支持互补输出和空闲状态 */
    if ((TIMx == TIM1) || (TIMx == TIM8))
    {
        assert_param(IS_TIM_OUTPUTN_STATE(TIM_OCInitStruct->TIM_OutputNState));
        assert_param(IS_TIM_OCN_POLARITY(TIM_OCInitStruct->TIM_OCNPolarity));
        assert_param(IS_TIM_OCNIDLE_STATE(TIM_OCInitStruct->TIM_OCNIdleState));
        assert_param(IS_TIM_OCIDLE_STATE(TIM_OCInitStruct->TIM_OCIdleState));

        /* 设置互补输出及空闲状态 */
        tmpCCER &= ~(TIM_CCER_CC2NP | TIM_CCER_CC2NE);
        tmpCCER |= ((TIM_OCInitStruct->TIM_OCNPolarity | TIM_OCInitStruct->TIM_OutputNState) << 4);

        tmpCR2 &= ~(TIM_CR2_OIS2 | TIM_CR2_OIS2N);
        tmpCR2 |= ((TIM_OCInitStruct->TIM_OCIdleState | TIM_OCInitStruct->TIM_OCNIdleState) << 2);
    }

    /* 写回寄存器 */
    TIMx->CCMR1 = tmpCCMR;   // 设置输出模式
    TIMx->CCER = tmpCCER;   // 设置输出极性/使能
    TIMx->CCR2 = TIM_OCInitStruct->TIM_Pulse;  // 设置捕获比较值
    TIMx->CR2 = tmpCR2;    // 设置空闲状态等
}


/**
  * @brief  初始化 TIMx 的通道 3 输出比较功能（Output Compare）。
  * @param  TIMx: 要操作的定时器外设，范围 TIM1~TIM5, TIM8（不包括 TIM6 和 TIM7）。
  * @param  TIM_OCInitStruct: 指向 TIM_OCInitTypeDef 结构体的指针，包含通道 3 输出比较的配置参数：
  *         - TIM_OCMode: 输出比较模式，如 PWM 模式、TOGGLE 模式等
  *         - TIM_OutputState: 通道输出使能状态，ENABLE 或 DISABLE
  *         - TIM_Pulse: CCR 寄存器值，决定定时器计数到多少时触发输出
  *         - TIM_OCPolarity: 输出极性，高电平或低电平有效
  *         - TIM_OutputNState / TIM_OCNPolarity / TIM_OCNIdleState: 高级定时器互补输出相关配置
  *         - TIM_OCIdleState: 定时器空闲状态配置
  * @retval None
  *
  * @note
  *  - 该函数会配置 CCMR2、CCER、CCR3、CR2 等寄存器
  *  - CNT 计数到 CCR3 值时，根据 TIM_OCMode 和极性产生对应输出
  *  - 对于高级定时器 TIM1/TIM8，支持互补输出、空闲状态配置
  *  - 小白理解：
  *      1. 先关闭通道 3 输出，防止配置过程中意外触发
  *      2. 设置输出模式和通道选择
  *      3. 设置输出极性和使能状态
  *      4. 高级定时器可设置互补输出和空闲状态
  *      5. 配置 CCR3 值决定触发时刻
  *      6. 写回寄存器，完成初始化
  */
void myTIM_OC3Init(TIM_TypeDef* TIMx, TIM_OCInitTypeDef* TIM_OCInitStruct)
{
    uint16_t tmpccmrx, tmpccer, tmpcr2;

    // 1. 参数检查
    assert_param(IS_TIM_LIST3_PERIPH(TIMx));
    assert_param(IS_TIM_OC_MODE(TIM_OCInitStruct->TIM_OCMode));
    assert_param(IS_TIM_OUTPUT_STATE(TIM_OCInitStruct->TIM_OutputState));
    assert_param(IS_TIM_OC_POLARITY(TIM_OCInitStruct->TIM_OCPolarity));

    // 2. 禁止通道 3 输出
    TIMx->CCER &= (uint16_t)~TIM_CCER_CC3E;

    // 3. 读取寄存器原值
    tmpccer = TIMx->CCER;
    tmpcr2 = TIMx->CR2;
    tmpccmrx = TIMx->CCMR2;

    // 4. 配置输出比较模式
    tmpccmrx &= (uint16_t)~TIM_CCMR2_OC3M;
    tmpccmrx &= (uint16_t)~TIM_CCMR2_CC3S;
    tmpccmrx |= TIM_OCInitStruct->TIM_OCMode;

    // 5. 配置极性和输出使能
    tmpccer &= (uint16_t)~TIM_CCER_CC3P;
    tmpccer |= (uint16_t)(TIM_OCInitStruct->TIM_OCPolarity << 8);
    tmpccer |= (uint16_t)(TIM_OCInitStruct->TIM_OutputState << 8);

    // 6. 高级定时器 TIM1/TIM8 额外配置互补输出和空闲状态
    if ((TIMx == TIM1) || (TIMx == TIM8))
    {
        assert_param(IS_TIM_OUTPUTN_STATE(TIM_OCInitStruct->TIM_OutputNState));
        assert_param(IS_TIM_OCN_POLARITY(TIM_OCInitStruct->TIM_OCNPolarity));
        assert_param(IS_TIM_OCNIDLE_STATE(TIM_OCInitStruct->TIM_OCNIdleState));
        assert_param(IS_TIM_OCIDLE_STATE(TIM_OCInitStruct->TIM_OCIdleState));

        tmpccer &= (uint16_t)~TIM_CCER_CC3NP;
        tmpccer |= (uint16_t)(TIM_OCInitStruct->TIM_OCNPolarity << 8);

        tmpccer &= (uint16_t)~TIM_CCER_CC3NE;
        tmpccer |= (uint16_t)(TIM_OCInitStruct->TIM_OutputNState << 8);

        tmpcr2 &= (uint16_t)~TIM_CR2_OIS3;
        tmpcr2 &= (uint16_t)~TIM_CR2_OIS3N;
        tmpcr2 |= (uint16_t)(TIM_OCInitStruct->TIM_OCIdleState << 4);
        tmpcr2 |= (uint16_t)(TIM_OCInitStruct->TIM_OCNIdleState << 4);
    }

    // 7. 写回寄存器
    TIMx->CR2 = tmpcr2;
    TIMx->CCMR2 = tmpccmrx;
    TIMx->CCR3 = TIM_OCInitStruct->TIM_Pulse;
    TIMx->CCER = tmpccer;
}

/**
  * @brief  初始化 TIMx 的通道 4 输出比较功能（Output Compare）。
  * @param  TIMx: 要操作的定时器外设，范围 TIM1~TIM5, TIM8（不包括 TIM6 和 TIM7）。
  * @param  TIM_OCInitStruct: 指向 TIM_OCInitTypeDef 结构体的指针，包含通道 4 输出比较的配置参数：
  *         - TIM_OCMode: 输出比较模式，如 PWM 模式、TOGGLE 模式等
  *         - TIM_OutputState: 通道输出使能状态，ENABLE 或 DISABLE
  *         - TIM_Pulse: CCR 寄存器值，决定定时器计数到多少时触发输出
  *         - TIM_OCPolarity: 输出极性，高电平或低电平有效
  *         - TIM_OCIdleState: 定时器空闲状态配置（高级定时器 TIM1/TIM8 可用）
  * @retval None
  *
  * @note
  *  - 该函数会配置 CCMR2、CCER、CCR4、CR2 等寄存器
  *  - CNT 计数到 CCR4 值时，根据 TIM_OCMode 和极性产生对应输出
  *  - 对于高级定时器 TIM1/TIM8，支持空闲状态配置
  *  - 小白理解：
  *      1. 先关闭通道 4 输出，防止配置过程中意外触发
  *      2. 设置输出模式和通道选择
  *      3. 设置输出极性和使能状态
  *      4. 高级定时器可设置空闲状态
  *      5. 配置 CCR4 值决定触发时刻
  *      6. 写回寄存器，完成初始化
  */
void myTIM_OC4Init(TIM_TypeDef* TIMx, TIM_OCInitTypeDef* TIM_OCInitStruct)
{
    uint16_t tmpccmrx, tmpccer, tmpcr2;

    // 1. 参数检查
    assert_param(IS_TIM_LIST3_PERIPH(TIMx));
    assert_param(IS_TIM_OC_MODE(TIM_OCInitStruct->TIM_OCMode));
    assert_param(IS_TIM_OUTPUT_STATE(TIM_OCInitStruct->TIM_OutputState));
    assert_param(IS_TIM_OC_POLARITY(TIM_OCInitStruct->TIM_OCPolarity));

    // 2. 禁止通道 4 输出
    TIMx->CCER &= (uint16_t)~TIM_CCER_CC4E;

    // 3. 读取寄存器原值
    tmpccer = TIMx->CCER;
    tmpcr2 = TIMx->CR2;
    tmpccmrx = TIMx->CCMR2;

    // 4. 配置输出比较模式
    tmpccmrx &= (uint16_t)~TIM_CCMR2_OC4M;
    tmpccmrx &= (uint16_t)~TIM_CCMR2_CC4S;
    tmpccmrx |= (uint16_t)(TIM_OCInitStruct->TIM_OCMode << 8);

    // 5. 配置极性和输出使能
    tmpccer &= (uint16_t)~TIM_CCER_CC4P;
    tmpccer |= (uint16_t)(TIM_OCInitStruct->TIM_OCPolarity << 12);
    tmpccer |= (uint16_t)(TIM_OCInitStruct->TIM_OutputState << 12);

    // 6. 高级定时器 TIM1/TIM8 配置空闲状态
    if ((TIMx == TIM1) || (TIMx == TIM8))
    {
        assert_param(IS_TIM_OCIDLE_STATE(TIM_OCInitStruct->TIM_OCIdleState));
        tmpcr2 &= (uint16_t)~TIM_CR2_OIS4;
        tmpcr2 |= (uint16_t)(TIM_OCInitStruct->TIM_OCIdleState << 6);
    }

    // 7. 写回寄存器
    TIMx->CR2 = tmpcr2;
    TIMx->CCMR2 = tmpccmrx;
    TIMx->CCR4 = TIM_OCInitStruct->TIM_Pulse;
    TIMx->CCER = tmpccer;
}


/**
  * @brief  将 TIM_OCInitTypeDef 结构体的各成员填充为默认值。
  * @param  TIM_OCInitStruct: 指向一个 TIM_OCInitTypeDef 结构体的指针，
  *         函数会将其各成员初始化为默认配置。
  * @retval None
  *
  * @note
  *  - 这个函数主要用于初始化结构体，避免未初始化带来的错误。
  *  - 小白理解：调用此函数后，你得到的结构体就有一套安全的默认配置，
  *    然后你只需要修改需要的字段即可，比如修改 CCR 值或者输出模式。
  *
  * @example
  *  TIM_OCInitTypeDef OC_Config;
  *  TIM_OCStructInit(&OC_Config);      // 填充默认值
  *  OC_Config.TIM_OCMode = TIM_OCMode_PWM1;  // 修改为 PWM 模式
  *  OC_Config.TIM_Pulse = 1000;             // 设置 CCR 值
  */
void myTIM_OCStructInit(TIM_OCInitTypeDef* TIM_OCInitStruct)
{
    /* 设置输出比较模式为定时模式（不输出） */
    TIM_OCInitStruct->TIM_OCMode = TIM_OCMode_Timing;

    /* 默认通道输出禁止 */
    TIM_OCInitStruct->TIM_OutputState = TIM_OutputState_Disable;

    /* 默认高级定时器互补输出禁止 */
    TIM_OCInitStruct->TIM_OutputNState = TIM_OutputNState_Disable;

    /* 默认 CCR 值为 0，计数到 0 会触发输出 */
    TIM_OCInitStruct->TIM_Pulse = 0x00000000;

    /* 默认输出极性为高电平有效 */
    TIM_OCInitStruct->TIM_OCPolarity = TIM_OCPolarity_High;

    /* 默认互补输出极性为高电平有效 */
    TIM_OCInitStruct->TIM_OCNPolarity = TIM_OCPolarity_High;

    /* 默认空闲状态为复位（低电平） */
    TIM_OCInitStruct->TIM_OCIdleState = TIM_OCIdleState_Reset;

    /* 默认互补输出空闲状态为复位（低电平） */
    TIM_OCInitStruct->TIM_OCNIdleState = TIM_OCNIdleState_Reset;
}


/**
  * @brief  设置指定 TIM 通道的输出比较模式（Output Compare Mode）。
  * @note   该函数在修改输出比较模式前，会先禁用对应通道。
  *         如果需要使用该通道，需要用户在外部调用 TIM_CCxCmd() 或 TIM_CCxNCmd() 使能通道。
  * @param  TIMx: 选择的定时器外设，范围 TIM1~TIM5、TIM8~TIM14（不包括 TIM6 和 TIM7）。
  * @param  TIM_Channel: 要配置的通道，可选：
  *            - TIM_Channel_1
  *            - TIM_Channel_2
  *            - TIM_Channel_3
  *            - TIM_Channel_4
  * @param  TIM_OCMode: 输出比较模式，可选：
  *            - TIM_OCMode_Timing    : 定时模式，不输出
  *            - TIM_OCMode_Active    : CNT = CCR 时输出高电平
  *            - TIM_OCMode_Toggle    : CNT = CCR 时翻转输出
  *            - TIM_OCMode_PWM1      : PWM1 模式
  *            - TIM_OCMode_PWM2      : PWM2 模式
  *            - TIM_ForcedAction_Active   : 强制输出高
  *            - TIM_ForcedAction_InActive : 强制输出低
  * @retval None
  *
  * @note 小白理解：
  *  1. 每个通道在修改模式前要先关闭，防止在配置过程中意外触发。
  *  2. CCMR 寄存器中不同通道对应不同位域，通道 1/3 和通道 2/4 的位偏移不同。
  *  3. 利用三目运算符可以统一计算 CCMR 地址和位偏移。
  */
void myTIM_SelectOCxM(TIM_TypeDef* TIMx, uint16_t TIM_Channel, uint16_t TIM_OCMode)
{
    uint32_t tmp = (uint32_t)TIMx + CCMR_OFFSET;  // CCMR 基地址
    uint16_t tmp1 = CCER_CCE_SET << TIM_Channel;  // CCxE 位掩码

    /* 参数检查 */
    assert_param(IS_TIM_LIST1_PERIPH(TIMx));
    assert_param(IS_TIM_CHANNEL(TIM_Channel));
    assert_param(IS_TIM_OCM(TIM_OCMode));

    /* 禁用对应通道：清零 CCxE 位 */
    TIMx->CCER &= (uint16_t)~tmp1;

    /* 计算 CCMR 寄存器地址并设置输出比较模式 */
    tmp += ((TIM_Channel == TIM_Channel_1 || TIM_Channel == TIM_Channel_3) ?
        (TIM_Channel >> 1) :
        ((TIM_Channel - 4) >> 1));

    /* 根据通道选择对应掩码，清 OCxM 位，然后设置模式 */
    *(__IO uint32_t*)tmp &= ((TIM_Channel == TIM_Channel_1 || TIM_Channel == TIM_Channel_3) ?
        CCMR_OC13M_MASK : CCMR_OC24M_MASK);

    *(__IO uint32_t*)tmp |= ((TIM_Channel == TIM_Channel_1 || TIM_Channel == TIM_Channel_3) ?
        TIM_OCMode : (TIM_OCMode << 8));
}


/**
  * @brief  设置 TIMx 指定通道的捕获比较寄存器 CCR 值
  * @param  TIMx: 要操作的定时器外设
  * @param  channel: 通道号，取值 1~4
  * @param  Compare: CCR 新值
  * @retval None
  *
  * @note
  *  - 根据 channel 自动写入 CCR1~CCR4
  *  - 使用前请确保 TIMx 已初始化
  */
void myTIM_SetCompare(TIM_TypeDef* TIMx, uint8_t channel, uint32_t Compare)
{
    /* 参数检查 */
    assert_param(channel >= 1 && channel <= 4);

    switch (channel)
    {
    case 1: assert_param(IS_TIM_LIST1_PERIPH(TIMx)); TIMx->CCR1 = Compare; break;
    case 2: assert_param(IS_TIM_LIST2_PERIPH(TIMx)); TIMx->CCR2 = Compare; break;
    case 3: assert_param(IS_TIM_LIST3_PERIPH(TIMx)); TIMx->CCR3 = Compare; break;
    case 4: assert_param(IS_TIM_LIST3_PERIPH(TIMx)); TIMx->CCR4 = Compare; break;
    }
}

/**
  * @brief  强制 TIMx 指定通道输出为指定电平（强制模式）
  * @param  TIMx: 要操作的定时器外设
  * @param  channel: 通道号，取值 1~4
  * @param  TIM_ForcedAction: 强制输出动作
  *          - TIM_ForcedAction_Active: 输出高电平
  *          - TIM_ForcedAction_InActive: 输出低电平
  * @retval None
  *
  * @note
  *  - 根据 channel 自动配置 CCMR1/CCMR2 寄存器
  *  - 通道1、2使用 CCMR1，通道3、4使用 CCMR2
  *  - 对于通道2、4，需左移 8 或 12 位
  */
void myTIM_ForcedOCConfig(TIM_TypeDef* TIMx, uint8_t channel, uint16_t TIM_ForcedAction)
{
    uint16_t tmp;

    /* 参数检查 */
    assert_param(channel >= 1 && channel <= 4);
    assert_param(IS_TIM_FORCED_ACTION(TIM_ForcedAction));

    if (channel <= 2)  /* 通道1和2使用 CCMR1 */
    {
        tmp = TIMx->CCMR1;

        /* 清除 OCxM 位 */
        tmp &= (channel == 1) ? ~TIM_CCMR1_OC1M : ~TIM_CCMR1_OC2M;

        /* 设置强制输出模式 */
        tmp |= (channel == 1) ? TIM_ForcedAction : (TIM_ForcedAction << 8);

        TIMx->CCMR1 = tmp;
    }
    else  /* 通道3和4使用 CCMR2 */
    {
        tmp = TIMx->CCMR2;

        /* 清除 OCxM 位 */
        tmp &= (channel == 3) ? ~TIM_CCMR2_OC3M : ~TIM_CCMR2_OC4M;

        /* 设置强制输出模式 */
        tmp |= (channel == 3) ? TIM_ForcedAction : (TIM_ForcedAction << 8);

        TIMx->CCMR2 = tmp;
    }
}

/**
  * @brief  配置 TIMx 指定通道的输出比较（Output Compare）预装载寄存器（Preload）使能或失能
  * @param  TIMx: 要操作的定时器外设，范围 TIM1~TIM5, TIM8（不包括 TIM6 和 TIM7）
  * @param  Channel: 指定要配置的通道
  *          取值范围：
  *            @arg 1: 通道 1
  *            @arg 2: 通道 2
  *            @arg 3: 通道 3
  *            @arg 4: 通道 4
  * @param  TIM_OCPreload: 预装载寄存器状态
  *          取值范围：
  *            @arg TIM_OCPreload_Enable: 使能预装载
  *            @arg TIM_OCPreload_Disable: 禁用预装载
  * @retval None
  *
  * @note
  *  - 该函数会自动选择 CCMR1/CCMR2 寄存器及对应位偏移
  *  - 通过参数 Channel 和 TIM_OCPreload，可一次函数配置任意通道
  *  - 优化了原来的四个函数，减少重复代码，便于维护
  *
  * @example
  *  // 使能 TIM3 通道 3 预装载
  *  myTIM_OCPreloadConfig(TIM3, 3, TIM_OCPreload_Enable);
  */
void myTIM_OCPreloadConfig(TIM_TypeDef* TIMx, uint8_t Channel, uint16_t TIM_OCPreload)
{
    uint16_t tmp = 0;
    volatile uint16_t* CCMR = NULL;
    uint8_t shift = 0;

    // 参数检查
    assert_param(IS_TIM_LIST3_PERIPH(TIMx));           // TIM1~TIM5, TIM8 可配置
    assert_param(Channel >= 1 && Channel <= 4);
    assert_param(IS_TIM_OCPRELOAD_STATE(TIM_OCPreload));

    // 根据通道选择 CCMR 寄存器及位偏移
    if (Channel <= 2)
    {
        CCMR = &TIMx->CCMR1; // 通道 1 和 2 在 CCMR1
        shift = (Channel == 1) ? 0 : 8;
    }
    else
    {
        CCMR = &TIMx->CCMR2; // 通道 3 和 4 在 CCMR2
        shift = (Channel == 3) ? 0 : 8;
    }

    tmp = *CCMR;

    // 清除对应通道的 OCxPE 位
    tmp &= (uint16_t)~(0x1 << shift);

    // 设置新的预装载状态
    tmp |= (TIM_OCPreload << shift);

    // 写回寄存器
    *CCMR = tmp;
}


/**
  * @brief  配置 TIMx 指定通道的输出比较（Output Compare）快速使能（Fast）功能
  * @param  TIMx: 要操作的定时器外设，范围 TIM1~TIM5, TIM8（不包括 TIM6 和 TIM7）
  * @param  Channel: 指定要配置的通道
  *          取值范围：
  *            @arg 1: 通道 1
  *            @arg 2: 通道 2
  *            @arg 3: 通道 3
  *            @arg 4: 通道 4
  * @param  TIM_OCFast: 输出比较快速使能状态
  *          取值范围：
  *            @arg TIM_OCFast_Enable: 使能快速功能
  *            @arg TIM_OCFast_Disable: 禁用快速功能
  * @retval None
  *
  * @note
  *  - 该函数会自动选择 CCMR1/CCMR2 寄存器及对应位偏移
  *  - 通过参数 Channel 和 TIM_OCFast，可一次函数配置任意通道
  *  - 优化了原来的四个函数，减少重复代码，便于维护
  * @note   快速使能仅影响输出信号的响应速度，不改变通道的功能或模式。
  *         开启后，当计数器 CNT 与 CCR 寄存器匹配时，输出立即更新；
  * @example
  *  // 使能 TIM3 通道 2 快速功能
  *  myTIM_OCFastConfig(TIM3, 2, TIM_OCFast_Enable);
  */
void myTIM_OCFastConfig(TIM_TypeDef* TIMx, uint8_t Channel, uint16_t TIM_OCFast)
{
    uint16_t tmp = 0;
    volatile uint16_t* CCMR = NULL;
    uint8_t shift = 0;

    // 参数检查
    assert_param(IS_TIM_LIST3_PERIPH(TIMx));           // TIM1~TIM5, TIM8 可配置
    assert_param(Channel >= 1 && Channel <= 4);
    assert_param(IS_TIM_OCFAST_STATE(TIM_OCFast));

    // 根据通道选择 CCMR 寄存器及位偏移
    if (Channel <= 2)
    {
        CCMR = &TIMx->CCMR1; // 通道 1 和 2 在 CCMR1
        shift = (Channel == 1) ? 0 : 8;
    }
    else
    {
        CCMR = &TIMx->CCMR2; // 通道 3 和 4 在 CCMR2
        shift = (Channel == 3) ? 0 : 8;
    }

    tmp = *CCMR;

    // 清除对应通道的 OCxFE 位
    tmp &= (uint16_t)~(0x1 << shift);

    // 设置新的快速功能状态
    tmp |= (TIM_OCFast << shift);

    // 写回寄存器
    *CCMR = tmp;
}



/**
  * @brief  配置 TIMx 指定通道的输出比较（Output Compare）清零（Clear）功能
  * @param  TIMx: 要操作的定时器外设，范围 TIM1~TIM5, TIM8（不包括 TIM6 和 TIM7）
  * @param  Channel: 指定要配置的通道
  *          取值范围：
  *            @arg 1: 通道 1
  *            @arg 2: 通道 2
  *            @arg 3: 通道 3
  *            @arg 4: 通道 4
  * @param  TIM_OCClear: 输出比较清零状态
  *          取值范围：
  *            @arg TIM_OCClear_Enable: 使能清零
  *            @arg TIM_OCClear_Disable: 禁用清零
  * @retval None
  *
  * @note
  *  - 该函数会根据通道自动选择 CCMR1/CCMR2 寄存器及对应位偏移
  *  - 优化了原来的四个函数，减少重复代码，便于维护
  *
  * @example
  *  // 使能 TIM3 通道 2 清零功能
  *  myTIM_OCClearConfig(TIM3, 2, TIM_OCClear_Enable);
  */
void myTIM_OCClearConfig(TIM_TypeDef* TIMx, uint8_t Channel, uint16_t TIM_OCClear)
{
    uint16_t tmp = 0;
    volatile uint16_t* CCMR = NULL;
    uint8_t shift = 0;

    // 参数检查
    assert_param(IS_TIM_LIST3_PERIPH(TIMx));       // TIM1~TIM5, TIM8 可配置
    assert_param(Channel >= 1 && Channel <= 4);
    assert_param(IS_TIM_OCCLEAR_STATE(TIM_OCClear));

    // 根据通道选择 CCMR 寄存器及位偏移
    if (Channel <= 2)
    {
        CCMR = &TIMx->CCMR1; // 通道 1 和 2 在 CCMR1
        shift = (Channel == 1) ? 0 : 8;
    }
    else
    {
        CCMR = &TIMx->CCMR2; // 通道 3 和 4 在 CCMR2
        shift = (Channel == 3) ? 0 : 8;
    }

    tmp = *CCMR;

    // 清除对应通道的 OCxCE 位
    tmp &= (uint16_t)~(0x1 << shift);

    // 设置新的清零状态
    tmp |= (TIM_OCClear << shift);

    // 写回寄存器
    *CCMR = tmp;
}



/**
  * @brief    配置 TIMx 指定通道的输出比较（Output Compare）极性
  *           可以配置主输出（OCx）或互补输出（OCxN）极性。
  * @param    TIMx: 选择 TIM 外设（高级定时器和一般定时器）
  * @param    Channel: 通道号
  *           - 1~4: 主输出 OC1~OC4
  *           - 0x81~0x84: 互补输出 OC1N~OC4N（高位加 0x80 表示互补）
  * @param    Polarity: 极性选择
  *           - TIM_OCPolarity_High / TIM_OCNPolarity_High: 高电平有效
  *           - TIM_OCPolarity_Low  / TIM_OCNPolarity_Low : 低电平有效
  * @retval   None
  * @note     互补输出仅 TIM1 和 TIM8 支持，普通定时器没有互补输出
  *           主输出与互补输出可同时存在，用于半桥/全桥驱动场景
  */
void myTIM_OCPolarityConfig(TIM_TypeDef* TIMx, uint8_t Channel, uint16_t Polarity)
{
    uint16_t tmpccer = TIMx->CCER;

    switch (Channel)
    {
    case 1: // 主输出 OC1
        assert_param(IS_TIM_LIST1_PERIPH(TIMx));
        assert_param(IS_TIM_OC_POLARITY(Polarity));
        tmpccer &= ~TIM_CCER_CC1P;       // 清除 CC1P 位
        tmpccer |= Polarity;              // 设置极性
        break;
    case 2: // 主输出 OC2
        assert_param(IS_TIM_LIST2_PERIPH(TIMx));
        assert_param(IS_TIM_OC_POLARITY(Polarity));
        tmpccer &= ~TIM_CCER_CC2P;
        tmpccer |= (Polarity << 4);
        break;
    case 3: // 主输出 OC3
        assert_param(IS_TIM_LIST3_PERIPH(TIMx));
        assert_param(IS_TIM_OC_POLARITY(Polarity));
        tmpccer &= ~TIM_CCER_CC3P;
        tmpccer |= (Polarity << 8);
        break;
    case 4: // 主输出 OC4
        assert_param(IS_TIM_LIST3_PERIPH(TIMx));
        assert_param(IS_TIM_OC_POLARITY(Polarity));
        tmpccer &= ~TIM_CCER_CC4P;
        tmpccer |= (Polarity << 12);
        break;
    case 0x81: // 互补输出 OC1N
        assert_param(IS_TIM_LIST4_PERIPH(TIMx));
        assert_param(IS_TIM_OCN_POLARITY(Polarity));
        tmpccer &= ~TIM_CCER_CC1NP;
        tmpccer |= Polarity;
        break;
    case 0x82: // 互补输出 OC2N
        assert_param(IS_TIM_LIST4_PERIPH(TIMx));
        assert_param(IS_TIM_OCN_POLARITY(Polarity));
        tmpccer &= ~TIM_CCER_CC2NP;
        tmpccer |= (Polarity << 4);
        break;
    case 0x83: // 互补输出 OC3N
        assert_param(IS_TIM_LIST4_PERIPH(TIMx));
        assert_param(IS_TIM_OCN_POLARITY(Polarity));
        tmpccer &= ~TIM_CCER_CC3NP;
        tmpccer |= (Polarity << 8);
        break;
    case 0x84: // 互补输出 OC4N (高级定时器没有 OC4N，但占位)
        assert_param(IS_TIM_LIST4_PERIPH(TIMx));
        assert_param(IS_TIM_OCN_POLARITY(Polarity));
        tmpccer &= ~TIM_CCER_CC4NP;
        tmpccer |= (Polarity << 12);
        break;
    default:
        return; // 非法通道直接返回
    }

    TIMx->CCER = tmpccer;
}


/**
  * @brief    配置 TIMx 指定通道捕获/比较输出使能状态
  *           可配置主输出（CCxE）或互补输出（CCxNE）
  * @param    TIMx: TIM 外设，范围 TIM1~TIM14（部分高级定时器支持互补输出）
  * @param    Channel: 通道号
  *           - 1~4: 主输出 CC1~CC4
  *           - 0x81~0x83: 互补输出 CC1N~CC3N（高位加 0x80 表示互补）
  * @param    Cmd: 新状态
  *           - TIM_CCx_Enable / TIM_CCx_Disable: 主输出使能/禁止
  *           - TIM_CCxN_Enable / TIM_CCxN_Disable: 互补输出使能/禁止
  * @retval   None
  * @note     主输出用于普通 PWM 或输出比较，互补输出用于半桥/全桥驱动场景
  */
void myTIM_CCxCmd(TIM_TypeDef* TIMx, uint8_t Channel, uint16_t Cmd)
{
    uint16_t tmp = 0;

    if (Channel <= 4) // 主输出 CCx
    {
        assert_param(IS_TIM_LIST1_PERIPH(TIMx));
        assert_param(IS_TIM_CCX(Cmd));

        tmp = CCER_CCE_SET << (Channel - 1);       // 计算对应 CCxE 位掩码
        TIMx->CCER &= ~tmp;                        // 清零
        TIMx->CCER |= (Cmd << (Channel - 1));      // 设置新状态
    }
    else if (Channel >= 0x81 && Channel <= 0x83) // 互补输出 CCxN
    {
        assert_param(IS_TIM_LIST4_PERIPH(TIMx));
        assert_param(IS_TIM_CCXN(Cmd));

        uint8_t ch = Channel - 0x80;              // 转换为 1~3
        tmp = CCER_CCNE_SET << (ch - 1);          // 计算对应 CCxNE 位掩码
        TIMx->CCER &= ~tmp;                        // 清零
        TIMx->CCER |= (Cmd << (ch - 1));          // 设置新状态
    }
    else
    {
        return; // 非法通道直接返回
    }
}
/*

##### 输入捕获管理函数 #####
##### TIM 驱动：如何在输入捕获模式下使用 #####
(#) 使用函数 RCC_APBxPeriphClockCmd(RCC_APBxPeriph_TIMx, ENABLE) 启用 TIM 时钟

(#) 配置 TIM 引脚，通过相应 GPIO 引脚进行配置

(#) 配置定时器基本单元（Time base unit），如果需要，否则定时器将以默认配置运行：
(++) 自动重装载值（Autoreload value） = 0xFFFF
(++) 预分频值（Prescaler value） = 0x0000
(++) 计数模式（Counter mode） = 向上计数（Up counting）
(++) 时钟分频（Clock Division） = TIM_CKD_DIV1

(#) 使用期望参数填充 TIM_ICInitStruct，包括：
(++) TIM 通道：TIM_Channel
(++) 输入捕获极性（Input Capture polarity）：TIM_ICPolarity
(++) 输入捕获选择（Input Capture selection）：TIM_ICSelection
(++) 输入捕获预分频器（Input Capture Prescaler）：TIM_ICPrescaler
(++) 输入捕获滤波器值（Input Capture filter value）：TIM_ICFilter

(#) 调用 TIM_ICInit(TIMx, &TIM_ICInitStruct) 配置指定通道，以测量输入信号的频率或占空比；
或调用 TIM_PWMIConfig(TIMx, &TIM_ICInitStruct) 配置指定通道，以测量输入信号的频率和占空比

(#) 启用 NVIC 或 DMA，以读取测量到的频率

(#) 使用 TIM_ITConfig(TIMx, TIM_IT_CCx) （或 TIM_DMA_Cmd(TIMx, TIM_DMA_CCx)）启用相应的中断（或 DMA 请求），以读取捕获值

(#) 调用 TIM_Cmd(ENABLE) 函数启用 TIM 计数器

(#) 使用 TIM_GetCapturex(TIMx) 读取捕获值

- @ - 其他函数可单独使用，以便在需要时修改定时器的特定功能
*/
/**
  * @brief   根据 TIM_ICInitStruct 中的参数初始化 TIM 外设的输入捕获功能。
  * @note    此函数可以配置 TIM 的任意通道（1~4），包括极性、输入选择、滤波器和预分频器。
  * @param   TIMx: 指定要配置的 TIM 外设，x 可以是 1~14（不包括 6 和 7）。
  * @param   TIM_ICInitStruct: 指向 TIM_ICInitTypeDef 结构体的指针，包含指定 TIM 外设的配置信息。
  *           结构体成员：
  *             - TIM_Channel: TIM 通道（TIM_Channel_1~TIM_Channel_4）
  *             - TIM_ICPolarity: 输入捕获极性（上升沿/下降沿）
  *             - TIM_ICSelection: 输入选择（直接/间接/其他）
  *             - TIM_ICPrescaler: 输入捕获预分频器
  *             - TIM_ICFilter: 输入捕获滤波器值
  * @retval  None
  */
void myTIM_ICInit(TIM_TypeDef* TIMx, TIM_ICInitTypeDef* TIM_ICInitStruct)
{

    /* 定义指向通道配置函数的指针 */
    void (*TI_Config[4])(TIM_TypeDef*, uint16_t, uint16_t, uint8_t) = {
        TI1_Config,  /* 通道1 */
        TI2_Config,  /* 通道2 */
        TI3_Config,  /* 通道3 */
        TI4_Config   /* 通道4 */
    };

    /* 定义设置预分频器的函数指针 */
    void (*SetICPrescaler[4])(TIM_TypeDef*, uint16_t) = {
        TIM_SetIC1Prescaler,
        TIM_SetIC2Prescaler,
        TIM_SetIC3Prescaler,
        TIM_SetIC4Prescaler
    };

    uint8_t ch_index = 0;
    /* 校验参数 */
    assert_param(IS_TIM_LIST1_PERIPH(TIMx));
    assert_param(IS_TIM_IC_POLARITY(TIM_ICInitStruct->TIM_ICPolarity));
    assert_param(IS_TIM_IC_SELECTION(TIM_ICInitStruct->TIM_ICSelection));
    assert_param(IS_TIM_IC_PRESCALER(TIM_ICInitStruct->TIM_ICPrescaler));
    assert_param(IS_TIM_IC_FILTER(TIM_ICInitStruct->TIM_ICFilter));
    assert_param(IS_TIM_CHANNEL(TIM_ICInitStruct->TIM_Channel));

    /* 根据通道号计算数组索引 */
    switch (TIM_ICInitStruct->TIM_Channel)
    {
    case TIM_Channel_1: ch_index = 0; break;
    case TIM_Channel_2:
        ch_index = 1;
        assert_param(IS_TIM_LIST2_PERIPH(TIMx));
        break;
    case TIM_Channel_3:
        ch_index = 2;
        assert_param(IS_TIM_LIST3_PERIPH(TIMx));
        break;
    case TIM_Channel_4:
        ch_index = 3;
        assert_param(IS_TIM_LIST3_PERIPH(TIMx));
        break;
    default: return; /* 非法通道直接返回 */
    }

    /* 配置输入捕获通道 */
    TI_Config[ch_index](TIMx,
        TIM_ICInitStruct->TIM_ICPolarity,
        TIM_ICInitStruct->TIM_ICSelection,
        TIM_ICInitStruct->TIM_ICFilter);

    /* 设置输入捕获预分频器 */
    SetICPrescaler[ch_index](TIMx, TIM_ICInitStruct->TIM_ICPrescaler);
}

/**
  * @brief  用默认值初始化 TIM 输入捕获配置结构体。
  * @note   本函数用于在使用 TIM 输入捕获功能前，将结构体成员设置为合理的初始值，
  *         避免未初始化带来的不确定行为。通常在调用 TIM_ICInit() 配置输入捕获
  *         通道前先调用此函数，然后再根据实际需求修改部分成员。
  *
  * @param  TIM_ICInitStruct: 指向 TIM_ICInitTypeDef 类型结构体的指针，该结构体
  *         将被初始化为默认值。
  *           - TIM_Channel: 默认通道为 TIM_Channel_1
  *           - TIM_ICPolarity: 输入捕获极性，默认上升沿捕获（TIM_ICPolarity_Rising）
  *           - TIM_ICSelection: 输入选择，默认直接连接到通道输入（TIM_ICSelection_DirectTI）
  *           - TIM_ICPrescaler: 输入捕获预分频器，默认不分频（TIM_ICPSC_DIV1）
  *           - TIM_ICFilter: 输入捕获滤波器值，默认禁用滤波（0x00）
  * @retval None
  *
  * @note   使用示例:
  *         TIM_ICInitTypeDef  TIM_ICInitStructure;
  *         TIM_ICStructInit(&TIM_ICInitStructure);
  *         TIM_ICInitStructure.TIM_Channel = TIM_Channel_2; // 修改需要的通道
  *         TIM_ICInit(TIMx, &TIM_ICInitStructure);
  */
void myTIM_ICStructInit(TIM_ICInitTypeDef* TIM_ICInitStruct)
{
    /* 设置默认配置 */
    TIM_ICInitStruct->TIM_Channel = TIM_Channel_1;           // 默认使用通道1
    TIM_ICInitStruct->TIM_ICPolarity = TIM_ICPolarity_Rising; // 默认上升沿捕获
    TIM_ICInitStruct->TIM_ICSelection = TIM_ICSelection_DirectTI; // 默认直接输入通道
    TIM_ICInitStruct->TIM_ICPrescaler = TIM_ICPSC_DIV1;     // 默认输入不分频
    TIM_ICInitStruct->TIM_ICFilter = 0x00;                  // 默认禁用滤波器
}


/**
  * @brief  配置定时器 TIM 外设以测量外部 PWM 信号
  * @param  TIMx: 指定 TIM 外设，x 可为 1,2,3,4,5,8,9 或 12
  * @param  TIM_ICInitStruct: 指向 TIM_ICInitTypeDef 结构体的指针，包含以下配置：
  *         - TIM_Channel: 主通道（测量通道），可为 TIM_Channel_1 或 TIM_Channel_2
  *         - TIM_ICPolarity: 输入捕获极性（上升沿/下降沿）
  *         - TIM_ICSelection: 输入捕获选择（直接或间接 TI）
  *         - TIM_ICPrescaler: 输入捕获预分频
  *         - TIM_ICFilter: 输入捕获滤波值
  * @retval None
  * @note   本函数用于 PWM 输入模式，会同时配置两个通道：
  *         - 主通道按照 TIM_ICInitStruct 配置
  *         - 辅通道配置相反极性和相反输入选择，用于测量另一相位
  */
void myTIM_PWMIConfig(TIM_TypeDef* TIMx, TIM_ICInitTypeDef* TIM_ICInitStruct)
{
    /* 参数检查 */
    assert_param(IS_TIM_LIST2_PERIPH(TIMx));

    /* 定义主通道和辅通道编号（使用数组映射） */
    uint8_t mainChannelIndex = (TIM_ICInitStruct->TIM_Channel == TIM_Channel_1) ? 0 : 1;
    uint8_t oppositeChannelIndex = 1 - mainChannelIndex;

    /* 数组映射：0 -> Channel1, 1 -> Channel2 */
    void (*TI_Config[2])(TIM_TypeDef*, uint16_t, uint16_t, uint16_t) = { TI1_Config, TI2_Config };
    void (*SetICPrescaler[2])(TIM_TypeDef*, uint16_t) = { TIM_SetIC1Prescaler, TIM_SetIC2Prescaler };

    /* 配置主通道极性和输入 */
    uint16_t mainPolarity = TIM_ICInitStruct->TIM_ICPolarity;
    uint16_t mainSelection = TIM_ICInitStruct->TIM_ICSelection;

    /* 配置辅通道极性和输入（相反极性/相反选择） */
    uint16_t oppositePolarity = (mainPolarity == TIM_ICPolarity_Rising) ? TIM_ICPolarity_Falling : TIM_ICPolarity_Rising;
    uint16_t oppositeSelection = (mainSelection == TIM_ICSelection_DirectTI) ? TIM_ICSelection_IndirectTI : TIM_ICSelection_DirectTI;

    /* 配置主通道 */
    TI_Config[mainChannelIndex](TIMx, mainPolarity, mainSelection, TIM_ICInitStruct->TIM_ICFilter);
    SetICPrescaler[mainChannelIndex](TIMx, TIM_ICInitStruct->TIM_ICPrescaler);

    /* 配置辅通道 */
    TI_Config[oppositeChannelIndex](TIMx, oppositePolarity, oppositeSelection, TIM_ICInitStruct->TIM_ICFilter);
    SetICPrescaler[oppositeChannelIndex](TIMx, TIM_ICInitStruct->TIM_ICPrescaler);
}


/**
  * @brief  获取指定定时器通道的输入捕获值
  * @param  TIMx: 要操作的定时器，例如 TIM1、TIM2 等
  * @param  Channel: 要读取的通道号
  *          可选：
  *            TIM_Channel_1 -> 通道 1
  *            TIM_Channel_2 -> 通道 2
  *            TIM_Channel_3 -> 通道 3
  *            TIM_Channel_4 -> 通道 4
  * @retval 返回该通道的捕获值（CCR 寄存器的值）
  *
  * @note  输入捕获模式下，每个通道都会把外部信号的计数值保存到 CCR 寄存器。
  *        调用这个函数就可以读取捕获值，用来测量信号的周期或脉宽。
  */
uint32_t myTIM_GetCapture(TIM_TypeDef* TIMx, uint8_t Channel)
{
    switch (Channel)
    {
    case TIM_Channel_1:
        assert_param(IS_TIM_LIST1_PERIPH(TIMx)); // 检查定时器是否支持通道 1
        return TIMx->CCR1; // 返回通道 1 捕获值

    case TIM_Channel_2:
        assert_param(IS_TIM_LIST2_PERIPH(TIMx)); // 检查定时器是否支持通道 2
        return TIMx->CCR2; // 返回通道 2 捕获值

    case TIM_Channel_3:
        assert_param(IS_TIM_LIST3_PERIPH(TIMx)); // 检查定时器是否支持通道 3
        return TIMx->CCR3; // 返回通道 3 捕获值

    case TIM_Channel_4:
        assert_param(IS_TIM_LIST3_PERIPH(TIMx)); // 检查定时器是否支持通道 4
        return TIMx->CCR4; // 返回通道 4 捕获值

    default:
        return 0; // 非法通道返回 0
    }
}


/**
  * @brief  设置指定定时器通道的输入捕获预分频器（Prescaler）。
  * @param  TIMx: 要操作的定时器，例如 TIM1、TIM2 等
  * @param  Channel: 要设置的通道号，可选：
  *            TIM_Channel_1 -> 通道 1
  *            TIM_Channel_2 -> 通道 2
  *            TIM_Channel_3 -> 通道 3
  *            TIM_Channel_4 -> 通道 4
  * @param  Prescaler: 捕获预分频值，可选：
  *            TIM_ICPSC_DIV1 -> 不分频，每次事件都捕获
  *            TIM_ICPSC_DIV2 -> 每 2 次事件捕获一次
  *            TIM_ICPSC_DIV4 -> 每 4 次事件捕获一次
  *            TIM_ICPSC_DIV8 -> 每 8 次事件捕获一次
  * @retval None
  *
  * @note  输入捕获模式下，如果外部信号太快，可以用预分频器降低捕获频率，
  *        避免捕获值溢出或丢失事件。
  */
void myTIM_SetICPrescaler(TIM_TypeDef* TIMx, uint8_t Channel, uint16_t Prescaler)
{
    /* 检查定时器和预分频值是否合法 */
    switch (Channel)
    {
    case TIM_Channel_1:
        assert_param(IS_TIM_LIST1_PERIPH(TIMx));
        TIMx->CCMR1 &= ~TIM_CCMR1_IC1PSC;  // 清空原来的预分频值
        TIMx->CCMR1 |= Prescaler;           // 设置新的预分频值
        break;

    case TIM_Channel_2:
        assert_param(IS_TIM_LIST2_PERIPH(TIMx));
        TIMx->CCMR1 &= ~TIM_CCMR1_IC2PSC;
        TIMx->CCMR1 |= (Prescaler << 8);
        break;

    case TIM_Channel_3:
        assert_param(IS_TIM_LIST3_PERIPH(TIMx));
        TIMx->CCMR2 &= ~TIM_CCMR2_IC3PSC;
        TIMx->CCMR2 |= Prescaler;
        break;

    case TIM_Channel_4:
        assert_param(IS_TIM_LIST3_PERIPH(TIMx));
        TIMx->CCMR2 &= ~TIM_CCMR2_IC4PSC;
        TIMx->CCMR2 |= (Prescaler << 8);
        break;

    default:
        // 非法通道，不做任何操作
        break;
    }
}

/**
  * @defgroup TIM_Group4 高级定时器（TIM1 和 TIM8）特有功能
  * @brief   高级定时器（TIM1 和 TIM8）的特殊功能
  *
  * 说明：高级定时器支持断路（Break）功能，用于保护外设和电路安全。
  *
  * 使用断路功能的步骤：
  *  1. 配置定时器通道的输出比较模式（Output Compare Mode）。
  *  2. 填写 TIM_BDTRInitStruct 结构体，设置：
  *       - 断路极性（Break Polarity）：断路触发时的信号状态
  *       - 死区时间（Dead Time）：避免开关器件短路
  *       - 锁定等级（Lock Level）：保护配置不被意外修改
  *       - OSSI/OSSR 状态：空闲输出状态设置
  *       - 自动输出使能（AOE）：使能后自动控制输出
  *  3. 调用 TIM_BDTRConfig(TIMx, &TIM_BDTRInitStruct) 配置定时器断路功能。
  *  4. 使用 TIM_CtrlPWMOutputs(TIM1, ENABLE) 使能定时器主输出。
  *  5. 当断路事件发生时：
  *       - 定时器输出会被置为复位状态或者已知安全状态
  *       - 根据 TIM_BDTRConfig 的设置来保护外设，避免损坏。
  *
  * 注：这个功能通常用于驱动功率电路（如电机或功率MOSFET）时，防止短路或异常电流损坏器件。
  */


  /**
    * @brief  配置高级定时器（TIM1 或 TIM8）的断路（Break）功能、死区时间、
    *         锁定等级、OSSR/OSSI 状态以及自动输出（AOE）。
    * @param  TIMx: 选择的高级定时器，取值 TIM1 或 TIM8。
    * @param  TIM_BDTRInitStruct: 指向 TIM_BDTRInitTypeDef 结构体的指针，
    *         该结构体包含了 BDTR 寄存器的所有配置参数。
    * @retval None
    *
    * 说明（小白也能懂）：
    *  1. 断路功能（Break）：当检测到异常情况（如过流、短路）时，自动将输出信号置为安全状态。
    *  2. 死区时间（Dead Time）：用于控制高低边开关的切换间隔，避免开关器件短路。
    *  3. 锁定等级（Lock Level）：防止重要寄存器被意外修改。
    *  4. OSSR/OSSI 状态：空闲输出状态设置（正常工作和断路后的行为）。
    *  5. 自动输出（AOE）：使能后，定时器会自动控制输出，无需手动开启。
    */
void myTIM_BDTRConfig(TIM_TypeDef* TIMx, TIM_BDTRInitTypeDef* TIM_BDTRInitStruct)
{
    /* 参数检查 */
    assert_param(IS_TIM_LIST4_PERIPH(TIMx));                              // 只允许 TIM1 和 TIM8
    assert_param(IS_TIM_OSSR_STATE(TIM_BDTRInitStruct->TIM_OSSRState));
    assert_param(IS_TIM_OSSI_STATE(TIM_BDTRInitStruct->TIM_OSSIState));
    assert_param(IS_TIM_LOCK_LEVEL(TIM_BDTRInitStruct->TIM_LOCKLevel));
    assert_param(IS_TIM_BREAK_STATE(TIM_BDTRInitStruct->TIM_Break));
    assert_param(IS_TIM_BREAK_POLARITY(TIM_BDTRInitStruct->TIM_BreakPolarity));
    assert_param(IS_TIM_AUTOMATIC_OUTPUT_STATE(TIM_BDTRInitStruct->TIM_AutomaticOutput));

    /* 配置 BDTR 寄存器，将所有功能一次性写入 */
    TIMx->BDTR = (uint32_t)TIM_BDTRInitStruct->TIM_OSSRState |
        TIM_BDTRInitStruct->TIM_OSSIState |
        TIM_BDTRInitStruct->TIM_LOCKLevel |
        TIM_BDTRInitStruct->TIM_DeadTime |
        TIM_BDTRInitStruct->TIM_Break |
        TIM_BDTRInitStruct->TIM_BreakPolarity |
        TIM_BDTRInitStruct->TIM_AutomaticOutput;
}

/**
  * @brief  将 TIM_BDTRInitStruct 结构体初始化为默认配置。
  * @param  TIM_BDTRInitStruct: 指向 TIM_BDTRInitTypeDef 结构体的指针，将被初始化。
  * @retval None
  *
  * 默认配置说明：
  *  - OSSRState = Disable：空闲状态复位输出关闭
  *  - OSSIState = Disable：空闲状态死区输出关闭
  *  - LockLevel = OFF：锁定等级关闭，可修改寄存器
  *  - DeadTime = 0x00：死区时间为0
  *  - Break = Disable：断路功能关闭
  *  - BreakPolarity = Low：断路信号低电平有效
  *  - AutomaticOutput = Disable：自动输出关闭
  *
  * 小白提示：调用此函数可以快速生成一个安全的默认配置，然后再根据需要修改某些参数。
  */
void myTIM_BDTRStructInit(TIM_BDTRInitTypeDef* TIM_BDTRInitStruct)
{
    TIM_BDTRInitStruct->TIM_OSSRState = TIM_OSSRState_Disable;
    TIM_BDTRInitStruct->TIM_OSSIState = TIM_OSSIState_Disable;
    TIM_BDTRInitStruct->TIM_LOCKLevel = TIM_LOCKLevel_OFF;
    TIM_BDTRInitStruct->TIM_DeadTime = 0x00;
    TIM_BDTRInitStruct->TIM_Break = TIM_Break_Disable;
    TIM_BDTRInitStruct->TIM_BreakPolarity = TIM_BreakPolarity_Low;
    TIM_BDTRInitStruct->TIM_AutomaticOutput = TIM_AutomaticOutput_Disable;
}


/**
  * @brief  控制高级定时器（TIM1 或 TIM8）主输出开关（MOE）。
  * @param  TIMx: 高级定时器，取值 TIM1 或 TIM8。
  * @param  NewState: 主输出的新状态，可以是 ENABLE（使能）或 DISABLE（关闭）。
  * @retval None
  *
  * 小白说明：
  *  - 高级定时器有主输出功能（MOE），用于驱动桥式电路或者外部PWM输出。
  *  - ENABLE：打开主输出，定时器可以输出PWM信号。
  *  - DISABLE：关闭主输出，所有输出被强制置为安全状态。
  */
void myTIM_CtrlPWMOutputs(TIM_TypeDef* TIMx, FunctionalState NewState)
{
    assert_param(IS_TIM_LIST4_PERIPH(TIMx));
    assert_param(IS_FUNCTIONAL_STATE(NewState));

    // 三目运算符实现开关
    NewState != DISABLE ? (TIMx->BDTR |= TIM_BDTR_MOE) : (TIMx->BDTR &= ~TIM_BDTR_MOE);
}

/**
  * @brief  控制高级定时器（TIM1 或 TIM8）的换相事件（Commutation Event）使能。
  * @param  TIMx: 高级定时器，取值 TIM1 或 TIM8。
  * @param  NewState: 换相事件的新状态，可以是 ENABLE 或 DISABLE。
  * @retval None
  *
  * 小白说明：
  *  - 换相事件用于电机控制，比如在三相电机PWM驱动中切换电流方向。
  *  - ENABLE：使能换相事件，定时器可在特定时刻触发换相。
  *  - DISABLE：关闭换相事件。
  */
void myTIM_SelectCOM(TIM_TypeDef* TIMx, FunctionalState NewState)
{
    assert_param(IS_TIM_LIST4_PERIPH(TIMx));
    assert_param(IS_FUNCTIONAL_STATE(NewState));

    // 三目运算符实现开关
    NewState != DISABLE ? (TIMx->CR2 |= TIM_CR2_CCUS) : (TIMx->CR2 &= ~TIM_CR2_CCUS);
}

/**
  * @brief  设置或清除高级定时器（TIM1 或 TIM8）的捕获比较寄存器预装载控制位（CCPC）。
  * @param  TIMx: 高级定时器，取值 TIM1 或 TIM8。
  * @param  NewState: CCPC位的新状态，可以是 ENABLE 或 DISABLE。
  * @retval None
  *
  * 小白说明：
  *  - 捕获比较预装载控制位用于控制 PWM 输出更新方式。
  *  - ENABLE：使能预装载，改变比较值后不会立即更新到输出，需要等待下一个更新事件。
  *  - DISABLE：关闭预装载，比较值修改立即生效。
  */
void myTIM_CCPreloadControl(TIM_TypeDef* TIMx, FunctionalState NewState)
{
    assert_param(IS_TIM_LIST4_PERIPH(TIMx));
    assert_param(IS_FUNCTIONAL_STATE(NewState));

    // 三目运算符实现开关
    NewState != DISABLE ? (TIMx->CR2 |= TIM_CR2_CCPC) : (TIMx->CR2 &= ~TIM_CR2_CCPC);
}


/** @defgroup TIM_Group5 中断、DMA 和标志管理函数
  * @brief   TIM 定时器的中断、DMA 和状态标志管理函数
  *
@verbatim
 ===============================================================================
          ##### 中断、DMA 和状态标志管理函数 #####
 ===============================================================================
    - 这一组函数用于管理定时器的中断、DMA 传输和状态标志。
    - 中断：当定时器发生特定事件（比如计数溢出、捕获比较事件）时触发。
    - DMA：定时器可以通过 DMA 自动把数据传输到内存或外设，减少CPU负担。
    - 标志：定时器内部事件的状态标志，程序可以用它来判断某个事件是否发生。
@endverbatim
  * @{
  */


  /**
   * @brief 使能或失能指定的 TIM 中断。
   * @param TIMx 指向 TIM 外设的指针，x 可以为 1~14。
   * @param TIM_IT 指定要使能或失能的 TIM 中断源。
   *               该参数可以是以下值的任意组合：
   *               @arg TIM_IT_Update   : TIM 更新中断
   *               @arg TIM_IT_CC1      : TIM 捕获比较 1 中断
   *               @arg TIM_IT_CC2      : TIM 捕获比较 2 中断
   *               @arg TIM_IT_CC3      : TIM 捕获比较 3 中断
   *               @arg TIM_IT_CC4      : TIM 捕获比较 4 中断
   *               @arg TIM_IT_COM      : TIM 组合事件中断
   *               @arg TIM_IT_Trigger  : TIM 触发事件中断
   *               @arg TIM_IT_Break    : TIM 断路事件中断
   * @note  TIM6 和 TIM7 只能使用 TIM_IT_Update。
   * @note  TIM9 和 TIM12 只能使用 TIM_IT_Update、TIM_IT_CC1、TIM_IT_CC2 或 TIM_IT_Trigger 中的一个。
   * @note  TIM10、TIM11、TIM13 和 TIM14 只能使用 TIM_IT_Update 或 TIM_IT_CC1。
   * @note  TIM_IT_COM 和 TIM_IT_Break 仅适用于 TIM1 和 TIM8。
   * @param NewState 新的中断状态，可以是 ENABLE 或 DISABLE。
   * @retval 无
   */
void myTIM_ITConfig(TIM_TypeDef* TIMx, uint16_t TIM_IT, FunctionalState NewState)
{
    /* 参数检查 */
    assert_param(IS_TIM_ALL_PERIPH(TIMx));
    assert_param(IS_TIM_IT(TIM_IT));
    assert_param(IS_FUNCTIONAL_STATE(NewState));

    /* 使用三目运算符设置中断 */
    NewState != DISABLE ? (TIMx->DIER |= TIM_IT) : (TIMx->DIER &= (uint16_t)(~TIM_IT));
}

/**
 * @brief 软件触发 TIMx 的指定事件。
 * @param TIMx 指向 TIM 外设的指针，x 可以为 1~14。
 * @param TIM_EventSource 指定事件源，可以是以下值之一或组合：
 *                        @arg TIM_EventSource_Update   : 更新事件
 *                        @arg TIM_EventSource_CC1      : 捕获比较 1 事件
 *                        @arg TIM_EventSource_CC2      : 捕获比较 2 事件
 *                        @arg TIM_EventSource_CC3      : 捕获比较 3 事件
 *                        @arg TIM_EventSource_CC4      : 捕获比较 4 事件
 *                        @arg TIM_EventSource_COM      : 组合事件
 *                        @arg TIM_EventSource_Trigger  : 触发事件
 *                        @arg TIM_EventSource_Break    : 断路事件
 * @note  TIM6 和 TIM7 仅能生成更新事件。
 * @note  TIM_EventSource_COM 和 TIM_EventSource_Break 仅适用于 TIM1 和 TIM8。
 * @retval 无
 */
void myTIM_GenerateEvent(TIM_TypeDef* TIMx, uint16_t TIM_EventSource)
{
    /* 参数检查 */
    assert_param(IS_TIM_ALL_PERIPH(TIMx));
    assert_param(IS_TIM_EVENT_SOURCE(TIM_EventSource));

    /* 设置事件源 */
    TIMx->EGR = TIM_EventSource;
}

/**
 * @brief 检查指定的 TIM 标志是否被置位。
 * @param TIMx 指向 TIM 外设的指针，x 可以为 1~14。
 * @param TIM_FLAG 指定要检查的标志，可以是以下值之一：
 *                 @arg TIM_FLAG_Update   : 更新标志
 *                 @arg TIM_FLAG_CC1      : 捕获比较 1 标志
 *                 @arg TIM_FLAG_CC2      : 捕获比较 2 标志
 *                 @arg TIM_FLAG_CC3      : 捕获比较 3 标志
 *                 @arg TIM_FLAG_CC4      : 捕获比较 4 标志
 *                 @arg TIM_FLAG_COM      : 组合事件标志
 *                 @arg TIM_FLAG_Trigger  : 触发事件标志
 *                 @arg TIM_FLAG_Break    : 断路事件标志
 *                 @arg TIM_FLAG_CC1OF    : 捕获比较 1 过捕获标志
 *                 @arg TIM_FLAG_CC2OF    : 捕获比较 2 过捕获标志
 *                 @arg TIM_FLAG_CC3OF    : 捕获比较 3 过捕获标志
 *                 @arg TIM_FLAG_CC4OF    : 捕获比较 4 过捕获标志
 * @note  TIM6 和 TIM7 仅有更新标志。
 * @note  TIM_FLAG_COM 和 TIM_FLAG_Break 仅适用于 TIM1 和 TIM8。
 * @retval TIM_FLAG 的状态 (SET 或 RESET)
 */
FlagStatus myTIM_GetFlagStatus(TIM_TypeDef* TIMx, uint16_t TIM_FLAG)
{
    /* 参数检查 */
    assert_param(IS_TIM_ALL_PERIPH(TIMx));
    assert_param(IS_TIM_GET_FLAG(TIM_FLAG));

    /* 使用三目运算符返回标志状态 */
    return ((TIMx->SR & TIM_FLAG) != (uint16_t)RESET) ? SET : RESET;
}


/**
  * @brief  清除 TIMx 的指定事件标志位。
  * @param  TIMx: 指向 TIM 外设的指针，x 可以为 1~14。
  * @param  TIM_FLAG: 指定要清除的标志位，可组合以下值：
  *          @arg TIM_FLAG_Update   : 更新标志
  *          @arg TIM_FLAG_CC1      : 捕获比较 1 标志
  *          @arg TIM_FLAG_CC2      : 捕获比较 2 标志
  *          @arg TIM_FLAG_CC3      : 捕获比较 3 标志
  *          @arg TIM_FLAG_CC4      : 捕获比较 4 标志
  *          @arg TIM_FLAG_COM      : 组合事件标志
  *          @arg TIM_FLAG_Trigger  : 触发事件标志
  *          @arg TIM_FLAG_Break    : 断路事件标志
  *          @arg TIM_FLAG_CC1OF    : 捕获比较 1 过捕获标志
  *          @arg TIM_FLAG_CC2OF    : 捕获比较 2 过捕获标志
  *          @arg TIM_FLAG_CC3OF    : 捕获比较 3 过捕获标志
  *          @arg TIM_FLAG_CC4OF    : 捕获比较 4 过捕获标志
  * @note   TIM6 和 TIM7 仅有更新标志。
  * @note   TIM_FLAG_COM 和 TIM_FLAG_Break 仅适用于 TIM1 和 TIM8。
  * @retval 无
  */
void myTIM_ClearFlag(TIM_TypeDef* TIMx, uint16_t TIM_FLAG)
{
    /* 参数检查 */
    assert_param(IS_TIM_ALL_PERIPH(TIMx));

    /* 清除标志位 */
    TIMx->SR = (uint16_t)(~TIM_FLAG);
}

/**
  * @brief  检查 TIM 指定中断是否已发生。
  * @param  TIMx: 指向 TIM 外设的指针，x 可以为 1~14。
  * @param  TIM_IT: 要检查的中断源，可选值：
  *          @arg TIM_IT_Update   : 更新中断
  *          @arg TIM_IT_CC1      : 捕获比较 1 中断
  *          @arg TIM_IT_CC2      : 捕获比较 2 中断
  *          @arg TIM_IT_CC3      : 捕获比较 3 中断
  *          @arg TIM_IT_CC4      : 捕获比较 4 中断
  *          @arg TIM_IT_COM      : 组合事件中断
  *          @arg TIM_IT_Trigger  : 触发事件中断
  *          @arg TIM_IT_Break    : 断路事件中断
  * @note   TIM6 和 TIM7 仅能产生更新中断。
  * @note   TIM_IT_COM 和 TIM_IT_Break 仅适用于 TIM1 和 TIM8。
  * @retval TIM_IT 的状态 (SET 或 RESET)
  */
ITStatus myTIM_GetITStatus(TIM_TypeDef* TIMx, uint16_t TIM_IT)
{
    ITStatus bitstatus = RESET;
    uint16_t itstatus = 0, itenable = 0;

    /* 参数检查 */
    assert_param(IS_TIM_ALL_PERIPH(TIMx));
    assert_param(IS_TIM_GET_IT(TIM_IT));

    itstatus = TIMx->SR & TIM_IT;
    itenable = TIMx->DIER & TIM_IT;

    /* 使用三目运算符判断中断状态 */
    bitstatus = ((itstatus != 0) && (itenable != 0)) ? SET : RESET;
    return bitstatus;
}

/**
  * @brief  清除 TIMx 的中断挂起位。
  * @param  TIMx: 指向 TIM 外设的指针，x 可以为 1~14。
  * @param  TIM_IT: 指定要清除的中断挂起位，可组合以下值：
  *          @arg TIM_IT_Update
  *          @arg TIM_IT_CC1
  *          @arg TIM_IT_CC2
  *          @arg TIM_IT_CC3
  *          @arg TIM_IT_CC4
  *          @arg TIM_IT_COM
  *          @arg TIM_IT_Trigger
  *          @arg TIM_IT_Break
  * @note   TIM6 和 TIM7 仅能产生更新中断。
  * @note   TIM_IT_COM 和 TIM_IT_Break 仅适用于 TIM1 和 TIM8。
  * @retval 无
  */
void myTIM_ClearITPendingBit(TIM_TypeDef* TIMx, uint16_t TIM_IT)
{
    /* 参数检查 */
    assert_param(IS_TIM_ALL_PERIPH(TIMx));

    /* 清除中断挂起位 */
    TIMx->SR = (uint16_t)(~TIM_IT);
}

/**
  * @brief  配置 TIMx 的 DMA 接口。
  * @param  TIMx: 指向 TIM 外设的指针，x 可以为 1,2,3,4,5 或 8。
  * @param  TIM_DMABase: DMA 基地址，可选值：
  *          @arg TIM_DMABase_CR1
  *          @arg TIM_DMABase_CR2
  *          @arg TIM_DMABase_SMCR
  *          @arg TIM_DMABase_DIER
  *          @arg TIM1_DMABase_SR
  *          @arg TIM_DMABase_EGR
  *          @arg TIM_DMABase_CCMR1
  *          @arg TIM_DMABase_CCMR2
  *          @arg TIM_DMABase_CCER
  *          @arg TIM_DMABase_CNT
  *          @arg TIM_DMABase_PSC
  *          @arg TIM_DMABase_ARR
  *          @arg TIM_DMABase_RCR
  *          @arg TIM_DMABase_CCR1
  *          @arg TIM_DMABase_CCR2
  *          @arg TIM_DMABase_CCR3
  *          @arg TIM_DMABase_CCR4
  *          @arg TIM_DMABase_BDTR
  *          @arg TIM_DMABase_DCR
  * @param  TIM_DMABurstLength: DMA 传输长度，可选值 TIM_DMABurstLength_1Transfer~TIM_DMABurstLength_18Transfers。
  * @retval 无
  */
void myTIM_DMAConfig(TIM_TypeDef* TIMx, uint16_t TIM_DMABase, uint16_t TIM_DMABurstLength)
{
    /* 参数检查 */
    assert_param(IS_TIM_LIST3_PERIPH(TIMx));
    assert_param(IS_TIM_DMA_BASE(TIM_DMABase));
    assert_param(IS_TIM_DMA_LENGTH(TIM_DMABurstLength));

    /* 配置 DMA 基地址和传输长度 */
    TIMx->DCR = TIM_DMABase | TIM_DMABurstLength;
}

/**
  * @brief  使能或失能 TIMx 的 DMA 请求。
  * @param  TIMx: 指向 TIM 外设的指针，x 可以为 1~8。
  * @param  TIM_DMASource: 指定 DMA 请求源，可组合以下值：
  *          @arg TIM_DMA_Update
  *          @arg TIM_DMA_CC1
  *          @arg TIM_DMA_CC2
  *          @arg TIM_DMA_CC3
  *          @arg TIM_DMA_CC4
  *          @arg TIM_DMA_COM
  *          @arg TIM_DMA_Trigger
  * @param  NewState: DMA 请求的新状态，ENABLE 或 DISABLE。
  * @retval 无
  */
void myTIM_DMACmd(TIM_TypeDef* TIMx, uint16_t TIM_DMASource, FunctionalState NewState)
{
    /* 参数检查 */
    assert_param(IS_TIM_LIST5_PERIPH(TIMx));
    assert_param(IS_TIM_DMA_SOURCE(TIM_DMASource));
    assert_param(IS_FUNCTIONAL_STATE(NewState));

    /* 使用三目运算符设置 DMA 请求 */
    NewState != DISABLE ? (TIMx->DIER |= TIM_DMASource) : (TIMx->DIER &= (uint16_t)(~TIM_DMASource));
}

/**
  * @brief  选择 TIMx 的捕获比较 DMA 请求源。
  * @param  TIMx: 指向 TIM 外设的指针，x 可以为 1,2,3,4,5 或 8。
  * @param  NewState: 捕获比较 DMA 请求的新状态，ENABLE 或 DISABLE。
  * @retval 无
  */
void myTIM_SelectCCDMA(TIM_TypeDef* TIMx, FunctionalState NewState)
{
    /* 参数检查 */
    assert_param(IS_TIM_LIST3_PERIPH(TIMx));
    assert_param(IS_FUNCTIONAL_STATE(NewState));

    /* 使用三目运算符设置或清除 CCDS 位 */
    NewState != DISABLE ? (TIMx->CR2 |= TIM_CR2_CCDS) : (TIMx->CR2 &= (uint16_t)(~TIM_CR2_CCDS));
}

/**
  * @defgroup TIM_Group6 定时器时钟管理函数
  * @brief    定时器时钟管理相关函数
  *
@verbatim
 ===============================================================================
                  ##### 定时器时钟管理函数 #####
 ===============================================================================
@endverbatim
*/


/**
  * @brief  配置 TIMx 使用内部时钟。
  * @param  TIMx: 指向 TIM 外设的指针，x 可以为 1,2,3,4,5,8,9 或 12。
  * @note   函数会禁用从模式，以便直接使用内部时钟驱动预分频器。
  * @retval 无
  */
void myTIM_InternalClockConfig(TIM_TypeDef* TIMx)
{
    /* 参数检查 */
    assert_param(IS_TIM_LIST2_PERIPH(TIMx));

    /* 禁用从模式，直接使用内部时钟 */
    TIMx->SMCR &= (uint16_t)(~TIM_SMCR_SMS);
}

/**
  * @brief  配置 TIMx 内部触发器作为外部时钟源。
  * @param  TIMx: 指向 TIM 外设的指针，x 可以为 1,2,3,4,5,8,9 或 12。
  * @param  TIM_InputTriggerSource: 内部触发源，可选值：
  *          @arg TIM_TS_ITR0 : 内部触发器 0
  *          @arg TIM_TS_ITR1 : 内部触发器 1
  *          @arg TIM_TS_ITR2 : 内部触发器 2
  *          @arg TIM_TS_ITR3 : 内部触发器 3
  * @retval 无
  *
  * @note 应用示例：
  *       1. 多个定时器同步：
  *          - TIM2 用内部时钟自己走；
  *          - TIM3 通过 TIM2 的内部触发器（ITR）驱动计数器；
  *          - 这样 TIM3 的计数与 TIM2 完全同步，适合同步生成 PWM 或 ADC 采样。
  *       2. 触发链：
  *          - TIM1 的输出事件触发 TIM4 开始计数；
  *          - TIM4 只在 TIM1 事件发生时才计数，实现精确事件控制。
  *       3. 工程中可用来建立“定时器链”，让多个定时器按特定节奏工作，而不依赖外部脉冲。
  */
void myTIM_ITRxExternalClockConfig(TIM_TypeDef* TIMx, uint16_t TIM_InputTriggerSource)
{
    /* 参数检查 */
    assert_param(IS_TIM_LIST2_PERIPH(TIMx));
    assert_param(IS_TIM_INTERNAL_TRIGGER_SELECTION(TIM_InputTriggerSource));

    /* 选择内部触发器 */
    TIM_SelectInputTrigger(TIMx, TIM_InputTriggerSource);

    /* 配置为外部时钟模式1 */
    TIMx->SMCR |= TIM_SlaveMode_External1;
}
/**
  * @brief  配置 TIMx 的触发器作为外部时钟源。
  * @param  TIMx: 指向 TIM 外设的指针，x 可以为 1~5,8,9,10,11,12,13,14。
  * @param  TIM_TIxExternalCLKSource: 外部时钟源，可选值：
  *          @arg TIM_TIxExternalCLK1Source_TI1ED : TI1 边沿检测器
  *          @arg TIM_TIxExternalCLK1Source_TI1   : 经过滤波的 Timer 输入 1
  *          @arg TIM_TIxExternalCLK1Source_TI2   : 经过滤波的 Timer 输入 2
  * @param  TIM_ICPolarity: 指定 TIx 的极性，可选值：
  *          @arg TIM_ICPolarity_Rising  : 上升沿触发
  *          @arg TIM_ICPolarity_Falling : 下降沿触发
  * @param  ICFilter: 输入滤波器值，取值范围 0x0~0xF。
  * @retval 无
  *
  * @note 应用示例：
  *       1. 外部脉冲计数：
  *          - 脉冲信号来自传感器或旋转编码器；
  *          - TIM 通过 TI1/TI2 捕捉脉冲，每收到一次脉冲计数器加 1；
  *          - 适合测量旋转速度、流量、脉冲数量。
  *
  *       2. 外部事件触发：
  *          - 比如按钮、开关或其他外部设备输出脉冲；
  *          - TIM 只在上升沿或下降沿触发计数，实现精确事件计数。
  *
  *       3. PWM 输入测量：
  *          - 输入一个外部 PWM 信号到 TI1/TI2；
  *          - TIM 配置为外部时钟模式，通过计数脉冲测量 PWM 周期或频率；
  *          - 可以实现频率/占空比测量。
  */
void myTIM_TIxExternalClockConfig(TIM_TypeDef* TIMx, uint16_t TIM_TIxExternalCLKSource,
    uint16_t TIM_ICPolarity, uint16_t ICFilter)
{
    /* 参数检查 */
    assert_param(IS_TIM_LIST1_PERIPH(TIMx));
    assert_param(IS_TIM_IC_POLARITY(TIM_ICPolarity));
    assert_param(IS_TIM_IC_FILTER(ICFilter));

    /* 配置 Timer 输入时钟源，使用三目运算符替代 if...else */
    (TIM_TIxExternalCLKSource == TIM_TIxExternalCLK1Source_TI2) ?
        TI2_Config(TIMx, TIM_ICPolarity, TIM_ICSelection_DirectTI, ICFilter) :
        TI1_Config(TIMx, TIM_ICPolarity, TIM_ICSelection_DirectTI, ICFilter);

    /* 选择触发源 */
    TIM_SelectInputTrigger(TIMx, TIM_TIxExternalCLKSource);

    /* 配置为外部时钟模式1 */
    TIMx->SMCR |= TIM_SlaveMode_External1;
}


/**
  * @brief  配置 TIMx 的外部时钟模式1（External Clock Mode1）
  * @param  TIMx: 指向 TIM 外设的指针，x 可以为 1,2,3,4,5 或 8。
  * @param  TIM_ExtTRGPrescaler: 外部触发器（ETR）预分频器，可选值：
  *          @arg TIM_ExtTRGPSC_OFF   : 不分频
  *          @arg TIM_ExtTRGPSC_DIV2  : 频率除以 2
  *          @arg TIM_ExtTRGPSC_DIV4  : 频率除以 4
  *          @arg TIM_ExtTRGPSC_DIV8  : 频率除以 8
  * @param  TIM_ExtTRGPolarity: 外部触发器极性，可选值：
  *          @arg TIM_ExtTRGPolarity_Inverted    : 低电平或下降沿有效
  *          @arg TIM_ExtTRGPolarity_NonInverted : 高电平或上升沿有效
  * @param  ExtTRGFilter: 外部触发器滤波器值，取值范围 0x0~0xF
  * @retval 无
  *
  * @note 功能说明：
  *       外部时钟模式1下，TIMx 计数器的时钟由外部触发信号（ETR）驱动，
  *       每收到一个有效脉冲计数器就加1，可用于精确外部事件计数。
  *
  * @note 应用示例：
  *       1. 旋转编码器计数：将旋转编码器的脉冲输入 ETR，引脚极性和滤波器根据实际信号设置。
  *       2. 外部传感器计数：例如流量计每输出一个脉冲，TIMx 计数器就加 1。
  *       3. 外部周期信号同步：计数器以外部信号频率为时钟，实现与外部设备同步。
  */
void TIM_ETRClockMode1Config(TIM_TypeDef* TIMx, uint16_t TIM_ExtTRGPrescaler,
    uint16_t TIM_ExtTRGPolarity, uint16_t ExtTRGFilter)
{
    uint16_t tmpsmcr = 0;

    /* 参数检查 */
    assert_param(IS_TIM_LIST3_PERIPH(TIMx));
    assert_param(IS_TIM_EXT_PRESCALER(TIM_ExtTRGPrescaler));
    assert_param(IS_TIM_EXT_POLARITY(TIM_ExtTRGPolarity));
    assert_param(IS_TIM_EXT_FILTER(ExtTRGFilter));

    /* 配置 ETR 时钟源 */
    TIM_ETRConfig(TIMx, TIM_ExtTRGPrescaler, TIM_ExtTRGPolarity, ExtTRGFilter);

    /* 读取 TIMx SMCR 寄存器 */
    tmpsmcr = TIMx->SMCR;

    /* 清除 SMS 位 */
    tmpsmcr &= (uint16_t)~TIM_SMCR_SMS;

    /* 选择外部时钟模式1 */
    tmpsmcr |= TIM_SlaveMode_External1;

    /* 选择触发源为 ETRF */
    tmpsmcr &= (uint16_t)~TIM_SMCR_TS;
    tmpsmcr |= TIM_TS_ETRF;

    /* 写回 SMCR 寄存器 */
    TIMx->SMCR = tmpsmcr;
}

/**
  * @brief  配置 TIMx 的外部时钟模式2（External Clock Mode2）
  * @param  TIMx: 指向 TIM 外设的指针，x 可以为 1,2,3,4,5 或 8。
  * @param  TIM_ExtTRGPrescaler: 外部触发器（ETR）预分频器，可选值：
  *          @arg TIM_ExtTRGPSC_OFF   : 不分频
  *          @arg TIM_ExtTRGPSC_DIV2  : 频率除以 2
  *          @arg TIM_ExtTRGPSC_DIV4  : 频率除以 4
  *          @arg TIM_ExtTRGPSC_DIV8  : 频率除以 8
  * @param  TIM_ExtTRGPolarity: 外部触发器极性，可选值：
  *          @arg TIM_ExtTRGPolarity_Inverted    : 低电平或下降沿有效
  *          @arg TIM_ExtTRGPolarity_NonInverted : 高电平或上升沿有效
  * @param  ExtTRGFilter: 外部触发器滤波器值，取值范围 0x0~0xF
  * @retval 无
  *
  * @note 功能说明：
  *       外部时钟模式2下，TIMx 计数器的计数由内部时钟或 ETR 信号驱动，
  *       同时 ETR 信号也可以作为触发输入（ECE 位使能），适用于外部信号同步和计数。
  *
  * @note 应用示例：
  *       1. 外部频率测量：将外部脉冲信号输入 ETR，通过计数器测量信号周期。
  *       2. 外部事件触发计数：每收到一个外部脉冲，计数器增加，实现事件统计。
  *       3. 多定时器同步：ETR 信号可用作其他定时器触发源，实现复杂同步控制。
  */
void TIM_ETRClockMode2Config(TIM_TypeDef* TIMx, uint16_t TIM_ExtTRGPrescaler,
    uint16_t TIM_ExtTRGPolarity, uint16_t ExtTRGFilter)
{
    /* 参数检查 */
    assert_param(IS_TIM_LIST3_PERIPH(TIMx));
    assert_param(IS_TIM_EXT_PRESCALER(TIM_ExtTRGPrescaler));
    assert_param(IS_TIM_EXT_POLARITY(TIM_ExtTRGPolarity));
    assert_param(IS_TIM_EXT_FILTER(ExtTRGFilter));

    /* 配置 ETR 时钟源 */
    TIM_ETRConfig(TIMx, TIM_ExtTRGPrescaler, TIM_ExtTRGPolarity, ExtTRGFilter);

    /* 使能外部时钟模式2 */
    TIMx->SMCR |= TIM_SMCR_ECE;
}


/**
  * @defgroup TIM_Group7 定时器同步管理函数
  * @brief    用来管理多个定时器同步工作或者跟外部触发同步
  *
@verbatim
 ===============================================================================
                ##### 定时器同步管理 #####
 ===============================================================================

          ##### TIM 驱动：同步模式怎么用 #####
 ===============================================================================
    [..]

    *** 多个定时器同步的情况 ***
    =========================
    [..]
      (#) 设置“主定时器”（Master）：
        (++) TIM_SelectOutputTrigger(TIMx, TIM_TRGOSource)
            - 选择主定时器输出给其他定时器的触发信号
        (++) TIM_SelectMasterSlaveMode(TIMx, TIM_MasterSlaveMode)
            - 配置主从模式，告诉主定时器它是“队长”
      (#) 设置“从定时器”（Slave）：
        (++) TIM_SelectInputTrigger(TIMx, TIM_InputTriggerSource)
            - 选择从定时器接收谁的触发信号
        (++) TIM_SelectSlaveMode(TIMx, TIM_SlaveMode)
            - 配置从定时器工作方式，比如：收到触发就计数

    *** 定时器跟外部信号同步的情况（ETR 引脚） ***
    ================================================
    [..]
      (#) 配置外部触发信号（ETR）：
        (++) TIM_ETRConfig(TIMx, TIM_ExtTRGPrescaler, TIM_ExtTRGPolarity, ExtTRGFilter)
            - 设置外部信号分频、上升/下降沿触发、滤波
      (#) 配置从定时器：
        (++) TIM_SelectInputTrigger(TIMx, TIM_InputTriggerSource)
            - 告诉从定时器用外部信号作为触发
        (++) TIM_SelectSlaveMode(TIMx, TIM_SlaveMode)
            - 配置从定时器收到触发信号后的动作
@endverbatim
  * @{
  */


  /**
    * @brief  选择定时器的输入触发源（Input Trigger）
    * @param  TIMx: 指向 TIM 外设的指针，x 可以为 1,2,3,4,5,8,9,10,11,12,13,14
    * @param  TIM_InputTriggerSource: 输入触发源，可选：
    *            TIM_TS_ITR0~TIM_TS_ITR3   : 内部触发器 0~3
    *            TIM_TS_TI1F_ED             : TI1 边沿检测器
    *            TIM_TS_TI1FP1              : 滤波后的 Timer 输入 1
    *            TIM_TS_TI2FP2              : 滤波后的 Timer 输入 2
    *            TIM_TS_ETRF                : 外部触发输入（ETR）
    * @retval 无
    *
    * @note 小白理解：就是告诉定时器，它的计数器什么时候开始走，
    *       可以跟另一个定时器走，也可以跟外部信号走。
    */
void myTIM_SelectInputTrigger(TIM_TypeDef* TIMx, uint16_t TIM_InputTriggerSource)
{
    uint16_t tmpsmcr = TIMx->SMCR;
    assert_param(IS_TIM_LIST1_PERIPH(TIMx));
    assert_param(IS_TIM_TRIGGER_SELECTION(TIM_InputTriggerSource));

    tmpsmcr &= (uint16_t)~TIM_SMCR_TS;           // 清除原来的触发源设置
    tmpsmcr |= TIM_InputTriggerSource;           // 设置新的触发源
    TIMx->SMCR = tmpsmcr;                        // 写回寄存器
}

/**
  * @brief  选择定时器的触发输出模式（Trigger Output，TRGO）
  * @param  TIMx: 指向 TIM 外设的指针，x 可以为 1~8
  * @param  TIM_TRGOSource: 输出触发源，可选：
  *           TIM_TRGOSource_Reset  : 更新事件（UG）作为触发输出
  *           TIM_TRGOSource_Enable : 计数器使能 CEN 作为触发输出
  *           TIM_TRGOSource_Update : 更新事件作为触发输出
  *           TIM_TRGOSource_OC1~OC4Ref : CCx 匹配或 OCxREF 信号作为触发输出
  * @retval 无
  *
  * @note 小白理解：就是主定时器输出一个触发信号给其他定时器，
  *       让从定时器或者其他外设按这个信号走。
  */
void myTIM_SelectOutputTrigger(TIM_TypeDef* TIMx, uint16_t TIM_TRGOSource)
{
    assert_param(IS_TIM_LIST5_PERIPH(TIMx));
    assert_param(IS_TIM_TRGO_SOURCE(TIM_TRGOSource));

    TIMx->CR2 &= (uint16_t)~TIM_CR2_MMS;   // 清除原来的 TRGO 配置
    TIMx->CR2 |= TIM_TRGOSource;            // 设置新的 TRGO 源
}

/**
  * @brief  设置定时器的从模式（Slave Mode）
  * @param  TIMx: 指向 TIM 外设的指针，x 可以为 1,2,3,4,5,8,9,12
  * @param  TIM_SlaveMode: 从模式，可选：
  *           TIM_SlaveMode_Reset      : 触发信号上升沿复位计数器并更新寄存器
  *           TIM_SlaveMode_Gated      : 计数器只有在触发信号为高时才计数
  *           TIM_SlaveMode_Trigger    : 触发信号上升沿开始计数
  *           TIM_SlaveMode_External1  : 触发信号上升沿直接驱动计数器
  * @retval 无
  *
  * @note 小白理解：就是告诉从定时器怎么“听”主定时器或者外部信号，
  *       收到信号后计数或者复位。
  */
void myTIM_SelectSlaveMode(TIM_TypeDef* TIMx, uint16_t TIM_SlaveMode)
{
    assert_param(IS_TIM_LIST2_PERIPH(TIMx));
    assert_param(IS_TIM_SLAVE_MODE(TIM_SlaveMode));

    TIMx->SMCR &= (uint16_t)~TIM_SMCR_SMS;  // 清除原来的从模式
    TIMx->SMCR |= TIM_SlaveMode;            // 设置新的从模式
}

/**
  * @brief  设置或清除定时器的主从模式（Master/Slave Mode）
  * @param  TIMx: 指向 TIM 外设的指针，x 可以为 1,2,3,4,5,8,9,12
  * @param  TIM_MasterSlaveMode: 主从模式，可选：
  *           TIM_MasterSlaveMode_Enable  : 启用当前定时器与从定时器同步（通过 TRGO）
  *           TIM_MasterSlaveMode_Disable : 关闭同步
  * @retval 无
  *
  * @note 小白理解：就是告诉主定时器，它是不是队长，如果是，就输出触发信号给从定时器。
  */
void myTIM_SelectMasterSlaveMode(TIM_TypeDef* TIMx, uint16_t TIM_MasterSlaveMode)
{
    assert_param(IS_TIM_LIST2_PERIPH(TIMx));
    assert_param(IS_TIM_MSM_STATE(TIM_MasterSlaveMode));

    TIMx->SMCR &= (uint16_t)~TIM_SMCR_MSM;      // 清除原来的 MSM 设置
    TIMx->SMCR |= TIM_MasterSlaveMode;          // 设置新的 MSM
}

/**
  * @brief  配置定时器的外部触发器（ETR）
  * @param  TIMx: 指向 TIM 外设的指针，x 可以为 1,2,3,4,5,8
  * @param  TIM_ExtTRGPrescaler: 外部触发预分频，可选：
  *           TIM_ExtTRGPSC_OFF  : 不分频
  *           TIM_ExtTRGPSC_DIV2 : 频率 ÷2
  *           TIM_ExtTRGPSC_DIV4 : 频率 ÷4
  *           TIM_ExtTRGPSC_DIV8 : 频率 ÷8
  * @param  TIM_ExtTRGPolarity: 外部触发极性，可选：
  *           TIM_ExtTRGPolarity_Inverted   : 下降沿有效
  *           TIM_ExtTRGPolarity_NonInverted : 上升沿有效
  * @param  ExtTRGFilter: 外部触发滤波，取值 0x0~0xF
  * @retval 无
  *
  * @note 小白理解：就是定时器用外部脉冲作为计数信号，
  *       可以设置脉冲的分频、上升/下降沿触发、滤波抗干扰。
  */
void myTIM_ETRConfig(TIM_TypeDef* TIMx, uint16_t TIM_ExtTRGPrescaler,
    uint16_t TIM_ExtTRGPolarity, uint16_t ExtTRGFilter)
{
    uint16_t tmpsmcr = TIMx->SMCR;

    assert_param(IS_TIM_LIST3_PERIPH(TIMx));
    assert_param(IS_TIM_EXT_PRESCALER(TIM_ExtTRGPrescaler));
    assert_param(IS_TIM_EXT_POLARITY(TIM_ExtTRGPolarity));
    assert_param(IS_TIM_EXT_FILTER(ExtTRGFilter));

    tmpsmcr &= SMCR_ETR_MASK;   // 清除原来的 ETR 配置
    tmpsmcr |= (uint16_t)(TIM_ExtTRGPrescaler | TIM_ExtTRGPolarity | (ExtTRGFilter << 8));
    TIMx->SMCR = tmpsmcr;       // 写回寄存器
}


/**
 * @defgroup TIM_Group8 特定接口管理功能
 * @brief    针对定时器的特殊接口进行管理的功能
 *
@verbatim
 ===============================================================================
            ##### 特定接口管理功能 #####
 ===============================================================================

@endverbatim
  * @{
 */
 /**
   * @brief  配置 TIMx 的编码器接口
   * @param  TIMx: 指向 TIM 外设的指针，x 可以为 1, 2, 3, 4, 5, 8, 9 或 12。
   * @param  TIM_EncoderMode: 编码器模式，定时器根据编码器输入计数。
   *          可选值：
   *            @arg TIM_EncoderMode_TI1  : 只在 TI1 输入的边沿计数，方向由 TI2 电平决定
   *            @arg TIM_EncoderMode_TI2  : 只在 TI2 输入的边沿计数，方向由 TI1 电平决定
   *            @arg TIM_EncoderMode_TI12 : 在 TI1 和 TI2 输入的边沿都计数，方向由另一个输入电平决定
   * @param  TIM_IC1Polarity: TI1 输入捕获极性
   *          可选值：
   *            @arg TIM_ICPolarity_Rising  : 上升沿计数
   *            @arg TIM_ICPolarity_Falling : 下降沿计数
   * @param  TIM_IC2Polarity: TI2 输入捕获极性
   *          可选值：
   *            @arg TIM_ICPolarity_Rising  : 上升沿计数
   *            @arg TIM_ICPolarity_Falling : 下降沿计数
   * @retval 无
   *
   * @note  此函数主要用于连接机械编码器（旋转编码器）到定时器，实现计数器随编码器旋转变化。
   *        - 编码器有两个信号线 A 和 B，对应 TIM 的 TI1 和 TI2。
   *        - 根据 TIM_EncoderMode 设置计数方式：
   *            TI1 或 TI2 单边沿计数，或两路都计数。
   *        - TI1Polarity 和 TI2Polarity 决定上升沿还是下降沿触发计数。
   *
   * 小白理解：
   * 1. 假设你有一个旋转编码器，当你转它时，定时器的计数器会随转动增加或减少。
   * 2. TIM_EncoderMode 决定计数器是只看一个信号还是两个信号都看。
   * 3. TIM_ICxPolarity 决定信号的哪条边沿（上升沿/下降沿）算一次计数。
   */
void TIM_EncoderInterfaceConfig(TIM_TypeDef* TIMx, uint16_t TIM_EncoderMode,
    uint16_t TIM_IC1Polarity, uint16_t TIM_IC2Polarity)
{
    uint16_t tmpsmcr = 0;
    uint16_t tmpccmr1 = 0;
    uint16_t tmpccer = 0;

    /* 参数检查 */
    assert_param(IS_TIM_LIST2_PERIPH(TIMx));
    assert_param(IS_TIM_ENCODER_MODE(TIM_EncoderMode));
    assert_param(IS_TIM_IC_POLARITY(TIM_IC1Polarity));
    assert_param(IS_TIM_IC_POLARITY(TIM_IC2Polarity));

    /* 读取寄存器当前值 */
    tmpsmcr = TIMx->SMCR;
    tmpccmr1 = TIMx->CCMR1;
    tmpccer = TIMx->CCER;

    /* 设置编码器模式 */
    tmpsmcr &= (uint16_t)~TIM_SMCR_SMS;  // 清除旧模式
    tmpsmcr |= TIM_EncoderMode;           // 设置新模式

    /* 将捕获比较1和2通道设置为输入 */
    tmpccmr1 &= ((uint16_t)~TIM_CCMR1_CC1S) & ((uint16_t)~TIM_CCMR1_CC2S);
    tmpccmr1 |= TIM_CCMR1_CC1S_0 | TIM_CCMR1_CC2S_0;

    /* 设置 TI1 和 TI2 极性 */
    tmpccer &= ((uint16_t)~TIM_CCER_CC1P) & ((uint16_t)~TIM_CCER_CC2P);
    tmpccer |= (uint16_t)(TIM_IC1Polarity | (uint16_t)(TIM_IC2Polarity << 4));

    /* 写回寄存器 */
    TIMx->SMCR = tmpsmcr;
    TIMx->CCMR1 = tmpccmr1;
    TIMx->CCER = tmpccer;
}


/**
  * @brief  使能或禁用 TIMx 的霍尔传感器接口
  * @param  TIMx: 指向 TIM 外设的指针，x 可以为 1, 2, 3, 4, 5, 8, 9 或 12。
  * @param  NewState: 霍尔传感器接口的新状态
  *          可选值：
  *            @arg ENABLE  : 使能霍尔传感器接口
  *            @arg DISABLE : 禁用霍尔传感器接口
  * @retval 无
  *
  * @note  霍尔传感器常用于无刷直流电机(BLDC)或旋转位置检测。
  *        - TIMx 可以直接读取霍尔传感器的三个信号线（U,V,W）来产生计数信号。
  *        - 使能后，定时器会自动把霍尔信号转换为计数脉冲。
  *
  * 小白理解：
  * 1. 如果你有一个电机，想知道它转了多少圈或者转速，可以用霍尔传感器。
  * 2. 这个函数就是告诉定时器：“去听霍尔传感器的信号，把转动换成计数器的数字”。
  * 3. ENABLE 就让定时器开始听，DISABLE 就不听了。
  */
void myTIM_SelectHallSensor(TIM_TypeDef* TIMx, FunctionalState NewState)
{
    /* 参数检查 */
    assert_param(IS_TIM_LIST2_PERIPH(TIMx));
    assert_param(IS_FUNCTIONAL_STATE(NewState));

    /* 使用三目运算符设置或清除 TI1S 位 */
    NewState != DISABLE ? (TIMx->CR2 |= TIM_CR2_TI1S) : (TIMx->CR2 &= (uint16_t)~TIM_CR2_TI1S);
}


/** @defgroup TIM_Group9 特定重映射管理函数
 *  @brief   用于管理定时器的特定引脚功能重映射
 *
@verbatim
 ===============================================================================
              ##### 特定重映射管理函数 #####
 ===============================================================================

@endverbatim
  * @{
  */


  /**
    * @brief  配置 TIM2, TIM5 和 TIM11 的输入重映射功能。
    * @param  TIMx: 指向 TIM 外设的指针，可以是 TIM2、TIM5 或 TIM11。
    * @param  TIM_Remap: 指定 TIM 输入的重映射来源。
    *          可选值：
    *            @arg TIM2_TIM8_TRGO: TIM2 ITR1 输入连接到 TIM8 的触发输出（默认）
    *            @arg TIM2_ETH_PTP:   TIM2 ITR1 输入连接到以太网 PTP 触发输出
    *            @arg TIM2_USBFS_SOF: TIM2 ITR1 输入连接到 USB FS SOF 信号
    *            @arg TIM2_USBHS_SOF: TIM2 ITR1 输入连接到 USB HS SOF 信号
    *            @arg TIM5_GPIO:      TIM5 CH4 输入连接到专用 Timer 引脚（默认）
    *            @arg TIM5_LSI:       TIM5 CH4 输入连接到 LSI 时钟
    *            @arg TIM5_LSE:       TIM5 CH4 输入连接到 LSE 时钟
    *            @arg TIM5_RTC:       TIM5 CH4 输入连接到 RTC 输出事件
    *            @arg TIM11_GPIO:     TIM11 CH4 输入连接到专用 Timer 引脚（默认）
    *            @arg TIM11_HSE:      TIM11 CH4 输入连接到 HSE_RTC 时钟（可分频）
    * @retval 无
    * @note   这个函数用于将定时器的特定输入信号映射到不同的硬件源。
    */
void myTIM_RemapConfig(TIM_TypeDef* TIMx, uint16_t TIM_Remap)
{
    assert_param(IS_TIM_LIST6_PERIPH(TIMx));
    assert_param(IS_TIM_REMAP(TIM_Remap));

    TIMx->OR = TIM_Remap;
}

/**
  * @brief  配置 TIM 定时器某个通道（TI1~TI4）为输入捕获模式
  * @param  TIMx: 定时器指针，如 TIM1, TIM2 等
  * @param  channel: 通道号，1~4
  * @param  TIM_ICPolarity: 输入捕获触发边沿
  *          @arg TIM_ICPolarity_Rising   上升沿触发
  *          @arg TIM_ICPolarity_Falling 下降沿触发
  *          @arg TIM_ICPolarity_BothEdge 上升沿或下降沿都触发
  * @param  TIM_ICSelection: 输入信号选择
  *          @arg TIM_ICSelection_DirectTI   直接用对应通道的输入引脚
  *          @arg TIM_ICSelection_IndirectTI 用另一个通道的输入信号
  *          @arg TIM_ICSelection_TRC        用定时器内部触发信号 TRC
  * @param  TIM_ICFilter: 输入滤波器，0~0xF，值越大滤波越强，可去掉抖动
  * @retval 无
  *
  * @note 这个函数用途：
  *       - 用于把定时器的某个通道配置成“捕获外部信号脉冲”模式
  *       - 适合做：
  *           - 测量方波频率
  *           - 测量脉宽
  *           - 读取编码器的脉冲信号
  *
  * @example
  *     // 例子：用 TIM2 通道1 捕获上升沿信号，直接用引脚输入，滤波值 0
  *     myTIx_Config(TIM2, 1, TIM_ICPolarity_Rising, TIM_ICSelection_DirectTI, 0);
  *
  *     // 例子：用 TIM3 通道2 捕获下降沿信号，使用 TRC 作为输入，滤波值 3
  *     myTIx_Config(TIM3, 2, TIM_ICPolarity_Falling, TIM_ICSelection_TRC, 3);
  */
static void myTIx_Config(TIM_TypeDef* TIMx, uint8_t channel,
    uint16_t TIM_ICPolarity, uint16_t TIM_ICSelection,
    uint16_t TIM_ICFilter)
{
    uint16_t tmpccmr = 0, tmpccer = 0; // 临时保存寄存器值
    uint16_t shift = 0; // 用于计算位偏移
    uint16_t tmpPol = 0; // 保存极性位

    // 参数检查，保证输入合法
    assert_param(channel >= 1 && channel <= 4);
    assert_param(IS_TIM_IC_POLARITY(TIM_ICPolarity));
    assert_param(IS_TIM_IC_SELECTION(TIM_ICSelection));
    assert_param(IS_TIM_IC_FILTER(TIM_ICFilter));

    tmpccer = TIMx->CCER; // 保存 CCER 寄存器值

    // 通道1或2，使用 CCMR1 寄存器
    if (channel <= 2)
    {
        tmpccmr = TIMx->CCMR1;
        shift = (channel == 1) ? 0 : 8; // 通道1用低8位，通道2用高8位
        tmpPol = (channel == 1) ? TIM_ICPolarity : (TIM_ICPolarity << 4);

        // 先关闭通道，避免配置时触发错误
        TIMx->CCER &= (channel == 1) ? ~TIM_CCER_CC1E : ~TIM_CCER_CC2E;

        // 清除旧的输入选择和滤波器位
        tmpccmr &= ~((channel == 1 ? TIM_CCMR1_CC1S : TIM_CCMR1_CC2S) |
            (channel == 1 ? TIM_CCMR1_IC1F : TIM_CCMR1_IC2F));

        // 设置新的输入选择和滤波器
        tmpccmr |= TIM_ICSelection << shift | (TIM_ICFilter << (shift + 4));

        // 设置极性并重新使能通道
        tmpccer &= ~((channel == 1 ? (TIM_CCER_CC1P | TIM_CCER_CC1NP) :
            (TIM_CCER_CC2P | TIM_CCER_CC2NP)));
        tmpccer |= tmpPol | (channel == 1 ? TIM_CCER_CC1E : TIM_CCER_CC2E);

        // 写回寄存器
        TIMx->CCMR1 = tmpccmr;
        TIMx->CCER = tmpccer;
    }
    else // 通道3或4，使用 CCMR2 寄存器
    {
        tmpccmr = TIMx->CCMR2;
        shift = (channel == 3) ? 0 : 8; // 通道3低8位，通道4高8位
        tmpPol = (channel == 3) ? (TIM_ICPolarity << 8) : (TIM_ICPolarity << 12);

        // 关闭通道
        TIMx->CCER &= (channel == 3) ? ~TIM_CCER_CC3E : ~TIM_CCER_CC4E;

        // 清除旧的输入选择和滤波器位
        tmpccmr &= ~((channel == 3 ? TIM_CCMR2_CC3S : TIM_CCMR2_CC4S) |
            (channel == 3 ? TIM_CCMR2_IC3F : TIM_CCMR2_IC4F));

        // 设置新的输入选择和滤波器
        tmpccmr |= TIM_ICSelection << shift | (TIM_ICFilter << (shift + 4));

        // 设置极性并使能通道
        tmpccer &= ~((channel == 3 ? (TIM_CCER_CC3P | TIM_CCER_CC3NP) :
            (TIM_CCER_CC4P | TIM_CCER_CC4NP)));
        tmpccer |= tmpPol | (channel == 3 ? TIM_CCER_CC3E : TIM_CCER_CC4E);

        // 写回寄存器
        TIMx->CCMR2 = tmpccmr;
        TIMx->CCER = tmpccer;
    }
}
