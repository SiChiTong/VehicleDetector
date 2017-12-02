#ifndef __ULTRASONICWAVE_H
#define __ULTRASONICWAVE_H

/******************************************************************************/
#include "stm32l1xx_hal.h"
#include "main.h"
#include "public.h"

/******************************************************************************/
#define ULTRASONICWAVE_HEAD					(0xFF)
#define ULTRASONICWAVE_TIME_PERIOD			(double)(0.5)	/* ��������ȡ���ڣ���λs */

#define ULTRASONICWAVE_UART					(huart2)
#define ULTRASONICWAVE_UART_DMA_RX_GL_FLAG	(DMA_FLAG_GL6)
#define ULTRASONICWAVE_UART_RX_BYTE_MAX		(20)
//#define ULTRASONICWAVE_PERIOD				(99)

/******************************************************************************/
typedef struct
{
	uint8_t head;						/* ֡ͷ */
	uint8_t dataH;						/* ���� */
	uint8_t dataL;
	uint8_t checkSum;					/* У��� */
} ULTRASONICWAVE_RecvStrcutTypedef;

typedef struct
{
	ULTRASONICWAVE_RecvStrcutTypedef buffer;
	uint8_t size;										/* �������ݵĳ��� */
	FunctionalState status;								/* ����״̬ */
} ULTRASONICWAVE_RecvTypedef;

/******************************************************************************/
void ULTRASONICWAVE_Require(void);
void ULTRASONICWAVE_Process(void);
void ULTRASONICWAVE_UartIdleDeal(void);

#endif
