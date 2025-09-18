/**
  ******************************************************************************
  * @file     stm32f4xx_exti_rewrite.c
  * @author   QiangGu
  * @version  V1.0.0
  * @date     2025-09-16
  * @brief    STM32F4系列EXTI外设驱动库（重写版）
  *
  * @attention
  *
  * 本文件为 STM32F4xx 标准外设库 EXTI 模块的自定义重写版本，编写过程中旨在加深对
  * STM32 编程和外部中断工作机制的研究理解。
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
  * 2. 在使用任何 EXTI 函数前，必须首先调用 RCC_APB2PeriphClockCmd() 使能 SYSCFG 时钟，
  *    并通过 SYSCFG_EXTILineConfig() 将 GPIO 引脚映射到 EXTI 线路
  * 3. 使用 EXTI_Init() 函数配置 EXTI 线路的触发模式（上升沿、下降沿、双边沿）
  * 4. 在 NVIC 中配置相应中断优先级，并编写对应的中断服务函数
  *
  * 本库提供的功能：
  * - EXTI 初始化与配置
  * - EXTI 线路使能/禁用
  * - 中断挂起标志位的读取与清除
  * - 支持完整的触发模式与线路选择
  *
  * 版本历史：
  * V1.0.0 初始版本
  *   - 实现基本的 EXTI 初始化、挂起标志清除功能
  *   - 提供完整的触发模式配置支持
  *   - 添加详细的错误检查和调试支持
  *
  * 免责声明：
  * 本软件按“原样”提供，不提供任何明示或暗示的担保，包括但不限于对适销性或特定用途适用性的保证。
  * 作者不对因使用本软件而产生的任何直接、间接、偶然、特殊、示例性或后果性损失承担责任。
  * 本软件仅供学习和研究使用，无偿共享，禁止用于商业用途。
  *
  ******************************************************************************
  */


  /**  ****************************官方库翻译区域**************************************************

  @verbatim

   ===============================================================================
                                ##### EXTI 特性 #####
   ===============================================================================

   [..] 外部中断/事件线的映射关系如下：
     (#) 所有可用的 GPIO 引脚都连接到 EXTI0 ~ EXTI15 共16条外部中断/事件线上。
     (#) EXTI 线16 连接到 PVD（电源电压检测器）输出。
     (#) EXTI 线17 连接到 RTC 闹钟事件。
     (#) EXTI 线18 连接到 USB OTG FS 从挂起唤醒事件。
     (#) EXTI 线19 连接到以太网唤醒事件。
     (#) EXTI 线20 连接到 USB OTG HS（配置为 FS 模式）唤醒事件。
     (#) EXTI 线21 连接到 RTC 篡改和时间戳事件。
     (#) EXTI 线22 连接到 RTC 唤醒事件。

                         ##### 如何使用该驱动 #####
   ===============================================================================

   [..] 若要将某个 I/O 引脚作为外部中断源使用，需按以下步骤操作：
     (1) 使用 GPIO_Init() 将该 I/O 引脚配置为输入模式。
     (2) 使用 SYSCFG_EXTILineConfig() 选择该 EXTI 线对应的输入源引脚。
     (3) 使用 EXTI_Init() 选择模式（中断或事件），并配置触发方式
         （上升沿、下降沿或双边沿）。
     (4) 使用 NVIC_Init() 配置映射到该 EXTI 线的 NVIC 中断通道。

   [..]
     (@) 注意：必须使能 SYSCFG 的 APB 时钟，才能写访问 SYSCFG_EXTICRx
         寄存器。这可通过如下函数实现：
         RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

  @endverbatim
    *
    ******************************************************************************
  

  /****************************官方库翻译区域**************************************************/


#include "mystm32f4_exti.h"
  /** @addtogroup STM32F4xx_StdPeriph_Driver
    * @{
    */

    /** @defgroup EXTI
      * @brief EXTI 驱动模块
      * @{
      */

      /* 私有类型定义 -----------------------------------------------------------*/
      /* 私有宏定义 -------------------------------------------------------------*/

#define EXTI_LINENONE    ((uint32_t)0x00000)  /* 未选择任何中断 */

