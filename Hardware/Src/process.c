#include "process.h"
#include "ultrasonicwave.h"
/******************************************************************************/
PROCESS_ModeEnum PROCESS_Mode = PROCESS_MODE_INVALID;
ItemValueTypedef     ItemValue;
ItemZeroValueTypedef ItemZeroValue;
ItemValueSetZeroEnableTypedef ItemValueSetZeroEnable;


/* �ƶ������� */
double BrakeDistance_speed;
double BrakeDistance_distance;
double BrakeDistance_brakeDistance;

/* �����½��ٶȼ�� */
uint16_t DownVelocity_Distance;
uint16_t DownVelocity_DistanceOld;
double   DownVelocity_Speed;
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
		ACCELERATE_Process();
		break;

	/* �����½��ٶȼ�� */
	case PROCESS_MODE_DETECTED_DOWN_VELOCITY:
		ULTRASONICWAVE_Process();
		break;

	/* �¶ȼ�� */
	case PROCESS_MODE_DETECTED_GRADIENT:
		ACCELERATE_Process();
		break;

	/* ﮵�ص������ */
//	case PROCESS_MODE_DETECTED_BATTERY_CAPACITY:
//		ANALOG_Process();
//		/* ﮵�ص�����⣬ֻ���һ�� */
//		PROCESS_Mode = PROCESS_MODE_INVALID;
//		break;

	default:
		break;
	}
}

/*******************************************************************************
 * @breif ���ݲ�ͬģʽ��ѡ��ǰģʽ�µĵ�ǰ״̬Ϊ�����ֵ
 */
void ZeroCalibration(void)
{
	switch (PROCESS_Mode)
	{
	/* ������ת������� */
	case PROCESS_MODE_DETECTED_STEERING_WHEEL_FORCE:
		ItemValueSetZeroEnable.steeringWheelForce = ENABLE;
		break;

	/* ������ת�Ǽ�� */
	case PROCESS_MODE_DETECTED_STEERING_WHEEL_ANGLE:
		ItemValueSetZeroEnable.steeringWheelAngle = ENABLE;
		break;

	/* �ƶ�̤������� */
	case PROCESS_MODE_DETECTED_PEDAL_FORCE:
		ItemValueSetZeroEnable.pedalForce = ENABLE;
		break;

	/* ��ɲ�ƶ������ */
	case PROCESS_MODE_DETECTED_HAND_BRAKE_FORCE:
		ItemValueSetZeroEnable.handBrakeForce = ENABLE;
		break;

	/* ���ȼ�� */
	case PROCESS_MODE_DETECTED_NOISE:
		ItemValueSetZeroEnable.noise = ENABLE;
		break;

	case PROCESS_MODE_DETECTED_BRAKING_DISTANCE:
		ItemValueSetZeroEnable.brakeAx = ENABLE;
		break;

	case PROCESS_MODE_DETECTED_SIDESLIP_DISTANCE:
		ItemValueSetZeroEnable.sideSlip = ENABLE;
		break;

	case PROCESS_MODE_DETECTED_DOWN_VELOCITY:
		ItemValueSetZeroEnable.downVelocity = ENABLE;
		break;


	case PROCESS_MODE_DETECTED_GRADIENT:
		ItemValueSetZeroEnable.gradient = ENABLE;
		break;

	default:
		break;
	}
}
