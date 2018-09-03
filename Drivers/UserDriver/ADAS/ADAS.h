#ifndef __ADAS_H
#define __ADAS_H

#include "stm32f4xx_hal.h"
#include "delay.h"
#include "WTN6040.h"

typedef struct
{
	uint8_t DEV_work_status;																//�����豸״̬
	uint8_t LDW_work_status;																//LDW����״̬
	uint8_t FCW_work_status;																//FCW����״̬
	uint8_t PCW_work_status;																//PCW����״̬
	uint8_t DFW_work_status;																//DFW����״̬
	uint8_t LDW_warning;																		//LDW����ƫ�� 0x00������0x01��ƫ�� 0x02��ƫ�� 0x03����
	uint8_t crash_type;																			//0x00 FCW ��0x01 PCW
	uint8_t crash_level;																		//0x00���� 0x01һ��Ԥ�� 0x02����Ԥ�� 0x03����Ԥ��
	uint8_t driver_status;																	//0x000���� 0x001ƣ�� 0x002��̬�쳣 0x003��绰 0x004����
	uint8_t	self_check_main;																//�Լ�״̬
	uint8_t line_type;																			//����������
	uint8_t distance_left_line;															//�ֱ���0.125
	uint8_t distance_right_line;														//�ֱ���0.125
	uint8_t line_circle_diameter;														//���������ʰ뾶 �ֱ���4
	uint8_t line_width;
	uint8_t direction_angle;																//�ֱ���1��ƫ��-90
	uint8_t distance_to_target; 														//�ֱ���0.5
	uint8_t speed_to_target;																//�ֱ���0.25
	float time_to_target;																		//�ֱ���0.05
	
} ADAS_HandleTypeDef;

uint8_t CalADASData(ADAS_HandleTypeDef *pADAS_dev, uint8_t *pRxBuf);
uint8_t DispADASData(uint8_t *pRxBuf, uint8_t *pDispBuf, float MinRangeLong_fp, float VrelLong_fp, float TimetoCrash_fp);

#endif

