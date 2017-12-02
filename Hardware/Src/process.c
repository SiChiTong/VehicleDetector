#include "process.h"
#include "ultrasonicwave.h"
#include "encode.h"
#include "pressure.h"
#include "oled.h"
#include "ble.h"

/******************************************************************************/
PROCESS_ModeEnum PROCESS_Mode = PROCESS_MODE_INVALID;
ItemValueTypedef     ItemValue;
ItemZeroValueTypedef ItemZeroValue;
ItemValueSetZeroEnableTypedef ItemValueSetZeroEnable;


/* �ƶ������� */
FunctionalState BrakeDistance_SetZeroEnable;
double BrakeDistance_pedalForce;				/* ̤���� */
double BrakeDistance_pedalForceZeroValue = PEDAL_FORCE_ZERO_VALUE;/* ̤������� */
double BrakeDistance_pedalForceFullValue = PEDAL_FORCE_FULL_VALUE;/* ̤�������� */
double BrakeDistance_speed;						/* �ƶ����-�ٶ� */
double BrakeDistance_oldSpeed;					/* �ƶ����-���ٶ� */
double BrakeDistance_initSpeed;					/* ��ʼ�ٶ� */
double BrakeDistance_distance;					/* �ƶ����-���� */

/* �����½��ٶȼ�� */
uint16_t DownVelocity_Distance;
uint16_t DownVelocity_DistanceOld;
double   DownVelocity_Speed;



uint16_t Encode_plusCnt = 0;				/* ������������ */
uint16_t Encode_plusCntOld = 0;				/* �������������� */
uint16_t Encode_periodCntTotal = 0;			/* �������� */
uint16_t Encode_periodCnt = 0;				/* ������ */
FunctionalState Encode_processEnable;		/* ������Processʹ�� */
FunctionalState Encode_initEnable;

extern BLE_SendStructTypedef BLE_SendStruct;

/******************************************************************************/
void SteeringWheel_ForceAndAngleProcess(void);
void PROCESS_PedalForceAndBrakeDistance(void);

/*******************************************************************************
 *
 */
