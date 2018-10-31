#ifndef __ARS408_H
#define __ARS408_H
/**
 * microwave radar - ARS408 by Continental
 * ===functions:
 * ---init & config
 * ARS_Init(hcan): start CAN2 for radar & (optional) config Radadr and RadarFilter
 * ARS_ConfigRadar(hcan): config Radar through can2
 * ARS_ConfigFilter(hcan): config Radar filter through can2
 * 
 * ---read Radar data
 * ARS_GetRadarObjStatus(...): read Radar Obj Status from can2, need to config can2 first
 * ARS_GetRadarObjGeneral(...): read Radar Obj General(distance, relVelocity...) from can2
 * ===#defines:
 * ADDR: (can id, std)
 * (write)config addr 0x200 ~ 0x408
 * (read) obj & cluster addr 0x600~0x60E
 * 
 * VALID && config value: values with shift(<<)
 * RADAR
 * FILTER
 *
 * structs
 */
#include "stm32f4xx_hal.h"

/* ��ַ����--------------------------------------*/
//�״�״̬������
#define RADAR_CFG_ADDR 					0x200
#define RADAR_STA_ADDR					0x201
//������״̬������
#define FILTER_CFG_ADDR					0x202
#define FILTER_STA_HEADER_ADDR	0x203
#define FILTER_STA_CFG_ADDR			0x204
//��ײ����״̬������
#define COLL_DET_CFG_ADDR				0x400
#define COLL_DET_REGN_CFG_ADDR  0x401 
#define COLL_DET_STA_ADDR				0x408
#define COLL_DET_REGN_STA_ADDR  0x402
//�ٶ���ת�������ź�
#define SPEED_INFO_ADDR					0x300
#define YAW_INFO_ADDR						0x301
//���伯Ⱥ���
#define CLUSTER_STA_ADDR				0x600
#define CLUSTER_GENERAL_ADDR		0x701
#define CLUSTER_QUALITY_ADDR		0x702
//Ŀ����
#define OBJ_STA_ADDR						0x60A
#define OBJ_GENERAL_ADDR				0x60B
#define OBJ_QUALITY_ADDR				0x60C
#define OBJ_EXTENDED_ADDR				0x60D
#define OBJ_WARNING_ADDR				0x60E
//����汾
#define VERSION_ID_ADDR					0x700 
//��ײ�̵�������
#define COLL_DET_RELAY_ADDR			0x008

#define INVALID 0x00
#define VALID		0x01

#define RADARCFG_MAXDISTANCE_VALID	(VALID<<0)
#define RADARCFG_SENSORID_VALID			(VALID<<1)
#define RADARCFG_RADARPOWER_VALID		(VALID<<2)
#define RADARCFG_OUTPUTTYPE_VALID		(VALID<<3)
#define RADARCFG_SENDQUALITY_VALID 	(INVALID<<4)
#define RADARCFG_SENDEXTINFO_VALID	(INVALID<<5)
#define RADARCFG_SORTINDEX_VALID		(VALID<<6)
#define RADARCFG_STOREINNVM_VALID		(VALID<<7)
#define RADARCFG_MAXDISTANCE 				(200)
#define RADARCFG_SENSORID 					(0)
#define RADARCFG_OUTPUTTYPE_NONE		(0<<3)
#define RADARCFG_OUTPUTTYPE_OBJ			(1<<3)
#define RADARCFG_OUTPUTTYPE_CLUSTER	(2<<3)
#define RADARCFG_RADARPOWER_STD			(0<<5)
#define RADARCFG_RADARPOWER_3dB			(1<<5)
#define RADARCFG_RADARPOWER_6dB			(2<<5)
#define RADARCFG_RADARPOWER_9dB			(3<<5)
#define RADARCFG_CTRLRELEY_VALID		(INVALID<<0)
#define RADARCFG_CTRLRELEY 					(INVALID<<1)
#define RADARCFG_SENDQUALITY 				(INVALID<<2)
#define RADARCFG_SENDEXTINFO 				(INVALID<<3)
#define RADARCFG_SORTINDEX_NONE			(0<<4)
#define RADARCFG_SORTINDEX_RANGE		(1<<4)
#define RADARCFG_SORTINDEX_RCS			(2<<4)
#define RADARCFG_STOREINNVM					(VALID<<7)
#define RADARCFG_RCS_THRES_VALID		(VALID<<0)
#define RADARCFG_RCSTHRES_STD				(0<<1)
#define RADARCFG_RCSTHRES_HIGHSENSE	(1<<1)

