#include "accelerate.h"

/******************************************************************************/
uint8_t ACCELERATE_RecvBytes[ACCELERATE_UART_RX_BYTE_MAX];
ACCELERATE_RecvTypedef       ACCELERATE_Recv;
ACCELERATE_SendStrcutTypedef ACCELERATE_SendStrcut;
ACCELERATE_ParamTypedef      ACCELERATE_Param;

/******************************************************************************/
static uint8_t CheckSum(uint8_t* buffer);
static void AccelerateSpeedProcess(ACCELERATE_RecvStrcutTypedef* buffer);
static void AccelerateAngleProcess(ACCELERATE_RecvStrcutTypedef* buffer);
static void ACCELERATE_SendCmd(ACCELERATE_AddrEnum addr, uint8_t dataL, uint8_t dataH);

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
		if (ACCELERATE_Recv.buffer.sum != CheckSum((uint8_t*)&ACCELERATE_Recv.buffer))
			return;

		/* �ж��������� */
		switch (ACCELERATE_Recv.buffer.type)
		{
		/* ʱ����� */
		case ACCELERATE_TYPE_DATE:
			break;

		/* ���ٶ���� */
		case ACCELERATE_TYPE_ACCELERATE_SPEED:
			AccelerateSpeedProcess(&ACCELERATE_Recv.buffer);
			break;

		/* ���ٶ���� */
		case ACCELERATE_TYPE_ANGULAR_SPEED:
			break;

		/* �Ƕ���� */
		case ACCELERATE_TYPE_ANGLE:
			AccelerateAngleProcess(&ACCELERATE_Recv.buffer);
			break;

		/* �ų���� */
		case ACCELERATE_TYPE_MAGNETIC_FIELD:
			break;

		/* �˿�״̬������� */
		case ACCELERATE_TYPE_PORT_STATUS:
			break;

		/* ��ѹ���߶���� */
		case ACCELERATE_TYPE_AIR_PRESS_AND_ALTITUDE:
			break;

		/* ��γ����� */
		case ACCELERATE_TYPE_LONGITUDE_AND_LATITUDE:
			break;

		/* ������� */
		case ACCELERATE_TYPE_GROUND_SPEED:
			break;

		/* ��Ԫ����� */
		case ACCELERATE_TYPE_FOUR_ELEMENTS:
			break;

		/* ���Ƕ�λ������� */
		case ACCELERATE_TYPE_LOCATION:
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
	ACCELERATE_SendCmd(ACCELERATE_ADDR_RSW, RSWL, RSWH);
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
 *
 */
static void ACCELERATE_SendCmd(ACCELERATE_AddrEnum addr, uint8_t dataL, uint8_t dataH)
{
	ACCELERATE_SendStrcut.address = addr;
	ACCELERATE_SendStrcut.dataL   = dataL;
	ACCELERATE_SendStrcut.dataH   = dataH;
	HAL_UART_Transmit_DMA(&ACCELERATE_UART, (uint8_t*)&ACCELERATE_SendStrcut,
			sizeof(ACCELERATE_SendStrcutTypedef));
}

/*******************************************************************************
 * @brief ����У���
 */
static uint8_t CheckSum(uint8_t* buffer)
{
	uint8_t index;
	uint8_t sum = 0;

	for (index = 0; index < (sizeof(ACCELERATE_RecvStrcutTypedef) - 1); index++)
	{
		sum += *buffer;
		buffer++;
	}

	return sum;
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
static void AccelerateSpeedProcess(ACCELERATE_RecvStrcutTypedef* buffer)
{
	double Ax = 0.0;

	/* x����ٶ���data1λ�� */
	Ax = GetAccelerateSpeed(buffer->data1);

	if (fabs(Ax) > 0.5)
	{
		ACCELERATE_Param.velocity += Ax * ACCELERATE_INTEGRAL_TIME;
		ACCELERATE_Param.distance += ACCELERATE_Param.velocity
				* ACCELERATE_INTEGRAL_TIME;

		if (Ax < 0)
		{
			ACCELERATE_Param.brakeDistance += ACCELERATE_Param.velocity
					* ACCELERATE_INTEGRAL_TIME;
		}
		else
		{
			ACCELERATE_Param.brakeDistance = 0;
		}
	}

#if DEVICE_TEST_MODE_ENABLE
	printf("x����ٶ�=%.1f���ٶ�=%.1f������=%.1f���ƶ�����=%.1f\r\n",
			Ax, ACCELERATE_Param.velocity,
			ACCELERATE_Param.distance, ACCELERATE_Param.brakeDistance);
#else

#endif
}

/*******************************************************************************
 * @brief ������ת�Ǽ��
 * 		  �������̵�ת����ͬһƽ���ڣ�ת����������Roll��x������ǣ���Pitch��y�ḩ���ǣ��ı仯����Yaw
 * 		      ������ǣ���Ϊ�����Ƕȵ�Ψһ������
 */
static void AccelerateAngleProcess(ACCELERATE_RecvStrcutTypedef* buffer)
{
	float angle;

	/* ��ǰ�ĽǶ�ֵ - ��λֵ */
	angle = GetAngleValue(buffer->data3) - ACCELERATE_Param.angleZero;

	printf("�����̽Ƕ�=%.1f\r\n", angle);
}

