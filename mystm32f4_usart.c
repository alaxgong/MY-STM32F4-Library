#include "mystm32f4xx_usart.h"
#include "stm32f4xx_rcc.h"

/**
  *******************************************************************************
  * @file     stm32f4xx_usart_rewrite.c
  * @author   QiangGu
  * @version  V1.0.0
  * @date     2025-09-16
  * @brief    STM32F4系列USART外设驱动库（重写版）
  *
  * @attention
  *
  * 本文件为 STM32F4xx 标准外设库 USART模块的自定义重写版本，编写过程中旨在加深对 STM32 编程和 USART 工作机制的研究理解。
  *
  * 此重写版本的目标：
  * 1. 提供更清晰的代码结构和命名规范
  * 2. 添加更详细的中文注释，便于学习理解
  * 3. 优化部分功能实现，提高代码可读性
  * 4. 保持与官方库函数的兼容性
  *
  * 与官方库的区别：
  * - 使用更现代的 C 语言特性（如 C99 标准）
  * - 增加更多的参数检查和错误处理
  * - 提供更详细的函数说明和使用示例
  * - 代码结构更模块化，便于理解和扩展
  *
  * 使用说明：
  * 1. 本库函数设计为与 STM32F4xx 系列微控制器配合使用
  * 2. 在使用任何 USART 函数前，必须首先调用 RCC_APB2PeriphClockCmd() 或 RCC_APB1PeriphClockCmd() 使能 USART 时钟
  * 3. 使用 USART_Init() 函数初始化 USART 外设配置
  * 4. 本库提供完整的 USART 操作函数，包括发送、接收、配置波特率、数据位、停止位、校验位等
  *
  * 版本历史：
  * V1.0.0 初始版本
  *   - 实现基本的 USART 初始化、发送和接收功能
  *   - 提供波特率、数据位、停止位和校验位配置支持
  *   - 添加详细的错误检查和调试支持
  *
  * 免责声明：
  * 本软件按“原样”提供，不提供任何明示或暗示的担保，包括但不限于对适销性或特定用途适用性的保证。
  * 作者不对因使用本软件而产生的任何直接、间接、偶然、特殊、示例性或后果性损失承担责任。
  * 本软件仅供学习和研究使用，无偿共享，禁止用于商业用途。
  *
  *******************************************************************************
*/

/**官方文档的翻译部分
  ******************************************************************************
  * @file    stm32f4xx_usart.c
  * @author  MCD 应用团队
  * @version V1.4.0
  * @date    2014年8月4日
  * @brief   本文件提供了管理以下功能的固件函数：
  *          通用同步异步收发器 (USART)：
  *           + 初始化和配置
  *           + 数据传输
  *           + 多处理器通信
  *           + LIN 模式
  *           + 半双工模式
  *           + 智能卡模式
  *           + IrDA 模式
  *           + DMA 传输管理
  *           + 中断和标志管理
  *
  @verbatim
 ===============================================================================
						##### 如何使用此驱动 #####
 ===============================================================================
	[..]
	  (#) 使用以下函数使能外设时钟
		  RCC_APB2PeriphClockCmd(RCC_APB2Periph_USARTx, ENABLE) 用于 USART1 和 USART6
		  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USARTx, ENABLE) 用于 USART2, USART3,
		  UART4 或 UART5。

	  (#) 根据 USART 模式，使用 RCC_AHB1PeriphClockCmd() 函数使能 GPIO 时钟。
		  （I/O 可以是 TX, RX, CTS 或/和 SCLK）。

	  (#) 外设的备用功能：
		(++) 使用 GPIO_PinAFConfig() 函数将引脚连接到所需外设的备用功能 (AF)
		(++) 将所需的引脚配置为备用功能模式：
			GPIO_InitStruct->GPIO_Mode = GPIO_Mode_AF
		(++) 通过 GPIO_PuPd, GPIO_OType 和 GPIO_Speed 成员选择类型、上下拉和输出速度
		(++) 调用 GPIO_Init() 函数

	  (#) 使用 USART_Init() 函数设置波特率、数据位长度、停止位、校验位、硬件流控制和模式（接收/发送）。

	  (#) 对于同步模式，使用 USART_ClockInit() 函数使能时钟并设置极性、相位和最后一位。

	  (#) 如果需要使用中断模式，使用 USART_ITConfig() 函数使能 NVIC 和相应的中断。

	  (#) 使用 DMA 模式时：
		(++) 使用 DMA_Init() 函数配置 DMA
		(++) 使用 USART_DMACmd() 函数激活所需的通道请求

	  (#) 使用 USART_Cmd() 函数使能 USART。

	  (#) 在使用 DMA 模式时，使用 DMA_Cmd() 函数使能 DMA。

	  -@- 更多细节请参阅多处理器、LIN、半双工、智能卡、IrDA 子部分。

	[..]
	为了达到更高的通信波特率，可以使用 USART_OverSampling8Cmd() 函数使能 8 倍过采样模式。
	此函数应该在使能 USART 时钟 (RCC_APBxPeriphClockCmd()) 后并在调用 USART_Init() 函数前调用。

	@endverbatim
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; 2014 STMicroelectronics 版权所有</center></h2>
  *
  * 根据 MCD-ST 自由软件许可证协议 V2（以下简称“许可证”）授权；
  * 您不得以任何方式使用此文件，除非符合许可证要求。
  * 您可以在以下位置获得许可证副本：
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * 除非适用法律要求或书面同意，否则根据许可证分发的软件按“原样”提供，
  * 不提供任何明示或暗示的担保，包括但不限于对适销性或特定用途适用性的担保。
  * 请参阅许可证了解相关的权限和限制。
  *
  ******************************************************************************
  * @official_translation  此处为官方翻译中文
  */


  /* Private typedef -----------------------------------------------------------*/
  /* Private define ------------------------------------------------------------*/

  /*!< USART CR1寄存器清除掩码 ((~(uint16_t)0xE9F3)) */
#define CR1_CLEAR_MASK            ((uint16_t)(USART_CR1_M | USART_CR1_PCE | \
                                              USART_CR1_PS | USART_CR1_TE | \
                                              USART_CR1_RE))

/*!< USART CR2寄存器时钟位清除掩码 ((~(uint16_t)0xF0FF)) */
#define CR2_CLOCK_CLEAR_MASK      ((uint16_t)(USART_CR2_CLKEN | USART_CR2_CPOL | \
                                              USART_CR2_CPHA | USART_CR2_LBCL))

/*!< USART CR3寄存器清除掩码 ((~(uint16_t)0xFCFF)) */
#define CR3_CLEAR_MASK            ((uint16_t)(USART_CR3_RTSE | USART_CR3_CTSE))

/*!< USART 中断掩码 */
#define IT_MASK                   ((uint16_t)0x001F)


/* 私有类型定义 -----------------------------------------------------------*/
/* 私有宏 -------------------------------------------------------------*/
/* 私有变量 ---------------------------------------------------------*/
/* 私有函数原型声明 -----------------------------------------------*/
/* 私有函数 ---------------------------------------------------------*/

/** @defgroup USART_Private_Functions
  * @{
  */

  /** @defgroup USART_Group1 初始化和配置函数
   *  @brief   初始化和配置函数
   *
  @verbatim
   ===============================================================================
			  ##### 初始化和配置函数 #####
   ===============================================================================
	  [..]
	  本小节提供了一组函数，用于在异步和同步模式下初始化 USART。
		(+) 仅在异步模式下，可以配置以下参数：
		  (++) 波特率
		  (++) 数据位长度
		  (++) 停止位
		  (++) 校验位：如果启用了校验位，则写入数据寄存器的数据的最高有效位（MSB）将被传输，但它将被校验位替代。
			   根据由 M 位（8 位或 9 位）定义的帧长度，可能的 USART 帧格式如下表所示：
	 +-------------------------------------------------------------+
	 |   M 位  |  PCE 位  |            USART 帧格式                |
	 |---------------------|---------------------------------------|
	 |    0    |    0      |    | SB | 8 位数据 | STB |          |
	 |---------|-----------|---------------------------------------|
	 |    0    |    1      |    | SB | 7 位数据 | PB | STB |     |
	 |---------|-----------|---------------------------------------|
	 |    1    |    0      |    | SB | 9 位数据 | STB |          |
	 |---------|-----------|---------------------------------------|
	 |    1    |    1      |    | SB | 8 位数据 | PB | STB |     |
	 +-------------------------------------------------------------+
		  (++) 硬件流控制
		  (++) 接收器/发送器模式

	  [..]
	  USART_Init() 函数遵循 USART 异步配置过程（该过程的详细信息请参阅参考手册 (RM0090)）。

	   (+) 对于同步模式，除了异步模式的参数外，还应配置以下参数：
		  (++) 启用 USART 时钟
		  (++) USART 极性
		  (++) USART 相位
		  (++) USART 最后一位

	  [..]
	  这些参数可以使用 USART_ClockInit() 函数进行配置。

  @endverbatim
	* @{
	*/


  /**
  * @brief  将指定的 USARTx 外设寄存器重置为默认的复位值
  * @param  USARTx: 选择一个 USART 或 UART 外设，可以是 1 到 8 中的一个
  *         USART 或 UART 外设。
  * @retval 无
  *
  * 该函数会根据传入的 USARTx 外设（USART1 到 UART8）来重置相应的外设控制寄存器。
  * 通过调用 `RCC_APBxPeriphResetCmd` 函数对外设进行软复位。每次复位后，外设将恢复为
  * 初始状态，所有控制寄存器的设置将被清除。
  *
  * - USART1 使用 `RCC_APB2PeriphResetCmd` 复位
  * - USART2、USART3、UART4、UART5、UART7、UART8 使用 `RCC_APB1PeriphResetCmd` 复位
  * - USART6 使用 `RCC_APB2PeriphResetCmd` 复位
  *
  * 该函数可以帮助用户在不再使用某个 USART 或 UART 外设时，进行必要的复位操作。
  * 适用于外设的初始化或重置操作。函数的核心工作是利用外设复位时钟寄存器清除指定外设的
  * 配置。
  */

  /**
	* @brief  将指定的 USARTx 外设寄存器重置为默认的复位值
	* @param  USARTx: 选择一个 USART 或 UART 外设，可以是 1 到 8 中的一个
	*         USART 或 UART 外设。
	* @retval 无
	*/
