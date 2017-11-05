#include "accelerate.h"
#include "ble.h"
#include "oled.h"

/******************************************************************************/
uint8_t ACCELERATE_RecvBytes[ACCELERATE_UART_RX_BYTE_MAX];
ACCELERATE_RecvTypedef       ACCELERATE_Recv;
ACCELERATE_SendStrcutTypedef ACCELERATE_SendStrcut;

extern ItemValueTypedef     ItemValue;
extern ItemZeroValueTypedef ItemZeroValue;
extern ItemValueSetZeroEnableTypedef ItemValueSetZeroEnable;

/******************************************************************************/
static void AccelerateSpeedProcess(ACCELERATE_RecvStrcutTypedef* buffer);
static void AccelerateAngleProcess(ACCELERATE_RecvStrcutTypedef* buffer);

/*******************************************************************************
 * @brief ���ٶ�ģ���ʼ��
 */
void ACCELERATE_Init(void)
{
	UART_DMAIdleConfig(&ACCELERATE_UART, ACCELERATE_RecvBytes, ACCELERATE_UART_RX_BYTE_MAX);
	ACCELERATE_SendStrcut.head1 = 0xFF;
	ACCELERATE_SendStrcut.head2 = 0xAA;
}

/*******************************************************************************
 * @brief ���ռ��ٶȴ��������ص���Ϣ
 *
 * 			���ٶȴ��������ݴ���2�������
 * 			1.���ô������ش���ϢΪ100Hz���ܻ�ȡ�����µ���ֵ�����ǻ���ʱ��10ms�޷�����
 * 			2.��ʱ��10ms�жϣ��ܻ�ȡ׼ȷ��10ms����ʱ�䣬������ֵ���ܻ�����һ�η��ص�ֵ
 * 			����������
 */
void ACCELERATE_Process(void)
{
	if (ACCELERATE_Recv.status == ENABLE)
	{
		ACCELERATE_Recv.status = DISABLE;

		/* �ж�֡ͷ */
		if (ACCELERATE_Recv.buffer.head != ACCELERATE_PROTOCOL_HEAD)
			return;

		/* У��� */
//		if (ACCELERATE_Recv.buffer.sum != CheckSum((uint8_t*)&ACCELERATE_Recv.buffer))
//			return;

		/* �ж��������� */
		switch (ACCELERATE_Recv.buffer.type)
		{
		/* ���ٶ���� */
		case ACCELERATE_TYPE_ACCELERATE_SPEED:
			AccelerateSpeedProcess(&ACCELERATE_Recv.buffer);
			break;

		/* �Ƕ���� */
		case ACCELERATE_TYPE_ANGLE:
			AccelerateAngleProcess(&ACCELERATE_Recv.buffer);
			break;

		default:
			break;
		}
	}
}

/*******************************************************************************
 * @breif ���ûش�����
 * @param RSWL������Ϣ��
			ACCELERATE_TYPE_DATE_MARK
			ACCELERATE_TYPE_ACCELERATE_SPEED_MARK
			ACCELERATE_TYPE_ANGULAR_SPEED_MARK
			ACCELERATE_TYPE_ANGLE_MARK
			ACCELERATE_TYPE_MAGNETIC_FIELD_MARK
			ACCELERATE_TYPE_PORT_STATUS_MARK
			ACCELERATE_TYPE_AIR_PRESS_AND_ALTITUDE_MARK
			ACCELERATE_TYPE_LONGITUDE_AND_LATITUDE_MARK
		  RSWH������Ϣ��
			ACCELERATE_TYPE_GROUND_SPEED_MARK
			ACCELERATE_TYPE_FOUR_ELEMENTS_MARK
			ACCELERATE_TYPE_LOCATION_MARK
 */
void ACCELERATE_SetBackInfo(uint8_t RSWL, uint8_t RSWH)
{
	ACCELERATE_SendStrcut.address = ACCELERATE_ADDR_RSW;
	ACCELERATE_SendStrcut.dataL   = RSWL;
	ACCELERATE_SendStrcut.dataH   = RSWH;
	HAL_UART_Transmit_DMA(&ACCELERATE_UART, (uint8_t*)&ACCELERATE_SendStrcut,
			sizeof(ACCELERATE_SendStrcutTypedef));
}

/*******************************************************************************
 *
 */
void ACCELERATE_UartIdleDeal(void)
{
	uint32_t tmp_flag = 0, tmp_it_source = 0;

	tmp_flag      = __HAL_UART_GET_FLAG(&ACCELERATE_UART, UART_FLAG_IDLE);
	tmp_it_source = __HAL_UART_GET_IT_SOURCE(&ACCELERATE_UART, UART_IT_IDLE);
	if((tmp_flag != RESET) && (tmp_it_source != RESET))
	{
		__HAL_DMA_DISABLE(ACCELERATE_UART.hdmarx);
		__HAL_DMA_CLEAR_FLAG(ACCELERATE_UART.hdmarx, ACCELERATE_UART_DMA_RX_GL_FLAG);

		/* Clear Uart IDLE Flag */
		__HAL_UART_CLEAR_IDLEFLAG(&ACCELERATE_UART);

		ACCELERATE_Recv.size = ACCELERATE_UART_RX_BYTE_MAX
						- __HAL_DMA_GET_COUNTER(ACCELERATE_UART.hdmarx);

		if (ACCELERATE_Recv.size >= sizeof(ACCELERATE_RecvStrcutTypedef))
		{
			memcpy(&ACCELERATE_Recv.buffer, ACCELERATE_RecvBytes, ACCELERATE_Recv.size);
			ACCELERATE_Recv.status = ENABLE;
		}
		memset(ACCELERATE_RecvBytes, 0, ACCELERATE_Recv.size);

		ACCELERATE_UART.hdmarx->Instance->CNDTR = ACCELERATE_UART.RxXferSize;
		__HAL_DMA_ENABLE(ACCELERATE_UART.hdmarx);
	}
}