/* 私有宏 -------------------------------------------------------------*/
/* 私有变量 -----------------------------------------------------------*/
/* 私有函数原型 -------------------------------------------------------*/
/* 私有函数 -----------------------------------------------------------*/

/** @defgroup EXTI_Private_Functions
  * @{
  */

  /** @defgroup EXTI_Group1 初始化与配置函数
   *  @brief   初始化与配置函数
   *
  @verbatim
   ===============================================================================
                        ##### 初始化与配置函数 #####
   ===============================================================================
   [..] 本节提供以下函数：
     (+) EXTI_DeInit
     (+) EXTI_Init
     (+) EXTI_StructInit
	 (+) EXTI_GenerateSWInterrupt

  /**
  * @brief  重置 EXTI 外设寄存器到默认状态
  * @note   本函数将 EXTI 的所有配置恢复到复位状态，常用于初始化或
  *         在程序运行中需要清除所有中断配置时调用。
  *
  *         EXTI 是 STM32 的外部中断/事件控制器，用于管理
  *         外部 GPIO 引脚或特定事件的中断触发。
  *
  *         主要寄存器及作用：
  *         1. IMR  - 中断屏蔽寄存器：决定哪些线路允许触发中断
  *         2. EMR  - 事件屏蔽寄存器：决定哪些线路允许触发事件
  *         3. RTSR - 上升沿触发寄存器：设置线路上升沿触发中断
  *         4. FTSR - 下降沿触发寄存器：设置线路下降沿触发中断
  *         5. PR   - 挂起寄存器：标记中断是否发生，写1清除对应标志
  *
  * @param  None
  * @retval None
  *
  * @example
  *    // 在程序开始时重置 EXTI 外设
  *    EXTI_DeInit();
  */
void myEXTI_DeInit(void)
{
    // 禁用所有中断线（IMR = 0）
    // 清零后，任何外部事件都不会触发中断
    EXTI->IMR = 0x00000000;

    // 禁用所有事件线（EMR = 0）
    // 清零后，任何外部事件都不会触发事件（非中断动作）
    EXTI->EMR = 0x00000000;

    // 关闭所有上升沿触发（RTSR = 0）
    // 清零后线路上升沿不会触发中断
    EXTI->RTSR = 0x00000000;

    // 关闭所有下降沿触发（FTSR = 0）
    // 清零后线路下降沿不会触发中断
    EXTI->FTSR = 0x00000000;

    // 清除所有挂起标志（PR）
    // 写1清除，确保之前触发的中断标志不残留
    EXTI->PR = 0x007FFFFF; // EXTI0~EXTI22 共23条线路
}



/**
  * @brief  根据 EXTI_InitStruct 指定的参数初始化 EXTI 外设
  * @param  EXTI_InitStruct: 指向 EXTI_InitTypeDef 结构体的指针
  *         结构体内容：
  *         - EXTI_Line    : EXTI线路（EXTI_Line0~EXTI_Line22）
  *         - EXTI_Mode    : 线路模式（中断或事件）
  *         - EXTI_Trigger : 触发方式（上升沿、下降沿、双边沿）
  *         - EXTI_LineCmd : 使能或禁止该线路
  * @retval None
  *
  * @note   初始化步骤：
  *         1. 检查参数合法性
  *         2. 如果禁用线路，直接清除模式寄存器对应位后返回
  *         3. 否则清除原有线路配置（IMR/EMR/RTSR/FTSR）
  *         4. 配置模式（中断/事件）
  *         5. 根据触发方式配置上升沿、下降沿或双边沿
  *         6. 完成线路初始化
  *
  * @example
  *    EXTI_InitTypeDef EXTI_InitStruct;
  *    EXTI_InitStruct.EXTI_Line = EXTI_Line0;
  *    EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt;
  *    EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Rising;
  *    EXTI_InitStruct.EXTI_LineCmd = ENABLE;
  *    EXTI_Init(&EXTI_InitStruct);
  */