typedef struct
{
	uint8_t MaxDistance_valid;		//�Ƿ�����Զ��ɨ�����仯
	uint8_t SensorID_valid;				//�Ƿ���������ID�仯
	uint8_t RadarPower_valid;			//�Ƿ������书�ʱ仯
	uint8_t OutputType_valid;			//�Ƿ�����������ݱ仯
	uint8_t SendQuality_valid;		//�Ƿ��������Ŀ���Ⱥ������Ϣ
	uint8_t SendExtInfo_valid;		//�Ƿ�������չ��Ϣ����仯
	uint8_t SortIndex_valid;			//����Ŀ������˳��仯
	uint8_t StoreInNVM_valid;			//����洢Ŀǰ������NVM�仯
	uint16_t MaxDistance;					//Զ��ɨ����Զ���룬����ɨ�����̶�ΪԶ��һ�룬��׼����196~260 �ֱ���1.79m for 200m far scan
	uint8_t SensorID;							//������ID��0��7
	uint8_t OutputType;						//������� 0x01 Ŀ�ꣻ0x02 ���伯Ⱥ
	uint8_t RadarPower;						//�״﷢�书�� 0x00 ��׼��0x01 -3dB; 0x02 -6dB; 0x03 -9dB;
	uint8_t CtrlRelay_valid;			//�Ƿ�����̵�������
	uint8_t CtrlRelay;						//�̵�������
	uint8_t SendQuality;					//��Ⱥ��Ŀ��������Ϣ����ʹ��
	uint8_t SendExtInfo;					//��չ��Ϣ���ʹ�ܣ�����Ŀ����Ϣ���ʱ��Ч
	uint8_t SortIndex;						//ѡ��Ŀ������˳�򣬽���Ŀ����Ϣ���ʱ��Ч����Ⱥ���ʱ���ǰ���������
	uint8_t StoreInNVM;						//�洢Ŀǰ������NVM
	uint8_t RCS_Threshold_valid;	//��������RCS�����ȱ仯
	uint8_t RCS_Threshold;				//�����״�ɢ������(radar cross section)��������� 0x00��׼�� 0x01 ��������
} MW_RadarConfig;	

#define FILTERCFG_VALID								(VALID<<1)
#define FILTERCFG_FILTERACTIVE_VALID	(VALID<<2)
#define FILTERCFG_INDEX_NOFOBJ 				(0<<3)
#define FILTERCFG_INDEX_DISTANCE 			(1<<3)
#define FILTERCFG_INDEX_AZIMUTH 			(2<<3)
#define FILTERCFG_INDEX_VRELONCOME 		(3<<3)
#define FILTERCFG_INDEX_VRELDEPART		(4<<3)
#define FILTERCFG_INDEX_RCS 					(5<<3)
#define FILTERCFG_INDEX_LIFETIME 			(6<<3)
#define FILTERCFG_INDEX_SIZE 					(7<<3)
#define FILTERCFG_INDEX_PROBEXIST 		(8<<3)
#define FILTERCFG_INDEX_Y 						(9<<3)
#define FILTERCFG_INDEX_X 						(10<<3)
#define FILTERCFG_INDEX_VYRIGHTLEFT 	(11<<3)
#define FILTERCFG_INDEX_VXONCOME 			(12<<3)
#define FILTERCFG_INDEX_VYLEFTRIGHT 	(13<<3)
#define FILTERCFG_INDEX_VXDEPART 			(14<<3)
#define FILTERCFG_INDEX_OBJCLASS			(0xF<<3)
#define FILTERCFG_TYPE_CLUSTER 				(0<<7)
#define FILTERCFG_TYPE_OBJ 						(1<<7)


typedef struct
{
	uint8_t FilterCfg_Valid;						//�Ƿ������޸�FilterCfg
	uint8_t FilterCfg_Active;						//ʹ�ܻ��߽�ֹ��Ӧ�ı�׼��FilterCfg��������
	uint8_t FilterCfg_Index;						//����15�����õ�Ŀ¼
	uint8_t FilterCfg_Type;							//��������Ŀ����߷��伯Ⱥ
	uint16_t FilterCfg_Min_XXX;					//����Ŀ¼��������Сֵ
	uint16_t FilterCfg_Max_XXX;					//����Ŀ¼���������ֵ
}MW_RadarFilterConfig;


