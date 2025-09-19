/**
  ******************************************************************************
  * @file     mystm32f4_wwdg.c
  * @author   MaXin
  * @version  V1.0.0
  * @date     2025-09-19
  * @brief    STM32F4系列WWDG外设驱动库（重写版）
  *
  * @attention
  *
  * 本文件为 STM32F4xx 标准外设库 WWDG 模块的自定义重写版本，编写过程中旨在加深对
  * STM32 编程和窗口看门狗（Window Watchdog, WWDG）工作机制的研究理解。
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
  * 2. 在使用任何 WWDG 函数前，必须首先调用 RCC_APB1PeriphClockCmd()
  *    使能 WWDG 外设时钟
  * 3. 使用 WWDG_Init() 函数进行窗口看门狗配置
  * 4. 使用 WWDG_SetCounter() 定期刷新计数器，防止复位
  * 5. 可以通过配置中断，监控提前唤醒事件
  *
  * 版本历史：
  * V1.0.0 初始版本
  *   - 实现基本的WWDG初始化、计数器设置与刷新功能
  *   - 提供完整的预分频器、窗口值配置支持
  *   - 添加详细的错误检查和调试支持
  *
  * 免责声明：
  * 本软件按“原样”提供，不提供任何明示或暗示的担保，包括但不限于对适销性或特定用途
  * 适用性的保证。作者不对因使用本软件而产生的任何直接、间接、偶然、特殊、示例性或
  * 后果性损失承担责任。本软件仅供学习和研究使用，无偿共享，禁止用于商业用途。
  *
  ******************************************************************************
  */



  /* ======================== 以下为官方库文字翻译 ========================
   ===============================================================================
                             ##### WWDG 特性 #####
   ===============================================================================
      [..]
          一旦使能，WWDG 会在设定的时间周期到期时产生系统复位，
          除非用户程序在计数器（递减计数器）到达 0x3F 之前刷新它。
          （即：当计数器值从 0x40 递减到 0x3F 时会触发复位）。

          如果计数器值在到达刷新窗口值之前被刷新，MCU 也会复位。
          这意味着计数器必须在一个受限的窗口时间内被刷新。

          一旦 WWDG 被使能，除非系统复位，否则不能被关闭。

          当发生 WWDG 复位时，可以通过 RCC_CSR 寄存器中的 WWDGRST 标志来判断。

          WWDG 计数器输入时钟来源于 APB 时钟，并且可以通过可编程分频器进行分频。

          WWDG 计数器时钟 = PCLK1 / 预分频系数
          WWDG 超时时间   = (WWDG 计数器时钟) * (计数器值)

          在 42 MHz (PCLK1) 下的最小-最大超时范围：约 97.5 μs 到 约 49.9 ms

                        ##### 如何使用此驱动库 #####
   ===============================================================================
      [..]
        (#) 使用 RCC_APB1PeriphClockCmd(RCC_APB1Periph_WWDG, ENABLE) 函数使能 WWDG 时钟

        (#) 使用 WWDG_SetPrescaler() 函数配置 WWDG 预分频器

        (#) 使用 WWDG_SetWindowValue() 函数配置 WWDG 刷新窗口值

        (#) 使用 WWDG_Enable() 函数设置 WWDG 计数器值并启动看门狗。
            当 WWDG 被使能时，计数器值必须大于 0x40，否则会立即触发复位。

        (#) 可选：可以使能提前唤醒中断（EWI），当计数器递减到 0x40 时触发。
            一旦启用，该中断只能通过系统复位才能关闭。

        (#) 在正常运行期间，应用程序必须周期性调用 WWDG_SetCounter()
            刷新 WWDG 计数器，以防止 MCU 被复位。
            刷新操作必须在计数器值小于预设的刷新窗口值时进行，
            该窗口值由 WWDG_SetWindowValue() 配置。

   ===============================================================================
   ======================== 官方库文字翻译结束 ======================== */


   /* 头文件包含 ------------------------------------------------------------------*/
#include "mystm32f4xx_wwdg.h"
#include "stm32f4xx_rcc.h"

/** @addtogroup STM32F4xx_StdPeriph_Driver
  * @{
  */

  /** @defgroup WWDG
    * @brief WWDG 驱动模块
    * @{
    */

    /* 私有类型定义 ---------------------------------------------------------------*/
    /* 私有常量定义 ---------------------------------------------------------------*/

    /* ----------- WWDG 寄存器在别名区（bit-band 区域）的偏移地址 ----------- */
