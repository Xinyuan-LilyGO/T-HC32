/******************************************************************************
* Copyright (C) 2016, Huada Semiconductor Co.,Ltd All rights reserved.
*
* This software is owned and published by:
* Huada Semiconductor Co.,Ltd ("HDSC").
*
* BY DOWNLOADING, INSTALLING OR USING THIS SOFTWARE, YOU AGREE TO BE BOUND
* BY ALL THE TERMS AND CONDITIONS OF THIS AGREEMENT.
*
* This software contains source code for use with HDSC
* components. This software is licensed by HDSC to be adapted only
* for use in systems utilizing HDSC components. HDSC shall not be
* responsible for misuse or illegal use of this software for devices not
* supported herein. HDSC is providing this software "AS IS" and will
* not be responsible for issues arising from incorrect user implementation
* of the software.
*
* Disclaimer:
* HDSC MAKES NO WARRANTY, EXPRESS OR IMPLIED, ARISING BY LAW OR OTHERWISE,
* REGARDING THE SOFTWARE (INCLUDING ANY ACOOMPANYING WRITTEN MATERIALS),
* ITS PERFORMANCE OR SUITABILITY FOR YOUR INTENDED USE, INCLUDING,
* WITHOUT LIMITATION, THE IMPLIED WARRANTY OF MERCHANTABILITY, THE IMPLIED
* WARRANTY OF FITNESS FOR A PARTICULAR PURPOSE OR USE, AND THE IMPLIED
* WARRANTY OF NONINFRINGEMENT.
* HDSC SHALL HAVE NO LIABILITY (WHETHER IN CONTRACT, WARRANTY, TORT,
* NEGLIGENCE OR OTHERWISE) FOR ANY DAMAGES WHATSOEVER (INCLUDING, WITHOUT
* LIMITATION, DAMAGES FOR LOSS OF BUSINESS PROFITS, BUSINESS INTERRUPTION,
* LOSS OF BUSINESS INFORMATION, OR OTHER PECUNIARY LOSS) ARISING FROM USE OR
* INABILITY TO USE THE SOFTWARE, INCLUDING, WITHOUT LIMITATION, ANY DIRECT,
* INDIRECT, INCIDENTAL, SPECIAL OR CONSEQUENTIAL DAMAGES OR LOSS OF DATA,
* SAVINGS OR PROFITS,
* EVEN IF Disclaimer HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
* YOU ASSUME ALL RESPONSIBILITIES FOR SELECTION OF THE SOFTWARE TO ACHIEVE YOUR
* INTENDED RESULTS, AND FOR THE INSTALLATION OF, USE OF, AND RESULTS OBTAINED
* FROM, THE SOFTWARE.
*
* This software may be replicated in part or whole for the licensed use,
* with the restriction that this Disclaimer and Copyright notice must be
* included with each copy of this software, whether used in part or whole,
* at all times.
*/
/******************************************************************************/
/** \file main.c
 **
 ** A detailed description is available at
 ** @link Sample Group Some description @endlink
 **
 **   - 2016-02-16  1.0  XYZ First version for Device Driver Library of Module.
 **
 ******************************************************************************/

/******************************************************************************
 * Include files
 ******************************************************************************/
#include "i2c.h"
#include "gpio.h"
/******************************************************************************
 * Local pre-processor symbols/macros ('#define')
 ******************************************************************************/

/******************************************************************************
 * Global variable definitions (declared in header file with 'extern')
 ******************************************************************************/

/******************************************************************************
 * Local type definitions ('typedef')
 ******************************************************************************/

/******************************************************************************
 * Local function prototypes ('static')
 ******************************************************************************/

/******************************************************************************
 * Local variable definitions ('static')                                      *
 ******************************************************************************/

/******************************************************************************
 * Local pre-processor symbols/macros ('#define')
 ******************************************************************************/
#define     T1_PORT                 (3)
#define     T1_PIN                  (3)
#define     I2C_SLAVEWT             0xA0//此处从机地址根据需要进行配置
#define     I2C_SLAVERD             0xA1
#define 	READLEN					10
/*****************************************************************************
 * Function implementation - global ('extern') and local ('static')
 ******************************************************************************/
