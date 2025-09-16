/**
  ******************************************************************************
  * @file     stm32f4xx_gpio_rewrite.c
  * @author   QiangGu
  * @version  V1.0.0
  * @date     2025-09-16
  * @brief    STM32F4系列GPIO外设驱动库（重写版）
  *
  * @attention
  *
  *本文件为 STM32F4xx 标准外设库 GPIO 模块的自定义重写版本，编写过程中旨在加深对 STM32 编程和 GPIO 工作机制的研究理解。
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
  * 1. 本库函数设计为与STM32F4xx系列微控制器配合使用
  * 2. 在使用任何GPIO函数前，必须首先调用RCC_APB2PeriphClockCmd()使能GPIO时钟
  * 3. 使用GPIO_Init()函数初始化GPIO引脚配置
  * 4. 本库提供完整的GPIO操作函数，包括读写、模式设置等
  *
  * 版本历史：
  * V1.0.0 初始版本
  *   - 实现基本的GPIO初始化、读写功能
  *   - 提供完整的模式配置支持
  *   - 添加详细的错误检查和调试支持
  * 免责声明：
  * 本软件按“原样”提供，不提供任何明示或暗示的担保，包括但不限于对适销性或特定用途适用性的保证。
  * 作者不对因使用本软件而产生的任何直接、间接、偶然、特殊、示例性或后果性损失承担责任。
  * 本软件仅供学习和研究使用，无偿共享，禁止用于商业用途。

  *
  ******************************************************************************
  */

#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"



void myGPIO_DeInit(GPIO_TypeDef* GPIOx)
{
    /* 首先声明变量 */
    uint32_t peripheral = 0;

    /* Check the parameters */
    assert_param(IS_GPIO_ALL_PERIPH(GPIOx));

    /* 然后使用三目运算符 */
    peripheral =
        (GPIOx == GPIOA) ? RCC_AHB1Periph_GPIOA :
        (GPIOx == GPIOB) ? RCC_AHB1Periph_GPIOB :
        (GPIOx == GPIOC) ? RCC_AHB1Periph_GPIOC :
        (GPIOx == GPIOD) ? RCC_AHB1Periph_GPIOD :
        (GPIOx == GPIOE) ? RCC_AHB1Periph_GPIOE :
        (GPIOx == GPIOF) ? RCC_AHB1Periph_GPIOF :
        (GPIOx == GPIOG) ? RCC_AHB1Periph_GPIOG :
        (GPIOx == GPIOH) ? RCC_AHB1Periph_GPIOH :
        (GPIOx == GPIOI) ? RCC_AHB1Periph_GPIOI :
        (GPIOx == GPIOJ) ? RCC_AHB1Periph_GPIOJ :
        (GPIOx == GPIOK) ? RCC_AHB1Periph_GPIOK : 0;

    if (peripheral != 0)
    {
        RCC_AHB1PeriphResetCmd(peripheral, ENABLE);
        RCC_AHB1PeriphResetCmd(peripheral, DISABLE);
    }
}



/**
  * @brief  初始化 GPIO 引脚
  * @note
  *         1. 本函数只会配置用户在 GPIO_InitStruct->GPIO_Pin 中选择的引脚，
  *            而不是遍历全部 16 个引脚，所以效率比传统写法更高。
  *         2. 支持同时配置多个引脚（用“或”运算组合，比如 GPIO_Pin_0 | GPIO_Pin_1）。
  *         3. 本函数不会配置引脚的复用功能(AF)，如果需要复用功能（如串口、I2C），
  *            还需调用 GPIO_PinAFConfig() 设置具体的复用通道。
  *
  * @param  GPIOx: GPIO端口基地址
  *                取值范围：GPIOA ~ GPIOI（不同芯片可能不全有）
  *
  * @param  GPIO_InitStruct: 指向 GPIO_InitTypeDef 类型结构体的指针，
  *                该结构体需要提前填充以下成员：
  *                - GPIO_Pin   : 需要配置的引脚集合，可多选
  *                - GPIO_Mode  : 引脚模式（输入、输出、复用、模拟）
  *                - GPIO_Speed : 输出速度（仅对输出/复用模式有效）
  *                - GPIO_OType : 输出类型（仅对输出/复用模式有效，推挽或开漏）
  *                - GPIO_PuPd  : 上下拉配置（无、上拉、下拉）
  *
  * @retval 无
  */