#define WWDG_OFFSET       (WWDG_BASE - PERIPH_BASE)

/* CFR 寄存器偏移地址（相对于 WWDG 基地址） */
#define CFR_OFFSET        (WWDG_OFFSET + 0x04)

/* EWI（提前唤醒中断）位在 CFR 寄存器中的位编号 */
#define EWI_BitNumber     0x09

/* CFR_EWI 位在位带区（bit-band）的别名地址 */
#define CFR_EWI_BB        (PERIPH_BB_BASE + (CFR_OFFSET * 32) + (EWI_BitNumber * 4))

/* --------------------- WWDG 寄存器位掩码 ------------------------ */
/* CFR 寄存器 WDGTB[1:0] 位的掩码（用于清除预分频器设置位） */
#define CFR_WDGTB_MASK    ((uint32_t)0xFFFFFE7F)

/* CFR 寄存器 W[6:0] 位的掩码（用于清除窗口值位） */
#define CFR_W_MASK        ((uint32_t)0xFFFFFF80)

/* 通用 7 位掩码（0x7F = 二进制 0111 1111） */
#define BIT_MASK          ((uint8_t)0x7F)

/* 私有宏定义 ---------------------------------------------------------------*/
/* 私有变量声明 ---------------------------------------------------------------*/
/* 私有函数原型 ---------------------------------------------------------------*/
/* 私有函数实现 ---------------------------------------------------------------*/



/** @defgroup WWDG_Group1 预分频器、刷新窗口和计数器配置函数
 *  @brief   提供 WWDG 的预分频器设置、刷新窗口设置以及计数器配置相关的函数
 */


   /**
     * @brief  将 WWDG 外设寄存器恢复到默认复位值（去初始化）
     * @note   该函数通过对 WWDG 外设产生一次总线复位来把寄存器恢复为出厂（复位）值。
     *         调用后 WWDG 的寄存器配置会被清空为默认值。
     * @param  None
     * @retval None
     *
     * 小白说明：
     *  - RCC_APB1PeriphResetCmd(..., ENABLE) 会给 WWDG 一个“复位脉冲”（把寄存器
     *    置为复位值），随后用 DISABLE 结束复位信号。
     *  - 在使用本函数前，通常不需要先使能 WWDG 时钟（该复位操作直接操作 RCC 寄存器）。
     *  - 注意：WWDG 是较特殊的外设，一旦真正启用后其行为由硬件决定；官方库仍采用这种复位
     *    方式来还原寄存器，若你遇到“无法停止计数器”的情况，请以芯片手册为准（部分情形需系统复位）。
     */
void myWWDG_DeInit(void)
{
    /* 置位 APB1 外设复位位，触发 WWDG 外设复位（寄存器回到默认值） */
    RCC_APB1PeriphResetCmd(RCC_APB1Periph_WWDG, ENABLE);

    /* 取消复位信号（完成一次脉冲式复位） */
    RCC_APB1PeriphResetCmd(RCC_APB1Periph_WWDG, DISABLE);
}

/**
  * @brief  设置 WWDG 的预分频器（Prescaler）。
  * @param  WWDG_Prescaler: 指定预分频器，可取以下值之一：
  *         - WWDG_Prescaler_1 : WWDG 计数器时钟 = (PCLK1 / 4096) / 1
  *         - WWDG_Prescaler_2 : WWDG 计数器时钟 = (PCLK1 / 4096) / 2
  *         - WWDG_Prescaler_4 : WWDG 计数器时钟 = (PCLK1 / 4096) / 4
  *         - WWDG_Prescaler_8 : WWDG 计数器时钟 = (PCLK1 / 4096) / 8
  * @retval None
  *
  * 小白说明（逐步理解）：
  *  - 预分频器决定 WWDG 的时钟频率，从而影响超时时间（计数器递减的速度）。
  *  - 函数内部采用“读-改-写”方式修改 CFR 寄存器：先读取 CFR，清除预分频位，
  *    然后把新的预分频值或回去并写回，从而不影响 CFR 上的其它位（比如窗口值 W 或 EWI）。
  *  - 在调试版（含 assert）里，如果传入非法参数会触发断言（assert_param），
  *    这有助于快速发现调用错误。发布版（release）通常不会做断言检查。
  */