typedef struct
{
	uint16_t FilterCfg_Min_NofObj;			//��СĿ���Ⱥ�������
	uint16_t FilterCfg_Max_NofObj;			//���Ŀ���Ⱥ�������
	uint16_t FilterCfg_Min_Distance;		//��С���˰뾶
	uint16_t FilterCfg_Max_Distance;		//�����˰뾶
	uint16_t FilterCfg_Min_Azimuth;			//��С���˺���Ƕ�
	uint16_t FilterCfg_Max_Azimuth;			//�����˺���Ƕ�
	uint16_t FilterCfg_Min_VrelOncome;	//��С��Կ����ٶ�
	uint16_t FilterCfg_Max_VrelOncome;	//�����Կ����ٶ�
	uint16_t FilterCfg_Min_VrelDepart;	//��С���Զ���ٶ�
	uint16_t FilterCfg_Max_VrelDepart;	//������Զ���ٶ�
	uint16_t FilterCfg_Min_RCS;					//��С��������
	uint16_t FilterCfg_Max_RCS;					//���������
	uint16_t FilterCfg_Min_Lifetime;		//��С��̽��ʱ��
	uint16_t FilterCfg_Max_Lifetime;		//���̽��ʱ��
	uint16_t FilterCfg_Min_Size;				//��С̽��ߴ�
	uint16_t FilterCfg_Max_Size;				//���̽��ߴ�
	uint16_t FilterCfg_Min_ProbExists;	//��С������ڸ���
	uint16_t FilterCfg_Max_ProbExists;	//���������ڸ���
	uint16_t FilterCfg_Min_Y;						//Y�᷽����С����
	uint16_t FilterCfg_Max_Y;						//Y�᷽��������
	uint16_t FilterCfg_Min_X;						//X�᷽����С����
	uint16_t FilterCfg_Max_X;						//X�᷽��������
	uint16_t FilterCfg_Min_VYRightLeft;	//Y���������ƶ���С�ٶ�
	uint16_t FilterCfg_Max_VYRightLeft;	//Y���������ƶ�����ٶ�
	uint16_t FilterCfg_Min_VXOncome;		//X�����ʻ����С�ٶ�
	uint16_t FilterCfg_Max_VXOncome;		//X�����ʻ������ٶ�
	uint16_t FilterCfg_Min_VYLeftRight;	//Y���������ƶ���С�ٶ�
	uint16_t FilterCfg_Max_VYLeftRight;	//Y���������ƶ�����ٶ�
	uint16_t FilterCfg_Min_VXDepart;		//X���໥�뿪��С�ٶ�
	uint16_t FilterCfg_Max_VXDepart;		//X���໥�뿪����ٶ�
	uint16_t FilterCfg_Min_Object_Class;//��С����ȼ�
	uint16_t FilterCfg_Max_Object_Class;//�������ȼ�
}MW_RadarFilterIndexContent;
	
typedef struct
{
	uint8_t RadarState_NVMReadStatus;		//����ʱ�Ӳ����ڴ�����ò���״̬
	uint8_t RadarState_NVMwriteStatus;	//�ɹ��洢�����ò�����Ŀ
	uint16_t RadarState_MaxDistanceCfg;	//��ǰ��Զ����ɨ������
	uint8_t RadarState_Persistent_Error;//�������
	uint8_t RadarState_Interference;		//����״�ĸ���
	uint8_t RadarState_Temperature_Error;//�¶����
	uint8_t RadarState_Temporary_Error;	//��ʱ����������ʱ��ʧ�����
	uint8_t RadarState_Voltage_Error;		//��ѹ������ΧʱΪ1
	uint8_t RadarState_SensorID;				//������ID 0-7
	uint8_t RadarState_SortIndex;				//����˳�򣬶����б��
	uint8_t RadarState_RadarPowerCfg;		//�����źŵ�����
	uint8_t RadarState_CtrlRelayCfg;		//���Ƽ̵���
	uint8_t RadarState_OutputTypeCfg;		//������ͣ���Ⱥ�����
	uint8_t RadarState_SendQualityCfg;	//��������
	uint8_t RadarState_SendExtInfoCfg;	//���Ͷ�����Ϣ
	uint8_t RadarState_MotionRxState;		//�ٶȡ�ƫ���Ƕ������źŵ�״̬
	uint8_t RadarState_RCS_Threshold;		//Ϊ��ʱ����������������ģʽ
}MW_RadarState;

#define STANDSTILL	0x0
#define FORWARD			0x1
#define BACKWORD		0x2

typedef struct
{
	uint8_t RadarDevice_SpeedDirection;
	uint16_t RadarDevice_Speed;
}MW_RadarSpeed;

typedef struct
{
	uint8_t Obj_NofObjects;
	uint16_t Obj_MeasCounter;
	uint8_t Obj_InterfaceVer;
}MW_RadarObjStatus;

typedef struct
{
	uint8_t Obj_ID;
	uint16_t Obj_DistLong;
	uint16_t Obj_DistLat;
	uint16_t Obj_VrelLong;
	uint8_t Obj_DynProp;
	uint16_t Obj_VrelLat;
	uint8_t Obj_RCS;
}MW_RadarGeneral;


uint8_t ARS_Init(CAN_HandleTypeDef *hcan);
uint8_t ARS_ConfigRadar(CAN_HandleTypeDef *hcan);
uint8_t ARS_ConfigFilter(CAN_HandleTypeDef *hcan);
void ARS_GetRadarObjStatus(uint8_t* pCANRxBuf, MW_RadarObjStatus *pRadarObjStatus);
void ARS_GetRadarObjGeneral(uint8_t* pCANRxBuf, MW_RadarGeneral *pRadarGeneral);
void ARS_SendVehicleSpeed(CAN_HandleTypeDef *hcan, uint16_t VehicleSpeed);
void ARS_SendVehicleYaw(CAN_HandleTypeDef *hcan,float YawRate);
uint8_t ARS_CalcTurn(MW_RadarGeneral *pRadargGeneral_Closet, float YawRate, float VehicleSpeed);

#endif

