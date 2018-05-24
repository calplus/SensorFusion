/**
 * microwave radar - ARS408 by Continental
 * ---init & config
 * ARS_Init(hcan): start CAN2 for radar & (optional) config Radadr and RadarFilter
 * ARS_ConfigRadar(hcan): config Radar through can2
 * ARS_ConfigFilter(hcan): config Radar filter through can2
 * 
 * RadarConfig_func(): fill RadarConfig struct with #define in ARS408.h
 * RadarFilterConfig_func(index): fill RadarFilterConfig struct with #define in ARS408.h
 * FilterContentCfg_func(hcan, index, filter_min, filter_max): fill CANTxBuf[] && send 1 content of filter to can2
 * 
 * ---read Radar data
 * ARS_GetRadarObjStatus(...): read Radar Obj Status from can2, need to config can2 first
 * ARS_GetRadarObjGeneral(...): read Radar Obj General(distance, relVelocity...) from can2
 */
#include "ARS408.h"

#define CONFIG_ARS408_RADAR 1
#define CONFIG_ARS408_FILTER 0

CAN_TxHeaderTypeDef CAN_TxConfigRadarHeader={RADAR_CFG_ADDR,0,CAN_ID_STD,CAN_RTR_DATA,8,DISABLE};
CAN_TxHeaderTypeDef CAN_TxConfigFilterHeader={FILTER_CFG_ADDR,0,CAN_ID_STD,CAN_RTR_DATA,8,DISABLE};

MW_RadarConfig RadarConfig;
MW_RadarFilterConfig RadarFilterConfig;
MW_RadarFilterIndexContent FilterContent;


uint8_t ARS_Init(CAN_HandleTypeDef *hcan)
{
	//����CAN�˲�������Objct_General��Ϣ�������Ŀ��ľ��롢�ٶȵ�
	CAN_FilterTypeDef MW_RadarCANFilter={OBJ_GENERAL_ADDR<<5,0,0x0<<5,0,CAN_FILTER_FIFO0, 14, CAN_FILTERMODE_IDMASK,CAN_FILTERSCALE_32BIT,ENABLE,14};		//0x60B �� 0x60Aͬʱ���
	//CAN_FilterTypeDef MW_RadarCANFilter = {0,OBJ_GENERAL_ADDR,0,0xEFF,CAN_FILTER_FIFO0,CAN_FILTERMODE_IDLIST,CAN_FILTERSCALE_32BIT,ENABLE,0};
	HAL_CAN_ConfigFilter(hcan, &MW_RadarCANFilter);
	HAL_CAN_Start(hcan);
	HAL_CAN_ActivateNotification(hcan, CAN_IT_RX_FIFO0_MSG_PENDING);
	
	#if CONFIG_ARS408_RADAR
		ARS_ConfigRadar(hcan);
		HAL_Delay(100);
	#endif
	#if CONFIG_ARS408_FILTER
		ARS_ConfigFilter(hcan);
		HAL_Delay(100);
	#endif

	return 0;
}

void RadarConfig_func()
{
	RadarConfig.MaxDistance_valid = RADARCFG_MAXDISTANCE_VALID;
  RadarConfig.SensorID_valid = RADARCFG_SENSORID_VALID;
  RadarConfig.RadarPower_valid = RADARCFG_RADARPOWER_VALID;
  RadarConfig.OutputType_valid = RADARCFG_OUTPUTTYPE_VALID;
  RadarConfig.SendQuality_valid = RADARCFG_SENDQUALITY_VALID;
  RadarConfig.SendExtInfo_valid = RADARCFG_SENDEXTINFO_VALID;
  RadarConfig.SortIndex_valid = RADARCFG_SORTINDEX_VALID;
  RadarConfig.StoreInNVM_valid = RADARCFG_STOREINNVM_VALID;
  RadarConfig.MaxDistance = RADARCFG_MAXDISTANCE;
  RadarConfig.SensorID = RADARCFG_SENSORID;
  RadarConfig.OutputType = RADARCFG_OUTPUTTYPE_OBJ;
  RadarConfig.RadarPower = RADARCFG_RADARPOWER_STD;
  RadarConfig.CtrlRelay_valid = RADARCFG_CTRLRELEY_VALID;
  RadarConfig.CtrlRelay = RADARCFG_CTRLRELEY;
  RadarConfig.SendQuality = RADARCFG_SENDQUALITY;
  RadarConfig.SendExtInfo = RADARCFG_SENDEXTINFO;
  RadarConfig.SortIndex = RADARCFG_SORTINDEX_RANGE;
  RadarConfig.StoreInNVM = RADARCFG_STOREINNVM;
  RadarConfig.RCS_Threshold_valid = RADARCFG_RCS_THRES_VALID;
  RadarConfig.RCS_Threshold = RADARCFG_RCSTHRES_STD;
}

