#ifndef __NOISE_H
#define __NOISE_H

/******************************************************************************/
#include "stm32l1xx_hal.h"
#include "main.h"

#include "public.h"

#include "usart.h"

/******************************************************************************/
#define NOISE_UART 						(huart2)
#define NOISE_UART_RX_BYTE_MAX			(20)
#define NOISE_UART_DMA_RX_GL_FLAG		(DMA_FLAG_GL6)

#define NOISE_MODULE_ADDR				(0x01)		/* ����ģ���ַ */
#define NOISE_MODULE_CMD_TYPE			(0x03)		/* ������ */
#define NOISE_MODULE_DATA_LENGTH		(0x02)		/* ���ݳ��� */
#define NOISE_MODULE_RANGE_DB_MAX		(120)		/* ����ģ���������ֵ120dB */
#define NOISE_MODULE_RANGE_DB_MIN		(30)		/* ����ģ��������Сֵ30dB */
#define NOISE_MODULE_RANGE_DB \
						(NOISE_MODULE_RANGE_DB_MAX - NOISE_MODULE_RANGE_DB_MIN)


/******************************************************************************/
#pragma pack(1)
typedef struct
{
	uint8_t addr;						/* ��ַλ */
	uint8_t cmdType;					/* ������ */
	int16_t dataLength;					/* ���ݳ��� */
	int16_t dataH;						/* ���ݸ�λ */
	int16_t dataL;						/* ���ݵ�λ */
	int16_t crcL;						/* CRC��λ */
	uint8_t crcH;						/* CRC��λ */
} NOISE_RecvStrcutTypedef;

typedef struct
{
	NOISE_RecvStrcutTypedef buffer;
	uint8_t size;										/* �������ݵĳ��� */
	FunctionalState status;								/* ����״̬ */
} NOISE_RecvTypedef;

#pragma pack()

/******************************************************************************/
void NOISE_Init(void);
void NOISE_Require(void);
void NOISE_Process(void);
void NOISE_UartIdleDeal(void);

#endif
