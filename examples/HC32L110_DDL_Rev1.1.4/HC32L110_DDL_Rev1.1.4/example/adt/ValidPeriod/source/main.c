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
 **   - 2017-06-26 LiuHL    First Version
 **
 ******************************************************************************/

/******************************************************************************
 * Include files
 ******************************************************************************/
#include "adt.h"
#include "adc.h"
#include "gpio.h"

/******************************************************************************
 * Local pre-processor symbols/macros ('#define')                            
 ******************************************************************************/
//#define DEBUG_PRINT

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
static stc_adt_cntstate_cfg_t stcAdtCntState;

/*****************************************************************************
 * Function implementation - global ('extern') and local ('static')
 ******************************************************************************/

void Adt4CompACalllback(void)
{
    #ifdef DEBUG_PRINT
        printf("Comp A Int!\n");
    #endif
}

void AdcContIrqCallback(void)
{
    uint16_t u16AdcResult;
    Adc_GetResult(&u16AdcResult);
    #ifdef DEBUG_PRINT
        printf("AdcVal: 0x%X\n", u16AdcResult);
    #endif
}

void AdcRegIrqCallback(void)
{
    uint16_t u16AdcResult;
    Adc_GetResult(&u16AdcResult);
    #ifdef DEBUG_PRINT
        printf("Reg IRQ. AdcVal: 0x%X\n", u16AdcResult);
    #endif
}

void AdcLltIrqCallback(void)
{
    uint16_t u16AdcResult;
    Adc_GetResult(&u16AdcResult);
    #ifdef DEBUG_PRINT
        printf("Llt IRQ. AdcVal: 0x%X\n", u16AdcResult);
    #endif
}

void AdcHhtIrqCallback(void)
{
    uint16_t u16AdcResult;
    Adc_GetResult(&u16AdcResult);
    #ifdef DEBUG_PRINT
        printf("Hht IRQ. AdcVal: 0x%X\n", u16AdcResult);
    #endif
}

