/**************************************************************************//**
 * @file     zdc_common.h
 * @version  V1.0
 * $Revision: 10 $
 * @Author:   Jackie Guo
 * $Date:     2022/06/16 18:00 $
 * @brief    ZD-Converter 1000 common header file. 
 *
 * @copyright (C) 2022 ZD-Automotive GmbH. All rights reserved.
 ******************************************************************************/

#ifndef __ZDC_COMMON_H
#define __ZDC_COMMON_H

#ifdef __cplusplus
extern "C" {
#endif

/***************************** Include Files ********************************/
#include <stdio.h>
#include "NuMicro.h"

/************************** Constant Definitions ****************************/


typedef struct
{
    /* program modes and line control */
	  int32_t prog_enable;        /* manual select, 1:manual, 0:program */
		int32_t line_enable;	/* Line control select, 1:OFF, 0:ON */
		int32_t trigger_enable;	/* trigger signal, 1:disaktive, 0:aktive */
		
} zdc_prog_t;





typedef struct
{
	int32_t line_ctrl_flag;
	int32_t program_ctrl_flag;
		
}test_loop_t;

//DIP Switch
#define PROGRAM_CTRL_BIT	PB5
#define PROGRAM_CTRL		PB,BIT5

#define LINE_CTRL_BIT		PB4//use to control trigger enable
#define LINE_CTRL			PB,BIT4

#define TRIGGER_CTRL_BIT	PF3
#define TRIGGER_CTRL		PF,BIT3

#define COIL_1_CTRL_BIT		PA3
#define COIL_1_CTRL			PA,BIT3

#define COIL_2_CTRL_BIT		PA2
#define COIL_2_CTRL			PA,BIT2

#define COIL_3_CTRL_BIT		PA1
#define COIL_3_CTRL			PA,BIT1

#define COIL_4_CTRL_BIT		PA0
#define COIL_4_CTRL			PA,BIT0

//LED
#define PROGRAM_CTRL_LED_BIT	PB2
#define PROGRAM_CTRL_LED		PB,BIT2//LOW AKTIVE

#define LINE_CTRL_LED_BIT		PB3
#define LINE_CTRL_LED			PB,BIT3






/* DIP and LED parameters */
#define LED_AKTIVE       		1
#define LED_DISAKTIVE         0    

#define AKTIVE       		1
#define DISAKTIVE         0  

#define ZDG_PROG_MODE          0     /* set program mode */
#define ZDG_MANUAL_MODE        1     /* set manual mode */

#define ZDG_LINE_ON          0     /* LINE MODE ON */
#define ZDG_LINE_OFF        1     /* LINE MODE OFF  */


#define TRIGGER_AKTIVE          0     /* LINE MODE ON */
#define TRIGGER_DISAKTIVE        1     /* LINE MODE OFF  */


/**************************** Type Definitions ******************************/
#if 0
typedef struct
{
	  /* GPIO state */
	  int gpio_legacy_mode;      /* 0: legacy;1: auto/compliant */
	  int gpio_speed_control;    /* 0: 100M; 1: 1000M */
	
	  /* 1000Base-T1 mode */
	  int t1_phy_detect_done;     /* 0:auto; 1:done */
	  int t1_phy_mode;            /* 0:compliant; 1:legacy */

	  /* phy speed */
	  int rj45_phy_speed;         /* 100; 1000 */
	
	  /* PHY type chipset */
	  int rj45_phy_type;          /* 1:88e1510; 2:YT8521*/
	
	  /* PHY ID */
	  int phyid_rj45;             /* RJ45 PHY ID */
	  int phyid_brr;              /* BRR PHY ID */
} zdc_hw_config_t;

#endif
/***************** Macros (Inline Functions) Definitions ********************/



/************************** Variable Definitions ****************************/



/************************** Function Prototypes *****************************/
void SYS_Init(void);
void SYS_Delay(int ms);
void UART0_Init(void);
void SYS_Disable_AnalogPORCircuit(void);	

void zdc_GPIO_Init(void);

#if 0
void zdc_config_init(zdc_hw_config_t *hw_config);
void	zdc_gpio_detect(zdc_hw_config_t *hw_config);
void	Legacy_Mode_Detect(zdc_hw_config_t *hw_config);
void	Speed_Ctrl_Detect(zdc_hw_config_t *hw_config);
void PowerDownFunction(void);	
#endif
#ifdef __cplusplus
}
#endif

#endif /*__ZDC_COMMON_H */

