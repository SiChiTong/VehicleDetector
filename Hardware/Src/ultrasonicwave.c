#include "ultrasonicwave.h"
#include "oled.h"
#include "noise.h"
#include "ble.h"

/******************************************************************************/
ULTRASONICWAVE_RecvTypedef ULTRASONICWAVE_Recv;

/* �����½��ٶȼ�� */
extern uint16_t DownVelocity_Distance;
extern uint16_t DownVelocity_DistanceOld;
extern double   DownVelocity_Speed;
extern uint8_t  NOISE_RecvBytes[NOISE_UART_RX_BYTE_MAX];

extern BLE_SendStructTypedef BLE_SendStruct;

/*******************************************************************************
 *
 */
void ULTRASONICWAVE_Require(void)
{
	LL_USART_TransmitData8(USART2, 0x55);
}

/*******************************************************************************
 *
 */
void ULTRASONICWAVE_Process(void)
{
	char value[7];
	uint8_t size;

	if (ENABLE == ULTRASONICWAVE_Recv.status)
	{
		ULTRASONICWAVE_Recv.status = DISABLE;

		if (ULTRASONICWAVE_Recv.buffer.head != ULTRASONICWAVE_HEAD)
			return;

		/* ��ȡ��ؾ��� */
		DownVelocity_Distance = (ULTRASONICWAVE_Recv.buffer.dataH << 8)
				| (ULTRASONICWAVE_Recv.buffer.dataL);

		/* �泵�½��������С�����ڱ�������Ĭ��Ϊ���� */
		if (DownVelocity_Distance < DownVelocity_DistanceOld)
		{
			/* ��ȡ��ǰ�ٶ� */
//			DownVelocity_Speed = (double)(DownVelocity_DistanceOld - DownVelocity_Distance)
//						/ ULTRASONICWAVE_TIME_PERIOD;
			DownVelocity_Speed = DownVelocity_DistanceOld - DownVelocity_Distance;
		}
		else
		{
			DownVelocity_Speed = 0;
		}

		/* ���浱ǰֵ */
		DownVelocity_DistanceOld = DownVelocity_Distance;

#if DEVICE_OLED_DISPLAY_ENABLE
	size = sprintf(value, "%4d", DownVelocity_Distance);
	OLED_ShowString(72, 2, value, size);
	size = sprintf(value, "%5.1f", DownVelocity_Speed);
	OLED_ShowString(72, 4, value, size);
#endif
#if DEVICE_BLE_SEND_ENABLE
	BLE_SendStruct.length = sizeof(DownVelocity_Distance) + sizeof(DownVelocity_Speed);
	BLE_SendStruct.pack.DownVelocity_SendBuffer.distance = DownVelocity_Distance;
	BLE_SendStruct.pack.DownVelocity_SendBuffer.speed = DownVelocity_Speed;
	BLE_SendBytes(BLE_DATA_TYPE_DOWN_VELOCITY);
#endif
	}
}
