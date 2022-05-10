/******************************************************************************
* Copyright (C) 2017, Huada Semiconductor Co.,Ltd All rights reserved.
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
 **   - 2017-05-17  1.0  CJ First version for Device Driver Library of Module.
 **
 ******************************************************************************/

/******************************************************************************
 * Include files
 ******************************************************************************/

#include "wdt.h"
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
#define     T1_PORT                 (0)
#define     T1_PIN                  (3)

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
uint8_t u32CountWdt;
uint8_t Data=0xff;
uint8_t cnt=0;

static void WdtCallback(void)
{
    // comment following to demonstrate the hardware watchdog reset
    Data = ~Data;
    Gpio_SetIO(T1_PORT,T1_PIN,Data);
    u32CountWdt++;
}
int32_t main(void)
{
    stc_wdt_config_t  stcWdt_Config;
    stc_lpm_config_t stcLpmCfg;
    
    DDL_ZERO_STRUCT(stcWdt_Config);
    DDL_ZERO_STRUCT(stcLpmCfg);
    
    stcLpmCfg.enSLEEPDEEP = SlpDpDisable;//SlpDpEnable;
    stcLpmCfg.enSLEEPONEXIT = SlpExtEnable;
    
    Gpio_InitIO(T1_PORT,T1_PIN,GpioDirOut);
    Gpio_SetIO(T1_PORT,T1_PIN,0);
    Gpio_InitIO(3,3,GpioDirIn);
    
    stcWdt_Config.u8LoadValue = 0x0b;//3.2s
    stcWdt_Config.enResetEnable = WINT_EN;//WRESET_EN;////
    stcWdt_Config.pfnWdtIrqCb = WdtCallback;
    
    Clk_SetPeripheralGate(ClkPeripheralWdt,TRUE);//
    Wdt_Init(&stcWdt_Config);
    
    Wdt_Start();
    Gpio_SetIO(T1_PORT,T1_PIN,1);
#if 0 
    while(1 == Gpio_GetIO(3,3));//注意：此处如果用户测试深度休眠模式，此处不能屏蔽    
    Lpm_Config(&stcLpmCfg);
    Lpm_GotoLpmMode();
#endif
    while (1)
    {
        //cnt = Wdt_ReadWdtValue();
        //Wdt_Feed();//测试喂狗功能时，放开屏蔽
    }
}

/******************************************************************************
 * EOF (not truncated)
 ******************************************************************************/


