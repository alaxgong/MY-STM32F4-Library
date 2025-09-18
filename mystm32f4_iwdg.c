/**
  ******************************************************************************
  * @file     stm32f4xx_iwdg.c
  * @author   IWDG
  * @version  V1.0.0
  * @date     2025-09-18
  * @brief    STM32F4系列独立看门狗（IWDG）外设驱动库
  *
  * @attention
  *
  * 本文件为 STM32F4xx 标准外设库 IWDG 模块的自定义驱动版本，编写过程中旨在加深对
  * STM32 编程和独立看门狗工作机制的研究理解。
  *
  * 本驱动版本的目标：
  * 1. 提供清晰的代码结构和规范的命名
  * 2. 添加详细中文注释，便于学习和理解
  * 3. 提供 IWDG 初始化、喂狗、超时配置功能
  * 4. 保持与官方库函数兼容
  *
  * 与官方库的区别：
  * - 使用现代 C 语言特性（如 C99 标准）
  * - 增加参数检查和错误处理
  * - 提供详细函数说明和使用示例
  * - 代码模块化，便于理解和扩展
  *
  * 使用说明：
  * 1. 本库函数适用于 STM32F4xx 系列微控制器
  * 2. 使用 IWDG_Init() 配置预分频和重载值，确定超时时间
  * 3. 调用 IWDG_Start() 启动独立看门狗
  * 4. 在主循环中定期调用 IWDG_Feed() 刷新计数器，防止复位
  *
  * 本库提供功能：
  * - IWDG 初始化配置（预分频器和重载值）
  * - IWDG 启动和喂狗操作
  * - 读取和判断 IWDG 状态
  *
  * 版本历史：
  * V1.0.0 初始版本
  *   - 实现基本 IWDG 初始化、启动和刷新功能
  *   - 提供超时时间配置支持
  *   - 添加错误检查和详细注释
  *
  * 免责声明：
  * 本软件按“原样”提供，不提供任何明示或暗示的担保，包括适销性或特定用途适用性。
  * 作者不对使用本软件造成的任何直接或间接损失承担责任。
  * 本软件仅供学习研究使用，禁止用于商业用途。
  *
  ******************************************************************************
  */



/* ***************************官方文档翻译区*************************************************/

/*
  ******************************************************************************
  * IWDG 特性说明
  ******************************************************************************
  * [..]
  * IWDG 可以通过软件或硬件启动（可通过选项字节配置）。
  *
  * IWDG 由其独立的低速时钟（LSI）驱动，因此即使主时钟失效，IWDG 仍然保持运行。
  * 一旦 IWDG 启动，LSI 将被强制开启且无法关闭（LSI 也不能被禁用），计数器从复位值 0xFFF 开始递减。
  * 当计数器递减到 0x000 时，系统将复位。
  * 在正常运行期间，应定期重新加载 IWDG 计数器，以防止 MCU 被复位。
  *
  * IWDG 位于 VDD 电压域内，即使在 STOP 或 STANDBY 模式下仍然有效
  * （IWDG 复位可从 STANDBY 唤醒 MCU）。
  *
  * RCC_CSR 寄存器中的 IWDGRST 标志可用于判断是否发生了 IWDG 复位。
  *
  * 32KHz(LSI) 下的最小-最大超时值：约 125us ~ 32.7s。
  * 由于 LSI 频率存在偏差，IWDG 超时可能有所不同。STM32F4xx 器件提供测量 LSI 频率的功能
  * （LSI 时钟内部连接到 TIM5 CH4 输入捕获）。测量值可用于实现具有可接受精度的 IWDG 超时。
  * 更多信息请参考 STM32F4xx 参考手册。
  *
  ******************************************************************************
  * 如何使用该驱动
  ******************************************************************************
  * [..]
  * (#) 使用 IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable) 函数使能对 IWDG_PR 和 IWDG_RLR 寄存器的写访问。
  *
  * (#) 使用 IWDG_SetPrescaler() 函数配置 IWDG 预分频器。
  *
  * (#) 使用 IWDG_SetReload() 函数配置 IWDG 计数器重载值。
  *     每次刷新计数器时，该值将被加载到 IWDG 计数器中，然后 IWDG 将从该值开始递减。
  *
  * (#) 使用 IWDG_Enable() 函数启动 IWDG（软件模式下使用时，无需手动启用 LSI，硬件会自动启用）。
  *
  * (#) 在应用程序正常运行期间，必须定期刷新 IWDG 计数器以防止 MCU 被复位，
  *     使用 IWDG_ReloadCounter() 函数即可。
  *
  * @endverbatim
  ******************************************************************************
*/


