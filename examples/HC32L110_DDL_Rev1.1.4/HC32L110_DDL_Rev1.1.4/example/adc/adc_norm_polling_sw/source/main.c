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
 **   - 2018-09-12 Husj	    Version0.2
 **
 ******************************************************************************/

/******************************************************************************
 * Include files
 ******************************************************************************/
#include "adc.h"
#include "gpio.h"
#include "ddl.h"

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
static uint16_t u16AdcResult;

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
int32_t main(void)
{
    stc_adc_cfg_t      stcAdcCfg;
    stc_adc_norm_cfg_t stcAdcNormCfg;
    
    DDL_ZERO_STRUCT(stcAdcCfg);
    DDL_ZERO_STRUCT(stcAdcNormCfg);
    
    //Clk_SwitchTo(ClkRCL);
    //Clk_SetRCHFreq(ClkFreq24Mhz);
    //Clk_SwitchTo(ClkRCH);
    
    if (Ok != Clk_SetPeripheralGate(ClkPeripheralGpio, TRUE))
    {
        return Error;
    }

    if (Ok != Clk_SetPeripheralGate(ClkPeripheralAdcBgr, TRUE))
    {
        return Error;
    }
    
    Gpio_SetAnalog(2, 4, TRUE);
    Gpio_SetAnalog(2, 6, TRUE);
    
    Adc_Enable();
    M0P_BGR->CR_f.BGR_EN = 0x1u;//BGR必须使能
    M0P_BGR->CR_f.TS_EN = 0x0u;

    delay100us(1);
    
    stcAdcCfg.enAdcOpMode = AdcNormalMode;          //单次采样模式
    stcAdcCfg.enAdcClkSel = AdcClkSysTDiv1;         //PCLK
    stcAdcCfg.enAdcSampTimeSel = AdcSampTime4Clk;   //4个采样时钟
    stcAdcCfg.enAdcRefVolSel = RefVolSelInBgr2p5;   //参考电压:内部2.5V(avdd>3V,SPS<=200kHz)  SPS速率 = ADC时钟 / (采样时钟 + 16CLK) 
    stcAdcCfg.bAdcInBufEn = FALSE;                  //电压跟随器如果使能，SPS采样速率 <=200K
    stcAdcCfg.u32AdcRegHighThd = 0u;                //比较阈值上门限
    stcAdcCfg.u32AdcRegLowThd = 0u;                 //比较阈值下门限
    stcAdcCfg.enAdcTrig0Sel = AdcTrigDisable;       //ADC转换自动触发设置
    stcAdcCfg.enAdcTrig1Sel = AdcTrigDisable;
    Adc_Init(&stcAdcCfg);    
    
            
    while(1)
    {    
        delay1ms(1000);
        
        stcAdcNormCfg.enAdcNormModeCh = AdcExInputCH0;  //通道0 P24
        stcAdcNormCfg.bAdcResultAccEn = FALSE;
        Adc_ConfigNormMode(&stcAdcCfg, &stcAdcNormCfg);
        
        Adc_Start();
        while(FALSE != Adc_PollBusyState());
        Adc_GetResult(&u16AdcResult);

        
        stcAdcNormCfg.enAdcNormModeCh = AdcExInputCH1;  //通道1 P26
        stcAdcNormCfg.bAdcResultAccEn = FALSE;
        Adc_ConfigNormMode(&stcAdcCfg, &stcAdcNormCfg);
        
        Adc_Start();
        while(FALSE != Adc_PollBusyState());
        Adc_GetResult(&u16AdcResult);
    }


}

/******************************************************************************
 * EOF (not truncated)
 ******************************************************************************/


