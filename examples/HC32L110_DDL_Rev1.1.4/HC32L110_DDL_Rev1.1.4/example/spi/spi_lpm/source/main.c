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
#include "spi.h"
#include "lpm.h"
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
#define    T1_PORT        (3)
#define    T1_PIN         (3)

#define    T2_PORT        (3)
#define    T2_PIN         (2)

#define    T3_PORT        (0)
#define    T3_PIN         (3)
/*****************************************************************************
 * Function implementation - global ('extern') and local ('static')
 ******************************************************************************/
uint8_t bIrq,bIrqData,Buff[20]; 
/**
 ******************************************************************************
 ** \brief  Main function of project
 **
 ** \return uint32_t return value, if needed
 **
 ** This sample
 **
 ******************************************************************************/

void Gpio_IRQHandler(void)
{
    if(TRUE == M0P_GPIO->P0STAT_f.P03)
    {
        bIrqData = Spi_ReceiveData();  //获取主机发送来的数据 
        M0P_GPIO->P0ICLR_f.P03 = 0;
        bIrq = 1;
    }
}




int32_t main(void)
{
    stc_spi_config_t  SPIConfig;
    stc_lpm_config_t  stcLpmCfg;
    
    DDL_ZERO_STRUCT(SPIConfig);
    DDL_ZERO_STRUCT(stcLpmCfg);
    
    Clk_SwitchTo(ClkXTH);
    Clk_SetPeripheralGate(ClkPeripheralGpio,TRUE);    
    Clk_SetPeripheralGate(ClkPeripheralSpi,TRUE); 
    
    Buff[0] = Buff[1] = 0x00;
    Gpio_InitIO(T1_PORT, T1_PIN, GpioDirIn);
    Gpio_InitIO(T2_PORT, T2_PIN, GpioDirOut);
    Gpio_SetIO(T2_PORT, T2_PIN, 0);

    //P25 CLK
    M0P_GPIO->P2ADS_f.P25 = 0;
    M0P_GPIO->P25_SEL_f.SEL = 1;
    M0P_GPIO->P2DIR_f.P25 = 1;
    //P24 MOSI
    M0P_GPIO->P2ADS_f.P24 = 0;
    M0P_GPIO->P24_SEL_f.SEL = 5;
    M0P_GPIO->P2DIR_f.P24 = 1;
    //P23 MISO
    M0P_GPIO->P2ADS_f.P23 = 0;
    M0P_GPIO->P23_SEL_f.SEL = 5;
    M0P_GPIO->P2DIR_f.P23 = 0; 
       
    //P03 AS cs
    M0P_GPIO->P0ADS_f.P03 = 0;
    M0P_GPIO->CTRL1_f.SSN_SEL = 0x05;
    M0P_GPIO->P0DIR_f.P03 = 1;
    
    Gpio_EnableIrq(T3_PORT,T3_PIN,GpioIrqFalling);
    EnableNvic(PORT0_IRQn,DDL_IRQ_LEVEL_DEFAULT,TRUE);
    
    //配置SPI
    SPIConfig.bCPHA = Spicphafirst;
    SPIConfig.bCPOL = Spicpollow;
    SPIConfig.bMasterMode = SpiSlave;
    SPIConfig.u8BaudRate = SpiClkDiv2;
    
    SPIConfig.bIrqEn = FALSE;
    SPIConfig.pfnIrqCb = NULL;

    Spi_Init(&SPIConfig);
    
#if 0 //休眠模式测试
    while(1 == Gpio_GetIO(T1_PORT,T1_PIN));//等待P33端口电平变低，进入休眠模式，为防止不能再次下载样例程序，不可删除
    stcLpmCfg.enSEVONPEND = SevPndDisable;
    stcLpmCfg.enSLEEPDEEP = SlpDpDisable;
    stcLpmCfg.enSLEEPONEXIT = SlpExtDisable;
    Lpm_Config(&stcLpmCfg);
    Lpm_GotoLpmMode();
#endif   
    

    while(1)
    {
        if(bIrq == 1)
        {
            if(bIrqData == 0x9F)
            {
                Gpio_SetIO(T2_PORT, T2_PIN, 1);
                while(1);
            }
        }
    }
}

/******************************************************************************
 * EOF (not truncated)
 ******************************************************************************/