void myEXTI_Init(EXTI_InitTypeDef* EXTI_InitStruct)
{
    uint32_t tmp = 0;

    /* 参数检查 */
    assert_param(IS_EXTI_MODE(EXTI_InitStruct->EXTI_Mode));
    assert_param(IS_EXTI_TRIGGER(EXTI_InitStruct->EXTI_Trigger));
    assert_param(IS_EXTI_LINE(EXTI_InitStruct->EXTI_Line));
    assert_param(IS_FUNCTIONAL_STATE(EXTI_InitStruct->EXTI_LineCmd));

    /* EXTI 基地址 */
    tmp = (uint32_t)EXTI_BASE;

    /* 如果线路被禁止，直接清除对应模式位并返回 */
    if (EXTI_InitStruct->EXTI_LineCmd == DISABLE)
    {
        tmp += EXTI_InitStruct->EXTI_Mode;
        *(__IO uint32_t*) tmp &= ~EXTI_InitStruct->EXTI_Line;
        return;
    }

    /* 清除旧配置 */
    EXTI->IMR &= ~EXTI_InitStruct->EXTI_Line;  // 中断屏蔽清零
    EXTI->EMR &= ~EXTI_InitStruct->EXTI_Line;  // 事件屏蔽清零
    EXTI->RTSR &= ~EXTI_InitStruct->EXTI_Line; // 上升沿触发清零
    EXTI->FTSR &= ~EXTI_InitStruct->EXTI_Line; // 下降沿触发清零

    /* 配置模式（中断或事件） */
    tmp = EXTI_BASE + EXTI_InitStruct->EXTI_Mode;
    *(__IO uint32_t*) tmp |= EXTI_InitStruct->EXTI_Line;

    /* 配置触发方式 */
    switch (EXTI_InitStruct->EXTI_Trigger)
    {
    case EXTI_Trigger_Rising:
        EXTI->RTSR |= EXTI_InitStruct->EXTI_Line; // 上升沿触发
        break;

    case EXTI_Trigger_Falling:
        EXTI->FTSR |= EXTI_InitStruct->EXTI_Line; // 下降沿触发
        break;

    case EXTI_Trigger_Rising_Falling:
        /* 双边沿触发：上升沿和下降沿同时设置 */
        EXTI->RTSR |= EXTI_InitStruct->EXTI_Line;
        EXTI->FTSR |= EXTI_InitStruct->EXTI_Line;
        break;

    default:
        /* 如果传入非法值，使用三目运算符默认设置为上升沿触发 */
        EXTI->RTSR |= EXTI_InitStruct->EXTI_Line;
        break;
    }

    /* 也可以将单边沿设置用三目运算符替代switch：
       EXTI_InitStruct->EXTI_Trigger == EXTI_Trigger_Rising_Falling ?
      (EXTI->RTSR |= EXTI_InitStruct->EXTI_Line, EXTI->FTSR |= EXTI_InitStruct->EXTI_Line) :
      (EXTI->RTSR |= (EXTI_InitStruct->EXTI_Trigger == EXTI_Trigger_Rising ? EXTI_InitStruct->EXTI_Line : 0),
      EXTI->FTSR |= (EXTI_InitStruct->EXTI_Trigger == EXTI_Trigger_Falling ? EXTI_InitStruct->EXTI_Line : 0));
    */
}


/**
  * @brief  将 EXTI_InitTypeDef 结构体的每个成员设置为默认复位值
  * @param  EXTI_InitStruct: 指向 EXTI_InitTypeDef 结构体的指针
  *         将要被初始化的结构体
  * @retval None
  *
  * @note   本函数的作用是将 EXTI 初始化结构体恢复到默认状态，方便用户在
  *         配置 EXTI 外设前先调用此函数，避免结构体成员中残留的垃圾值
  *         影响后续初始化。
  *
  *         默认复位值说明：
  *         - EXTI_Line    : EXTI_LINENONE（不选择任何线路）
  *         - EXTI_Mode    : EXTI_Mode_Interrupt（中断模式）
  *         - EXTI_Trigger : EXTI_Trigger_Falling（下降沿触发）
  *         - EXTI_LineCmd : DISABLE（禁止该线路）
  *
  * @example
  *    EXTI_InitTypeDef EXTI_InitStruct;
  *    EXTI_StructInit(&EXTI_InitStruct); // 先初始化为默认值
  *    EXTI_InitStruct.EXTI_Line = EXTI_Line0;
  *    EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt;
  *    EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Rising;
  *    EXTI_InitStruct.EXTI_LineCmd = ENABLE;
  *    EXTI_Init(&EXTI_InitStruct);
  */
