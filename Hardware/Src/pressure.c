#include "pressure.h"
#include "ble.h"
#include "oled.h"

/******************************************************************************/
PRESSURE_ParamTypedef PRESSURE_Param;

extern ItemValueTypedef     ItemValue;
extern ItemZeroValueTypedef ItemZeroValue;
extern ItemValueSetZeroEnableTypedef ItemValueSetZeroEnable;

/******************************************************************************/
static uint32_t HX711_ReadValue(void);

/*******************************************************************************
 * @brief ѹ��������������ʼ��
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
	uint32_t data = 0;
	char value[7];

	/* ��ȡ24bitADֵ */
	data = HX711_ReadValue();
	/* �ж�ADֵ�Ϸ� */
	if (data > PRESSURE_Param.pedalValueMin)
	{
		data -= PRESSURE_Param.pedalValueMin;
	}
	else
	{
		data = 0;
	}
	/* ת��̤����ֵ */
	ItemValue.pedalForce = (data / (double)PRESSURE_Param.pedalValueRange)
			* PRESSURE_RANGE_PEDAL_FORCE * 1.916;
	/* ���У׼ʹ�� */
	if (ItemValueSetZeroEnable.pedalForce == ENABLE)
	{
		ItemValueSetZeroEnable.pedalForce = DISABLE;
		ItemZeroValue.pedalForce = ItemValue.pedalForce;
	}

	/* ���У׼ */
	ItemValue.pedalForce -= ItemZeroValue.pedalForce;

	sprintf(value, "%6.1f", ItemValue.pedalForce);
#if DEVICE_OLED_DISPLAY_ENABLE
	OLED_ShowString(56, 2, value, sizeof(value));
#endif
#if DEVICE_BLE_SEND_ENABLE
	BLE_SendBytes(BLE_DATA_TYPE_PEDAL_FORCE, value);
#endif

	HAL_Delay(10);
}

/*******************************************************************************
 * @brief ��ȡת����
 */
void PRESSURE_GetSteeringWheelForce(void)
{
	uint32_t data = 0;
	char value[7];

	/* ��ȡ24bitADֵ */
	data = HX711_ReadValue();
	/* �ж�ADֵ�Ϸ� */
	if (data > PRESSURE_Param.steeringWheelForceMin)
	{
		data -= PRESSURE_Param.steeringWheelForceMin;
	}
	else
	{
		data = 0;
	}
	/* ת��ת����ֵ */
	ItemValue.steeringWheelForce =
			(data / (double)PRESSURE_Param.steeringWheelForceRange)
			* PRESSURE_RANGE_STEERING_WHEEL_FORCE * 5.44;

	if (ItemValueSetZeroEnable.steeringWheelForce == ENABLE)
	{
		ItemValueSetZeroEnable.steeringWheelForce = DISABLE;
		ItemZeroValue.steeringWheelForce = ItemValue.steeringWheelForce;
	}

	/* ���У׼ */
	ItemValue.steeringWheelForce -= ItemZeroValue.steeringWheelForce;

	sprintf(value, "%6.1f", ItemValue.steeringWheelForce);
#if DEVICE_OLED_DISPLAY_ENABLE
	OLED_ShowString(56, 2, value, sizeof(value));
#endif
#if DEVICE_BLE_SEND_ENABLE
	BLE_SendBytes(BLE_DATA_TYPE_STEERING_WHEEL_FORCE, value);
#endif

	HAL_Delay(10);
}

/*******************************************************************************
 * @brief ��ȡת����
 */
void PRESSURE_GetHandBrakeForce(void)
{
	uint32_t data = 0;
	char value[7];

	/* ��ȡ24bitADֵ */
	data = HX711_ReadValue();
	/* �ж�ADֵ�Ϸ� */
	if (data > PRESSURE_Param.handBrakeForceMin)
	{
		data -= PRESSURE_Param.handBrakeForceMin;
	}
	else
	{
		data = 0;
	}
	/* ת��̤����ֵ */
	ItemValue.handBrakeForce =
			(data / (double)PRESSURE_Param.handBrakeForceRange)
			* PRESSURE_RANGE_HAND_BRAKE_FORCE;

	if (ItemValueSetZeroEnable.handBrakeForce == ENABLE)
	{
		ItemValueSetZeroEnable.handBrakeForce = DISABLE;
		ItemZeroValue.handBrakeForce = ItemValue.handBrakeForce;
	}

	/* ���У׼ */
	ItemValue.handBrakeForce -= ItemZeroValue.handBrakeForce;

	sprintf(value, "%6.1f", ItemValue.handBrakeForce);
#if DEVICE_OLED_DISPLAY_ENABLE
	OLED_ShowString(56, 2, value, sizeof(value));
#endif
#if DEVICE_BLE_SEND_ENABLE
	BLE_SendBytes(BLE_DATA_TYPE_HAND_BRAKE_FORCE, value);
#endif

	HAL_Delay(10);
}

/*******************************************************************************
 * @brief ��HX711��˳���ȡ24λADֵ
 */
static uint32_t ReadValue(void)
{
	uint32_t value = 0;
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
