/**
  ******************************************************************************
/**
  ******************************************************************************
  * @file     stm32f4xx_rtc_rewrite.c
  * @author   QiangGu
  * @version  V1.0.0
  * @date     2025-10-02
  * @brief    STM32F4系列RTC外设驱动库（重写版）
  *
  * @attention
  *
  * 本文件为 STM32F4xx 标准外设库 RTC 模块的自定义重写版本，编写过程中旨在加深对
  * STM32 编程、RTC 工作机制以及低功耗模式下的时间管理的理解。
  *
  * 此重写版本的目标：
  * 1. 提供更清晰的代码结构和命名规范
  * 2. 添加更详细的中文注释，便于学习理解
  * 3. 优化部分功能实现，提高代码可读性和可维护性
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
  * 2. 在使用 RTC 之前，必须先使能 PWR APB1 时钟，并通过 PWR_BackupAccessCmd() 允许访问备份域
  * 3. 配置 RTC 时钟源并使能 RTC 时钟（RCC_RTCCLKConfig & RCC_RTCCLKCmd）
  * 4. 使用 RTC_Init() 配置分频器和时制
  * 5. 使用 RTC_SetTime() / RTC_SetDate() 配置时间与日期
  * 6. 使用 RTC_SetAlarm() 配置闹钟，并通过 RTC_AlarmCmd() 启用
  * 7. RTC 还支持时间戳、唤醒定时器、防篡改事件及平滑/粗略校准等功能
  *
  * 本库提供的功能：
  * - RTC 初始化与分频器配置
  * - 时间和日期读取与设置
  * - 闹钟（Alarm A/B）设置与管理
  * - 唤醒定时器配置与控制
  * - 防篡改与时间戳功能配置
  * - 输出引脚配置及校准功能
  * - 备份数据寄存器读写
  * - 支持低功耗模式下的时间管理
  *
  * 版本历史：
  * V1.0.0 初始版本
  *   - 实现基本 RTC 初始化、时间/日期设置、闹钟功能
  *   - 支持低功耗模式唤醒、时间戳、防篡改及备份寄存器功能
  *   - 添加详细中文注释，方便学习和理解
  *
  * 免责声明：
  * 本软件按“原样”提供，不提供任何明示或暗示的担保，包括但不限于对适销性或特定用途适用性的保证。
  * 作者不对因使用本软件而产生的任何直接、间接、偶然、特殊、示例性或后果性损失承担责任。
  * 本软件仅供学习和研究使用，无偿共享，禁止用于商业用途。
  *
  ******************************************************************************
  */
/**  ****************************官方库翻译区域**************************************************/
/*
 ===================================================================
              ##### 备份域工作条件 #####
 ===================================================================
 [..] 实时时钟（RTC）、RTC 备份寄存器和备份 SRAM（BKP SRAM）可以通过 VBAT 电压供电，
      即使主电源 VDD 已关闭。为了在 VDD 关闭时保持 RTC 备份寄存器和备份 SRAM 的内容，
      VBAT 引脚可以连接到备用电源（如电池或其他电源）。

 [..] 为了使 RTC 在主电源 VDD 关闭时仍能工作，VBAT 为以下模块供电：
   (#) RTC 本身
   (#) LSE 振荡器
   (#) 当启用低功耗备份稳压器时的备份 SRAM
   (#) PC13~PC15 I/O 以及 PI8 I/O（如可用）
  
 [..] 当备份域由 VDD 供电（模拟开关连接到 VDD）时，可用功能：
   (#) PC14、PC15 可用作 GPIO 或 LSE 引脚
   (#) PC13 可用作 GPIO 或 RTC_AF1
   (#) PI8 可用作 GPIO 或 RTC_AF2
  
 [..] 当备份域由 VBAT 供电（模拟开关连接到 VBAT，因为 VDD 不存在）时，可用功能：
   (#) PC14、PC15 仅可用作 LSE 引脚
   (#) PC13 仅可用作 RTC_AF1
   (#) PI8 仅可用作 RTC_AF2
  

                   ##### 备份域复位 #####
 ===================================================================
 [..] 备份域复位会将所有 RTC 寄存器和 RCC_BDCR 寄存器复位为默认值。
      BKPSRAM 不受复位影响。复位 BKPSRAM 只能通过 Flash 接口，改变保护等级从 1 到 0。
 [..] 当发生以下事件之一时，会生成备份域复位：
   (#) 软件复位，通过设置 RCC_BDCR 寄存器的 BDRST 位触发。可使用 RCC_BackupResetCmd()。
   (#) VDD 或 VBAT 上电，如果之前两者均关闭。

                   ##### 备份域访问 #####
 ===================================================================
 [..] 复位后，备份域（RTC 寄存器、备份数据寄存器和备份 SRAM）被保护，防止无意写入。
 [..] 启用 RTC 域和 RTC 寄存器访问步骤：
   (+) 使能电源控制器（PWR）APB1 时钟：RCC_APB1PeriphClockCmd()
   (+) 允许访问 RTC 域：PWR_BackupAccessCmd()
   (+) 选择 RTC 时钟源：RCC_RTCCLKConfig()
   (+) 使能 RTC 时钟：RCC_RTCCLKCmd()

                  ##### RTC 驱动使用方法 #####
 ===================================================================
 [..] 
   (+) 启用 RTC 域访问（见上文）
   (+) 配置 RTC 分频器（异步/同步）和 RTC 时制：RTC_Init()

 *** 时间和日期配置 ***
 ===================================
 [..] 
   (+) 配置 RTC 日历（时间和日期）：RTC_SetTime() 和 RTC_SetDate()
   (+) 读取 RTC 日历：RTC_GetTime() 和 RTC_GetDate()
   (+) 使用 RTC_DayLightSavingConfig() 加减一小时（夏令时）  

 *** 闹钟配置 ***
 ===========================
 [..]
   (+) 配置 RTC 闹钟：RTC_SetAlarm()
   (+) 使能选定的闹钟：RTC_AlarmCmd()
   (+) 读取 RTC 闹钟：RTC_GetAlarm()
   (+) 读取闹钟子秒：RTC_GetAlarmSubSecond()

 *** RTC 唤醒配置 ***
 ================================
 [..] 
   (+) 配置 RTC 唤醒时钟源：RTC_WakeUpClockConfig()
   (+) 配置 RTC 唤醒计数器：RTC_SetWakeUpCounter()
   (+) 使能 RTC 唤醒：RTC_WakeUpCmd()
   (+) 读取 RTC 唤醒计数器：RTC_GetWakeUpCounter()

 *** 输出配置 ***
 ==============================
 [..] RTC 有两个输出：
   (+) AFO_ALARM：管理 RTC 闹钟 A、B 和唤醒信号，可用 RTC_OutputConfig() 输出到 RTC_AF1 引脚  
   (+) AFO_CALIB：512Hz 或 1Hz 信号，可用 RTC_CalibOutputCmd() 输出到 RTC_AF1

 *** 平滑数字校准配置 ***
 ==================================
 [..]
   (+) 配置 RTC 平滑数字校准值和校准周期（32s、16s、8s）：RTC_SmoothCalibConfig()

 *** 粗略数字校准配置 ***
 ==================================
 [..]
   (+) 配置 RTC 粗略校准值及符号：RTC_CoarseCalibConfig()
   (+) 使能 RTC 粗略校准：RTC_CoarseCalibCmd()

 *** 时间戳配置 ***
 ==============================
 [..]
   (+) 配置 RTC_AF1 触发并启用 RTC 时间戳：RTC_TimeStampCmd()
   (+) 读取时间戳时间和日期：RTC_GetTimeStamp()
   (+) 读取时间戳子秒：RTC_GetTimeStampSubSecond()
   (+) TAMPER1 可映射到 RTC_AF1(PC13) 或 RTC_AF2(PI8)，使用 RTC_TamperPinSelection() 选择

 *** 防篡改配置 ***
 ===========================
 [..]
   (+) 使能 RTC 防篡改：RTC_TamperCmd()
   (+) 配置防篡改滤波计数：RTC_TamperFilterConfig()
   (+) 配置防篡改触发边沿/电平：RTC_TamperConfig()
   (+) 配置采样频率：RTC_TamperSamplingFreqConfig()
   (+) 配置预充电/放电时间：RTC_TamperPinsPrechargeDuration()
   (+) 使能上拉：RTC_TamperPullUpDisableCmd()
   (+) 防篡改事件启用时间戳：TC_TSOnTamperDetecCmd()
   (+) 时间戳映射到 RTC_AF1 或 RTC_AF2，根据 TSINSEL 设置：RTC_TimeStampPinSelection()

 *** 备份数据寄存器配置 ***
 ===========================================
 [..]
   (+) 写 RTC 备份寄存器：RTC_WriteBackupRegister()
   (+) 读 RTC 备份寄存器：RTC_ReadBackupRegister()

                  ##### RTC 与低功耗模式 #####
 ===================================================================
 [..] MCU 可以通过 RTC 备用功能从低功耗模式唤醒。
 [..] RTC 备用功能包括 RTC 闹钟（A/B）、RTC 唤醒、RTC 防篡改事件和时间戳事件。
 [..] RTC 可唤醒系统从 Stop/Standby 模式。
 [..] 系统也可使用 RTC 闹钟或唤醒事件在低功耗模式下自动唤醒，无需外部中断。
 [..] RTC 提供可编程时间基准，用于定期唤醒 Stop/Standby 模式。
      Stop/Standby 唤醒仅当 RTC 时钟源为 LSE 或 LSI 时可用。

          ##### RTC_AF1 备用功能选择 #####
 ===================================================================
 [..] RTC_AF1 引脚（PC13）可用于：
   (+) AFO_ALARM 输出
   (+) AFO_CALIB 输出
   (+) AFI_TAMPER
   (+) AFI_TIMESTAMP

 [..]   
   +-------------------------------------------------------------------------------------------------------------+
   |     引脚        |AFO_ALARM |AFO_CALIB |AFI_TAMPER |AFI_TIMESTAMP | TAMP1INSEL |   TSINSEL    |ALARMOUTTYPE |
   |  配置和功能     | 启用     | 启用     | 启用       | 启用         | TAMPER1引脚 | TIMESTAMP引脚 | AFO_ALARM   |
   |-----------------|----------|----------|-----------|--------------|------------|--------------|--------------|
   |   闹钟输出 OD    | 1        | 不关心   | 不关心    | 不关心       | 不关心     | 不关心       | 0            |
   |   闹钟输出 PP    | 1        | 不关心   | 不关心    | 不关心       | 不关心     | 不关心       | 1            |
   |   校准输出 PP    | 0        | 1        | 不关心    | 不关心       | 不关心     | 不关心       | 不关心       |
   |   防篡改输入     | 0        | 0        | 1         | 0            | 0          | 不关心       | 不关心       |
   |   时间戳/防篡改  | 0        | 0        | 1         | 1            | 0          | 0            | 不关心       |
   |   时间戳输入     | 0        | 0        | 0         | 1            | 不关心     | 0            | 不关心       |
   |   标准 GPIO      | 0        | 0        | 0         | 0            | 不关心     | 不关心       | 不关心       |
   +-------------------------------------------------------------------------------------------------------------+

        ##### RTC_AF2 备用功能选择 #####
 ===================================================================
 [..] RTC_AF2 引脚（PI8）可用于：
   (+) AFI_TAMPER
   (+) AFI_TIMESTAMP
 [..]
   +---------------------------------------------------------------------------------------+
   |     引脚        |AFI_TAMPER |AFI_TIMESTAMP | TAMP1INSEL |   TSINSEL    |ALARMOUTTYPE |
   |  配置和功能     | 启用       | 启用         | TAMPER1引脚 | TIMESTAMP引脚 | AFO_ALARM   |
   |-----------------|-----------|--------------|------------|--------------|--------------|
   |  防篡改输入     | 1         | 0            | 1          | 不关心       | 不关心       |
   |  时间戳/防篡改  | 1         | 1            | 1          | 1            | 不关心       |
   |  时间戳输入     | 0         | 1            | 不关心     | 1            | 不关心       |
   |  标准 GPIO      | 0         | 0            | 不关心     | 不关心       | 不关心       |
   +---------------------------------------------------------------------------------------+   
 
@endverbatim
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2014 STMicroelectronics</center></h2>
  *
  * 根据 MCD-ST Liberty SW 许可协议 V2 使用本文件，必须遵守该许可协议。
  * 你可以从以下地址获取许可协议：
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * 除非法律要求或协议另有规定，否则按“原样”提供本软件，不提供任何明示或暗示的保证。
  * 包括但不限于适销性、特定用途适用性或不侵权等保证。
  *
  ******************************************************************************
  */


