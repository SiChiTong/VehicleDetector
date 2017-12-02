#include "accelerate.h"
#include "ble.h"
#include "oled.h"
#include "tim.h"

/******************************************************************************/
uint8_t ACCELERATE_RecvBytes[ACCELERATE_UART_RX_BYTE_MAX];
ACCELERATE_RecvTypedef       ACCELERATE_Recv;
ACCELERATE_SendStrcutTypedef ACCELERATE_SendStrcut;

extern ItemValueTypedef     ItemValue;
extern ItemZeroValueTypedef ItemZeroValue;
extern ItemValueSetZeroEnableTypedef ItemValueSetZeroEnable;
extern BLE_SendStructTypedef BLE_SendStruct;
extern uint16_t huaTimeBase;
/******************************************************************************/
static void AccelerateSpeedProcess(ACCELERATE_RecvStrcutTypedef* buffer);
static void AccelerateAngleProcess(ACCELERATE_RecvStrcutTypedef* buffer);
//static void AccelerateDownSpeedProcess(ACCELERATE_RecvStrcutTypedef* buffer);
static void AccelerateGradientProcess(ACCELERATE_RecvStrcutTypedef* buffer);
static void AccelerateSideSlipProcess(ACCELERATE_RecvTypedef* recv);

/*******************************************************************************
 * @brief ���ٶ�ģ���ʼ��
 */
void ACCELERATE_Init(void)
{
	/* ��ʼ�����ٶȴ���������DMA���� */
	UART_DMAIdleConfig(&ACCELERATE_UART, ACCELERATE_RecvBytes,
			ACCELERATE_UART_RX_BYTE_MAX);
	/* �趨���ٶȷ������ݵĹ̶����� */
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
		if (ACCELERATE_Recv.buffer[0].head != ACCELERATE_PROTOCOL_HEAD)
			return;

		/* У��� */
//		if (ACCELERATE_Recv.buffer.sum != CheckSum((uint8_t*)&ACCELERATE_Recv.buffer))
//			return;

		/* �ж��������� */
		switch (ACCELERATE_Recv.buffer[0].type)
		{
		/* ���ٶ���� */
		case ACCELERATE_TYPE_ACCELERATE_SPEED:
			switch (PROCESS_Mode)
			{
//			case PROCESS_MODE_DETECTED_BRAKING_DISTANCE:
//				AccelerateSpeedProcess(&ACCELERATE_Recv.buffer[0]);
//				break;

			case PROCESS_MODE_DETECTED_DOWN_VELOCITY:
//				AccelerateDownSpeedProcess(&ACCELERATE_Recv.buffer[0]);
				break;

			case PROCESS_MODE_DETECTED_SIDESLIP_DISTANCE:
				AccelerateSideSlipProcess(&ACCELERATE_Recv);
				break;

			default:
				break;
			}
			break;
			
			
		/* �Ƕ���� */
		case ACCELERATE_TYPE_ANGLE:
			switch (PROCESS_Mode)
			{
			/* ������ת�� */
			case PROCESS_MODE_DETECTED_STEERING_WHEEL_FORCE_AND_ANGLE:
				AccelerateAngleProcess(&ACCELERATE_Recv.buffer[0]);
				break;

			/* �¶ȼ�� */
			case PROCESS_MODE_DETECTED_GRADIENT:
				AccelerateGradientProcess(&ACCELERATE_Recv.buffer[0]);
				break;

//			case PROCESS_MODE_DETECTED_SIDESLIP_DISTANCE:
//				AccelerateSideSlipDistanceProcess(&ACCELERATE_Recv);
//				break;

			default:
				break;
			}
			break;

		default:
			break;
		}
	}
}