void RadarFilterConfig_func(uint8_t index)
{
  RadarFilterConfig.FilterCfg_Valid = FILTERCFG_VALID;
  RadarFilterConfig.FilterCfg_Active = FILTERCFG_FILTERACTIVE_VALID;
  RadarFilterConfig.FilterCfg_Index = index;
  RadarFilterConfig.FilterCfg_Type = FILTERCFG_TYPE_OBJ;
}

/**
 * [ARS_ConfigRadar config Radar]
 * @param  hcan [hcan index]
 * @return      [ok]
 */
uint8_t ARS_ConfigRadar(CAN_HandleTypeDef *hcan)
{
	uint32_t CAN_TxMailBox=CAN_TX_MAILBOX0;
	uint8_t CANTxBuf[8]={0};
  RadarConfig_func();
  //if change RadarConfig struct, add RadarConfig.*** = someValue here
	CANTxBuf[0]=RadarConfig.StoreInNVM_valid|RadarConfig.SortIndex_valid|RadarConfig.SendExtInfo_valid|RadarConfig.SendQuality_valid|\
		RadarConfig.OutputType_valid|RadarConfig.RadarPower_valid|RadarConfig.SensorID_valid|RadarConfig.MaxDistance_valid;
	CANTxBuf[1]=RadarConfig.MaxDistance>>2;
	CANTxBuf[2]=RadarConfig.MaxDistance<<6 & 0xC0;
	CANTxBuf[4]=RadarConfig.RadarPower|RadarConfig.OutputType|RadarConfig.SensorID;
	CANTxBuf[5]=RadarConfig.StoreInNVM|RadarConfig.SortIndex|RadarConfig.SendExtInfo|RadarConfig.SendQuality|RadarConfig.CtrlRelay|RadarConfig.CtrlRelay_valid;
	CANTxBuf[6]=RadarConfig.RCS_Threshold|RadarConfig.RCS_Threshold_valid;
	//CAN���߷�������
	HAL_CAN_AddTxMessage(hcan, &CAN_TxConfigRadarHeader, CANTxBuf, &CAN_TxMailBox);
	return 0;
}

/**
 * [FilterContentCfg_func config content of filter]
 * @param  hcan       [hcan index]
 * @param  index      [filter index]
 * @param  filter_min [content min]
 * @param  filter_max [content max]
 * @return            [ok]
 */
uint8_t FilterContentCfg_func(CAN_HandleTypeDef *hcan, uint8_t index, uint16_t filter_min, uint16_t filter_max)
{
	uint32_t CAN_TxMailBox=CAN_TX_MAILBOX0;
	uint8_t CANTxBuf[8]={0};
  RadarFilterConfig_func(index);
	CANTxBuf[0]= RadarFilterConfig.FilterCfg_Type|RadarFilterConfig.FilterCfg_Index|RadarFilterConfig.FilterCfg_Active|RadarFilterConfig.FilterCfg_Valid;
	CANTxBuf[1]= filter_min>>8 & 0x1F;
	CANTxBuf[2]= filter_min & 0xFF;
	CANTxBuf[3]= filter_max>>8 & 0x1F;
	CANTxBuf[4]= filter_max & 0xFF;
	//CAN���߷�������
	HAL_CAN_AddTxMessage(hcan, &CAN_TxConfigFilterHeader, CANTxBuf, &CAN_TxMailBox);
	return 0;
}

