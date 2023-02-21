/**************************************************************************//**
 * @file     zdc_phy.c
 * @version  V1.0
 * $Revision: 10 $
 * @Author:   Jackie Guo
 * $Date:     2022/06/16 18:00 $
 * @brief    ZD-Converter 1000 ethernet phy driver file. 
 *
 * @copyright (C) 2022 ZD-Automotive GmbH. All rights reserved.
 ******************************************************************************/


/***************************** Include Files ********************************/

#include "zdc_common.h"
#include "zdc_phy.h"


extern int32_t gZDC_debug;


/*  WORKAROUND for BRR PORT PHY ID Error ISSUE!!!
 * zdc_phyid_check():
 *   detect brr phy id 2 or 3
 */
void zdc_phyid_verify(zdc_hw_config_t *hw_config)
{
    uint16_t PhyData=0;
    int PhyAddr=0;

		/* Display MDIO PHY ID info */
    if( gZDC_debug )		
        zdc_phyid_test(hw_config);
		
    /* check phy id */	
    PhyAddr = hw_config->phyid_brr;        /* phyid to 2 */
    PhyData = 0xFFFF;
    mdio_PhyRead(PhyAddr, PHY_REG0_OFFSET, &PhyData);

    /* phy id ok */
    if( PhyData != 0xFFFF )
		{
        printf("INFO: BRR ethernet PHY id %d check OK\n\n", hw_config->phyid_brr);
        return;
		}
				
    /* check phy id */	
    PhyAddr = hw_config->phyid_brr | 0x01;  /* set phyid to 3 */
    PhyData = 0xFFFF;
    mdio_PhyRead(PhyAddr, PHY_REG0_OFFSET, &PhyData);
				
    /* new phy id ok */
    if( PhyData != 0xFFFF )
    {
         printf("WARNING: BRR ethernet PHY id change from %d to %d\n\n", hw_config->phyid_brr, PhyAddr);
				 hw_config->phyid_brr = PhyAddr;
				 return;
		}
				
    /* reset system */
    printf("ERROR: BRR ethernet PHY ID %d & %d check error!\n\n", hw_config->phyid_brr, PhyAddr);
    SYS_Delay(1000);
}


/* zdc_phyid_test():
 *   print out all 32 phy id registers
 */
void zdc_phyid_test(zdc_hw_config_t *hw_config)
{
    uint16_t PhyData=0;
    int PhyAddr=0;
				
 	  /*
	   * Print out all phy id (1-7) registers.
	   */
    printf("\n---------------------------- MDIO Registers -------------------------------\n");

    for( PhyAddr=1; PhyAddr<8; PhyAddr++ ) {
			
			      PhyData = 0xFFFF;
			      mdio_PhyRead(PhyAddr, PHY_REG0_OFFSET, &PhyData);
			      printf("(phy %2d): [00]%04X ", PhyAddr, PhyData);
	
			      PhyData = 0xFFFF;
			      mdio_PhyRead(PhyAddr, PHY_REG1_OFFSET, &PhyData);
			      printf("[01]%04X ", PhyData);

			      PhyData = 0xFFFF;
			      mdio_PhyRead(PhyAddr, PHY_REG2_OFFSET, &PhyData);
			      printf("[02]%04X ", PhyData);
			
		      	PhyData = 0xFFFF;
			      mdio_PhyRead(PhyAddr, PHY_REG4_OFFSET,	&PhyData);
			      printf("[04]%04X ", PhyData);
	
			      PhyData = 0xFFFF;
			      mdio_PhyRead(PhyAddr, PHY_REG5_OFFSET,	&PhyData);
			      printf("[05]%04X ", PhyData);
	
			      PhyData = 0xFFFF;
			      mdio_PhyRead(PhyAddr, PHY_REG9_OFFSET, &PhyData);
			      printf("[09]%04X ", PhyData);
						
			      PhyData = 0xFFFF;
			      mdio_PhyRead(PhyAddr, PHY_REG10_OFFSET, &PhyData);
			      printf("[10]=%04X\n", PhyData);						
    }		
		
    printf("\n---------------------------- XMDIO Registers ------------------------------\n");
    for( PhyAddr=1; PhyAddr<8; PhyAddr++ ) 
		{
			
			      PhyData = 0xFFFF;
			      xmdio_PhyRead(PCS_CONTROL_DEVICE, PhyAddr, PHY_REG0_OFFSET, &PhyData);
			      printf("(phy %2d): [00]%04X ", PhyAddr, PhyData);

			      PhyData = 0xFFFF;
			      xmdio_PhyRead(PCS_CONTROL_DEVICE, PhyAddr, PHY_REG1_OFFSET, &PhyData);
			      printf("[01]%04X ", PhyData);

			      PhyData = 0xFFFF;
			      xmdio_PhyRead(PCS_CONTROL_DEVICE, PhyAddr, PHY_REG2_OFFSET, &PhyData);
			      printf("[02]%04X ", PhyData);
			
			      PhyData = 0xFFFF;
			      xmdio_PhyRead(PCS_CONTROL_DEVICE, PhyAddr, PHY_REG8_OFFSET, &PhyData);
			      printf("[08]%04X ", PhyData);

			      PhyData = 0xFFFF;
			      xmdio_PhyRead(PCS_CONTROL_DEVICE, PhyAddr, PHY_REG33_OFFSET, &PhyData);
			      printf("[33]%04X ", PhyData);					
	
			      PhyData = 0xFFFF;
			      xmdio_PhyRead(PCS_CONTROL_DEVICE, PhyAddr, PCS_1000BASET1_LINK_STATUS_REG, &PhyData);
			      printf("[901h]%04X\n", PhyData);
	  }  		
 
    printf("****************************************************************\n\n");
}

/* zdc_phy_detect():
 *   print out system ethernet phy status
 */
