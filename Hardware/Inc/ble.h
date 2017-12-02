#ifndef __BLE_H
#define __BLE_H

/******************************************************************************/
#include "stm32l1xx_hal.h"
#include "main.h"

#include "accelerate.h"
#include "process.h"
#include "analog.h"

/******************************************************************************/
#define BLE_UART 						(huart1)
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
	BLE_CMD_TYPE_DETECTED_STEERING_WHEEL_FORCE_AND_ANGLE,/* ����������ת�����ͽǶȼ�� */
	BLE_CMD_TYPE_DETECTED_PEDAL_FORCE_AND_BRAKING_DISTANCE = 0X06,/* ����̤�������ƶ������� */
	BLE_CMD_TYPE_DETECTED_HAND_BRAKE_FORCE = 0X08,				/* ������ɲ�ƶ������ */
	BLE_CMD_TYPE_DETECTED_NOISE,						/* �������ȼ�� */
	BLE_CMD_TYPE_DETECTED_SIDESLIP_DISTANCE,			/* �����໬����� */
	BLE_CMD_TYPE_DETECTED_DOWN_VELOCITY,				/* ���������½��ٶȼ�� */
	BLE_CMD_TYPE_DETECTED_GRADIENT,						/* �¶ȼ�� */
	BLE_CMD_TYPE_DETECTED_BATTERY_CAPACITY,				/* ������ص������ */

	BLE_DATA_TYPE_STEERING_WHELL_FORCE_AND_ANGLE = 0x10,		/* ������ת�����ͽǶȼ�� */

	/* �ƶ����� */
	BLE_DATA_TYPE_PEDAL_FORCE_AND_BRAKING_DISTANCE = 0x30,		/* ����̤�������ƶ������� */

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
	BLE_DATA_TYPE_DOWN_VELOCITY = 0x80,					/* ������� */

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

/******************************************************************************/
#pragma pack(push)
#pragma pack(1)						/* ���ֽڶ��� */

typedef struct
{
	uint16_t distance;
	double   speed;
} DownVelocity_SendBufferTypedef;

typedef struct
{
	double force;
	double angle;
} SteeringWheel_ForceAndAngleTypedef;

typedef struct
{
	double pedalForce;
	double initSpeed;
	double speed;
	double distance;
} PedalForce_BrakeDistanceTypedef;


typedef struct
{
	uint8_t head;						/* ֡ͷ */
	uint8_t type;						/* �������� */
	uint8_t length;						/* ���ݳ��� */
	union
	{
		uint8_t buffer[50];				/* ���� */
		double  data;					/* ���� */
		DownVelocity_SendBufferTypedef DownVelocity_SendBuffer;
		SteeringWheel_ForceAndAngleTypedef SteeringWheel_ForceAndAngle;
		PedalForce_BrakeDistanceTypedef PedalForce_BrakeDistance;
	} pack;
	uint8_t verify;						/* У��� */
	uint8_t tail;						/* ֡β */
} BLE_SendStructTypedef;				/* �������ͽṹ�� */

#pragma pack(pop)

/******************************************************************************/
void BLE_Init(void);
void BLE_UartIdleDeal(void);
void BLE_Process(void);
void BLE_SendBytes(BLE_CmdDataTypeEnum type);

#endif
