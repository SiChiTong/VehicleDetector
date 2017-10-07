#ifndef __BLE_H
#define __BLE_H

/******************************************************************************/
#include "stm32l1xx_hal.h"
#include "main.h"

#include "accelerate.h"
#include "process.h"

/******************************************************************************/
#define BLE_UART 						(huart3)
#define BLE_UART_RX_BYTE_MAX			(30)		/* ���ٶȴ����������������ֽڳ��� */
#define BLE_UART_DMA_RX_GL_FLAG			(DMA_FLAG_GL3)

/******************************************************************************/
typedef enum
{
	/* �豸������� */
	BLE_CMD_TYPE_CLEAR_SENSOR_CACHE = 0x00,				/* ������������� */
	BLE_CMD_TYPE_SWITCH_OFF,							/* �Ͽ����� */
	BLE_CMD_TYPE_STOP_DETECTED,							/* ֹͣ��� */

	/* ������ת���� */
	BLE_CMD_TYPE_STEERING_WHEEL_FORCE_SET_ZERO = 0x10,	/* ��������λУ�� */
	BLE_CMD_TYPE_DETECTED_STEERING_WHEEL_FORCE,			/* ����������ת������� */
	BLE_DATA_TYPE_STEERING_WHEEL_FORCE = 0x18,			/* ʵʱת����ֵ */
	BLE_DATA_TYPE_STEERING_WHEEL_FORCE_MAX,				/* ���ת����ֵ */

	/* ������ת�� */
	BLE_CMD_TYPE_STEERING_WHEEL_ANGLE_SET_ZERO = 0x20,	/* ��������λУ�� */
	BLE_CMD_TYPE_DETECTED_STEERING_WHEEL_ANGLE,			/* ����������ת�Ǽ�� */
	BLE_DATA_TYPE_STEERING_WHEEL_ANGLE = 0x28,			/* ʵʱת��Ƕ� */
	BLE_DATA_TYPE_STEERING_WHEEL_ANGLE_MAX,				/* ���ת��Ƕ� */

	/* �ƶ����� */
	BLE_CMD_TYPE_BRAKING_DISTANCE_SET_ZERO = 0x30,		/* ��������λУ�� */
	BLE_CMD_TYPE_DETECTED_BRAKING_DISTANCE,				/* �����ƶ������� */
	BLE_DATA_TYPE_BRAKING_INITIAL_VELOCITY = 0x38,		/* ���ٶ�ֵ */
	BLE_DATA_TYPE_BRAKING_ACCELERATE_AVERAGE,			/* ƽ�����ٶ�ֵ */
	BLE_DATA_TYPE_BRAKING_DISTANCE,						/* �ƶ����� */

	/* ̤���� */
	BLE_CMD_TYPE_PEDAL_FORCE_SET_ZERO = 0x40,			/* ��������λУ�� */
	BLE_CMD_TYPE_DETECTED_PEDAL_FORCE,					/* �����ƶ�̤������� */
	BLE_DATA_TYPE_PEDAL_FORCE = 0x48,					/* ʵʱ̤����ֵ */
	BLE_DATA_TYPE_PEDAL_FORCE_MAX,						/* ���̤����ֵ */

	/* ��ɲ�� */
	BLE_CMD_TYPE_HAND_BRAKE_FORCE_SET_ZERO = 0x50,		/* ��������λУ�� */
	BLE_CMD_TYPE_DETECTED_HAND_BRAKE_FORCE,				/* ������ɲ�ƶ������ */
	BLE_DATA_TYPE_HAND_BRAKE_FORCE = 0x58,				/* ʵʱ��ɲ��ֵ */
	BLE_DATA_TYPE_HAND_BRAKE_FORCE_MAX,					/* �����ɲ��ֵ */

	/* ���� */
	BLE_CMD_TYPE_NOISE_SET_ZERO = 0x60,					/* ��������λУ�� */
	BLE_CMD_TYPE_DETECTED_NOISE,						/* �������ȼ�� */
	BLE_DATA_TYPE_NOISE = 0x68,							/* ʵʱ����ֵ */
	BLE_DATA_TYPE_NOISE_MAX,							/* �������ֵ */

	/* �໬�� */
	BLE_CMD_TYPE_SIDESLIP_DISTANCE_SET_ZERO = 0x70,		/* ��������λУ�� */
	BLE_CMD_TYPE_DETECTED_SIDESLIP_DISTANCE,			/* �����໬����� */
	BLE_DATA_TYPE_SIDESLIP_FORCE = 0x78,				/* ʵʱ�໬��ֵ */
	BLE_DATA_TYPE_SIDESLIP_DISTANCE_MAX,				/* ���໬��ֵ */

	/* �½��ٶ� */
	BLE_CMD_TYPE_DOWN_VELOCITY_SET_ZERO = 0x80,			/* ��������λУ�� */
	BLE_CMD_TYPE_DETECTED_DOWN_VELOCITY,				/* ���������½��ٶȼ�� */
	BLE_DATA_TYPE_DOWN_DISTANCE = 0x88,					/* ������ؾ��� */
	BLE_DATA_TYPE_DOWN_VELOCITY,						/* ʵʱ�����½��ٶ� */
	BLE_DATA_TYPE_DOWN_VELOCITY_MAX,					/* ��������½��ٶ� */
} BLE_CmdDataTypeEnum;



/******************************************************************************/
#pragma pack(1)
typedef struct
{
	uint8_t buffer[BLE_UART_RX_BYTE_MAX];				/* ���ն������� */
//	ACCELERATE_RecvStrcutTypedef buffer;
	uint8_t size;										/* �������ݵĳ��� */
	FunctionalState status;								/* ����״̬ */
} BLE_RecvTypedef;



#pragma pack()

/******************************************************************************/
void BLE_Init(void);
void BLE_UartIdleDeal(void);
void BLE_Process(void);

#endif