void zdc_phy_detect(zdc_hw_config_t *hw_config)
{
    uint16_t PhyAddr=0, PhyData=0;
		int16_t tmpData=0;
    char roleStr[3][8]={" Auto ","Master","Slave "};

	  /*
	   * Verify all MDIO ports.
	   */
    printf("\n------------------------------------ Link Status ---------------------------------\n");

		PhyAddr = hw_config->phyid_rj45;
		/*================== ETH: phy id 1 ================*/
		/* if Marvell 88E1510 PHY, reset page id to 0 */
	  if( hw_config->rj45_phy_type == RJ45_PHY_TYPE_MARVELL)
		{
				//change reg22 to page 0
				mdio_PhyWrite(PhyAddr,22,0);
		}			
		
    printf("ETH(1000Base-T )");

    /* master/slave */
    printf("    %s", roleStr[0]);  /* always auto for GE */

    /* set speed */
	  if( hw_config->rj45_phy_speed == ETH_PHY_SPEED_1000M )  /* set 1000M */
		{
        printf("    1000Mbps    ");							
		}
		else
		{
        printf("     100Mbps    ");							
		}
					
     /* link status */
     if( !get_linkup_stdphy(PhyAddr, &tmpData) )
		 {	
	      if( tmpData ) {
			              printf("*Up*");	/* Found a valid PHY device */
        }
        else {
			              printf("Down");	/* Found a valid PHY device */
        }
        get_speed_stdphy(PhyAddr, &tmpData, 1);  /* auto-nego */
  	    printf(" (link: %4dMbps)\n", tmpData);								 
	   }
     else
     {
         printf("invalid\n");   /* phy not found */
     }
						 						 
		/*================== BRR: phy id 2 ================*/
		PhyAddr = hw_config->phyid_brr;		 
    printf("BRR(1000Base-T1)");
			
    /* master/slave */
    get_role_88q2112(PhyAddr, &tmpData);
    printf("    %s", roleStr[tmpData]);

    /* speed */
    get_speed_88q2112(PhyAddr, &tmpData);
    printf("    %4dMbps    ", tmpData);

    if( tmpData != PHY_SPEED_INVALID )
    {
          /* link status */
          get_linkup_88q2112(PhyAddr, &tmpData);
		      if (tmpData) {
			            printf("*UP*");	/* Found a valid PHY device */
		      }
          else {
			            printf("Down");	/* Found a valid PHY device */
          }
     }
     else
     {
                printf("invalid");  /* phy not found */
     }

     /* mode */
     get_mode_88q2112(PhyAddr, &tmpData);
     if( tmpData == ETH_PHY_MODE_COMPLIANT )
           printf(" (ieee_compliant)\n");
     else
           printf(" (legacy)\n");

					
 	  /*
	   * Print out all BRR&ETH MDIO port registers.
	   */
    printf("\n------------------------------------ Registers -----------------------------------\n");
    /* ETH */
		printf("ETH(1000Base-T )");

		PhyAddr = hw_config->phyid_rj45;
		PhyData = 0xFFFF;
		mdio_PhyRead(PhyAddr, PHY_REG0_OFFSET, &PhyData);
		printf("(phy %d): [00]%04X ", PhyAddr, PhyData);
	
		PhyData = 0xFFFF;
		mdio_PhyRead(PhyAddr, PHY_REG1_OFFSET, &PhyData);
		printf("[01]%04X ", PhyData);

		PhyData = 0xFFFF;
		mdio_PhyRead(PhyAddr, PHY_REG2_OFFSET,	&PhyData);
		printf("[02]%04X ", PhyData);
		
		PhyData = 0xFFFF;
		mdio_PhyRead(PhyAddr, PHY_REG4_OFFSET,	&PhyData);
		printf("[04]%04X ", PhyData);
	
		PhyData = 0xFFFF;
		mdio_PhyRead(PhyAddr, PHY_REG5_OFFSET,	&PhyData);
		printf("[05]%04X ", PhyData);
	
		PhyData = 0xFFFF;
		mdio_PhyRead(PhyAddr, PHY_REG9_OFFSET, &PhyData);
		printf("[09]%04X\n", PhyData);
						
    /* BRR */
    printf("BRR(1000Base-T1)");
        
    PhyAddr = hw_config->phyid_brr;				
    PhyData = 0xFFFF;
    xmdio_PhyRead(PCS_CONTROL_DEVICE, PhyAddr, PHY_REG0_OFFSET, &PhyData);
    printf("(phy %d): [00]%04X ", PhyAddr, PhyData);

    PhyData = 0xFFFF;
    xmdio_PhyRead(PCS_CONTROL_DEVICE, PhyAddr, PHY_REG8_OFFSET, &PhyData);
    printf("[08]%04X ", PhyData);

	
    PhyData = 0xFFFF;
    xmdio_PhyRead(PCS_CONTROL_DEVICE, PhyAddr, PCS_1000BASET1_LINK_STATUS_REG, &PhyData);
    printf("[901h]%04X ", PhyData);

    PhyData = 0xFFFF;
    xmdio_PhyRead(RGMII_REGISTER_DEVICE, PhyAddr, RGMII_COM_PORT_CONTROL_REG, &PhyData);
    printf("[31.8000h]%04X ", PhyData);

    PhyData = 0xFFFF;
    xmdio_PhyRead(RGMII_REGISTER_DEVICE, PhyAddr, RGMII_OUTPUT_IMPEDENCE_CONTROL_REG, &PhyData);
    printf("[31.8001h]0x%04X\n", PhyData);
    printf("----------------------------------------------------------------------------------\n\n");
}



/*
 * Detect 88q2112 link status under compliant/legacy mode
 * Input:
 *    PhyAddress - phy id
 * Output:	
 *		ModeFlag - 0: Compliant/Legacy Mode no switch
 *               1: Compliant/Legacy Mode switch
 *	return:
 *		0:  link down
 *		1:  link up 
 */
int get_linkup_88q2112_round(int PhyAddress, int *ModeFlag)
{
    int i;
    int link_down_count=0;
    int16_t link=0;
	
    *ModeFlag = 0;
						
    for(i=0; i<MAX_DETECT_TIME_ONE_ROUND; i++)	{
								
				SYS_Delay(10);		// delay 10ms 
			
			  get_linkup_88q2112(PhyAddress, &link);
					
			  if( !link ) 
				    link_down_count++;
				
				if( link_down_count >= MAX_DOWN_TIME_ONE_ROUND)
        {
					*ModeFlag = 1;	// switch mode					
					return 0;				// link down
		    }   
		} /* for-loop */
				
    if(link_down_count == 0)	
				return 1;				// link up
		else
				return 0;				// link down
}

/*
 * Detect ksz9031 link status in a round
 * Input:
 *    PhyAddress - phy id
 *
 *	return:
 *		0:  link down
 *		1:  link up 
 */
int get_linkup_stdphy_round(int PhyAddress)
{
    int i;
    int link_down_count=0;
		int16_t	link=0;
	
    for(i=0; i<MAX_DETECT_TIME_ONE_ROUND; i++)	{
								
				SYS_Delay(10);		// delay 10ms 
			
		    get_linkup_stdphy(PhyAddress, &link);
					
			  if( !link ) 
				    link_down_count++;
				
		} /* for-loop */
			
    if(link_down_count == 0)	
				return 1;				// link up
		else
				return 0;				// link down
}