void myEXTI_StructInit(EXTI_InitTypeDef* EXTI_InitStruct)
{
    /* 将结构体成员设置为默认复位值 */
    EXTI_InitStruct->EXTI_Line = EXTI_LINENONE,
    EXTI_InitStruct->EXTI_Mode = EXTI_Mode_Interrupt,
    EXTI_InitStruct->EXTI_Trigger = EXTI_Trigger_Falling,
   EXTI_InitStruct->EXTI_LineCmd = DISABLE;

}


/**
  * @brief  在指定的 EXTI 线路上产生一次软件触发中断
  * @param  EXTI_Line: 指定要产生软件中断的 EXTI 线路。
  *         该参数可以是任意 EXTI_Linex 的组合，其中 x = 0~22。
  *         例如：EXTI_Line0 | EXTI_Line3 表示同时触发 EXTI0 和 EXTI3。
  * @retval None
  *
  * @note   本函数用于模拟外部中断的触发，而不依赖实际的外部信号。
  *         常用于测试中断处理函数是否正常工作，或在软件中主动触发中断。
  *         调用该函数执行后会立刻产生中断请求，
  *
  * @example
  *    // 触发 EXTI0 软件中断
  *    EXTI_GenerateSWInterrupt(EXTI_Line0);
  *
  *    // 同时触发 EXTI0 和 EXTI3
  *    EXTI_GenerateSWInterrupt(EXTI_Line0 | EXTI_Line3);
  */
void myEXTI_GenerateSWInterrupt(uint32_t EXTI_Line)
{
    /* 参数检查，确保线路合法 */
    assert_param(IS_EXTI_LINE(EXTI_Line));

    /* 通过设置 SWIER 寄存器对应位，产生软件触发中断 */
    EXTI->SWIER |= EXTI_Line;
}

/** @defgroup EXTI_Group2 中断和标志管理函数
 *  @brief   中断和标志管理相关函数
 *
@verbatim
 ===============================================================================
             ##### 中断和标志管理函数 #####
 ===============================================================================
 [..] 本节提供以下函数：
   (+) EXTI_GetFlagStatus
   (+) EXTI_ClearFlag
   (+) EXTI_GetITStatus
   (+) EXTI_ClearITPendingBit


/**
  * @brief  检查指定的 EXTI 线路标志位是否被置位
  * @param  EXTI_Line: 指定要检查的 EXTI 线路标志位
  *         该参数可以是 EXTI_Linex，其中 x = 0~22
  * @retval FlagStatus: 返回指定线路标志的状态
  *         - SET   : 标志位已被置位（该中断事件已发生）
  *         - RESET : 标志位未被置位
  *
  * @note   本函数用于判断指定的 EXTI 线路是否已经产生中断事件。
  *         EXTI 标志位在对应中断发生后会被置位，可以通过本函数检测。
  *         检测完成后，若需要再次响应同一事件，需要清除标志位。
  *
  * @example
  *    if (EXTI_GetFlagStatus(EXTI_Line0) == SET)
  *    {
  *        // 执行对应处理逻辑
  *        EXTI->PR = EXTI_Line0; // 清除标志位
  *    }
  */
FlagStatus myEXTI_GetFlagStatus(uint32_t EXTI_Line)
{
    /* 参数检查，确保传入的线路合法 */
    assert_param(IS_GET_EXTI_LINE(EXTI_Line));

    /* 使用三目运算符判断标志位是否被置位 */
    return ((EXTI->PR & EXTI_Line) != (uint32_t)RESET) ? SET : RESET;
}



