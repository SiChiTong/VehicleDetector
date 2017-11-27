#ifndef __BLE_H
#define __BLE_H

/******************************************************************************/
#include "stm32l1xx_hal.h"
#include "main.h"

#include "accelerate.h"
#include "process.h"
#include "analog.h"

/******************************************************************************/
#define BLE_UART 						(huart3)
#define BLE_UART_RX_BYTE_MAX			(30)		/* ���ٶȴ����������������ֽڳ��� */
#define BLE_UART_DMA_RX_GL_FLAG			(DMA_FLAG_GL3)

#define BLE_PROTOCOL_HEAD				(0xAA)
#define BLE_PROTOCOL_TAIL				(0x55)
/******************************************************************************/
typedef enum
{
	/* �豸������� */
	BLE_CMD_TYPE_SENSOR_VALUE_SET_ZERO = 0x00,			/* ��������λУ�� */
	BLE_CMD_TYPE_CLEAR_SENSOR_CACHE,					/* ������������� */
	BLE_CMD_TYPE_SWITCH_OFF,							/* �Ͽ����� */
	BLE_CMD_TYPE_STOP_DETECTED,							/* ֹͣ��� */
	BLE_CMD_TYPE_DETECTED_STEERING_WHEEL_FORCE,			/* ����������ת������� */
	BLE_CMD_TYPE_DETECTED_STEERING_WHEEL_ANGLE,			/* ����������ת�Ǽ�� */
	BLE_CMD_TYPE_DETECTED_BRAKING_DISTANCE,				/* �����ƶ������� */
	BLE_CMD_TYPE_DETECTED_PEDAL_FORCE,					/* �����ƶ�̤������� */
	BLE_CMD_TYPE_DETECTED_HAND_BRAKE_FORCE,				/* ������ɲ�ƶ������ */
	BLE_CMD_TYPE_DETECTED_NOISE,						/* �������ȼ�� */
	BLE_CMD_TYPE_DETECTED_SIDESLIP_DISTANCE,			/* �����໬����� */
	BLE_CMD_TYPE_DETECTED_DOWN_VELOCITY,				/* ���������½��ٶȼ�� */
	BLE_CMD_TYPE_DETECTED_GRADIENT,						/* �¶ȼ�� */
	BLE_CMD_TYPE_DETECTED_BATTERY_CAPACITY,				/* ������ص������ */
	
	/* ������ת���� */
	BLE_DATA_TYPE_STEERING_WHEEL_FORCE = 0x10,			/* ʵʱת����ֵ */
	BLE_DATA_TYPE_STEERING_WHEEL_FORCE_MAX,				/* ���ת����ֵ */

	/* ������ת�� */
	BLE_DATA_TYPE_STEERING_WHEEL_ANGLE = 0x20,			/* ʵʱת��Ƕ� */
	BLE_DATA_TYPE_STEERING_WHEEL_ANGLE_MAX,				/* ���ת��Ƕ� */

	/* �ƶ����� */
	BLE_DATA_TYPE_BRAKING_INITIAL_VELOCITY = 0x30,		/* ���ٶ�ֵ */
	BLE_DATA_TYPE_BRAKING_ACCELERATE_AVERAGE,			/* ƽ�����ٶ�ֵ */
	BLE_DATA_TYPE_BRAKING_DISTANCE,						/* �ƶ����� */

	/* ̤���� */
	BLE_DATA_TYPE_PEDAL_FORCE = 0x40,					/* ʵʱ̤����ֵ */
	BLE_DATA_TYPE_PEDAL_FORCE_MAX,						/* ���̤����ֵ */

	/* ��ɲ�� */
	BLE_DATA_TYPE_HAND_BRAKE_FORCE = 0x50,				/* ʵʱ��ɲ��ֵ */
	BLE_DATA_TYPE_HAND_BRAKE_FORCE_MAX,					/* �����ɲ��ֵ */

	/* ���� */
	BLE_DATA_TYPE_NOISE = 0x60,							/* ʵʱ����ֵ */
	BLE_DATA_TYPE_NOISE_MAX,							/* �������ֵ */

	/* �໬�� */
	BLE_DATA_TYPE_SIDESLIP_FORCE = 0x70,				/* ʵʱ�໬��ֵ */
	BLE_DATA_TYPE_SIDESLIP_DISTANCE_MAX,				/* ���໬��ֵ */

	/* �½��ٶ� */
	BLE_DATA_TYPE_DOWN_DISTANCE = 0x80,					/* ������ؾ��� */
	BLE_DATA_TYPE_DOWN_VELOCITY,						/* ʵʱ�����½��ٶ� */
	BLE_DATA_TYPE_DOWN_VELOCITY_MAX,					/* ��������½��ٶ� */

	/* �¶ȼ�� */
	BLE_DATA_TYPE_GRADIENT = 0x90,						/* �¶�ֵ */

	/* ﮵�ص��� */
	BLE_DATA_TYPE_BATTERY_CAPACITY = 0XA0,				/* ��ص��� */
} BLE_CmdDataTypeEnum;

typedef enum
{
	BLE_CMD_SUBTYPE_SET_ZORO_STEERING_WHEEL_FORCE = 0x01, /* ת�������� */
	BLE_CMD_SUBTYPE_SET_ZORO_STEERING_WHEEL_ANGLE,		  /* ת������ */
	BLE_CMD_SUBTYPE_SET_ZORO_BRAKING_DISTANCE,			  /* �ƶ��������� */
	BLE_CMD_SUBTYPE_SET_ZORO_PEDAL_FORCE,				  /* ̤�������� */
	BLE_CMD_SUBTYPE_SET_ZORO_HAND_BRAKE_FORCE,			  /* ��ɲ������ */
	BLE_CMD_SUBTYPE_SET_ZORO_NOISE,						  /* �������� */
	BLE_CMD_SUBTYPE_SET_ZORO_SIDESLIP_FORCE,			  /* �໬������ */
	BLE_CMD_SUBTYPE_SET_ZORO_DOWN_VELOCITY,				  /* �½��ٶ����� */
	BLE_CMD_SUBTYPE_SET_ZERO_GRADIENT,					  /* �¶����� */
} BLE_CmdSubtypeEnum;

/******************************************************************************/
#pragma pack(1)

typedef struct
{
	uint8_t head;						/* ֡ͷ */
	uint8_t cmdType;					/* �������� */
	uint8_t cmdSubType;					/* ���������� */
	uint8_t verify;						/* У�� */
	uint8_t tail;						/* ֡β */
} BLE_RecvStruct;

typedef struct
{
	BLE_RecvStruct buffer;				/* ���ն������� */
	uint8_t size;						/* �������ݵĳ��� */
	FunctionalState status;				/* ����״̬ */
} BLE_RecvTypedef;

typedef struct
{
	uint8_t head;						/* ֡ͷ */
	uint8_t type;						/* �������� */
	uint8_t length;						/* ���ݳ��� */
	char    data[6];					/* ���ݣ�ASCII�� ���ݷ�Χ-999.9~9999.9*/
	uint8_t verify;						/* У��� */
	uint8_t tail;						/* ֡β */
} BLE_SendStructTypedef;				/* �������ͽṹ�� */

#pragma pack()

/******************************************************************************/
void BLE_Init(void);
void BLE_UartIdleDeal(void);
void BLE_Process(void);
void BLE_SendBytes(BLE_CmdDataTypeEnum type, char* value);

#endif