/****================================ MDIO/XMDIO Driver =================================***/

void	zdc_mdio_Time_Delay(void)
{
		int i = 0;
		for(i=0; i>TIME_DELAY; i++);
		//TIMER_Delay(TIMER0, 100);	//100uS
}

//MDC output high level
void	MDC_H(void)
{
		MDC_BIT=1;
}	


//MDC output low level
void	MDC_L(void)
{
		MDC_BIT=0;
}	

//Get one bit from MDIO
int  MDIO_Read_Bit(void)
{
    int data=0;
	
		MDC_L();                  // set MDC low 
		zdc_mdio_Time_Delay();
		MDC_H();                  // set MDC high
		zdc_mdio_Time_Delay();
	  data = MDIO_BIT;          // read date
		MDC_L();                  // restore MDC	  
		return	data;
}	

//Set one bit for MDIO
void	MDIO_Send_Bit(int a)
{
		MDC_L();                  // set MDC low
		MDIO_BIT=a;               // output date
		zdc_mdio_Time_Delay();
		MDC_H();                  // set MDC high
		zdc_mdio_Time_Delay();
		MDC_L();                  // restore MDC
}	


/*
MDIO send a data, MDIO needs to be set to output mode
data: the data needs to sent
bits: number of bit
*/
void	MDIO_Send_Data(int senddata, int bits)
{
	int i;
	//MDIO_Set_Output();
	i=bits-1;
	for(;i>=0;i--)
	{
			MDIO_Send_Bit((senddata>>i)&1);		
	}	
}

/*
MDIO get a data, MDIO needs to be set to input mode first
bits: number of bit
*/
int	MDIO_Get_Data(int bits)
{
		int i=0;
		int getdata=0;
		//MDIO_Set_Input();
		for(i=bits-1;i>=0;i--)
		{
			getdata<<=1;
			getdata|=MDIO_Read_Bit();
		}
		return getdata;
}	

/*
Utility to send the preamble, address, and
register (common to read and write). 
*/
void	MDIO_CMD(int op,int phy, int reg)
{
	
/*
* Clause 22:	start bit(01), read bit(10), write bit(01)
* Clause 45:	start bit(00), read bit(11), write bit(01)
*/
		int i;
	
		//send 1 with 32bit for preamble
		for(i=0;i<32;i++)
		{
				MDIO_Send_Bit(1);
		}
		
		//send start bit 01, operation bit: read 10, write 01
		MDIO_Send_Bit(0);	//clause 22
		MDIO_Send_Bit(1); 
	
		//op bit
		MDIO_Send_Bit((op>>1)&1);
		MDIO_Send_Bit(op&1);
		
		//phy address
		MDIO_Send_Data(phy,5);	
		
		//register address
		MDIO_Send_Data(reg,5);	
}


/*
 * Clause22 mdio read
*/
void mdio_PhyRead(uint32_t PhyAddress, uint32_t RegisterNum, uint16_t *PhyDataPtr)
{
		int i=0;

	  /* init value */
	  *PhyDataPtr = 0xFFFF;
	
		MDIO_CMD(MDIO_READ_BIT, PhyAddress, RegisterNum);
			
		//MDIO_Set_Input();	//set the MDIO to input mode
	  MDIO_BIT=1;                       // bus state Z
	  zdc_mdio_Time_Delay();            // wait time for Turn Around

    /* check the turnaround bit: the PHY should driving it to zero */ 
    if(MDIO_Read_Bit() != 0)
    {
        /* PHY didn't driver TA low -- flush any bits it may be trying to send*/
        for(i = 0; i < 32; i++)
           MDIO_Read_Bit();
        /* no data read */
				return;
    }
 
    *PhyDataPtr = MDIO_Get_Data(16);
    MDIO_Read_Bit();
}

/*
 * Clause22 mdio write
*/
void mdio_PhyWrite(uint32_t PhyAddress, uint32_t RegisterNum, uint16_t PhyData)
{
		MDIO_CMD(MDIO_WRITE_BIT, PhyAddress, RegisterNum);

		//send the turnaround(10)
		MDIO_Send_Bit(1);
		zdc_mdio_Time_Delay();
		MDIO_Send_Bit(0);
		zdc_mdio_Time_Delay();
		MDIO_Send_Data(PhyData, 16);
}


/*
   Clause45 XMDIO PHY Read Operation
*/
void xmdio_PhyRead(uint8_t MMD_device, uint32_t PhyAddress, uint32_t RegisterNum, uint16_t *PhyDataPtr)
{
    //write control_reg 0xd with device id
    mdio_PhyWrite(PhyAddress, PHY_XMDIO_CONTROL_REG, (uint16_t)MMD_device);
    //write data_reg 0xe with reg addr
    mdio_PhyWrite(PhyAddress, PHY_XMDIO_DATA_REG, (uint16_t)RegisterNum);
    //write control_reg 0xd with data operation
    mdio_PhyWrite(PhyAddress, PHY_XMDIO_CONTROL_REG, (uint16_t)(PHY_XMDIO_DATA_NOINC|MMD_device)); 
    //read data_reg to get value    
    mdio_PhyRead(PhyAddress, PHY_XMDIO_DATA_REG, PhyDataPtr);
}

/*
   Clause45 XMDIO PHY Write Operation
*/
void xmdio_PhyWrite(uint8_t MMD_device, uint32_t PhyAddress, uint32_t RegisterNum, uint16_t PhyData)
{
    //write control_reg 0xd with device id
    mdio_PhyWrite(PhyAddress, PHY_XMDIO_CONTROL_REG, (uint16_t)MMD_device);
    //write data_reg 0xe with reg addr
    mdio_PhyWrite(PhyAddress, PHY_XMDIO_DATA_REG, (uint16_t)RegisterNum);
    //write control_reg 0xd with data operation
    mdio_PhyWrite(PhyAddress, PHY_XMDIO_CONTROL_REG, (uint16_t)(PHY_XMDIO_DATA_NOINC|MMD_device)); 
    //write data_reg with value    
    mdio_PhyWrite(PhyAddress, PHY_XMDIO_DATA_REG, PhyData);
}
/****================================ MDIO/XMDIO Driver =================================***/
/*==========================================================================================*/

/****======================== Marvel 88Q2112 1000Base-T1 PHY Driver =======================***/

/*
 *  Get 88q2112 mode:
 *    ieee_compliant: DEVM_ETH_PHY_MODE_COMPLIANT
 *    legacy: DEVM_ETH_PHY_MODE_LEGACY 
 */
