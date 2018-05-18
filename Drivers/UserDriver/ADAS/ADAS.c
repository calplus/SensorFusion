#include "ADAS.h"

uint8_t CalADASData(ADAS_HandleTypeDef *pADAS_dev, uint8_t *pRxBuf)
{
	if(*pRxBuf==0x4B&&*(pRxBuf+1)==0x59&&*(pRxBuf+2)==0x4B&&*(pRxBuf+3)==0x4A&&*(pRxBuf+4)==0x77)
	{
		//for(j=0;j<23;j++)	UART3_TX_BUF[j]=UART2_RX_BUF[j+i];
		//HAL_UART_Transmit(&huart3, UART3_TX_BUF, 23, 1000);		//���ݱ����źŸ�����
		pADAS_dev->DEV_work_status=(*(pRxBuf+6)&0x80)>>7;
		pADAS_dev->LDW_work_status=(*(pRxBuf+6)&0x40)>>6;
		pADAS_dev->FCW_work_status=(*(pRxBuf+6)&0x20)>>5;
		pADAS_dev->PCW_work_status=(*(pRxBuf+6)&0x10)>>4;		//PCW����״̬
		pADAS_dev->DFW_work_status=(*(pRxBuf+6)&0x08)>>3;		//DFW����״̬
		pADAS_dev->self_check_main=(*(pRxBuf+6)&0x01)>>0;
		
		pADAS_dev->LDW_warning=(*(pRxBuf+7)&0xC0)>>6;		//LDW����ƫ�� 0x00������0x01��ƫ�� 0x02��ƫ�� 0x03����			
		pADAS_dev->crash_type=(*(pRxBuf+7)&0x20)>>5;		//0x00 FCW ��0x01 PCW
		pADAS_dev->crash_level=(*(pRxBuf+7)&0x18)>>3;		//0x00���� 0x01һ��Ԥ�� 0x02����Ԥ�� 0x03����Ԥ��
		pADAS_dev->driver_status=(*(pRxBuf+7)&0x07);			//0x000���� 0x001ƣ�� 0x002��̬�쳣 0x003��绰 0x004����															
		
		pADAS_dev->distance_left_line=*(pRxBuf+8);			//���󳵵��߾���
		pADAS_dev->distance_right_line=*(pRxBuf+9);			//���ҳ����߾���
		
		pADAS_dev->line_circle_diameter=*(pRxBuf+10);		//��������ֱ��		
		
		pADAS_dev->line_width=(*(pRxBuf+11)&0xF8)>>3;		//�����߿��
		pADAS_dev->line_type=(*(pRxBuf+11)&0x07);				//����������														
										
		pADAS_dev->direction_angle=*(pRxBuf+12);				//��ʻ�����복���߼н�
		
		pADAS_dev->distance_to_target=*(pRxBuf+13); 		//��ǰ��Ŀ�����
		pADAS_dev->speed_to_target=*(pRxBuf+14);				//��ǰ��Ŀ����Գ���
		pADAS_dev->time_to_target=0.05**(pRxBuf+15);		//��ǰ��Ŀ��Ԥ��ײʱ��
		
//����Э�飬���
//LDW״̬��ȡ//////////////////////////////////////////////////////////////	
		switch(pADAS_dev->LDW_warning)
		{
			case 0x00:
				//��ʾ�޳���ƫ��ͼƬ
				break;
			case 0x01:
				//��ʾ�󳵵�ƫ��ͼƬ
				break;
			case 0x02:
				//��ʾ�ҳ���ƫ��ͼƬ
				break;
			default:
				//��ʾ�޳���ƫ��ͼƬ
				break;
		}
//��ײǿ�Ȼ�ȡ//////////////////////////////////////////////////////////////						
		switch(pADAS_dev->crash_level)
		{
			case 0x00:
				//��ʾ��������ײ����ͼƬ
				//��ʾ�޳�����ײ����ͼƬ
				break;
			case 0x01:
				//��ʾ��������ײ����ͼƬ
				//��ʾ�޳�����ײ����ͼƬ
				break;
			case 0x02:
				if(pADAS_dev->crash_type==0x00) 
				{
					//��ʾ�еȳ�����ײ����ͼƬ
					//����е�������ײ����ͼƬ
				}
				else
				{
					//����еȳ�����ײ����ͼƬ
					//��ʾ�е�������ײ����ͼƬ
				}
				
				break;
			case 0x03:
				if(pADAS_dev->crash_type==0x00) 
				{
					//��ʾ���س�����ײ����ͼƬ
					//�������������ײ����ͼƬ
				}
				else
				{
					//������س�����ײ����ͼƬ
					//��ʾ����������ײ����ͼƬ
				}
				break;
			default:
				//��ʾ��������ײ����ͼƬ
				//��ʾ�޳�����ײ����ͼƬ
				break;
		}
//ƣ��״̬��ȡ//////////////////////////////////////////////////////////////					
		switch(pADAS_dev->driver_status)
		{
			case 0x000:
				//Display_Face(0);
				break;
			case 0x001:
				//Display_Face(1);
				
				break;
			case 0x002:
				//Display_Face(1);
				
				break;
			case 0x003:
				//Display_Face(1);
				
				break;
			case 0x004:
				//Display_Face(1);
				
				break;
			default:
				//Display_Face(0);
				break;
		}
		return 0;
	}
	return 1;
}