/*******************************************************************************
 * @breif ���ü��ٶȴ������ش�����
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
 * @brief ����DMA���ճ�ʼ��
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
static void AccelerateSpeedProcess(ACCELERATE_RecvStrcutTypedef* buffer)
{
	char value[7];

	/* x����ٶ���data1λ�� */
	ItemValue.Ax = GetAccelerateSpeed(buffer->data1);
	/* ���У׼ʹ�� */
	if (ItemValueSetZeroEnable.brakeAx == ENABLE)
	{
		  ItemValueSetZeroEnable.brakeAx = DISABLE;
		/* ����ǰֵ��ΪУ׼ֵ */
		ItemZeroValue.Ax = ItemValue.Ax;
		/* У׼�����֮ǰ�ۼӵ�ֵ */
		ItemValue.brakeVelocity = 0;
		ItemValue.brakeVelocityInit = 0;
		ItemValue.brakeDistance = 0;
	}

	/* ���ٶ����У׼ */
	ItemValue.Ax -= ItemZeroValue.Ax;

	/* ���ٶ�Ϊ��ֵ���ٶ��ۼ�0.1~1���������Ϊ��ֵ���ȥ����� */
//	if (ItemValue.Ax > 0)
//	{
//		/* �����ֵΪ��ǰ��ʱ��ֵ�����һλֵ/10 */
//		ItemValue.brakeVelocity += (float)(htim7.Instance->CNT % 10) / 10;
//	}
//	else
//	{
//		ItemValue.brakeVelocity -= (float)(htim7.Instance->CNT % 10) / 10;
//	}    
	
#if 1
	/* �����������Ư�ƣ�������ֵС��0.5��ֵ��ΪΪ��ֹ�������� */
	if (fabs(ItemValue.Ax) > 3)
	{
		/* ���ٶȻ��ֻ�ȡ�ٶ� */
		ItemValue.brakeVelocity += ItemValue.Ax*0.01;// * ACCELERATE_INTEGRAL_TIME;
		/* ǰ�������ٶȲ�����Ϊ��ֵ */
		if (ItemValue.brakeVelocity < 0)
		{
			ItemValue.brakeVelocity = 0;
		}
		/* �ٶȵ�λת����km/h */
		ItemValue.speed = ItemValue.brakeVelocity * 3.6;
		/* ���ٶ�Ϊ��ֵ���������ɲ������ʼ���ٶȻ��֣���λ�� */
		
		if (ItemValue.Ax < -3)
		{
			ItemValue.brakeDistance+=ItemValue.Ax*ItemValue.Ax/20000+ItemValue.brakeVelocity*0.01;
		}
		else	/* ������֤���ڼ��ٹ��̣����ۼ�λ�� */
		{
			ItemValue.brakeVelocityInit = 0;
			//ItemValue.brakeDistance     = 0;   hua
		}
	}
	else	/* Ĭ��Ϊ��ֹģʽ���ٶ�Ϊ0 */
	{
//		ItemValue.brakeVelocity = 0;					hua
//		ItemValue.brakeVelocityInit  = 0;
//		ItemValue.brakeDistance = 0;
//		ItemValue.speed = 0;									hua
	}
#endif

	/* ��ʾʵʱ�ٶ� */
//	sprintf(value, "%6.1f", Speed);
//#if DEVICE_OLED_DISPLAY_ENABLE
//	OLED_ShowString(64, 2, value, 6);
//#endif
//#if DEVICE_BLE_SEND_ENABLE
//	BLE_SendBytes(BLE_DATA_TYPE_BRAKING_INITIAL_VELOCITY, value);
//#endif

	/* ��ʾʵʱλ�� */

#if DEVICE_OLED_DISPLAY_ENABLE
	sprintf(value, "%6.1f", ItemValue.brakeDistance);
	OLED_ShowString(64, 4, value, 6);
#endif
#if DEVICE_BLE_SEND_ENABLE
//	BLE_SendBytes(BLE_DATA_TYPE_BRAKING_DISTANCE);
#endif
}

/*******************************************************************************
 * @brief �½��ٶȼ���
 */
