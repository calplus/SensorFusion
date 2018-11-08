#include "aebs.h"
#include "vehicle.h"

extern DAC_HandleTypeDef hdac;
uint8_t CrashWarningLv = WARNING_NONE;
float TimetoCrash_g;
AEBS_Status vAEBS_Status = {OFF, OFF};
#if VEHICLE_MODEL == DONGFENG
CAN_TxHeaderTypeDef CAN_TxXBRHeader={0,VEHICLE_BRAKE_ADDR, CAN_ID_EXT,CAN_RTR_DATA,8,DISABLE};
#endif


void StopBuzzer(AEBS_Status *pAEBS_Status)
{
//if(pAEBS_Status->BuzzerStatus == ON)
//{
	pAEBS_Status->BuzzerStatus = OFF;
	HAL_TIM_Base_Stop_IT(&htim2);
	BUZZER_OFF();
	LED_WARNING_OFF();
//}
}

void StartBuzzer(uint8_t warningLv)
{
	if (warningLv == WARNING_HIGH || warningLv == WARNING_MID)
	{
		if (htim2.Init.Period != HIGH_WARNING_TIM_PERIOD)
		{
			HAL_TIM_Base_Stop_IT(&htim2);
			MX_TIM2_Init(HIGH_WARNING_TIM_PERIOD);
			HAL_TIM_Base_Start_IT(&htim2);
		}
	}
	else if (warningLv == WARNING_LOW)
	{
		if (htim2.Init.Period != LOW_WARNING_TIM_PERIOD)
		{
			HAL_TIM_Base_Stop_IT(&htim2);
			MX_TIM2_Init(LOW_WARNING_TIM_PERIOD);
			HAL_TIM_Base_Start_IT(&htim2);
		}
	}
}

void DisableAEBS(AEBS_Status *pAEBS_Status)
{
	if (pAEBS_Status->valveStatus == ON)
	{
		pAEBS_Status->valveStatus = OFF;
		#if VEHICLE_MODEL != DONGFENG
		VALVE_DIS(); //��ֹɲ����Դ
		HAL_DACEx_DualSetValue(&hdac, DAC_ALIGN_12B_R, 0, 0);				//���DAC���
		#else
		
		#endif
	}
}

void EnableAEBS(float ttc, uint8_t warningLv)
{
	if (vAEBS_Status.valveStatus == OFF)
		vAEBS_Status.valveStatus = ON;
	#if VEHICLE_MODEL != DONGFENG
	ValveCalc(&hdac, ttc); //����ɲ��ǿ��
	if (HAL_GPIO_ReadPin(VALVE_ENABLE_GPIO_Port, VALVE_ENABLE_Pin) != GPIO_PIN_SET)
		VALVE_EN(); //ʹ��ɲ����Դ
	#else
	XBRCalc(&hcan2, ttc);
	#endif
}

/** 
 * @brief  Calculate Proportional Relay Valve
 * @note   
 * @param  *hdac: 
 * @retval 0 for ok
 */
uint8_t ValveCalc(DAC_HandleTypeDef *hdac, float ttc)
{
	uint32_t data1;
	if (ttc > HIGH_WARNING_TIME)
		data1 = 0;
	else
	{
		data1 = (HIGH_WARNING_TIME - ttc) / HIGH_WARNING_TIME * 4 / 3.3f * 4096;
		if (data1 > 2 / 3.3f * 4096)
			data1 = 2 / 3.3f * 4096;
	}
	HAL_DACEx_DualSetValue(hdac, DAC_ALIGN_12B_R, data1, data1);
	return 0;
}

/** 
 * @brief  Calculate external brake request
 * @note   
 * @param  *hcan: 
 * @retval 0 for ok
 */
uint8_t XBRCalc(CAN_HandleTypeDef *hcan, float ttc)
{
	float XAcc;
	uint16_t XAcc_int = 0;
	uint32_t CAN_TxMailBox = CAN_TX_MAILBOX0;
	uint8_t CANTxBuf[8] = {0};
	uint8_t temp_checksum, i;
	static uint8_t message_counter = 0;
	message_counter += 1;
	message_counter %= 64;
	if (ttc > HIGH_WARNING_TIME)
		XAcc = 0.0f;
	else
	{
		XAcc = (ttc - HIGH_WARNING_TIME) / HIGH_WARNING_TIME * 10.0f;
	}
	XAcc_int = (uint16_t)((XAcc + 15.687f)/1024);			//Acc Demand, Res1/1024, offset -15.687m/s2
	CANTxBuf[0] = (XAcc_int >> 8) & 0xFF;	
	CANTxBuf[1] = XAcc_int & 0xFF;
	CANTxBuf[2] = 0x00;	//XBR mode, if XBR EBI mode is 00, XBR urgency not using
	CANTxBuf[3] = (-XAcc/10.0f) / 0.004f;	//XBR urgency %, Res 0.4%
	
	for(i = 0;i < 7; i++)
		temp_checksum += CANTxBuf[i];
	temp_checksum += message_counter & 0x0F;
	temp_checksum += VEHICLE_BRAKE_ADDR & 0x0F;
	temp_checksum += VEHICLE_BRAKE_ADDR & 0xF0 >> 8;
	temp_checksum += VEHICLE_BRAKE_ADDR & 0xF00 >> 16;
	temp_checksum += VEHICLE_BRAKE_ADDR & 0xF000 >> 24;
	CANTxBuf[7] = (((temp_checksum >> 4) + temp_checksum) & 0x0F) & (message_counter << 4);
	
	HAL_CAN_AddTxMessage(hcan, &CAN_TxXBRHeader, CANTxBuf, &CAN_TxMailBox);
	return 0;
}