void myWWDG_SetPrescaler(uint32_t WWDG_Prescaler)
{
    uint32_t tmpreg = 0U;

    /* 参数检查（IS_WWDG_PRESCALER 宏在头文件中定义） */
    assert_param(IS_WWDG_PRESCALER(WWDG_Prescaler));

    /* 1) 读取当前 CFR 寄存器值，并通过掩码清除 WDGTB（预分频）位，
          保留 CFR 的其它位（比如窗口值、EWI 位等）。 */
    tmpreg = WWDG->CFR & CFR_WDGTB_MASK;

    /* 2) 把新的预分频值合并进 tmpreg。注意 WWDG_Prescaler 宏已经是按位对齐好的值 */
    tmpreg |= WWDG_Prescaler;

    /* 3) 写回 CFR 寄存器（完成设置） */
    WWDG->CFR = tmpreg;
}



/**
  * @brief  设置 WWDG 的窗口值（Window Value）。
  *
  * @param  WindowValue: 指定窗口值，与 WWDG 的递减计数器比较。
  *         - 取值范围：必须小于 0x80（即 0 ~ 127）。
  *         - 窗口值的作用：当应用程序刷新计数器时，如果计数器的当前值大于
  *           WindowValue，则会触发复位；只有当计数器递减到 ≤ WindowValue
  *           时，刷新才是“合法”的。
  *
  * @retval None
  *
  * 小白说明：
  *  - 窗口值 W 是“允许刷新计数器的门槛值”。
  *  - 刷新太早（计数器值 > W）：系统复位。
  *  - 刷新太晚（计数器减到 0x3F）：系统复位。
  *  - 正确刷新时机：计数器在 (0x3F ~ W] 之间。
  *
  * 使用举例：
  *  - 假设 WWDG 初始计数器设为 0x7F（127），
  *  - 如果 WindowValue = 0x50（80），
  *  - 那么应用必须等计数器递减到 80 以下时才能刷新，
  *    否则 WWDG 会立即触发复位。
  *
  * 注意：
  *  - 本函数只是修改寄存器里的窗口值，并不会启动看门狗。
  *  - 在调用 WWDG_Enable() 前应先配置好窗口值。
  */
void myWWDG_SetWindowValue(uint8_t WindowValue)
{
    __IO uint32_t tmpreg = 0;  /* 定义临时变量，存放 CFR 的修改结果 */

    /* 参数检查：WindowValue 必须 < 0x80，否则断言失败 */
    assert_param(IS_WWDG_WINDOW_VALUE(WindowValue));

    /* 1) 读取当前 CFR 寄存器，并清除 W[6:0] 位（窗口值位），保留其它配置 */
    tmpreg = WWDG->CFR & CFR_W_MASK;

    /* 2) 把新的 WindowValue 值写入到 tmpreg（只保留 7 位有效值） */
    tmpreg |= WindowValue & (uint32_t)BIT_MASK;

    /* 3) 将新的配置写回 CFR 寄存器，更新窗口值 */
    WWDG->CFR = tmpreg;
}



/**
  ******************************************************************************
  * @brief  使能 WWDG 的提前唤醒中断 (EWI: Early Wakeup Interrupt)。
  *
  * @note   一旦启用 EWI 中断，它就不能被关闭，除非整个系统发生复位。
  *         - 当 WWDG 计数器递减到 0x40 时，会触发 EWI 中断。
  *         - 应用程序可以在 EWI 中断服务函数中，执行必要的预处理，
  *           例如保存重要数据、记录错误信息等。
  *         - 如果不在中断后及时刷新计数器，计数器会继续递减并最终复位系统。
  *
  * 使用场景：
  *   - 作为“最后的警告”机制，当系统快要复位时，给应用一个机会做紧急处理。
  *
  * @param  None
  * @retval None
  ******************************************************************************
  */
void myWWDG_EnableIT(void)
{
    /* CFR_EWI_BB 是 WWDG->CFR 中 EWI 位的位带别名地址。
       向这里写入 ENABLE (1)，就等于设置了 EWI 位，开启提前唤醒中断 */
    *(__IO uint32_t*) CFR_EWI_BB = (uint32_t)ENABLE;
}


