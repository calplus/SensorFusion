#ifndef __VEHICLE_H
#define __VEHICLE_H

#include "stm32f4xx_hal.h"

#define KINGLONG 0
#define YUTONG 1
#define BYD 2
#define BENZ 3
#define DONGFENG 4

#define DONGFENG_VEHICLE_SPEED_ADDR		0x18FEBF0B	//3.1.4 EBC2, wheel speed(from EBS)
#define DONGFENG_VEHICLE_ANGLE_ADDR		0x18F0090B	//from EBS, or 0x18F0093E from dynamic stability controller
#define DONGFENG_VEHICLE_SWITHC_ADDR	0x0CFDCC00	//00 = xx, 3.1.17 OEL Operators External Light, 3.1.18 front Operator Wiper and Washer

#define KINGLONG_VEHICLE_SPEED_ADDR		0x0CFE6CEE
#define KINGLONG_VEHICLE_SWITCH_ADDR	0x18FA0517
#define KINGLONG_VEHICLE_ANGLE_ADDR		0x18F0090B
#define KINGLONG_VEHICLE_AEBS_ADDR   	0x0CF02FA0

#define YUTONG_VEHICLE_SPEED_ADDR	0x18FE6E0B

#define BYD_VEHICLE_SPEED_ADDR	0x18FEF100
#define BENZ_VEHICLE_SPEED_ADDR 0x18FE6C00

#define GYRO_ADDR 0x18FEE0D8

/*-------- define vehicle model ---------*/
#define VEHICLE_MODEL KINGLONG

#if VEHICLE_MODEL == DONGFENG
	#define VEHICLE_SPEED_ADDR	DONGFENG_VEHICLE_SPEED_ADDR
	#define VEHICLE_SWITCH_ADDR	DONGFENG_VEHICLE_SWITCH_ADDR
	#define VEHICLE_ANGLE_ADDR	DONGFENG_VEHICLE_ANGLE_ADDR
#elif	VEHICLE_MODEL == KINGLONG
	#define VEHICLE_SPEED_ADDR 	KINGLONG_VEHICLE_SPEED_ADDR
	#define VEHICLE_SWITCH_ADDR KINGLONG_VEHICLE_SWITCH_ADDR
	#define VEHICLE_ANGLE_ADDR 	KINGLONG_VEHICLE_ANGLE_ADDR
	#define VEHICLE_AEBS_ADDR		KINGLONG_VEHICLE_AEBS_ADDR
#elif VEHICLE_MODEL == YUTONG
	#define VEHICLE_SPEED_ADDR	YUTONG_VEHICLE_SPEED_ADDR
#elif VEHICLE_MODEL == BYD
	#define VEHICLE_SPEED_ADDR 	BYD_VEHICLE_SPEED_ADDR
#elif VEHICLE_MODEL == BENZ
	#define VEHICLE_SPEED_ADDR	BENZ_VEHICLE_SPEED_ADDR
#endif

#define GYRO_VEHICLE 0
#define GYRO_MPU6050 1
/*-------- define Gryoscope type ---------*/
#define GYRO_TYPE GYRO_VEHICLE

typedef struct
{
  uint8_t brake;
  uint8_t right_turn;
  uint8_t left_turn;
}VehicleSwitchTypeDef;

typedef struct
{
	float tw_angle;	//turn wheel
	uint8_t  tw_circle;	
	uint8_t  tw_type;
	float yawRate;
	float latAcc;	//latitude
	float longAcc;//longitude
	uint8_t speed;
}VehicleTypeDef;

extern VehicleSwitchTypeDef vehicleSwitch;
extern VehicleTypeDef vehicle;

#endif

