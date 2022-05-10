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
#include "bt.h"

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
static  uint16_t  u16ScanResult[8];

/*****************************************************************************
 * Function implementation - global ('extern') and local ('static')
 ******************************************************************************/

void AdcContIrqCallback(void)
{    
    //中断标志位判断和清零，已在库函数中处理Adc_IRQHandler();   
  
    Adc_GetScanResult(0, &u16ScanResult[0]);
    Adc_GetScanResult(1, &u16ScanResult[1]);
    
    M0P_ADC->CR0_f.STATERST = 1u;  //ADC连续转换复位
    
    M0P_BT2->ICLR_f.TFC = 0;
    // Adc_ClrContIrqState();    
}
void AdcRegIrqCallback(void)
{    
    // 中断标志位判断和清零，已在库函数中处理Adc_IRQHandler();
    
    
    // Adc_ClrRegIrqState();    
}

void AdcHhtIrqCallback(void)
{    
    // 中断标志位判断和清零，已在库函数中处理Adc_IRQHandler();
    
    // Adc_ClrHhtIrqState();
}

void AdcLltIrqCallback(void)
{    
    // 中断标志位判断和清零，已在库函数中处理Adc_IRQHandler();
    
    
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
    stc_adc_cfg_t             stcAdcCfg;
    stc_adc_scan_cfg_t        stcAdcScanCfg;
    stc_adc_irq_t             stcAdcIrq;
    stc_adc_irq_calbakfn_pt_t stcAdcIrqCalbaks;
    stc_bt_config_t           stcBtConfig;
    uint16_t                  u16ArrData;
    uint16_t                  u16InitCntData;
    
        
    DDL_ZERO_STRUCT(stcAdcCfg);
    DDL_ZERO_STRUCT(stcAdcScanCfg);
    DDL_ZERO_STRUCT(stcAdcIrq);
    DDL_ZERO_STRUCT(stcAdcIrqCalbaks);
    DDL_ZERO_STRUCT(stcBtConfig);
    
    Clk_SwitchTo(ClkRCL);
    Clk_SetRCHFreq(ClkFreq24Mhz);
    Clk_SwitchTo(ClkRCH);
     
    Clk_SetPeripheralGate(ClkPeripheralGpio, TRUE);              //GPIO 外设时钟使能
    Clk_SetPeripheralGate(ClkPeripheralBt, TRUE);                //Basetime 外设时钟使能
    
    if (Ok != Clk_SetPeripheralGate(ClkPeripheralAdcBgr, TRUE))  //ADCBGR 外设时钟使能
    {
        return Error;
    } 

    Gpio_SetAnalog(2, 4, TRUE);
    Gpio_SetAnalog(2, 6, TRUE);
    
    
    Adc_Enable();
    M0P_BGR->CR_f.BGR_EN = 0x1u;   //BGR必须使能
    M0P_BGR->CR_f.TS_EN = 0x0u;    //内置温度传感器，视使用需求
    delay100us(1);
    
    stcAdcCfg.enAdcOpMode = AdcScanMode;               //扫描采样模式
    stcAdcCfg.enAdcClkSel = AdcClkSysTDiv1;            //PCLK
    stcAdcCfg.enAdcSampTimeSel = AdcSampTime8Clk;      //8个采样时钟
    //stcAdcCfg.enAdcRefVolSel = RefVolSelInBgr2p5;    //参考电压:内部2.5V(avdd>3V,SPS<=200kHz)
    stcAdcCfg.enAdcRefVolSel = RefVolSelAVDD;          //参考电压:AVDD
    stcAdcCfg.bAdcInBufEn = FALSE;                     //电压跟随器如果使能，SPS采样速率 <=200K
    stcAdcCfg.enAdcTrig0Sel = AdcTrigTimer2;           //Timer2中断触发ADC采样
    stcAdcCfg.enAdcTrig1Sel = AdcTrigDisable;
    Adc_Init(&stcAdcCfg);    
    
    stcAdcIrq.bAdcIrq = TRUE;                            //转换完成中断函数入口配置使能
    stcAdcIrq.bAdcRegCmp = FALSE;
    stcAdcIrq.bAdcHhtCmp = FALSE;
    stcAdcIrq.bAdcLltCmp = FALSE;
    stcAdcIrqCalbaks.pfnAdcContIrq = AdcContIrqCallback; //转换完成中断入口函数
    stcAdcIrqCalbaks.pfnAdcRegIrq = AdcRegIrqCallback;
    stcAdcIrqCalbaks.pfnAdcHhtIrq = AdcHhtIrqCallback;
    stcAdcIrqCalbaks.pfnAdcLltIrq = AdcLltIrqCallback;
    Adc_ConfigIrq(&stcAdcIrq, &stcAdcIrqCalbaks);        //转换中断入口函数配置
    Adc_EnableIrq();                                     //中断使能
    Adc_CmpCfg(&stcAdcIrq);                              //结果比较中断使能/禁止配置
    
    stcAdcScanCfg.u8AdcScanModeCh = ADC_SCAN_CH0_EN 
                                    | ADC_SCAN_CH1_EN;

    
    stcAdcScanCfg.u8AdcSampCnt = 0x1;                   //连续扫描转换次数，保持通道的倍数，2通道 = 0x1+1(1倍)，或者2+1(2倍)……
    Adc_ConfigScanMode(&stcAdcCfg, &stcAdcScanCfg);
    
    stcBtConfig.enGateP = BtPositive;
    stcBtConfig.enGate  = BtGateDisable;    //无门控
    stcBtConfig.enPRS   = BtPCLKDiv256;     //PCLK / 256
    stcBtConfig.enTog   = BtTogDisable;     //TOG输出Disable
    stcBtConfig.enCT    = BtTimer;
    stcBtConfig.enMD    = BtMode2;
    
    Bt_Init(TIM2, &stcBtConfig);
    
    //TIM2中断使能
    Bt_ClearIntFlag(TIM2);
    Bt_EnableIrq(TIM2);
    //EnableNvic(TIM2_IRQn, 3, TRUE);
    
    //设置重载值和计数值，启动计数
    u16ArrData = 0x10000 - 9375;       //24M 主频，100ms触发采样一次：CNT 9375
    u16InitCntData = 0x10000 - 9375;
    Bt_ARRSet(TIM2, u16ArrData);
    Bt_Cnt16Set(TIM2, u16InitCntData);
    Bt_Run(TIM2);
    
    while(1)
    {
        
    }
}

/******************************************************************************
 * EOF (not truncated)
 ******************************************************************************/