void myGPIO_Init(GPIO_TypeDef* GPIOx, GPIO_InitTypeDef* GPIO_InitStruct)
{
    uint32_t pins = GPIO_InitStruct->GPIO_Pin; // 保存用户选择的引脚集合
    uint32_t pinpos;

    while (pins) // 只要还有引脚需要处理
    {
        // 找到最低位为1的引脚位置（pinpos）
        // 用简单循环替代 __builtin_ctz
        for (pinpos = 0; pinpos < 16; pinpos++)
        {
            if (pins & (1U << pinpos))
                break; // 找到最低位
        }

        // 清除已处理的引脚
        pins &= ~(1U << pinpos);

        // ---------------- 配置 MODER ----------------
        GPIOx->MODER &= ~(0x3U << (pinpos * 2));
        GPIOx->MODER |= ((uint32_t)GPIO_InitStruct->GPIO_Mode << (pinpos * 2));

        // ---------------- 配置输出速度和类型（仅输出/复用模式） ----------------
        ((GPIO_InitStruct->GPIO_Mode == GPIO_Mode_OUT) ||
            (GPIO_InitStruct->GPIO_Mode == GPIO_Mode_AF)) ?
            ( // 三目运算符写法
                // 输出速度 OSPEEDR
                GPIOx->OSPEEDR = (GPIOx->OSPEEDR & ~(0x3U << (pinpos * 2))) |
                ((uint32_t)GPIO_InitStruct->GPIO_Speed << (pinpos * 2)),
                // 输出类型 OTYPER
                GPIOx->OTYPER = (GPIOx->OTYPER & ~(0x1U << pinpos)) |
                ((uint16_t)GPIO_InitStruct->GPIO_OType << pinpos)
                )
            : 0; // 如果不是输出或复用模式，则不操作

        // ---------------- 配置上下拉 PUPDR ----------------
        GPIOx->PUPDR &= ~(0x3U << (pinpos * 2));
        GPIOx->PUPDR |= ((uint32_t)GPIO_InitStruct->GPIO_PuPd << (pinpos * 2));
    }
}

/**
  * @brief  将 GPIO_InitTypeDef 结构体初始化为默认值
  * @note   1. 本函数用于在初始化前设置结构体默认值，然后可调用 myGPIO_Init 使用。
  *         2. 默认值如下：
  *            - GPIO_Pin  = GPIO_Pin_All
  *            - GPIO_Mode = GPIO_Mode_IN
  *            - GPIO_Speed = GPIO_Speed_2MHz
  *            - GPIO_OType = GPIO_OType_PP
  *            - GPIO_PuPd = GPIO_PuPd_NOPULL
  *
  * @param  GPIO_InitStruct : 指向需要初始化的 GPIO_InitTypeDef 结构体指针
  * @retval None
  */
void myGPIO_StructInit(GPIO_InitTypeDef* GPIO_InitStruct)
{
    static const GPIO_InitTypeDef default_init = {
        GPIO_Pin_All,      // 所有引脚
        GPIO_Mode_IN,      // 输入模式
        GPIO_Speed_2MHz,   // 速度 2MHz
        GPIO_OType_PP,     // 推挽输出
        GPIO_PuPd_NOPULL   // 无上下拉
    };

    *GPIO_InitStruct = default_init;
}