void myUSART_DeInit(USART_TypeDef* USARTx)
{
	/* 检查参数 */
	assert_param(IS_USART_ALL_PERIPH(USARTx));

	/* 通过三目运算符简化条件判断 */
	(USARTx == USART1) ? (RCC_APB2PeriphResetCmd(RCC_APB2Periph_USART1, ENABLE), RCC_APB2PeriphResetCmd(RCC_APB2Periph_USART1, DISABLE)) :
		(USARTx == USART2) ? (RCC_APB1PeriphResetCmd(RCC_APB1Periph_USART2, ENABLE), RCC_APB1PeriphResetCmd(RCC_APB1Periph_USART2, DISABLE)) :
		(USARTx == USART3) ? (RCC_APB1PeriphResetCmd(RCC_APB1Periph_USART3, ENABLE), RCC_APB1PeriphResetCmd(RCC_APB1Periph_USART3, DISABLE)) :
		(USARTx == UART4) ? (RCC_APB1PeriphResetCmd(RCC_APB1Periph_UART4, ENABLE), RCC_APB1PeriphResetCmd(RCC_APB1Periph_UART4, DISABLE)) :
		(USARTx == UART5) ? (RCC_APB1PeriphResetCmd(RCC_APB1Periph_UART5, ENABLE), RCC_APB1PeriphResetCmd(RCC_APB1Periph_UART5, DISABLE)) :
		(USARTx == USART6) ? (RCC_APB2PeriphResetCmd(RCC_APB2Periph_USART6, ENABLE), RCC_APB2PeriphResetCmd(RCC_APB2Periph_USART6, DISABLE)) :
		(USARTx == UART7) ? (RCC_APB1PeriphResetCmd(RCC_APB1Periph_UART7, ENABLE), RCC_APB1PeriphResetCmd(RCC_APB1Periph_UART7, DISABLE)) :
		(USARTx == UART8) ? (RCC_APB1PeriphResetCmd(RCC_APB1Periph_UART8, ENABLE), RCC_APB1PeriphResetCmd(RCC_APB1Periph_UART8, DISABLE)) : (void)0;
}


/**
  * @brief  初始化指定的 USART 外设，配置为传入的参数。
  *         根据 USART_InitStruct 中指定的参数初始化 USARTx 外设。
  * @param  USARTx: 选择的 USART 或 UART 外设，x 可以是 1, 2, 3, 4, 5, 6, 7 或 8。
  * @param  USART_InitStruct: 指向 USART_InitTypeDef 结构体的指针，包含了指定 USART 外设的配置参数。
  * @retval None
  *
  * 该函数会初始化 USART 外设的各个控制寄存器，包括波特率、数据位长度、停止位、校验位和工作模式等。
  */
void myUSART_Init(USART_TypeDef* USARTx, USART_InitTypeDef* USART_InitStruct)
{
	uint32_t tmpreg = 0x00, apbclock = 0x00;
	uint32_t integerdivider = 0x00;
	uint32_t fractionaldivider = 0x00;
	RCC_ClocksTypeDef RCC_ClocksStatus;

	/* 检查参数合法性 */
	assert_param(IS_USART_ALL_PERIPH(USARTx));                          // 检查 USART 外设是否有效
	assert_param(IS_USART_BAUDRATE(USART_InitStruct->USART_BaudRate));  // 检查波特率是否合法  
	assert_param(IS_USART_WORD_LENGTH(USART_InitStruct->USART_WordLength)); // 检查数据位长度是否合法
	assert_param(IS_USART_STOPBITS(USART_InitStruct->USART_StopBits));  // 检查停止位是否合法
	assert_param(IS_USART_PARITY(USART_InitStruct->USART_Parity));      // 检查校验位是否合法
	assert_param(IS_USART_MODE(USART_InitStruct->USART_Mode));          // 检查工作模式是否合法
	assert_param(IS_USART_HARDWARE_FLOW_CONTROL(USART_InitStruct->USART_HardwareFlowControl)); // 检查硬件流控制是否合法

	/* 如果使用硬件流控制，则只允许在 USART1, USART2, USART3 和 USART6 外设上使用 */
	assert_param((USART_InitStruct->USART_HardwareFlowControl == USART_HardwareFlowControl_None) ||
		(USARTx == USART1 || USARTx == USART2 || USARTx == USART3 || USARTx == USART6));  // 使用三目运算符

	/*---------------------------- 配置 USART CR2 寄存器 -----------------------*/
	tmpreg = USARTx->CR2;  // 读取 CR2 寄存器值

	tmpreg &= (uint32_t)~((uint32_t)USART_CR2_STOP);  // 清除 STOP[13:12] 位

	tmpreg |= (uint32_t)USART_InitStruct->USART_StopBits;  // 设置停止位

	USARTx->CR2 = (uint16_t)tmpreg;  // 写回 CR2 寄存器

	/*---------------------------- 配置 USART CR1 寄存器 -----------------------*/
	tmpreg = USARTx->CR1;  // 读取 CR1 寄存器值

	tmpreg &= (uint32_t)~((uint32_t)CR1_CLEAR_MASK);  // 清除 M, PCE, PS, TE 和 RE 位

	tmpreg |= (uint32_t)USART_InitStruct->USART_WordLength |
		USART_InitStruct->USART_Parity |
		USART_InitStruct->USART_Mode;  // 配置数据位长度、校验位和模式

	USARTx->CR1 = (uint16_t)tmpreg;  // 写回 CR1 寄存器

	/*---------------------------- 配置 USART CR3 寄存器 -----------------------*/
	tmpreg = USARTx->CR3;  // 读取 CR3 寄存器值

	tmpreg &= (uint32_t)~((uint32_t)CR3_CLEAR_MASK);  // 清除 CTSE 和 RTSE 位

	tmpreg |= USART_InitStruct->USART_HardwareFlowControl;  // 配置硬件流控制

	USARTx->CR3 = (uint16_t)tmpreg;  // 写回 CR3 寄存器

	/*---------------------------- 配置 USART 波特率寄存器 -----------------------*/
	RCC_GetClocksFreq(&RCC_ClocksStatus);  // 获取系统时钟频率

	// 根据不同的 USART 外设选择不同的时钟频率
	apbclock = (USARTx == USART1 || USARTx == USART6) ? RCC_ClocksStatus.PCLK2_Frequency : RCC_ClocksStatus.PCLK1_Frequency;  // 使用三目运算符

	/* 计算波特率的整数部分 */
	integerdivider = (USARTx->CR1 & USART_CR1_OVER8) ?
		((25 * apbclock) / (2 * (USART_InitStruct->USART_BaudRate))) :
		((25 * apbclock) / (4 * (USART_InitStruct->USART_BaudRate)));  // 使用三目运算符

	tmpreg = (integerdivider / 100) << 4;  // 计算波特率的整数部分并移位

	/* 计算波特率的分数部分 */
	fractionaldivider = integerdivider - (100 * (tmpreg >> 4));  // 计算分数部分

	/* 实现分数部分到寄存器 */
	tmpreg |= (USARTx->CR1 & USART_CR1_OVER8) ?
		((((fractionaldivider * 8) + 50) / 100)) & ((uint8_t)0x07) :
		((((fractionaldivider * 16) + 50) / 100)) & ((uint8_t)0x0F);  // 使用三目运算符

	USARTx->BRR = (uint16_t)tmpreg;  // 写回 BRR 寄存器
}

