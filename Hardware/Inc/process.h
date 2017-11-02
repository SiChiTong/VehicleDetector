#ifndef __PROCESS_H
#define __PROCESS_H

/******************************************************************************/
#include "stm32l1xx_hal.h"
#include "main.h"

#include "accelerate.h"
#include "noise.h"
#include "pressure.h"
#include "analog.h"

/******************************************************************************/
typedef enum
{
	PROCESS_MODE_INVALID,							/* ��Чģʽ */
	PROCESS_MODE_DETECTED_STEERING_WHEEL_FORCE,		/* ������ת������� */
	PROCESS_MODE_DETECTED_STEERING_WHEEL_ANGLE,		/* ������ת�Ǽ�� */
	PROCESS_MODE_DETECTED_BRAKING_DISTANCE,			/* �ƶ������� */
	PROCESS_MODE_DETECTED_PEDAL_FORCE,				/* �ƶ�̤������� */
	PROCESS_MODE_DETECTED_HAND_BRAKE_FORCE,			/* ��ɲ�ƶ������ */
	PROCESS_MODE_DETECTED_NOISE,					/* ���ȼ�� */
	PROCESS_MODE_DETECTED_SIDESLIP_DISTANCE,		/* �໬����� */
	PROCESS_MODE_DETECTED_DOWN_VELOCITY,			/* �����½��ٶȼ�� */
	PROCESS_MODE_DETECTED_BATTERY_CAPACITY,			/* ﮵�ص������ */
} PROCESS_ModeEnum;

typedef struct
{
	double steeringWheelForce;						/* ������ת���� */
	double steeringWheelAngle;						/* ������ת�� */
	double brakeVelocity;							/* �ƶ��ٶ� */
	double brakeVelocityInit;						/* �ƶ����ٶ� */
	double brakeDistance;							/* �ƶ����� */
	double pedalForce;								/* ̤���� */
	double handBrakeForce;							/* ��ɲ�� */
	float  noise;									/* ����ֵ */
	double downVelocity;							/* �½��ٶ� */
	uint8_t batteryCapacity;						/* ��ص��� */
} ItemValueTypedef;

typedef struct
{
	double steeringWheelForce;						/* ������ת�������ֵ */
	double steeringWheelAngle;						/* ������ת�����ֵ */
	double pedalForce;								/* ̤�������ֵ */
	double handBrakeForce;							/* ��ɲ�����ֵ */
	float  noise;									/* �������ֵ */
} ItemZeroValueTypedef;

/******************************************************************************/
extern PROCESS_ModeEnum PROCESS_Mode;

/******************************************************************************/
void PROCESS(void);
void ZeroCalibration(void);

#endif