/**
  * @brief  锁定 GPIO 引脚配置寄存器，防止在程序运行期间被修改
  *
  * @note   1. STM32 的 GPIO 寄存器包括以下可以锁定的寄存器：
  *           - GPIOx_MODER   : 模式寄存器（输入/输出/复用/模拟）
  *           - GPIOx_OTYPER  : 输出类型（推挽/开漏）
  *           - GPIOx_OSPEEDR : 输出速度
  *           - GPIOx_PUPDR   : 上下拉电阻
  *           - GPIOx_AFRL    : 低 8 个引脚复用功能寄存器
  *           - GPIOx_AFRH    : 高 8 个引脚复用功能寄存器
  *         这些寄存器一旦锁定，对应引脚的配置将 **不能被修改**，除非芯片复位。
  *
  *         2. 锁定操作的顺序必须严格按照 STM32 官方手册：
  *            - 第一次写 LCKR：设置 LCKK=1 + 选中要锁定的引脚
  *            - 第二次写 LCKR：清 LCKK=0，只写引脚掩码
  *            - 第三次写 LCKR：再次设置 LCKK=1
  *            - 连续两次读取 LCKR：触发硬件锁定生效
  *
  * @param  GPIOx : 指向 GPIO 外设的寄存器结构体
  *                 - STM32F405/407/415/417: GPIOA~GPIOK
  *                 - STM32F42xxx/43xxx: GPIOA~GPIOI
  *                 - STM32F401xx: GPIOA, B, C, D, H
  *
  * @param  GPIO_Pin : 要锁定的 GPIO 引脚
  *                    - 可以是单个引脚，如 GPIO_Pin_0
  *                    - 也可以组合多个引脚，如 GPIO_Pin_0 | GPIO_Pin_3 | GPIO_Pin_5
  *
  * @retval None
  *
  * @attention
  *   1. 一旦锁定成功，锁定的引脚在运行期间无法修改配置。
  *   2. 只有复位芯片后才会解锁。
  *   3. 必须严格按照官方要求的写入 + 读取顺序，否则锁定可能不生效。
  *   4. 锁定只是硬件机制，不会改变引脚当前状态（高低电平），只禁止修改配置。
  */
void myGPIO_PinLockConfig(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin)
{
    volatile uint32_t tmp;
    /* 检查参数合法性 */
    assert_param(IS_GPIO_ALL_PERIPH(GPIOx));
    assert_param(IS_GPIO_PIN(GPIO_Pin));

    /* LCKR 寄存器锁定位 + 要锁定的引脚 */
   tmp = GPIO_Pin | 0x00010000U; // LCKK 位在第16位

    /* STM32 官方要求的锁定顺序 */
    GPIOx->LCKR = tmp;       // 第一次写：设置 LCKK = 1 + 选中引脚
    GPIOx->LCKR = GPIO_Pin;  // 第二次写：清 LCKK = 0，只写引脚掩码
    GPIOx->LCKR = tmp;       // 第三次写：再次设置 LCKK = 1

    /* 读取两次寄存器，确保锁定生效 */
    (void)GPIOx->LCKR;
    (void)GPIOx->LCKR;
}


/**
  * @brief  读取指定 GPIOx引脚的输入电平状态
  *
  * @note   1. GPIO 的每个引脚都有一个对应的输入数据寄存器 IDR 位：
  *            - bit = 1 → 高电平（逻辑 1）
  *            - bit = 0 → 低电平（逻辑 0）
  *         本函数用来读取指定引脚的状态。
  *
  *         2. 适合读取按键、开关、传感器信号等数字输入。
  *
  * @param  GPIOx : 指向 GPIO 外设寄存器结构体
  *                 - 例如 GPIOA、GPIOB 等
  *                 - STM32F405/407/415/417: GPIOA~GPIOK
  *                 - STM32F42xxx/43xxx: GPIOA~GPIOI
  *                 - STM32F401xx: GPIOA, B, C, D, H
  *
  * @param  GPIO_Pin : 要读取的 GPIO 引脚
  *                    - 可以是单个引脚，如 GPIO_Pin_0
  *                    - 也可以组合多个引脚，如 GPIO_Pin_0 | GPIO_Pin_3 | GPIO_Pin_5
  *                    - 本函数只读取指定引脚的电平
  *
  * @retval uint8_t : 返回引脚电平状态
  *                    - Bit_SET   (1) → 高电平
  *                    - Bit_RESET (0) → 低电平
  *
  * @attention
  *   1. 返回值只表示引脚当前电平，与引脚模式（输入/输出）无关。
  *   2. 如果指定多个引脚，函数只返回掩码中最低位引脚的状态。
  */
