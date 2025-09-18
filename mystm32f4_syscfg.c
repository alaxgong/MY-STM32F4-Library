/**
  ******************************************************************************
  * @file     mystm32f4_syscfg.c
  * @author   QiangGu
  * @version  V1.0.0
  * @date     2025-09-16
  * @brief    STM32F4系列SYSCFG外设驱动库（重写版）
  *
  * @attention
  *
  * 本文件为 STM32F4xx 标准外设库 SYSCFG 模块的自定义重写版本，编写过程中旨在加深对
  * STM32 编程和系统配置控制器工作机制的研究理解。
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
  * 2. 在使用任何 SYSCFG 函数前，必须首先调用 RCC_APB2PeriphClockCmd() 使能 SYSCFG 时钟
  * 3. 使用 SYSCFG 提供的函数配置 EXTI 线路映射、DMA 通道映射等系统配置
  * 4. 在 NVIC 中配置相应中断优先级，并编写对应的中断服务函数（若使用 EXTI）
  *
  * 本库提供的功能：
  * - EXTI 线路 GPIO 映射配置
  * - DMA 通道映射配置（部分型号支持）
  * - 系统级复用功能配置（如 FPU 异常、引脚复用等）
  * - 参数检查和错误处理支持
  *
  * 版本历史：
  * V1.0.0 初始版本
  *   - 实现基本的 SYSCFG 配置功能
  *   - 添加详细的错误检查和调试支持
  *
  * 免责声明：
  * 本软件按“原样”提供，不提供任何明示或暗示的担保，包括但不限于对适销性或特定用途适用性的保证。
  * 作者不对因使用本软件而产生的任何直接、间接、偶然、特殊、示例性或后果性损失承担责任。
  * 本软件仅供学习和研究使用，无偿共享，禁止用于商业用途。
  *
  ******************************************************************************
  */
  /* 包含头文件 ----------------------------------------------------------------*/
#include "mystm32f4xx_syscfg.h"
#include "stm32f4xx_rcc.h"

/** @addtogroup STM32F4xx_StdPeriph_Driver
  * @{
  */

  /** @defgroup SYSCFG
	* @brief SYSCFG 驱动模块
	* @{
	*/

	/* 私有类型定义 -------------------------------------------------------------*/
	/* 私有宏定义 ---------------------------------------------------------------*/
	/* ------------ RCC 寄存器在别名区的位地址 ----------- */
#define SYSCFG_OFFSET             (SYSCFG_BASE - PERIPH_BASE) // SYSCFG 基地址偏移

/* --- MEMRMP 寄存器 ---*/
/* UFB_MODE 位的别名地址 */
#define MEMRMP_OFFSET             SYSCFG_OFFSET 
#define UFB_MODE_BitNumber        ((uint8_t)0x8) 
#define UFB_MODE_BB               (PERIPH_BB_BASE + (MEMRMP_OFFSET * 32) + (UFB_MODE_BitNumber * 4)) 
// 通过位带操作访问 MEMRMP 寄存器的 UFB_MODE 位

/* --- PMC 寄存器 ---*/
/* MII_RMII_SEL 位的别名地址 */
#define PMC_OFFSET                (SYSCFG_OFFSET + 0x04) 
#define MII_RMII_SEL_BitNumber    ((uint8_t)0x17) 
#define PMC_MII_RMII_SEL_BB       (PERIPH_BB_BASE + (PMC_OFFSET * 32) + (MII_RMII_SEL_BitNumber * 4)) 
// 通过位带操作访问 PMC 寄存器的 MII_RMII_SEL 位，用于选择 MII/RMII 接口模式

/* --- CMPCR 寄存器 ---*/
/* CMP_PD 位的别名地址 */
#define CMPCR_OFFSET              (SYSCFG_OFFSET + 0x20) 
#define CMP_PD_BitNumber          ((uint8_t)0x00) 
#define CMPCR_CMP_PD_BB           (PERIPH_BB_BASE + (CMPCR_OFFSET * 32) + (CMP_PD_BitNumber * 4)) 
// 通过位带操作访问 CMPCR 寄存器的 CMP_PD 位，用于比较器电源控制