/* ***************************官方文档翻译区*************************************************/
/* Includes ------------------------------------------------------------------*/
#include "mystm32f4_iwdg.h"

/** @addtogroup STM32F4xx_StdPeriph_Driver
  * @{
  */

  /** @defgroup IWDG
	* @brief IWDG 驱动模块
	* @{
	*/

	/* 私有类型定义 -------------------------------------------------------------*/
	/* 私有宏定义 ---------------------------------------------------------------*/

	/* KR 寄存器键值掩码 */
#define KR_KEY_RELOAD    ((uint16_t)0xAAAA)  /* 刷新计数器键值 */
#define KR_KEY_ENABLE    ((uint16_t)0xCCCC)  /* 启动 IWDG 键值 */

/* 私有宏 ---------------------------------------------------------------*/
/* 私有变量 ---------------------------------------------------------------*/
/* 私有函数原型声明 -------------------------------------------------------*/
/* 私有函数 ---------------------------------------------------------------*/

/** @defgroup IWDG_Group1 预分频器和计数器配置函数
 *  @brief   配置独立看门狗的计数器和预分频器
 *
@verbatim
 ===============================================================================
			  ##### 预分频器和计数器配置函数 #####
 ===============================================================================
@endverbatim
  * @{
  */

  /**
	* @brief  允许或禁止修改看门狗的配置寄存器
	* @param  IWDG_WriteAccess: 写寄存器的状态
	*          取值：
	*            @arg IWDG_WriteAccess_Enable: 允许修改寄存器
	*            @arg IWDG_WriteAccess_Disable: 不允许修改寄存器
	* @retval 无
	* @note   只有在允许写寄存器的情况下，才能修改预分频器和重载值
	*/
void myIWDG_WriteAccessCmd(uint16_t IWDG_WriteAccess)
{
	/* 检查参数是否合法 */
	assert_param(IS_IWDG_WRITE_ACCESS(IWDG_WriteAccess));
	IWDG->KR = IWDG_WriteAccess;
}

/**
  * @brief  设置看门狗计数速度（预分频器）
  * @param  IWDG_Prescaler: 预分频器值，控制计数速度
  *          取值：
  *            @arg IWDG_Prescaler_4:   计数速度较快
  *            @arg IWDG_Prescaler_8:
  *            @arg IWDG_Prescaler_16:
  *            @arg IWDG_Prescaler_32:
  *            @arg IWDG_Prescaler_64:
  *            @arg IWDG_Prescaler_128:
  *            @arg IWDG_Prescaler_256: 计数速度最慢
  * @retval 无
  * @note   计数速度越慢，看门狗超时时间越长
  */
void myIWDG_SetPrescaler(uint8_t IWDG_Prescaler)
{
	assert_param(IS_IWDG_PRESCALER(IWDG_Prescaler));
	IWDG->PR = IWDG_Prescaler;
}

/**
  * @brief  设置看门狗超时时间
  * @param  Reload: 重载值，范围 0~0x0FFF
  * @retval 无
  * @note   重载值越大，超时时间越长；计数器每次达到 0 时，MCU 会复位
  */
void myIWDG_SetReload(uint16_t Reload)
{
	assert_param(IS_IWDG_RELOAD(Reload));
	IWDG->RLR = Reload;
}

/**
  * @brief  刷新看门狗计数器
  * @param  无
  * @retval 无
  * @note   必须定期调用，否则 MCU 会被复位，相当于“喂狗”
  */
