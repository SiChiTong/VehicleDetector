#ifndef __ACCELERATE_H
#define __ACCELERATE_H

/******************************************************************************/
#include "stm32l1xx_hal.h"
#include "main.h"

#include "public.h"

#include "usart.h"

/******************************************************************************/
#define ACCELERATE_UART 						(huart1)
#define ACCELERATE_UART_RX_BYTE_MAX				(30)		/* ���ٶȴ����������������ֽڳ��� */
#define ACCELERATE_UART_DMA_RX_GL_FLAG			(DMA_FLAG_GL5)
#define ACCELERATE_PROTOCOL_HEAD				(0x55)

#define ACCELERATE_TYPE_DATE_MARK					(1 << 0)	/* ʱ����� */
#define ACCELERATE_TYPE_ACCELERATE_SPEED_MARK		(1 << 1)	/* ���ٶ���� */
#define ACCELERATE_TYPE_ANGULAR_SPEED_MARK			(1 << 2)	/* ���ٶ���� */
#define ACCELERATE_TYPE_ANGLE_MARK					(1 << 3)	/* �Ƕ���� */
#define ACCELERATE_TYPE_MAGNETIC_FIELD_MARK			(1 << 4)	/* �ų���� */
#define ACCELERATE_TYPE_PORT_STATUS_MARK			(1 << 5)	/* �˿�״̬������� */
#define ACCELERATE_TYPE_AIR_PRESS_AND_ALTITUDE_MARK	(1 << 6)	/* ��ѹ���߶���� */
#define ACCELERATE_TYPE_LONGITUDE_AND_LATITUDE_MARK	(1 << 7)	/* ��γ����� */
#define ACCELERATE_TYPE_GROUND_SPEED_MARK			(1 << 0)	/* ������� */
#define ACCELERATE_TYPE_FOUR_ELEMENTS_MARK			(1 << 1)	/* ��Ԫ����� */
#define ACCELERATE_TYPE_LOCATION_MARK				(1 << 2)	/* ���Ƕ�λ������� */

#define ACCELERATE_INTEGRAL_TIME				(0.1)		/* ����ʱ��Ϊ100ms */

#define ACCELERATE_DIGITAL_RANGE		(double)(32768)	  /* ���ַ�Χ��ǿתdouble */
#define ACCELERATE_RANGE_ACCELERATE		(double)(16 * 9.8) /* ���ٶȷ�Χ */
#define ACCELERATE_RANGE_ANGLE			(double)(180.0)    /* �Ƕȷ�Χ */

/******************************************************************************/
typedef enum
{
	ACCELERATE_TYPE_DATE = 0x50,				/* ʱ����� */
	ACCELERATE_TYPE_ACCELERATE_SPEED,			/* ���ٶ���� */
	ACCELERATE_TYPE_ANGULAR_SPEED,				/* ���ٶ���� */
	ACCELERATE_TYPE_ANGLE,						/* �Ƕ���� */
	ACCELERATE_TYPE_MAGNETIC_FIELD,				/* �ų���� */
	ACCELERATE_TYPE_PORT_STATUS,				/* �˿�״̬������� */
	ACCELERATE_TYPE_AIR_PRESS_AND_ALTITUDE,		/* ��ѹ���߶���� */
	ACCELERATE_TYPE_LONGITUDE_AND_LATITUDE,		/* ��γ����� */
	ACCELERATE_TYPE_GROUND_SPEED,				/* ������� */
	ACCELERATE_TYPE_FOUR_ELEMENTS,				/* ��Ԫ����� */
	ACCELERATE_TYPE_LOCATION,					/* ���Ƕ�λ������� */
} ACCELERATE_TypeEnum;