void get_mode_88q2112(int16_t phy_id, int16_t *mode)
{
    int device=0;
    uint16_t regval=0;

    /* Get training register: 1.0902 */
    device = PMD_PMA_CONTROL_DEVICE; 
    xmdio_PhyRead(device, phy_id, PMD_1000BASET1_TRAINING_REG, &regval);

    if( (regval&MRVL_88Q2112_MODE_MSK) == MRVL_88Q2112_MODE_LEGACY )
    {
        *mode = ETH_PHY_MODE_LEGACY;
    }
    else
    {
        *mode = ETH_PHY_MODE_COMPLIANT;
    }

    return;
}

/*
 *  Set 88q2112 mode:
 *    ieee_compliant: ETH_PHY_MODE_COMPLIANT
 *    legacy:         ETH_PHY_MODE_LEGACY 
 */
void set_mode_88q2112(int16_t phy_id, int16_t mode)
{
    int device=0;

    if( mode == ETH_PHY_MODE_LEGACY )
    {

        /***** Set 1000 BASE-T1 legacy mode to support 88Q2112-A0/A1 chipset *****/
        /* 1) Enable 1000 BASE-T1 legacy mode support: 3.FDB8=0x0001, 3.FD3D=0x0C14 */
        device = PCS_CONTROL_DEVICE;
        xmdio_PhyWrite(device, phy_id, MRVL_88Q2112_MODE_REG1_ADDR, MRVL_88Q2112_MODE_REG1_VAL);
        xmdio_PhyWrite(device, phy_id, MRVL_88Q2112_MODE_REG2_ADDR, MRVL_88Q2112_MODE_REG2_VAL);

        /* 2) Set mode advertise: 1.0902=0x02 */
        device = PMD_PMA_CONTROL_DEVICE; 
        xmdio_PhyWrite(device, phy_id, PMD_1000BASET1_TRAINING_REG, MRVL_88Q2112_MODE_LEGACY | MRVL_88Q2112_MODE_ADVERTISE);
//        if( gZDC_debug )
//            printf("Phy#%d is set 1000Base-T1 legacy mode\r\n", phy_id);
    }        
    else
    {
        /***** reset compliant mode *****/			
        /* 1) Disable 1000 BASE-T1 legacy mode support: 3.FDB8=0x0001, 3.FD3D=0x0C14 */
        device = PCS_CONTROL_DEVICE;
        xmdio_PhyWrite(device, phy_id, MRVL_88Q2112_MODE_REG1_ADDR, MRVL_88Q2112_MODE_REG1_DEFAULT_VAL);
        xmdio_PhyWrite(device, phy_id, MRVL_88Q2112_MODE_REG2_ADDR, MRVL_88Q2112_MODE_REG2_DEFAULT_VAL);
			
        /* 2) Set mode advertise: 1.0902=0x02 */
        device = PMD_PMA_CONTROL_DEVICE; 
        xmdio_PhyWrite(device, phy_id, PMD_1000BASET1_TRAINING_REG, MRVL_88Q2112_MODE_DEFAULT | MRVL_88Q2112_MODE_ADVERTISE);
//        if( gZDC_debug )        
//            printf("Phy#%d is set 1000Base-T1 ieee-compliant mode\r\n", phy_id);
    }

    return;
}

/*
 *  Get 88q2112 speed:
 *    100M:  ETH_PHY_SPEED_100M
 *    1000M: ETH_PHY_SPEED_1000M 
 */
void get_speed_88q2112(int16_t phy_id, int16_t *speed)
{
    uint16_t device=0, reg=0, regval=0;
    int speed_sel=0;

    /* get phy speed from pcs register 3.0.6:13 */
    device = PCS_CONTROL_DEVICE;
    reg=PCS_CONTROL_REG;

    xmdio_PhyRead(device, phy_id, reg, &regval);

    speed_sel = ( ((regval>>6)&0x1)<<1 ) | ((regval>>13)&0x1);
 
    if( speed_sel==0x2 ) {
            *speed = ETH_PHY_SPEED_1000M;  /* set 1000M  */
    }
    else {
            *speed = ETH_PHY_SPEED_100M;   /* set 100M  */
    }

    return;
}

/*
 *  Set 88q2112 speed:
 *    100M:  ETH_PHY_SPEED_100M
 *    1000M: ETH_PHY_SPEED_1000M 
 */
void set_speed_88q2112(int16_t phy_id, int16_t speed)
{
    uint16_t device=0, reg=0, regval=0;
    int write_flag=0;

    device = PMD_PMA_CONTROL_DEVICE;
    reg = PMD_BASET1_CONTROL_REG;
    /* read base-t1 pma/pmd control reg */  
    xmdio_PhyRead(device, phy_id, reg, &regval);

    if( gZDC_debug )
        printf("Phy#%d Base-T1 pma/pmd control register: dev=%d, reg=0x%x, value=0x%04X\r\n", 
            phy_id, device, reg, regval);     

    if(  speed == PHY_SPEED_100 )
    {
        if( (regval&0x000F) != 0 )
        {
            regval = regval & 0xFFF0;    /* 0000 = 100Base-T1 */
            write_flag = 1;
        }
    }
    else
    {
        if( (regval&0x000F) != 0x1 )
        {    
            regval = regval & 0xFFF0;    
            regval = regval | 0x0001;    /* 0001 = 1000Base-T1 */
            write_flag = 1;
        }
    }

    /* need to re-write register */
    if( write_flag )
    {
        xmdio_PhyWrite(device, phy_id, reg, regval);        
        if( gZDC_debug )        
            printf("Phy#%d is set 1000Base-T1 speed %d with Base-T1 pma/pmd control reg value 0x%04X\r\n", 
            phy_id, speed, regval);
 
        /* reset pma/pmd */
        device = PMD_PMA_CONTROL_DEVICE;
        reg = PMD_PMA_CONTROL_REG1;   

        /* read pma/pmd control reg1 */  
        xmdio_PhyRead(device, phy_id, reg, &regval);

        /* write pma/pmd control reg1 bit15-reset */   
        regval = (regval | 0x8000);
        xmdio_PhyWrite(device, phy_id, reg, regval);

        if( gZDC_debug )        
            printf("Phy#%d is reset with pma/pmd control reg1 value 0x%04X\r\n",
                phy_id, regval); 
    }
    else
    {
        if( gZDC_debug )        
            printf("Phy#%d is already 1000Base-T1 speed %d and no need to set\r\n", phy_id, speed);
    }

    return;
}

/*
 *  Get 88q2112 link status:
 *    0:  link down
 *    1:  link up 
 */