void myIWDG_ReloadCounter(void)
{
	IWDG->KR = KR_KEY_RELOAD;
}


/**
  * @}
  */

  /** @defgroup IWDG_Group2 IWDG 启动函数
   *  @brief   独立看门狗启动相关函数
   *
  @verbatim
   ===============================================================================
					  ##### IWDG 启动函数 #####
   ===============================================================================
  @endverbatim
	* @{
	*/

	/**
	  * @brief  启动独立看门狗（IWDG）
	  * @param  无
	  * @retval 无
	  * @note   一旦启动，IWDG 将开始计数，无法通过软件关闭。
	  *         此时 IWDG_PR 和 IWDG_RLR 寄存器的写访问已被禁用。
	  *         必须在主程序中定期调用 IWDG_ReloadCounter() 来刷新计数器，
	  *         否则 MCU 会被复位，相当于“喂狗”。
	  */
void myIWDG_Enable(void)
{
	IWDG->KR = KR_KEY_ENABLE;
}


/**
  * @}
  */

  /** @defgroup IWDG_Group3 标志管理函数
   *  @brief  独立看门狗标志管理函数
   *
  @verbatim
   ===============================================================================
					  ##### 标志管理函数 #####
   ===============================================================================
  @endverbatim
	* @{
	*/

	/**
	  * @brief  检查指定的 IWDG 标志是否被置位
	  * @param  IWDG_FLAG: 要检查的标志
	  *          可选值：
	  *            @arg IWDG_FLAG_PVU: 预分频器值正在更新
	  *            @arg IWDG_FLAG_RVU: 重载值正在更新
	  * @retval 标志状态 (SET 或 RESET)
	  * @note   如果返回 SET 表示对应寄存器正在更新，不能立即修改预分频器或重载值
	  */
FlagStatus myIWDG_GetFlagStatus(uint16_t IWDG_FLAG)
{
	FlagStatus bitstatus;

	/* 参数检查 */
	assert_param(IS_IWDG_FLAG(IWDG_FLAG));

	/* 使用三目运算符判断标志状态 */
	return  ((IWDG->SR & IWDG_FLAG) != (uint32_t)RESET) ? SET : RESET;

}




/**
  * @brief  独立看门狗（IWDG）初始化函数
  * @param  prer: IWDG 预分频系数，可选值：
  *                IWDG_Prescaler_4, IWDG_Prescaler_8, IWDG_Prescaler_16,
  *                IWDG_Prescaler_32, IWDG_Prescaler_64, IWDG_Prescaler_128,
  *                IWDG_Prescaler_256
  * @param  rlr: IWDG 重载值，范围 0~0x0FFF，值越大超时时间越长
  * @retval 无
  * @note
  *   调用此函数后，看门狗立即启动，必须定期调用 IWDG_ReloadCounter() 来刷新计数器，
  *   否则 MCU 会被复位，相当于“喂狗”。
  *
  *   独立看门狗超时时间计算公式：
  *     t_timeout = (Reload + 1) * Prescaler / f_LSI
  *   其中：
  *     - Reload: 重载值 rlr
  *     - Prescaler: 预分频系数 prer
  *     - f_LSI: LSI 时钟频率（一般 STM32F4 约 32 kHz）
  *
  *   举例：
  *     Reload = 0x0FFF，Prescaler = 32，f_LSI = 32kHz
  *     t_timeout = (4095+1) * 32 / 32000 ≈ 4.096 秒
  */
/*******在循环前调用该函数执行看门狗初始化配置*/
void myIWDG_Init(uint8_t prer, uint16_t rlr)
{
	IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable); // 允许写 IWDG_PR 和 IWDG_RLR
	IWDG_SetPrescaler(prer);                       // 设置分频系数
	IWDG_SetReload(rlr);                           // 设置重载值
	IWDG_ReloadCounter();                          // 刷新计数器（喂狗一次）
	IWDG_Enable();                                 // 启动独立看门狗
}