/**
  ******************************************************************************
  * @brief  设置 WWDG 的计数器值。
  *
  * @param  Counter: 要写入的计数器初始值。
  *         必须在 0x40 ~ 0x7F 之间。
  *         - 如果小于 0x40，会立刻触发复位（因为计数器继续递减会到 0x3F）。
  *         - 如果大于 0x7F，是非法值（超出 7 位计数器范围）。
  *
  * 工作机制说明：
  *   - WWDG 是一个 7 位递减计数器 (范围 0x7F~0x40)。
  *   - 每次设置 Counter，其值会作为新的起始点。
  *   - 程序必须在 Counter 递减到低于窗口值之前刷新，否则会复位。
  *
  * 注意事项：
  *   - 这里直接写 WWDG->CR 的低 7 位（T[6:0]）。
  *   - 写入时不需要先读改写，因为 WDGA 位（bit7）不会因写 0 而被清除。
  *
  * 举例：
  *   - 如果 Counter = 0x7F，计数器会从 127 开始递减。
  *   - 程序必须在到达窗口区间 (WindowValue~0x40) 时刷新，
  *     否则要么太早触发复位，要么太晚复位。
  *
  * @retval None
  ******************************************************************************
  */
void myWWDG_SetCounter(uint8_t Counter)
{
    /* 参数检查：Counter 必须在 0x40~0x7F 之间 */
    assert_param(IS_WWDG_COUNTER(Counter));

    /* 设置计数器值（T[6:0]）。
       由于 WDGA 位（bit7）写 0 没有效果，所以不用担心误关掉 WWDG。 */
    WWDG->CR = Counter & BIT_MASK;
}



/** @defgroup WWDG_Group2 WWDG 激活函数
 *  @brief   WWDG 的启动与激活相关的函数
 *
@verbatim
 ===============================================================================
                    ##### WWDG 激活函数 #####
 ===============================================================================
  这些函数用于启动 WWDG（窗口看门狗）并装载初始计数值。
  一旦启动，WWDG 无法被软件关闭，只有系统复位才能停止。
@endverbatim
  * @{
  */

  /**
    ******************************************************************************
    * @brief  启动 WWDG（窗口看门狗），并设置计数器的初始值。
    *
    * @param  Counter: 初始计数器值。
    *         - 取值范围：0x40 ~ 0x7F。
    *         - 如果小于 0x40，会立即触发复位（因为计数器继续递减到 0x3F）。
    *         - 大于 0x7F 是非法值（超出 7 位计数器的范围）。
    *
    * 功能说明：
    *   - WWDG 是一个 7 位递减计数器（范围 0x7F~0x40）。
    *   - 调用本函数会同时：
    *       1. 设置 WDGA 使能位（bit7），启动看门狗。
    *       2. 装载计数器初始值 Counter。
    *   - 一旦启动 WWDG，无法通过软件关闭，必须依靠 MCU 复位来停止。
    *
    * 使用注意：
    *   - 一般第一次调用 `WWDG_Enable()` 来启动 WWDG。
    *   - 之后程序中必须定期调用 `WWDG_SetCounter()` 来刷新计数器，
    *     保持系统不被复位。
    *   - 如果刷新太早（高于 WindowValue），或太晚（小于 0x40），
    *     系统都会复位。
    *
    * 举例：
    *   - 假设调用 WWDG_Enable(0x7F)，看门狗启动并从 127 开始倒数。
    *   - 如果设置的 WindowValue = 0x50，则应用必须在计数器递减到
    *     0x50 ~ 0x40 之间刷新，否则会复位。
    *
    * @retval None
    ******************************************************************************
    */
void myWWDG_Enable(uint8_t Counter)
{
    /* 检查参数是否在合法范围内 (0x40~0x7F) */
    assert_param(IS_WWDG_COUNTER(Counter));

    /* 设置 WDGA 使能位（bit7），并写入计数器初始值。
       WWDG_CR_WDGA 表示启动 WWDG。
       Counter & 0x7F 用于保证只写入低 7 位作为计数器值。 */
    WWDG->CR = WWDG_CR_WDGA | Counter;
}
/**
  * @}
  */


  /** @defgroup WWDG_Group3 中断与标志管理函数
   *  @brief   WWDG 中断与状态标志的管理函数
   *
  @verbatim
   ===============================================================================
              ##### 中断与标志管理函数 #####
   ===============================================================================
    这些函数用于查询 WWDG 的状态标志以及中断状态。
    例如，提前唤醒中断（EWI）是否已经触发。
  @endverbatim
    * @{
    */

    /**
      ******************************************************************************
      * @brief  检查 WWDG 提前唤醒中断（EWI）标志是否被置位。
      *
      * @note   当计数器递减到 0x40 时，如果 EWI 中断被使能，则 SR 寄存器的
      *         相应标志位会被置位。
      *
      * @param  None
      * @retval FlagStatus:
      *         - SET   ：EWI 标志已被置位
      *         - RESET ：EWI 标志未被置位
      *
      * 小白说明：
      *   - 这个函数相当于问硬件：“提前唤醒中断有没有发生？”
      *   - 返回 SET 表示中断条件已经触发，但不意味着一定马上会复位，
      *     还需要程序刷新计数器。
      *   - 返回 RESET 表示中断条件未触发。
      *
      * 使用举例：
      *   if(WWDG_GetFlagStatus() == SET)
      *   {
      *       // 做一些紧急处理，比如保存数据、记录日志
      *   }
      ******************************************************************************
      */