/**
 ******************************************************************************
 ** \brief  Main function of project
 **
 ** \return uint32_t return value, if needed
 **
 ** This sample
 **
 ******************************************************************************/

uint8_t u8Senddata[10] = {0x12,0x34,0x77,0x66,0x55,0x44,0x33,0x22,0x11,0x55};
uint8_t u8Recdata[10]={0x00};
boolean_t bIrq = 0;
uint8_t u8State = 0,u8SendCnt = 0,u8RecCnt = 0;
uint8_t SendFlg = 0,Comm_flg = 0;
uint8_t data=0;
uint8_t u8Addr = 0x00;//地址字节
static void I2cCallBack(void)
{
    u8State = I2C_GetState();
    switch(u8State)
    {
        case 0x08:
            I2C_ClearFunc(I2cStart_En);
            I2C_WriteByte(I2C_SLAVEWT);
            break;
        case 0x10:
            I2C_ClearFunc(I2cStart_En);
            if(0 == SendFlg)
            {
                I2C_WriteByte(I2C_SLAVEWT);
            }
            else
            {
                I2C_WriteByte(I2C_SLAVERD);
            }
            break;
        case 0x18:
            I2C_ClearFunc(I2cStart_En);
            I2C_WriteByte(u8Addr);//地址字节
            break;
        case 0x20:
        case 0x38:
        case 0x30:
        case 0x48:
            I2C_SetFunc(I2cStart_En);
            I2C_SetFunc(I2cStop_En);
        break;			
        case 0x58:
            u8Recdata[u8RecCnt++] = I2C_ReadByte();
            I2C_SetFunc(I2cStop_En);
            break;				
        case 0x28:
            if(SendFlg == 1)//读数据发送完地址字节后，重复起始条件
            {
                I2C_SetFunc(I2cStart_En);
                I2C_ClearFunc(I2cStop_En);
            }
            else
                I2C_WriteByte(u8Senddata[u8SendCnt++]);
            if(u8SendCnt>10)
            {
                u8SendCnt = 0;
                Comm_flg = 1;
                SendFlg = 1;
                I2C_SetFunc(I2cStop_En);
            }
            break;
        case 0x40:
            u8RecCnt = 0;
			if(READLEN>1)
			{
				I2C_SetFunc(I2cAck_En);
			}
            break;	
        case 0x50:
			u8Recdata[u8RecCnt++] = I2C_ReadByte();
            if(u8RecCnt==READLEN-1)
            {
                I2C_ClearFunc(I2cAck_En);
            }
            break;
    }
    
    I2C_ClearIrq();
}
int32_t main(void)
{
    
    stc_i2c_config_t stcI2cCfg;
    stc_clk_config_t stcCfg;
    DDL_ZERO_STRUCT(stcCfg);
    DDL_ZERO_STRUCT(stcI2cCfg);

    Gpio_InitIOExt(2,5,GpioDirOut,TRUE,FALSE,TRUE,FALSE); 
    Gpio_InitIOExt(2,6,GpioDirOut,TRUE,FALSE,TRUE,FALSE);
    
    Gpio_SetFunc_I2CDAT_P25() 
    Gpio_SetFunc_I2CCLK_P26()

    Clk_SetPeripheralGate(ClkPeripheralI2c,TRUE);
    
    stcI2cCfg.enFunc = I2cBaud_En;
    stcI2cCfg.u8Tm = 0x04;//100K=(4000000/(8*(4+1))
    stcI2cCfg.pfnI2cCb = I2cCallBack;
    stcI2cCfg.bTouchNvic = TRUE;
    
    I2C_DeInit(); 
    I2C_Init(&stcI2cCfg);
    I2C_SetFunc(I2cMode_En);
    if(stcI2cCfg.bTouchNvic == TRUE)
	{
		EnableNvic(I2C_IRQn,3,TRUE);
	}
    I2C_SetFunc(I2cStart_En);
    while(1)
    {
      if(1 == Comm_flg)
      {
        Comm_flg = 0;
        delay1ms(100);
        I2C_SetFunc(I2cStart_En);
      }
    }
     
    
}

/******************************************************************************
 * EOF (not truncated)
 ******************************************************************************/