/*******************************************************************************
 * @brief ���ݼ��㷽����ȡ���ٶ�ֵ
 */
static double GetAccelerateSpeed(int16_t data)
{
	double accelerate = 0.0;

	accelerate = ((data / ACCELERATE_DIGITAL_RANGE) * ACCELERATE_RANGE_ACCELERATE);
	return accelerate;
}

/*******************************************************************************
 * @brief ���ݼ��㷽ʽ��ȡ�Ƕ�ֵ
 */
static float GetAngleValue(int16_t data)
{
	float angle = 0.0;

	angle = (float)((data / ACCELERATE_DIGITAL_RANGE) * ACCELERATE_RANGE_ANGLE);
	return angle;
}

/*******************************************************************************
 * @brief ���ٶȴ�����ͨ�����ַ�ʽ�����ٶȺ�λ��
 * 		  �ٶ� = ���ٶȶ�ʱ��Ļ���
 * 		 λ�� = �ٶȶ�ʱ��Ļ���
 * 		 �ƶ����룺���ٶ�Ϊ���������˶����̣�һ�����ּ��ٶ�Ϊ�������ƶ��������
 *
 * 		 ���Է��֣���ֹ״̬�£����ٶ�ֵ�С�0.5�����ƫ�ƣ����Ծ���ֵ<0.5Ĭ��Ϊ��ֹ��������
 */
//double Ax = 0.0;
//double Ay = 0.0;
//double Az = 0.0;
//double Axy = 0.0;
double Speed = 0.0;

static void AccelerateSpeedProcess(ACCELERATE_RecvStrcutTypedef* buffer)
{
//	double Ax = 0.0;
	char value[7];

	/* x����ٶ���data1λ�� */
	ItemValue.Ax = GetAccelerateSpeed(buffer->data1);
//	Ay = GetAccelerateSpeed(buffer->data2);
//	Az = GetAccelerateSpeed(buffer->data3);
//	Axy = sqrt(Ax * Ax + Ay * Ay + Az * Az);
//	Axy = sqrt(Ax * Ax + Ay * Ay);

	if (ItemValueSetZeroEnable.brakeAx == ENABLE)
	{
		ItemValueSetZeroEnable.brakeAx = DISABLE;
		ItemZeroValue.Ax = ItemValue.Ax;
		ItemValue.brakeVelocity = 0;
		ItemValue.brakeVelocityInit = 0;
		ItemValue.brakeDistance = 0;
	}

	/* ���ٶ����У׼ */
	ItemValue.Ax -= ItemZeroValue.Ax;

	if (fabs(ItemValue.Ax) > 0.05)
	{
		ItemValue.brakeVelocity += ItemValue.Ax * ACCELERATE_INTEGRAL_TIME;

		Speed = ItemValue.brakeVelocity * 3.6;

		if (ItemValue.Ax < 0)
		{
			ItemValue.brakeVelocityInit = ItemValue.brakeVelocity;
			ItemValue.brakeDistance += ItemValue.brakeVelocity
					* ACCELERATE_INTEGRAL_TIME;
		}
		else
		{
			ItemValue.brakeVelocityInit = 0;
			ItemValue.brakeDistance     = 0;
		}
	}
	else
	{
		ItemValue.brakeVelocity = 0;
		ItemValue.brakeVelocityInit  = 0;
		ItemValue.brakeDistance = 0;
		Speed = 0;
	}

	/* ��ʾʵʱ�ٶ� */
	sprintf(value, "%6.1f", Speed);
#if DEVICE_OLED_DISPLAY_ENABLE
	OLED_ShowString(64, 2, value, 6);
#endif
#if DEVICE_BLE_SEND_ENABLE
	BLE_SendBytes(BLE_DATA_TYPE_BRAKING_INITIAL_VELOCITY, value);
#endif

	/* ��ʾʵʱ�ٶ� */
	sprintf(value, "%6.1f", ItemValue.brakeDistance);
#if DEVICE_OLED_DISPLAY_ENABLE
	OLED_ShowString(64, 4, value, 6);
#endif
#if DEVICE_BLE_SEND_ENABLE
	BLE_SendBytes(BLE_DATA_TYPE_BRAKING_DISTANCE, value);
#endif
}

/*******************************************************************************
 * @brief ������ת�Ǽ��
 * 		  �������̵�ת����ͬһƽ���ڣ�ת����������Roll��x������ǣ���Pitch��y�ḩ���ǣ��ı仯����Yaw
 * 		      ������ǣ���Ϊ�����Ƕȵ�Ψһ������
 */
static void AccelerateAngleProcess(ACCELERATE_RecvStrcutTypedef* buffer)
{
	char value[7];

	/* ��ǰ�ĽǶ�ֵ - ��λֵ */
	ItemValue.steeringWheelAngle = GetAngleValue(buffer->data3);

	if (ItemValueSetZeroEnable.steeringWheelAngle == ENABLE)
	{
		ItemValueSetZeroEnable.steeringWheelAngle = DISABLE;
		ItemZeroValue.steeringWheelAngle = ItemValue.steeringWheelAngle;
	}

	ItemValue.steeringWheelAngle -= ItemZeroValue.steeringWheelAngle;

	sprintf(value, "%6.1f", ItemValue.steeringWheelAngle);
#if DEVICE_OLED_DISPLAY_ENABLE
	OLED_ShowString(64, 2, value, 6);
#endif
#if DEVICE_BLE_SEND_ENABLE
	BLE_SendBytes(BLE_DATA_TYPE_STEERING_WHEEL_ANGLE, value);
#endif
}