uint8_t ARS_ConfigFilter(CAN_HandleTypeDef *hcan)
{
  FilterContent.FilterCfg_Min_NofObj = 0;
  FilterContent.FilterCfg_Max_NofObj = 1;
  FilterContent.FilterCfg_Min_Distance = 0;   //0~200m, offset 0, Res 0.1
  FilterContent.FilterCfg_Max_Distance = 2000;
  FilterContent.FilterCfg_Min_Azimuth = 160;  //-8��~8�㣬offset -50, Res 0.025
  FilterContent.FilterCfg_Max_Azimuth = 200;
  FilterContent.FilterCfg_Min_VrelOncome = 3; //0.1~100m/s, offset 0, Res 0.0315
  FilterContent.FilterCfg_Max_VrelOncome = 3175;
  FilterContent.FilterCfg_Min_VrelDepart = 3; //0.1~100m/s, offset 0, Res 0.0315
  FilterContent.FilterCfg_Max_VrelDepart = 3175;
  FilterContent.FilterCfg_Min_RCS = 2001;     //0.025~30dBm2, offset -50, Res 0.025
  FilterContent.FilterCfg_Max_RCS = 3200;
  FilterContent.FilterCfg_Min_Lifetime = 2;   //0.1~409.5s, offset 0, Res 0.1
  FilterContent.FilterCfg_Max_Lifetime = 4095;
  FilterContent.FilterCfg_Min_Size = 1;       //0.025~20m2, offset 0, Res 0.025
  FilterContent.FilterCfg_Max_Size = 800;
  FilterContent.FilterCfg_Min_ProbExists = 0x5;//99%~100%, 0x0: 0%, 0x1: 25%, 0x2: 50%
  FilterContent.FilterCfg_Max_ProbExists = 0x7;//0x3: 75%, 0x4: 90%, 0x5: 99%, 0x6: 99.9%, 0x7: 100%
  FilterContent.FilterCfg_Min_Y = 2040;       //-1.5~1.5m, offset -409.5, Res 0.2
  FilterContent.FilterCfg_Max_Y = 2055;
  FilterContent.FilterCfg_Min_X = 2500;       //0~200m, offset -500, Res 0.2
  FilterContent.FilterCfg_Max_X = 3500;
  FilterContent.FilterCfg_Min_VYRightLeft = 3;//0.1~100m/s, offset 0, Res 0.0315
  FilterContent.FilterCfg_Max_VYRightLeft = 3175;
  FilterContent.FilterCfg_Min_VXOncome = 3;   //0.1~100m/s, offset 0, Res 0.0315
  FilterContent.FilterCfg_Max_VXOncome = 3175;
  FilterContent.FilterCfg_Min_VYLeftRight = 3;//0.1~100m/s, offset 0, Res 0.0315
  FilterContent.FilterCfg_Max_VYLeftRight = 3175;
  FilterContent.FilterCfg_Min_VXDepart = 3;   //0.1~100m/s, offset 0, Res 0.0315
  FilterContent.FilterCfg_Max_VXDepart = 3175;
  FilterContent.FilterCfg_Min_Object_Class = 0x5;//0x0: point, 0x1: car, 0x2: truck, 0x3: not used
  FilterContent.FilterCfg_Max_Object_Class = 0x2;//0x4: motorcyc, 0x5: bicycle, 0x6: wide, 0x7: reserved

  FilterContentCfg_func(hcan, FILTERCFG_INDEX_NOFOBJ, FilterContent.FilterCfg_Min_NofObj, FilterContent.FilterCfg_Max_NofObj);
  FilterContentCfg_func(hcan, FILTERCFG_INDEX_DISTANCE, FilterContent.FilterCfg_Min_Distance, FilterContent.FilterCfg_Max_Distance);
  FilterContentCfg_func(hcan, FILTERCFG_INDEX_AZIMUTH, FilterContent.FilterCfg_Min_Azimuth, FilterContent.FilterCfg_Max_Azimuth);
  FilterContentCfg_func(hcan, FILTERCFG_INDEX_VRELONCOME, FilterContent.FilterCfg_Min_VrelOncome, FilterContent.FilterCfg_Max_VrelOncome);
  FilterContentCfg_func(hcan, FILTERCFG_INDEX_VRELDEPART, FilterContent.FilterCfg_Min_VrelDepart, FilterContent.FilterCfg_Max_VrelDepart);
  FilterContentCfg_func(hcan, FILTERCFG_INDEX_RCS, FilterContent.FilterCfg_Min_RCS, FilterContent.FilterCfg_Max_RCS);
  FilterContentCfg_func(hcan, FILTERCFG_INDEX_LIFETIME, FilterContent.FilterCfg_Min_Lifetime, FilterContent.FilterCfg_Max_Lifetime);
  FilterContentCfg_func(hcan, FILTERCFG_INDEX_SIZE, FilterContent.FilterCfg_Min_Size, FilterContent.FilterCfg_Max_Size);
  FilterContentCfg_func(hcan, FILTERCFG_INDEX_PROBEXIST, FilterContent.FilterCfg_Min_ProbExists, FilterContent.FilterCfg_Max_ProbExists);
  FilterContentCfg_func(hcan, FILTERCFG_INDEX_Y, FilterContent.FilterCfg_Min_Y, FilterContent.FilterCfg_Max_Y);
  FilterContentCfg_func(hcan, FILTERCFG_INDEX_X, FilterContent.FilterCfg_Min_X, FilterContent.FilterCfg_Max_X);
  FilterContentCfg_func(hcan, FILTERCFG_INDEX_VYRIGHTLEFT, FilterContent.FilterCfg_Min_VYRightLeft, FilterContent.FilterCfg_Max_VYRightLeft);
  FilterContentCfg_func(hcan, FILTERCFG_INDEX_VXONCOME, FilterContent.FilterCfg_Min_VXOncome, FilterContent.FilterCfg_Max_VXOncome);
  FilterContentCfg_func(hcan, FILTERCFG_INDEX_VYLEFTRIGHT, FilterContent.FilterCfg_Min_VYLeftRight, FilterContent.FilterCfg_Max_VYLeftRight);
  FilterContentCfg_func(hcan, FILTERCFG_INDEX_VXDEPART, FilterContent.FilterCfg_Min_VXDepart, FilterContent.FilterCfg_Max_VXDepart);
  FilterContentCfg_func(hcan, FILTERCFG_INDEX_OBJCLASS, FilterContent.FilterCfg_Min_Object_Class, FilterContent.FilterCfg_Max_Object_Class);

  return 0;
}

