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


/******************************************************************************
 * Global variable definitions (declared in header file with 'extern')
 ******************************************************************************/

/******************************************************************************
 * Local type definitions ('typedef')                                         
 ******************************************************************************/

/******************************************************************************
 * Local function prototypes ('static')
 ******************************************************************************/
static stc_adt_cntstate_cfg_t stcAdt4CntState;
static uint16_t u16Capture;

/******************************************************************************
 * Local variable definitions ('static')                                      *
 ******************************************************************************/


/*****************************************************************************
 * Function implementation - global ('extern') and local ('static')
 ******************************************************************************/

void Adt4CaptureBCalllback(void)
{
    Adt_GetCaptureValue(AdTIM4, AdtCHxB, &u16Capture);
    Adt_GetCntState(AdTIM4, &stcAdt4CntState);
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
    stc_adt_CHxX_port_cfg_t stcAdtTIM4BCfg;
    
    DDL_ZERO_STRUCT(stcAdt4CntState);
    DDL_ZERO_STRUCT(stcAdtBaseCntCfg);
    DDL_ZERO_STRUCT(stcAdtTIM4ACfg);
    DDL_ZERO_STRUCT(stcAdtTIM4BCfg);
    
    Clk_SwitchTo(ClkXTH);
    
    Clk_SetPeripheralGate(ClkPeripheralGpio, TRUE); //端口外设时钟使能
    
    // P23 -> P24
    Gpio_SetFunc_TIM4_CHA_P23();
    Gpio_SetFunc_TIM4_CHB_P24();

    if (Ok != Clk_SetPeripheralGate(ClkPeripheralAdt, TRUE))//ADT外设时钟使能
    {
        return Error;
    }
    
    enAdt = AdTIM4;
    
    stcAdtBaseCntCfg.enCntMode = AdtSawtoothMode;
    stcAdtBaseCntCfg.enCntDir = AdtCntUp;
    stcAdtBaseCntCfg.enCntClkDiv = AdtClkPClk0Div1024;
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
    Adt_CHxXPortConfig(enAdt, AdtCHxA, &stcAdtTIM4ACfg);     //端口CHA配置，比较输出功能
    
    stcAdtTIM4BCfg.enCap = AdtCHxCompareInput;
    Adt_CHxXPortConfig(enAdt, AdtCHxB, &stcAdtTIM4BCfg);     //端口CHB配置，捕获输入功能
    
    Adt_ConfigHwCaptureB(enAdt, AdtHwTrigTimxBRise);         //硬件捕获B条件配置
    
    Adt_ConfigIrq(enAdt, AdtCMBIrq, TRUE, Adt4CaptureBCalllback); //捕获中断B配置
    
    Adt_StartCount(enAdt);
    
    while(1);
}

/******************************************************************************
 * EOF (not truncated)
 ******************************************************************************/