void PROCESS(void)
{
	switch (PROCESS_Mode)
	{
	/* ������ת������� */
	case PROCESS_MODE_DETECTED_STEERING_WHEEL_FORCE_AND_ANGLE:
		SteeringWheel_ForceAndAngleProcess();
		break;

	/* ̤�������ƶ������� */
	case PROCESS_MODE_DETECTED_PEDAL_FORCE_BRAKING_DISTANCE:
//		ACCELERATE_Process();
//		ENCODE_Process();
		/* ��ȡ̤���� */
//		PRESSURE_GetPedalForce();
		PROCESS_PedalForceAndBrakeDistance();
		break;

//	/* �ƶ�̤������� */
//	case PROCESS_MODE_DETECTED_PEDAL_FORCE:
//		/* ��ȡ̤���� */
//		PRESSURE_GetPedalForce();
//		break;

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
	case PROCESS_MODE_DETECTED_BATTERY_CAPACITY:
		ANALOG_Process();
		/* ﮵�ص�����⣬ֻ���һ�� */
		PROCESS_Mode = PROCESS_MODE_INVALID;
		break;

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
	case PROCESS_MODE_DETECTED_STEERING_WHEEL_FORCE_AND_ANGLE:
		ItemValueSetZeroEnable.steeringWheelForce = ENABLE;
		break;

	/* �ƶ�̤������� */
	case PROCESS_MODE_DETECTED_PEDAL_FORCE_BRAKING_DISTANCE:
		BrakeDistance_SetZeroEnable = ENABLE;
		break;

	/* ��ɲ�ƶ������ */
	case PROCESS_MODE_DETECTED_HAND_BRAKE_FORCE:
		ItemValueSetZeroEnable.handBrakeForce = ENABLE;
		break;

	/* ���ȼ�� */
	case PROCESS_MODE_DETECTED_NOISE:
		ItemValueSetZeroEnable.noise = ENABLE;
		break;

//	case PROCESS_MODE_DETECTED_BRAKING_DISTANCE:
//		ItemValueSetZeroEnable.brakeAx = ENABLE;
//		break;

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

/*******************************************************************************
 *
 */
void SteeringWheel_ForceAndAngleProcess(void)
{
	int32_t data = 0;
	char value[7];

	/* ��ȡ24bitADֵ */
	data = HX711_ReadValue();
	/* ��ȡ��ADֵ��ȥ�м�ֵ */
	data -= PRESSURE_RANGE_MEDIAN_VALUE;

	/* ת��ת����ֵ */
//	ItemValue.steeringWheelForce =
//			(data / (double)PRESSURE_Param.steeringWheelForceRange)
//			* PRESSURE_RANGE_STEERING_WHEEL_FORCE * 5.44;
	ItemValue.steeringWheelForce =
			(data / (double)1)
			* PRESSURE_RANGE_STEERING_WHEEL_FORCE * 5.44;

	if (ItemValueSetZeroEnable.steeringWheelForce == ENABLE)
	{
		ItemValueSetZeroEnable.steeringWheelForce = DISABLE;
		ItemZeroValue.steeringWheelForce = ItemValue.steeringWheelForce;
	}

	/* ���У׼ */
	ItemValue.steeringWheelForce -= ItemZeroValue.steeringWheelForce;

	/* ��ȡ�����̽Ƕ� */
	ACCELERATE_Process();

#if DEVICE_OLED_DISPLAY_ENABLE
	sprintf(value, "%6.1f", ItemValue.steeringWheelForce);
	OLED_ShowString(56, 2, value, sizeof(value));
#endif
#if DEVICE_BLE_SEND_ENABLE
	BLE_SendStruct.length = sizeof(ItemValue.steeringWheelForce) +
			sizeof(ItemValue.steeringWheelAngle);
	BLE_SendStruct.pack.SteeringWheel_ForceAndAngle.force = ItemValue.steeringWheelForce;
	BLE_SendStruct.pack.SteeringWheel_ForceAndAngle.angle = ItemValue.steeringWheelAngle;
	BLE_SendBytes(BLE_DATA_TYPE_STEERING_WHELL_FORCE_AND_ANGLE);
#endif

	HAL_Delay(200);
}

/*******************************************************************************
 *
 */
void PROCESS_PedalForceAndBrakeDistance(void)
{
	uint16_t plusCnt = 0;
	uint32_t data = 0;
	uint8_t  value[10];
	uint8_t  size = 0;

	if (Encode_processEnable == ENABLE)
	{
		Encode_processEnable = DISABLE;

		plusCnt = ((ENCODE_PERIOD_PLUS_CNT - Encode_plusCntOld) +
				((Encode_periodCnt - 1) * ENCODE_PERIOD_PLUS_CNT) + Encode_plusCnt) / 2;
		/* �����ɵ���������Ҫ��� */
		Encode_periodCnt = 0;
		
		/* �����ֵ */
		Encode_plusCntOld = Encode_plusCnt;

		/* �����ٶ� */
		BrakeDistance_speed = ((ENCODE_WHEEL_PERIMETER / (double)ENCODE_PERIOD_PLUS_CNT)
								* plusCnt) / GET_VALUE_TIME_PERIOD;

		/* ��ȡ24bitADֵ */
		data = HX711_ReadValue();

		if (BrakeDistance_SetZeroEnable == ENABLE)
		{
			BrakeDistance_SetZeroEnable = DISABLE;
			BrakeDistance_pedalForceZeroValue = data;
		}

		/* ת��̤����ֵ */
		data -= BrakeDistance_pedalForceZeroValue;
		BrakeDistance_pedalForce = (data /
				(double)(BrakeDistance_pedalForceFullValue - BrakeDistance_pedalForceZeroValue))
				* PRESSURE_RANGE_PEDAL_FORCE;

		/* ��ʼ�ƶ� */
		if (BrakeDistance_pedalForce > 5)
		{
			if (Encode_initEnable == DISABLE)
			{
				Encode_initEnable = ENABLE;
				LL_TIM_SetCounter(TIM3, 0);
				Encode_plusCntOld = 0;
				Encode_periodCnt = 0;
				Encode_periodCntTotal = 0;
				BrakeDistance_initSpeed = BrakeDistance_speed;
			}
			/* ������� */
			BrakeDistance_distance = ((Encode_periodCntTotal * ENCODE_WHEEL_PERIMETER)
					+ ((ENCODE_WHEEL_PERIMETER / (double)ENCODE_PERIOD_PLUS_CNT)
					* Encode_plusCnt)) / 2;
		}
		else
		{
			Encode_initEnable = DISABLE;
			BrakeDistance_initSpeed = 0;
			BrakeDistance_distance = 0;
		}
#if DEVICE_OLED_DISPLAY_ENABLE
		size = sprintf(value, "%6.1f", BrakeDistance_pedalForce);
		OLED_ShowString(56, 0, value, size);
		size = sprintf(value, "%6.1f", BrakeDistance_initSpeed);
		OLED_ShowString(56, 2, value, size);
		size = sprintf(value, "%6.1f", BrakeDistance_speed);
		OLED_ShowString(56, 4, value, size);
		size = sprintf(value, "%6.1f", BrakeDistance_distance);
		OLED_ShowString(56, 6, value, size);
#endif
#if DEVICE_BLE_SEND_ENABLE
		BLE_SendStruct.length = sizeof(BrakeDistance_pedalForce)
				+ sizeof(BrakeDistance_initSpeed) + sizeof (BrakeDistance_speed)
				+ sizeof(BrakeDistance_distance);
		BLE_SendStruct.pack.PedalForce_BrakeDistance.pedalForce = BrakeDistance_pedalForce;
		BLE_SendStruct.pack.PedalForce_BrakeDistance.initSpeed  = BrakeDistance_initSpeed;
		BLE_SendStruct.pack.PedalForce_BrakeDistance.speed      = BrakeDistance_speed;
		BLE_SendStruct.pack.PedalForce_BrakeDistance.distance   = BrakeDistance_distance;
		BLE_SendBytes(BLE_DATA_TYPE_PEDAL_FORCE_AND_BRAKING_DISTANCE);
#endif
	}
}