uint8_t myGPIO_ReadInputDataBit(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin)
{
    /* 检查参数是否合法 */
    assert_param(IS_GPIO_ALL_PERIPH(GPIOx));
    assert_param(IS_GET_GPIO_PIN(GPIO_Pin));

    /* 使用三目运算符判断引脚电平：
       - GPIOx->IDR & GPIO_Pin ：取出指定引脚的电平位
       - 如果不为 0，则引脚为高电平 Bit_SET
       - 否则为低电平 Bit_RESET */
    return ((GPIOx->IDR & GPIO_Pin) != (uint32_t)Bit_RESET) ? (uint8_t)Bit_SET : (uint8_t)Bit_RESET;
}


/**
  * @brief  读取指定 GPIOX端口的全部引脚输入电平
  *
  * @note   1. 每个 GPIO 端口最多有 16 个引脚（Pin0 ~ Pin15），
  *            每个位在输入数据寄存器 IDR 中对应一个引脚的状态：
  *            - bit = 1 → 高电平（逻辑 1）
  *            - bit = 0 → 低电平（逻辑 0）
  *         本函数返回整个端口的 16 位输入状态。
  *
  *         2. 适合一次性读取整个端口的引脚电平，例如：
  *            - 按键矩阵扫描
  *            - GPIO 输入状态监测
  *
  * @param  GPIOx : 指向要读取的 GPIO 外设寄存器结构体
  *                 - STM32F405/407/415/417: GPIOA~GPIOK
  *                 - STM32F42xxx/43xxx: GPIOA~GPIOI
  *                 - STM32F401xx: GPIOA, B, C, D, H
  *
  * @retval uint16_t : 返回整个 GPIO 端口的输入电平状态
  *                    - 每一位对应一个引脚
  *                    - bit = 1 → 高电平
  *                    - bit = 0 → 低电平
  *
  * @attention
  *   1. 返回值包含端口所有引脚的状态，如果只想读取某个引脚，请使用
  *      GPIO_ReadInputDataBit()。
  *   2. 输入电平的高低取决于外部信号或上拉/下拉配置。
  */
uint16_t myGPIO_ReadInputData(GPIO_TypeDef* GPIOx)
{
    /* ---------------- 参数检查 ---------------- */
    assert_param(IS_GPIO_ALL_PERIPH(GPIOx)); // 确保 GPIO 外设有效

    /* ---------------- 读取寄存器 ---------------- */
    // GPIOx->IDR 是输入数据寄存器，每个位对应一个引脚电平
    // 将其强制转换为 uint16_t 并返回
    return (uint16_t)(GPIOx->IDR);
}


/**
  * @brief  读取指定 GPIO 引脚的输出寄存器状态(注意读的是寄存器不是电平)
  *
  * @note   1. GPIO 的每个引脚都有对应的输出数据寄存器 ODR 位：
  *            - bit = 1 → 输出寄存器为高（程序设置高电平）
  *            - bit = 0 → 输出寄存器为低（程序设置低电平）
  *         本函数用于查看 **程序设置的输出状态**，而不是物理电平。
  *
  *         2. 如果引脚是推挽输出，ODR 高电平通常对应引脚实际输出高电平；
  *            但如果外部强制拉低，引脚物理电平可能不同。
  *
  * @param  GPIOx : 指向 GPIO 外设寄存器结构体
  *                 - 例如 GPIOA、GPIOB 等
  *                 - STM32F405/407/415/417: GPIOA~GPIOK
  *                 - STM32F42xxx/43xxx: GPIOA~GPIOI
  *                 - STM32F401xx: GPIOA, B, C, D, H
  *
  * @param  GPIO_Pin : 要读取的 GPIO 引脚
  *                    - 可以是单个引脚，如 GPIO_Pin_0
  *                    - 也可以组合多个引脚，如 GPIO_Pin_0 | GPIO_Pin_3
  *
  * @retval uint8_t : 返回引脚输出状态
  *                    - Bit_SET   (1) → 输出寄存器为高
  *                    - Bit_RESET (0) → 输出寄存器为低
  *
  * @attention
  *   1. 返回值表示 **寄存器状态**，不一定等于实际物理电平。
  *   2. 如果想判断物理电平，请使用 GPIO_ReadInputDataBit()。
  */