void ARS_GetRadarObjStatus(uint8_t* pCANRxBuf, MW_RadarObjStatus *pRadarObjStatus)
{
	pRadarObjStatus->Obj_NofObjects=*pCANRxBuf;
	pRadarObjStatus->Obj_MeasCounter=((uint16_t)*(pCANRxBuf+1))<<8|*(pCANRxBuf+2);
}

void ARS_GetRadarObjGeneral(uint8_t* pCANRxBuf, MW_RadarGeneral *pRadarGeneral)
{
	(pRadarGeneral)->Obj_ID=*pCANRxBuf;	//OBJ_ID
	(pRadarGeneral)->Obj_DistLong= (((uint16_t)*(pCANRxBuf+1))<<8|*(pCANRxBuf+2))>>3;
	(pRadarGeneral)->Obj_DistLat= ((uint16_t)*(pCANRxBuf+2)&0x07)<<8|(*(pCANRxBuf+3));
	(pRadarGeneral)->Obj_VrelLong= (((uint16_t)*(pCANRxBuf+4))<<8|*(pCANRxBuf+5))>>6;//��������ٶ�
	(pRadarGeneral)->Obj_VrelLat= (((uint16_t)*(pCANRxBuf+5)&0x3F)<<8|(*(pCANRxBuf+6)&0xE0))>>5;;//��������ٶ�
	(pRadarGeneral)->Obj_DynProp= *(pCANRxBuf+6)&0x07;		//Ŀ�궯̬���ԣ��˶����Ǿ�ֹ��
	(pRadarGeneral)->Obj_RCS= *(pCANRxBuf+7);
}

