#ifndef __AEBS_H
#define __AEBS_H

#include "stm32f4xx_hal.h"
#include "tim.h"
#include "gpio.h"
#include "user_radar.h"

#define LOW_WARNING_TIM_PERIOD 200 //ms
#define HIGH_WARNING_TIM_PERIOD 50 //ms

#define VEHICLE_SPEED_THRESHOLD 0

#define LOW_WARNING_TIME 4.8f
#define MID_WARNING_TIME 4.5f
#define HIGH_WARNING_TIME 4.2f

#define KSHINE 1
#define KY 0
/*------ADAS type------*/
#define ADAS_TYPE KY

#define LIGHT_TIME 3.8f
#define SOUND_TIME 4.4f

#define LANEWIDTH 1.5f
#define MAX_DECELARATION 0.4 * 9.8f
#define DELAY_TIME 0.4f
#define LIMIT_RANGE 200 

#define CAN_READ_VEHICLE 1 //Vehicle Speed & Switch
#define AEB_CAN_TX_TIME 50 //CAN Bus 50ms interval

#define WARNING_NONE 0	//3
#define WARNING_LOW 1		//5 
#define WARNING_MID 4		//6
#define WARNING_HIGH 7	//7

#define BRAKE_SYS_RDY		0x03
#define COLLISION_WARNING_ACTIVE  0x05
#define BRAKE_SYS_ON	0x06
#define BRAKE_SYS_EMER	0x07

#define OBJECT_NOT_DETECTED 0
#define OBJECT_DETECTED 1

#define ENTER_LANE_TIME_THRESHOLD 5.0f

#define ON 1
#define OFF 0

/* ---Turning_Flag values---*/
#define TURNING 1
#define STRAIGHT 0
/* ---Turning_Collision flag values---*/
#define TURNING_COLLISION_WARNING 1
#define TURNING_COLLISION_NONE 0

#define TURNING_WHEEL_TH 0.5f

/*------ADAS switch----*/
extern uint8_t adas_switch;
/*------AEBS switch----*/
extern uint8_t aebs_switch;
/*------AEBS quit switch----*/
extern uint8_t aebs_quit;

typedef struct
{
	uint8_t BuzzerStatus;
	uint8_t valveStatus;
	uint8_t AEBStimes;
	uint16_t onlyRadarTimes;
} AEBS_Status;

extern float TimetoCrash_g;
extern uint8_t crashWarningLv;
extern AEBS_Status vAEBS_Status;
extern CAN_HandleTypeDef hcan2;

void ClearAEBSStatus(void);
void StopBuzzer(AEBS_Status *pAEBS_Status);
void StartBuzzer(AEBS_Status *pAEBS_Status, uint8_t warningLv);
void DisableAEBS(AEBS_Status *pAEBS_Status);
void EnableAEBS(float ttc, uint8_t warningLv);
void CorrectDistance(float speed, uint16_t *RadarRxBuf);
uint8_t ValveCalc(DAC_HandleTypeDef *hdac, float ttc);
uint8_t EnterLaneCalc(ObjectTypeDef obj, float *enterLaneTime);
uint8_t ExecuteAEBS(uint8_t adas_crash_level, AEBS_Status *pAEBS_Status, float minRangeLong, float ttc, uint8_t warningLv);
#if VEHICLE_MODEL == DONGFENG
uint8_t XBRCalc(CAN_HandleTypeDef *hcan, float ttc, uint8_t XBR_Ctrl, float relSpeed, float range);
uint8_t PrePareAEBS1Data(CAN_HandleTypeDef *hcan, uint8_t brakeSysState, uint8_t warningLv, uint8_t objectDetected, float ttc, ObjectTypeDef *object);
#endif

#endif
