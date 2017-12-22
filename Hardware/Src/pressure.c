#include "pressure.h"
#include "ble.h"
#include "oled.h"

/******************************************************************************/
extern BLE_SendStructTypedef BLE_SendStruct;


extern FunctionalState HandBrake_SetZeroEnable;
extern double HandBrake_force;											/* ��ɲ�� */
extern double HandBrake_forceZeroValue;	/* ��ɲ����� */
extern double HandBrake_forceFullValue;	/* ��ɲ������ */

/******************************************************************************/

/*******************************************************************************
 * @brief ��ɲ��
 */
void PRESSURE_GetHandBrakeForce(void)
{
	int32_t data = 0;
	char value[7];

	/* ��ȡ24bitADֵ */
	data = HX711_ReadValue();

	if (HandBrake_SetZeroEnable == ENABLE)
	{
		HandBrake_SetZeroEnable = DISABLE;
		HandBrake_forceZeroValue = data;
	}

	data -= HandBrake_forceZeroValue;

	if (data > 0)
	{
		HandBrake_force = (data / (HandBrake_forceFullValue - HandBrake_forceZeroValue))
				* PRESSURE_RANGE_HAND_BRAKE_FORCE;
		/* �ڴ˴����У��ϵ�� */
		HandBrake_force = HandBrake_force * 1.0;
	}

#if DEVICE_OLED_DISPLAY_ENABLE
	sprintf(value, "%6.1f", HandBrake_force);
	OLED_ShowString(56, 2, value, sizeof(value));
#endif
#if DEVICE_BLE_SEND_ENABLE
	BLE_SendStruct.length = 3;
	Double2Format(HandBrake_force, BLE_SendStruct.pack.doubleData);
	BLE_SendBytes(BLE_DATA_TYPE_HAND_BRAKE_FORCE);
#endif

	HAL_Delay(200);
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
uint32_t HX711_ReadValue(void)
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
