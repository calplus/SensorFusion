/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
**/
/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "cmsis_os.h"

/* USER CODE BEGIN Includes */
#include "thread.h"
#include "ADAS.h"
#include "ARS408.h"
#include "can.h"
#include "cmd.h"
#include "dac.h"
#include "EMRR.h"
#include "main.h"
#include "usart.h"

#include "tim.h"
#include "vehicle.h"
#include "aebs.h"
#include "user_adas.h"
#include "user_can.h"
#include "user_radar.h"
/* USER CODE END Includes */

/* Variables -----------------------------------------------------------------*/


/* USER CODE BEGIN Variables */
#define YAWRATE_LIMIT 327.68f

osThreadId defaultTaskHandle;
osThreadId soundWarningHandle;
osThreadId radarCalcHandle;
#ifdef ADAS_COMM
osThreadId ADAS_CommTaskHandle;
#endif

osSemaphoreId bSemPrepareCanDataSigHandle;
osSemaphoreId bSemRadarCalcSigHandle;

//uint8_t Turning_Collision = 0;
//uint8_t Turning_Flag = 0;

/* USER CODE END Variables */

/* Function prototypes -------------------------------------------------------*/
void StartDefaultTask(void const * argument);			//always running
void StartADAS_CommTask(void const * argument);		//always running
void StartPrepareCANDataTask(void const * argument);
void StartRadarCalcTask(void const * argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

uint8_t minRadarDistFlag = 0;			//���Ŀ��״̬λ
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

/* Hook prototypes */

/* Init FreeRTOS */

void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */
       
  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* Create the semaphores(s) */
  /* definition and creation of bSemPrepareCANDataSig */

  osSemaphoreDef(bSemPrepareCANDataSig);
  bSemPrepareCanDataSigHandle = osSemaphoreCreate(osSemaphore(bSemPrepareCANDataSig), 1);
  /* definition and creation of bSemRadarCalcSig */
  osSemaphoreDef(bSemRadarCalcSig);
  bSemRadarCalcSigHandle = osSemaphoreCreate(osSemaphore(bSemRadarCalcSig), 1);

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  osSemaphoreWait(bSemPrepareCanDataSigHandle, osWaitForever);	//read once to disable
  osSemaphoreWait(bSemRadarCalcSigHandle, osWaitForever);			//read once to disable
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* Create the thread(s) */
  /* definition and creation of defaultTask */
  osThreadDef(defaultTask, StartDefaultTask, osPriorityIdle, 0, 128);
  defaultTaskHandle = osThreadCreate(osThread(defaultTask), NULL);
	
	#ifdef ADAS_COMM
	osThreadDef(ADAS_CommTask, StartADAS_CommTask, osPriorityNormal, 0, 128);
  ADAS_CommTaskHandle = osThreadCreate(osThread(ADAS_CommTask), NULL);
	#endif
  /* definition and creation of PrepareCANData */
  osThreadDef(PrepareCANData, StartPrepareCANDataTask, osPriorityNormal, 0, 128);
  soundWarningHandle = osThreadCreate(osThread(PrepareCANData), NULL);

  /* definition and creation of RadarCalc */
  osThreadDef(RadarCalc, StartRadarCalcTask, osPriorityNormal, 0, 128);
  radarCalcHandle = osThreadCreate(osThread(RadarCalc), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */
	  #if ADAS_COMM
	__HAL_UART_ENABLE_IT(&huart3, UART_IT_IDLE);	//ADAS
  #endif
	
	#if RADAR_TYPE == ARS408
		ARS_Init(&hcan3);
	#else
		EMRR_Init(&hcan3);
	#endif
	
	#if CAN_READ_VEHICLE
		CAN1_Init(&hcan2);
		Vehicle_CAN_Init(&hcan2);
  #endif
	DisableAEBS(&vAEBS_Status);
	HAL_DAC_Start(&hdac, DAC_CHANNEL_1);
	HAL_DAC_Start(&hdac, DAC_CHANNEL_2);
	HAL_TIM_Base_Start_IT(&htim3);
}

/* StartDefaultTask function */
void StartDefaultTask(void const * argument)
{
  /* USER CODE BEGIN StartDefaultTask */
  /* Infinite loop */
  for(;;)
  {
		if(AEB_CAN_TxReady==1)
		{
			AEB_CAN_TxReady=0;
			HAL_CAN_AddTxMessage(&hcan2, &AEB_CAN_TxHeader, AEB_CAN_TxBuf, &AEB_CAN_TxMailBox);
		}
		osDelay(20);
  }
  /* USER CODE END StartDefaultTask */
}

void StartADAS_CommTask(void const * argument)
{
	for(;;)
	{
		if(ADASRxComplete==1)
		{
			ADASRxComplete=0;
      //DispADASData(ADASRxBuf, ADASDispBuf, MinRangeLong_g, VrelLong_g, TimetoCrash_g);//���͸���ʾ��
			//HAL_UART_Transmit(&huart2, ADASDispBuf, 32, 100);//transmit ADAS data to screen
			CalADASData(&ADAS_dev, ADASRxBuf);
			HAL_GPIO_TogglePin(LED2_GPIO_Port,LED2_Pin);
			#ifdef LDW
				if(ADAS_dev.LDW_warning==0x01 || ADAS_dev.LDW_warning == 0x02)//Lane departure warning(left/right)
				{
					osSemaphoreRelease(bSemPrepareCanDataSigHandle);
				}
				#endif
		}
		osDelay(10);
	}
}

/* StartPrepareCANDataTask function */
void StartPrepareCANDataTask(void const * argument)
{
  /* USER CODE BEGIN StartPrepareCANDataTask */
  /* Infinite loop */
	uint8_t tempTTC;
  for(;;)
  {
		osSemaphoreWait(bSemPrepareCanDataSigHandle, osWaitForever);
		//Step3.������CAN��AEBS��������׼��
		AEB_CAN_TxBuf[0] = CrashWarningLv;			//5, 6, 7��Ӧ�����������ȼ�
		if(TimetoCrash_g<0) tempTTC=0;
		else if(TimetoCrash_g>9.9f) tempTTC=9.9f*20;
		else tempTTC = TimetoCrash_g*20;
		AEB_CAN_TxBuf[2] = tempTTC;
		AEB_CAN_TxBuf[3] = (uint8_t)RadarObject.MinRangeLong;
		AEB_CAN_TxBuf[4] = (uint8_t)(RadarObject.VrelLong * 3.6f);
		AEB_CAN_TxBuf[5] = (uint8_t)vehicle.speed; 
		AEB_CAN_TxBuf[6] = ADAS_dev.crash_level;
		//��ʱ�жϷ���//HAL_CAN_AddTxMessage(&hcan2, &CAN_TxAEBHeader, AEB_CAN_TxBuf, &AEB_CAN_TxMailBox);
		
		osDelay(50);
  }
  /* USER CODE END StartPrepareCANDataTask */
}

/* StartRadarCalcTask function */
void StartRadarCalcTask(void const * argument)
{
  /* USER CODE BEGIN StartRadarCalcTask */
  /* Infinite loop */
  for(;;)
  {
		uint8_t AEBS_Deal=0;
    osSemaphoreWait(bSemRadarCalcSigHandle, osWaitForever);

		MW_RadarObjStatus RadarObjStatus;
		#if RADAR_TYPE == ARS408
		if(RadarCAN_RxHeader.StdId==0x60A)												//read all messages before 60B, start calculate
		{
			minRadarDistFlag = 1;	//��һ��60B�����Ǿ��������Ŀ��
			ARS_GetRadarObjStatus(RadarCANRxBuf, &RadarObjStatus);
			if(RadarObjStatus.Obj_NofObjects == 0)	//Ŀ������Ϊ0
			{
				CrashWarningLv = WARNING_NONE;
				TimetoCrash_g=20;	//����CAN��
				RadarObject.MinRangeLong=255;//����CAN��
				StopBeeper(&vAEBS_Status);
				DisableAEBS(&vAEBS_Status);
			}
		}
		else if(RadarCAN_RxHeader.StdId == 0x60B)
		{
			AEBS_Deal=0;
			if(vehicle.speed > 5)//���ٱ�����ֵ,�����ֵ̫���ƶ����ڵ���ʱ�����ƶ�
			{
				if(minRadarDistFlag)		//��������Ŀ��
			{
				minRadarDistFlag = 0;			//���Ŀ��״̬λ����
				//step1.���͵�CAN1����
				uint16_t dist = 0;
				uint8_t temp;
				dist = (uint16_t)(((*(RadarCANRxBuf+1))<<5) | ((*(RadarCANRxBuf+2))>>3));
				dist -= ((vehicle.speed / 22)-0.0)*5;		
				*(RadarCANRxBuf + 1) =(dist>>5);
				temp =((dist<<3)&0xF8);
				*(RadarCANRxBuf + 2) &=0x07;
				*(RadarCANRxBuf + 2) |=temp;
				//��ʱ������DBC������
				//HAL_CAN_AddTxMessage(&hcan1, &DBC_CAN_TxHeader, RadarCANRxBuf, &DBC_CAN_TxMailBox);
			
				//step2.������½�״�����
				ARS_GetRadarObjGeneral(RadarCANRxBuf, RadarGeneral);//get closet obj data
				uint16_t MinRange=RadarGeneral[0].Obj_DistLong;
				uint32_t relSpeed=RadarGeneral[0].Obj_VrelLong;
				
				//if(!Turning_Flag || (Turning_Flag && Turning_Collision))
				//{
					if((0.2*MinRange-500) < LIMIT_RANGE && MinRange != 0)	//calculate when dist is near enough
					{
						RadarObject.VrelLong =  0.25*relSpeed - 128;									//get real relative speed
						if(RadarObject.VrelLong<0)
						{
							RadarObject.MinRangeLong = 0.2 * MinRange - 500;						//get real range(longitude)
							if(RadarObject.MinRangeLong>0)
							{
								AEBS_Deal=1;			//�����˱���
								TimetoCrash_g = -(float)RadarObject.MinRangeLong/RadarObject.VrelLong;//relative Velocity is minus
								if(TimetoCrash_g < HIGH_WARNING_TIME)
								{
									CrashWarningLv = WARNING_HIGH;
									#if ADAS_COMM
									if(ADAS_dev.crash_level > 0)
									{
									#endif
										StartBeeper(WARNING_HIGH);
										EnableAEBS(TimetoCrash_g, WARNING_HIGH);
									#if ADAS_COMM
									}
									#endif
								}
								else if(TimetoCrash_g < MID_WARNING_TIME)
								{
									CrashWarningLv = WARNING_MID;
									#if ADAS_COMM
									if(ADAS_dev.crash_level > 0)
									{
									#endif
										StartBeeper(WARNING_MID);
										EnableAEBS(TimetoCrash_g, WARNING_MID);
									#if ADAS_COMM
									}
									#endif
								}
								else if(TimetoCrash_g < LOW_WARNING_TIME)
								{
									CrashWarningLv = WARNING_LOW;
									#if ADAS_COMM
									if(ADAS_dev.crash_level > 0)
									{
									#endif
										StartBeeper(WARNING_LOW);
										DisableAEBS(&vAEBS_Status);	
									#if ADAS_COMM
									}
									#endif
								}
							else
							{							
								 AEBS_Deal=0;
							}
						}	
						else AEBS_Deal=0;
					}
					else AEBS_Deal=0;
				}
					else AEBS_Deal=0;
			}
		if(AEBS_Deal==0)//û�д����˱���
				{
						CrashWarningLv = WARNING_NONE;
						TimetoCrash_g=20;	//����CAN��
						RadarObject.MinRangeLong=255;//����CAN��	
					  RadarObject.VrelLong=0;
						StopBeeper(&vAEBS_Status);
						DisableAEBS(&vAEBS_Status);
				}
		}
			if(AEBS_Deal==0)//û�д����˱���
				{
						CrashWarningLv = WARNING_NONE;
						TimetoCrash_g=20;	//����CAN��
						RadarObject.MinRangeLong=255;//����CAN��	
					  RadarObject.VrelLong=0;
						StopBeeper(&vAEBS_Status);
						DisableAEBS(&vAEBS_Status);
				}	
		osSemaphoreRelease(bSemPrepareCanDataSigHandle);			//�����״�׼�������ź���
		#else
		HAL_GPIO_TogglePin(LED1_GPIO_Port,LED1_Pin);
		MinRangeLong_g = EMRRGeneral_Closet.trackRange;// - VehicleSpeed_g / 6;
    VrelLong_g = EMRRGeneral_Closet.trackSpeed;
    if(!Turning_Flag || (Turning_Flag && Turning_Collision))
    {
			if(MinRangeLong_g < LIMIT_RANGE && MinRangeLong_g > 0 && VrelLong_g < 0)
      {
        TimetoCrash_g = - MinRangeLong_g / VrelLong_g;
        if(TimetoCrash_g < HIGH_WARNING_TIME)
        {
          CrashWarningLv = WARNING_HIGH;
          osSemaphoreRelease(bSemPrepareCanDataSigHandle);
        }
        else if(TimetoCrash_g < LOW_WARNING_TIME)
        {
          CrashWarningLv = WARNING_LOW;
          //osSemaphoreRelease(bSemPrepareCanDataSigHandle);
        }
        else
          CrashWarningLv = WARNING_NONE;
      }
    }
		#endif
	}
		osDelay(10);
  }
  /* USER CODE END StartRadarCalcTask */
}
	


/* USER CODE BEGIN Application */
     
/* USER CODE END Application */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/