/**
  * @brief  将每个 USART_InitStruct 成员填充为默认值。
  * @param  USART_InitStruct: 指向一个 USART_InitTypeDef 结构体的指针，该结构体将被初始化。
  * @retval None
  *
  * 该函数将 USART 配置结构体的各个成员设置为默认值。这个函数通常用于初始化结构体，以便在后续的配置中使用。
  */
void myUSART_StructInit(USART_InitTypeDef* USART_InitStruct)
{
	/* 设置默认值 */
	USART_InitStruct->USART_BaudRate = 9600;  // 默认波特率为 9600
	USART_InitStruct->USART_WordLength = USART_WordLength_8b;  // 默认数据位长度为 8 位
	USART_InitStruct->USART_StopBits = USART_StopBits_1;  // 默认停止位为 1 位
	USART_InitStruct->USART_Parity = USART_Parity_No;  // 默认不使用校验位
	USART_InitStruct->USART_Mode = USART_Mode_Rx | USART_Mode_Tx;  // 默认启用接收和发送模式
	USART_InitStruct->USART_HardwareFlowControl = USART_HardwareFlowControl_None;  // 默认不启用硬件流控制  
}


/**
  * @brief  根据指定的 USART_ClockInitStruct 中的参数初始化 USARTx 外设的时钟。
  * @param  USARTx: 选择的 USART 外设，x 可以是 1、2、3 或 6。
  * @param  USART_ClockInitStruct: 指向一个 USART_ClockInitTypeDef 结构体的指针，包含配置 USART 时钟的参数。
  * @note   UART4 和 UART5 不支持智能卡和同步模式。
  * @retval None
  *
  * 该函数用于配置 USART 时钟相关的参数，包括时钟源、极性、相位和最后一位时钟等。
  */
void myUSART_ClockInit(USART_TypeDef* USARTx, USART_ClockInitTypeDef* USART_ClockInitStruct)
{
	uint32_t tmpreg = 0x00;  // 用于存储寄存器的临时变量

	/* 检查参数 */
	assert_param(IS_USART_1236_PERIPH(USARTx));  // 检查 USARTx 是否是有效的外设
	assert_param(IS_USART_CLOCK(USART_ClockInitStruct->USART_Clock));  // 检查时钟设置是否合法
	assert_param(IS_USART_CPOL(USART_ClockInitStruct->USART_CPOL));  // 检查时钟极性是否合法
	assert_param(IS_USART_CPHA(USART_ClockInitStruct->USART_CPHA));  // 检查时钟相位是否合法
	assert_param(IS_USART_LASTBIT(USART_ClockInitStruct->USART_LastBit));  // 检查最后一位时钟设置是否合法

	/*---------------------------- 配置 USART CR2 寄存器 -----------------------*/
	tmpreg = USARTx->CR2;  // 读取 CR2 寄存器的当前值

	tmpreg &= (uint32_t)~((uint32_t)CR2_CLOCK_CLEAR_MASK);  // 清除时钟设置相关的位：CLKEN、CPOL、CPHA、LBCL

	/* 配置时钟、时钟极性、时钟相位和最后一位时钟 */
	tmpreg |= (uint32_t)USART_ClockInitStruct->USART_Clock |  // 配置时钟源
		USART_ClockInitStruct->USART_CPOL |            // 配置时钟极性
		USART_ClockInitStruct->USART_CPHA |            // 配置时钟相位
		USART_ClockInitStruct->USART_LastBit;          // 配置最后一位时钟

	/* 写回配置到 CR2 寄存器 */
	USARTx->CR2 = (uint16_t)tmpreg;  // 更新 CR2 寄存器
}

/**
  * @brief  将每个 USART_ClockInitStruct 成员填充为默认值。
  * @param  USART_ClockInitStruct: 指向一个 USART_ClockInitTypeDef 结构体的指针，该结构体将被初始化。
  * @retval None
  *
  * 该函数将 USART 时钟配置结构体的成员初始化为默认值。结构体可以用来配置 USART 的时钟设置，包括时钟源、极性、相位等。
  */
void myUSART_ClockStructInit(USART_ClockInitTypeDef* USART_ClockInitStruct)
{
	/* 设置默认值 */
	USART_ClockInitStruct->USART_Clock = USART_Clock_Disable;  // 默认禁用时钟
	USART_ClockInitStruct->USART_CPOL = USART_CPOL_Low;  // 默认时钟极性为低电平
	USART_ClockInitStruct->USART_CPHA = USART_CPHA_1Edge;  // 默认时钟相位为第一个边沿
	USART_ClockInitStruct->USART_LastBit = USART_LastBit_Disable;  // 默认禁用最后一位时钟
}


/**
  * @brief  启用或禁用指定的 USART 外设。
  * @param  USARTx: 选择的 USART 外设，x 可以是 1、2、3、4、5、6、7 或 8。
  * @param  NewState: USART 外设的新状态。
  *          该参数可以是：ENABLE 或 DISABLE。
  * @retval None
  *
  * 该函数用于启用或禁用指定的 USART 外设。当启用时，USART 外设可以进行数据的发送和接收；当禁用时，USART 外设无法工作。
  */
void myUSART_Cmd(USART_TypeDef* USARTx, FunctionalState NewState)
{
	/* 检查参数 */
	assert_param(IS_USART_ALL_PERIPH(USARTx));  // 检查 USARTx 是否为有效外设
	assert_param(IS_FUNCTIONAL_STATE(NewState));  // 检查 NewState 是否为 ENABLE 或 DISABLE

	if (NewState != DISABLE)
	{
		USARTx->CR1 |= USART_CR1_UE;  // 启用 USART 外设
	}
	else
	{
		USARTx->CR1 &= (uint16_t)~USART_CR1_UE;  // 禁用 USART 外设
	}
}


/**
  * @brief  设置系统时钟分频器
  * @param  USARTx: 选择 USART 或 UART 外设，其中 x 可以是 1, 2, 3, 4, 5, 6, 7 或 8。
  * @param  USART_Prescaler: 设置时钟分频值。
  * @note   此函数用于 UART4 和 UART5 的 IrDA 模式下配置时钟分频。
  * @retval None
  *
  * 详细说明：
  * 该函数用于设置 USART 外设的时钟分频器。时钟分频器通常用于 IrDA 模式，调整 USART 外设的波特率。
  * 该功能通过修改 USART 的 GTPR（Guard Time and Prescaler Register）寄存器来实现。
  *
  * @note   在调用此函数之前，确保 USART 外设已经初始化并启用了时钟。
  */
void myUSART_SetPrescaler(USART_TypeDef* USARTx, uint8_t USART_Prescaler)
{
	/* 检查参数有效性 */
	assert_param(IS_USART_ALL_PERIPH(USARTx));

	/* 清除原有的 USART 时钟分频器设置 */
	USARTx->GTPR &= USART_GTPR_GT;

	/* 设置新的时钟分频值 */
	USARTx->GTPR |= USART_Prescaler;
}


/**
  * @brief  启用或禁用 USART 的 8x 超采样模式。
  * @note   必须在调用 USART_Init() 函数之前调用此函数，以确保正确的波特率分频值。
  * @param  USARTx: 选择 USART 或 UART 外设，x 可以是 1, 2, 3, 4, 5, 6, 7 或 8。
  * @param  NewState: USART 8x 超采样模式的新状态。
  *          此参数可以是: ENABLE 或 DISABLE。
  * @retval 无
  */
void myUSART_OverSampling8Cmd(USART_TypeDef* USARTx, FunctionalState NewState)
{
	/* 检查参数 */
	assert_param(IS_USART_ALL_PERIPH(USARTx));  // 确保选中的外设是有效的 USART 或 UART 外设
	assert_param(IS_FUNCTIONAL_STATE(NewState)); // 确保 NewState 参数有效

	// 使用三目运算符代替 if else，启用或禁用 8x 超采样模式
	(NewState != DISABLE) ? (USARTx->CR1 |= USART_CR1_OVER8) : (USARTx->CR1 &= (uint16_t)~((uint16_t)USART_CR1_OVER8));
}


/**
  * @brief  启用或禁用 USART 的单比特采样方法。
  * @param  USARTx: 选择 USART 或 UART 外设，x 可以是 1, 2, 3, 4, 5, 6, 7 或 8。
  * @param  NewState: USART 单比特采样方法的新状态。
  *          此参数可以是: ENABLE 或 DISABLE。
  * @retval 无
  */
void myUSART_OneBitMethodCmd(USART_TypeDef* USARTx, FunctionalState NewState)
{
	/* 检查参数 */
	assert_param(IS_USART_ALL_PERIPH(USARTx));  // 确保选中的外设是有效的 USART 或 UART 外设
	assert_param(IS_FUNCTIONAL_STATE(NewState)); // 确保 NewState 参数有效

	// 使用三目运算符代替 if else，启用或禁用单比特采样方法
	(NewState != DISABLE) ? (USARTx->CR3 |= USART_CR3_ONEBIT) : (USARTx->CR3 &= (uint16_t)~((uint16_t)USART_CR3_ONEBIT));
}