typedef enum
{
	ACCELERATE_ADDR_SAVE,						/* ���浱ǰ���� */
	ACCELERATE_ADDR_CALSW,						/* У׼ */
	ACCELERATE_ADDR_RSW,						/* �ش��������� */
	ACCELERATE_ADDR_RATE,						/* �ش��������� */
	ACCELERATE_ADDR_BAUD,						/* ���ڲ����� */
	ACCELERATE_ADDR_AXOFFSET,					/* X����ٶ���ƫ */
	ACCELERATE_ADDR_AYOFFSET,					/* Y����ٶ���ƫ */
	ACCELERATE_ADDR_AZOFFSET,					/* Z����ٶ���ƫ */
	ACCELERATE_ADDR_GXOFFSET,					/* X����ٶ���ƫ */
	ACCELERATE_ADDR_GYOFFSET,					/* Y����ٶ���ƫ */
	ACCELERATE_ADDR_GZOFFSET,					/* Z����ٶ���ƫ */
	ACCELERATE_ADDR_HXOFFSET,					/* X��ų���ƫ */
	ACCELERATE_ADDR_HYOFFSET,					/* Y��ų���ƫ */
	ACCELERATE_ADDR_HZOFFSET,					/* Z��ų���ƫ */
	ACCELERATE_ADDR_D0MODE,						/* D0ģʽ */
	ACCELERATE_ADDR_D1MODE,						/* D1ģʽ */
	ACCELERATE_ADDR_D2MODE,						/* D2ģʽ */
	ACCELERATE_ADDR_D3MODE,						/* D3ģʽ */
	ACCELERATE_ADDR_D0PWMH,						/* D0PWM�ߵ�ƽ��� */
	ACCELERATE_ADDR_D1PWMH,						/* D1PWM�ߵ�ƽ��� */
	ACCELERATE_ADDR_D2PWMH,						/* D2PWM�ߵ�ƽ��� */
	ACCELERATE_ADDR_D3PWMH,						/* D3PWM�ߵ�ƽ��� */
	ACCELERATE_ADDR_D0PWMT,						/* D0PWM���� */
	ACCELERATE_ADDR_D1PWMT,						/* D1PWM���� */
	ACCELERATE_ADDR_D2PWMT,						/* D2PWM���� */
	ACCELERATE_ADDR_D3PWMT,						/* D3PWM���� */
	ACCELERATE_ADDR_IICADDR,					/* IIC��ַ */
	ACCELERATE_ADDR_LEDOFF,						/* �ر�LEDָʾ�� */
	ACCELERATE_ADDR_GPSBAUD,					/* GPS���Ӳ����� */

	/* IICͨѶ��ַ */
	ACCELERATE_ADDR_YYMM = 0x30,				/* �ꡢ�� */
	ACCELERATE_ADDR_DDHH,						/* �ա�ʱ */
	ACCELERATE_ADDR_MMSS,						/* �֡��� */
	ACCELERATE_ADDR_MS,							/* ���� */
	ACCELERATE_ADDR_AX,
	ACCELERATE_ADDR_AY,
	ACCELERATE_ADDR_AZ,
	ACCELERATE_ADDR_GX,
	ACCELERATE_ADDR_GY,
	ACCELERATE_ADDR_GZ,
	ACCELERATE_ADDR_HX,
	ACCELERATE_ADDR_HY,
	ACCELERATE_ADDR_HZ,
	ACCELERATE_ADDR_ROLL,
	ACCELERATE_ADDR_PITCH,
	ACCELERATE_ADDR_YAW,
	ACCELERATE_ADDR_TEMP,
	ACCELERATE_ADDR_D0STATUS,
	ACCELERATE_ADDR_D1STATUS,
	ACCELERATE_ADDR_D2STATUS,
	ACCELERATE_ADDR_D3STATUS,
	ACCELERATE_ADDR_PRESSUREL,
	ACCELERATE_ADDR_PRESSUREH,
	ACCELERATE_ADDR_HIGHTL,
	ACCELERATE_ADDR_HIGHTH,
	ACCELERATE_ADDR_LONL,
	ACCELERATE_ADDR_LONH,
	ACCELERATE_ADDR_LATL,
	ACCELERATE_ADDR_LATH,
	ACCELERATE_ADDR_GPS_HEIGHT,
	ACCELERATE_ADDR_GPS_YAW,
	ACCELERATE_ADDR_GPS_VL,
	ACCELERATE_ADDR_GPS_VH,
	ACCELERATE_ADDR_Q0,
	ACCELERATE_ADDR_Q1,
	ACCELERATE_ADDR_Q2,
	ACCELERATE_ADDR_Q3,
}ACCELERATE_AddrEnum;

/******************************************************************************/
#pragma pack(1)
typedef struct
{
	uint8_t head;								/* ��ͷ */
	uint8_t type;								/* �������� */
	int16_t data1;								/* ����1 */
	int16_t data2;								/* ����2 */
	int16_t data3;								/* ����3 */
	int16_t data4;								/* ����4 */
	uint8_t sum;								/* У��� */
} ACCELERATE_RecvStrcutTypedef;					/* ���սṹ�� */

typedef struct
{
	uint8_t head1;								/* ��ͷ1 */
	uint8_t head2;								/* ��ͷ2 */
	uint8_t address;							/* ��ַ */
	uint8_t dataL;								/* ����L */
	uint8_t dataH;								/* ����H */
} ACCELERATE_SendStrcutTypedef;					/* ���ͽṹ�� */

typedef struct
{
	ACCELERATE_RecvStrcutTypedef buffer;		/* ���ն������� */
	uint8_t size;								/* �������ݵĳ��� */
	FunctionalState status;						/* ����״̬ */
} ACCELERATE_RecvTypedef;

#pragma pack()
/******************************************************************************/
void ACCELERATE_Init(void);
void ACCELERATE_Process(void);
void ACCELERATE_UartIdleDeal(void);
void ACCELERATE_SetBackInfo(uint8_t RSWL, uint8_t RSWH);

#endif
