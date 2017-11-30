#include "encode.h"

/******************************************************************************/
uint16_t Encode_plusCnt = 0;				/* ������������ */
uint16_t Encode_plusCntOld = 0;				/* �������������� */
uint16_t Encode_periodCnt = 0;
FunctionalState Encode_processEnable;		/* ������Processʹ�� */
FunctionalState Encode_initEnable;

extern double BrakeDistance_speed;
extern double BrakeDistance_distance;

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

		Encode_plusCntOld = Encode_plusCnt;

		/* �����ٶ� */
		BrakeDistance_speed = ((ENCODE_WHEEL_PERIMETER / ENCODE_PERIOD_PLUS_CNT)
								* Encode_plusCnt) / 0.1;
		/* ������� */
		BrakeDistance_distance = (Encode_periodCnt * ENCODE_WHEEL_PERIMETER)
				+ ((ENCODE_WHEEL_PERIMETER / ENCODE_PERIOD_PLUS_CNT)
				* Encode_plusCnt);

		/* ��ʼ�ƶ� */
		if (BrakeDistance_speed < BrakeDistance_oldSpeed)
		{
			if (Encode_initEnable == DISABLE)
			{
				Encode_initEnable = ENABLE;
				LL_TIM_SetCounter(TIM3, 0);
				Encode_plusCntOld = 0;
				Encode_periodCnt = 0;
			}
			BrakeDistance_brakeDistance = (Encode_periodCnt * ENCODE_WHEEL_PERIMETER)
				+ ((ENCODE_WHEEL_PERIMETER / ENCODE_PERIOD_PLUS_CNT)
				* Encode_plusCnt);
		}
		else
		{
			Encode_initEnable = DISABLE;
		}
	}
}