/*@verbatim
============================================================================== =
##### 数据传输函数 #####
============================================================================== =
[..]
本小节提供了一组函数，用于管理 USART 数据传输。
[..]
在 USART 接收过程中，数据从 RX 引脚按最低有效位优先的顺序移入。在此模式下，USART_DR 寄存器由一个缓冲区(RDR) 组成，位于内部总线和接收的移位寄存器之间。
[..]
当发生传输时，写入 USART_DR 寄存器的指令会将数据存储到 TDR 寄存器中，并且在当前传输结束时，数据会被复制到移位寄存器中。
[..]
通过 USART_ReceiveData() 函数可以读取 USART_DR 寄存器，并返回 RDR 缓冲区的值。而写入访问 USART_DR 则通过 USART_SendData() 函数进行，它将写入的数据存储到 TDR 缓冲区中。

@endverbatim
* @{
*/

/**
  * @brief  通过 USARTx 外设发送单个数据。
  * @param  USARTx: 选择 USART 或 UART 外设，x 可以是 1, 2, 3, 4, 5, 6, 7 或 8。
  * @param  Data: 要发送的数据。
  * @retval 无
  */
void myUSART_SendData(USART_TypeDef* USARTx, uint16_t Data)
{
	/* 检查参数 */
	assert_param(IS_USART_ALL_PERIPH(USARTx));  // 确保选中的外设是有效的 USART 或 UART 外设
	assert_param(IS_USART_DATA(Data)); // 确保数据符合 USART 规定的范围

	/* 发送数据 */
	USARTx->DR = (Data & (uint16_t)0x01FF); // 将数据写入数据寄存器，传输数据
}

/**
  * @brief  返回 USARTx 外设接收到的最新数据。
  * @param  USARTx: 选择 USART 或 UART 外设，x 可以是 1, 2, 3, 4, 5, 6, 7 或 8。
  * @retval 返回接收到的数据。
  */
uint16_t myUSART_ReceiveData(USART_TypeDef* USARTx)
{
	/* 检查参数 */
	assert_param(IS_USART_ALL_PERIPH(USARTx));  // 确保选中的外设是有效的 USART 或 UART 外设

	/* 接收数据 */
	return (uint16_t)(USARTx->DR & (uint16_t)0x01FF); // 从数据寄存器中读取接收到的数据
}



/**
  * @}
  */

  /** @defgroup USART_Group3 多处理器通信功能
	* @brief 多处理器通信功能
	*
	@verbatim
   ===============================================================================
				##### 多处理器通信功能 #####
   ===============================================================================
	  [..]
	  本小节提供了一组函数，用于管理 USART 的多处理器通信。
	  [..]
	  例如，其中一个 USART 可以作为主设备，其 TX 输出连接到另一个 USART 的 RX 输入。其他的 USART 则为从设备，它们各自的 TX 输出通过逻辑与操作（AND）连接到主设备的 RX 输入。
	  [..]
	  USART 多处理器通信的操作步骤如下：
		(#) 使用 USART_Init() 函数配置波特率、数据位长度（9 位）、停止位、奇偶校验、工作模式（发送模式或接收模式）以及硬件流控制设置。
		(#) 使用 USART_SetAddress() 函数配置 USART 地址。
		(#) 为从设备配置唤醒方法（USART_WakeUp_IdleLine 或 USART_WakeUp_AddressMark），使用 USART_WakeUpConfig() 函数。
		(#) 使用 USART_Cmd() 函数使能 USART。
		(#) 使用 USART_ReceiverWakeUpCmd() 函数将 USART 从设备置于静默模式。
	  [..]
	  当从设备接收到唤醒条件时，它将退出静默模式。

	@endverbatim
	* @{
	*/

	/**
	  * @brief  设置 USART 节点的地址。
	  * @param  USARTx: 选择 USART 或 UART 外设，x 可以是 1, 2, 3, 4, 5, 6, 7 或 8。
	  * @param  USART_Address: 指定 USART 节点的地址，取值范围为 0 到 15。
	  * @retval 无
	  *
	  * 该函数用于为 USART 外设设置一个唯一的地址。该地址可以用于多处理器通信模式，
	  * 使得多个 USART 设备能够相互识别。设置后，USART 节点将以该地址进行通信。
	  *
	  * 使用步骤：
	  * 1. 首先，检查 USART 外设是否有效。
	  * 2. 清除 USART CR2 寄存器中的地址设置位。
	  * 3. 使用给定的地址值更新 USART 的地址。
	  */
void myUSART_SetAddress(USART_TypeDef* USARTx, uint8_t USART_Address)
{
	/* 检查参数 */
	assert_param(IS_USART_ALL_PERIPH(USARTx));   // 检查 USART 外设是否有效
	assert_param(IS_USART_ADDRESS(USART_Address)); // 检查地址值是否合法

	/* 清除当前的 USART 地址 */
	USARTx->CR2 &= (uint16_t)~((uint16_t)USART_CR2_ADD);

	/* 设置新的 USART 节点地址 */
	USARTx->CR2 |= USART_Address;
}


/**
  * @brief  判断 USART 是否处于静默模式。
  * @param  USARTx: 选择 USART 或 UART 外设，其中 x 可以是 1, 2, 3, 4, 5, 6, 7 或 8。
  * @param  NewState: USART 静默模式的新状态。
  *          该参数可以是：ENABLE 或 DISABLE。
  * @retval 无
  */
void myUSART_ReceiverWakeUpCmd(USART_TypeDef* USARTx, FunctionalState NewState)
{
	/* 检查参数 */
	assert_param(IS_USART_ALL_PERIPH(USARTx));
	assert_param(IS_FUNCTIONAL_STATE(NewState));

	/* 使用三目运算符判断 NewState 是否为 ENABLE，启用或禁用静默模式 */
	(NewState != DISABLE) ? (USARTx->CR1 |= USART_CR1_RWU) : (USARTx->CR1 &= (uint16_t)~USART_CR1_RWU);
}


/**
  * @brief  选择 USART 唤醒方法。
  * @param  USARTx: 选择 USART 或 UART 外设，其中 x 可以是 1, 2, 3, 4, 5, 6, 7 或 8。
  * @param  USART_WakeUp: 指定 USART 唤醒方法。
  *          该参数可以是以下值之一：
  *            @arg USART_WakeUp_IdleLine: 通过空闲线路检测唤醒
  *            @arg USART_WakeUp_AddressMark: 通过地址标志唤醒
  * @retval 无
  *
  * @note 该函数会先清除 USART_CR1 寄存器中的唤醒控制位（USART_CR1_WAKE），
  * 然后根据传入的 `USART_WakeUp` 参数设置正确的唤醒方法。
  */
void myUSART_WakeUpConfig(USART_TypeDef* USARTx, uint16_t USART_WakeUp)
{
	/* 检查参数 */
	assert_param(IS_USART_ALL_PERIPH(USARTx));        // 检查 USART 外设参数是否有效
	assert_param(IS_USART_WAKEUP(USART_WakeUp));      // 检查唤醒方法是否有效

	/* 清除唤醒控制位 */
	USARTx->CR1 &= (uint16_t)~((uint16_t)USART_CR1_WAKE);
	/* 根据指定的唤醒方法设置唤醒控制位 */
	USARTx->CR1 |= USART_WakeUp;
}


/**
  * @defgroup USART_Group4 LIN 模式函数
  * @brief   LIN 模式函数
  *
  @verbatim
 ===============================================================================
						##### LIN 模式函数 #####
 ===============================================================================
	[..]
	本小节提供了一组函数，用于管理 USART LIN 模式通信。
	[..]
	在 LIN 模式下，按照 LIN 标准，要求使用 8 位数据格式和 1 个停止位。
	[..]
	USART IP 仅支持此 LIN 功能：
	  (+) LIN 主机同步断开发送功能和 LIN 从机断开检测功能：
		  支持 13 位断开生成和 10/11 位断开检测。

	[..]
	USART LIN 主机发送通信的步骤如下：
	  (#) 使用 USART_Init() 函数设置波特率、数据位长度 = 8 位、停止位 = 1 位、校验位、发送/接收模式和硬件流控制。
	  (#) 使用 USART_Cmd() 函数启用 USART。
	  (#) 使用 USART_LINCmd() 函数启用 LIN 模式。
	  (#) 使用 USART_SendBreak() 函数发送断开字符。
	[..]
	USART LIN 主机接收通信的步骤如下：
	  (#) 使用 USART_Init() 函数设置波特率、数据位长度 = 8 位、停止位 = 1 位、校验位、发送/接收模式和硬件流控制。
	  (#) 使用 USART_Cmd() 函数启用 USART。
	  (#) 使用 USART_LINBreakDetectLengthConfig() 函数配置断开检测长度。
	  (#) 使用 USART_LINCmd() 函数启用 LIN 模式。

	  -@- 在 LIN 模式下，以下位必须保持清除：
	   (+@) USART_CR2 寄存器中的 CLKEN 位，
	   (+@) USART_CR3 寄存器中的 STOP[1:0]、SCEN、HDSEL 和 IREN 位。
  @endverbatim
  * @{
  */

  /**
	* @brief  设置 USART 的 LIN 断开检测长度。
	* @param  USARTx: 选择 USART 或 UART 外设，其中 x 可以是 1, 2, 3, 4, 5, 6, 7 或 8。
	* @param  USART_LINBreakDetectLength: 指定 LIN 断开检测长度。
	*          该参数可以是以下值之一：
	*            @arg USART_LINBreakDetectLength_10b: 10 位断开检测
	*            @arg USART_LINBreakDetectLength_11b: 11 位断开检测
	* @retval 无返回值
	*/