/* 私有宏 -------------------------------------------------------------*/
/* 私有变量 -------------------------------------------------------------*/
/* 私有函数原型 ---------------------------------------------------------*/
/* 私有函数实现 -----------------------------------------------------------*/

/** @defgroup SYSCFG_Private_Functions
  * @{
  */
  // 以下为 SYSCFG 模块的私有函数，通常用于内部寄存器操作和配置，不建议用户直接调用


/**
  * @brief  将 SYSCFG 外设寄存器复位为默认状态
  * @details
  *  该函数会重置 SYSCFG 模块中所有可配置的寄存器，包括：
  *   - 外设引脚复用（AFIO/Remap）配置
  *   - EXTI 线路映射配置
  *   - 其他 SYSCFG 控制寄存器
  *  复位后，SYSCFG 模块处于出厂默认状态。
  *
  * @param  None
  * @retval None
  *
  * @note
  * 1. 函数通过 RCC_APB2 外设复位命令实现复位：
  *    - 先使能 SYSCFG 外设复位
  *    - 再取消复位
  * 2. 调用本函数后，如果要继续使用 SYSCFG 配置 EXTI 或引脚复用，需要重新初始化。
  *
  * @example
  *    // 复位 SYSCFG 外设
  *    SYSCFG_DeInit();
  */
void mySYSCFG_DeInit(void)
{
	// 先使能 SYSCFG 外设复位，再立即取消复位（逗号运算符一次执行两步）
	RCC_APB2PeriphResetCmd(RCC_APB2Periph_SYSCFG, ENABLE),
	RCC_APB2PeriphResetCmd(RCC_APB2Periph_SYSCFG, DISABLE);
}

/**
  * @brief  配置指定引脚或总线的内存映射
  * @details
  *  该函数用于修改系统的内存映射，将不同类型的存储器映射到起始地址 0x00000000。
  *  常用于启动时选择从哪个存储器执行代码或访问数据。
  *
  * @param  SYSCFG_MemoryRemap: 内存重映射选项，可选择以下值：
  *            @arg SYSCFG_MemoryRemap_Flash:       主 Flash 映射到 0x00000000
  *            @arg SYSCFG_MemoryRemap_SystemFlash: 系统 Flash 映射到 0x00000000
  *            @arg SYSCFG_MemoryRemap_FSMC:        FSMC（Bank1 NOR/PSRAM 1 和 2）映射到 0x00000000
  *                                                适用于 STM32F405xx/407xx 和 STM32F415xx/417xx
  *            @arg SYSCFG_MemoryRemap_FMC:         FMC（Bank1 NOR/PSRAM 1 和 2）映射到 0x00000000
  *                                                适用于 STM32F42xxx/43xxx
  *            @arg SYSCFG_MemoryRemap_SRAM:        内部 SRAM（112 KB）映射到 0x00000000
  *            @arg SYSCFG_MemoryRemap_SDRAM:       FMC 外部 SDRAM 映射到 0x00000000
  *                                                适用于 STM32F42xxx/43xxx
  *
  * @retval None
  *
  * @note   调用本函数会直接修改 SYSCFG->MEMRMP 寄存器，确保传入的参数合法。
  *         使用前需要使能 SYSCFG 外设时钟。
  *
  * @example
  *    // 将主 Flash 映射到 0x00000000
  *    SYSCFG_MemoryRemapConfig(SYSCFG_MemoryRemap_Flash);
  */
