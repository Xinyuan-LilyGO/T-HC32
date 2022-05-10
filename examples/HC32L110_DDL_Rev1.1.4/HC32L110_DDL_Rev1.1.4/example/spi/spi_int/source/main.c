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
#include "ddl.h"
#include "spi.h"
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
#define     T1_PIN                  (2)
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


static void SpiCallBack(void)
{
    if(TRUE == Spi_GetStatus(SpiIf))
    {
        bIrq = 1;
    }
    bIrqData = M0P_SPI->DATA; 
}

void WriteData(uint8_t *sendstr,uint8_t sendlen)
{
    uint8_t i;
    uint32_t u32TimeOut;

    bIrq = 0;

    for(i=0;i<sendlen;i++)
    {
        u32TimeOut = 1000;
        M0P_SPI->DATA = *(sendstr + i);
        while(u32TimeOut--)
        {
            if(1 == bIrq)
            {
                break;
            }
        }
        bIrq = 0;
    }

}

void ReadData(uint8_t *recvstr,uint8_t recvlen)
{
    uint8_t i;
    uint32_t u32TimeOut;

    bIrq = 0;

    for(i=0;i<recvlen;i++)
    {
        u32TimeOut = 1000;  
        M0P_SPI->DATA = 0;     
        while(u32TimeOut--)
        {
            if(1 == bIrq)
            {
                *(recvstr+i) = bIrqData;
                break;
            }
        }
        bIrq = 0;
    }
}


int32_t main(void)
{
    
    stc_spi_config_t  SPIConfig;
    uint8_t   i,j;
    
    Clk_SwitchTo(ClkXTH);
    Clk_SetPeripheralGate(ClkPeripheralSpi,TRUE); //SPI外设时钟打开
    
    Gpio_InitIO(T1_PORT, T1_PIN, GpioDirOut);
    Gpio_SetIO(T1_PORT, T1_PIN, 0);               //P32输出低
    
    Gpio_SetFunc_SPI_CS_P03();
    Gpio_SetFunc_SPIMISO_P23();
    Gpio_SetFunc_SPIMOSI_P24();
    Gpio_SetFunc_SPICLK_P25();                    //配置SPI外设引脚
    
    Spi_SetCS(TRUE);
    //配置SPI
    SPIConfig.bCPHA = Spicphafirst;
    SPIConfig.bCPOL = Spicpollow;
    SPIConfig.bIrqEn = TRUE;
    SPIConfig.bMasterMode = SpiMaster;
    SPIConfig.u8BaudRate = SpiClkDiv2;
    SPIConfig.pfnIrqCb = SpiCallBack;

    Spi_Init(&SPIConfig);
    
    //操作FM25640
    for(i=3;i<8;i++)
    {
        Buff[i] = i+0x10;
    }
    
    Buff[0] = 0x06;
    Spi_SetCS(FALSE);
    WriteData(&Buff[0],1);
    Spi_SetCS(TRUE);
    
    Buff[0] = 0x02;        //写命令
    Buff[1] = 0x00;        //写地址
    Buff[2] = 0x00;
    Spi_SetCS(FALSE);
    WriteData(&Buff[0],8); //发送命令和数据
    Spi_SetCS(TRUE);

    delay1ms(2000);     
    
    Buff[0] = 0x03;        //读命令
    Buff[1] = 0x00;        //地址
    Buff[2] = 0x00;
    Spi_SetCS(FALSE);
    WriteData(&Buff[0],3); //发送读命令
    ReadData(&Buff[13],5); //收数据
    Spi_SetCS(TRUE);
    
    j=0;
    for(i=3;i<8;i++)   
    {
        if(Buff[10+i] == Buff[i])
        {
            j++;
        }
    }
    if(j == 5)
    {
        Gpio_SetIO(T1_PORT, T1_PIN, 1);  //P32 置高
    }

    while(1);
}

/******************************************************************************
 * EOF (not truncated)
 ******************************************************************************/