void myUSART_LINBreakDetectLengthConfig(USART_TypeDef* USARTx, uint16_t USART_LINBreakDetectLength)
{
	/* 检查参数 */
	assert_param(IS_USART_ALL_PERIPH(USARTx));  // 确保 USARTx 是有效的 USART 外设
	assert_param(IS_USART_LIN_BREAK_DETECT_LENGTH(USART_LINBreakDetectLength));  // 确保传入的断开检测长度是有效的

	/* 清除 LIN 断开检测长度位 */
	USARTx->CR2 &= (uint16_t)~((uint16_t)USART_CR2_LBDL);

	/* 设置 LIN 断开检测长度 */
	USARTx->CR2 |= USART_LINBreakDetectLength;
}

/**
  * @brief  启用或禁用 USART 的 LIN 模式。
  * @param  USARTx: 选择 USART 或 UART 外设，其中 x 可以是 1, 2, 3, 4, 5, 6, 7 或 8。
  * @param  NewState: USART LIN 模式的新状态。
  *          该参数可以是：ENABLE 或 DISABLE。
  * @retval 无返回值
  */
void myUSART_LINCmd(USART_TypeDef* USARTx, FunctionalState NewState)
{
	/* 检查参数 */
	assert_param(IS_USART_ALL_PERIPH(USARTx));  // 确保 USARTx 是有效的 USART 外设
	assert_param(IS_FUNCTIONAL_STATE(NewState));  // 确保传入的状态有效

	/* 使用三目运算符来判断启用或禁用 LIN 模式 */
	NewState != DISABLE ? (USARTx->CR2 |= USART_CR2_LINEN) : (USARTx->CR2 &= (uint16_t)~((uint16_t)USART_CR2_LINEN));
}

/**
  * @brief  发送断开字符。
  * @param  USARTx: 选择 USART 或 UART 外设，其中 x 可以是 1, 2, 3, 4, 5, 6, 7 或 8。
  * @retval 无返回值
  */
void myUSART_SendBreak(USART_TypeDef* USARTx)
{
	/* 检查参数 */
	assert_param(IS_USART_ALL_PERIPH(USARTx));  // 确保 USARTx 是有效的 USART 外设

	/* 发送断开字符 */
	USARTx->CR1 |= USART_CR1_SBK;
}

/**
  * @}
  */

  /** @defgroup USART_Group5 半双工模式功能
   *  @brief   半双工模式功能
   *
  @verbatim
   ===============================================================================
					  ##### 半双工模式功能 #####
   ===============================================================================
	  [..]
	  本小节提供了一组函数，用于管理 USART 半双工通信。
	  [..]
	  USART 可以配置为单线半双工协议，其中 TX 和 RX 引脚内部连接。
	  [..]
	  USART 半双工通信可以通过以下过程实现：
		(#) 使用 USART_Init() 函数设置波特率、数据位长度、停止位、校验位、模式（发送或接收）和硬件流控制。
		(#) 使用 USART_SetAddress() 函数配置 USART 地址。
		(#) 使用 USART_Cmd() 函数启用 USART。
		(#) 使用 USART_HalfDuplexCmd() 函数启用半双工模式。

	  -@- 在半双工模式下，RX 引脚不再使用。
	  -@- 在半双工模式下，必须保持以下位为清除状态：
		(+@) USART_CR2 寄存器中的 LINEN 和 CLKEN 位。
		(+@) USART_CR3 寄存器中的 SCEN 和 IREN 位。

  @endverbatim
	* @{
	*/

	/**
	  * @brief  启用或禁用 USART 的半双工通信。
	  * @param  USARTx: 选择 USART 或 UART 外设，其中 x 可以是 1, 2, 3, 4, 5, 6, 7 或 8。
	  * @param  NewState: USART 通信的新状态。
	  *          该参数可以是：ENABLE（启用）或 DISABLE（禁用）。
	  * @retval 无返回值
	  *
	  * @note   此函数通过设置或清除 USART 的 CR3 寄存器中的 HDSEL 位来启用或禁用半双工模式。
	  */
void myUSART_HalfDuplexCmd(USART_TypeDef* USARTx, FunctionalState NewState)
{
	/* 检查输入的参数是否合法 */
	assert_param(IS_USART_ALL_PERIPH(USARTx));  // 检查 USARTx 是否是有效的外设
	assert_param(IS_FUNCTIONAL_STATE(NewState)); // 检查 NewState 是否是 ENABLE 或 DISABLE

	// 根据传入的状态启用或禁用半双工模式
	NewState != DISABLE ? (USARTx->CR3 |= USART_CR3_HDSEL) : (USARTx->CR3 &= (uint16_t)~((uint16_t)USART_CR3_HDSEL));
}


/**
  * @defgroup USART_Group6 智能卡模式函数
  * @brief   智能卡模式函数
  *
@verbatim
 ===============================================================================
							##### 智能卡模式函数 #####
 ===============================================================================
	[..]
	本小节提供了一组函数，用于管理 USART 智能卡通信。
	[..]
	智能卡接口旨在支持 ISO 7816-3 标准定义的异步协议智能卡。
	[..]
	USART 可以通过 SCLK 输出为智能卡提供时钟。在智能卡模式下，SCLK 与通信无关，仅通过 5 位预分频器从内部外设输入时钟派生。
	[..]
	智能卡通信的配置流程如下：
	  (#) 使用 USART_SetPrescaler() 函数配置智能卡的预分频器。
	  (#) 使用 USART_SetGuardTime() 函数配置智能卡的保护时间。
	  (#) 使用 USART_ClockInit() 函数配置 USART 时钟，具体如下：
		(++) 启用 USART 时钟
		(++) USART 时钟极性（CPOL）设置为低
		(++) USART 时钟相位（CPHA）设置为第一边沿
		(++) 启用 USART 最后一位时钟
	  (#) 使用 USART_Init() 函数配置智能卡接口，具体如下：
		(++) 字长 = 9 位
		(++) 停止位 = 1.5 位
		(++) 偶校验位
		(++) 波特率 = 12096 波特
		(++) 禁用硬件流控（RTS 和 CTS 信号）
		(++) 启用 Tx 和 Rx
	  (#) 可选：使用 USART_ITConfig() 函数启用校验错误中断。
	  (#) 启用 USART 使用 USART_Cmd() 函数。
	  (#) 启用智能卡 NACK 使用 USART_SmartCardNACKCmd() 函数。
	  (#) 启用智能卡接口使用 USART_SmartCardCmd() 函数。

	更多细节请参见 ISO 7816-3 规范。

	  -@- 也可以选择 0.5 停止位接收，但建议使用 1.5 停止位进行传输和接收，以避免在两种配置之间切换。
	  -@- 在智能卡模式下，以下位需要保持清除：
		(+@) USART_CR2 寄存器中的 LINEN 位
		(+@) USART_CR3 寄存器中的 HDSEL 和 IREN 位
	  -@- 智能卡模式仅在 USART 外设上可用（UART4 和 UART5 外设不支持）。
@endverbatim
*/

/**
  * @brief  设置指定的 USART 防护时间。
  * @param  USARTx: 选择 USART 或 UART 外设，x 可以是 1, 2, 3 或 6。
  *         例如，可以选择 USART1、USART2、USART3、USART6。
  * @param  USART_GuardTime: 指定防护时间。防护时间用来控制接收数据之间的间隔时间。
  *         该值的单位通常是 USART 的时钟周期。
  * @retval 无
  *
  * @note
  * 在智能卡通信中，防护时间（Guard Time）用于确保接收到的数据与后续的命令或数据之间有足够的间隔。
  * 如果你正在配置智能卡通信模式，防护时间非常重要。
  *
  * 详细过程：
  * - 首先清除原有的防护时间设置，以确保不影响新的配置。
  * - 然后根据传入的 `USART_GuardTime` 设置新的防护时间。
  */
void myUSART_SetGuardTime(USART_TypeDef* USARTx, uint8_t USART_GuardTime)
{
	/* 检查 USARTx 是否是有效的 USART 外设，确保选择了正确的外设 */
	assert_param(IS_USART_1236_PERIPH(USARTx));

	/* 清除原有的防护时间设置（只保留预分频器部分） */
	USARTx->GTPR &= USART_GTPR_PSC;

	/* 将新的防护时间值设置到 GTPR 寄存器 */
	USARTx->GTPR |= (uint16_t)((uint16_t)USART_GuardTime << 0x08);
}

