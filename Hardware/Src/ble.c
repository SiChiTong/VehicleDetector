#include "../Inc/ble.h"
#include "oled.h"
#include "process.h"
#include "tim.h"
/******************************************************************************/
uint8_t BLE_RecvBytes[BLE_UART_RX_BYTE_MAX];
BLE_RecvTypedef BLE_Recv;
uint8_t BLE_sendBuffer[BLE_UART_RX_BYTE_MAX];
BLE_SendStructTypedef BLE_SendStruct;

extern const char ChineseFont_SteeringWheelForce[CHINESE_FONT_SIZE * 6];
extern const char ChineseFont_CurrentValue[CHINESE_FONT_SIZE * 3];
extern const char ChineseFont_ValueMax[CHINESE_FONT_SIZE * 3];
extern const char ChineseFont_ValueMin[CHINESE_FONT_SIZE * 3];
extern const char ChineseFont_SteeringWheelAngle[CHINESE_FONT_SIZE * 5];
extern const char ChineseFont_BrakingDistance[CHINESE_FONT_SIZE * 4];
extern const char ChineseFont_DownVelocityTitle[CHINESE_FONT_SIZE * 8];
extern const char ChineseFont_DownVelocityDistance[CHINESE_FONT_SIZE * 4];
extern const char ChineseFont_DownVelocitySpeed[CHINESE_FONT_SIZE * 4];

/* �ƶ������� */
extern double BrakeDistance_pedalForce;					/* ̤���� */
extern double BrakeDistance_speed;						/* �ƶ����-�ٶ� */
extern double BrakeDistance_oldSpeed;					/* �ƶ����-���ٶ� */
extern double BrakeDistance_initSpeed;					/* ��ʼ�ٶ� */
extern double BrakeDistance_distance;					/* �ƶ����-���� */

extern uint16_t Encode_plusCnt;						/* ������������ */
extern uint16_t Encode_plusCntOld;					/* �������������� */
extern uint16_t Encode_periodCntTotal;				/* �������� */
extern uint16_t Encode_periodCnt;					/* ������ */
extern FunctionalState Encode_errorStatus;				/* ����״̬ */

/* �����½��ٶȼ�� */
extern uint16_t DownVelocity_Distance;
extern uint16_t DownVelocity_DistanceOld;
extern double   DownVelocity_Speed;

/* ������ */
extern double SteeringWheel_Force;
extern double SteeringWheel_Angle;

/* ��ɲ�� */
extern double HandBrake_force;											/* ��ɲ�� */

/* ������� */
extern float  Noisy_Value;

/* �¶ȼ�� */
extern float  Gradient_Value;

/* ��ص��� */
extern uint8_t BatteryVoltage_Value;

/* �໬����� */
extern double   SideSlip_distance;
extern uint32_t SideSlip_plusCnt;
extern double   SideSlip_angle;


/*******************************************************************************
 * @brief ������ʼ��
 */
void BLE_Init(void)
{
	/* �������ͽṹ��̶�ֵ */
	BLE_SendStruct.head = BLE_PROTOCOL_HEAD;
	BLE_SendStruct.length = 6;
	BLE_SendStruct.tail = BLE_PROTOCOL_TAIL;
	/* ��������DMA���ճ�ʼ�� */
	UART_DMAIdleConfig(&BLE_UART, BLE_RecvBytes, BLE_UART_RX_BYTE_MAX);
}

/*******************************************************************************
 * @brief ��������DMA���ճ�ʼ��
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
 * @brief ��������͸��
 */
void BLE_SendBytes(BLE_CmdDataTypeEnum type)
{
	/* �������ͽṹ��̶�ֵ */
	BLE_SendStruct.head = BLE_PROTOCOL_HEAD;
	BLE_SendStruct.tail = BLE_PROTOCOL_TAIL;

	/* �������� */
	BLE_SendStruct.type = type;

	/* ����У���,У�鳤��Ϊ�������� + ���ݳ��� + ����  */
	BLE_SendStruct.verify = CheckSum(&BLE_SendStruct.type, BLE_SendStruct.length + 2);
	memcpy(&BLE_SendStruct.pack.buffer[BLE_SendStruct.length], &BLE_SendStruct.verify, 2);

	/* ���ڷ��� */
	HAL_UART_Transmit_DMA(&BLE_UART, (uint8_t*)&BLE_SendStruct,
			BLE_SendStruct.length + 5);
}

/*******************************************************************************
 * @brief �������ڽ��յ�����Ϣ����
 */