//static void AccelerateDownSpeedProcess(ACCELERATE_RecvStrcutTypedef* buffer)
//{
//	char value[7];
//	/* x����ٶ���data1λ�� */
//	ItemValue.downAx = GetAccelerateSpeed(buffer->data1);
//	/* ���У׼ʹ�� */
//	if (ItemValueSetZeroEnable.downVelocity == ENABLE)
//	{
//		ItemValueSetZeroEnable.downVelocity = DISABLE;
//		/* ����ǰֵ��ΪУ׼ֵ */
//		ItemZeroValue.downAx = ItemValue.downAx;
//		/* У׼�����֮ǰ�ۼӵ�ֵ */
//		ItemValue.downVelocity = 0;
//	}

//	/* ���ٶ����У׼ */
//	ItemValue.downAx -= ItemZeroValue.downAx;

//  if(fabs(ItemValue.downAx)>3) {ItemValue.downVelocity+=ItemValue.downAx*0.01;huaTimeBase=0;}
//		else huaTimeBase++;
//	if(huaTimeBase>=30) {huaTimeBase=0;ItemValue.downVelocity=0;}
//	if(ItemValue.downVelocity<0) ItemValue.downVelocity=0;
//	/* ��ʾʵʱ�ٶ� */
//	sprintf(value, "%6.1f", ItemValue.downVelocity);
//#if DEVICE_OLED_DISPLAY_ENABLE
//	OLED_ShowString(64, 2, value, 6);
//#endif
//#if DEVICE_BLE_SEND_ENABLE
//	BLE_SendBytes(BLE_DATA_TYPE_DOWN_VELOCITY, value);
//#endif
//}

/*******************************************************************************
 * @brief ������ת�Ǽ��
 * 		  �������̵�ת����ͬһƽ���ڣ�ת����������Roll��x������ǣ���Pitch��y�ḩ���ǣ��ı仯����Yaw
 * 		      ������ǣ���Ϊ�����Ƕȵ�Ψһ������
 */
static void AccelerateAngleProcess(ACCELERATE_RecvStrcutTypedef* buffer)
{
	char value[7];

	/* ��ȡ��ǰ�ĽǶ�ֵ */
	ItemValue.steeringWheelAngle = GetAngleValue(buffer->data3);
	/* ���У׼ʹ�� */
	if (ItemValueSetZeroEnable.steeringWheelAngle == ENABLE)
	{
		ItemValueSetZeroEnable.steeringWheelAngle = DISABLE;
		ItemZeroValue.steeringWheelAngle = ItemValue.steeringWheelAngle;
	}
	/* ���У׼ */
	ItemValue.steeringWheelAngle -= ItemZeroValue.steeringWheelAngle;

//#if DEVICE_OLED_DISPLAY_ENABLE
//	sprintf(value, "%6.1f", ItemValue.steeringWheelAngle);
//	OLED_ShowString(64, 2, value, 6);
//#endif
//#if DEVICE_BLE_SEND_ENABLE
//	BLE_SendStruct.length = sizeof(ItemValue.steeringWheelAngle);
//	BLE_SendStruct.pack.data = ItemValue.steeringWheelAngle;
////	BLE_SendBytes(BLE_DATA_TYPE_STEERING_WHEEL_ANGLE);
//#endif
}

/*******************************************************************************
 * @brief �¶ȼ�⣬ֻ����ŷ�����еĸ�����
 */
static void AccelerateGradientProcess(ACCELERATE_RecvStrcutTypedef* buffer)
{
	char value[7];
	double angle = 0;

	/* �����Ƕ�Ӧdata2 */
	angle = GetAngleValue(buffer->data2);
	/* ���У׼ʹ�� */
	if (ItemValueSetZeroEnable.gradient == ENABLE)
	{
		ItemValueSetZeroEnable.gradient = DISABLE;
		ItemZeroValue.gradient = angle;
		ItemValue.gradient = 0;
	}

	/* �����ûУ׼��ֱ�������ǰ�¶� */
	if (ItemZeroValue.gradient == 0)
	{
		ItemValue.gradient = angle;
	}
	else /* ����Ѿ�У׼�������ƽ��ֵ */
	{
		/* ���У׼ */
		ItemValue.gradient = ((angle - ItemZeroValue.gradient) - ItemValue.gradient) / 2;
	}

	/* �����ʾ */
	sprintf(value, "%6.1f", ItemValue.gradient);
#if DEVICE_OLED_DISPLAY_ENABLE
	OLED_ShowString(64, 2, value, 6);
#endif
#if DEVICE_BLE_SEND_ENABLE
	BLE_SendStruct.length = sizeof(ItemValue.gradient);
	BLE_SendStruct.pack.data = ItemValue.gradient;
	BLE_SendBytes(BLE_DATA_TYPE_GRADIENT);
#endif
}