/**
  * @brief  启用或禁用 USART 的智能卡模式。
  * @param  USARTx: 选择 USART 或 UART 外设，x 可以是 1, 2, 3 或 6。
  *         例如，可以选择 USART1、USART2、USART3、USART6。
  * @param  NewState: 设置智能卡模式的新状态。
  *          该参数可以是：ENABLE（启用智能卡模式）或 DISABLE（禁用智能卡模式）。
  * @retval 无
  *
  * @note
  * 智能卡模式是为与符合 ISO 7816-3 标准的智能卡进行通信而设计的。在此模式下，USART 提供一个时钟信号，
  * 用于驱动智能卡的时序。
  *
  * 详细过程：
  * - 如果 `NewState` 为 ENABLE，则启用智能卡模式，将 `SCEN` 位设置为 1。
  * - 如果 `NewState` 为 DISABLE，则禁用智能卡模式，将 `SCEN` 位清除为 0。
  */
void myUSART_SmartCardCmd(USART_TypeDef* USARTx, FunctionalState NewState)
{
	/* 检查 USARTx 是否是有效的 USART 外设 */
	assert_param(IS_USART_1236_PERIPH(USARTx));
	/* 检查 NewState 是否为有效的功能状态（ENABLE 或 DISABLE） */
	assert_param(IS_FUNCTIONAL_STATE(NewState));

	/* 启用或禁用智能卡模式 */
	(NewState != DISABLE) ? (USARTx->CR3 |= USART_CR3_SCEN) : (USARTx->CR3 &= (uint16_t)~((uint16_t)USART_CR3_SCEN));
}

/**
  * @brief  启用或禁用智能卡的 NACK（负确认）传输功能。
  * @param  USARTx: 选择 USART 或 UART 外设，x 可以是 1, 2, 3 或 6。
  *         例如，可以选择 USART1、USART2、USART3、USART6。
  * @param  NewState: 设置 NACK 传输的新状态。
  *          该参数可以是：ENABLE（启用 NACK 传输）或 DISABLE（禁用 NACK 传输）。
  * @retval 无
  *
  * @note
  * 在智能卡通信中，NACK（负确认）用于告诉通信方某些操作失败或没有成功。
  * 启用 NACK 传输时，USART 会在通信中插入一个负确认响应，表示操作没有成功。
  *
  * 详细过程：
  * - 如果 `NewState` 为 ENABLE，则启用 NACK 传输，将 `NACK` 位设置为 1。
  * - 如果 `NewState` 为 DISABLE，则禁用 NACK 传输，将 `NACK` 位清除为 0。
  */
void myUSART_SmartCardNACKCmd(USART_TypeDef* USARTx, FunctionalState NewState)
{
	/* 检查 USARTx 是否是有效的 USART 外设 */
	assert_param(IS_USART_1236_PERIPH(USARTx));
	/* 检查 NewState 是否为有效的功能状态（ENABLE 或 DISABLE） */
	assert_param(IS_FUNCTIONAL_STATE(NewState));

	/* 启用或禁用 NACK 传输 */
	(NewState != DISABLE) ? (USARTx->CR3 |= USART_CR3_NACK) : (USARTx->CR3 &= (uint16_t)~((uint16_t)USART_CR3_NACK));
}


/** @defgroup USART_Group7 IrDA模式函数
 *  @brief   IrDA模式函数
 *
@verbatim
 ===============================================================================
						##### IrDA模式函数 #####
 ===============================================================================
	[..]
	本小节提供了一组函数，用于管理USART IrDA通信。
	[..]
	IrDA是一种半双工通信协议。如果发送器忙碌，IrDA接收线上任何数据将被IrDA解码器忽略；如果接收器忙碌，USART发送到IrDA的数据将不会被IrDA编码。
	在接收数据时，应避免同时传输数据，因为传输中的数据可能会被破坏。
	[..]
	IrDA通信的配置步骤：
	  (#) 使用USART_Init()函数配置波特率、数据位长度 = 8 位、停止位、奇偶校验、传输/接收模式和硬件流控制等参数。
	  (#) 使用USART_Cmd()函数启用USART外设。
	  (#) 使用USART_SetPrescaler()函数配置IrDA脉冲宽度。
	  (#) 使用USART_IrDAConfig()函数配置IrDA模式，可以选择USART_IrDAMode_LowPower或USART_IrDAMode_Normal模式。
	  (#) 使用USART_IrDACmd()函数启用IrDA。

	  -@- 脉冲宽度小于两个且大于一个PSC周期的脉冲可能会被拒绝。
	  -@- 接收端的设置时间应由软件管理。IrDA物理层规范要求传输和接收之间有至少10毫秒的延迟（因为IrDA是半双工协议）。
	  -@- 在IrDA模式下，以下位必须保持清除：
		(+@) USART_CR2寄存器中的LINEN、STOP和CLKEN位。
		(+@) USART_CR3寄存器中的SCEN和HDSEL位。

@endverbatim
  * @{
  */

  /**
	* @brief  配置USART的IrDA接口模式。
	* @param  USARTx: 选择USART或UART外设的编号，x可以是1, 2, 3, 4, 5, 6, 7或8。
	* @param  USART_IrDAMode: 指定IrDA模式。
	*          该参数可以是以下值之一：
	*            @arg USART_IrDAMode_LowPower: 低功耗模式
	*            @arg USART_IrDAMode_Normal: 普通模式
	* @retval None
	*/
void myUSART_IrDAConfig(USART_TypeDef* USARTx, uint16_t USART_IrDAMode)
{
	/* 检查参数 */
	assert_param(IS_USART_ALL_PERIPH(USARTx)); // 检查是否是有效的USART外设
	assert_param(IS_USART_IRDA_MODE(USART_IrDAMode)); // 检查IrDA模式是否有效

	/* 清除CR3寄存器中的IRLP位，准备设置IrDA模式 */
	USARTx->CR3 &= (uint16_t)~((uint16_t)USART_CR3_IRLP);

	/* 设置IrDA模式 */
	USARTx->CR3 |= USART_IrDAMode; // 根据传入的模式值配置IrDA模式
}

/**
  * @brief  启用或禁用USART的IrDA接口。
  * @param  USARTx: 选择USART或UART外设的编号，x可以是1, 2, 3, 4, 5, 6, 7或8。
  * @param  NewState: IrDA模式的新状态。
  *          该参数可以是：ENABLE或DISABLE。
  * @retval None
  */
void myUSART_IrDACmd(USART_TypeDef* USARTx, FunctionalState NewState)
{
	/* 检查参数 */
	assert_param(IS_USART_ALL_PERIPH(USARTx)); // 检查是否是有效的USART外设
	assert_param(IS_FUNCTIONAL_STATE(NewState)); // 检查状态是否有效（ENABLE或DISABLE）

	/* 使用三目运算符代替if-else */
	(NewState != DISABLE) ? (USARTx->CR3 |= USART_CR3_IREN) : (USARTx->CR3 &= (uint16_t)~((uint16_t)USART_CR3_IREN));
	// 如果NewState为ENABLE，则设置IREN位启用IrDA模式；否则清除IREN位禁用IrDA模式
}

/**
  * @}
  */
  /** @defgroup USART_Group8 DMA传输管理函数
   *  @brief   DMA传输管理函数
   *
  @verbatim
   ===============================================================================
				##### DMA传输管理函数 #####
   ===============================================================================

  @endverbatim
	* @{
	*/
	/**
	  * @brief  启用或禁用USART的DMA接口。
	  * @param  USARTx: 选择USART或UART外设，可以是1, 2, 3, 4, 5, 6, 7或8。
	  * @param  USART_DMAReq: 指定DMA请求。
	  *          该参数可以是以下任意组合：
	  *            @arg USART_DMAReq_Tx: USART DMA发送请求
	  *            @arg USART_DMAReq_Rx: USART DMA接收请求
	  * @param  NewState: DMA请求源的新状态。
	  *          该参数可以是：ENABLE或DISABLE。
	  * @retval 无返回值
	  */
void myUSART_DMACmd(USART_TypeDef* USARTx, uint16_t USART_DMAReq, FunctionalState NewState)
{
	/* 检查参数是否合法 */
	assert_param(IS_USART_ALL_PERIPH(USARTx));  // 检查USART外设是否合法
	assert_param(IS_USART_DMAREQ(USART_DMAReq));  // 检查DMA请求是否合法
	assert_param(IS_FUNCTIONAL_STATE(NewState));  // 检查功能状态是否合法

	/* 使用三目运算符根据NewState的状态决定启用或禁用DMA */
	(NewState != DISABLE) ? (USARTx->CR3 |= USART_DMAReq)  // 如果启用DMA请求，设置CR3寄存器的DMAT和/或DMAR位
		: (USARTx->CR3 &= (uint16_t)~USART_DMAReq);  // 如果禁用DMA请求，清除CR3寄存器的DMAT和/或DMAR位
}