FlagStatus myWWDG_GetFlagStatus(void)
{
    /* 用三目运算符优化判断：
       如果 SR 寄存器不为 0，说明 EWI 标志已置位，返回 SET，否则返回 RESET */
    return (WWDG->SR != (uint32_t)RESET) ? SET : RESET;
}
/**
  * @}
  */


  /**
    ******************************************************************************
    * @brief  清除 WWDG 的提前唤醒中断（EWI）标志。
    *
    * @note
    *   - 当计数器递减到 0x40 时，如果 EWI 中断被使能，SR 寄存器中的 EWI 标志会被置位。
    *   - 如果不清除该标志，下一次读取 SR 仍会显示 EWI 已触发。
    *   - 清除标志不会影响计数器的递减，也不会关闭看门狗。
    *
    * 使用场景：
    *   - 在中断服务函数（EWI Handler）里，通常在处理完紧急任务后调用此函数，
    *     以清除标志，准备下一次中断。
    *
    * @param  None
    * @retval None
    *
    * 小白说明：
    *   - SR 寄存器里记录了看门狗的状态标志。
    *   - 调用 WWDG_ClearFlag() 就相当于告诉硬件：“我已经处理过这次提前唤醒中断了，可以清掉标志位”。
    ******************************************************************************
    */
void myWWDG_ClearFlag(void)
{
    /* 将 SR 寄存器写 0 清除 EWI 标志 */
    WWDG->SR = (uint32_t)RESET;
}



/**
  ******************************************************************************
  * @brief  初始化 WWDG（窗口看门狗），并可选配置 EWI 中断 NVIC
  *
  * @param  prescaler : 预分频器
  * @param  window    : 刷新窗口值（0x40 ~ 0x7F）
  * @param  counter   : 初始计数器值（0x40 ~ 0x7F）
  * @param  enableEWI : 是否启用提前唤醒中断（0=关闭, 1=开启）
  * @retval None
  *
  * 使用示例：
  *   #define ENABLE_WWDG_EWI_NVIC
  *   WWDG_Init(WWDG_Prescaler_8, 0x50, 0x7F, 1);
  ******************************************************************************
  */
void myWWDG_Init(uint32_t prescaler, uint8_t window, uint8_t counter, uint8_t enableEWI)
{
    /* 1. 使能 WWDG 时钟 */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_WWDG, ENABLE);

    /* 2. 设置 WWDG 预分频器 */
    assert_param(IS_WWDG_PRESCALER(prescaler));
    myWWDG_SetPrescaler(prescaler);

    /* 3. 设置刷新窗口 */
    assert_param(IS_WWDG_WINDOW_VALUE(window));
    myWWDG_SetWindowValue(window);

    if (enableEWI)
    {
        myWWDG_ClearFlag();    // 清除 EWI 标志位
        myWWDG_EnableIT();     // 启用 EWI 中断

        // 配置 NVIC 响应 WWDG_IRQn
        NVIC_InitTypeDef NVIC_InitStructure;
        NVIC_InitStructure.NVIC_IRQChannel = WWDG_IRQn;
        NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x0F;
        NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x0F;
        NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
        NVIC_Init(&NVIC_InitStructure);
    }


    /* 5. 设置计数器值并启动 WWDG */
    assert_param(IS_WWDG_COUNTER(counter));
    myWWDG_Enable(counter);

    /* 小白提示：
       - WWDG 启动后不可关闭，只能靠系统复位。
       - 程序必须在 window~0x40 区间内刷新计数器，否则复位。
       - 如果启用了 EWI 中断，且定义了 ENABLE_WWDG_EWI_NVIC，会在 NVIC 配置中触发中断。
    */
}
