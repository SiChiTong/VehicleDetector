#include "pressure.h"

/******************************************************************************/
PRESSURE_ParamTypedef PRESSURE_Param;

/******************************************************************************/
static uint32_t HX711_ReadValue(void);

/*******************************************************************************
 *
 */
void PRESSURE_Init(void)
{
	PRESSURE_Param.pedalValueMax = 0xFFFFFF;
	PRESSURE_Param.pedalValueMin = 0x7FEEEE;
	PRESSURE_Param.pedalValueRange = PRESSURE_Param.pedalValueMax
			- PRESSURE_Param.pedalValueMin;

	PRESSURE_Param.steeringWheelForceMax = 0xFFFFFF;
	PRESSURE_Param.steeringWheelForceMin = 0x7FEEEE;
	PRESSURE_Param.steeringWheelForceRange = PRESSURE_Param.steeringWheelForceMax
			- PRESSURE_Param.steeringWheelForceMin;

	PRESSURE_Param.handBrakeForceMax = 0xFFFFFF;
	PRESSURE_Param.handBrakeForceMin = 0x7FEEEE;
	PRESSURE_Param.handBrakeForceRange = PRESSURE_Param.handBrakeForceMax
			- PRESSURE_Param.handBrakeForceMin;
}

/*******************************************************************************
 * @brief ��ȡ̤����ֵ
 * 		  ̤���������Թ�ϵ
 * 		  1.�����Сֱֵ��ȡ����
 * 		  2.�������
 * 		  3.���
 */
void PRESSURE_GetPedalForce(void)
{
	double pedal = 0.0;
	uint32_t value = 0;

	/* ��ȡ24bitADֵ */
	value = HX711_ReadValue();
	/* �ж�ADֵ�Ϸ� */
	if (value > PRESSURE_Param.pedalValueMin)
	{
		value -= PRESSURE_Param.pedalValueMin;
	}
	else
	{
		value = 0;
	}
	/* ת��̤����ֵ */
	pedal = (value / (double)PRESSURE_Param.pedalValueRange)
			* PRESSURE_RANGE_PEDAL_FORCE;

#if DEVICE_TEST_MODE_ENABLE
	printf("HX711=%d,%x\r\n", value, value);
	printf("̤����=%.1fN\r\n", pedal);
#else

#endif
}

/*******************************************************************************
 * @brief ��ȡת����
 */
void PRESSURE_GetSteeringWheelForce(void)
{
	double steeringWheelForce = 0.0;
	uint32_t value = 0;

	/* ��ȡ24bitADֵ */
	value = HX711_ReadValue();
	/* �ж�ADֵ�Ϸ� */
	if (value > PRESSURE_Param.steeringWheelForceMin)
	{
		value -= PRESSURE_Param.steeringWheelForceMin;
	}
	else
	{
		value = 0;
	}
	/* ת��̤����ֵ */
	steeringWheelForce = (value / (double)PRESSURE_Param.steeringWheelForceRange)
			* PRESSURE_RANGE_STEERING_WHEEL_FORCE;

#if DEVICE_TEST_MODE_ENABLE
	printf("HX711=%d,%x\r\n", value, value);
	printf("ת����=%.1fN\r\n", steeringWheelForce);
#else

#endif
}

/*******************************************************************************
 * @brief ��ȡת����
 */
void PRESSURE_GetHandBrakeForce(void)
{
	double handBrakeForce = 0.0;
	uint32_t value = 0;

	/* ��ȡ24bitADֵ */
	value = HX711_ReadValue();
	/* �ж�ADֵ�Ϸ� */
	if (value > PRESSURE_Param.handBrakeForceMin)
	{
		value -= PRESSURE_Param.handBrakeForceMin;
	}
	else
	{
		value = 0;
	}
	/* ת��̤����ֵ */
	handBrakeForce = (value / (double)PRESSURE_Param.handBrakeForceRange)
			* PRESSURE_RANGE_HAND_BRAKE_FORCE;

#if DEVICE_TEST_MODE_ENABLE
	printf("HX711=%d,%x\r\n", value, value);
	printf("��ɲ��=%.1fN\r\n", handBrakeForce);
#else

#endif
}

/*******************************************************************************
 * @brief ��HX711��˳���ȡ24λADֵ
 */
static uint32_t ReadValue(void)
{
	uint64_t value = 0;
	uint8_t  i;

	/* ����DSCK�ţ�ʹ��ADת�� */
	PRESSURE_SCK_WRITE(GPIO_PIN_RESET);
	/* �ȴ�DOUT�øߣ�˵��ת����� */
	while (PRESSURE_DT_READ());
	/* ˳���ȡ24λADֵ */
	for (i = 0; i < 24; i++)
	{
		/* DSCK�øߣ����������壩 */
		PRESSURE_SCK_WRITE(GPIO_PIN_SET);
		/* ����һλ���ȴ��½��ػ�ȡ���� */
		value = value << 1;
		PRESSURE_SCK_WRITE(GPIO_PIN_RESET);
		/* �½��ػ�ȡ���� */
		if (PRESSURE_DT_READ())
		{
			value++;
		}
	}
	/* ���͵�25�����壬��ʾ��һ��ת��ΪAͨ��128������ */
	PRESSURE_SCK_WRITE(GPIO_PIN_SET);
	value = value ^ 0x800000;
	PRESSURE_SCK_WRITE(GPIO_PIN_RESET);

	/* ȡ��16bits���� */
	return value;
}

/*******************************************************************************
 * @brief HX711Ĭ�ϲ���Ƶ��Ϊ10Hz����80Hz
 * 			��RATE = 0 : ���10Hz		RATE = DVDD : ���80Hz��
 * 			�������Ƶ�� = 10Hz������Ϊ����Ƶ�ʲ����ͱ����������˲����������˲�
 * 			����һ����Ϊ��RATE�Žӵأ����ø��ߣ�������ʱ�Ȱ��趨����Ƶ��Ϊ10Hz��
 *
 * 			ģ���������ѹVavdd = Vbg * (R1 + R2)/ R2
 * 			ע�⣺Vavdd��ֵҪ < ģ��Ĺ����ѹ - 100mV�����٣�
 *
 */
static uint32_t HX711_ReadValue(void)
{
	uint32_t sum = 0;
	
#if (HX711_RATE != HZ711_RATE_10HZ)
	uint8_t  i;
	
	for (i = 0; i < 10; i++)
	{
		sum += ReadValue();
	}

	sum /= 10;
#else
	sum = ReadValue();
#endif

	return sum;
}
