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
static stc_adc_irq_t stcAdcIrqFlag;

/*****************************************************************************
 * Function implementation - global ('extern') and local ('static')
 ******************************************************************************/

void AdcContIrqCallback(void)
{    
    //中断标志位判断和清零，已在库函数中处理Adc_IRQHandler();
    
    stcAdcIrqFlag.bAdcIrq = TRUE;
    
    // Adc_ClrContIrqState();    
}

void AdcRegIrqCallback(void)
{    
    //中断标志位判断和清零，已在库函数中处理Adc_IRQHandler();
  
    stcAdcIrqFlag.bAdcRegCmp = TRUE;
  
    // Adc_ClrRegIrqState();    
}

void AdcHhtIrqCallback(void)
{    
    //中断标志位判断和清零，已在库函数中处理Adc_IRQHandler();
  
    stcAdcIrqFlag.bAdcHhtCmp = TRUE;
    
    // Adc_ClrHhtIrqState();    
}

void AdcLltIrqCallback(void)
{    
    //中断标志位判断和清零，已在库函数中处理Adc_IRQHandler();
  
    stcAdcIrqFlag.bAdcLltCmp = TRUE;
  
    // Adc_ClrLltIrqState();    
}

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
    stc_adc_cfg_t stcAdcCfg;
    stc_adc_norm_cfg_t stcAdcNormCfg;
    stc_adc_irq_t stcAdcIrq;
    stc_adc_irq_calbakfn_pt_t stcAdcIrqCalbaks;
    uint16_t u16AdcResult;
    
    DDL_ZERO_STRUCT(stcAdcCfg);
    DDL_ZERO_STRUCT(stcAdcNormCfg);
    DDL_ZERO_STRUCT(stcAdcIrq);
    DDL_ZERO_STRUCT(stcAdcIrqCalbaks);
    DDL_ZERO_STRUCT(stcAdcIrqFlag);
    
    Clk_SwitchTo(ClkXTH);    
    
    
    if (Ok != Clk_SetPeripheralGate(ClkPeripheralGpio, TRUE))    //GPIO 外设时钟使能
    {
        return Error;
    }
    if (Ok != Clk_SetPeripheralGate(ClkPeripheralAdcBgr, TRUE))  //ADCBGR 外设时钟使能
    {
        return Error;
    }
    
    
    Gpio_SetAnalog(2, 6, TRUE);
    
    Adc_Enable();
    M0P_BGR->CR_f.BGR_EN = 0x1u;  //BGR必须使能
    M0P_BGR->CR_f.TS_EN = 0x0u;   //内置温度传感器，视使用需求
    delay100us(1);
    
    stcAdcCfg.enAdcOpMode = AdcNormalMode;           //单次采样模式
    stcAdcCfg.enAdcClkSel = AdcClkSysTDiv2;          //PCLK/2
    stcAdcCfg.enAdcSampTimeSel = AdcSampTime4Clk;    //4个采样时钟
    stcAdcCfg.enAdcRefVolSel = RefVolSelAVDD;        //参考电压:AVDD
    stcAdcCfg.bAdcInBufEn = FALSE;                   //电压跟随器如果使能，SPS采样速率 <=200K
    //stcAdcCfg.u32AdcRegHighThd = 0xCCCu;           //比较阈值上门限
    //stcAdcCfg.u32AdcRegLowThd = 0x800u;            //比较阈值下门限
    stcAdcCfg.u32AdcRegHighThd = 0xFFFu;             //比较阈值上门限
    stcAdcCfg.u32AdcRegLowThd = 0x000u;              //比较阈值下门限
    stcAdcCfg.enAdcTrig0Sel = AdcTrigDisable;        //ADC转换自动触发设置
    stcAdcCfg.enAdcTrig1Sel = AdcTrigDisable;
    Adc_Init(&stcAdcCfg);    
    
    stcAdcIrq.bAdcIrq = TRUE;                            //转换完成中断函数入口配置使能
    stcAdcIrq.bAdcRegCmp = TRUE;                         //区间比较中断函数入口配置使能
    stcAdcIrq.bAdcHhtCmp = TRUE;                         //高于阈值比较中断函数入口配置使能
    stcAdcIrq.bAdcLltCmp = TRUE;                         //低于阈值比较中断函数入口配置使能
    stcAdcIrqCalbaks.pfnAdcContIrq = AdcContIrqCallback; //转换完成中断入口函数
    stcAdcIrqCalbaks.pfnAdcRegIrq = AdcRegIrqCallback;
    stcAdcIrqCalbaks.pfnAdcHhtIrq = AdcHhtIrqCallback;
    stcAdcIrqCalbaks.pfnAdcLltIrq = AdcLltIrqCallback;
    Adc_ConfigIrq(&stcAdcIrq, &stcAdcIrqCalbaks);        //转换中断入口函数配置
    Adc_EnableIrq();                                     //中断使能
    Adc_CmpCfg(&stcAdcIrq);                              //结果比较中断使能/禁止配置
    
    stcAdcNormCfg.enAdcNormModeCh = AdcExInputCH0;       //通道0 P24
    stcAdcNormCfg.bAdcResultAccEn = FALSE;               //累加禁止
    Adc_ConfigNormMode(&stcAdcCfg, &stcAdcNormCfg);    
    
    while(1)
    {
        Adc_Start();
        
        while(FALSE == stcAdcIrqFlag.bAdcIrq
              && FALSE == stcAdcIrqFlag.bAdcRegCmp
              && FALSE == stcAdcIrqFlag.bAdcHhtCmp
              && FALSE == stcAdcIrqFlag.bAdcLltCmp);
        
        Adc_GetResult(&u16AdcResult);
        
        delay1ms(1000);
        
        stcAdcIrqFlag.bAdcIrq = 0;
        stcAdcIrqFlag.bAdcRegCmp = 0;
        stcAdcIrqFlag.bAdcHhtCmp = 0;
        stcAdcIrqFlag.bAdcLltCmp = 0;
    }
}

/******************************************************************************
 * EOF (not truncated)
 ******************************************************************************/


