#ifndef __ULTRASONICWAVE_H
#define __ULTRASONICWAVE_H

/******************************************************************************/
#include "stm32l1xx_hal.h"
#include "main.h"
#include "public.h"

/******************************************************************************/
#define ULTRASONICWAVE_HEAD					(0xFF)
#define ULTRASONICWAVE_TIME_PERIOD			(0.1)	/* ��������ȡ���ڣ���λs */

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

#endif
