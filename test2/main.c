/**************************************************************************//**
 * @file     main.c
 * @version  V1.00
 * $Revision: 10 $
 * @Author:   Jackie Guo
 * $Date:     2022/06/16 18:00 $
 * @brief    Show how to set GPIO pin mode and use pin data input/output control.
 *
 * @copyright (C) 2022 ZD-Automotive GmbH. All rights reserved.
 * @history
 *      2022-06-16 ver1.2, support YT8521SH phy instead of Marvell 88W1510
 ******************************************************************************/
#include "stdio.h"
#include <string.h>
#include "NuMicro.h"

#include "zdc_common.h"
#include "zdc_version.h"
#include "zdc_program.h"
#include "zdc_function.h"


int32_t gZDC_debug = 1;       /* debug ,0-debug off, 1-debug on*/
zdc_prog_t gZDG_hw_config = {1};     /* global data */


/* program */
extern void UARTCOM_Init(void);
extern void UARTCOM_INT_Enable(void);



/*=======================================================================*/

/* ------------- */
/* Main function */
/* ------------- */
int32_t main(void)
{
	  

    /* Unlock protected registers */
    SYS_UnlockReg();
    /* Init System, IP clock and multi-function I/O. */
    SYS_Init();
    /* Lock protected registers */
    SYS_LockReg();

    /* Configure UART0: 115200, 8-bit word, no parity bit, 1 stop bit. */
    UARTCOM_Init();
	
 	 // printf("\n\nCPU @ %dMHz\n\n", SystemCoreClock/1000000);
		
	
#if defined(_ZD_GRABER_LINK_)
    printf("+----------------------------------------------------------------------+\n");
    printf("|                 ZD-Graber-link Configuration                       |\n");
    printf("|            Copyright(c) ZD Automotive GmbH 2012-2022                 |\n");		
    printf("+----------------------------------------------------------------------+\n");	
#endif	
			
		printf("Firmware Version: V%d.%d (%s %s)(Debug %s)\n\n", 
		    ZDC_VERSION_MAJOR, ZDC_VERSION_MINOR, ZDC_VERSION_DATE, ZDC_VERSION_TIME,	gZDC_debug?"ON":"OFF");
		
				
		/* Configure GPIO */
		zdc_GPIO_Init();
		

		/* check board DIP and init*/
		check_board(&gZDG_hw_config);

		/* now enable uart program interrupt */
		UARTCOM_INT_Enable();  
		
		
		/* line control mode*/
		Line_control_mode(gZDG_hw_config);
		
		while(1);
				
}

void SYS_Init(void)
{
    /* Unlock protected registers */
    SYS_UnlockReg();

    /* Enable HIRC */
    CLK_EnableXtalRC(CLK_PWRCTL_HIRCEN_Msk);

    /* Waiting for HIRC clock ready */
    CLK_WaitClockReady(CLK_STATUS_HIRCSTB_Msk);

    /* Switch HCLK clock source to HIRC */
    CLK_SetHCLK(CLK_CLKSEL0_HCLKSEL_HIRC, CLK_CLKDIV0_HCLK(1));

    /* Set both PCLK0 and PCLK1 as HCLK/2 */
    CLK->PCLKDIV = (CLK_PCLKDIV_APB0DIV_DIV2 | CLK_PCLKDIV_APB1DIV_DIV2);

    /* Switch UART0 clock source to HIRC */
    CLK_SetModuleClock(UART0_MODULE, CLK_CLKSEL1_UART0SEL_HIRC, CLK_CLKDIV0_UART0(1));

    /* Enable UART peripheral clock */
    CLK_EnableModuleClock(UART0_MODULE);
//BIN ADD
    /* Enable IP clock */
    CLK_EnableModuleClock(TMR0_MODULE);

    /* Select IP clock source */
    CLK_SetModuleClock(TMR0_MODULE, CLK_CLKSEL1_TMR0SEL_HIRC, 0);
//BIN ADD
    
		/* Update System Core Clock */
    /* User can use SystemCoreClockUpdate() to calculate PllClock, SystemCoreClock and CycylesPerUs automatically. */
    SystemCoreClockUpdate();

    /*----------------------------------------------------------------------*/
    /* Init I/O Multi-function                                              */
    /*----------------------------------------------------------------------*/
    /* Set GPB multi-function pins for UART0 RXD and TXD */
    SYS->GPB_MFPH = (SYS->GPB_MFPH & ~(SYS_GPB_MFPH_PB12MFP_Msk | SYS_GPB_MFPH_PB13MFP_Msk)) |
                    (SYS_GPB_MFPH_PB12MFP_UART0_RXD | SYS_GPB_MFPH_PB13MFP_UART0_TXD);

    /* Lock protected registers */
    SYS_LockReg();
}