void get_linkup_88q2112(int16_t phy_id, int16_t *link)
{
    uint16_t device=0, reg=0, regval=0;
    int16_t speed=0;

    /* get current phy speed */
    get_speed_88q2112(phy_id, &speed);

    /* 
        get link status from phy register:
          100Base-T1: 3.8109.2
         1000Base-T1: 3.0901.2
    */
    device = PCS_CONTROL_DEVICE;
    if(  speed == ETH_PHY_SPEED_1000M )
        reg=PCS_1000BASET1_LINK_STATUS_REG;
    else
        reg=PCS_100BASET1_LINK_STATUS_REG;

    /* fake read */
    xmdio_PhyRead(device, phy_id, reg, &regval);
    /* read again */
    xmdio_PhyRead(device, phy_id, reg, &regval);

    if( ((regval>>2)&0x01) ) {
        *link = 1;           /* set link up */
    }
    else {
        *link = 0;           /* set link down */
    }

    return;
}

/*
 *  Set 88q2112 link status:
 *    0:  low power mode (INH will not change, only link down)
 *    1:  normal operation mode (link up) 
 */
void set_linkup_88q2112(int16_t phy_id, int16_t link)
{
    uint16_t device=0, reg=0, regval=0;


    /* set register 1.0000.11 */
    device = PMD_PMA_CONTROL_DEVICE;
    reg = PMD_PMA_CONTROL_REG1;

    xmdio_PhyRead(device, phy_id, reg, &regval);

	  if( link )   /* MVRL_88Q2112_SLEEP_NORMAL_MODE */
		{
		    regval = (regval & 0xF7FF);			
		} else    /* MVRL_88Q2112_SLEEP_LOWPOWER_MODE */
		{
         regval = (regval | 0x0800);
    }
    xmdio_PhyWrite(device, phy_id, reg, regval);

    /* set register 1.0900.11 */
    device = PMD_PMA_CONTROL_DEVICE;
    reg = PMD_BASET1_DEVICE_CONTROL_REG;

    xmdio_PhyRead(device, phy_id, reg, &regval);
	  if( link )   /* MVRL_88Q2112_SLEEP_NORMAL_MODE */
		{
		    regval = (regval & 0xF7FF);			
		} else    /* MVRL_88Q2112_SLEEP_LOWPOWER_MODE */
		{
         regval = (regval | 0x0800);
    }
    xmdio_PhyWrite(device, phy_id, reg, regval);

    /* set register 3.0000.11 */
    device = PCS_CONTROL_DEVICE;
    reg = PCS_CONTROL_REG;

    xmdio_PhyRead(device, phy_id, reg, &regval);
	  if( link )   /* MVRL_88Q2112_SLEEP_NORMAL_MODE */
		{
		    regval = (regval & 0xF7FF);			
		} else    /* MVRL_88Q2112_SLEEP_LOWPOWER_MODE */
		{
         regval = (regval | 0x0800);
    }

    xmdio_PhyWrite(device, phy_id, reg, regval);

    return;
}

/*
 *  Get 88q2112 1000bae-T1 master/slave role:
 *    0:  auto
 *    1:  master
 *    2:  slave 
 */
void get_role_88q2112(int16_t phy_id, int16_t *role)
{
    uint16_t device=0, reg=0, regval=0;

    /* 
        get baseT1 control from phy register:
          1.0834.14
    */
    device = PMD_PMA_CONTROL_DEVICE;
    reg = PMD_BASET1_CONTROL_REG;

    xmdio_PhyRead(device, phy_id, reg, &regval);

    *role =0;
    if( ((regval>>14)&0x01) ) {
        *role = ETH_PHY_ROLE_MASTER;           /* master */
    }
    else {
        *role = ETH_PHY_ROLE_SLAVE;           /* slave */
    }

    return;
}

/*
 *  Set 88q2112 role:
 *    1:  master
 *    2:  slave 
 */
void set_role_88q2112(int16_t phy_id, int16_t role)
{
    uint16_t device=0, reg=0, regval=0;
    uint16_t regval_set=0;
    char strRole[3][8]={"auto","master","slave"};    

    device = PMD_PMA_CONTROL_DEVICE;
    reg = PMD_BASET1_CONTROL_REG;
    /* read base-t1 pma/pmd control reg */  
    xmdio_PhyRead(device, phy_id, reg, &regval);

    if( gZDC_debug )
        printf("Phy#%d Base-T1 pma/pmd control register: dev=%d, reg=0x%x, value=0x%04X\r\n", 
            phy_id, device, reg, regval);   

    /* master */
    if( role == ETH_PHY_ROLE_MASTER )
    {
        regval_set = regval | 0x4000;      /* master bit */
    }
    else if( role == ETH_PHY_ROLE_SLAVE )
    {
        regval_set = regval & 0xBFFF;      /* slave bit */
    }
    else
    {
        printf("set role error: phy #%d, role=%d\r\n", phy_id, role);
        return;
    }
    
    /* check if need to write back register */
    if ( regval_set != regval )
    {
        xmdio_PhyWrite(device, phy_id, reg, regval_set); 
        if( gZDC_debug )        
            printf("Phy#%d is set to 1000Base-T1 role %s\r\n", phy_id, strRole[role]);     

       /* reset pma/pmd */
        device = PMD_PMA_CONTROL_DEVICE;
        reg = PMD_PMA_CONTROL_REG1;   

        /* read pma/pmd control reg1 */  
        xmdio_PhyRead(device, phy_id, reg, &regval);

        /* write pma/pmd control reg1 bit15-reset */   
        regval = (regval | 0x8000);
        xmdio_PhyWrite(device, phy_id, reg, regval);

        if( gZDC_debug )        
            printf("Phy#%d is reset with pma/pmd control reg1 value 0x%04X\r\n",
                phy_id, regval);                   
    }
    else
    {
        if( gZDC_debug )        
            printf("Phy#%d is already 1000Base-T1 role %s and no need to set\r\n", phy_id, strRole[role]);
    }

    return;
}

/*
 *  reset 88q2112 rgmii delay (tx_delay, rx_delay)
 *  tx_delay/rx_delay:
 *         0 = no delay
 *         1 = delay
 */
