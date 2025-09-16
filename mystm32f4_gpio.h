#ifndef __mystm32f4_gpio_H
#define __mystm32f4_gpio_H
#include "stm32f4xx_gpio.h"  // �ٷ�ͷ�ļ����ṩ�ṹ��ͼĴ�������
#ifdef __cplusplus
extern "C" {
#endif

/* GPIO ���躯���������Ҳ�Ӽ�̹���˵���� */

void myGPIO_DeInit(GPIO_TypeDef* GPIOx);                           // ��ָ�� GPIO �˿ڼĴ�����λ��Ĭ��״̬
void myGPIO_Init(GPIO_TypeDef* GPIOx, GPIO_InitTypeDef* GPIO_InitStruct);  // �������ýṹ���ʼ��ָ�� GPIO �˿�
void myGPIO_StructInit(GPIO_InitTypeDef* GPIO_InitStruct);        // �� GPIO ��ʼ���ṹ�����ΪĬ��ֵ
void myGPIO_PinLockConfig(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin); // ����ָ�� GPIO �������ã���ֹ�޸�
uint8_t myGPIO_ReadInputDataBit(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);  // ��ȡָ�� GPIO �������ŵĵ�ƽ����/�ͣ�
uint16_t myGPIO_ReadInputData(GPIO_TypeDef* GPIOx);                // ��ȡָ�� GPIO �˿������������ŵĵ�ƽ
uint8_t myGPIO_ReadOutputDataBit(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin); // ��ȡָ�� GPIO ������ŵ�״̬����/�ͣ�
uint16_t myGPIO_ReadOutputData(GPIO_TypeDef* GPIOx);               // ��ȡָ�� GPIO �˿�����������ŵ�״̬
void myGPIO_SetBits(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);       // ��ָ�� GPIO ��������øߵ�ƽ
void myGPIO_ResetBits(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);     // ��ָ�� GPIO ��������õ͵�ƽ
void myGPIO_WriteBit(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, BitAction BitVal); // ����ָ�� GPIO ��������߻��
void myGPIO_Write(GPIO_TypeDef* GPIOx, uint16_t PortVal);          // д������ GPIO �˿��������
void myGPIO_ToggleBits(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);    // �л�ָ�� GPIO ���ŵ������ƽ
void myGPIO_PinAFConfig(GPIO_TypeDef* GPIOx, uint16_t GPIO_PinSource, uint8_t GPIO_AF); // ����ָ�� GPIO ���ŵĸ��ù���
#ifdef __cplusplus
}
#endif

#endif