void ConfigAdc(void)
{
    stc_adc_cfg_t stcAdcCfg;
    stc_adc_norm_cfg_t stcAdcNormCfg;
    stc_adc_irq_t stcAdcIrq;
    stc_adc_irq_calbakfn_pt_t stcAdcIrqCalbaks;
    
    DDL_ZERO_STRUCT(stcAdcCfg);
    DDL_ZERO_STRUCT(stcAdcNormCfg);
    DDL_ZERO_STRUCT(stcAdcIrq);
    DDL_ZERO_STRUCT(stcAdcIrqCalbaks);
    
    Clk_SetPeripheralGate(ClkPeripheralAdcBgr, TRUE);//ADC外设时钟使能
    Gpio_SetAnalog(2, 6, TRUE);                      //P26作为ADC采样口
    
    Adc_Enable();
    M0P_BGR->CR_f.BGR_EN = 0x1u;                     //BGR使能
    M0P_BGR->CR_f.TS_EN = 0x0u;
    delay100us(1);
    
    stcAdcCfg.enAdcOpMode = AdcNormalMode;
    stcAdcCfg.enAdcClkSel = AdcClkSysTDiv8;
    stcAdcCfg.enAdcSampTimeSel = AdcSampTime4Clk;
    //stcAdcCfg.enAdcRefVolSel = RefVolSelExtern1;
    stcAdcCfg.enAdcRefVolSel = RefVolSelAVDD;
    stcAdcCfg.bAdcInBufEn = FALSE;
    stcAdcCfg.u32AdcRegHighThd = 0xFFFu;
    stcAdcCfg.u32AdcRegLowThd = 0;
    stcAdcCfg.enAdcTrig0Sel = AdcTrigTimer4;
    stcAdcCfg.enAdcTrig1Sel = AdcTrigDisable;
    Adc_Init(&stcAdcCfg);                            //ADC初始化
    
    stcAdcIrq.bAdcIrq = TRUE;
    stcAdcIrq.bAdcRegCmp = TRUE;
    stcAdcIrq.bAdcHhtCmp = TRUE;
    stcAdcIrq.bAdcLltCmp = TRUE;
    stcAdcIrqCalbaks.pfnAdcContIrq = AdcContIrqCallback;
    stcAdcIrqCalbaks.pfnAdcRegIrq = AdcRegIrqCallback;
    stcAdcIrqCalbaks.pfnAdcHhtIrq = AdcHhtIrqCallback;
    stcAdcIrqCalbaks.pfnAdcLltIrq = AdcLltIrqCallback;
    Adc_ConfigIrq(&stcAdcIrq, &stcAdcIrqCalbaks);    //中断入口函数配置
    Adc_EnableIrq();                                 //中断使能
    Adc_CmpCfg(&stcAdcIrq);                          //比较使能
    
    stcAdcNormCfg.enAdcNormModeCh = AdcExInputCH1;   //选择P26通道作为采样端口
    stcAdcNormCfg.bAdcResultAccEn = FALSE;
    Adc_ConfigNormMode(&stcAdcCfg, &stcAdcNormCfg);  //普通转换模式配置
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
    en_adt_unit_t enAdt;
    uint16_t u16Period;
    en_adt_compare_t enAdtCompareA;
    uint16_t u16CompareA;
    
    stc_adt_basecnt_cfg_t stcAdtBaseCntCfg;
    stc_adt_CHxX_port_cfg_t stcAdtTIM4ACfg;
    stc_adt_validper_cfg_t stcAdtValidPerCfg;
    stc_adt_irq_trig_cfg_t stcAdtIrqTrigCfg;
    
    DDL_ZERO_STRUCT(stcAdtCntState);
    DDL_ZERO_STRUCT(stcAdtBaseCntCfg);
    DDL_ZERO_STRUCT(stcAdtTIM4ACfg);
    DDL_ZERO_STRUCT(stcAdtValidPerCfg);
    DDL_ZERO_STRUCT(stcAdtIrqTrigCfg);
    
    Clk_SwitchTo(ClkXTH);
    
    #ifdef DEBUG_PRINT
    Clk_SetPeripheralGate(ClkPeripheralUart0, TRUE);//串口0外设时钟使能
    Clk_SetPeripheralGate(ClkPeripheralBt, TRUE);   //基础时钟外设时钟使能
    #endif
    Clk_SetPeripheralGate(ClkPeripheralGpio, TRUE); //端口外设时钟使能
    #ifdef DEBUG_PRINT
        Debug_UartInit();
    #endif
        
    // Gpio_SetFunc_TIM4_CHA_P23();
    // Gpio_SetFunc_TIM4_CHB_P24();
        
    if (Ok != Clk_SetPeripheralGate(ClkPeripheralAdt, TRUE))//ADT外设时钟使能
    {
        return Error;
    }
    enAdt = AdTIM4;
    #ifdef DEBUG_PRINT
        printf("ADT %d valid period test start!\n", enAdt);
    #endif
    
    stcAdtBaseCntCfg.enCntMode = AdtSawtoothMode;
    stcAdtBaseCntCfg.enCntDir = AdtCntUp;
    stcAdtBaseCntCfg.enCntClkDiv = AdtClkPClk0Div256;
    Adt_Init(enAdt, &stcAdtBaseCntCfg);                      //ADT载波、计数模式、时钟配置
    
    u16Period = 0xC000;
    Adt_SetPeriod(enAdt, u16Period);                         //周期设置
    
    enAdtCompareA = AdtCompareA;
    u16CompareA = 0x6000;
    Adt_SetCompareValue(enAdt, enAdtCompareA, u16CompareA);  //通用比较基准值寄存器A设置


    stcAdtTIM4ACfg.enCap = AdtCHxCompareOutput;
    stcAdtTIM4ACfg.bOutEn = TRUE;
    stcAdtTIM4ACfg.enPerc = AdtCHxPeriodLow;
    stcAdtTIM4ACfg.enCmpc = AdtCHxCompareHigh;
    stcAdtTIM4ACfg.enStaStp = AdtCHxStateSelSS;
    stcAdtTIM4ACfg.enStaOut = AdtCHxPortOutLow;
    Adt_CHxXPortConfig(enAdt, AdtCHxA, &stcAdtTIM4ACfg);   //端口CHA配置
    
    stcAdtValidPerCfg.enValidCnt = AdtPeriodCnts4;
    stcAdtValidPerCfg.enValidCdt = AdtPeriodCnteMax;
    stcAdtValidPerCfg.bPeriodA = TRUE;
    Adt_SetValidPeriod(enAdt, &stcAdtValidPerCfg);           //间隔有效期设置
    
    Adt_ConfigIrq(enAdt, AdtCMAIrq, TRUE, Adt4CompACalllback);//计数比较匹配A中断使能配置
    
    stcAdtIrqTrigCfg.bAdtCntMatchATrigEn = TRUE;
    Adt_IrqTrigConfig(enAdt, &stcAdtIrqTrigCfg);              //计数匹配A使能触发ADC
    
    ConfigAdc();
    
    Adt_StartCount(enAdt);
    
    while(1);
}

/******************************************************************************
 * EOF (not truncated)
 ******************************************************************************/