uint8_t myGPIO_ReadOutputDataBit(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin)
{
    /* ---------------- 参数检查 ---------------- */
    assert_param(IS_GPIO_ALL_PERIPH(GPIOx));   // 确保 GPIO 外设有效
    assert_param(IS_GET_GPIO_PIN(GPIO_Pin));  // 确保 GPIO 引脚有效

    /* ---------------- 三目运算符读取 ODR ---------------- */
    // 核心逻辑：
    // - GPIOx->ODR & GPIO_Pin ：取出指定引脚的输出寄存器位
    // - 如果不为 0，则该引脚寄存器为高 Bit_SET
    // - 否则为低 Bit_RESET
    return ((GPIOx->ODR & GPIO_Pin) != (uint32_t)Bit_RESET) ? (uint8_t)Bit_SET : (uint8_t)Bit_RESET;
}


/**
  * @brief  读取指定 GPIO 端口的全部输出寄存器状态
  *
  * @note   1. GPIO 端口的输出寄存器 ODR（Output Data Register）：
  *            - 每个位对应一个引脚的输出状态
  *            - bit = 1 → 输出寄存器设置为高电平（Bit_SET）
  *            - bit = 0 → 输出寄存器设置为低电平（Bit_RESET）
  *
  *         2. 本函数 **读取的是寄存器值**，反映程序设置的输出状态
  *            - 如果引脚是推挽输出，ODR 高通常对应实际高电平
  *            - 但如果外部强制拉低，物理电平可能不同
  *
  * @param  GPIOx : 指向 GPIO 外设寄存器结构体
  *                 - STM32F405/407/415/417: GPIOA~GPIOK
  *                 - STM32F42xxx/43xxx: GPIOA~GPIOI
  *                 - STM32F401xx: GPIOA, B, C, D, H
  *
  * @retval uint16_t : 返回整个 GPIO 端口的输出寄存器状态
  *                    - 每个位对应一个引脚
  *                    - bit = 1 → 高电平（程序设置）
  *                    - bit = 0 → 低电平（程序设置）
  *
  * @attention
  *   1. 返回值只表示 **寄存器状态**，不一定等于实际引脚电平
  *   2. 如果只想读取单个引脚，使用 GPIO_ReadOutputDataBit() 更方便
  */
uint16_t myGPIO_ReadOutputData(GPIO_TypeDef* GPIOx)
{
    /* ---------------- 参数检查 ---------------- */
    // 确保传入的 GPIO 外设合法，防止非法寄存器访问
    assert_param(IS_GPIO_ALL_PERIPH(GPIOx));

    /* ---------------- 读取输出寄存器 ---------------- */
    // GPIOx->ODR 每个位对应一个引脚的输出寄存器状态
    // 例如 bit0 = PA0, bit1 = PA1, ... , bit15 = PA15
    // 将寄存器值转换为 uint16_t 并返回
    return (uint16_t)(GPIOx->ODR);
}



/**
  * @brief  设置指定 GPIO 引脚为高电平
  *
  * @note   1. 本函数使用 GPIOx_BSRR 寄存器（位设置/复位寄存器）来修改引脚状态，
  *            这样可以保证操作是原子性的（atomic），即在修改过程中不会被中断打断。
  *         2. 使用 BSRR 寄存器设置引脚高电平比直接修改 ODR 寄存器更安全，避免中断导致的竞争问题。
  *
  * @param  GPIOx : 指向要操作的 GPIO 外设寄存器结构体
  *                 - STM32F405/407/415/417: GPIOA~GPIOK
  *                 - STM32F42xxx/43xxx: GPIOA~GPIOI
  *                 - STM32F401xx: GPIOA, B, C, D, H
  *
  * @param  GPIO_Pin : 要设置为高电平的 GPIO 引脚
  *                    - 可以是单个引脚，例如 GPIO_Pin_0
  *                    - 也可以是多个引脚组合，例如 GPIO_Pin_0 | GPIO_Pin_3 | GPIO_Pin_5
  *
  * @retval None : 本函数没有返回值
  *
  * @attention
  *   1. 本函数只会将指定引脚置为高电平，不会影响其他引脚状态。
  *   2. 如果想将引脚置低，请使用 GPIO_ResetBits()。
  */
