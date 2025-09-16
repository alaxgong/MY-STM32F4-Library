/**
  ******************************************************************************
  * @file     stm32f4xx_gpio_rewrite.c
  * @author   QiangGu
  * @version  V1.0.0
  * @date     2025-09-16
  * @brief    STM32F4ϵ��GPIO���������⣨��д�棩
  *
  * @attention
  *
  *���ļ�Ϊ STM32F4xx ��׼����� GPIO ģ����Զ�����д�汾����д������ּ�ڼ���� STM32 ��̺� GPIO �������Ƶ��о���⡣
  *
  * ����д�汾��Ŀ�꣺
  * 1. �ṩ�������Ĵ���ṹ�������淶
  * 2. ��Ӹ���ϸ������ע�ͣ�����ѧϰ���
  * 3. �Ż����ֹ���ʵ�֣���ߴ���ɶ���
  * 4. ������ٷ��⺯���ļ�����
  *
  * ��ٷ��������
  * - ʹ�ø��ִ���C�������ԣ���C99��׼��
  * - ���Ӹ���Ĳ������ʹ�����
  * - �ṩ����ϸ�ĺ���˵����ʹ��ʾ��
  * - ����ṹ��ģ�黯������������չ
  *
  * ʹ��˵����
  * 1. ���⺯�����Ϊ��STM32F4xxϵ��΢���������ʹ��
  * 2. ��ʹ���κ�GPIO����ǰ���������ȵ���RCC_APB2PeriphClockCmd()ʹ��GPIOʱ��
  * 3. ʹ��GPIO_Init()������ʼ��GPIO��������
  * 4. �����ṩ������GPIO����������������д��ģʽ���õ�
  *
  * �汾��ʷ��
  * V1.0.0 ��ʼ�汾
  *   - ʵ�ֻ�����GPIO��ʼ������д����
  *   - �ṩ������ģʽ����֧��
  *   - �����ϸ�Ĵ�����͵���֧��
  * ����������
  * ���������ԭ�����ṩ�����ṩ�κ���ʾ��ʾ�ĵ����������������ڶ������Ի��ض���;�����Եı�֤��
  * ���߲�����ʹ�ñ�������������κ�ֱ�ӡ���ӡ�żȻ�����⡢ʾ���Ի�������ʧ�е����Ρ�
  * ���������ѧϰ���о�ʹ�ã��޳�������ֹ������ҵ��;��

  *
  ******************************************************************************
  */

#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"