/** @defgroup RTC_Group1 初始化与配置函数
 *  @brief   RTC 初始化与配置函数
 *
@verbatim
 ===============================================================================
             ##### 初始化与配置函数 #####
 ===============================================================================

 [..] 本节提供了一组函数，用于初始化和配置 RTC，包括：
      - 分频器（同步和异步）
      - RTC 时制（12/24 小时）
      - 禁用 RTC 寄存器写保护
      - 进入和退出 RTC 初始化模式
      - RTC 寄存器同步检查
      - 参考时钟检测使能

   (#) RTC 分频器用于生成 1Hz 的 RTC 时间基准。分频器分为两个可编程部分，以最小化功耗：
       (++) 7 位异步分频器和 13 位同步分频器
       (++) 当同时使用两者时，建议将异步分频器配置为较大值，以降低功耗

   (#) 所有 RTC 寄存器默认写保护。写入 RTC 寄存器需要先向写保护寄存器 RTC_WPR 写入解锁键

   (#) 配置 RTC 日历时，用户应用程序需要进入初始化模式。在该模式下，日历计数器停止，可以安全更新其值。
       初始化序列完成后，日历将在 4 个 RTCCLK 时钟周期后重新开始计数

   (#) 若要通过影子寄存器读取日历（在日历初始化、更新或从低功耗模式唤醒后），
       软件必须先清除 RSF 标志，然后等待其再次被置位，才能读取日历。
       这保证了日历寄存器已正确复制到 RTC_TR 和 RTC_DR 影子寄存器中。
       RTC_WaitForSynchro() 函数实现了上述软件序列（RSF 清零和 RSF 检查）。
@endverbatim
  * @{
  */

/**
  * @brief  将 RTC 寄存器复位到默认值
  * @note   本函数不会复位 RTC 时钟源和备份数据寄存器
  * @param  None
  * @retval ErrorStatus:
  *          - SUCCESS: RTC 寄存器成功复位
  *          - ERROR  : RTC 寄存器未能复位
  * @details
  * 1. 禁用写保护，允许修改 RTC 寄存器
  * 2. 进入初始化模式
  * 3. 复位 TR、DR 寄存器，并清除 CR 除最低 3 位外的所有位
  * 4. 等待 WUTWF 标志置位（唤醒定时器可写标志），超时则返回 ERROR
  * 5. 复位 RTC 相关寄存器：
  *    - CR 寄存器、WUTR、PRER、CALIBR、ALRMAR、ALRMBR
  *    - SHIFTR、CALR、ALRMASSR、ALRMBSSR
  *    - ISR、TAFCR
  * 6. 等待寄存器同步完成
  * 7. 启用写保护
  */
