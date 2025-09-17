
#ifndef __MYSTM32F4_USART_H
#define __MYSTM32F4_USART_H
#include "stm32f4xx_usart.h"
#include "stm32f4xx_rcc.h"
/* USART �����ʼ�������ú��� */
void myUSART_DeInit(USART_TypeDef* USARTx);                              // ��λUSART���裬�ָ�Ĭ������
void myUSART_Init(USART_TypeDef* USARTx, USART_InitTypeDef* USART_InitStruct); // ��ʼ��USART���裬���ò����ʡ�����λ��
void myUSART_StructInit(USART_InitTypeDef* USART_InitStruct);            // ��ʼ��USART���ýṹ��ΪĬ��ֵ
void myUSART_ClockInit(USART_TypeDef* USARTx, USART_ClockInitTypeDef* USART_ClockInitStruct); // ����USARTʱ��Դ
void myUSART_ClockStructInit(USART_ClockInitTypeDef* USART_ClockInitStruct); // ��ʼ��USARTʱ�����ýṹ��ΪĬ��ֵ
void myUSART_Cmd(USART_TypeDef* USARTx, FunctionalState NewState);       // ���û����USART����
void myUSART_SetPrescaler(USART_TypeDef* USARTx, uint8_t USART_Prescaler); // ����USART��Ԥ��Ƶ��
void myUSART_OverSampling8Cmd(USART_TypeDef* USARTx, FunctionalState NewState); // ����/����8������
void myUSART_OneBitMethodCmd(USART_TypeDef* USARTx, FunctionalState NewState); // ����/���õ����ط���
void myUSART_SendData(USART_TypeDef* USARTx, uint16_t Data);            // ����һ���ֽ�����
uint16_t myUSART_ReceiveData(USART_TypeDef* USARTx);                    // ����һ���ֽ�����
void myUSART_SetAddress(USART_TypeDef* USARTx, uint8_t USART_Address);  // ����USART�ĵ�ַ�����ڶ��豸ͨ�ţ�
void myUSART_ReceiverWakeUpCmd(USART_TypeDef* USARTx, FunctionalState NewState); // ����/���ý��������ѹ���
void myUSART_WakeUpConfig(USART_TypeDef* USARTx, uint16_t USART_WakeUp); // ����USART����Դ
void myUSART_LINBreakDetectLengthConfig(USART_TypeDef* USARTx, uint16_t USART_LINBreakDetectLength); // ����LIN�жϼ�ⳤ��
void myUSART_LINCmd(USART_TypeDef* USARTx, FunctionalState NewState);    // ����/����LINģʽ
void myUSART_SendBreak(USART_TypeDef* USARTx);                          // ����Break�ź�
void myUSART_HalfDuplexCmd(USART_TypeDef* USARTx, FunctionalState NewState); // ����/���ð�˫��ģʽ
void myUSART_SetGuardTime(USART_TypeDef* USARTx, uint8_t USART_GuardTime); // ����USART���շ���ʱ��
void myUSART_SmartCardCmd(USART_TypeDef* USARTx, FunctionalState NewState); // ����/�������ܿ�����
void myUSART_IrDAConfig(USART_TypeDef* USARTx, uint16_t USART_IrDAMode); // ���ú��⹦��ģʽ
void myUSART_SmartCardNACKCmd(USART_TypeDef* USARTx, FunctionalState NewState); // ����/�������ܿ�NACK��Ӧ
void myUSART_IrDACmd(USART_TypeDef* USARTx, FunctionalState NewState);   // ����/���ú��⹦��
void myUSART_DMACmd(USART_TypeDef* USARTx, uint16_t USART_DMAReq, FunctionalState NewState); // ����/����DMA����
void myUSART_ITConfig(USART_TypeDef* USARTx, uint16_t USART_IT, FunctionalState NewState); // ����/����USART�ж�
void myUSART_ClearFlag(USART_TypeDef* USARTx, uint16_t USART_FLAG);      // ���USART��־
FlagStatus myUSART_GetFlagStatus(USART_TypeDef* USARTx, uint16_t USART_FLAG); // ��ȡUSART��־״̬
ITStatus myUSART_GetITStatus(USART_TypeDef* USARTx, uint16_t USART_IT); // ��ȡUSART�ж�״̬
void myUSART_ClearITPendingBit(USART_TypeDef* USARTx, uint16_t USART_IT);  // ���USART�жϹ���λ



#endif