void myGPIO_DeInit(GPIO_TypeDef* GPIOx)
{
    /* ������������ */
    uint32_t peripheral = 0;

    /* Check the parameters */
    assert_param(IS_GPIO_ALL_PERIPH(GPIOx));

    /* Ȼ��ʹ����Ŀ����� */
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
  * @brief  ��ʼ�� GPIO ����
  * @note
  *         1. ������ֻ�������û��� GPIO_InitStruct->GPIO_Pin ��ѡ������ţ�
  *            �����Ǳ���ȫ�� 16 �����ţ�����Ч�ʱȴ�ͳд�����ߡ�
  *         2. ֧��ͬʱ���ö�����ţ��á���������ϣ����� GPIO_Pin_0 | GPIO_Pin_1����
  *         3. �����������������ŵĸ��ù���(AF)�������Ҫ���ù��ܣ��紮�ڡ�I2C����
  *            ������� GPIO_PinAFConfig() ���þ���ĸ���ͨ����
  *
  * @param  GPIOx: GPIO�˿ڻ���ַ
  *                ȡֵ��Χ��GPIOA ~ GPIOI����ͬоƬ���ܲ�ȫ�У�
  *
  * @param  GPIO_InitStruct: ָ�� GPIO_InitTypeDef ���ͽṹ���ָ�룬
  *                �ýṹ����Ҫ��ǰ������³�Ա��
  *                - GPIO_Pin   : ��Ҫ���õ����ż��ϣ��ɶ�ѡ
  *                - GPIO_Mode  : ����ģʽ�����롢��������á�ģ�⣩
  *                - GPIO_Speed : ����ٶȣ��������/����ģʽ��Ч��
  *                - GPIO_OType : ������ͣ��������/����ģʽ��Ч�������©��
  *                - GPIO_PuPd  : ���������ã��ޡ�������������
  *
  * @retval ��
  */
void myGPIO_Init(GPIO_TypeDef* GPIOx, GPIO_InitTypeDef* GPIO_InitStruct)
{
    uint32_t pins = GPIO_InitStruct->GPIO_Pin; // �����û�ѡ������ż���
    uint32_t pinpos;

    while (pins) // ֻҪ����������Ҫ����
    {
        // �ҵ����λΪ1������λ�ã�pinpos��
        // �ü�ѭ����� __builtin_ctz
        for (pinpos = 0; pinpos < 16; pinpos++)
        {
            if (pins & (1U << pinpos))
                break; // �ҵ����λ
        }

        // ����Ѵ��������
        pins &= ~(1U << pinpos);

        // ---------------- ���� MODER ----------------
        GPIOx->MODER &= ~(0x3U << (pinpos * 2));
        GPIOx->MODER |= ((uint32_t)GPIO_InitStruct->GPIO_Mode << (pinpos * 2));

        // ---------------- ��������ٶȺ����ͣ������/����ģʽ�� ----------------
        ((GPIO_InitStruct->GPIO_Mode == GPIO_Mode_OUT) ||
            (GPIO_InitStruct->GPIO_Mode == GPIO_Mode_AF)) ?
            ( // ��Ŀ�����д��
                // ����ٶ� OSPEEDR
                GPIOx->OSPEEDR = (GPIOx->OSPEEDR & ~(0x3U << (pinpos * 2))) |
                ((uint32_t)GPIO_InitStruct->GPIO_Speed << (pinpos * 2)),
                // ������� OTYPER
                GPIOx->OTYPER = (GPIOx->OTYPER & ~(0x1U << pinpos)) |
                ((uint16_t)GPIO_InitStruct->GPIO_OType << pinpos)
                )
            : 0; // ��������������ģʽ���򲻲���

        // ---------------- ���������� PUPDR ----------------
        GPIOx->PUPDR &= ~(0x3U << (pinpos * 2));
        GPIOx->PUPDR |= ((uint32_t)GPIO_InitStruct->GPIO_PuPd << (pinpos * 2));
    }
}


/**
  * @brief  �� GPIO_InitTypeDef �ṹ���ʼ��ΪĬ��ֵ
  * @note   1. �����������ڳ�ʼ��ǰ���ýṹ��Ĭ��ֵ��Ȼ��ɵ��� myGPIO_Init ʹ�á�
  *         2. Ĭ��ֵ���£�
  *            - GPIO_Pin  = GPIO_Pin_All
  *            - GPIO_Mode = GPIO_Mode_IN
  *            - GPIO_Speed = GPIO_Speed_2MHz
  *            - GPIO_OType = GPIO_OType_PP
  *            - GPIO_PuPd = GPIO_PuPd_NOPULL
  *
  * @param  GPIO_InitStruct : ָ����Ҫ��ʼ���� GPIO_InitTypeDef �ṹ��ָ��
  * @retval None
  */
void myGPIO_StructInit(GPIO_InitTypeDef* GPIO_InitStruct)
{
    static const GPIO_InitTypeDef default_init = {
        GPIO_Pin_All,      // ��������
        GPIO_Mode_IN,      // ����ģʽ
        GPIO_Speed_2MHz,   // �ٶ� 2MHz
        GPIO_OType_PP,     // �������
        GPIO_PuPd_NOPULL   // ��������
    };

    *GPIO_InitStruct = default_init;
}


/**
  * @brief  ���� GPIO �������üĴ�������ֹ�ڳ��������ڼ䱻�޸�
  *
  * @note   1. STM32 �� GPIO �Ĵ����������¿��������ļĴ�����
  *           - GPIOx_MODER   : ģʽ�Ĵ���������/���/����/ģ�⣩
  *           - GPIOx_OTYPER  : ������ͣ�����/��©��
  *           - GPIOx_OSPEEDR : ����ٶ�
  *           - GPIOx_PUPDR   : ����������
  *           - GPIOx_AFRL    : �� 8 �����Ÿ��ù��ܼĴ���
  *           - GPIOx_AFRH    : �� 8 �����Ÿ��ù��ܼĴ���
  *         ��Щ�Ĵ���һ����������Ӧ���ŵ����ý� **���ܱ��޸�**������оƬ��λ��
  *
  *         2. ����������˳������ϸ��� STM32 �ٷ��ֲ᣺
  *            - ��һ��д LCKR������ LCKK=1 + ѡ��Ҫ����������
  *            - �ڶ���д LCKR���� LCKK=0��ֻд��������
  *            - ������д LCKR���ٴ����� LCKK=1
  *            - �������ζ�ȡ LCKR������Ӳ��������Ч
  *
  * @param  GPIOx : ָ�� GPIO ����ļĴ����ṹ��
  *                 - STM32F405/407/415/417: GPIOA~GPIOK
  *                 - STM32F42xxx/43xxx: GPIOA~GPIOI
  *                 - STM32F401xx: GPIOA, B, C, D, H
  *
  * @param  GPIO_Pin : Ҫ������ GPIO ����
  *                    - �����ǵ������ţ��� GPIO_Pin_0
  *                    - Ҳ������϶�����ţ��� GPIO_Pin_0 | GPIO_Pin_3 | GPIO_Pin_5
  *
  * @retval None
  *
  * @attention
  *   1. һ�������ɹ��������������������ڼ��޷��޸����á�
  *   2. ֻ�и�λоƬ��Ż������
  *   3. �����ϸ��չٷ�Ҫ���д�� + ��ȡ˳�򣬷����������ܲ���Ч��
  *   4. ����ֻ��Ӳ�����ƣ�����ı����ŵ�ǰ״̬���ߵ͵�ƽ����ֻ��ֹ�޸����á�
  */
void myGPIO_PinLockConfig(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin)
{
    /* �������Ϸ��� */
    assert_param(IS_GPIO_ALL_PERIPH(GPIOx));
    assert_param(IS_GPIO_PIN(GPIO_Pin));

    /* LCKR �Ĵ�������λ + Ҫ���������� */
    volatile uint32_t tmp = GPIO_Pin | 0x00010000U; // LCKK λ�ڵ�16λ

    /* STM32 �ٷ�Ҫ�������˳�� */
    GPIOx->LCKR = tmp;       // ��һ��д������ LCKK = 1 + ѡ������
    GPIOx->LCKR = GPIO_Pin;  // �ڶ���д���� LCKK = 0��ֻд��������
    GPIOx->LCKR = tmp;       // ������д���ٴ����� LCKK = 1

    /* ��ȡ���μĴ�����ȷ��������Ч */
    (void)GPIOx->LCKR;
    (void)GPIOx->LCKR;
}


/**
  * @brief  ��ȡָ�� GPIOx���ŵ������ƽ״̬
  *
  * @note   1. GPIO ��ÿ�����Ŷ���һ����Ӧ���������ݼĴ��� IDR λ��
  *            - bit = 1 �� �ߵ�ƽ���߼� 1��
  *            - bit = 0 �� �͵�ƽ���߼� 0��
  *         ������������ȡָ�����ŵ�״̬��
  *
  *         2. �ʺ϶�ȡ���������ء��������źŵ��������롣
  *
  * @param  GPIOx : ָ�� GPIO ����Ĵ����ṹ��
  *                 - ���� GPIOA��GPIOB ��
  *                 - STM32F405/407/415/417: GPIOA~GPIOK
  *                 - STM32F42xxx/43xxx: GPIOA~GPIOI
  *                 - STM32F401xx: GPIOA, B, C, D, H
  *
  * @param  GPIO_Pin : Ҫ��ȡ�� GPIO ����
  *                    - �����ǵ������ţ��� GPIO_Pin_0
  *                    - Ҳ������϶�����ţ��� GPIO_Pin_0 | GPIO_Pin_3 | GPIO_Pin_5
  *                    - ������ֻ��ȡָ�����ŵĵ�ƽ
  *
  * @retval uint8_t : �������ŵ�ƽ״̬
  *                    - Bit_SET   (1) �� �ߵ�ƽ
  *                    - Bit_RESET (0) �� �͵�ƽ
  *
  * @attention
  *   1. ����ֵֻ��ʾ���ŵ�ǰ��ƽ��������ģʽ������/������޹ء�
  *   2. ���ָ��������ţ�����ֻ�������������λ���ŵ�״̬��
  */
uint8_t myGPIO_ReadInputDataBit(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin)
{
    /* �������Ƿ�Ϸ� */
    assert_param(IS_GPIO_ALL_PERIPH(GPIOx));
    assert_param(IS_GET_GPIO_PIN(GPIO_Pin));

    /* ʹ����Ŀ������ж����ŵ�ƽ��
       - GPIOx->IDR & GPIO_Pin ��ȡ��ָ�����ŵĵ�ƽλ
       - �����Ϊ 0��������Ϊ�ߵ�ƽ Bit_SET
       - ����Ϊ�͵�ƽ Bit_RESET */
    return ((GPIOx->IDR & GPIO_Pin) != (uint32_t)Bit_RESET) ? (uint8_t)Bit_SET : (uint8_t)Bit_RESET;
}


/**
  * @brief  ��ȡָ�� GPIOX�˿ڵ�ȫ�����������ƽ
  *
  * @note   1. ÿ�� GPIO �˿������ 16 �����ţ�Pin0 ~ Pin15����
  *            ÿ��λ���������ݼĴ��� IDR �ж�Ӧһ�����ŵ�״̬��
  *            - bit = 1 �� �ߵ�ƽ���߼� 1��
  *            - bit = 0 �� �͵�ƽ���߼� 0��
  *         ���������������˿ڵ� 16 λ����״̬��
  *
  *         2. �ʺ�һ���Զ�ȡ�����˿ڵ����ŵ�ƽ�����磺
  *            - ��������ɨ��
  *            - GPIO ����״̬���
  *
  * @param  GPIOx : ָ��Ҫ��ȡ�� GPIO ����Ĵ����ṹ��
  *                 - STM32F405/407/415/417: GPIOA~GPIOK
  *                 - STM32F42xxx/43xxx: GPIOA~GPIOI
  *                 - STM32F401xx: GPIOA, B, C, D, H
  *
  * @retval uint16_t : �������� GPIO �˿ڵ������ƽ״̬
  *                    - ÿһλ��Ӧһ������
  *                    - bit = 1 �� �ߵ�ƽ
  *                    - bit = 0 �� �͵�ƽ
  *
  * @attention
  *   1. ����ֵ�����˿��������ŵ�״̬�����ֻ���ȡĳ�����ţ���ʹ��
  *      GPIO_ReadInputDataBit()��
  *   2. �����ƽ�ĸߵ�ȡ�����ⲿ�źŻ�����/�������á�
  */
uint16_t myGPIO_ReadInputData(GPIO_TypeDef* GPIOx)
{
    /* ---------------- ������� ---------------- */
    assert_param(IS_GPIO_ALL_PERIPH(GPIOx)); // ȷ�� GPIO ������Ч

    /* ---------------- ��ȡ�Ĵ��� ---------------- */
    // GPIOx->IDR ���������ݼĴ�����ÿ��λ��Ӧһ�����ŵ�ƽ
    // ����ǿ��ת��Ϊ uint16_t ������
    return (uint16_t)(GPIOx->IDR);
}


/**
  * @brief  ��ȡָ�� GPIO ���ŵ�����Ĵ���״̬(ע������ǼĴ������ǵ�ƽ)
  *
  * @note   1. GPIO ��ÿ�����Ŷ��ж�Ӧ��������ݼĴ��� ODR λ��
  *            - bit = 1 �� ����Ĵ���Ϊ�ߣ��������øߵ�ƽ��
  *            - bit = 0 �� ����Ĵ���Ϊ�ͣ��������õ͵�ƽ��
  *         ���������ڲ鿴 **�������õ����״̬**�������������ƽ��
  *
  *         2. ������������������ODR �ߵ�ƽͨ����Ӧ����ʵ������ߵ�ƽ��
  *            ������ⲿǿ�����ͣ����������ƽ���ܲ�ͬ��
  *
  * @param  GPIOx : ָ�� GPIO ����Ĵ����ṹ��
  *                 - ���� GPIOA��GPIOB ��
  *                 - STM32F405/407/415/417: GPIOA~GPIOK
  *                 - STM32F42xxx/43xxx: GPIOA~GPIOI
  *                 - STM32F401xx: GPIOA, B, C, D, H
  *
  * @param  GPIO_Pin : Ҫ��ȡ�� GPIO ����
  *                    - �����ǵ������ţ��� GPIO_Pin_0
  *                    - Ҳ������϶�����ţ��� GPIO_Pin_0 | GPIO_Pin_3
  *
  * @retval uint8_t : �����������״̬
  *                    - Bit_SET   (1) �� ����Ĵ���Ϊ��
  *                    - Bit_RESET (0) �� ����Ĵ���Ϊ��
  *
  * @attention
  *   1. ����ֵ��ʾ **�Ĵ���״̬**����һ������ʵ�������ƽ��
  *   2. ������ж������ƽ����ʹ�� GPIO_ReadInputDataBit()��
  */
uint8_t myGPIO_ReadOutputDataBit(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin)
{
    /* ---------------- ������� ---------------- */
    assert_param(IS_GPIO_ALL_PERIPH(GPIOx));   // ȷ�� GPIO ������Ч
    assert_param(IS_GET_GPIO_PIN(GPIO_Pin));  // ȷ�� GPIO ������Ч

    /* ---------------- ��Ŀ�������ȡ ODR ---------------- */
    // �����߼���
    // - GPIOx->ODR & GPIO_Pin ��ȡ��ָ�����ŵ�����Ĵ���λ
    // - �����Ϊ 0��������żĴ���Ϊ�� Bit_SET
    // - ����Ϊ�� Bit_RESET
    return ((GPIOx->ODR & GPIO_Pin) != (uint32_t)Bit_RESET) ? (uint8_t)Bit_SET : (uint8_t)Bit_RESET;
}


/**
  * @brief  ��ȡָ�� GPIO �˿ڵ�ȫ������Ĵ���״̬
  *
  * @note   1. GPIO �˿ڵ�����Ĵ��� ODR��Output Data Register����
  *            - ÿ��λ��Ӧһ�����ŵ����״̬
  *            - bit = 1 �� ����Ĵ�������Ϊ�ߵ�ƽ��Bit_SET��
  *            - bit = 0 �� ����Ĵ�������Ϊ�͵�ƽ��Bit_RESET��
  *
  *         2. ������ **��ȡ���ǼĴ���ֵ**����ӳ�������õ����״̬
  *            - ������������������ODR ��ͨ����Ӧʵ�ʸߵ�ƽ
  *            - ������ⲿǿ�����ͣ������ƽ���ܲ�ͬ
  *
  * @param  GPIOx : ָ�� GPIO ����Ĵ����ṹ��
  *                 - STM32F405/407/415/417: GPIOA~GPIOK
  *                 - STM32F42xxx/43xxx: GPIOA~GPIOI
  *                 - STM32F401xx: GPIOA, B, C, D, H
  *
  * @retval uint16_t : �������� GPIO �˿ڵ�����Ĵ���״̬
  *                    - ÿ��λ��Ӧһ������
  *                    - bit = 1 �� �ߵ�ƽ���������ã�
  *                    - bit = 0 �� �͵�ƽ���������ã�
  *
  * @attention
  *   1. ����ֵֻ��ʾ **�Ĵ���״̬**����һ������ʵ�����ŵ�ƽ
  *   2. ���ֻ���ȡ�������ţ�ʹ�� GPIO_ReadOutputDataBit() ������
  */
uint16_t myGPIO_ReadOutputData(GPIO_TypeDef* GPIOx)
{
    /* ---------------- ������� ---------------- */
    // ȷ������� GPIO ����Ϸ�����ֹ�Ƿ��Ĵ�������
    assert_param(IS_GPIO_ALL_PERIPH(GPIOx));

    /* ---------------- ��ȡ����Ĵ��� ---------------- */
    // GPIOx->ODR ÿ��λ��Ӧһ�����ŵ�����Ĵ���״̬
    // ���� bit0 = PA0, bit1 = PA1, ... , bit15 = PA15
    // ���Ĵ���ֵת��Ϊ uint16_t ������
    return (uint16_t)(GPIOx->ODR);
}



/**
  * @brief  ����ָ�� GPIO ����Ϊ�ߵ�ƽ
  *
  * @note   1. ������ʹ�� GPIOx_BSRR �Ĵ�����λ����/��λ�Ĵ��������޸�����״̬��
  *            �������Ա�֤������ԭ���Եģ�atomic���������޸Ĺ����в��ᱻ�жϴ�ϡ�
  *         2. ʹ�� BSRR �Ĵ����������Ÿߵ�ƽ��ֱ���޸� ODR �Ĵ�������ȫ�������жϵ��µľ������⡣
  *
  * @param  GPIOx : ָ��Ҫ������ GPIO ����Ĵ����ṹ��
  *                 - STM32F405/407/415/417: GPIOA~GPIOK
  *                 - STM32F42xxx/43xxx: GPIOA~GPIOI
  *                 - STM32F401xx: GPIOA, B, C, D, H
  *
  * @param  GPIO_Pin : Ҫ����Ϊ�ߵ�ƽ�� GPIO ����
  *                    - �����ǵ������ţ����� GPIO_Pin_0
  *                    - Ҳ�����Ƕ��������ϣ����� GPIO_Pin_0 | GPIO_Pin_3 | GPIO_Pin_5
  *
  * @retval None : ������û�з���ֵ
  *
  * @attention
  *   1. ������ֻ�Ὣָ��������Ϊ�ߵ�ƽ������Ӱ����������״̬��
  *   2. ����뽫�����õͣ���ʹ�� GPIO_ResetBits()��
  */
void myGPIO_SetBits(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin)
{
    /* ---------------- ������� ---------------- */
    // ȷ�� GPIO ����Ϸ�
    assert_param(IS_GPIO_ALL_PERIPH(GPIOx));
    // ȷ�� GPIO ������Ч
    assert_param(IS_GPIO_PIN(GPIO_Pin));

    /* ---------------- �������Ÿߵ�ƽ ---------------- */
    // BSRRL �Ĵ�����λ���üĴ������� 16 λ��Ӧ Pin0~Pin15��
    // д 1 ����Ӧλ���ɽ������øߵ�ƽ��д 0 ��Ӱ��
    GPIOx->BSRRL = GPIO_Pin;
}



/**
  * @brief  ��ָ�� GPIO ���������Ϊ�͵�ƽ
  *
  * @note   1. ������ʹ�� GPIOx_BSRR �Ĵ����ĸ� 16 λ��BSRRH������λ���ţ�
  *            �������Ա�֤������ԭ���Եģ�atomic�������ᱻ�жϴ�ϡ�
  *         2. ʹ�� BSRRH �Ĵ�����λ���ű�ֱ���޸� ODR �Ĵ�������ȫ�������жϵ��µľ������⡣
  *
  * @param  GPIOx : ָ��Ҫ������ GPIO ����Ĵ����ṹ��
  *                 - STM32F405/407/415/417: GPIOA~GPIOK
  *                 - STM32F42xxx/43xxx: GPIOA~GPIOI
  *                 - STM32F401xx: GPIOA, B, C, D, H
  *
  * @param  GPIO_Pin : Ҫ�õ͵�ƽ�� GPIO ����
  *                    - �����ǵ������ţ����� GPIO_Pin_0
  *                    - Ҳ�����Ƕ��������ϣ����� GPIO_Pin_0 | GPIO_Pin_3 | GPIO_Pin_5
  *
  * @retval None : ������û�з���ֵ
  *
  * @attention
  *   1. ������ֻ�Ὣָ��������Ϊ�͵�ƽ������Ӱ����������״̬��
  *   2. ����뽫�����øߣ���ʹ�� GPIO_SetBits()��
  */
void myGPIO_ResetBits(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin)
{
    /* ---------------- ������� ---------------- */
    // ȷ�� GPIO ����Ϸ�
    assert_param(IS_GPIO_ALL_PERIPH(GPIOx));
    // ȷ�� GPIO ������Ч
    assert_param(IS_GPIO_PIN(GPIO_Pin));

    /* ---------------- �������ŵ͵�ƽ ---------------- */
    // BSRRH �Ĵ�����λ��λ�Ĵ������� 16 λ��Ӧ Pin0~Pin15��
    // д 1 ����Ӧλ���ɽ������õ͵�ƽ��д 0 ��Ӱ��
    GPIOx->BSRRH = GPIO_Pin;
}


/**
  * @brief  ���û����ָ�� GPIO �������
  *
  * @note   1. ���������� BitVal ���������� GPIO ��������Ϊ�ߵ�ƽ���ǵ͵�ƽ��
  *         2. ʹ�� GPIOx_BSRR �Ĵ�������ԭ�Ӳ�����
  *            - BSRRL���� 16 λ��д 1 �� ��Ӧ�����ø�
  *            - BSRRH���� 16 λ��д 1 �� ��Ӧ�����õ�
  *
  * @param  GPIOx : ָ��Ҫ������ GPIO ����Ĵ����ṹ��
  *                 - STM32F405/407/415/417: GPIOA~GPIOK
  *                 - STM32F42xxx/43xxx: GPIOA~GPIOI
  *                 - STM32F401xx: GPIOA, B, C, D, H
  *
  * @param  GPIO_Pin : Ҫ������ GPIO ����
  *                    - �������ţ��� GPIO_Pin_0
  *                    - ����������ϣ��� GPIO_Pin_0 | GPIO_Pin_3
  *
  * @param  BitVal : ָ�����������ֵ
  *                  - Bit_SET   �� ���øߵ�ƽ
  *                  - Bit_RESET �� ���õ͵�ƽ
  *
  * @retval None : ������û�з���ֵ
  *
  * @attention
  *   1. ������ֻ�޸�ָ������״̬����Ӱ���������š�
  *   2. ԭ�Ӳ�������ֹ�жϸ��š�
  */
void myGPIO_WriteBit(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, BitAction BitVal)
{
    /* ---------------- ������� ---------------- */
    assert_param(IS_GPIO_ALL_PERIPH(GPIOx));    // ��� GPIO �����Ƿ�Ϸ�
    assert_param(IS_GET_GPIO_PIN(GPIO_Pin));    // ��� GPIO �����Ƿ�Ϸ�
    assert_param(IS_GPIO_BIT_ACTION(BitVal));   // ��� BitVal �Ƿ���Ч

    /* ---------------- ʹ����Ŀ��������û�������� ---------------- */
    (BitVal != Bit_RESET) ? (GPIOx->BSRRL = GPIO_Pin) : (GPIOx->BSRRH = GPIO_Pin);

}


/**
  * @brief  ��ָ�� GPIO �˿�����Ĵ���д������
  *
  * @note   1. ������һ����д������ GPIO �˿ڵ� ODR��������ݼĴ�������
  *            ��һ�������ö˿����������ŵ����״̬��
  *         2. д���ֱֵ�Ӹ���ԭ�Ĵ������ݣ���������״̬���ᱻ���£�
  *            ���ᱣ��ԭ��״̬��
  *
  * @param  GPIOx : ָ��Ҫ������ GPIO ����Ĵ����ṹ��
  *                 - STM32F405/407/415/417: GPIOA~GPIOK
  *                 - STM32F42xxx/43xxx: GPIOA~GPIOI
  *                 - STM32F401xx: GPIOA, B, C, D, H
  *
  * @param  PortVal : Ҫд�� GPIO �˿�����Ĵ�����ֵ
  *                   - ÿһλ��Ӧһ�����ŵ����״̬
  *                   - 1 �� ���ö�Ӧ���Ÿߵ�ƽ
  *                   - 0 �� ���ö�Ӧ���ŵ͵�ƽ
  *
  * @retval None : ������û�з���ֵ
  *
  * @attention
  *   1. �Ḳ�������˿ڼĴ��������ã����ֻ���޸Ĳ������ţ���ʹ�� GPIO_WriteBit() �� GPIO_SetBits()/GPIO_ResetBits()��
  *   2. ������ֱ��д�� ODR �Ĵ���������֤������ԭ���ԣ��жϿ���Ӱ�������
  */
void myGPIO_Write(GPIO_TypeDef* GPIOx, uint16_t PortVal)
{
    /* ---------------- ������� ---------------- */
    assert_param(IS_GPIO_ALL_PERIPH(GPIOx)); // ȷ�� GPIO ����Ϸ�

    /* ---------------- д������Ĵ��� ---------------- */
    // ODR��Output Data Register��ÿ��λ��Ӧһ�� GPIO ����
    // д 1 �� ��������ߵ�ƽ
    // д 0 �� ��������͵�ƽ
    GPIOx->ODR = PortVal;
}



/**
  * @brief  �л�����ת��ָ�� GPIO ���ŵ����״̬
  *
  * @note   1. ������ͨ���� ODR��������ݼĴ��������а�λ������ʵ�ַ�ת��
  *            - ��ǰ���Ÿߵ�ƽ �� �л�Ϊ�͵�ƽ
  *            - ��ǰ���ŵ͵�ƽ �� �л�Ϊ�ߵ�ƽ
  *         2. ��ͬʱ��ת������ţ�ͨ����λ����� GPIO_Pin ʵ�֡�
  *
  * @param  GPIOx : ָ��Ҫ������ GPIO ����Ĵ����ṹ��
  *                 - STM32F405/407/415/417: GPIOA~GPIOK
  *                 - STM32F42xxx/43xxx: GPIOA~GPIOI
  *                 - STM32F401xx: GPIOA, B, C, D, H
  *
  * @param  GPIO_Pin : Ҫ��ת�� GPIO ����
  *                    - �������ţ����� GPIO_Pin_0
  *                    - ����������ϣ����� GPIO_Pin_0 | GPIO_Pin_3
  *
  * @retval None : ������û�з���ֵ
  *
  * @attention
  *   1. ֱ�Ӳ��� ODR �Ĵ��������������п��ܱ��жϴ�ϣ�����֤ԭ���ԡ�
  *   2. ��ת������ı�ָ�����ŵ�״̬��δָ�������ű��ֲ��䡣
  */
void myGPIO_ToggleBits(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin)
{
    /* ---------------- ������� ---------------- */
    assert_param(IS_GPIO_ALL_PERIPH(GPIOx)); // ��� GPIO �����Ƿ�Ϸ�

    /* ---------------- ��תָ������ ---------------- */
    // ODR �Ĵ��� ^= GPIO_Pin
    // ��λ��������1��0��0��1��ʵ�ַ�ת
    GPIOx->ODR ^= GPIO_Pin;
}



/**
  * @brief  ����ָ�� GPIO ���ŵĸ��ù��ܣ�Alternate Function, AF��
  *
  * @note
  * 1. STM32 ��ÿ�� GPIO ���ſ��Ը���Ϊ��ͬ���蹦�ܣ����� USART��SPI��TIM��I2C �ȡ�
  * 2. GPIO �ĸ��ù���ͨ�� AFR[0] �� AFR[1] �Ĵ������ƣ�
  *      - AFR[0] �������� 0~7
  *      - AFR[1] �������� 8~15
  * 3. ÿ������ռ 4 λ����˿������� 0~15 �ĸ��ù��ܣ�AF0~AF15����
  *
  * @param  GPIOx : Ҫ������ GPIO �˿ڣ����� GPIOA~GPIOK
  * @param  GPIO_PinSource : Ҫ���õ����ű�ţ�0~15��
  * @param  GPIO_AF : Ҫӳ��ĸ��ù��ܱ�ţ�0~15����Ӧ AF0~AF15��
  *
  * @retval None : ������û�з���ֵ
  *
  * @attention
  * 1. ������ֱ�Ӳ��� AFR �Ĵ������Ḳ��ԭ���� AF ���á�
  * 2. ʹ��ǰ��ȷ�� GPIO �Ѿ�����ʱ�Ӳ���ʼ����
  *
  * @par GPIO AF ���ù���ʾ������ֳ��ã�
  * | AF��� | ����       | ��������                          |
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
  * | AF11   | ETH        | ��̫�� Ethernet                   |
  * | AF12   | FMC/SDIO   | FSMC/FMC/SDIO                     |
  * | AF13   | DCMI       | ����ͷ�ӿ�                         |
  * | AF14   | LTDC       | LCD ������                        |
  * | AF15   | EVENTOUT   | �ⲿ�¼����                       |
  */
void myGPIO_PinAFConfig(GPIO_TypeDef* GPIOx, uint16_t GPIO_PinSource, uint8_t GPIO_AF)
{
    uint32_t afr_index;   // AFR �Ĵ���������0 ��ʾ AFR[0]��1 ��ʾ AFR[1]��
    uint32_t afr_offset;  // ������ AFR �Ĵ����е�ƫ��λ��ÿ������ռ 4 λ��

    /* ---------------- ������� ---------------- */
    assert_param(IS_GPIO_ALL_PERIPH(GPIOx));        // ��� GPIO �˿��Ƿ�Ϸ�
    assert_param(IS_GPIO_PIN_SOURCE(GPIO_PinSource)); // ������ű���Ƿ�Ϸ�
    assert_param(IS_GPIO_AF(GPIO_AF));             // ��鸴�ù��ܱ���Ƿ�Ϸ�

    /* ---------------- ���� AFR �Ĵ���������ƫ�� ---------------- */
    afr_index = GPIO_PinSource >> 3;              // 0~7 �� AFR[0]��8~15 �� AFR[1]
    afr_offset = (GPIO_PinSource & 0x07) * 4;    // ÿ������ռ 4 λ������ƫ��λ

    /* ---------------- ���������ԭ���� AF ���� ---------------- */
    GPIOx->AFR[afr_index] &= ~(0xF << afr_offset);

    /* ---------------- ���ø����ŵ��� AF ���� ---------------- */
    GPIOx->AFR[afr_index] |= ((uint32_t)GPIO_AF << afr_offset);
}