void myGPIO_SetBits(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin)
{
    /* ---------------- 参数检查 ---------------- */
    // 确保 GPIO 外设合法
    assert_param(IS_GPIO_ALL_PERIPH(GPIOx));
    // 确保 GPIO 引脚有效
    assert_param(IS_GPIO_PIN(GPIO_Pin));

    /* ---------------- 设置引脚高电平 ---------------- */
    // BSRRL 寄存器：位设置寄存器（低 16 位对应 Pin0~Pin15）
    // 写 1 到对应位即可将引脚置高电平，写 0 无影响
    GPIOx->BSRRL = GPIO_Pin;
}



/**
  * @brief  将指定 GPIO 引脚输出置为低电平
  *
  * @note   1. 本函数使用 GPIOx_BSRR 寄存器的高 16 位（BSRRH）来复位引脚，
  *            这样可以保证操作是原子性的（atomic），不会被中断打断。
  *         2. 使用 BSRRH 寄存器复位引脚比直接修改 ODR 寄存器更安全，避免中断导致的竞争问题。
  *
  * @param  GPIOx : 指向要操作的 GPIO 外设寄存器结构体
  *                 - STM32F405/407/415/417: GPIOA~GPIOK
  *                 - STM32F42xxx/43xxx: GPIOA~GPIOI
  *                 - STM32F401xx: GPIOA, B, C, D, H
  *
  * @param  GPIO_Pin : 要置低电平的 GPIO 引脚
  *                    - 可以是单个引脚，例如 GPIO_Pin_0
  *                    - 也可以是多个引脚组合，例如 GPIO_Pin_0 | GPIO_Pin_3 | GPIO_Pin_5
  *
  * @retval None : 本函数没有返回值
  *
  * @attention
  *   1. 本函数只会将指定引脚置为低电平，不会影响其他引脚状态。
  *   2. 如果想将引脚置高，请使用 GPIO_SetBits()。
  */
void myGPIO_ResetBits(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin)
{
    /* ---------------- 参数检查 ---------------- */
    // 确保 GPIO 外设合法
    assert_param(IS_GPIO_ALL_PERIPH(GPIOx));
    // 确保 GPIO 引脚有效
    assert_param(IS_GPIO_PIN(GPIO_Pin));

    /* ---------------- 设置引脚低电平 ---------------- */
    // BSRRH 寄存器：位复位寄存器（高 16 位对应 Pin0~Pin15）
    // 写 1 到对应位即可将引脚置低电平，写 0 无影响
    GPIOx->BSRRH = GPIO_Pin;
}


/**
  * @brief  设置或清除指定 GPIO 引脚输出
  *
  * @note   1. 本函数根据 BitVal 参数决定将 GPIO 引脚设置为高电平还是低电平。
  *         2. 使用 GPIOx_BSRR 寄存器进行原子操作：
  *            - BSRRL（低 16 位）写 1 → 对应引脚置高
  *            - BSRRH（高 16 位）写 1 → 对应引脚置低
  *
  * @param  GPIOx : 指向要操作的 GPIO 外设寄存器结构体
  *                 - STM32F405/407/415/417: GPIOA~GPIOK
  *                 - STM32F42xxx/43xxx: GPIOA~GPIOI
  *                 - STM32F401xx: GPIOA, B, C, D, H
  *
  * @param  GPIO_Pin : 要操作的 GPIO 引脚
  *                    - 单个引脚，如 GPIO_Pin_0
  *                    - 或多个引脚组合，如 GPIO_Pin_0 | GPIO_Pin_3
  *
  * @param  BitVal : 指定引脚输出的值
  *                  - Bit_SET   → 设置高电平
  *                  - Bit_RESET → 设置低电平
  *
  * @retval None : 本函数没有返回值
  *
  * @attention
  *   1. 本函数只修改指定引脚状态，不影响其他引脚。
  *   2. 原子操作，防止中断干扰。
  */