void mySYSCFG_MemoryRemapConfig(uint8_t SYSCFG_MemoryRemap)
{
	/* 参数检查，确保传入值合法 */
	assert_param(IS_SYSCFG_MEMORY_REMAP_CONFING(SYSCFG_MemoryRemap));

	/* 修改内存映射寄存器 MEMRMP */
	SYSCFG->MEMRMP = SYSCFG_MemoryRemap;
}
/**
  * @brief  启用或禁用内部 Flash Bank 交换
  * @details
  *  该函数用于 STM32F42xxx/43xxx 系列芯片，通过控制 UFB_MODE 位，实现 Flash Bank1 与 Bank2 的映射互换。
  *  作用：可以改变 CPU 从 0x08000000 开始访问的是哪一块 Flash。
  *
  * @note   仅适用于 STM32F42xxx/43xxx 系列设备。
  *
  * @param  NewState: FLASH Bank 交换状态，可选值：
  *            @arg ENABLE: 交换 Flash Bank 映射
  *                         - Bank2 映射到 0x08000000（别名 0x00000000）
  *                         - Bank1 映射到 0x08100000（别名 0x00100000）
  *            @arg DISABLE: 默认状态，不交换
  *                         - Bank1 映射到 0x08000000（别名 0x00000000）
  *                         - Bank2 映射到 0x08100000（别名 0x00100000）
  *
  * @retval None
  *
  * @example
  *    // 启用 Flash Bank 交换
  *    SYSCFG_MemorySwappingBank(ENABLE);
  */
void mySYSCFG_MemorySwappingBank(FunctionalState NewState)
{
	/* 参数检查，确保状态合法 */
	assert_param(IS_FUNCTIONAL_STATE(NewState));

	/* 通过位带操作设置 UFB_MODE 位，实现 Bank 交换 */
	*(__IO uint32_t*) UFB_MODE_BB = (uint32_t)NewState;
}


/**
  * @brief  选择用于 EXTI 的 GPIO 引脚
  * @details
  *  该函数用于配置 EXTI 外部中断线路的源 GPIO 引脚，
  *  即指定某个 GPIO 口的某个引脚作为 EXTI 中断线路。
  *
  * @param  EXTI_PortSourceGPIOx : 指定 EXTI 源 GPIO 端口
  *          - STM32F42xxx/43xxx: A..K
  *          - STM32F405xx/407xx & STM32F415xx/417xx: A..I
  *          - STM32F401xx: A, B, C, D, H
  *
  * @param  EXTI_PinSourcex : 指定 EXTI 线路引脚号
  *          - x 可为 0..15（部分端口范围有限，如 GPIOI/ GPIOK）
  *          - 作用：指定该端口的哪一条引脚连接到 EXTI 线路
  *
  * @retval None
  *
  * @example
  *    // 将 PA0 配置为 EXTI0 的中断源
  *    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource0);
  */

void mySYSCFG_EXTILineConfig(uint8_t EXTI_PortSourceGPIOx, uint8_t EXTI_PinSourcex)
{
	/* 参数检查 */
	assert_param(IS_EXTI_PORT_SOURCE(EXTI_PortSourceGPIOx));
	assert_param(IS_EXTI_PIN_SOURCE(EXTI_PinSourcex));

	/* 计算 EXTICR 数组索引和位偏移 */

	/* regIndex: 计算 EXTI_PinSourcex 属于哪个 EXTICR 寄存器
	   - 每个 EXTICR 寄存器控制 4 条 EXTI 线路：
		   EXTICR[0] 控制 EXTI0~3
		   EXTICR[1] 控制 EXTI4~7
		   EXTICR[2] 控制 EXTI8~11
		   EXTICR[3] 控制 EXTI12~15
	   - 所以用 EXTI_PinSourcex / 4 得到寄存器索引
		 例如 EXTI_PinSourcex = 5，则 regIndex = 5 / 4 = 1，对应 EXTICR[1] */

		 /* bitOffset: 计算该引脚在寄存器中的位偏移量
			- 每条 EXTI 线路占 4 位，用于存储 GPIO 端口号
			- 在寄存器中位置 = (引脚号 % 4) * 4
			  例如 EXTI_PinSourcex = 5，则 bitOffset = (5 % 4) * 4 = 1*4 = 4
			  表示该引脚对应的 4 位从寄存器第 4 位开始 */

	uint8_t regIndex = EXTI_PinSourcex / 4;       // EXTICR 寄存器索引
	uint8_t bitOffset = (EXTI_PinSourcex % 4) * 4; // 位偏移量

	/* 清除原有配置 */
	SYSCFG->EXTICR[regIndex] &= ~(0x0F << bitOffset);

	/* 设置新的端口映射 */
	SYSCFG->EXTICR[regIndex] |= ((uint32_t)EXTI_PortSourceGPIOx << bitOffset);
}


