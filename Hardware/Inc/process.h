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
#define GET_VALUE_TIME_PERIOD			(double)(0.5)

#define PEDAL_FORCE_ZERO_VALUE			(0X7FFFFF)
#define PEDAL_FORCE_FULL_VALUE			(0XFFFFFF)

#define STEERING_WHEEL_FORCE_ZERO_VALUE			(0X7FFFFF)
#define STEERING_WHEEL_FORCE_FULL_VALUE			(0XFFFFFF)
#define STEERING_WHEEL_FORCE_FULL_VALUE_N		(0X000000)
/******************************************************************************/
typedef enum
{
	PROCESS_MODE_INVALID,							/* ��Чģʽ */
	PROCESS_MODE_DETECTED_STEERING_WHEEL_FORCE_AND_ANGLE,/* ������ת������� */
	PROCESS_MODE_DETECTED_PEDAL_FORCE_BRAKING_DISTANCE = 0x03,	/* ̤�������ƶ������� */
	PROCESS_MODE_DETECTED_HAND_BRAKE_FORCE = 0x05,			/* ��ɲ�ƶ������ */
	PROCESS_MODE_DETECTED_NOISE,					/* ���ȼ�� */
	PROCESS_MODE_DETECTED_SIDESLIP_DISTANCE,		/* �໬����� */
	PROCESS_MODE_DETECTED_DOWN_VELOCITY,			/* �����½��ٶȼ�� */
	PROCESS_MODE_DETECTED_GRADIENT,					/* �¶ȼ�� */
	PROCESS_MODE_DETECTED_BATTERY_CAPACITY,			/* ﮵�ص������ */
} PROCESS_ModeEnum;

typedef struct
{
	double steeringWheelForce;						/* ������ת���� */
	double steeringWheelAngle;						/* ������ת�� */
	double Ax;										/* ���ٶ� */
	double brakeVelocity;							/* �ƶ��ٶ� */
	double speed;									/* �ƶ��ٶȣ���λkm/h�� */
	double brakeVelocityInit;						/* �ƶ����ٶ� */
	double brakeDistance;							/* �ƶ����� */
	double pedalForce;								/* ̤���� */
	double handBrakeForce;							/* ��ɲ�� */
	float  noise;									/* ����ֵ */
	double sideSlipAngle;							/* �໬���Ƕ� */
	double sideSlipAx;								/* �໬�����ٶ� */
	double sideSlipVelocity;						/* �໬���ٶ� */
	double sideSlipDistance;						/* �໬������ */
	double sideSlipOffset;							/* �໬�� */
//	double downAx;									/* �½����ٶ� */
	uint16_t downVelocityDistance;					/* ��ؾ��� */
	double downVelocity;							/* �½��ٶ� */
	double gradientAverage;							/* �¶�ƽ��ֵ */
	double gradient;								/* �¶�ֵ */
	uint8_t batteryCapacity;						/* ��ص��� */
} ItemValueTypedef;

typedef struct
{
	FunctionalState steeringWheelForce;						/* ������ת���� */
	FunctionalState steeringWheelAngle;						/* ������ת�� */
	FunctionalState brakeAx;								/* ���ٶ� */
	FunctionalState pedalForce;								/* ̤���� */
	FunctionalState handBrakeForce;							/* ��ɲ�� */
	FunctionalState noise;									/* ����ֵ */
	FunctionalState sideSlip;								/* �໬�� */
	FunctionalState downVelocity;							/* �½��ٶ� */
	FunctionalState gradient;								/* �¶�ֵ */
	FunctionalState batteryCapacity;						/* ��ص��� */
} ItemValueSetZeroEnableTypedef;

typedef struct
{
	double Ax;										/* ���ٶ����ֵ */
	double steeringWheelForce;						/* ������ת�������ֵ */
	double steeringWheelAngle;						/* ������ת�����ֵ */
	double pedalForce;								/* ̤�������ֵ */
	double handBrakeForce;							/* ��ɲ�����ֵ */
	float  noise;									/* �������ֵ */
	double sideSlipAngle;							/* �໬���Ƕ� */
	double sideSlipAx;								/* �໬�����ٶ� */
	double sideSlipVelocity;						/* �໬���ٶ� */
	double sideSlipDistance;						/* �໬������ */
	double downAx;									/* �½����ٶ� */
	double gradient;								/* �¶�ֵ */
} ItemZeroValueTypedef;

/******************************************************************************/
extern PROCESS_ModeEnum PROCESS_Mode;

/******************************************************************************/
void PROCESS(void);
void ZeroCalibration(void);

#endif
