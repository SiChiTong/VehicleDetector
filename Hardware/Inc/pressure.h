#ifndef __PRESSURE_H
#define __PRESSURE_H

/******************************************************************************/
#include "stm32l1xx_hal.h"
#include "main.h"

#include "public.h"

/******************************************************************************/
#define PRESSURE_DT_READ() \
					HAL_GPIO_ReadPin(PRESSURE_DT_GPIO_Port, PRESSURE_DT_Pin)
#define PRESSURE_SCK_WRITE(status) \
			HAL_GPIO_WritePin(PRESSURE_SCK_GPIO_Port, PRESSURE_SCK_Pin, status)

/******************************************************************************/
#define HZ711_RATE_10HZ						(10)
#define HX711_RATE_80HZ						(80)
#define HX711_RATE							(HZ711_RATE_10HZ)

#define PRESSURE_RANGE_PEDAL_FORCE			(double)(100 * 9.8) /* 1kg = 9.8N */
#define PRESSURE_RANGE_STEERING_WHEEL_FORCE (double)(30 * 9.8)
#define PRESSURE_RANGE_HAND_BRAKE_FORCE		(double)(30 * 9.8)
#define PRESSURE_RANGE_MEDIAN_VALUE			(0x7FFFFF)

/******************************************************************************/
typedef struct
{
	uint32_t pedalValueMax;					/* ̤�������ֵ */
	uint32_t pedalValueMin;					/* ̤������Сֵ */
	uint32_t pedalValueRange;				/* ̤������Χ */
	uint32_t steeringWheelForceMax;			/* ת�������ֵ */
	uint32_t steeringWheelForceMin;			/* ת�������ֵ */
	uint32_t steeringWheelForceRange;		/* ת������Χ */
	uint32_t handBrakeForceMax;  			/* ��ɲ�����ֵ */
	uint32_t handBrakeForceMin;				/* ��ɲ����Сֵ */
	uint32_t handBrakeForceRange;			/* ��ɲ����Χ */
} PRESSURE_ParamTypedef;

/******************************************************************************/
void PRESSURE_Init(void);
void PRESSURE_GetPedalForce(void);
void PRESSURE_GetSteeringWheelForce(void);
void PRESSURE_GetHandBrakeForce(void);
uint32_t HX711_ReadValue(void);
#endif