/**
  * @brief  配置以太网接口类型（ETHERNET Media Interface）
  * @details
  *  该函数用于选择 STM32 内部以太网 MAC 的接口模式：
  *  MII 或 RMII。
  *  - MII (Media Independent Interface)：标准以太网接口，使用较多引脚
  *  - RMII (Reduced MII)：精简接口，节省引脚
  *
  * @param  SYSCFG_ETH_MediaInterface : 以太网接口模式，可选：
  *            @arg SYSCFG_ETH_MediaInterface_MII   : 选择 MII 模式
  *            @arg SYSCFG_ETH_MediaInterface_RMII  : 选择 RMII 模式
  *
  * @retval None
  *
  * @example
  *    // 配置以太网接口为 RMII 模式
  *    SYSCFG_ETH_MediaInterfaceConfig(SYSCFG_ETH_MediaInterface_RMII);
  */
void mySYSCFG_ETH_MediaInterfaceConfig(uint32_t SYSCFG_ETH_MediaInterface)
{
	/* 参数检查，确保传入值合法 */
	assert_param(IS_SYSCFG_ETH_MEDIA_INTERFACE(SYSCFG_ETH_MediaInterface));

	/* 配置 PMC 寄存器中的 MII/RMII 选择位 */
	*(__IO uint32_t*) PMC_MII_RMII_SEL_BB = SYSCFG_ETH_MediaInterface;
}


/**
  * @brief  启用或禁用 I/O 补偿单元
  * @details
  *  I/O 补偿单元用于优化 GPIO 输出驱动能力和输入特性，
  *  当芯片供电电压在 2.4V~3.6V 范围内时可以使用。
  *  - ENABLE: 启用 I/O 补偿单元
  *  - DISABLE: 禁用 I/O 补偿单元（省电模式）
  *
  * @note   请确保芯片电压在 2.4~3.6V，否则补偿单元可能无法正常工作
  *
  * @param  NewState: I/O 补偿单元状态，可选：
  *            @arg ENABLE  : 启用补偿单元
  *            @arg DISABLE : 禁用补偿单元
  *
  * @retval None
  *
  * @example
  *    // 启用 I/O 补偿单元
  *    SYSCFG_CompensationCellCmd(ENABLE);
  */
void mySYSCFG_CompensationCellCmd(FunctionalState NewState)
{
	/* 参数检查，确保状态合法 */
	assert_param(IS_FUNCTIONAL_STATE(NewState));

	/* 设置 CMPCR 寄存器中的 CMP_PD 位，控制补偿单元状态 */
	*(__IO uint32_t*) CMPCR_CMP_PD_BB = (uint32_t)NewState;
}


/**
  * @brief  获取 I/O 补偿单元是否准备就绪
  * @details
  *  该函数用于检查 I/O 补偿单元的 READY 标志位。
  *  当启用补偿单元后，需要等待 READY 位置位，表示补偿单元初始化完成，
  *  GPIO 才能获得优化的输出驱动和输入特性。
  *
  * @param  None
  *
  * @retval FlagStatus: READY 状态
  *         - SET   : 补偿单元已准备好
  *         - RESET : 补偿单元尚未准备好
  *
  * @example
  *    // 检查补偿单元是否准备就绪
  *    if(SYSCFG_GetCompensationCellStatus() == SET)
  *    {
  *        // 补偿单元已就绪，可以安全使用高速 GPIO
  *    }
  */
FlagStatus mySYSCFG_GetCompensationCellStatus(void)
{
	/* 使用三目运算符简化 if-else 判断 */
	return ((SYSCFG->CMPCR & SYSCFG_CMPCR_READY) != 0) ? SET : RESET;
}