void myGPIO_WriteBit(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, BitAction BitVal)
{
    /* ---------------- 参数检查 ---------------- */
    assert_param(IS_GPIO_ALL_PERIPH(GPIOx));    // 检查 GPIO 外设是否合法
    assert_param(IS_GET_GPIO_PIN(GPIO_Pin));    // 检查 GPIO 引脚是否合法
    assert_param(IS_GPIO_BIT_ACTION(BitVal));   // 检查 BitVal 是否有效

    /* ---------------- 使用三目运算符设置或清除引脚 ---------------- */
    (BitVal != Bit_RESET) ? (GPIOx->BSRRL = GPIO_Pin) : (GPIOx->BSRRH = GPIO_Pin);

}


/**
  * @brief  向指定 GPIO 端口输出寄存器写入数据
  *
  * @note   1. 本函数一次性写入整个 GPIO 端口的 ODR（输出数据寄存器），
  *            即一次性设置端口上所有引脚的输出状态。
  *         2. 写入的值直接覆盖原寄存器内容，所有引脚状态都会被更新，
  *            不会保留原有状态。
  *
  * @param  GPIOx : 指向要操作的 GPIO 外设寄存器结构体
  *                 - STM32F405/407/415/417: GPIOA~GPIOK
  *                 - STM32F42xxx/43xxx: GPIOA~GPIOI
  *                 - STM32F401xx: GPIOA, B, C, D, H
  *
  * @param  PortVal : 要写入 GPIO 端口输出寄存器的值
  *                   - 每一位对应一个引脚的输出状态
  *                   - 1 → 设置对应引脚高电平
  *                   - 0 → 设置对应引脚低电平
  *
  * @retval None : 本函数没有返回值
  *
  * @attention
  *   1. 会覆盖整个端口寄存器，慎用！如果只想修改部分引脚，请使用 GPIO_WriteBit() 或 GPIO_SetBits()/GPIO_ResetBits()。
  *   2. 本函数直接写入 ODR 寄存器，不保证操作的原子性，中断可能影响操作。
  */
void myGPIO_Write(GPIO_TypeDef* GPIOx, uint16_t PortVal)
{
    /* ---------------- 参数检查 ---------------- */
    assert_param(IS_GPIO_ALL_PERIPH(GPIOx)); // 确保 GPIO 外设合法

    /* ---------------- 写入输出寄存器 ---------------- */
    // ODR（Output Data Register）每个位对应一个 GPIO 引脚
    // 写 1 → 引脚输出高电平
    // 写 0 → 引脚输出低电平
    GPIOx->ODR = PortVal;
}



/**
  * @brief  切换（翻转）指定 GPIO 引脚的输出状态
  *
  * @note   1. 本函数通过对 ODR（输出数据寄存器）进行按位异或操作实现翻转：
  *            - 当前引脚高电平 → 切换为低电平
  *            - 当前引脚低电平 → 切换为高电平
  *         2. 可同时翻转多个引脚，通过按位或组合 GPIO_Pin 实现。
  *
  * @param  GPIOx : 指向要操作的 GPIO 外设寄存器结构体
  *                 - STM32F405/407/415/417: GPIOA~GPIOK
  *                 - STM32F42xxx/43xxx: GPIOA~GPIOI
  *                 - STM32F401xx: GPIOA, B, C, D, H
  *
  * @param  GPIO_Pin : 要翻转的 GPIO 引脚
  *                    - 单个引脚，例如 GPIO_Pin_0
  *                    - 或多个引脚组合，例如 GPIO_Pin_0 | GPIO_Pin_3
  *
  * @retval None : 本函数没有返回值
  *
  * @attention
  *   1. 直接操作 ODR 寄存器，操作过程中可能被中断打断，不保证原子性。
  *   2. 翻转操作会改变指定引脚的状态，未指定的引脚保持不变。
  */
void myGPIO_ToggleBits(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin)
{
    /* ---------------- 参数检查 ---------------- */
    assert_param(IS_GPIO_ALL_PERIPH(GPIOx)); // 检查 GPIO 外设是否合法

    /* ---------------- 翻转指定引脚 ---------------- */
    // ODR 寄存器 ^= GPIO_Pin
    // 按位异或操作：1变0，0变1，实现翻转
    GPIOx->ODR ^= GPIO_Pin;
}