/**
  * @brief  清除指定 EXTI 线路的挂起标志位（Pending Flag）
  * @param  EXTI_Line: 指定要清除的 EXTI 线路标志位
  *         该参数可以是任意 EXTI_Linex 的组合，其中 x = 0~22
  *         例如：EXTI_Line0 | EXTI_Line3 表示同时清除 EXTI0 和 EXTI3 的标志位
  * @retval None
  *
  * @note   EXTI 的挂起标志位在中断发生后会被置位，表示该中断事件已触发。
  *         如果想再次响应同一条线路的中断，必须先清除其挂起标志。
  *         本函数通过向 EXTI->PR 寄存器写 1 来清除对应线路的挂起标志。
  *
  * @example
  *    // 清除 EXTI0 的挂起标志
  *    EXTI_ClearFlag(EXTI_Line0);
  *
  *    // 同时清除 EXTI0 和 EXTI3 的挂起标志
  *    EXTI_ClearFlag(EXTI_Line0 | EXTI_Line3);
  */
void myXTI_ClearFlag(uint32_t EXTI_Line)
{
    /* 参数检查，确保传入的线路合法 */
    assert_param(IS_EXTI_LINE(EXTI_Line));

    /* 写 1 清除指定线路的挂起标志 */
    EXTI->PR = EXTI_Line;
}


/**
  * @brief  检查指定 EXTI 线路的中断是否已触发（挂起）
  * @param  EXTI_Line: 指定要检查的 EXTI 线路
  *         该参数可以是 EXTI_Linex，其中 x = 0~22
  * @retval ITStatus: 返回指定线路中断状态
  *         - SET   : 中断已触发（挂起标志被置位）
  *         - RESET : 中断未触发
  *
  * @note   本函数用于判断指定的 EXTI 线路是否已经产生中断请求。
  *         EXTI 的中断挂起标志在中断发生后会被置位，可以通过本函数检测。
  *         若需要再次响应同一中断，需要先清除挂起标志（使用 EXTI_ClearFlag）。
  *
  * @example
  *    if (EXTI_GetITStatus(EXTI_Line0) == SET)
  *    {
  *        // 中断已触发，执行对应处理逻辑
  *        EXTI_ClearFlag(EXTI_Line0); // 清除中断标志
  *    }
  */
ITStatus myEXTI_GetITStatus(uint32_t EXTI_Line)
{
    /* 参数检查，确保传入的线路合法 */
    assert_param(IS_GET_EXTI_LINE(EXTI_Line));

    /* 使用三目运算符判断中断挂起状态 */
    return ((EXTI->PR & EXTI_Line) != (uint32_t)RESET) ? SET : RESET;
}

/**
  * @brief  清除指定 EXTI 线路的中断挂起位（Pending Bit）
  * @param  EXTI_Line: 指定要清除的 EXTI 线路
  *         该参数可以是任意 EXTI_Linex 的组合，其中 x = 0~22
  *         例如：EXTI_Line0 | EXTI_Line3 表示同时清除 EXTI0 和 EXTI3 的挂起位
  * @retval None
  *
  * @note
  * 1. EXTI 中断发生时，对应的挂起位（PR 寄存器）会被置位，表示该中断事件已经触发。
  * 2. 若希望同一条线路的中断可以再次响应，必须先清除其挂起位。
  * 3. 本函数通过向 EXTI->PR 寄存器写入 1 来清除指定线路的挂起位。
  * 4. 与 EXTI_ClearFlag() 功能类似，作用都是清除挂起标志位。
  *
  * @example
  *    // 清除 EXTI0 的挂起位
  *    EXTI_ClearITPendingBit(EXTI_Line0);
  *
  *    // 同时清除 EXTI0 和 EXTI3 的挂起位
  *    EXTI_ClearITPendingBit(EXTI_Line0 | EXTI_Line3);
  */
void myEXTI_ClearITPendingBit(uint32_t EXTI_Line)
{
    /* 参数检查，确保传入的线路合法 */
    assert_param(IS_EXTI_LINE(EXTI_Line));

    /* 向 PR 寄存器写 1，清除指定线路的中断挂起位 */
    EXTI->PR = EXTI_Line;
}
