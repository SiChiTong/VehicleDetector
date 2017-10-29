#include "process.h"

/******************************************************************************/
PROCESS_ModeEnum PROCESS_Mode = PROCESS_MODE_INVALID;

/*******************************************************************************
 *
 */
void PROCESS(void)
{
	switch (PROCESS_Mode)
	{
	/* ������ת������� */
	case PROCESS_MODE_DETECTED_STEERING_WHEEL_FORCE:
		/* ��ȡת���� */
		PRESSURE_GetSteeringWheelForce();
		break;

	/* ������ת�Ǽ�� */
	case PROCESS_MODE_DETECTED_STEERING_WHEEL_ANGLE:
		ACCELERATE_Process();
		break;

	/* �ƶ������� */
	case PROCESS_MODE_DETECTED_BRAKING_DISTANCE:
		ACCELERATE_Process();
		break;

	/* �ƶ�̤������� */
	case PROCESS_MODE_DETECTED_PEDAL_FORCE:
		/* ��ȡ̤���� */
		PRESSURE_GetPedalForce();
		break;

	/* ��ɲ�ƶ������ */
	case PROCESS_MODE_DETECTED_HAND_BRAKE_FORCE:
		/* ��ȡ��ɲ�� */
		PRESSURE_GetHandBrakeForce();
		break;

	/* ���ȼ�� */
	case PROCESS_MODE_DETECTED_NOISE:
		/* ������������ */
		NOISE_Require();
		break;

	/* �໬����� */
	case PROCESS_MODE_DETECTED_SIDESLIP_DISTANCE:
		break;

	/* �����½��ٶȼ�� */
	case PROCESS_MODE_DETECTED_DOWN_VELOCITY:
		break;

	/* ﮵�ص������ */
	case PROCESS_MODE_DETECTED_BAT_VOLTAGE:
		ANALOG_Process();
		break;

	default:
		break;
	}
}



