void reset_rgmii_delay_88q2112(int16_t phy_id, int16_t tx_delay, int16_t rx_delay)
{
    uint16_t device=0, reg=0, regval=0;

    /* set rgmii tx,rx delay */
    device = RGMII_REGISTER_DEVICE;
    reg = RGMII_COM_PORT_CONTROL_REG;

  	regval = RGMII_COM_PORT_TXRX_DELAY;
	  regval &= ((tx_delay<<15) | (rx_delay<<14)); 
	
    xmdio_PhyWrite(device, phy_id, reg, regval);
    if( gZDC_debug )        
        printf("Phy#%d rgmii tx&rx clock delay are reset (tx_delay=%d, rx_delay=%d)\r\n", 
		           phy_id, tx_delay, rx_delay); 	
	
    /* restart rgmii */  
    device = PCS_CONTROL_DEVICE;
    reg = PCS_RESET_CONTROL_REG;	
    regval = PCS_RESET_CONTROL_RGMII_RESET;
    xmdio_PhyWrite(device, phy_id, reg, regval);

    /* set to rgmii normal */
    regval = PCS_RESET_CONTROL_NORMAL;
    xmdio_PhyWrite(device, phy_id, reg, regval);
	
    if( gZDC_debug )        
        printf("Phy#%d rgmii is re-started\r\n", phy_id);     

    return;
}

/*
 *  set 88q2112 rgmii impedance (pmos, nmos)
 */
void set_rgmii_impedance_pnmos_88q2112(int16_t phy_id, int16_t pmos, int16_t nmos)
{
    uint16_t device=0, reg=0, regval=0;
    uint16_t temp=0;

    device = RGMII_REGISTER_DEVICE;
    reg = RGMII_OUTPUT_IMPEDENCE_CONTROL_REG;
	
	  /* read current */
    xmdio_PhyRead(device, phy_id, reg, &regval);
	
    /* set pmos, nmos */
	  temp = regval & 0xF00F;  /* clear pmos, nmos */
	  regval = temp | (1<<12) | (pmos<<8) | (nmos<<4);  /* fore PMOS/NMOS */
    xmdio_PhyWrite(device, phy_id, reg, regval);
    if( gZDC_debug )        
			printf("Phy#%d rgmii pmos, nmos register set to: 0x%04X\r\n", phy_id, regval);

    /* restart calibration */
	  regval |= 0x8000;
    xmdio_PhyWrite(device, phy_id, reg, regval);		

    if( gZDC_debug )        
        printf("Phy#%d rgmii calibration is restarted with value 0x%04X\r\n", phy_id, regval);     

    return;
}
/****================================ 88Q2112 PHY Driver =================================***/
/*===========================================================================================*/


/****============================ ksz9031(mdio) PHY Driver ===============================***/

/* get normal phy link status */
int get_linkup_stdphy(int16_t phy_id, int16_t *link)
{
    uint16_t PhyData;
	
    /* link status */
	  *link = 0;
    PhyData = 0xFFFF;
		mdio_PhyRead(phy_id, PHY_REG1_OFFSET, &PhyData);
    
	  if (PhyData != 0xFFFF) {
  	    if ((PhyData & PHY_REG1_DETECT_MASK) == PHY_REG1_DETECT_MASK)
			  {
			      *link = 1;
			  }
        else {
			      *link = 0;
         }
				return 0;    /* phy is valid */
	   }
     else
     {
         return -1;  /* phy not found */
     }
}

/*
 * Set phy link power-down and isolate mode.
 *   Input: 0:admin down, 1:admin up
 */
void set_linkup_stdphy(int16_t phy_id, int16_t link)
{
    uint16_t PhyData = 0;

	  /* read phy register 0 */
    mdio_PhyRead(phy_id, PHY_REG0_OFFSET, &PhyData);
	
    if( link )
		{
        /* clear power-down mode and isolate */
        PhyData &= ~PHY_BMCR_PDOWN;
        PhyData &= ~PHY_BMCR_ISOLATE;		
    }
		else
		{
        /* power-down mode and isolate the PHY */
        PhyData |= PHY_BMCR_PDOWN;
        PhyData	|= PHY_BMCR_ISOLATE;			
		}
			
    mdio_PhyWrite(phy_id, PHY_REG0_OFFSET, PhyData);

    return;
}

/* get normal phy speed 
 * 
 * input: 
 *     phy_id    - phy id
 *     autoneg   - auto-negociation(0-no auto, 1-auto)
 * output:
 *     speed     - 10/100/1000 (Mbps)
 */
void get_speed_stdphy(int16_t phy_id, int16_t *speed, int16_t autonego)
{
    uint16_t speed_sel=0, speed_lsb=0, speed_msb=0, PhyData=0;
    uint16_t adv, lpa, lpagb=0, common_adv, common_adv_gb=0;

    if( autonego )
    {
        /* 1000baset-T */
        mdio_PhyRead(phy_id, PHY_REG10_OFFSET, &lpagb);  /* MII_STAT1000 */
        mdio_PhyRead(phy_id, PHY_REG9_OFFSET , &adv);    /* MII_CTRL1000 */
        common_adv_gb = lpagb & (adv << 2);

        /* 100base-T */
        mdio_PhyRead(phy_id, PHY_REG5_OFFSET, &lpa);         /* MII_LPA */
        mdio_PhyRead(phy_id, PHY_REG4_OFFSET, &adv);   /* MII_ADVERTISE */
        common_adv = lpa & adv;

        *speed = PHY_SPEED_10;

        if (common_adv_gb & (LPA_1000FULL | LPA_1000HALF))
        {
            *speed = PHY_SPEED_1000;
        } else if (common_adv & (LPA_100FULL | LPA_100HALF))
        {
            *speed = PHY_SPEED_100;
        }
				

    } else 
    {
	    mdio_PhyRead(phy_id, PHY_REG0_OFFSET, &PhyData);

        /* here we don't consider auto-negociation */
        speed_lsb = (PhyData >> 13)& 0x01;
        speed_msb = (PhyData >> 6) & 0x01;
        speed_sel = (speed_msb<<1) | speed_lsb;
        switch(speed_sel)
        {
            case 0:
                *speed = PHY_SPEED_10;
                break;
            case 1:
                *speed = PHY_SPEED_100;
                break;
            case 2:
                *speed = PHY_SPEED_1000;
                break;
            default:
                *speed = PHY_SPEED_INVALID;
                break;
        } /* switch */
    } /* if(autoneg) */

    return;
}

/*
 * Set speed and Auto-Negociation mode.
    Input:
      speed    -  100(100Mbps), 1000(1000Mbps)
      autonego -  0(non auto-nego), 1(auto-nego)
 *   if autonego=1, speed will be ignored according to negociation.
 */