void SYS_Delay(int ms)
{
    int i=0, j=0;
	
		for( i=ms; i>0; i-- ) 
		{
			for( j=10000; j>0; j--)
			{
				//
			}
    }	
}

void zdc_GPIO_Init(void)
{
    GPIO_SetMode(COIL_1_CTRL, GPIO_MODE_OUTPUT);	
    GPIO_SetMode(COIL_2_CTRL, GPIO_MODE_OUTPUT);
    GPIO_SetMode(COIL_3_CTRL, GPIO_MODE_OUTPUT);	
    GPIO_SetMode(COIL_4_CTRL, GPIO_MODE_OUTPUT);
	
    GPIO_SetMode(PROGRAM_CTRL, GPIO_MODE_INPUT);	
    GPIO_SetMode(LINE_CTRL, GPIO_MODE_INPUT);	
    GPIO_SetMode(TRIGGER_CTRL, GPIO_MODE_INPUT);

	
	//LED init
	GPIO_SetMode(PROGRAM_CTRL_LED, GPIO_MODE_OUTPUT);
    GPIO_SetMode(LINE_CTRL_LED, GPIO_MODE_OUTPUT);	
}	



//power down related:
/*---------------------------------------------------------------------------------------------------------*/
/*  Function for diasble internal analog POR circuit                                                       */
/*---------------------------------------------------------------------------------------------------------*/
void SYS_Disable_AnalogPORCircuit(void)
{
    SYS->PORDISAN = 0x5AA5;
}

/*---------------------------------------------------------------------------------------------------------*/
/*  Function for System Entry to Power Down Mode                                                           */
/*---------------------------------------------------------------------------------------------------------*/

#if 0

void PowerDownFunction(void)
{
		//enter powndown mode
		/* Check if all the debug messages are finished */
    UART_WAIT_TX_EMPTY(UART0);

    /* Configure all GPIO as Quasi-bidirectional Mode*/
    GPIO_SetMode(PA, GPIO_P0_TO_P15, GPIO_MODE_QUASI);
    GPIO_SetMode(PB, GPIO_P0_TO_P15, GPIO_MODE_QUASI);
    //GPIO_SetMode(PC, GPIO_P0_TO_P15, GPIO_MODE_QUASI);
    //GPIO_SetMode(PD, GPIO_P0_TO_P15, GPIO_MODE_QUASI);
    //GPIO_SetMode(PE, GPIO_P0_TO_P15, GPIO_MODE_QUASI);
    GPIO_SetMode(PF, GPIO_P0_TO_P15, GPIO_MODE_QUASI);

    /* Configure PA.15 as Quasi-bidirectional mode and enable interrupt by falling edge trigger */
    //GPIO_SetMode(PA, BIT15, GPIO_MODE_QUASI);
    //GPIO_EnableInt(PA, 15, GPIO_INT_FALLING);
    //NVIC_EnableIRQ(GPIO_PAPBPGPH_IRQn);

    /* Enable interrupt de-bounce function and select de-bounce sampling cycle time is 1024 clocks of LIRC clock */
    //GPIO_SET_DEBOUNCE_TIME(GPIO_DBCTL_DBCLKSRC_LIRC, GPIO_DBCTL_DBCLKSEL_1024);
    //GPIO_ENABLE_DEBOUNCE(PA, BIT15);

    /* Unlock protected registers before entering Power-down mode */
    SYS_UnlockReg();

    /* LVR must be enabled and the POR will be enabled automatically */
    SYS_ENABLE_LVR();
    /* Turn off internal analog POR circuit */
    SYS_Disable_AnalogPORCircuit();
    /* Disable Power-on Reset */
    SYS_DISABLE_POR();

		printf("\nMCU enter into Power-Down ......\n");
		
		/* Check if all the debug messages are finished */
    UART_WAIT_TX_EMPTY(UART0);

    /* Enter to Power-down mode */
    CLK_PowerDown();
}



#endif