/**
  * @}
  */


  /** @defgroup USART_Group9 中断和标志管理函数
   *  @brief   中断和标志管理函数
   *
  @verbatim
   ===============================================================================
			  ##### 中断和标志管理函数 #####
   ===============================================================================
	  [..]
	  本小节提供了一组函数，用于配置USART的中断源、DMA通道请求，并检查或清除标志或挂起位的状态。
	  用户应根据其应用程序选择合适的工作模式：轮询模式、中断模式或DMA模式。

	  *** 轮询模式 ***
	  ====================
	  [..]
	  在轮询模式下，USART通信可以通过10个标志来管理：
		(#) USART_FLAG_TXE : 指示发送缓冲区寄存器的状态
		(#) USART_FLAG_RXNE : 指示接收缓冲区寄存器的状态
		(#) USART_FLAG_TC : 指示发送操作的状态
		(#) USART_FLAG_IDLE : 指示空闲线的状态
		(#) USART_FLAG_CTS : 指示nCTS输入的状态
		(#) USART_FLAG_LBD : 指示LIN断开检测的状态
		(#) USART_FLAG_NE : 指示是否发生噪声错误
		(#) USART_FLAG_FE : 指示是否发生帧错误
		(#) USART_FLAG_PE : 指示是否发生奇偶校验错误
		(#) USART_FLAG_ORE : 指示是否发生溢出错误
	  [..]
	  在此模式下，建议使用以下函数：
		(+) FlagStatus USART_GetFlagStatus(USART_TypeDef* USARTx, uint16_t USART_FLAG);
		(+) void USART_ClearFlag(USART_TypeDef* USARTx, uint16_t USART_FLAG);

	  *** 中断模式 ***
	  ======================
	  [..]
	  在中断模式下，USART通信可以通过8个中断源和10个挂起位进行管理：

		(#) 挂起位：

		  (##) USART_IT_TXE : 指示发送缓冲区寄存器的状态
		  (##) USART_IT_RXNE : 指示接收缓冲区寄存器的状态
		  (##) USART_IT_TC : 指示发送操作的状态
		  (##) USART_IT_IDLE : 指示空闲线的状态
		  (##) USART_IT_CTS : 指示nCTS输入的状态
		  (##) USART_IT_LBD : 指示LIN断开检测的状态
		  (##) USART_IT_NE : 指示是否发生噪声错误
		  (##) USART_IT_FE : 指示是否发生帧错误
		  (##) USART_IT_PE : 指示是否发生奇偶校验错误
		  (##) USART_IT_ORE : 指示是否发生溢出错误

		(#) 中断源：

		  (##) USART_IT_TXE : 指定Tx缓冲区空中断源
		  (##) USART_IT_RXNE : 指定Rx缓冲区非空中断源
		  (##) USART_IT_TC : 指定传输完成中断源
		  (##) USART_IT_IDLE : 指定空闲线中断源
		  (##) USART_IT_CTS : 指定CTS中断源
		  (##) USART_IT_LBD : 指定LIN断开检测中断源
		  (##) USART_IT_PE : 指定奇偶校验错误中断源
		  (##) USART_IT_ERR :  指定错误中断源

		-@@- 一些参数被编码，以便作为中断源或挂起位使用。
	  [..]
	  在此模式下，建议使用以下函数：
		(+) void USART_ITConfig(USART_TypeDef* USARTx, uint16_t USART_IT, FunctionalState NewState);
		(+) ITStatus USART_GetITStatus(USART_TypeDef* USARTx, uint16_t USART_IT);
		(+) void USART_ClearITPendingBit(USART_TypeDef* USARTx, uint16_t USART_IT);

	  *** DMA模式 ***
	  ================
	  [..]
	  在DMA模式下，USART通信可以通过2个DMA通道请求来管理：
		(#) USART_DMAReq_Tx: 指定Tx缓冲区DMA传输请求
		(#) USART_DMAReq_Rx: 指定Rx缓冲区DMA传输请求
	  [..]
	  在此模式下，建议使用以下函数：
		(+) void USART_DMACmd(USART_TypeDef* USARTx, uint16_t USART_DMAReq, FunctionalState NewState);

  @endverbatim
	* @{
	*/

	/**
	  * @brief  启用或禁用指定的 USART 中断。
	  * @param  USARTx: 选择 USART 或 UART 外设，其中 x 可以是 1、2、3、4、5、6、7 或 8。
	  * @param  USART_IT: 指定要启用或禁用的 USART 中断源。
	  *          该参数可以是以下值之一：
	  *            @arg USART_IT_CTS:  CTS 变化中断
	  *            @arg USART_IT_LBD:  LIN 中断
	  *            @arg USART_IT_TXE:  发送数据寄存器为空中断
	  *            @arg USART_IT_TC:   传输完成中断
	  *            @arg USART_IT_RXNE: 接收数据寄存器不为空中断
	  *            @arg USART_IT_IDLE: 空闲线路检测中断
	  *            @arg USART_IT_PE:   奇偶校验错误中断
	  *            @arg USART_IT_ERR:  错误中断（帧错误、噪声错误、溢出错误）
	  * @param  NewState: 指定的 USARTx 中断的新状态。
	  *          该参数可以是：ENABLE 或 DISABLE。
	  * @retval 无
	  */
// 在头文件或文件顶部定义常量数组
const uint32_t reg_offsets[] = { 0x0C, 0x10, 0x14 };

void myUSART_ITConfig(USART_TypeDef* USARTx, uint16_t USART_IT, FunctionalState NewState)
{
    uint32_t usartreg = 0x00, itpos = 0x00, itmask = 0x00;
    uint32_t usartxbase = 0x00;

    /* 检查参数 */
    assert_param(IS_USART_ALL_PERIPH(USARTx));  // 检查 USART 外设是否有效
    assert_param(IS_USART_CONFIG_IT(USART_IT)); // 检查中断源是否有效
    assert_param(IS_FUNCTIONAL_STATE(NewState)); // 检查新状态是否有效

    /* 如果是 CTS 中断，检查所选外设是否支持 CTS */
    if (USART_IT == USART_IT_CTS)
    {
        assert_param(IS_USART_1236_PERIPH(USARTx));  // 仅 USART1、USART2、USART3 和 USART6 支持 CTS
    }

    usartxbase = (uint32_t)USARTx;

    /* 获取中断所在的寄存器索引（CR1: 0x01, CR2: 0x02, CR3: 0x03） */
    usartreg = (((uint8_t)USART_IT) >> 0x05);

    /* 获取中断位的位置并生成中断掩码 */
    itpos = USART_IT & IT_MASK;  // 获取中断位的位置
    itmask = (((uint32_t)0x01) << itpos);  // 生成中断掩码

    /* 使用 switch-case 选择中断所在的寄存器，并计算寄存器地址 */
    switch (usartreg)
    {
        case 0x01: /* 中断位在 CR1 寄存器 */
            usartxbase += 0x0C;  // CR1 寄存器的偏移地址
            break;
        case 0x02: /* 中断位在 CR2 寄存器 */
            usartxbase += 0x10;  // CR2 寄存器的偏移地址
            break;
        case 0x03: /* 中断位在 CR3 寄存器 */
            usartxbase += 0x14;  // CR3 寄存器的偏移地址
            break;
        default:
            break;  // 如果不匹配，什么也不做
    }

    /* 启用或禁用中断 */
    if (NewState != DISABLE)
    {
        *(__IO uint32_t*)usartxbase |= itmask;  // 启用中断
    }
    else
    {
        *(__IO uint32_t*)usartxbase &= ~itmask; // 禁用中断
    }
}



/**
  * @brief  检查指定的 USART 标志是否已设置。
  * @param  USARTx: 选择要检查的 USART 外设，可以是 1、2、3、4、5、6、7 或 8。
  * @param  USART_FLAG: 要检查的标志，可以是以下值之一：
  *          @arg USART_FLAG_CTS:  CTS 变化标志（对于 UART4 和 UART5 不可用）
  *          @arg USART_FLAG_LBD:  LIN 中断标志
  *          @arg USART_FLAG_TXE:  发送数据寄存器空标志
  *          @arg USART_FLAG_TC:   传输完成标志
  *          @arg USART_FLAG_RXNE: 接收数据寄存器非空标志
  *          @arg USART_FLAG_IDLE: 空闲线检测标志
  *          @arg USART_FLAG_ORE:  接收溢出错误标志
  *          @arg USART_FLAG_NE:   噪声错误标志
  *          @arg USART_FLAG_FE:   帧错误标志
  *          @arg USART_FLAG_PE:   奇偶校验错误标志
  * @retval 该标志的状态，SET 或 RESET。
  */
