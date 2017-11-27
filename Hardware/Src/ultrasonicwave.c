#include "ultrasonicwave.h"

ULTRASONICWAVE_RecvTypedef ULTRASONICWAVE_Recv;

/* �����½��ٶȼ�� */
extern uint16_t DownVelocity_Distance;
extern uint16_t DownVelocity_DistanceOld;
extern double   DownVelocity_Speed;

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
		/* ��ȡ��ǰ�ٶ� */
		DownVelocity_Speed = (DownVelocity_Distance - DownVelocity_DistanceOld)
				/ ULTRASONICWAVE_TIME_PERIOD;
		/* ���浱ǰֵ */
		DownVelocity_DistanceOld = DownVelocity_Distance;

#if DEVICE_OLED_DISPLAY_ENABLE
		size = sprintf(value, "%.1f", DownVelocity_Distance);
		OLED_ShowString(72, 2, value, size);
		size = sprintf(value, "%.1f", DownVelocity_Speed);
		OLED_ShowString(72, 4, value, size);
#endif
	}
}