/*
--> Find mostImportantObject  <--
 # Safe (green): There is no car in the ego lane (no MIO), the MIO is
   moving away from the car, or the distance is maintained constant.
 # Caution (yellow): The MIO is moving closer to the car, but is still at
   a distance above the FCW distance. FCW distance is calculated using the
   Euro NCAP AEB Test Protocol. Note that this distance varies with the
   relative speed between the MIO and the car, and is greater when the
   closing speed is higher.
 # Warn (red): The MIO is moving closer to the car, and its distance is
   less than the FCW distance.
*/
/*
uint8_t FindMIObj(MW_RadarGeneral *pRadarGeneral)
{
	uint8_t FCW = 0;
	uint8_t Find0x60B = 0;
	uint16_t i = 0;
    float MinRange = 0.0;
    float MaxRange = MAX_RANGE;
    float gAccel = 9.8;
    float maxDeceleration = 0.4 * gAccel;		//���賵�������ٶ���0.4g
    float delayTime = 1.2;
    float relSpeed = 0.0;

    //ARS_GetRadarObjStatus(CANRxBuf);
    //ARS_GetRadarObjGeneral(CANRxBuf, RadarGeneral);
    
    for(i = 0; i < taskMsg->TaskMsgNum.Index[isRead] ; i++) //    for(i = 0; i < taskMsg->TaskMsgNum.Index[isRead] + 1; i++)
    {
        if(taskMsg->RxMessage[isRead][i].StdId == 0x60B)
        {
        	ARS_GetRadarObjGeneral(CANRxBuf, RadarGeneral);
            //ARS_ObjList = ARS_Obj_Handle(&ARSCANmsg.RxMessage[isRead][i]);
            //if(RadarGeneral.Obj_DynProp == 0x2)//0x2 means oncoming
            if( ARS_ObjList.Obj_LongDispl != 0 &&((ABS((ARS_ObjList.Obj_LatDispl) * 2.0)) < LANEWIDTH ) &&//�Ƿ��ڳ�����
                    ARS_ObjList.Obj_LongDispl < MaxRange)//
            {
                MinRange = RadarGeneral.Obj_LongDispl;						//MaxRange��ֵ������С����
                MaxRange = MinRange;
                relSpeed = RadarGeneral.Obj_VrelLong;
            }
        }
    }

    Segment_Num = MinRange;
    if(MinRange == 0)
    {
        return FCW = 0;			//����������Ϊ0����û��FCW����
    }
    else if(MinRange > 0)		//����˾������0��˵���п����б���
    {
        if(relSpeed < 0)		//��������ڿ���
        {
        	//����ɲ������
            float distance = relSpeed * (-1)  * delayTime +  relSpeed * relSpeed / 2 / maxDeceleration;//����ʽ-vt+v*v/2/a �������

            if(MinRange <= distance)
            {
                return  FCW = 2; //red			//���̫������ɫ����		
            }
            else
            {
                return  FCW = 1; //yellow		//�����������ٶ���ɲס����Ҳ���ڱ������뷶Χ֮����
            }
        }
        else
        {
            //there is a stationary object in front of the vehilce .//���Զ����߾�ֹ���ϰ���
            return FCW = 0;
        }
    }
}*/