void set_speed_stdphy(int16_t phy_id, int16_t speed, int16_t autonego)
{
    uint16_t PhyData = 0;
    int loop_cnt=0;

    /* check if phy mdio is ok */
    loop_cnt=0;
    do
    {
		    if(loop_cnt > PHY_MAX_LOOPCOUNT )
		    {
		        printf("PHY %d device mdio bus timeout\r\n", phy_id);
                
            /* print out phy regs */
            mdio_PhyRead(phy_id, PHY_REG0_OFFSET, &PhyData);
            printf("  Reg[0]=0x%04X, ", PhyData);
            mdio_PhyRead(phy_id, PHY_REG1_OFFSET, &PhyData);
            printf("  Reg[1]=0x%04X, ", PhyData);
            mdio_PhyRead(phy_id, PHY_REG2_OFFSET, &PhyData);
            printf("  Reg[2]=0x%04X, ", PhyData);
            mdio_PhyRead(phy_id, PHY_REG3_OFFSET, &PhyData);
            printf("  Reg[3]=0x%04X\r\n", PhyData);
					
						break;
		    }

        loop_cnt ++;

        /* read phy register 0 */
        mdio_PhyRead(phy_id, PHY_REG0_OFFSET, &PhyData);
   
        /* check reset bit */
        if( (PhyData & PHY_BMCR_RESET) ) SYS_Delay(20);    /* delay 20ms */

    } while( (PhyData & PHY_BMCR_RESET) );

    /* reset phy by REG0 */
    PhyData = PHY_BMCR_RESET;
    mdio_PhyWrite(phy_id, PHY_REG0_OFFSET, PhyData);   

    loop_cnt=0;
    do
    {
		    if(loop_cnt > PHY_MAX_LOOPCOUNT )
		    {
		        printf("PHY %d device mdio bus timeout\r\n", phy_id);
                
            /* print out phy regs */
            mdio_PhyRead(phy_id, PHY_REG0_OFFSET, &PhyData);
            printf("  Reg[0]=0x%04X, ", PhyData);
            mdio_PhyRead(phy_id, PHY_REG1_OFFSET, &PhyData);
            printf("  Reg[1]=0x%04X, ", PhyData);
            mdio_PhyRead(phy_id, PHY_REG2_OFFSET, &PhyData);
            printf("  Reg[2]=0x%04X, ", PhyData);
            mdio_PhyRead(phy_id, PHY_REG3_OFFSET, &PhyData);
            printf("  Reg[3]=0x%04X\r\n", PhyData);
					
						break;
		    }

        loop_cnt ++;

        /* read phy register 0 */
        mdio_PhyRead(phy_id, PHY_REG0_OFFSET, &PhyData);
   
        /* check reset bit */
        if( (PhyData & PHY_BMCR_RESET) ) SYS_Delay(20);    /* delay 20ms */

    } while( (PhyData & PHY_BMCR_RESET) ); 


    if(speed == PHY_SPEED_1000 )
    {
        /* REG9: Advertise PHY is 1000BASE-T full-duplex capable */
        PhyData = 0x0200;
        mdio_PhyWrite(phy_id, PHY_REG9_OFFSET, PhyData);			
        PhyData = 0x0140;         /* 1000M, Full-Duplex, non-negociation */
    }
    else {
        /* REG9: Advertise PHY is not 1000BASE-T full-duplex capable */
        PhyData = 0x0;
        mdio_PhyWrite(phy_id, PHY_REG9_OFFSET, PhyData);

        PhyData = 0x2100;         /* 100M, Full-Duplex, non-negociation */
    }

	/* Don't isolate the PHY if we're negotiating */
	PhyData &= ~PHY_BMCR_ISOLATE;

	if( autonego ) {
		PhyData |= (PHY_BMCR_ANENABLE | PHY_BMCR_ANRESTART);    // reset phy in auto-negociation
	}
	else {
		PhyData |= (PHY_BMCR_ANRESTART);                        // reset phy in non-negociation
	}

	mdio_PhyWrite(phy_id, PHY_REG0_OFFSET, PhyData);

	return;
}



#if 0

/****============================ Marvell1510(xmdio) PHY Driver ===============================***/
void	zdc_check_rj45_phy_type(zdc_hw_config_t *hw_config)
{
		uint16_t reg_val, reg_addr;	//page 0, register 0
	
	  /* default reg22=page 0 */
		/* read register from page 0, register 2*/
  	reg_addr=2;
		mdio_PhyRead(hw_config->phyid_rj45,reg_addr,&reg_val);
	
	  if(reg_val == CHIPSET_88E1510_OUI)
		{
			  hw_config->rj45_phy_type = RJ45_PHY_TYPE_MARVELL;
		}
		else
		{
			  hw_config->rj45_phy_type = RJ45_PHY_TYPE_YT8521;			
		}
}
#endif

//Set no internall time delay in 88E1510,clause 22
void zdc_set_88E1510_rgmii_NoDelay(int16_t phy_id)
{
		uint16_t reg_val, reg_addr, page_id;

		// MAC Specific Control is in Register 21 Page 2
	
		//change page 2
		mdio_PhyRead(phy_id, 22, &page_id);
		if(gZDC_debug)
		    printf("Current page: page 0x%02X\r\n", page_id);		

		page_id=2;
		reg_addr=22;
		mdio_PhyWrite(phy_id,reg_addr,page_id);
		mdio_PhyRead(phy_id, 22, &page_id);
	  if(gZDC_debug)
		    printf("New page: page 0x%02X\r\n", page_id);			

	  /* read register from register 21*/
		reg_addr=21;
		mdio_PhyRead(phy_id,reg_addr, &reg_val);
    if(reg_val==0xffff) return;
	  if(gZDC_debug)
		    printf("Current register setting: page %d reg_addr %d, value=0x%04X\r\n", page_id, reg_addr, reg_val);
	
    /* write register */
    reg_val &= (~0x0030);   /* bit 4:tx delay, bit 5:rx_delay, 0:no delay, 1:add delay*/
    //Reg_val |= ((phy_delay<<4) & 0x0030);
	  if(gZDC_debug)
		    printf("New register value=0x%04X\r\n", reg_val);
		
		reg_addr=21;
    mdio_PhyWrite(phy_id,reg_addr, reg_val);
	  if(gZDC_debug)		
        printf("Set RGMII delay (page2, reg21, bit5:4) ok, no time delay.\r\n\r\n");
	
		//RESET PHY
		//Soft reset phy.
		//write page 0, register 0
		page_id = 0;
		reg_addr = 22;

		//Set page select in register 22 page
		mdio_PhyWrite(phy_id,reg_addr,page_id);

	
		/* read register */
		reg_addr=0;
		mdio_PhyRead(phy_id,reg_addr, &reg_val);
	  if(gZDC_debug)		
		    printf("Current register setting: page %d reg_addr %d, value=0x%04X\r\n",	page_id, reg_addr, reg_val);

   /* write soft reset */
    reg_val |= 0x8000;
	  if(gZDC_debug)		
        printf("New register value=0x%04X\r\n", reg_val);
		mdio_PhyWrite(phy_id,reg_addr,reg_val);
	  if(gZDC_debug)		
        printf("Software reset phy (page0, reg0, bit15) ok\r\n");
}	