void BLE_Process(void)
{
	if (BLE_Recv.status == ENABLE)
	{
		BLE_Recv.status = DISABLE;

		/* ����Э���ж� */
		if ((BLE_Recv.buffer.head != BLE_PROTOCOL_HEAD)
				|| (BLE_Recv.buffer.tail != BLE_PROTOCOL_TAIL))
			return;
		if (CheckSum(&BLE_Recv.buffer.cmdType, 2) != BLE_Recv.buffer.verify)
			return;
		/* �ж��������� */
		switch (BLE_Recv.buffer.cmdType)
		{
		/* ��������λУ�� */
		case BLE_CMD_TYPE_SENSOR_VALUE_SET_ZERO:
			ZeroCalibration();
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

		/* ����������ת�Ǽ�� */
		case BLE_CMD_TYPE_DETECTED_STEERING_WHEEL_FORCE_AND_ANGLE:
			PROCESS_Mode = PROCESS_MODE_DETECTED_STEERING_WHEEL_FORCE_AND_ANGLE;
			/* ���ýǶ���Ϣ�ش� */
			ACCELERATE_SetBackInfo(ACCELERATE_TYPE_ANGLE_MARK, 0x00);
//			OLED_Clear();
			SteeringWheel_Force = 0;
			SteeringWheel_Angle = 0;
			break;

		/* �����ƶ������� */
		case BLE_CMD_TYPE_DETECTED_PEDAL_FORCE_AND_BRAKING_DISTANCE:
			PROCESS_Mode = PROCESS_MODE_DETECTED_PEDAL_FORCE_BRAKING_DISTANCE;
			LL_TIM_SetCounter(TIM3, 0);
			LL_TIM_EnableCounter(TIM3);
			/* ������ʱ�� */
			LL_TIM_SetAutoReload(TIM7, 499);
			LL_TIM_SetCounter(TIM7, 0);
			LL_TIM_EnableCounter(TIM7);

//			OLED_Clear();
//			OLED_ShowChineseString(0, 0, (char*)ChineseFont_BrakingDistance,
//					sizeof(ChineseFont_BrakingDistance) / CHINESE_FONT_SIZE);
			BrakeDistance_pedalForce = 0;					/* ̤���� */
			BrakeDistance_speed      = 0;					/* �ƶ����-�ٶ� */
			BrakeDistance_oldSpeed   = 0;					/* �ƶ����-���ٶ� */
			BrakeDistance_initSpeed  = 0;					/* ��ʼ�ٶ� */
			BrakeDistance_distance   = 0;					/* �ƶ����-���� */

			Encode_plusCnt = 0;								/* ������������ */
			Encode_plusCntOld = 0;							/* �������������� */
			Encode_periodCntTotal = 0;						/* �������� */
			Encode_periodCnt = 0;							/* ������ */
			Encode_errorStatus = DISABLE;
			break;

		/* ������ɲ�ƶ������ */
		case BLE_CMD_TYPE_DETECTED_HAND_BRAKE_FORCE:
			PROCESS_Mode = PROCESS_MODE_DETECTED_HAND_BRAKE_FORCE;
			OLED_Clear();
			OLED_ShowString(0, 2, "value = ", 8);
			OLED_ShowString(104, 2, "N", 1);
			HandBrake_force = 0;
			break;

		/* �������ȼ�� */
		case BLE_CMD_TYPE_DETECTED_NOISE:
			PROCESS_Mode = PROCESS_MODE_DETECTED_NOISE;
			LL_TIM_SetAutoReload(TIM7, 499);
			/* ������ʱ�� */
			LL_TIM_SetCounter(TIM7, 0);
			LL_TIM_EnableCounter(TIM7);
			OLED_Clear();
			OLED_ShowString(0, 2, "value = ", 8);
			OLED_ShowString(104, 2, "dB", 2);
			Noisy_Value = 0;
			break;

		/* �����໬����� */
		case BLE_CMD_TYPE_DETECTED_SIDESLIP_DISTANCE:
			/* �򿪼��ٶȺͽǶȼ�� */
			ACCELERATE_SetBackInfo(ACCELERATE_TYPE_ANGLE_MARK, 0x00);
			PROCESS_Mode = PROCESS_MODE_DETECTED_SIDESLIP_DISTANCE;
			LL_TIM_SetCounter(TIM3, 0);
			LL_TIM_EnableCounter(TIM3);
			SideSlip_plusCnt  = 0;
			SideSlip_angle    = 0;
			SideSlip_distance = 0;
			OLED_Clear();
			break;

		/* ���������½��ٶȼ�� */
		case BLE_CMD_TYPE_DETECTED_DOWN_VELOCITY:
			PROCESS_Mode = PROCESS_MODE_DETECTED_DOWN_VELOCITY;
			LL_TIM_SetAutoReload(TIM7, 999);
			LL_TIM_SetCounter(TIM7, 0);
			LL_TIM_EnableCounter(TIM7);

			OLED_Clear();
			OLED_ShowChineseString(0, 0, (char*)ChineseFont_DownVelocityTitle,
						sizeof(ChineseFont_DownVelocityTitle) / CHINESE_FONT_SIZE);
			OLED_ShowChineseString(0, 2, (char*)ChineseFont_DownVelocityDistance,
						sizeof(ChineseFont_DownVelocityDistance) / CHINESE_FONT_SIZE);
			OLED_ShowChineseString(0, 4, (char*)ChineseFont_DownVelocitySpeed,
						sizeof(ChineseFont_DownVelocitySpeed) / CHINESE_FONT_SIZE);
			DownVelocity_Distance    = 0;
			DownVelocity_DistanceOld = 0;
			DownVelocity_Speed       = 0;
			break;

		/* �¶ȼ�� */
		case BLE_CMD_TYPE_DETECTED_GRADIENT:
			/* ���ýǶ���Ϣ�ش� */
			ACCELERATE_SetBackInfo(ACCELERATE_TYPE_ANGLE_MARK, 0x00);
			PROCESS_Mode = PROCESS_MODE_DETECTED_GRADIENT;
			OLED_Clear();
			Gradient_Value = 0;
			break;

		/* ������ص������ */
		case BLE_CMD_TYPE_DETECTED_BATTERY_CAPACITY:
			ANALOG_ConvertEnable();
			PROCESS_Mode = PROCESS_MODE_DETECTED_BATTERY_CAPACITY;
			OLED_Clear();
			break;

		default:
			break;
		}
	}
}

