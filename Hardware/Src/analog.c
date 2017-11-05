#include "analog.h"
#include "ble.h"
#include "oled.h"


/******************************************************************************/
static uint16_t convertValueBuffer[ANALOG_SAMPLE_NUMB];
static FunctionalState convertFinished = DISABLE;

extern ItemValueTypedef     ItemValue;

/******************************************************************************/
static uint16_t GetAverageValue(void);
static uint8_t GetBatVoltage(uint16_t value);

/*******************************************************************************
 * ����ADCת��
 */
void ANALOG_ConvertEnable(void)
{
	HAL_ADC_Start_DMA(&ANALOG_ADC, (uint32_t*)convertValueBuffer,
								sizeof(convertValueBuffer));
}

/*******************************************************************************
 * ֹͣADCת��
 */
void ANALOG_ConvertDisable(void)
{
	HAL_ADC_Stop_DMA(&ANALOG_ADC);
	convertFinished = ENABLE;
}

/*******************************************************************************
 * @brief ģ����ת������
 */
void ANALOG_Process(void)
{
	uint16_t data;
	char value[7];
	/* �ȴ�ת������ */
	if (ENABLE == convertFinished)
	{
		convertFinished = DISABLE;
		/* ��ȡģ������ֵ */
		data = GetAverageValue();
		/* ��ȡ��ص���ֵ */
		ItemValue.batteryCapacity = GetBatVoltage(data);

		sprintf(value, "%6d", ItemValue.batteryCapacity);
#if DEVICE_OLED_DISPLAY_ENABLE
		OLED_ShowString(64, 2, value, 6);
#endif
#if DEVICE_BLE_SEND_ENABLE
		BLE_SendBytes(BLE_DATA_TYPE_BATTERY_CAPACITY, value);
#endif

	}
}

/*******************************************************************************
 * @brief ð�ݷ������˲��㷨
 * 		�㷨������20��ֵ����20��ֵ�Ӵ�С����ȥ�м��10��ֵȡƽ��
 */
static uint16_t GetAverageValue(void)
{
	uint8_t  j, k;
	uint16_t value;
	uint32_t average;

	/* ð�ݷ����� */
	for (j = RESET; j < ANALOG_SAMPLE_NUMB - 1; j++)
	{
		for (k = j + 1; k < ANALOG_SAMPLE_NUMB; k++)
		{
			if (convertValueBuffer[k] > convertValueBuffer[j])
			{
				value = convertValueBuffer[j];
				convertValueBuffer[j] = convertValueBuffer[k];
				convertValueBuffer[k] = value;
			}
		}
	}

	/* ���ƽ��ֵ */
	average = RESET;

	/* ȡ�м����ֵ��ƽ���� */
	for(j = ANALOG_SAMPLE_NUMB / 2 - 5; j < ANALOG_SAMPLE_NUMB / 2 + 5; j++)
	{
		average += convertValueBuffer[j];
	}
	return (uint16_t)(average / 10);
}

/*******************************************************************************
 * @brief ��ȡ﮵�ص�ѹֵ
 * 		  ﮵�ص�ѹ��ΧΪ3.2V~4.2V�����3.7V��������Ϊ4.2V
 * 		  ����ѹ����3.4V���£����ܵ���3.3V��ѹоƬ�޷���������ϵͳ����
 */
static uint8_t GetBatVoltage(uint16_t value)
{
	uint32_t voltage;
	float percent;

	/* ����2������Ϊ4.2V�����ѹ */
	voltage = (((uint32_t)value * 3300) * 2 / 4096);

	percent = (float)voltage / 4200;

	if (percent > 1)
		percent = 1;

	return (uint8_t)(percent * 100);
}