void set_speed_88e1510(int16_t phy_id, int speed)
{
		uint16_t reg_val, page_id, reg_addr;	//page 0, register 0
	
	  if(speed == ETH_PHY_SPEED_100M )
		{
			  printf("PHY 88E1510 is set to 100M auto-nego OFF\r\n");
	
				//change to page 0
				page_id=0;
				reg_addr=22;
				mdio_PhyWrite(phy_id,reg_addr,page_id);

				/*
				page 0, register 0
				bit12:
				1 = Enable Auto-Negotiation Process
				0 = Disable Auto-Negotiation Process	
				bit6,13:
				11 = Reserved
				10 = 1000 Mbps
				01 = 100 Mbps
				00 = 10 Mbps	

				page 0, register 4
				bit8:	100BASE-TX Full-Duplex
				1 = Advertise
				0 = Not advertised

				page 0, register 9
				100M:  bit9:bit8=00
				1000M: bit9:bit8=11 (default)

				*/	
				/* read register from page 0, register 0*/
				reg_addr=0;
				mdio_PhyRead(phy_id,reg_addr,&reg_val);		
//				printf("Current register setting: page %d reg_addr %d, value=0x%04X\r\n", page_id, reg_addr, reg_val);
			
				/* write register */
				reg_val &= (~0x3040);   /* bit 12=0, bit6,13=01 	xx10|xxxx|x0xx|xxxx	*/	
				reg_val |= 0x2000;
//				printf("New register value=0x%04X\r\n", reg_val);
				reg_addr=0;
				mdio_PhyWrite(phy_id,reg_addr,reg_val);
//				printf("Set RGMII speed to 100Mbps ok.\r\n");

				/* read register from page 0, register 4*/
				reg_addr=4;
				mdio_PhyRead(phy_id,reg_addr,&reg_val);
//				printf("Current register setting: page %d reg_addr %d, value=0x%04X\r\n", page_id, reg_addr, reg_val);
			
				/* write register */
				reg_val |= 0x0100;   /* bit 8=1, xxxx|xxx1|xxxx|xxxx	*/	
//				printf("New register value=0x%04X\r\n", reg_val);
				reg_addr=4;
				mdio_PhyWrite(phy_id,reg_addr, reg_val);
//				printf("Set RGMII with 100BASE-TX Full-Duplex advertise ok.\r\n\r\n");		

				/* read register from page 0, register 9*/
				reg_addr=9;
				mdio_PhyRead(phy_id,reg_addr,&reg_val);
//				printf("Current register setting: page %d reg_addr %d, value=0x%04X\r\n", page_id, reg_addr, reg_val);
			
				/* write register */
				reg_val &= 0xfcff;   /* bit9:bit8=00, xxxx|xx00|xxxx|xxxx	*/	
//				printf("New register value=0x%04X\r\n", reg_val);
				reg_addr=9;
				mdio_PhyWrite(phy_id,reg_addr, reg_val);
				
				printf("Set 1000BASE-T Full-Duplex Not advertise ok.\r\n\r\n");		
		}
		else
		{
			  printf("PHY 88E1510 is set to 1000M auto-nego ON\r\n");
		}
}


/****============================ Motorcomm YT8521SH(xmdio) PHY Driver ===============================***/
static void yt8521_PhyRead_ext(int16_t phy_id, uint16_t regnum, uint16_t *val)
{
	mdio_PhyWrite(phy_id, REG_DEBUG_ADDR_OFFSET, regnum);	

	mdio_PhyRead(phy_id, REG_DEBUG_DATA, val);
}

static void yt8521_PhyWrite_ext(int16_t phy_id, uint16_t regnum, uint16_t val)
{
	mdio_PhyWrite(phy_id, REG_DEBUG_ADDR_OFFSET, regnum);	

	mdio_PhyWrite(phy_id, REG_DEBUG_DATA, val);
}


//Set no internal rx time delay in YT8521SH
void zdc_set_YT8521_rgmii_NoDelay(int16_t phy_id)
{
		uint16_t reg_val=0;

	  /* disable rxc_dly_en */
	  yt8521_PhyRead_ext(phy_id, YT8521_EXTREG_CHIP_CFG, &reg_val);
	  if( gZDC_debug )
		    printf("Read    Chip_Config(EXT_0xA001): 0x%04X\n", reg_val);			
	
		reg_val &= 0xFEFF; /* clear rxc_dly_en */
    yt8521_PhyWrite_ext(phy_id, YT8521_EXTREG_CHIP_CFG, reg_val);
	  if( gZDC_debug )
		    printf("Write   Chip_Config(EXT_0xA001): 0x%04X\n", reg_val);
#if 1	 /* if not set: keep default tx delay set for 100M & 10000M */
	  yt8521_PhyRead_ext(phy_id, YT8521_EXTREG_RGMII_CFG1, &reg_val);
	  if( gZDC_debug )
		    printf("Read  RGMII_Config1(EXT_0xA003): 0x%04X\n", reg_val);			
	
		reg_val = 0x0011; /* set tx_delay_sel_fe, tx_delay_sel to 1 */
    yt8521_PhyWrite_ext(phy_id, YT8521_EXTREG_RGMII_CFG1, reg_val);
	  if( gZDC_debug )
	    printf("Write RGMII_Config1(EXT_0xA003): 0x%04X\n", reg_val);
#endif
}

/*
    Default: 
        LED0 -   10M link/act
        LED1 -  100M link/act
        LED2 - 1000M link/act
    Set:
        LED1 - 100M/1000M link/act
        LED2 - 1000M link/act (no change)
*/
void	zdc_set_yt8521_led(int16_t phy_id)
{	
	  /* LED1: blink, Led_gt_on_en_1, Led_ht_on_en_1 */
    yt8521_PhyWrite_ext(phy_id, YT8521_EXTREG_LED1_CFG, 0x0606);
	
	  /* set blink freq 8Hz */
	  yt8521_PhyWrite_ext(phy_id, YT8521_EXTREG_LED_BLINK_CFG, 0x000A);
}

/*===========================================================================================*/