/*******************************************************************************
 * @breif �໬���Ƕ�
 */
static void AccelerateSideSlipProcess(ACCELERATE_RecvTypedef* recv)
{
	char value[7];

	/**************************************************************************/
	/* ��ȡ��ǰ�ĺ���� */
	ItemValue.sideSlipAngle = GetAngleValue(recv->buffer[1].data3);
	/* ���У׼ʹ�� */
	if (ItemValueSetZeroEnable.sideSlip == ENABLE)
	{
		/* �Ȳ�Ҫ��ձ�־λ���ȴ����ٶ�У׼ */
//		ItemValueSetZeroEnable.sideSlip = DISABLE;
		ItemZeroValue.sideSlipAngle = ItemValue.sideSlipAngle;
	}
	/* ���У׼ */
	ItemValue.sideSlipAngle -= ItemZeroValue.sideSlipAngle;


	/**************************************************************************/
	/* x����ٶ���data1λ�� */
	ItemValue.sideSlipAx = GetAccelerateSpeed(recv->buffer[0].data1);
	/* ���У׼ʹ�� */
	if (ItemValueSetZeroEnable.sideSlip == ENABLE)
	{
		ItemValueSetZeroEnable.sideSlip = DISABLE;
		/* ����ǰֵ��ΪУ׼ֵ */
		ItemZeroValue.sideSlipAx = ItemValue.sideSlipAx;
		/* У׼�����֮ǰ�ۼӵ�ֵ */
		ItemValue.sideSlipDistance = 0;
	}

	/* ���ٶ����У׼ */
	ItemValue.sideSlipAx -= ItemZeroValue.sideSlipAx;

	/* �����������Ư�ƣ�������ֵС��0.5��ֵ��ΪΪ��ֹ�������� */
	if (fabs(ItemValue.sideSlipAx) > 3)
	{
		/* ���ٶȻ��ֻ�ȡ�ٶ� */
		ItemValue.sideSlipVelocity += ItemValue.sideSlipAx*0.01;
		/* ���ٶ�Ϊ��ֵ���������ɲ������ʼ���ٶȻ��֣���λ�� */
		if (ItemValue.sideSlipAx < -3)
		{
			ItemValue.sideSlipDistance+=ItemValue.sideSlipAx*ItemValue.sideSlipAx/20000+ItemValue.sideSlipVelocity*0.01;
			/* ����λ�ƺͽǶȣ����ƫ���� */
			ItemValue.sideSlipOffset = ItemValue.sideSlipDistance * sin(ItemValue.sideSlipAngle);
		}
		else	/* ������֤���ڼ��ٹ��̣����ۼ�λ�� */
		{
//			ItemValue.brakeVelocityInit = 0;
			//ItemValue.brakeDistance     = 0;   hua
		}
	}

	/* ��ʾʵʱλ�� */
	sprintf(value, "%6.1f", ItemValue.sideSlipOffset);
#if DEVICE_OLED_DISPLAY_ENABLE
	OLED_ShowString(64, 4, value, 6);
#endif
#if DEVICE_BLE_SEND_ENABLE
	BLE_SendStruct.length = sizeof(ItemValue.sideSlipOffset);
	BLE_SendStruct.pack.data = ItemValue.sideSlipOffset;
	BLE_SendBytes(BLE_DATA_TYPE_SIDESLIP_DISTANCE_MAX);
#endif
}