ErrorStatus myRTC_DeInit(void)
{
    __IO uint32_t wutcounter = 0x00;
    uint32_t wutwfstatus = 0x00;
    ErrorStatus status;

    /* 禁用 RTC 寄存器写保护 */
    RTC->WPR = 0xCA;
    RTC->WPR = 0x53;

    /* 尝试进入初始化模式，成功则继续，否则直接返回 ERROR */
    status = (RTC_EnterInitMode() == ERROR) ? ERROR : SUCCESS;

    /* 若成功进入初始化模式，则复位寄存器 */
    status = (status == SUCCESS) ? (
        /* 复位 TR、DR 寄存器，CR 保留最低3位 */
        (RTC->TR = 0x00000000,
         RTC->DR = 0x00002101,
         RTC->CR &= 0x00000007,
         /* 等待 WUTWF 置位或超时 */
         (void)((wutcounter = 0, 
                 do { wutwfstatus = RTC->ISR & RTC_ISR_WUTWF; wutcounter++; } 
                 while((wutcounter != INITMODE_TIMEOUT) && (wutwfstatus == 0))),
         /* 检查 WUTWF */
         (RTC->ISR & RTC_ISR_WUTWF) ? (
             /* 复位所有 RTC 寄存器 */
             RTC->CR &= 0x00000000,
             RTC->WUTR = 0x0000FFFF,
             RTC->PRER = 0x007F00FF,
             RTC->CALIBR = 0x00000000,
             RTC->ALRMAR = 0x00000000,
             RTC->ALRMBR = 0x00000000,
             RTC->SHIFTR = 0x00000000,
             RTC->CALR = 0x00000000,
             RTC->ALRMASSR = 0x00000000,
             RTC->ALRMBSSR = 0x00000000,
             RTC->ISR = 0x00000000,
             RTC->TAFCR = 0x00000000,
             /* 等待同步完成 */
             (RTC_WaitForSynchro() == ERROR) ? ERROR : SUCCESS
         ) : ERROR)
    ) : ERROR;

    /* 恢复 RTC 寄存器写保护 */
    RTC->WPR = 0xFF;

    return status;
}


/**
  * @brief  根据 RTC_InitStruct 的配置初始化 RTC 寄存器
  * @param  RTC_InitStruct: 指向 RTC_InitTypeDef 结构体的指针，包含 RTC 外设的配置参数：
  *         - RTC_HourFormat  : 12 小时或 24 小时制
  *         - RTC_AsynchPrediv: 异步分频器值
  *         - RTC_SynchPrediv : 同步分频器值
  * @note   RTC 分频器寄存器受写保护，仅在初始化模式下可写
  * @retval ErrorStatus:
  *          - SUCCESS: RTC 寄存器成功初始化
  *          - ERROR  : RTC 寄存器初始化失败
  * @details
  * 1. 检查传入参数合法性
  * 2. 禁用 RTC 寄存器写保护
  * 3. 进入初始化模式
  * 4. 配置 RTC_CR 寄存器（设置 12/24 小时制）
  * 5. 配置 RTC_PRER 寄存器（同步/异步分频器）
  * 6. 退出初始化模式
  * 7. 启用 RTC 寄存器写保护
  */
ErrorStatus myRTC_Init(RTC_InitTypeDef* RTC_InitStruct)
{
    ErrorStatus status;

    /* 参数检查 */
    assert_param(IS_RTC_HOUR_FORMAT(RTC_InitStruct->RTC_HourFormat));
    assert_param(IS_RTC_ASYNCH_PREDIV(RTC_InitStruct->RTC_AsynchPrediv));
    assert_param(IS_RTC_SYNCH_PREDIV(RTC_InitStruct->RTC_SynchPrediv));

    /* 禁用写保护 */
    RTC->WPR = 0xCA;
    RTC->WPR = 0x53;

    /* 尝试进入初始化模式，成功则继续，否则返回 ERROR */
    status = (RTC_EnterInitMode() == ERROR) ? ERROR : SUCCESS;

    /* 初始化寄存器（仅在进入初始化模式成功时执行） */
    if (status == SUCCESS)
    {
        /* 配置 RTC_CR 寄存器：先清除 FMT 位，再设置 12/24 小时制 */
        RTC->CR &= ~RTC_CR_FMT;
        RTC->CR |= (uint32_t)(RTC_InitStruct->RTC_HourFormat);

        /* 配置同步/异步分频器 PRER 寄存器 */
        RTC->PRER = (uint32_t)(RTC_InitStruct->RTC_SynchPrediv) |
                    ((uint32_t)(RTC_InitStruct->RTC_AsynchPrediv) << 16);

        /* 退出初始化模式 */
        RTC_ExitInitMode();
    }

    /* 恢复写保护 */
    RTC->WPR = 0xFF;

    return status;
}


/**
  * @brief  用默认值初始化 RTC_InitTypeDef 结构体成员
  * @param  RTC_InitStruct: 指向 RTC_InitTypeDef 结构体的指针，将被初始化
  * @retval None
  * @note   默认值：
  *         - RTC_HourFormat   : 24 小时制
  *         - RTC_AsynchPrediv : 0x7F
  *         - RTC_SynchPrediv  : 0xFF
  * @details 此函数常在 RTC 初始化前使用，用于将配置结构体重置为安全默认值，
  *          避免未初始化导致的寄存器写入异常。
  */
void myRTC_StructInit(RTC_InitTypeDef* RTC_InitStruct)
{
    RTC_InitStruct->RTC_HourFormat   = RTC_HourFormat_24;  // 默认 24 小时制
    RTC_InitStruct->RTC_AsynchPrediv = 0x7F;               // 默认异步分频器
    RTC_InitStruct->RTC_SynchPrediv  = 0xFF;               // 默认同步分频器
}

/**
  * @brief  启用或禁用 RTC 寄存器写保护
  * @param  NewState: 写保护状态，可取值：
  *          - ENABLE  : 启用写保护
  *          - DISABLE : 禁用写保护
  * @retval None
  * @note   1. 所有 RTC 寄存器受写保护，除 RTC_ISR[13:8]、RTC_TAFCR 和 RTC_BKPxR 外
  *         2. 写入错误的密钥会重新启用写保护
  *         3. 系统复位不会影响写保护机制
  * @details 写保护机制通过写入特定密钥控制：
  *         - 禁用写保护：先写 0xCA，再写 0x53
  *         - 启用写保护：写 0xFF
  *         可保证 RTC 寄存器在非初始化模式下不被误修改。
  */
void myRTC_WriteProtectionCmd(FunctionalState NewState)
{
    /* 参数检查 */
    assert_param(IS_FUNCTIONAL_STATE(NewState));

    /* 根据状态选择写保护或解锁，使用三目运算符简化 */
    (NewState != DISABLE) ? (RTC->WPR = 0xFF)   // 启用写保护
                          : (RTC->WPR = 0xCA, RTC->WPR = 0x53); // 禁用写保护
}


/**
  * @brief  使 RTC 进入初始化模式
  * @note   RTC 初始化模式受写保护，调用本函数前需先禁用写保护：
  *         RTC_WriteProtectionCmd(DISABLE)
  * @param  None
  * @retval ErrorStatus:
  *          - SUCCESS: RTC 已进入初始化模式
  *          - ERROR  : RTC 未进入初始化模式（超时或失败）
  * @details
  * 1. 检查 RTC 是否已经在初始化模式（ISR 寄存器 INITF 位）。
  * 2. 若未在初始化模式，设置 INIT 位，等待硬件进入初始化状态。
  * 3. 等待时间由 INITMODE_TIMEOUT 控制，防止死循环。
  * 4. 若超时未进入初始化模式，则返回 ERROR；否则返回 SUCCESS。
  * 5. 如果 RTC 已在初始化模式，直接返回 SUCCESS。
  */
ErrorStatus myRTC_EnterInitMode(void)
{
    __IO uint32_t initcounter = 0;               // 初始化等待计数器
    ErrorStatus status = ERROR;                  // 默认返回错误
    uint32_t initstatus = 0;

    // RTC 是否已在初始化模式
    if ((RTC->ISR & RTC_ISR_INITF) == RESET)
    {
        RTC->ISR = RTC_INIT_MASK;               // 设置 INIT 位，进入初始化模式

        // 等待 INITF 置位或超时
        do
        {
            initstatus = RTC->ISR & RTC_ISR_INITF;
            initcounter++;
        } while ((initcounter != INITMODE_TIMEOUT) && (initstatus == 0));

        // 根据 INITF 位判断是否成功进入初始化模式
        status = (initstatus != 0) ? SUCCESS : ERROR;
    }
    else
    {
        status = SUCCESS;  // 已在初始化模式，直接返回成功
    }

    return status;
}

/**
  * @brief  退出 RTC 初始化模式
  * @note   当初始化完成后，RTC 日历计数将在 4 个 RTCCLK 周期后重新开始计数。
  * @note   RTC 初始化模式受写保护，调用前需先禁用写保护：
  *         RTC_WriteProtectionCmd(DISABLE)
  * @param  None
  * @retval None
  * @details
  * 通过清除 ISR 寄存器的 INIT 位，使 RTC 退出初始化模式。
  */
void myRTC_ExitInitMode(void)
{ 
    RTC->ISR &= ~RTC_ISR_INIT;  // 清除 INIT 位，退出初始化模式
}

/**
  * @brief  等待 RTC 时间和日期寄存器（RTC_TR 和 RTC_DR）与 RTC APB 时钟同步
  * @note   RTC 重同步模式受写保护，调用前需先禁用写保护：
  *         RTC_WriteProtectionCmd(DISABLE)
  * @note   为通过影子寄存器读取日历：
  *         1. 初始化日历、日历更新或从低功耗模式唤醒后，必须先清除 RSF 标志。
  *         2. 然后等待 RSF 标志置位，表示 RTC_TR 和 RTC_DR 已正确同步。
  * @param  None
  * @retval ErrorStatus:
  *          - SUCCESS: RTC 寄存器已同步
  *          - ERROR  : RTC 寄存器未同步（超时或异常）
  * @details
  * 1. 禁用 RTC 写保护。
  * 2. 清除 RSF 标志，触发重同步。
  * 3. 等待 RSF 标志置位或超时（SYNCHRO_TIMEOUT）。
  * 4. 根据 RSF 标志判断同步状态，返回 SUCCESS 或 ERROR。
  * 5. 重新启用 RTC 写保护。
  */
ErrorStatus myRTC_WaitForSynchro(void)
{
    __IO uint32_t synchrocounter = 0;
    uint32_t synchrostatus = 0;
    
    // 禁用写保护
    RTC->WPR = 0xCA;
    RTC->WPR = 0x53;
    
    // 清除 RSF 标志，触发重同步
    RTC->ISR &= RTC_RSF_MASK;

    // 等待寄存器同步或超时
    do
    {
        synchrostatus = RTC->ISR & RTC_ISR_RSF;
        synchrocounter++;
    } while ((synchrocounter != SYNCHRO_TIMEOUT) && (synchrostatus == 0));

    // 判断同步状态并返回，使用三目运算符优化
    ErrorStatus status = (synchrostatus != 0) ? SUCCESS : ERROR;

    // 重新启用写保护
    RTC->WPR = 0xFF;

    return status;
}

/**
  * @brief  启用或禁用 RTC 参考时钟检测
  * @param  NewState: RTC 参考时钟的新状态
  *          此参数可取值：
  *            - ENABLE  : 启用参考时钟检测
  *            - DISABLE : 禁用参考时钟检测
  * @retval ErrorStatus:
  *          - SUCCESS: 参考时钟检测成功设置
  *          - ERROR  : 参考时钟检测设置失败（初始化模式未进入）
  * @note   RTC 参考时钟检测寄存器受写保护，调用前需禁用写保护：
  *         RTC_WriteProtectionCmd(DISABLE)
  * @details
  * 1. 禁用 RTC 写保护。
  * 2. 进入 RTC 初始化模式。
  * 3. 根据 NewState 启用或禁用 RTC_CR_REFCKON 位。
  * 4. 退出 RTC 初始化模式。
  * 5. 重新启用 RTC 写保护。
  */
ErrorStatus myRTC_RefClockCmd(FunctionalState NewState)
{ 
    ErrorStatus status = ERROR;

    // 参数检查
    assert_param(IS_FUNCTIONAL_STATE(NewState));

    // 禁用写保护
    RTC->WPR = 0xCA;
    RTC->WPR = 0x53;

    // 进入初始化模式并设置参考时钟
    status = (RTC_EnterInitMode() == SUCCESS) ? (
                 (NewState != DISABLE ? (RTC->CR |= RTC_CR_REFCKON) 
                                      : (RTC->CR &= ~RTC_CR_REFCKON)),
                 RTC_ExitInitMode(),
                 SUCCESS
             ) : ERROR;

    // 重新启用写保护
    RTC->WPR = 0xFF;

    return status;
}

/** @defgroup RTC_Group2 时间和日期配置函数
 *  @brief   时间和日期配置相关函数
 *
@verbatim   
 =============================================================================== 
                 ##### 时间和日期配置函数 #####
 ===============================================================================  
 
 [..] 本节提供用于设置和读取 RTC 日历（时间和日期）的函数。


/**
  * @brief   设置 RTC 当前时间
  * @param   RTC_Format: 指定时间数据格式
  *           - RTC_Format_BIN : 二进制格式
  *           - RTC_Format_BCD : BCD 格式
  * @param   RTC_TimeStruct: 指向 RTC_TimeTypeDef 结构体，包含时间配置信息
  * @retval  ErrorStatus:
  *           - SUCCESS : RTC 时间寄存器配置成功
  *           - ERROR   : RTC 时间寄存器配置失败
  * @note    RTC 时间寄存器受写保护，调用前应禁用写保护：
  *          RTC_WriteProtectionCmd(DISABLE)
  * @details
  * 1. 根据输入格式（BIN/BCD）检查小时、分钟、秒数是否合法。
  * 2. 将时间数据转换为 RTC_TR 寄存器格式。
  * 3. 进入 RTC 初始化模式，写入时间寄存器。
  * 4. 退出初始化模式，必要时等待同步完成。
  * 5. 重新启用写保护。
  */
ErrorStatus myRTC_SetTime(uint32_t RTC_Format, RTC_TimeTypeDef* RTC_TimeStruct)
{
    uint32_t tmpreg = 0;
    ErrorStatus status = ERROR;
    uint8_t is12Hour = ((RTC->CR & RTC_CR_FMT) != RESET) ? 1 : 0;

    // 参数检查
    assert_param(IS_RTC_FORMAT(RTC_Format));
    
    if (RTC_Format == RTC_Format_BIN) {
        is12Hour ? (assert_param(IS_RTC_HOUR12(RTC_TimeStruct->RTC_Hours)),
                    assert_param(IS_RTC_H12(RTC_TimeStruct->RTC_H12)))
                 : (RTC_TimeStruct->RTC_H12 = 0x00,
                    assert_param(IS_RTC_HOUR24(RTC_TimeStruct->RTC_Hours)));

        assert_param(IS_RTC_MINUTES(RTC_TimeStruct->RTC_Minutes));
        assert_param(IS_RTC_SECONDS(RTC_TimeStruct->RTC_Seconds));
        
        tmpreg = ((uint32_t)RTC_ByteToBcd2(RTC_TimeStruct->RTC_Hours) << 16) |
                 ((uint32_t)RTC_ByteToBcd2(RTC_TimeStruct->RTC_Minutes) << 8) |
                 ((uint32_t)RTC_ByteToBcd2(RTC_TimeStruct->RTC_Seconds)) |
                 ((uint32_t)RTC_TimeStruct->RTC_H12 << 16);
    }
	else { // BCD 格式
        is12Hour ? (tmpreg = ((uint32_t)RTC_TimeStruct->RTC_Hours << 16) |
                               ((uint32_t)RTC_TimeStruct->RTC_Minutes << 8) |
                               ((uint32_t)RTC_TimeStruct->RTC_Seconds) |
                               ((uint32_t)RTC_TimeStruct->RTC_H12 << 16),
                    assert_param(IS_RTC_HOUR12(RTC_Bcd2ToByte(RTC_TimeStruct->RTC_Hours))),
                    assert_param(IS_RTC_H12(RTC_TimeStruct->RTC_H12)))
                 : (RTC_TimeStruct->RTC_H12 = 0x00,
                    assert_param(IS_RTC_HOUR24(RTC_Bcd2ToByte(RTC_TimeStruct->RTC_Hours))));

        assert_param(IS_RTC_MINUTES(RTC_Bcd2ToByte(RTC_TimeStruct->RTC_Minutes)));
        assert_param(IS_RTC_SECONDS(RTC_Bcd2ToByte(RTC_TimeStruct->RTC_Seconds)));

        tmpreg = ((uint32_t)RTC_TimeStruct->RTC_Hours << 16) |
                 ((uint32_t)RTC_TimeStruct->RTC_Minutes << 8) |
                 ((uint32_t)RTC_TimeStruct->RTC_Seconds) |
                 ((uint32_t)RTC_TimeStruct->RTC_H12 << 16);
    }

    // 禁用写保护
    RTC->WPR = 0xCA;
    RTC->WPR = 0x53;

    // 进入初始化模式写入时间
    status = (RTC_EnterInitMode() == SUCCESS) ? (
                 RTC->TR = tmpreg & RTC_TR_RESERVED_MASK,
                 RTC_ExitInitMode(),
                 ((RTC->CR & RTC_CR_BYPSHAD) == RESET ? 
                     ((RTC_WaitForSynchro() == SUCCESS) ? SUCCESS : ERROR) : SUCCESS)
             ) : ERROR;

    // 重新启用写保护
    RTC->WPR = 0xFF;

    return status;
}



/**
  * @brief  将 RTC_TimeTypeDef 结构体的成员初始化为默认值
  *         （时间 = 00时:00分:00秒）。
  * @param  RTC_TimeStruct: 指向需要初始化的 RTC_TimeTypeDef 结构体指针
  * @retval None
  * @note   用于在设置 RTC 时间之前初始化结构体，避免未定义数据。
  */
void myRTC_TimeStructInit(RTC_TimeTypeDef* RTC_TimeStruct)
{
  /* 时间 = 00时:00分:00秒，默认 AM */
  RTC_TimeStruct->RTC_H12 = RTC_H12_AM;
  RTC_TimeStruct->RTC_Hours = 0;
  RTC_TimeStruct->RTC_Minutes = 0;
  RTC_TimeStruct->RTC_Seconds = 0; 
}

/**
  * @brief  获取 RTC 当前时间
  * @param  RTC_Format: 指定返回时间的格式，可取值：
  *            @arg RTC_Format_BIN: 二进制格式
  *            @arg RTC_Format_BCD: BCD 格式
  * @param  RTC_TimeStruct: 指向 RTC_TimeTypeDef 结构体指针，用于存储获取的时间
  * @retval None
  * @note   从 RTC_TR 寄存器读取当前时间，并根据 RTC_Format 转换格式。
  */
void myRTC_GetTime(uint32_t RTC_Format, RTC_TimeTypeDef* RTC_TimeStruct)
{
  uint32_t tmpreg = 0;

  /* 参数检查 */
  assert_param(IS_RTC_FORMAT(RTC_Format));

  /* 读取 RTC_TR 寄存器 */
  tmpreg = (uint32_t)(RTC->TR & RTC_TR_RESERVED_MASK); 
  
  /* 将寄存器值填充到结构体中 */
  RTC_TimeStruct->RTC_Hours   = (uint8_t)((tmpreg & (RTC_TR_HT | RTC_TR_HU)) >> 16);
  RTC_TimeStruct->RTC_Minutes = (uint8_t)((tmpreg & (RTC_TR_MNT | RTC_TR_MNU)) >> 8);
  RTC_TimeStruct->RTC_Seconds = (uint8_t)(tmpreg & (RTC_TR_ST | RTC_TR_SU));
  RTC_TimeStruct->RTC_H12     = (uint8_t)((tmpreg & (RTC_TR_PM)) >> 16);  

  /* 如果需要二进制格式，将 BCD 转换为二进制 */
  if (RTC_Format == RTC_Format_BIN)
  {
    RTC_TimeStruct->RTC_Hours   = (uint8_t)RTC_Bcd2ToByte(RTC_TimeStruct->RTC_Hours);
    RTC_TimeStruct->RTC_Minutes = (uint8_t)RTC_Bcd2ToByte(RTC_TimeStruct->RTC_Minutes);
    RTC_TimeStruct->RTC_Seconds = (uint8_t)RTC_Bcd2ToByte(RTC_TimeStruct->RTC_Seconds);   
  }
}

/**
  * @brief  获取 RTC 当前日历子秒值
  * @note   读取 SSR 寄存器后会冻结时间和日期寄存器，确保读取一致性。
  *         为了释放寄存器冻结，需要访问 DR 寄存器。
  * @param  None
  * @retval RTC 当前日历的子秒值
  */
uint32_t myRTC_GetSubSecond(void)
{
  uint32_t tmpreg = 0;
  
  /* 从 SSR 寄存器读取子秒值 */
  tmpreg = (uint32_t)(RTC->SSR);
  
  /* 访问 DR 寄存器以解冻时间寄存器 */
  (void)(RTC->DR);
  
  return tmpreg;
}



/**
  * @brief  设置 RTC 当前日期
  * @param  RTC_Format: 指定输入参数的格式，可取值：
  *            @arg RTC_Format_BIN: 二进制格式
  *            @arg RTC_Format_BCD: BCD 格式
  * @param  RTC_DateStruct: 指向 RTC_DateTypeDef 结构体指针，包含日期配置信息
  * @retval An ErrorStatus 枚举值:
  *          - SUCCESS: RTC 日期寄存器已配置
  *          - ERROR: RTC 日期寄存器配置失败
  * @note   调用前应先关闭写保护（RTC_WriteProtectionCmd(DISABLE)），
  *         设置完成后会自动恢复写保护。
  *         如果 RTC_CR_BYPSHAD 位为 0，则会等待寄存器同步完成。
  */
ErrorStatus myRTC_SetDate(uint32_t RTC_Format, RTC_DateTypeDef* RTC_DateStruct)
{
  uint32_t tmpreg = 0;
  ErrorStatus status = ERROR;

  /* 参数检查 */
  assert_param(IS_RTC_FORMAT(RTC_Format));
  assert_param(IS_RTC_WEEKDAY(RTC_DateStruct->RTC_WeekDay));

  /* BCD 或二进制参数验证 */
  if (RTC_Format == RTC_Format_BIN)
  {
    /* 特殊处理月份 > 9 的情况，BCD 转换 */
    if ((RTC_DateStruct->RTC_Month & 0x10) == 0x10)
      RTC_DateStruct->RTC_Month = (RTC_DateStruct->RTC_Month & ~0x10) + 0x0A;

    assert_param(IS_RTC_YEAR(RTC_DateStruct->RTC_Year));
    assert_param(IS_RTC_MONTH(RTC_DateStruct->RTC_Month));
    assert_param(IS_RTC_DATE(RTC_DateStruct->RTC_Date));
    
    tmpreg = ((uint32_t)RTC_ByteToBcd2(RTC_DateStruct->RTC_Year) << 16) |
             ((uint32_t)RTC_ByteToBcd2(RTC_DateStruct->RTC_Month) << 8) |
             ((uint32_t)RTC_ByteToBcd2(RTC_DateStruct->RTC_Date)) |
             ((uint32_t)RTC_DateStruct->RTC_WeekDay << 13);
  }
  else
  {
    assert_param(IS_RTC_YEAR(RTC_Bcd2ToByte(RTC_DateStruct->RTC_Year)));
    assert_param(IS_RTC_MONTH(RTC_Bcd2ToByte(RTC_DateStruct->RTC_Month)));
    assert_param(IS_RTC_DATE(RTC_Bcd2ToByte(RTC_DateStruct->RTC_Date)));

    tmpreg = ((uint32_t)RTC_DateStruct->RTC_Year << 16) |
             ((uint32_t)RTC_DateStruct->RTC_Month << 8) |
             ((uint32_t)RTC_DateStruct->RTC_Date) |
             ((uint32_t)RTC_DateStruct->RTC_WeekDay << 13);
  }

  /* 禁止写保护 */
  RTC->WPR = 0xCA;
  RTC->WPR = 0x53;

  /* 进入初始化模式 */
  status = (RTC_EnterInitMode() == SUCCESS) ? SUCCESS : ERROR;

  if (status == SUCCESS)
  {
    /* 设置 RTC_DR 寄存器 */
    RTC->DR = tmpreg & RTC_DR_RESERVED_MASK;

    /* 退出初始化模式 */
    RTC_ExitInitMode();

    /* 如果 BYPSHAD=0，则等待同步 */
    status = ((RTC->CR & RTC_CR_BYPSHAD) == RESET) ?
              ((RTC_WaitForSynchro() == SUCCESS) ? SUCCESS : ERROR) :
              SUCCESS;
  }

  /* 恢复写保护 */
  RTC->WPR = 0xFF;

  return status;
}


/**
  * @brief  用默认值初始化 RTC_DateTypeDef 结构体
  * @param  RTC_DateStruct: 指向需要初始化的 RTC_DateTypeDef 结构体指针
  * @retval None
  * @note   默认值为：星期一，1月1日，年份 00（即 xx00 年）
  */
void myRTC_DateStructInit(RTC_DateTypeDef* RTC_DateStruct)
{
  RTC_DateStruct->RTC_WeekDay = RTC_Weekday_Monday;   /* 星期一 */
  RTC_DateStruct->RTC_Date = 1;                        /* 日期 1 */
  RTC_DateStruct->RTC_Month = RTC_Month_January;       /* 1 月 */
  RTC_DateStruct->RTC_Year = 0;                        /* 年份 00 */
}

/**
  * @brief  获取 RTC 当前日期
  * @param  RTC_Format: 指定返回参数的格式，可取值：
  *            @arg RTC_Format_BIN: 二进制格式
  *            @arg RTC_Format_BCD: BCD 格式
  * @param  RTC_DateStruct: 指向 RTC_DateTypeDef 结构体指针，用于存储读取到的日期信息
  * @retval None
  * @note   读取 RTC_DR 寄存器获取日期信息，如果指定格式为二进制，将 BCD 转换为二进制。
  *         RTC_DR 寄存器字段：
  *           - 年份：RTC_DR_YT | RTC_DR_YU
  *           - 月份：RTC_DR_MT | RTC_DR_MU
  *           - 日期：RTC_DR_DT | RTC_DR_DU
  *           - 星期：RTC_DR_WDU
  */
void myRTC_GetDate(uint32_t RTC_Format, RTC_DateTypeDef* RTC_DateStruct)
{
  uint32_t tmpreg = 0;

  /* 参数检查 */
  assert_param(IS_RTC_FORMAT(RTC_Format));
  
  /* 读取 RTC_DR 寄存器并屏蔽保留位 */
  tmpreg = RTC->DR & RTC_DR_RESERVED_MASK; 

  /* 将寄存器值填入结构体 */
  RTC_DateStruct->RTC_Year = (uint8_t)((tmpreg & (RTC_DR_YT | RTC_DR_YU)) >> 16);
  RTC_DateStruct->RTC_Month = (uint8_t)((tmpreg & (RTC_DR_MT | RTC_DR_MU)) >> 8);
  RTC_DateStruct->RTC_Date = (uint8_t)(tmpreg & (RTC_DR_DT | RTC_DR_DU));
  RTC_DateStruct->RTC_WeekDay = (uint8_t)((tmpreg & RTC_DR_WDU) >> 13);

  /* 若指定为二进制格式，则进行 BCD 转换 */
  if (RTC_Format == RTC_Format_BIN)
  {
    RTC_DateStruct->RTC_Year  = (uint8_t)RTC_Bcd2ToByte(RTC_DateStruct->RTC_Year);
    RTC_DateStruct->RTC_Month = (uint8_t)RTC_Bcd2ToByte(RTC_DateStruct->RTC_Month);
    RTC_DateStruct->RTC_Date  = (uint8_t)RTC_Bcd2ToByte(RTC_DateStruct->RTC_Date);
  }
}

/**
  * @}
  */

/** @defgroup RTC_Group3 闹钟配置函数
 *  @brief   闹钟（闹钟A和闹钟B）配置函数
 *
@verbatim   
 =============================================================================== 
         ##### 闹钟A和闹钟B配置函数 #####
 ===============================================================================  
 
 [..] 本部分提供用于设置和读取 RTC 闹钟的函数。

@endverbatim
  * @{
  */


/**
  * @brief  设置指定的 RTC 闹钟（闹钟 A 或 B）。
  * @note   闹钟寄存器只能在对应闹钟被禁用时写入（使用 RTC_AlarmCmd(DISABLE) 禁用）。
  * @param  RTC_Format: 指定传入时间参数的格式，可选值：
  *            @arg RTC_Format_BIN: 二进制格式
  *            @arg RTC_Format_BCD: BCD 格式
  * @param  RTC_Alarm: 指定要配置的闹钟，可选值：
  *            @arg RTC_Alarm_A: 闹钟 A
  *            @arg RTC_Alarm_B: 闹钟 B
  * @param  RTC_AlarmStruct: 指向 RTC_AlarmTypeDef 结构体的指针，包含闹钟的时间和日期配置参数
  * @retval None
  */
void RTC_SetAlarm(uint32_t RTC_Format, uint32_t RTC_Alarm, RTC_AlarmTypeDef* RTC_AlarmStruct)
{
    uint32_t tmpreg = 0;

    /* 参数检查 */
    assert_param(IS_RTC_FORMAT(RTC_Format));
    assert_param(IS_RTC_ALARM(RTC_Alarm));
    assert_param(IS_ALARM_MASK(RTC_AlarmStruct->RTC_AlarmMask));
    assert_param(IS_RTC_ALARM_DATE_WEEKDAY_SEL(RTC_AlarmStruct->RTC_AlarmDateWeekDaySel));

    /* 校验并转换时间格式 */
    if (RTC_Format == RTC_Format_BIN)
    {
        if ((RTC->CR & RTC_CR_FMT) != 0)  /* 12 小时格式 */
        {
            assert_param(IS_RTC_HOUR12(RTC_AlarmStruct->RTC_AlarmTime.RTC_Hours));
            assert_param(IS_RTC_H12(RTC_AlarmStruct->RTC_AlarmTime.RTC_H12));
        }
        else  /* 24 小时格式 */
        {
            RTC_AlarmStruct->RTC_AlarmTime.RTC_H12 = 0x00;
            assert_param(IS_RTC_HOUR24(RTC_AlarmStruct->RTC_AlarmTime.RTC_Hours));
        }
        assert_param(IS_RTC_MINUTES(RTC_AlarmStruct->RTC_AlarmTime.RTC_Minutes));
        assert_param(IS_RTC_SECONDS(RTC_AlarmStruct->RTC_AlarmTime.RTC_Seconds));

        /* 校验闹钟日期/星期选择 */
        if(RTC_AlarmStruct->RTC_AlarmDateWeekDaySel == RTC_AlarmDateWeekDaySel_Date)
            assert_param(IS_RTC_ALARM_DATE_WEEKDAY_DATE(RTC_AlarmStruct->RTC_AlarmDateWeekDay));
        else
            assert_param(IS_RTC_ALARM_DATE_WEEKDAY_WEEKDAY(RTC_AlarmStruct->RTC_AlarmDateWeekDay));
    }
    else /* BCD 格式 */
    {
        uint32_t tmp;

        tmp = (RTC->CR & RTC_CR_FMT) ? RTC_Bcd2ToByte(RTC_AlarmStruct->RTC_AlarmTime.RTC_Hours) : 0;
        if ((RTC->CR & RTC_CR_FMT) != 0)
        {
            assert_param(IS_RTC_HOUR12(tmp));
            assert_param(IS_RTC_H12(RTC_AlarmStruct->RTC_AlarmTime.RTC_H12));
        }
        else
        {
            RTC_AlarmStruct->RTC_AlarmTime.RTC_H12 = 0x00;
            assert_param(IS_RTC_HOUR24(tmp));
        }

        assert_param(IS_RTC_MINUTES(RTC_Bcd2ToByte(RTC_AlarmStruct->RTC_AlarmTime.RTC_Minutes)));
        assert_param(IS_RTC_SECONDS(RTC_Bcd2ToByte(RTC_AlarmStruct->RTC_AlarmTime.RTC_Seconds)));

        tmp = RTC_Bcd2ToByte(RTC_AlarmStruct->RTC_AlarmDateWeekDay);
        if(RTC_AlarmStruct->RTC_AlarmDateWeekDaySel == RTC_AlarmDateWeekDaySel_Date)
            assert_param(IS_RTC_ALARM_DATE_WEEKDAY_DATE(tmp));
        else
            assert_param(IS_RTC_ALARM_DATE_WEEKDAY_WEEKDAY(tmp));
    }

    /* 构造闹钟寄存器值 */
    tmpreg = (RTC_Format != RTC_Format_BIN) ?
        ((uint32_t)RTC_AlarmStruct->RTC_AlarmTime.RTC_Hours << 16 |
         (uint32_t)RTC_AlarmStruct->RTC_AlarmTime.RTC_Minutes << 8 |
         (uint32_t)RTC_AlarmStruct->RTC_AlarmTime.RTC_Seconds |
         (uint32_t)RTC_AlarmStruct->RTC_AlarmTime.RTC_H12 << 16 |
         (uint32_t)RTC_AlarmStruct->RTC_AlarmDateWeekDay << 24 |
         (uint32_t)RTC_AlarmStruct->RTC_AlarmDateWeekDaySel |
         (uint32_t)RTC_AlarmStruct->RTC_AlarmMask)
        :
        ((uint32_t)RTC_ByteToBcd2(RTC_AlarmStruct->RTC_AlarmTime.RTC_Hours) << 16 |
         (uint32_t)RTC_ByteToBcd2(RTC_AlarmStruct->RTC_AlarmTime.RTC_Minutes) << 8 |
         (uint32_t)RTC_ByteToBcd2(RTC_AlarmStruct->RTC_AlarmTime.RTC_Seconds) |
         (uint32_t)RTC_AlarmStruct->RTC_AlarmTime.RTC_H12 << 16 |
         (uint32_t)RTC_ByteToBcd2(RTC_AlarmStruct->RTC_AlarmDateWeekDay) << 24 |
         (uint32_t)RTC_AlarmStruct->RTC_AlarmDateWeekDaySel |
         (uint32_t)RTC_AlarmStruct->RTC_AlarmMask);

    /* 禁用 RTC 寄存器写保护 */
    RTC->WPR = 0xCA;
    RTC->WPR = 0x53;

    /* 写入闹钟寄存器 */
    if (RTC_Alarm == RTC_Alarm_A)
        RTC->ALRMAR = tmpreg;
    else
        RTC->ALRMBR = tmpreg;

    /* 启用 RTC 寄存器写保护 */
    RTC->WPR = 0xFF;
}


/**
  * @brief  用默认值填充 RTC_AlarmStruct 结构体成员。
  * @note   默认时间为 00h:00min:00sec，日期为每月第一天，掩码为全部字段未屏蔽。
  * @param  RTC_AlarmStruct: 指向将被初始化的 @ref RTC_AlarmTypeDef 结构体的指针。
  * @retval None
  */
void myRTC_AlarmStructInit(RTC_AlarmTypeDef* RTC_AlarmStruct)
{
    /* 闹钟时间初始化: 00h:00min:00sec */
    RTC_AlarmStruct->RTC_AlarmTime.RTC_H12 = RTC_H12_AM;
    RTC_AlarmStruct->RTC_AlarmTime.RTC_Hours = 0;
    RTC_AlarmStruct->RTC_AlarmTime.RTC_Minutes = 0;
    RTC_AlarmStruct->RTC_AlarmTime.RTC_Seconds = 0;

    /* 闹钟日期初始化: 每月第一天 */
    RTC_AlarmStruct->RTC_AlarmDateWeekDaySel = RTC_AlarmDateWeekDaySel_Date;
    RTC_AlarmStruct->RTC_AlarmDateWeekDay = 1;

    /* 闹钟掩码初始化: 所有字段未屏蔽 */
    RTC_AlarmStruct->RTC_AlarmMask = RTC_AlarmMask_None;
}

/**
  * @brief  获取指定 RTC 闹钟的配置值和掩码。
  * @param  RTC_Format: 指定输出参数的格式，可选值：
  *            @arg RTC_Format_BIN: 二进制格式
  *            @arg RTC_Format_BCD: BCD 格式
  * @param  RTC_Alarm: 指定要读取的闹钟，可选值：
  *            @arg RTC_Alarm_A: 读取闹钟 A
  *            @arg RTC_Alarm_B: 读取闹钟 B
  * @param  RTC_AlarmStruct: 指向 @ref RTC_AlarmTypeDef 结构体的指针，用于存放读取到的闹钟配置
  * @retval None
  */
void myRTC_GetAlarm(uint32_t RTC_Format, uint32_t RTC_Alarm, RTC_AlarmTypeDef* RTC_AlarmStruct)
{
    uint32_t tmpreg = 0;

    /* 参数检查 */
    assert_param(IS_RTC_FORMAT(RTC_Format));
    assert_param(IS_RTC_ALARM(RTC_Alarm));

    /* 读取指定闹钟寄存器 */
    tmpreg = (RTC_Alarm == RTC_Alarm_A) ? RTC->ALRMAR : RTC->ALRMBR;

    /* 解析寄存器值填充结构体 */
    RTC_AlarmStruct->RTC_AlarmTime.RTC_Hours = (tmpreg & (RTC_ALRMAR_HT | RTC_ALRMAR_HU)) >> 16;
    RTC_AlarmStruct->RTC_AlarmTime.RTC_Minutes = (tmpreg & (RTC_ALRMAR_MNT | RTC_ALRMAR_MNU)) >> 8;
    RTC_AlarmStruct->RTC_AlarmTime.RTC_Seconds = tmpreg & (RTC_ALRMAR_ST | RTC_ALRMAR_SU);
    RTC_AlarmStruct->RTC_AlarmTime.RTC_H12 = (tmpreg & RTC_ALRMAR_PM) >> 16;

    RTC_AlarmStruct->RTC_AlarmDateWeekDay = (tmpreg & (RTC_ALRMAR_DT | RTC_ALRMAR_DU)) >> 24;
    RTC_AlarmStruct->RTC_AlarmDateWeekDaySel = tmpreg & RTC_ALRMAR_WDSEL;
    RTC_AlarmStruct->RTC_AlarmMask = tmpreg & RTC_AlarmMask_All;

    /* 如果格式为二进制，则将 BCD 转为二进制 */
    if (RTC_Format == RTC_Format_BIN)
    {
        RTC_AlarmStruct->RTC_AlarmTime.RTC_Hours = RTC_Bcd2ToByte(RTC_AlarmStruct->RTC_AlarmTime.RTC_Hours);
        RTC_AlarmStruct->RTC_AlarmTime.RTC_Minutes = RTC_Bcd2ToByte(RTC_AlarmStruct->RTC_AlarmTime.RTC_Minutes);
        RTC_AlarmStruct->RTC_AlarmTime.RTC_Seconds = RTC_Bcd2ToByte(RTC_AlarmStruct->RTC_AlarmTime.RTC_Seconds);
        RTC_AlarmStruct->RTC_AlarmDateWeekDay = RTC_Bcd2ToByte(RTC_AlarmStruct->RTC_AlarmDateWeekDay);
    }
}


/**
  * @brief  使能或禁用指定的 RTC 闹钟。
  * @param  RTC_Alarm: 指定要配置的闹钟，可为以下任意组合：
  *            @arg RTC_Alarm_A: 选择闹钟 A
  *            @arg RTC_Alarm_B: 选择闹钟 B
  * @param  NewState: 闹钟的新状态，可为：
  *            @arg ENABLE : 使能闹钟
  *            @arg DISABLE: 禁用闹钟
  * @retval An ErrorStatus enumeration value:
  *          - SUCCESS: 闹钟成功使能/禁用
  *          - ERROR  : 闹钟未能成功使能/禁用
  */
ErrorStatus myRTC_AlarmCmd(uint32_t RTC_Alarm, FunctionalState NewState)
{
    __IO uint32_t alarmcounter = 0x00;
    uint32_t alarmstatus = 0x00;
    ErrorStatus status = ERROR;

    /* 参数检查 */
    assert_param(IS_RTC_CMD_ALARM(RTC_Alarm));
    assert_param(IS_FUNCTIONAL_STATE(NewState));

    /* 禁止写保护 */
    RTC->WPR = 0xCA;
    RTC->WPR = 0x53;

    if (NewState != DISABLE)
    {
        /* 使能指定闹钟 */
        RTC->CR |= RTC_Alarm;
        status = SUCCESS;
    }
    else
    {
        /* 禁用指定闹钟 */
        RTC->CR &= ~RTC_Alarm;

        /* 等待 ALRxWF 标志置位，确保可以修改闹钟寄存器 */
        do
        {
            alarmstatus = RTC->ISR & (RTC_Alarm >> 8);
            alarmcounter++;
        } while ((alarmcounter != INITMODE_TIMEOUT) && (alarmstatus == 0x00));

        status = ((RTC->ISR & (RTC_Alarm >> 8)) != RESET) ? SUCCESS : ERROR;
    }

    /* 使能写保护 */
    RTC->WPR = 0xFF;

    return status;
}

/**
  * @brief  配置 RTC 闹钟 A/B 的亚秒值及掩码。
  * @note   仅在闹钟禁用状态下调用。
  * @param  RTC_Alarm: 指定要配置的闹钟：
  *            @arg RTC_Alarm_A: 闹钟 A
  *            @arg RTC_Alarm_B: 闹钟 B
  * @param  RTC_AlarmSubSecondValue: 指定亚秒值，可取 0~0x7FFF
  * @param  RTC_AlarmSubSecondMask: 指定亚秒掩码，可为以下任意组合：
  *            @arg RTC_AlarmSubSecondMask_All    : 所有亚秒位屏蔽
  *            @arg RTC_AlarmSubSecondMask_SS14_1 : SS[14:1] 不参与比较，仅 SS[0] 比较
  *            @arg ... (其他掩码模式按硬件手册)
  *            @arg RTC_AlarmSubSecondMask_None   : 所有亚秒位参与比较
  * @retval None
  */
void myRTC_AlarmSubSecondConfig(uint32_t RTC_Alarm, uint32_t RTC_AlarmSubSecondValue, uint32_t RTC_AlarmSubSecondMask)
{
    uint32_t tmpreg = 0;

    /* 参数检查 */
    assert_param(IS_RTC_ALARM(RTC_Alarm));
    assert_param(IS_RTC_ALARM_SUB_SECOND_VALUE(RTC_AlarmSubSecondValue));
    assert_param(IS_RTC_ALARM_SUB_SECOND_MASK(RTC_AlarmSubSecondMask));

    /* 禁止写保护 */
    RTC->WPR = 0xCA;
    RTC->WPR = 0x53;

    /* 配置闹钟亚秒寄存器 */
    tmpreg = RTC_AlarmSubSecondValue | RTC_AlarmSubSecondMask;
    if (RTC_Alarm == RTC_Alarm_A)
        RTC->ALRMASSR = tmpreg;
    else
        RTC->ALRMBSSR = tmpreg;

    /* 使能写保护 */
    RTC->WPR = 0xFF;
}

/**
  * @brief  获取 RTC 闹钟的亚秒值。
  * @param  RTC_Alarm: 指定要读取的闹钟：
  *            @arg RTC_Alarm_A: 闹钟 A
  *            @arg RTC_Alarm_B: 闹钟 B
  * @retval 返回指定闹钟的亚秒值。
  */
uint32_t myRTC_GetAlarmSubSecond(uint32_t RTC_Alarm)
{
    /* 使用三目运算符选择对应闹钟的亚秒寄存器值 */
    uint32_t tmpreg = (RTC_Alarm == RTC_Alarm_A) ? 
                      (RTC->ALRMASSR & RTC_ALRMASSR_SS) : 
                      (RTC->ALRMBSSR & RTC_ALRMBSSR_SS);

    return tmpreg;
}



/**
  * @}
  */

/** @defgroup RTC_Group4 RTC 唤醒定时器配置函数
 *  @brief   RTC 唤醒定时器配置相关函数
 *
@verbatim   
 ===============================================================================
                 ##### RTC 唤醒定时器配置函数 #####
 ===============================================================================  

 [..] 本节提供用于配置和读取 RTC 唤醒定时器的函数。
@endverbatim
  * @{
  */

/**
  * @brief  配置 RTC 唤醒定时器的时钟源。
  * @note   唤醒定时器的时钟源只能在 RTC 唤醒定时器被禁用时修改
  *         （使用 RTC_WakeUpCmd(DISABLE) 禁用定时器）。
  * @param  RTC_WakeUpClock: 唤醒定时器时钟源。
  *          可选值:
  *            @arg RTC_WakeUpClock_RTCCLK_Div16: RTC 时钟除以 16
  *            @arg RTC_WakeUpClock_RTCCLK_Div8:  RTC 时钟除以 8
  *            @arg RTC_WakeUpClock_RTCCLK_Div4:  RTC 时钟除以 4
  *            @arg RTC_WakeUpClock_RTCCLK_Div2:  RTC 时钟除以 2
  *            @arg RTC_WakeUpClock_CK_SPRE_16bits: 16 位 CK_SPRE 时钟
  *            @arg RTC_WakeUpClock_CK_SPRE_17bits: 17 位 CK_SPRE 时钟
  * @retval None
  */
void myRTC_WakeUpClockConfig(uint32_t RTC_WakeUpClock)
{
  assert_param(IS_RTC_WAKEUP_CLOCK(RTC_WakeUpClock));

  /* 关闭 RTC 寄存器写保护 */
  RTC->WPR = 0xCA;
  RTC->WPR = 0x53;

  /* 清除 CR 寄存器中 WUCKSEL 位 */
  RTC->CR &= ~RTC_CR_WUCKSEL;

  /* 配置唤醒定时器时钟源 */
  RTC->CR |= RTC_WakeUpClock;

  /* 重新开启 RTC 寄存器写保护 */
  RTC->WPR = 0xFF;
}

/**
  * @brief  配置 RTC 唤醒定时器计数器。
  * @note   唤醒定时器计数器只能在 RTC 唤醒定时器被禁用时写入
  *         （使用 RTC_WakeUpCmd(DISABLE) 禁用定时器）。
  * @param  RTC_WakeUpCounter: 唤醒定时器计数值，可取 0x0000~0xFFFF。
  * @retval None
  */
void myRTC_SetWakeUpCounter(uint32_t RTC_WakeUpCounter)
{
  assert_param(IS_RTC_WAKEUP_COUNTER(RTC_WakeUpCounter));

  /* 关闭写保护 */
  RTC->WPR = 0xCA;
  RTC->WPR = 0x53;

  /* 设置唤醒定时器计数器 */
  RTC->WUTR = RTC_WakeUpCounter;

  /* 开启写保护 */
  RTC->WPR = 0xFF;
}

/**
  * @brief  获取 RTC 唤醒定时器当前计数值。
  * @param  None
  * @retval RTC 唤醒定时器计数值
  */
uint32_t myRTC_GetWakeUpCounter(void)
{
  /* 返回 WUTR 寄存器中 WUT 位值 */
  return RTC->WUTR & RTC_WUTR_WUT;
}

/**
  * @brief  启用或禁用 RTC 唤醒定时器。
  * @param  NewState: 定时器新状态，可取值 ENABLE 或 DISABLE。
  * @retval ErrorStatus:
  *          - SUCCESS: 定时器启用/禁用成功
  *          - ERROR: 定时器启用/禁用失败
  */
ErrorStatus myRTC_WakeUpCmd(FunctionalState NewState)
{
    __IO uint32_t counter = 0;
    ErrorStatus status = ERROR;

    assert_param(IS_FUNCTIONAL_STATE(NewState));

    /* 关闭写保护 */
    RTC->WPR = 0xCA;
    RTC->WPR = 0x53;

    switch (NewState)
    {
        case ENABLE:
            /* 启用唤醒定时器 */
            RTC->CR |= RTC_CR_WUTE;
            status = SUCCESS;
            break;

        case DISABLE:
            /* 禁用唤醒定时器 */
            RTC->CR &= ~RTC_CR_WUTE;

            /* 等待 WUTWF 标志位置位 */
            while (((RTC->ISR & RTC_ISR_WUTWF) == 0) && (counter++ < INITMODE_TIMEOUT));

            status = ((RTC->ISR & RTC_ISR_WUTWF) != 0) ? SUCCESS : ERROR;
            break;

        default:
            /* 非法状态 */
            status = ERROR;
            break;
    }

    /* 开启写保护 */
    RTC->WPR = 0xFF;

    return status;
}


/**
  * @}
  */

/** @defgroup RTC_Group5 夏令时配置函数
	@brief 夏令时配置函数
 *
@verbatim   
 ===============================================================================
              #####      ##### 夏令时配置函数 ##########
 ===============================================================================  

 [..] 本节提供用于配置 RTC 夏令时的函数。
/**
  * @brief  配置 RTC 夏令时，增加或减少当前时间一小时。
  * @param  RTC_DayLightSaving: 指定时间调整操作。
  *          该参数可以是以下值之一：
  *            @arg RTC_DayLightSaving_SUB1H: 减少一小时（冬令时）
  *            @arg RTC_DayLightSaving_ADD1H: 增加一小时（夏令时）
  * @param  RTC_StoreOperation: 指定是否在 CR 寄存器的 BCK 位中保存该操作。
  *          该参数可以是以下值之一：
  *            @arg RTC_StoreOperation_Reset: 重置 BCK 位
  *            @arg RTC_StoreOperation_Set: 设置 BCK 位
  * @retval None
  * @note   调用此函数前需关闭写保护，函数内部已处理写保护开启和关闭。
  */
void myRTC_DayLightSavingConfig(uint32_t RTC_DayLightSaving, uint32_t RTC_StoreOperation)
{
  /* 参数检查 */
  assert_param(IS_RTC_DAYLIGHT_SAVING(RTC_DayLightSaving));
  assert_param(IS_RTC_STORE_OPERATION(RTC_StoreOperation));

  /* 关闭写保护 */
  RTC->WPR = 0xCA;
  RTC->WPR = 0x53;

  /* 清除 BCK 位 */
  RTC->CR &= ~RTC_CR_BCK;

  /* 配置夏令时和存储操作 */
  RTC->CR |= (RTC_DayLightSaving | RTC_StoreOperation);

  /* 开启写保护 */
  RTC->WPR = 0xFF; 
}

/**
  * @brief  获取 RTC 夏令时的存储操作状态。
  * @param  None
  * @retval RTC 夏令时存储操作状态：
  *          - RTC_StoreOperation_Reset: BCK 位为 0
  *          - RTC_StoreOperation_Set: BCK 位为 1
  * @note   返回值直接读取 CR 寄存器的 BCK 位。
  */
uint32_t myRTC_GetStoreOperation(void)
{
  return (RTC->CR & RTC_CR_BCK);
}

/**
  * @}
  */

/** @defgroup RTC_Group6 输出引脚配置函数
 *  @brief   输出引脚配置函数
 *
@verbatim   
 ===============================================================================  
                 ##### 输出引脚配置函数 #####
 ===============================================================================  

 [..] 本节提供了用于配置 RTC 输出信号源的函数。
/**
  * @brief  配置 RTC 输出信号源（AFO_ALARM）。
  * @param  RTC_Output: 指定要映射到 RTC 输出的信号。
  *          该参数可以是以下值之一：
  *            @arg RTC_Output_Disable: 不输出任何信号
  *            @arg RTC_Output_AlarmA: Alarm A 信号映射到输出
  *            @arg RTC_Output_AlarmB: Alarm B 信号映射到输出
  *            @arg RTC_Output_WakeUp: WakeUp 信号映射到输出
  * @param  RTC_OutputPolarity: 指定输出信号的极性。
  *          该参数可以是以下值之一：
  *            @arg RTC_OutputPolarity_High: 当 ALRAF/ALRBF/WUTF 为高电平时，输出引脚为高电平（取决于 OSEL 配置）
  *            @arg RTC_OutputPolarity_Low: 当 ALRAF/ALRBF/WUTF 为高电平时，输出引脚为低电平（取决于 OSEL 配置）
  * @retval None
  */
void myRTC_OutputConfig(uint32_t RTC_Output, uint32_t RTC_OutputPolarity)
{
  /* 检查参数有效性 */
  assert_param(IS_RTC_OUTPUT(RTC_Output));
  assert_param(IS_RTC_OUTPUT_POL(RTC_OutputPolarity));

  /* 关闭 RTC 寄存器写保护 */
  RTC->WPR = 0xCA;
  RTC->WPR = 0x53;

  /* 清除需要配置的位 */
  RTC->CR &= (uint32_t)~(RTC_CR_OSEL | RTC_CR_POL);

  /* 配置输出选择和极性 */
  RTC->CR |= (uint32_t)(RTC_Output | RTC_OutputPolarity);

  /* 开启 RTC 寄存器写保护 */
  RTC->WPR = 0xFF; 
}


/**
  * @}
  */

/** @defgroup RTC_Group7 数字校准配置函数
 *  @brief   粗校准配置函数
 *
@verbatim   
 =============================================================================== 
              ##### 数字校准配置函数 #####
 ===============================================================================  
 [..] 本节提供用于配置RTC粗校准功能的函数。
@endverbatim
  * @{
  */

/**
  * @brief  配置RTC粗校准参数。
  * @param  RTC_CalibSign: 指定粗校准值的符号。
  *          该参数可以是以下值之一：
  *            @arg RTC_CalibSign_Positive: 校准值为正
  *            @arg RTC_CalibSign_Negative: 校准值为负
  * @param  Value: 粗校准值，单位为ppm（5位编码）。
  * @note   使用负号时，校准值应在0~63之间，步长为2 ppm。
  * @note   使用正号时，校准值应在0~126之间，步长为4 ppm。
  * @retval 返回ErrorStatus枚举值：
  *          - SUCCESS: RTC粗校准设置成功
  *          - ERROR: RTC粗校准设置失败
  */
ErrorStatus myRTC_CoarseCalibConfig(uint32_t RTC_CalibSign, uint32_t Value)
{
  ErrorStatus status = ERROR;
   
  /* 参数检查 */
  assert_param(IS_RTC_CALIB_SIGN(RTC_CalibSign));
  assert_param(IS_RTC_CALIB_VALUE(Value)); 

  /* 关闭写保护 */
  RTC->WPR = 0xCA;
  RTC->WPR = 0x53;

  /* 进入初始化模式 */
  if (RTC_EnterInitMode() == ERROR)
  {
    status = ERROR;
  } 
  else
  {
    /* 设置粗校准寄存器 */
    RTC->CALIBR = (uint32_t)(RTC_CalibSign | Value);
    /* 退出初始化模式 */
    RTC_ExitInitMode();
    
    status = SUCCESS;
  } 

  /* 开启写保护 */
  RTC->WPR = 0xFF; 
  
  return status;
}

/**
  * @brief  启用或禁用RTC粗校准功能。
  * @param  NewState: 粗校准的新状态。
  *          该参数可以是：ENABLE 或 DISABLE。
  * @retval 返回ErrorStatus枚举值：
  *          - SUCCESS: RTC粗校准已启用/禁用
  *          - ERROR: RTC粗校准未启用/禁用
  */
ErrorStatus myRTC_CoarseCalibCmd(FunctionalState NewState)
{
  ErrorStatus status = ERROR;
  
  /* 参数检查 */
  assert_param(IS_FUNCTIONAL_STATE(NewState));

  /* 关闭写保护 */
  RTC->WPR = 0xCA;
  RTC->WPR = 0x53;
  
  /* 进入初始化模式 */
  if (RTC_EnterInitMode() == ERROR)
  {
    status =  ERROR;
  }
  else
  {
    /* 根据NewState设置粗校准使能位 */
    if (NewState != DISABLE)
      RTC->CR |= (uint32_t)RTC_CR_DCE;  /* 启用粗校准 */
    else
      RTC->CR &= (uint32_t)~RTC_CR_DCE; /* 禁用粗校准 */

    /* 退出初始化模式 */
    RTC_ExitInitMode();
    status = SUCCESS;
  } 
  
  /* 开启写保护 */
  RTC->WPR = 0xFF; 
  
  return status;
}

/**
  * @brief  启用或禁用RTC数字校准时钟输出。
  * @param  NewState: 数字校准输出的新状态。
  *          该参数可以是：ENABLE 或 DISABLE。
  * @retval None
  */
void myRTC_CalibOutputCmd(FunctionalState NewState)
{
  /* 参数检查 */
  assert_param(IS_FUNCTIONAL_STATE(NewState));
  
  /* 关闭写保护 */
  RTC->WPR = 0xCA;
  RTC->WPR = 0x53;
  
  /* 设置或清除数字校准输出使能位 */
  if (NewState != DISABLE)
    RTC->CR |= (uint32_t)RTC_CR_COE;  /* 启用输出 */
  else
    RTC->CR &= (uint32_t)~RTC_CR_COE; /* 禁用输出 */
  
  /* 开启写保护 */
  RTC->WPR = 0xFF; 
}


/**
  * @brief  配置RTC校准引脚输出 (RTC_CALIB) 频率选择 (1Hz 或 512Hz)。
  * @param  RTC_CalibOutput: 校准输出选择。
  *          该参数可以是以下值之一：
  *            @arg RTC_CalibOutput_512Hz: 输出信号为512Hz。
  *            @arg RTC_CalibOutput_1Hz  : 输出信号为1Hz。
  * @retval None
  */
void myRTC_CalibOutputConfig(uint32_t RTC_CalibOutput)
{
  /* 参数检查 */
  assert_param(IS_RTC_CALIB_OUTPUT(RTC_CalibOutput));

  /* 关闭RTC寄存器写保护 */
  RTC->WPR = 0xCA;
  RTC->WPR = 0x53;
  
  /* 清除配置位 */
  RTC->CR &= (uint32_t)~(RTC_CR_COSEL);

  /* 配置RTC_CR寄存器 */
  RTC->CR |= (uint32_t)RTC_CalibOutput;

  /* 开启RTC寄存器写保护 */
  RTC->WPR = 0xFF;
}

/**
  * @brief  配置RTC平滑校准设置。
  * @param  RTC_SmoothCalibPeriod: 平滑校准周期选择。
  *          该参数可以是以下值之一：
  *            @arg RTC_SmoothCalibPeriod_32sec : 校准周期为32秒
  *            @arg RTC_SmoothCalibPeriod_16sec : 校准周期为16秒
  *            @arg RTC_SmoothCalibPeriod_8sec  : 校准周期为8秒
  * @param  RTC_SmoothCalibPlusPulses: 设置或清除CALP位。
  *          该参数可以是以下值之一：
  *            @arg RTC_SmoothCalibPlusPulses_Set   : 每2^11个脉冲增加1个RTCCLK脉冲
  *            @arg RTC_SmoothCalibPlusPulses_Reset : 不增加RTCCLK脉冲
  * @param  RTC_SmouthCalibMinusPulsesValue: CALM[8:0]位的值，范围为0~0x1FF。
  * @retval ErrorStatus枚举值：
  *          - SUCCESS: RTC平滑校准寄存器配置成功
  *          - ERROR  : RTC平滑校准寄存器配置失败
  */
ErrorStatus myRTC_SmoothCalibConfig(uint32_t RTC_SmoothCalibPeriod,
                                  uint32_t RTC_SmoothCalibPlusPulses,
                                  uint32_t RTC_SmouthCalibMinusPulsesValue)
{
  ErrorStatus status = ERROR;
  uint32_t recalpfcount = 0;

  /* 参数检查 */
  assert_param(IS_RTC_SMOOTH_CALIB_PERIOD(RTC_SmoothCalibPeriod));
  assert_param(IS_RTC_SMOOTH_CALIB_PLUS(RTC_SmoothCalibPlusPulses));
  assert_param(IS_RTC_SMOOTH_CALIB_MINUS(RTC_SmouthCalibMinusPulsesValue));

  /* 关闭RTC寄存器写保护 */
  RTC->WPR = 0xCA;
  RTC->WPR = 0x53;
  
  /* 检查是否有校准操作正在进行 */
  if ((RTC->ISR & RTC_ISR_RECALPF) != RESET)
  {
    /* 等待校准完成 */
    while (((RTC->ISR & RTC_ISR_RECALPF) != RESET) && (recalpfcount != RECALPF_TIMEOUT))
    {
      recalpfcount++;
    }
  }

  /* 如果校准完成或无校准操作，则进行配置 */
  if ((RTC->ISR & RTC_ISR_RECALPF) == RESET)
  {
    /* 配置平滑校准寄存器 */
    RTC->CALR = (uint32_t)(RTC_SmoothCalibPeriod | RTC_SmoothCalibPlusPulses | RTC_SmouthCalibMinusPulsesValue);
    status = SUCCESS;
  }
  else
  {
    status = ERROR;
  }

  /* 开启RTC寄存器写保护 */
  RTC->WPR = 0xFF;
  
  return status;
}


/**
  * @}
  */

/** @defgroup RTC_Group8 时间戳配置函数
 *  @brief   RTC时间戳配置函数
 *
@verbatim   
 ===============================================================================
                 ##### 时间戳配置函数 #####
 ===============================================================================  

 [..] 本节提供用于配置RTC时间戳功能的函数。
@endverbatim
  * @{
  */

/**
  * @brief  启用或禁用RTC时间戳功能，并配置触发时间戳的引脚边沿。
  * @param  RTC_TimeStampEdge: 指定触发时间戳的引脚边沿。
  *          该参数可以是以下值之一：
  *            @arg RTC_TimeStampEdge_Rising : 时间戳事件在上升沿触发。
  *            @arg RTC_TimeStampEdge_Falling: 时间戳事件在下降沿触发。
  * @param  NewState: 时间戳功能的新状态。
  *          该参数可以是：ENABLE 或 DISABLE。
  * @retval None
  */
void myRTC_TimeStampCmd(uint32_t RTC_TimeStampEdge, FunctionalState NewState)
{
  uint32_t tmpreg = 0;

  /* 参数检查 */
  assert_param(IS_RTC_TIMESTAMP_EDGE(RTC_TimeStampEdge));
  assert_param(IS_FUNCTIONAL_STATE(NewState));

  /* 获取RTC_CR寄存器，并清除待配置位 */
  tmpreg = (uint32_t)(RTC->CR & (uint32_t)~(RTC_CR_TSEDGE | RTC_CR_TSE));

  /* 配置新状态 */
  tmpreg |= (NewState != DISABLE) ? (RTC_TimeStampEdge | RTC_CR_TSE) : RTC_TimeStampEdge;

  /* 关闭写保护 */
  RTC->WPR = 0xCA;
  RTC->WPR = 0x53;

  /* 配置时间戳边沿及使能位 */
  RTC->CR = tmpreg;

  /* 开启写保护 */
  RTC->WPR = 0xFF; 
}

/**
  * @brief  获取RTC时间戳的时间和日期。
  * @param  RTC_Format: 输出参数格式。
  *          该参数可以是以下值之一：
  *            @arg RTC_Format_BIN: 二进制格式
  *            @arg RTC_Format_BCD: BCD格式
  * @param  RTC_StampTimeStruct: 指向RTC_TimeTypeDef结构体，存储时间戳时间值。
  * @param  RTC_StampDateStruct: 指向RTC_DateTypeDef结构体，存储时间戳日期值。
  * @retval None
  */
void myRTC_GetTimeStamp(uint32_t RTC_Format, RTC_TimeTypeDef* RTC_StampTimeStruct, 
                      RTC_DateTypeDef* RTC_StampDateStruct)
{
  uint32_t tmptime = 0, tmpdate = 0;

  /* 参数检查 */
  assert_param(IS_RTC_FORMAT(RTC_Format));

  /* 获取时间戳时间和日期寄存器值 */
  tmptime = RTC->TSTR & RTC_TR_RESERVED_MASK;
  tmpdate = RTC->TSDR & RTC_DR_RESERVED_MASK;

  /* 填充时间结构体 */
  RTC_StampTimeStruct->RTC_Hours   = (uint8_t)((tmptime & (RTC_TR_HT | RTC_TR_HU)) >> 16);
  RTC_StampTimeStruct->RTC_Minutes = (uint8_t)((tmptime & (RTC_TR_MNT | RTC_TR_MNU)) >> 8);
  RTC_StampTimeStruct->RTC_Seconds = (uint8_t)(tmptime & (RTC_TR_ST | RTC_TR_SU));
  RTC_StampTimeStruct->RTC_H12     = (uint8_t)((tmptime & RTC_TR_PM) >> 16);  

  /* 填充日期结构体 */
  RTC_StampDateStruct->RTC_Year    = 0;
  RTC_StampDateStruct->RTC_Month   = (uint8_t)((tmpdate & (RTC_DR_MT | RTC_DR_MU)) >> 8);
  RTC_StampDateStruct->RTC_Date    = (uint8_t)(tmpdate & (RTC_DR_DT | RTC_DR_DU));
  RTC_StampDateStruct->RTC_WeekDay = (uint8_t)((tmpdate & RTC_DR_WDU) >> 13);

  /* 根据格式将BCD转换为二进制 */
  if (RTC_Format == RTC_Format_BIN)
  {
    RTC_StampTimeStruct->RTC_Hours   = (uint8_t)RTC_Bcd2ToByte(RTC_StampTimeStruct->RTC_Hours);
    RTC_StampTimeStruct->RTC_Minutes = (uint8_t)RTC_Bcd2ToByte(RTC_StampTimeStruct->RTC_Minutes);
    RTC_StampTimeStruct->RTC_Seconds = (uint8_t)RTC_Bcd2ToByte(RTC_StampTimeStruct->RTC_Seconds);

    RTC_StampDateStruct->RTC_Month   = (uint8_t)RTC_Bcd2ToByte(RTC_StampDateStruct->RTC_Month);
    RTC_StampDateStruct->RTC_Date    = (uint8_t)RTC_Bcd2ToByte(RTC_StampDateStruct->RTC_Date);
    RTC_StampDateStruct->RTC_WeekDay = (uint8_t)RTC_Bcd2ToByte(RTC_StampDateStruct->RTC_WeekDay);
  }
}

/**
  * @brief  获取RTC时间戳的亚秒值。
  * @param  None
  * @retval RTC当前时间戳亚秒值。
  */
uint32_t myRTC_GetTimeStampSubSecond(void)
{
  /* 从对应寄存器获取亚秒值 */
  return RTC->TSSSR;
}


/**
  * @}
  */

/** @defgroup RTC_Group9 防篡改配置函数
 *  @brief   RTC防篡改（Tamper）配置函数
 *
@verbatim   
 ===============================================================================
                 ##### 防篡改配置函数 #####
 ===============================================================================  

 [..] 本节提供用于配置RTC防篡改功能的函数。
@endverbatim
  * @{
  */

/**
  * @brief  配置选定防篡改引脚的触发边沿。
  * @param  RTC_Tamper: 选定的防篡改引脚。
  *          该参数可以是 RTC_Tamper_1。
  * @param  RTC_TamperTrigger: 指定触发防篡改事件的引脚电平或边沿。
  *          该参数可以是以下值之一：
  *            @arg RTC_TamperTrigger_RisingEdge  : 上升沿触发防篡改事件
  *            @arg RTC_TamperTrigger_FallingEdge : 下降沿触发防篡改事件
  *            @arg RTC_TamperTrigger_LowLevel    : 低电平触发防篡改事件
  *            @arg RTC_TamperTrigger_HighLevel   : 高电平触发防篡改事件
  * @retval None
  */
void myRTC_TamperTriggerConfig(uint32_t RTC_Tamper, uint32_t RTC_TamperTrigger)
{
  assert_param(IS_RTC_TAMPER(RTC_Tamper)); 
  assert_param(IS_RTC_TAMPER_TRIGGER(RTC_TamperTrigger));
 
  if (RTC_TamperTrigger == RTC_TamperTrigger_RisingEdge)
    RTC->TAFCR &= ~((uint32_t)(RTC_Tamper << 1));
  else
    RTC->TAFCR |= (uint32_t)(RTC_Tamper << 1);  
}

/**
  * @brief  启用或禁用防篡改检测。
  * @param  RTC_Tamper: 选定的防篡改引脚。
  *          该参数可以是 RTC_Tamper_1。
  * @param  NewState: 防篡改引脚的新状态。
  *          该参数可以是：ENABLE 或 DISABLE。
  * @retval None
  */
void myRTC_TamperCmd(uint32_t RTC_Tamper, FunctionalState NewState)
{
  assert_param(IS_RTC_TAMPER(RTC_Tamper));  
  assert_param(IS_FUNCTIONAL_STATE(NewState));
  
  if (NewState != DISABLE)
    RTC->TAFCR |= RTC_Tamper;
  else
    RTC->TAFCR &= ~RTC_Tamper;    
}

/**
  * @brief  配置防篡改滤波器。
  * @param  RTC_TamperFilter: 指定防篡改滤波器。
  *          该参数可以是以下值之一：
  *            @arg RTC_TamperFilter_Disable : 禁用防篡改滤波器
  *            @arg RTC_TamperFilter_2Sample : 防篡改事件在连续2个采样有效后触发
  *            @arg RTC_TamperFilter_4Sample : 防篡改事件在连续4个采样有效后触发
  *            @arg RTC_TamperFilter_8Sample : 防篡改事件在连续8个采样有效后触发
  * @retval None
  */
void myRTC_TamperFilterConfig(uint32_t RTC_TamperFilter)
{
  assert_param(IS_RTC_TAMPER_FILTER(RTC_TamperFilter));
   
  RTC->TAFCR &= ~RTC_TAFCR_TAMPFLT;
  RTC->TAFCR |= RTC_TamperFilter;
}

/**
  * @brief  配置防篡改采样频率。
  * @param  RTC_TamperSamplingFreq: 指定防篡改采样频率。
  *          该参数可以是以下值之一：
  *            @arg RTC_TamperSamplingFreq_RTCCLK_Div32768: 采样频率 = RTCCLK / 32768
  *            @arg RTC_TamperSamplingFreq_RTCCLK_Div16384: 采样频率 = RTCCLK / 16384
  *            @arg RTC_TamperSamplingFreq_RTCCLK_Div8192 : 采样频率 = RTCCLK / 8192
  *            @arg RTC_TamperSamplingFreq_RTCCLK_Div4096 : 采样频率 = RTCCLK / 4096
  *            @arg RTC_TamperSamplingFreq_RTCCLK_Div2048 : 采样频率 = RTCCLK / 2048
  *            @arg RTC_TamperSamplingFreq_RTCCLK_Div1024 : 采样频率 = RTCCLK / 1024
  *            @arg RTC_TamperSamplingFreq_RTCCLK_Div512  : 采样频率 = RTCCLK / 512
  *            @arg RTC_TamperSamplingFreq_RTCCLK_Div256  : 采样频率 = RTCCLK / 256
  * @retval None
  */
void myRTC_TamperSamplingFreqConfig(uint32_t RTC_TamperSamplingFreq)
{
  assert_param(IS_RTC_TAMPER_SAMPLING_FREQ(RTC_TamperSamplingFreq));
 
  RTC->TAFCR &= ~RTC_TAFCR_TAMPFREQ;
  RTC->TAFCR |= RTC_TamperSamplingFreq;
}

/**
  * @brief  配置防篡改引脚的预充电时间。
  * @param  RTC_TamperPrechargeDuration: 指定防篡改引脚预充电时间。
  *          该参数可以是以下值之一：
  *            @arg RTC_TamperPrechargeDuration_1RTCCLK : 1个RTCCLK周期
  *            @arg RTC_TamperPrechargeDuration_2RTCCLK : 2个RTCCLK周期
  *            @arg RTC_TamperPrechargeDuration_4RTCCLK : 4个RTCCLK周期
  *            @arg RTC_TamperPrechargeDuration_8RTCCLK : 8个RTCCLK周期
  * @retval None
  */
void myRTC_TamperPinsPrechargeDuration(uint32_t RTC_TamperPrechargeDuration)
{
  assert_param(IS_RTC_TAMPER_PRECHARGE_DURATION(RTC_TamperPrechargeDuration));
   
  RTC->TAFCR &= ~RTC_TAFCR_TAMPPRCH;
  RTC->TAFCR |= RTC_TamperPrechargeDuration;
}

/**
  * @brief  启用或禁用在防篡改事件时记录时间戳。
  * @note   即使TSE位复位，时间戳仍然有效。   
  * @param  NewState: 防篡改事件时间戳的新状态。
  *          该参数可以是：ENABLE 或 DISABLE。
  * @retval None
  */
void myRTC_TimeStampOnTamperDetectionCmd(FunctionalState NewState)
{
  assert_param(IS_FUNCTIONAL_STATE(NewState));
   
  if (NewState != DISABLE)
    RTC->TAFCR |= RTC_TAFCR_TAMPTS;
  else
    RTC->TAFCR &= ~RTC_TAFCR_TAMPTS;    
}

/**
  * @brief  启用或禁用防篡改引脚的预充电（上拉）。
  * @param  NewState: 防篡改引脚预充电的新状态。
  *          该参数可以是：ENABLE 或 DISABLE。
  * @retval None
  */
void myRTC_TamperPullUpCmd(FunctionalState NewState)
{
  /* 检查参数 */
  assert_param(IS_FUNCTIONAL_STATE(NewState));
  
  /* 使用三目运算符设置防篡改引脚的预充电状态 */
  RTC->TAFCR = (NewState != DISABLE) ? (RTC->TAFCR & ~RTC_TAFCR_TAMPPUDIS)
                                     : (RTC->TAFCR | RTC_TAFCR_TAMPPUDIS);
}


/**
  * @}
  */

/** @defgroup RTC_Group10 备份数据寄存器配置函数
 *  @brief   备份数据寄存器的读写配置函数
 *
@verbatim   
 ===============================================================================
             ##### 备份数据寄存器配置函数 ##### 
 ===============================================================================  
@endverbatim
  * @{
  */

/**
  * @brief  将数据写入指定的 RTC 备份数据寄存器。
  * @param  RTC_BKP_DR: RTC 备份数据寄存器编号。
  *          该参数可以是：RTC_BKP_DRx，其中 x 可以从 0 到 19，用于指定寄存器。
  * @param  Data: 要写入指定 RTC 备份数据寄存器的数据。
  * @retval None
  */
void myRTC_WriteBackupRegister(uint32_t RTC_BKP_DR, uint32_t Data)
{
  __IO uint32_t tmp = 0;
  
  /* 检查参数 */
  assert_param(IS_RTC_BKP(RTC_BKP_DR));

  /* 计算备份寄存器地址 */
  tmp = RTC_BASE + 0x50 + (RTC_BKP_DR * 4);

  /* 写入指定寄存器 */
  *(__IO uint32_t *)tmp = (uint32_t)Data;
}

/**
  * @brief  从指定的 RTC 备份数据寄存器读取数据。
  * @param  RTC_BKP_DR: RTC 备份数据寄存器编号。
  *          该参数可以是：RTC_BKP_DRx，其中 x 可以从 0 到 19，用于指定寄存器。
  * @retval 返回指定寄存器的值
  */
uint32_t myRTC_ReadBackupRegister(uint32_t RTC_BKP_DR)
{
  __IO uint32_t tmp = 0;
  
  /* 检查参数 */
  assert_param(IS_RTC_BKP(RTC_BKP_DR));

  /* 计算备份寄存器地址 */
  tmp = RTC_BASE + 0x50 + (RTC_BKP_DR * 4);
  
  /* 读取指定寄存器 */
  return (*(__IO uint32_t *)tmp);
}


/**
  * @}
  */

/** @defgroup RTC_Group11 RTC 防篡改与时间戳引脚选择及输出类型配置函数
 *  @brief   RTC 防篡改和时间戳引脚选择及输出类型配置函数
 *
@verbatim   
 ==================================================================================================
 ##### RTC 防篡改和时间戳引脚选择及输出类型配置函数 ##### 
 ==================================================================================================  
@endverbatim
  * @{
  */

/**
  * @brief  选择 RTC 防篡改引脚。
  * @param  RTC_TamperPin: 指定 RTC 防篡改引脚。
  *          该参数可以是以下值之一：
  *            @arg RTC_TamperPin_PC13: 选择 PC13 作为 RTC 防篡改引脚。
  *            @arg RTC_TamperPin_PI8:  选择 PI8 作为 RTC 防篡改引脚。    
  * @retval None
  */
void myRTC_TamperPinSelection(uint32_t RTC_TamperPin)
{
  /* 检查参数 */
  assert_param(IS_RTC_TAMPER_PIN(RTC_TamperPin));
  
  /* 清除原有防篡改引脚选择位，并设置新的引脚 */
  RTC->TAFCR &= (uint32_t)~(RTC_TAFCR_TAMPINSEL);
  RTC->TAFCR |= (uint32_t)(RTC_TamperPin);  
}

/**
  * @brief  选择 RTC 时间戳引脚。
  * @param  RTC_TimeStampPin: 指定 RTC 时间戳引脚。
  *          该参数可以是以下值之一：
  *            @arg RTC_TimeStampPin_PC13: 选择 PC13 作为 RTC 时间戳引脚。
  *            @arg RTC_TimeStampPin_PI8:  选择 PI8 作为 RTC 时间戳引脚。    
  * @retval None
  */
void myRTC_TimeStampPinSelection(uint32_t RTC_TimeStampPin)
{
  /* 检查参数 */
  assert_param(IS_RTC_TIMESTAMP_PIN(RTC_TimeStampPin));
  
  /* 清除原有时间戳引脚选择位，并设置新的引脚 */
  RTC->TAFCR &= (uint32_t)~(RTC_TAFCR_TSINSEL);
  RTC->TAFCR |= (uint32_t)(RTC_TimeStampPin);  
}

/**
  * @brief  配置 RTC 输出引脚模式。
  * @param  RTC_OutputType: 指定 RTC 输出 (PC13) 引脚模式。
  *          该参数可以是以下值之一：
  *            @arg RTC_OutputType_OpenDrain: RTC 输出 (PC13) 配置为开漏模式。
  *            @arg RTC_OutputType_PushPull:  RTC 输出 (PC13) 配置为推挽模式。    
  * @retval None
  */
void myRTC_OutputTypeConfig(uint32_t RTC_OutputType)
{
  /* 检查参数 */
  assert_param(IS_RTC_OUTPUT_TYPE(RTC_OutputType));
  
  /* 清除原有输出类型位，并设置新的输出类型 */
  RTC->TAFCR &= (uint32_t)~(RTC_TAFCR_ALARMOUTTYPE);
  RTC->TAFCR |= (uint32_t)(RTC_OutputType);  
}


/**
  * @brief  配置 RTC 同步位移控制设置。
  * @note   当 REFCKON 位被设置时，固件不能写入位移控制寄存器。
  * @param  RTC_ShiftAdd1S: 是否向日历时间增加 1 秒。
  *          该参数可以是以下值之一：
  *            @arg RTC_ShiftAdd1S_Set: 向时钟日历增加 1 秒。
  *            @arg RTC_ShiftAdd1S_Reset: 无影响。
  * @param  RTC_ShiftSubFS: 指定要替换的秒分数 (Sub Second) 数量。
  *          该参数可以是 0 到 0x7FFF 之间的任意值。
  * @retval ErrorStatus 枚举值：
  *          - SUCCESS: RTC 位移寄存器配置成功
  *          - ERROR: RTC 位移寄存器配置失败
  */
ErrorStatus myRTC_SynchroShiftConfig(uint32_t RTC_ShiftAdd1S, uint32_t RTC_ShiftSubFS)
{
    ErrorStatus status = ERROR;
    uint32_t shpfcount = 0;

    /* 检查参数 */
    assert_param(IS_RTC_SHIFT_ADD1S(RTC_ShiftAdd1S));
    assert_param(IS_RTC_SHIFT_SUBFS(RTC_ShiftSubFS));

    /* 关闭写保护 */
    RTC->WPR = 0xCA;
    RTC->WPR = 0x53;

    /* 等待位移完成，如果有操作正在进行 */
    while ((RTC->ISR & RTC_ISR_SHPF) && (shpfcount != SHPF_TIMEOUT)) shpfcount++;

    /* 条件合并判断 */
    status = ((RTC->ISR & RTC_ISR_SHPF) == 0 && (RTC->CR & RTC_CR_REFCKON) == 0) 
             ? ((RTC->SHIFTR = RTC_ShiftSubFS | RTC_ShiftAdd1S, 
                 RTC_WaitForSynchro() == SUCCESS) ? SUCCESS : ERROR) 
             : ERROR;

    /* 开启写保护 */
    RTC->WPR = 0xFF;

    return status;
}


/**
  * @}
  */

/** @defgroup RTC_Group13 中断与标志管理函数
 *  @brief   RTC 中断与标志管理函数
 *
@verbatim   
 ===============================================================================
              ##### RTC 中断与标志管理函数 #####
 ===============================================================================  

 [..] 所有 RTC 中断均连接到 EXTI 控制器。
 
   (+) 若要使能 RTC Alarm 中断，需按如下顺序操作：
       (++) 使用 EXTI_Init() 配置并使能 EXTI 17 线的中断模式，并选择上升沿触发。
       (++) 使用 NVIC_Init() 配置并使能 RTC_Alarm IRQ 通道。
       (++) 使用 RTC_SetAlarm() 和 RTC_AlarmCmd() 配置 RTC 生成 Alarm A 和/或 Alarm B。

   (+) 若要使能 RTC Wakeup 中断，需按如下顺序操作：
       (++) 使用 EXTI_Init() 配置并使能 EXTI 22 线的中断模式，并选择上升沿触发。
       (++) 使用 NVIC_Init() 配置并使能 RTC_WKUP IRQ 通道。
       (++) 使用 RTC_WakeUpClockConfig()、RTC_SetWakeUpCounter() 和 RTC_WakeUpCmd() 配置 RTC 生成唤醒定时器事件。

   (+) 若要使能 RTC Tamper 中断，需按如下顺序操作：
       (++) 使用 EXTI_Init() 配置并使能 EXTI 21 线的中断模式，并选择上升沿触发。
       (++) 使用 NVIC_Init() 配置并使能 TAMP_STAMP IRQ 通道。
       (++) 使用 RTC_TamperTriggerConfig() 和 RTC_TamperCmd() 配置 RTC 检测 Tamper 事件。

   (+) 若要使能 RTC TimeStamp 中断，需按如下顺序操作：
       (++) 使用 EXTI_Init() 配置并使能 EXTI 21 线的中断模式，并选择上升沿触发。
       (++) 使用 NVIC_Init() 配置并使能 TAMP_STAMP IRQ 通道。
       (++) 使用 RTC_TimeStampCmd() 配置 RTC 检测 TimeStamp 事件。
@endverbatim
  * @{
  */

/**
  * @brief  使能或失能指定的 RTC 中断。
  * @param  RTC_IT: 指定要使能或失能的 RTC 中断源，可为以下值的任意组合：
  *            @arg RTC_IT_TS:  时间戳中断掩码
  *            @arg RTC_IT_WUT: 唤醒定时器中断掩码
  *            @arg RTC_IT_ALRB: Alarm B 中断掩码
  *            @arg RTC_IT_ALRA: Alarm A 中断掩码
  *            @arg RTC_IT_TAMP: Tamper 事件中断掩码
  * @param  NewState: 指定中断的新状态，可为 ENABLE 或 DISABLE。
  * @retval None
  */
void myRTC_ITConfig(uint32_t RTC_IT, FunctionalState NewState)
{
    /* 参数检查 */
    assert_param(IS_RTC_CONFIG_IT(RTC_IT));
    assert_param(IS_FUNCTIONAL_STATE(NewState));

    /* 禁止 RTC 寄存器写保护 */
    RTC->WPR = 0xCA;
    RTC->WPR = 0x53;

    /* 使用三目运算符优化 if else 结构 */
    NewState != DISABLE ?
        (RTC->CR |= (RTC_IT & ~RTC_TAFCR_TAMPIE),
         RTC->TAFCR |= (RTC_IT & RTC_TAFCR_TAMPIE)) :
        (RTC->CR &= ~(RTC_IT & ~RTC_TAFCR_TAMPIE),
         RTC->TAFCR &= ~(RTC_IT & RTC_TAFCR_TAMPIE));

    /* 使能 RTC 寄存器写保护 */
    RTC->WPR = 0xFF; 
}


/**
  * @}
  */

/** @defgroup RTC_Group14 Flags and Interrupts checking and clearing functions
 *  @brief   RTC 标志和中断状态检查及清除函数
 *
@verbatim   
 ===============================================================================
              ##### RTC 标志和中断管理函数 #####
 ===============================================================================  
@endverbatim
  * @{
  */

/**
  * @brief  检查指定的 RTC 标志是否被置位。
  * @param  RTC_FLAG: 指定要检查的标志，可为以下值之一：
  *            @arg RTC_FLAG_RECALPF: 校准完成标志
  *            @arg RTC_FLAG_TAMP1F: Tamper 1 事件标志
  *            @arg RTC_FLAG_TSOVF: 时间戳溢出标志
  *            @arg RTC_FLAG_TSF: 时间戳事件标志
  *            @arg RTC_FLAG_WUTF: 唤醒定时器标志
  *            @arg RTC_FLAG_ALRBF: Alarm B 标志
  *            @arg RTC_FLAG_ALRAF: Alarm A 标志
  *            @arg RTC_FLAG_INITF: 初始化模式标志
  *            @arg RTC_FLAG_RSF: 寄存器同步标志
  *            @arg RTC_FLAG_INITS: 寄存器配置完成标志
  *            @arg RTC_FLAG_SHPF: Shift 操作挂起标志
  *            @arg RTC_FLAG_WUTWF: 唤醒定时器写标志
  *            @arg RTC_FLAG_ALRBWF: Alarm B 写标志
  *            @arg RTC_FLAG_ALRAWF: Alarm A 写标志
  * @retval RTC_FLAG 状态 (SET 或 RESET)
  */
FlagStatus myRTC_GetFlagStatus(uint32_t RTC_FLAG)
{
    /* 参数检查 */
    assert_param(IS_RTC_GET_FLAG(RTC_FLAG));

    /* 检查标志位 */
    return ((RTC->ISR & RTC_FLAGS_MASK & RTC_FLAG) != RESET) ? SET : RESET;
}

/**
  * @brief  清除指定的 RTC 标志。
  * @param  RTC_FLAG: 指定要清除的标志，可为以下值的任意组合：
  *            @arg RTC_FLAG_TAMP1F: Tamper 1 事件标志
  *            @arg RTC_FLAG_TSOVF: 时间戳溢出标志
  *            @arg RTC_FLAG_TSF: 时间戳事件标志
  *            @arg RTC_FLAG_WUTF: 唤醒定时器标志
  *            @arg RTC_FLAG_ALRBF: Alarm B 标志
  *            @arg RTC_FLAG_ALRAF: Alarm A 标志
  *            @arg RTC_FLAG_RSF: 寄存器同步标志
  * @retval None
  */
void myRTC_ClearFlag(uint32_t RTC_FLAG)
{
    assert_param(IS_RTC_CLEAR_FLAG(RTC_FLAG));

    /* 清除 RTC_ISR 中的标志位，同时保留 INIT 标志 */
    RTC->ISR = (~((RTC_FLAG | RTC_ISR_INIT) & 0x0000FFFF)) | (RTC->ISR & RTC_ISR_INIT);
}

/**
  * @brief  检查指定的 RTC 中断是否发生。
  * @param  RTC_IT: 指定要检查的中断，可为以下值之一：
  *            @arg RTC_IT_TS: 时间戳中断
  *            @arg RTC_IT_WUT: 唤醒定时器中断
  *            @arg RTC_IT_ALRB: Alarm B 中断
  *            @arg RTC_IT_ALRA: Alarm A 中断
  *            @arg RTC_IT_TAMP1: Tamper 1 事件中断
  * @retval RTC_IT 状态 (SET 或 RESET)
  */
ITStatus myRTC_GetITStatus(uint32_t RTC_IT)
{
    uint32_t enablestatus = ((RTC->CR & RTC_IT) | (RTC->TAFCR & (RTC_IT >> 15)));
    uint32_t pendingbit = RTC->ISR & (RTC_IT >> 4);
    return (enablestatus != RESET && (pendingbit & 0xFFFF) != RESET) ? SET : RESET;
}

/**
  * @brief  清除指定的 RTC 中断挂起位。
  * @param  RTC_IT: 指定要清除的中断挂起位，可为以下值的任意组合：
  *            @arg RTC_IT_TS: 时间戳中断
  *            @arg RTC_IT_WUT: 唤醒定时器中断
  *            @arg RTC_IT_ALRB: Alarm B 中断
  *            @arg RTC_IT_ALRA: Alarm A 中断
  *            @arg RTC_IT_TAMP1: Tamper 1 事件中断
  * @retval None
  */
void myRTC_ClearITPendingBit(uint32_t RTC_IT)
{
    uint32_t tmpmask = RTC_IT >> 4;
    RTC->ISR = (~((tmpmask | RTC_ISR_INIT) & 0xFFFF)) | (RTC->ISR & RTC_ISR_INIT);
}

/**
  * @brief  将 2 位十进制数转换为 BCD 格式。
  * @param  Value: 待转换的字节值 (0~99)
  * @retval 转换后的 BCD 值
  */
static uint8_t RTC_ByteToBcd2(uint8_t Value)
{
    return ((Value / 10) << 4) | (Value % 10);
}

/**
  * @brief  将 2 位 BCD 值转换为二进制。
  * @param  Value: 待转换的 BCD 值
  * @retval 转换后的二进制值
  */
static uint8_t RTC_Bcd2ToByte(uint8_t Value)
{
    return ((Value >> 4) * 10) + (Value & 0x0F);
}
