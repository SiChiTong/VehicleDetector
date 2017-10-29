#include "../Inc/ble.h"

/******************************************************************************/
uint8_t BLE_RecvBytes[BLE_UART_RX_BYTE_MAX];
BLE_RecvTypedef BLE_Recv;
uint8_t BLE_sendBuffer[BLE_UART_RX_BYTE_MAX];

/*******************************************************************************
 *
 */
void BLE_Init(void)
{
	UART_DMAIdleConfig(&BLE_UART, BLE_RecvBytes, BLE_UART_RX_BYTE_MAX);
}

/*******************************************************************************
 *
 */
void BLE_UartIdleDeal(void)
{
	uint32_t tmp_flag = 0, tmp_it_source = 0;

	tmp_flag      = __HAL_UART_GET_FLAG(&BLE_UART, UART_FLAG_IDLE);
	tmp_it_source = __HAL_UART_GET_IT_SOURCE(&BLE_UART, UART_IT_IDLE);
	if((tmp_flag != RESET) && (tmp_it_source != RESET))
	{
		__HAL_DMA_DISABLE(BLE_UART.hdmarx);
		__HAL_DMA_CLEAR_FLAG(BLE_UART.hdmarx, BLE_UART_DMA_RX_GL_FLAG);

		/* Clear Uart IDLE Flag */
		__HAL_UART_CLEAR_IDLEFLAG(&BLE_UART);

		BLE_Recv.size = BLE_UART_RX_BYTE_MAX
						- __HAL_DMA_GET_COUNTER(BLE_UART.hdmarx);

		memcpy(&BLE_Recv.buffer, BLE_RecvBytes, BLE_Recv.size);
		BLE_Recv.status = ENABLE;

		memset(BLE_RecvBytes, 0, BLE_Recv.size);

		BLE_UART.hdmarx->Instance->CNDTR = BLE_UART.RxXferSize;
		__HAL_DMA_ENABLE(BLE_UART.hdmarx);
	}
}

/*******************************************************************************
 *
 */
void BLE_SendBytes(uint8_t size)
{
	HAL_UART_Transmit_DMA(&BLE_UART, BLE_sendBuffer, size);
}

/*******************************************************************************
 * @brief �������ڽ��յ�����Ϣ����
 */
void BLE_Process(void)
{
	if (BLE_Recv.status == ENABLE)
	{
		BLE_Recv.status = DISABLE;

#if DEVICE_TEST_MODE_ENABLE
		/* ����ǲ���ģʽ���жϵ�һ���ֽ� */
		switch (BLE_Recv.buffer[0])
#else
		/* ����Э���ж� */
		/* todo */
#endif
		{
		/* ��������λУ�� */
		case BLE_CMD_TYPE_SENSOR_VALUE_SET_ZERO:
			break;

		/* ������������� */
		case BLE_CMD_TYPE_CLEAR_SENSOR_CACHE:
			break;

		/* �Ͽ����� */
		case BLE_CMD_TYPE_SWITCH_OFF:
			break;

		/* ֹͣ��� */
		case BLE_CMD_TYPE_STOP_DETECTED:
			PROCESS_Mode = PROCESS_MODE_INVALID;
			/* ȡ�����лش���Ϣ */
			ACCELERATE_SetBackInfo(0x00, 0x00);
			break;

		/* ����������ת������� */
		case BLE_CMD_TYPE_DETECTED_STEERING_WHEEL_FORCE:
			PROCESS_Mode = PROCESS_MODE_DETECTED_STEERING_WHEEL_FORCE;
			break;

		/* ����������ת�Ǽ�� */
		case BLE_CMD_TYPE_DETECTED_STEERING_WHEEL_ANGLE:
			PROCESS_Mode = PROCESS_MODE_DETECTED_STEERING_WHEEL_ANGLE;
			/* ���ýǶ���Ϣ�ش� */
			ACCELERATE_SetBackInfo(ACCELERATE_TYPE_ANGLE_MARK, 0x00);
			break;

		/* �����ƶ������� */
		case BLE_CMD_TYPE_DETECTED_BRAKING_DISTANCE:
			PROCESS_Mode = PROCESS_MODE_DETECTED_BRAKING_DISTANCE;
			/* ���ü��ٶ���Ϣ�ش� */
			ACCELERATE_SetBackInfo(ACCELERATE_TYPE_ACCELERATE_SPEED_MARK, 0x00);
			break;

		/* �����ƶ�̤������� */
		case BLE_CMD_TYPE_DETECTED_PEDAL_FORCE:
			PROCESS_Mode = PROCESS_MODE_DETECTED_PEDAL_FORCE;
			break;

		/* ������ɲ�ƶ������ */
		case BLE_CMD_TYPE_DETECTED_HAND_BRAKE_FORCE:
			PROCESS_Mode = PROCESS_MODE_DETECTED_HAND_BRAKE_FORCE;
			break;

		/* �������ȼ�� */
		case BLE_CMD_TYPE_DETECTED_NOISE:
			PROCESS_Mode = PROCESS_MODE_DETECTED_NOISE;
			break;

		/* �����໬����� */
		case BLE_CMD_TYPE_DETECTED_SIDESLIP_DISTANCE:
			PROCESS_Mode = PROCESS_MODE_DETECTED_SIDESLIP_DISTANCE;
			break;

		/* ���������½��ٶȼ�� */
		case BLE_CMD_TYPE_DETECTED_DOWN_VELOCITY:
			PROCESS_Mode = PROCESS_MODE_DETECTED_DOWN_VELOCITY;
			break;

		/* ������ص������ */
		case BLE_CMD_TYPE_DETECTED_BAT_VOLTAGE:
			ANALOG_ConvertEnable();
			PROCESS_Mode = PROCESS_MODE_DETECTED_BAT_VOLTAGE;
			break;

		default:
			break;
		}
	}
}

/*******************************************************************************
 * @brief �������������ʹ���
 */
void BLE_SonsorSetZeroProcess(uint8_t subType)
{
	switch (subType)
	{
	/* ת�������� */
	case BLE_CMD_SUBTYPE_SET_ZORO_STEERING_WHEEL_FORCE:
		break;

	/* ת������ */
	case BLE_CMD_SUBTYPE_SET_ZORO_STEERING_WHEEL_ANGLE:
		break;

	/* �ƶ��������� */
	case BLE_CMD_SUBTYPE_SET_ZORO_BRAKING_DISTANCE:
		break;

	/* ̤�������� */
	case BLE_CMD_SUBTYPE_SET_ZORO_PEDAL_FORCE:
		break;

	/* ��ɲ������ */
	case BLE_CMD_SUBTYPE_SET_ZORO_HAND_BRAKE_FORCE:
		break;

	/* �������� */
	case BLE_CMD_SUBTYPE_SET_ZORO_NOISE:
		break;

	 /* �໬������ */
	case BLE_CMD_SUBTYPE_SET_ZORO_SIDESLIP_FORCE:
		break;

	 /* �½��ٶ����� */
	case BLE_CMD_SUBTYPE_SET_ZORO_DOWN_VELOCITY:
		break;

	default:
		break;
	}
}
