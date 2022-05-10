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
 **   - 2017-06-20 LiuHL    First Version
 **
 ******************************************************************************/

/******************************************************************************
 * Include files
 ******************************************************************************/
#include "adt.h"
#include "gpio.h"

/******************************************************************************
 * Local pre-processor symbols/macros ('#define')                            
 ******************************************************************************/
#define DEBUG_PRINT

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
    en_adt_unit_t enAdt;
    uint16_t u16Period;
    en_adt_compare_t enAdtCompareA;
    uint16_t u16CompareA;
    en_adt_compare_t enAdtCompareB;
    uint16_t u16CompareB;
    stc_adt_basecnt_cfg_t stcAdtBaseCntCfg;
    stc_adt_CHxX_port_cfg_t stcAdtTIM4ACfg;
    stc_adt_CHxX_port_cfg_t stcAdtTIM4BCfg;
    stc_adt_disable_1_cfg_t stcAdtDisable1;
    stc_adt_cntstate_cfg_t stcAdtCntState;
    
    DDL_ZERO_STRUCT(stcAdtBaseCntCfg);
    DDL_ZERO_STRUCT(stcAdtTIM4ACfg);
    DDL_ZERO_STRUCT(stcAdtTIM4BCfg);
    DDL_ZERO_STRUCT(stcAdtDisable1);
    DDL_ZERO_STRUCT(stcAdtCntState);
    
    Clk_SwitchTo(ClkXTH);
    
    Clk_SetPeripheralGate(ClkPeripheralUart0, TRUE);    //串口0外设时钟使能
    Clk_SetPeripheralGate(ClkPeripheralBt, TRUE);       //基础时钟外设时钟使能
    Clk_SetPeripheralGate(ClkPeripheralGpio, TRUE);     //端口外设时钟使能
    #ifdef DEBUG_PRINT
        Debug_UartInit();
    #endif
    
    Gpio_SetFunc_TIM4_CHA_P23();
    Gpio_SetFunc_TIM4_CHB_P24();
    
    if (Ok != Clk_SetPeripheralGate(ClkPeripheralAdt, TRUE))//ADT外设时钟使能
    {
        return Error;
    }
    enAdt = AdTIM4;
    
    
    
    stcAdtBaseCntCfg.enCntMode = AdtSawtoothMode;
    stcAdtBaseCntCfg.enCntDir = AdtCntUp;
    stcAdtBaseCntCfg.enCntClkDiv = AdtClkPClk0Div64;
    Adt_Init(enAdt, &stcAdtBaseCntCfg);                      //ADT载波、计数模式、时钟配置
    
    #ifdef DEBUG_PRINT
        printf("\n***************************************\n");
        printf("ADT SameH Break test start!\n");
    #endif
    u16Period = 0xC000;
    Adt_SetPeriod(enAdt, u16Period);                         //周期设置
    
    enAdtCompareA = AdtCompareA;
    u16CompareA = 0x6000;
    Adt_SetCompareValue(enAdt, enAdtCompareA, u16CompareA);  //通用比较基准值寄存器A设置
    
    enAdtCompareB = AdtCompareB;
    u16CompareB = 0x8000;
    Adt_SetCompareValue(enAdt, enAdtCompareB, u16CompareB);  //通用比较基准值寄存器B设置
    
    stcAdtTIM4ACfg.enCap = AdtCHxCompareOutput;
    stcAdtTIM4ACfg.bOutEn = TRUE;
    stcAdtTIM4ACfg.enPerc = AdtCHxPeriodLow;
    stcAdtTIM4ACfg.enCmpc = AdtCHxCompareHigh;
    stcAdtTIM4ACfg.enStaStp = AdtCHxStateSelSS;
    stcAdtTIM4ACfg.enStaOut = AdtCHxPortOutLow;
    stcAdtTIM4ACfg.enStpOut = AdtCHxPortOutLow;
    stcAdtTIM4ACfg.enDisSel = AdtCHxDisSel1;
    stcAdtTIM4ACfg.enDisVal = AdtTIMxDisValNorm;
    Adt_CHxXPortConfig(enAdt, AdtCHxA, &stcAdtTIM4ACfg);   //端口CHA配置
                                
    stcAdtTIM4BCfg.enCap = AdtCHxCompareOutput;
    stcAdtTIM4BCfg.bOutEn = TRUE;
    stcAdtTIM4BCfg.enPerc = AdtCHxPeriodLow;
    stcAdtTIM4BCfg.enCmpc = AdtCHxCompareHigh;
    stcAdtTIM4BCfg.enStaStp = AdtCHxStateSelSS;
    stcAdtTIM4BCfg.enStaOut = AdtCHxPortOutLow;
    stcAdtTIM4BCfg.enStpOut = AdtCHxPortOutLow;
    stcAdtTIM4BCfg.enDisSel = AdtCHxDisSel1;
    stcAdtTIM4BCfg.enDisVal = AdtTIMxDisValNorm;
    Adt_CHxXPortConfig(enAdt, AdtCHxB, &stcAdtTIM4BCfg);   //端口CHB配置
    Adt_StartCount(enAdt);
    
    DDL_ZERO_STRUCT(stcAdtDisable1);
    stcAdtDisable1.bTim4OutSH = TRUE;                       //Timer4输出同高检测使能
    Adt_Disable1Cfg(&stcAdtDisable1);                       //无效条件1配置
    
    while(!Adt_GetSameBrakeFlag());
    Adt_StopCount(enAdt);
    stcAdtDisable1.bTim4OutSH = FALSE;                      //Timer4输出同高检测解除
    Adt_Disable1Cfg(&stcAdtDisable1);                       //无效条件1配置
    Adt_ClearSameBrakeFlag();
    Adt_GetCntState(enAdt, &stcAdtCntState);
    #ifdef DEBUG_PRINT
        printf("SameH Break! Count val 0x%X, exp 0x%x.\n", stcAdtCntState.u16Counter, u16CompareB);
    #endif

    Adt_ClearCount(enAdt);
    
    #ifdef DEBUG_PRINT
        printf("\n***************************************\n");
        printf("ADT SameL Break test start!\n");
    #endif
    u16Period = 0xC000;
    Adt_SetPeriod(enAdt, u16Period);
    
    enAdtCompareA = AdtCompareA;
    u16CompareA = 0x6000;
    Adt_SetCompareValue(enAdt, enAdtCompareA, u16CompareA);
    
    enAdtCompareB = AdtCompareB;
    u16CompareB = 0xA000;
    Adt_SetCompareValue(enAdt, enAdtCompareB, u16CompareB);
    
    stcAdtTIM4ACfg.enCap = AdtCHxCompareOutput;
    stcAdtTIM4ACfg.bOutEn = TRUE;
    stcAdtTIM4ACfg.enPerc = AdtCHxPeriodHigh;
    stcAdtTIM4ACfg.enCmpc = AdtCHxCompareLow;
    stcAdtTIM4ACfg.enStaStp = AdtCHxStateSelSS;
    stcAdtTIM4ACfg.enStaOut = AdtCHxPortOutHigh;
    stcAdtTIM4ACfg.enStpOut = AdtCHxPortOutHigh;
    stcAdtTIM4ACfg.enDisSel = AdtCHxDisSel1;
    stcAdtTIM4ACfg.enDisVal = AdtTIMxDisValNorm;
    Adt_CHxXPortConfig(enAdt, AdtCHxA, &stcAdtTIM4ACfg);
                                
    stcAdtTIM4BCfg.enCap = AdtCHxCompareOutput;
    stcAdtTIM4BCfg.bOutEn = TRUE;
    stcAdtTIM4BCfg.enPerc = AdtCHxPeriodHigh;
    stcAdtTIM4BCfg.enCmpc = AdtCHxCompareLow;
    stcAdtTIM4BCfg.enStaStp = AdtCHxStateSelSS;
    stcAdtTIM4BCfg.enStaOut = AdtCHxPortOutHigh;
    stcAdtTIM4BCfg.enStpOut = AdtCHxPortOutHigh;
    stcAdtTIM4BCfg.enDisSel = AdtCHxDisSel1;
    stcAdtTIM4BCfg.enDisVal = AdtTIMxDisValNorm;
    Adt_CHxXPortConfig(enAdt, AdtCHxB, &stcAdtTIM4BCfg);
    Adt_StartCount(enAdt);
    
    DDL_ZERO_STRUCT(stcAdtDisable1);
    stcAdtDisable1.bTim4OutSL = TRUE;                       //Timer4输出同低检测使能
    Adt_Disable1Cfg(&stcAdtDisable1);                       //无效条件1配置
    
    while(!Adt_GetSameBrakeFlag());
    Adt_StopCount(enAdt);
    stcAdtDisable1.bTim4OutSL = FALSE;                      //Timer4输出同低检测解除
    Adt_Disable1Cfg(&stcAdtDisable1);                       //无效条件1配置
    Adt_ClearSameBrakeFlag();
    
    Adt_GetCntState(enAdt, &stcAdtCntState);
    #ifdef DEBUG_PRINT
        printf("SameL Break! Count val 0x%X, exp 0x%X.\n", stcAdtCntState.u16Counter, u16CompareB);
    #endif

    Adt_ClearCount(enAdt);
    
    while(1);
}

/******************************************************************************
 * EOF (not truncated)
 ******************************************************************************/