FlagStatus myUSART_GetFlagStatus(USART_TypeDef* USARTx, uint16_t USART_FLAG)
{
	// 确保参数有效性
	assert_param(IS_USART_ALL_PERIPH(USARTx));
	assert_param(IS_USART_FLAG(USART_FLAG));

	// 如果检查的是 CTS 标志，确保 USART 是可用的（不能是 UART4 或 UART5）
	if (USART_FLAG == USART_FLAG_CTS)
	{
		assert_param(IS_USART_1236_PERIPH(USARTx)); // 只有 USART1、USART2、USART3、USART6 支持 CTS
	}

	// 直接检查标志并返回结果
	return ((USARTx->SR & USART_FLAG) != RESET) ? SET : RESET;
}


/**
  * @brief  清除指定的 USART 标志。
  * @param  USARTx: 选择要清除的 USART 外设，可以是 1、2、3、4、5、6、7 或 8。
  * @param  USART_FLAG: 要清除的标志，可以是以下值之一：
  *          @arg USART_FLAG_CTS:  CTS 状态变化标志（对于 UART4 和 UART5 不可用）
  *          @arg USART_FLAG_LBD:  LIN 中断标志
  *          @arg USART_FLAG_TC:   传输完成标志
  *          @arg USART_FLAG_RXNE: 接收数据寄存器非空标志
  * @note   PE（奇偶校验错误）、FE（帧错误）、NE（噪声错误）、ORE（接收溢出错误）和 IDLE（空闲线检测）标志
  *          必须通过软件序列进行清除：先读取 USART_SR 寄存器（USART_GetFlagStatus），然后再读取
  *          USART_DR 寄存器（USART_ReceiveData）。
  * @note   RXNE 标志也可以通过读取 USART_DR 寄存器来清除（USART_ReceiveData）。
  * @note   TC 标志可以通过软件序列清除：先读取 USART_SR 寄存器（USART_GetFlagStatus），然后
  *          写入数据到 USART_DR 寄存器（USART_SendData）。
  * @note   TXE 标志只能通过写入数据到 USART_DR 寄存器来清除（USART_SendData）。
  * @retval None
  */
void myUSART_ClearFlag(USART_TypeDef* USARTx, uint16_t USART_FLAG)
{
	// 确保传入的 USART 外设有效
	assert_param(IS_USART_ALL_PERIPH(USARTx));

	// 确保要清除的标志有效
	assert_param(IS_USART_CLEAR_FLAG(USART_FLAG));

	// 如果清除的是 CTS 标志，确保外设支持此标志
	if ((USART_FLAG & USART_FLAG_CTS) == USART_FLAG_CTS)
	{
		assert_param(IS_USART_1236_PERIPH(USARTx));  // 仅 USART1, USART2, USART3, USART6 支持 CTS
	}

	// 清除指定的标志，通过将标志位清零
	USARTx->SR = (uint16_t)~USART_FLAG;
}


/**
  * @brief  检查指定的 USART 中断是否已经发生。
  * @param  USARTx: 选择要检查的 USART 外设，可以是 1、2、3、4、5、6、7 或 8。
  * @param  USART_IT: 要检查的 USART 中断源，可以是以下值之一：
  *          @arg USART_IT_CTS:  CTS 状态变化中断（对于 UART4 和 UART5 不可用）
  *          @arg USART_IT_LBD:  LIN 中断检测中断
  *          @arg USART_IT_TXE:  发送数据寄存器空中断
  *          @arg USART_IT_TC:   传输完成中断
  *          @arg USART_IT_RXNE: 接收数据寄存器非空中断
  *          @arg USART_IT_IDLE: 空闲线检测中断
  *          @arg USART_IT_ORE_RX : 接收溢出错误中断（如果 RXNEIE 位设置）
  *          @arg USART_IT_ORE_ER : 接收溢出错误中断（如果 EIE 位设置）
  *          @arg USART_IT_NE:   噪声错误中断
  *          @arg USART_IT_FE:   帧错误中断
  *          @arg USART_IT_PE:   奇偶校验错误中断
  * @retval USART_IT 状态（SET 或 RESET）。
  */
ITStatus myUSART_GetITStatus(USART_TypeDef* USARTx, uint16_t USART_IT)
{
    uint32_t bitpos = 0x00;   // 存储中断标志位的位置
    uint32_t itmask = 0x00;   // 存储中断掩码
    uint32_t usartreg = 0x00; // 存储中断所在的寄存器（CR1, CR2, CR3）
    ITStatus bitstatus = RESET; // 默认中断状态为RESET（未设置）
    
    // 参数校验，检查USART外设有效性和中断源有效性
    assert_param(IS_USART_ALL_PERIPH(USARTx));   
    assert_param(IS_USART_GET_IT(USART_IT));

    // 如果是CTS中断，验证所选择的USART是否为支持CTS的USART（即USART1, USART2, USART3, USART6）
    if (USART_IT == USART_IT_CTS)
    {
        assert_param(IS_USART_1236_PERIPH(USARTx));
    }

    // 获取中断所在的寄存器（CR1: 0x01, CR2: 0x02, CR3: 0x03）
    usartreg = (((uint8_t)USART_IT) >> 0x05);

    // 获取中断的位置并生成中断掩码
    itmask = USART_IT & IT_MASK;  // 提取中断的位置
    itmask = (uint32_t)0x01 << itmask; // 生成中断掩码

    // 使用 switch-case 语句选择不同的寄存器
    switch (usartreg)
    {
        case 0x01: // 如果是CR1寄存器中的中断
            itmask &= USARTx->CR1; // 与CR1寄存器中的内容进行按位与操作
            break;
        case 0x02: // 如果是CR2寄存器中的中断
            itmask &= USARTx->CR2; // 与CR2寄存器中的内容进行按位与操作
            break;
        case 0x03: // 如果是CR3寄存器中的中断
            itmask &= USARTx->CR3; // 与CR3寄存器中的内容进行按位与操作
            break;
        default:
            break;
    }

    // 计算中断标志位在状态寄存器（SR）中的位置
    bitpos = USART_IT >> 0x08;  // 提取SR寄存器中的位位置
    bitpos = (uint32_t)0x01 << bitpos;  // 生成标志位掩码

    // 检查SR寄存器中的相应标志位
    bitpos &= USARTx->SR;  // 将SR寄存器与标志位掩码按位与操作，得到相应标志位的状态

    // 如果中断掩码和状态寄存器中的标志位都被设置，表示中断发生
    if ((itmask != (uint16_t)RESET) && (bitpos != (uint16_t)RESET))
    {
        bitstatus = SET; // 中断已发生，返回SET
    }
    else
    {
        bitstatus = RESET; // 中断未发生，返回RESET
    }

    return bitstatus;  // 返回中断状态（SET 或 RESET）
}




/**
  * @brief  清除指定的 USART 中断挂起位。
  * @param  USARTx: 选择 USART 外设，x 可以是 1, 2, 3, 4, 5, 6, 7 或 8。
  * @param  USART_IT: 指定需要清除的中断挂起位。可以是以下值之一：
  *          @arg USART_IT_CTS:  CTS 状态变化中断（仅在 USART1, 2, 3, 6, 7, 8 上有效）
  *          @arg USART_IT_LBD:  LIN 断点检测中断
  *          @arg USART_IT_TC:   传输完成中断
  *          @arg USART_IT_RXNE: 接收数据寄存器非空中断
  *
  * @note   PE（奇偶校验错误）、FE（帧错误）、NE（噪声错误）、ORE（溢出错误）和 IDLE（空闲线检测）中断位
  *         是通过软件序列清除：先读取 USART_SR 寄存器，然后再读取 USART_DR 寄存器。
  * @note   RXNE 中断位可以通过直接读取 USART_DR 寄存器来清除。
  * @note   TC 中断位可以通过软件序列清除：先读取 USART_SR 寄存器，然后写入数据到 USART_DR 寄存器。
  * @note   TXE 中断位只能通过写入数据到 USART_DR 寄存器来清除。
  *
  * @retval 无返回值
  */
void myUSART_ClearITPendingBit(USART_TypeDef* USARTx, uint16_t USART_IT)
{
	uint16_t itmask;  // 用于存储中断掩码

	// 参数检查，确保传入的 USART 外设和中断标志位合法
	assert_param(IS_USART_ALL_PERIPH(USARTx));
	assert_param(IS_USART_CLEAR_IT(USART_IT));

	// 判断 CTS 中断是否在有效的 USART 外设上
	if (USART_IT == USART_IT_CTS)
	{
		assert_param(IS_USART_1236_PERIPH(USARTx));  // 仅支持 USART1, 2, 3, 6, 7, 8
	}

	// 根据中断标志位获取对应的掩码
	// 将 USART_IT 的高8位右移，获取中断位置的掩码
	itmask = (0x01 << (USART_IT >> 8));  // 计算出中断位掩码

	// 清除中断挂起位
	USARTx->SR = (uint16_t)~itmask;  // 将 SR 寄存器中对应的中断标志位清零
}