/**
  * @brief  配置指定 GPIO 引脚的复用功能（Alternate Function, AF）
  *
  * @note
  * 1. STM32 的每个 GPIO 引脚可以复用为不同外设功能，例如 USART、SPI、TIM、I2C 等。
  * 2. GPIO 的复用功能通过 AFR[0] 和 AFR[1] 寄存器控制：
  *      - AFR[0] 控制引脚 0~7
  *      - AFR[1] 控制引脚 8~15
  * 3. 每个引脚占 4 位，因此可以配置 0~15 的复用功能（AF0~AF15）。
  *
  * @param  GPIOx : 要操作的 GPIO 端口，例如 GPIOA~GPIOK
  * @param  GPIO_PinSource : 要配置的引脚编号（0~15）
  * @param  GPIO_AF : 要映射的复用功能编号（0~15，对应 AF0~AF15）
  *
  * @retval None : 本函数没有返回值
  *
  * @attention
  * 1. 本函数直接操作 AFR 寄存器，会覆盖原来的 AF 设置。
  * 2. 使用前请确保 GPIO 已经开启时钟并初始化。
  *
  * @par GPIO AF 复用功能示意表（部分常用）
  * | AF编号 | 功能       | 适用外设                          |
  * |--------|------------|----------------------------------|
  * | AF0    | RTC_50Hz   | RTC                              |
  * | AF1    | TIM1/TIM2  | TIM1/TIM2                        |
  * | AF2    | TIM3/TIM4  | TIM3/TIM4/TIM5                   |
  * | AF3    | TIM8/TIM9  | TIM8/TIM9/TIM10/TIM11            |
  * | AF4    | I2C1/I2C2  | I2C1/I2C2/I2C3                    |
  * | AF5    | SPI1/SPI2  | SPI1/SPI2/SPI4/SPI5/SPI6         |
  * | AF6    | SPI3/SAI1  | SPI3/I2S3/SAI1                    |
  * | AF7    | USART1/2/3 | USART1/USART2/USART3              |
  * | AF8    | UART4/5/6  | UART4/UART5/USART6/UART7/UART8  |
  * | AF9    | CAN1/CAN2  | CAN1/CAN2/TIM12/TIM13/TIM14      |
  * | AF10   | OTG_FS/HS  | USB OTG FS/HS                     |
  * | AF11   | ETH        | 以太网 Ethernet                   |
  * | AF12   | FMC/SDIO   | FSMC/FMC/SDIO                     |
  * | AF13   | DCMI       | 摄像头接口                         |
  * | AF14   | LTDC       | LCD 控制器                        |
  * | AF15   | EVENTOUT   | 外部事件输出                       |
  */
void myGPIO_PinAFConfig(GPIO_TypeDef* GPIOx, uint16_t GPIO_PinSource, uint8_t GPIO_AF)
{
    uint32_t afr_index;   // AFR 寄存器索引（0 表示 AFR[0]，1 表示 AFR[1]）
    uint32_t afr_offset;  // 引脚在 AFR 寄存器中的偏移位（每个引脚占 4 位）

    /* ---------------- 参数检查 ---------------- */
    assert_param(IS_GPIO_ALL_PERIPH(GPIOx));        // 检查 GPIO 端口是否合法
    assert_param(IS_GPIO_PIN_SOURCE(GPIO_PinSource)); // 检查引脚编号是否合法
    assert_param(IS_GPIO_AF(GPIO_AF));             // 检查复用功能编号是否合法

    /* ---------------- 计算 AFR 寄存器索引和偏移 ---------------- */
    afr_index = GPIO_PinSource >> 3;              // 0~7 → AFR[0]，8~15 → AFR[1]
    afr_offset = (GPIO_PinSource & 0x07) * 4;    // 每个引脚占 4 位，计算偏移位

    /* ---------------- 清除该引脚原来的 AF 配置 ---------------- */
    GPIOx->AFR[afr_index] &= ~(0xF << afr_offset);

    /* ---------------- 设置该引脚的新 AF 配置 ---------------- */
    GPIOx->AFR[afr_index] |= ((uint32_t)GPIO_AF << afr_offset);
}
