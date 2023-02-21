/**************************************************************************//**
 * @file     zdc_program.c
 * @version  V1.0
 * $Revision: 10 $
 * @Author:   Jackie Guo,jiaming
 * $Date:     2022/11/30 12:00 $
 * @brief    ZD-Converter 1000H UART Program commands. 
 *
 * @copyright (C) 2022 ZD-Automotive GmbH. All rights reserved.
 ******************************************************************************/
 #include <stdio.h>
#include <string.h>
#include "NuMicro.h"
#include "zdc_common.h"

extern int32_t gZDC_debug;
extern void SYS_Delay(int ms);


 /*   read GPIO pin ,By jiaming,2022.11.10   */ 
  /*     GPIO PIN read,By jiaming,2022.11.10   */ 
typedef enum
{
  GPIO_PIN_RESET = 0,
  GPIO_PIN_SET = 1,
	
}GPIO_PinState;



GPIO_PinState GPIO_ReadPin(GPIO_T *port, uint32_t GPIO_Pin)

{
	GPIO_PinState bitstatus;
	
	int GPIO_cnt=0;
	int i=0;
	
	//GPIO_PIN_RESET = 0
	//GPIO_PIN_SET = 1
  /* Check the parameters */
  //assert_param(IS_GPIO_PIN(GPIO_Pin));
	
	for(i=0; i<20; i++)
		{
		  if((port->PIN & GPIO_Pin) != (uint32_t)GPIO_PIN_RESET)  GPIO_cnt++;	
		//	SYS_Delay(1);		
		}

		bitstatus = GPIO_PIN_RESET;
  
		if( GPIO_cnt >= 10 )       bitstatus = GPIO_PIN_SET;		 //read ten times
  
  return bitstatus;
  
}	



void init_board_set(void)//init all 
{
COIL_1_CTRL_BIT = AKTIVE;
COIL_2_CTRL_BIT = AKTIVE;	
COIL_3_CTRL_BIT = AKTIVE;	
COIL_4_CTRL_BIT = AKTIVE;
	
PROGRAM_CTRL_LED_BIT = 	LED_DISAKTIVE;
LINE_CTRL_LED_BIT = LED_DISAKTIVE;	
	
}	

void check_board(zdc_prog_t *gZDG_config)
{

	
	if (PROGRAM_CTRL_BIT == 0)//aktive
	{
		printf("Detect Program mode DIP Switch: ON\r\n");
		PROGRAM_CTRL_LED_BIT=LED_AKTIVE;
		gZDG_config->prog_enable=ZDG_PROG_MODE;
	}
	else if (PROGRAM_CTRL_BIT == 1)//inaktive
	{
		printf("Detect Program mode DIP Switch: OFF\r\n");
		PROGRAM_CTRL_LED_BIT=LED_DISAKTIVE;
		gZDG_config->prog_enable=ZDG_MANUAL_MODE;
	}
	
		
	if (LINE_CTRL_BIT == 0)//aktive
	{
		printf("Detect Line control mode DIP Switch: ON\r\n");
		LINE_CTRL_LED_BIT=LED_AKTIVE;
		gZDG_config->line_enable=ZDG_LINE_ON;

			if (TRIGGER_CTRL_BIT == 0) 	gZDG_config->trigger_enable=TRIGGER_AKTIVE;
			else if (TRIGGER_CTRL_BIT == 1) 	gZDG_config->trigger_enable=TRIGGER_DISAKTIVE;
		
	}
	else if (LINE_CTRL_BIT == 1)//inaktive
	{
		printf("Detect Line control mode DIP Switch: OFF\r\n");
		LINE_CTRL_LED_BIT=LED_DISAKTIVE;
		gZDG_config->line_enable=ZDG_LINE_OFF;
	}
	

}



void LINE_CTRL_loop (GPIO_T *port, uint32_t GPIO_Pin ,int SWITCH_CTRL_BIT, int* SWITCH_FLAG)
	
{

	int32_t flag;
	int current_status;
	
	current_status=GPIO_ReadPin(port,GPIO_Pin);
	flag=* SWITCH_FLAG;
	
	if (current_status != flag)
	{
					
		if (SWITCH_CTRL_BIT == 0)//low ,DIP aktive
			{			
			printf("\r\nDetect Line DIP Switch: ON\r\n");
			printf("\r\nDPlease connect to the line control signal\r\n");	
			*SWITCH_FLAG = current_status;	

			if (TRIGGER_CTRL_BIT == 0)//trigger aktive ,SOS!
				{
			COIL_1_CTRL_BIT = DISAKTIVE;
			COIL_2_CTRL_BIT = DISAKTIVE;	
			COIL_3_CTRL_BIT = DISAKTIVE;	
			COIL_4_CTRL_BIT = DISAKTIVE;
				}
		//else 
				//{
			//COIL_1_CTRL_BIT = AKTIVE;
			//COIL_2_CTRL_BIT = AKTIVE;	
			//COIL_3_CTRL_BIT = AKTIVE;	
			//COIL_4_CTRL_BIT = AKTIVE;
				//}		
				
			}
			
		else if (SWITCH_CTRL_BIT == 1)//high ,DIP disaktive
		
			{
			printf("\r\nDetect Line DIP Switch: ON: OFF\r\n");
			*SWITCH_FLAG = current_status;		
			}	
			
	}

}


void TRIGGER_CTRL_loop (GPIO_T *port, uint32_t GPIO_Pin ,int SWITCH_CTRL_BIT, int* SWITCH_FLAG)
	
{

	int32_t flag;
	int current_status;
	
	current_status=GPIO_ReadPin(port,GPIO_Pin);
	flag=* SWITCH_FLAG;
	
	if (current_status != flag)
	{			
		if (TRIGGER_CTRL_BIT == 0)//trigger aktive
				{
				COIL_1_CTRL_BIT = AKTIVE;
				COIL_2_CTRL_BIT = AKTIVE;	
				COIL_3_CTRL_BIT = AKTIVE;	
				COIL_4_CTRL_BIT = AKTIVE;
				*SWITCH_FLAG = current_status;
					
				printf("\r\n[All Relays enable]\r\n");
				}
		else if (TRIGGER_CTRL_BIT == 1)//trigger disaktive
		{
			
				COIL_1_CTRL_BIT = DISAKTIVE;
				COIL_2_CTRL_BIT = DISAKTIVE;	
				COIL_3_CTRL_BIT = DISAKTIVE;	
				COIL_4_CTRL_BIT = DISAKTIVE;
				*SWITCH_FLAG = current_status;
			printf("\r\n[All Relays disable]\r\n");
		}
		
	}

}


void Line_control_mode(zdc_prog_t gZDG_config)

{
	int status;
	status=gZDG_config.line_enable;		
	
	if (status == ZDG_LINE_ON )		
	{		
		
		printf("\r\nWaiting for the Trigger signal...\r\n");
		while (1) 
		{
		TRIGGER_CTRL_loop (TRIGGER_CTRL ,TRIGGER_CTRL_BIT,&gZDG_config.line_enable);
		}		
	}
	#if 0
	else 
	{
		while (1)
		//{
		//	COIL_1_CTRL_BIT = AKTIVE;
		//	COIL_2_CTRL_BIT = AKTIVE;	
		//	COIL_3_CTRL_BIT = AKTIVE;	
		//	COIL_4_CTRL_BIT = AKTIVE;
		//}			
	}	
#endif	
			
}
