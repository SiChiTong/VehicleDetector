#include "encode.h"

/******************************************************************************/
uint16_t Encode_plusCnt = 0;				/* ������������ */
uint16_t Encode_plusCntOld = 0;				/* �������������� */
uint16_t Encode_periodCnt = 0;
FunctionalState Encode_processEnable;		/* ������Processʹ�� */
FunctionalState Encode_initEnable;

extern double BrakeDistance_speed;
extern double BrakeDistance_oldSpeed;
extern double BrakeDistance_distance;
extern double BrakeDistance_brakeDistance;


/*******************************************************************************
 *
 */
void ENCODE_Process(void)
{
	if (Encode_processEnable == ENABLE)
	{
		Encode_processEnable = DISABLE;

		/* ��������������� */
		if (Encode_plusCnt > Encode_plusCntOld)
		{
			Encode_plusCnt -= Encode_plusCntOld;
		}
		else
		{
			Encode_plusCnt += ENCODE_PERIOD_PLUS_CNT - Encode_plusCntOld;
		}

		/* �����ֵ */
		Encode_plusCntOld = Encode_plusCnt;

		/* �����ٶ� */
		BrakeDistance_speed = ((ENCODE_WHEEL_PERIMETER / ENCODE_PERIOD_PLUS_CNT)
								* Encode_plusCnt);
		/* ������� */
		BrakeDistance_distance = (Encode_periodCnt * ENCODE_WHEEL_PERIMETER)
				+ ((ENCODE_WHEEL_PERIMETER / ENCODE_PERIOD_PLUS_CNT)
				* Encode_plusCnt);

		/* ��ʼ�ƶ� */
		/* todo �ƶ����������� */
		/* ���̤�����������ٶȣ��ж��ٶȣ� */
//		if (BrakeDistance_speed < BrakeDistance_oldSpeed)
//		{
//			if (Encode_initEnable == DISABLE)
//			{
//				Encode_initEnable = ENABLE;
//				LL_TIM_SetCounter(TIM3, 0);
//				Encode_plusCntOld = 0;
//				Encode_periodCnt = 0;
//			}
//			BrakeDistance_brakeDistance = (Encode_periodCnt * ENCODE_WHEEL_PERIMETER)
//				+ ((ENCODE_WHEEL_PERIMETER / ENCODE_PERIOD_PLUS_CNT)
//				* Encode_plusCnt);
//		}
//		else
//		{
//			Encode_initEnable = DISABLE;
//		}

#if DEVICE_OLED_DISPLAY_ENABLE
//		size = sprintf(value, "%4d", DownVelocity_Distance);
//		OLED_ShowString(72, 2, value, size);
//		size = sprintf(value, "%5.1f", DownVelocity_Speed);
//		OLED_ShowString(72, 4, value, size);
//#endif
//#if DEVICE_BLE_SEND_ENABLE
//		BLE_SendStruct.length = sizeof(DownVelocity_Distance) + sizeof(DownVelocity_Speed);
//		BLE_SendStruct.pack.DownVelocity_SendBuffer.distance = DownVelocity_Distance;
//		BLE_SendStruct.pack.DownVelocity_SendBuffer.speed = DownVelocity_Speed;
//		BLE_SendBytes(BLE_DATA_TYPE_DOWN_VELOCITY);
#endif
	}
}






