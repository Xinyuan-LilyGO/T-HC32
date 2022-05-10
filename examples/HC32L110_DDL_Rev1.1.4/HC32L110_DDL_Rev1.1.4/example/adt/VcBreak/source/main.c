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
#include "vc.h"
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

void VcIrqCallback(void)
{
    //Vc_DisableIrq(VcChannel0);
    #ifdef DEBUG_PRINT
        printf("VC high IRQ fired!\n");
    #endif
}

void ConfigVc(void)
{
    stc_vc_general_config_t stcVcGeneralCfg;
    stc_vc_channel_config_t stcVcChannelCfg;
    en_vc_channel_t genChannel;

    DDL_ZERO_STRUCT(stcVcGeneralCfg);
    DDL_ZERO_STRUCT(stcVcChannelCfg);
    
    Clk_SetPeripheralGate(ClkPeripheralVcLvd, TRUE);//比较器外设时钟使能
    
    Gpio_SetAnalog(2, 5, TRUE);
    
    genChannel = VcChannel0;

    stcVcGeneralCfg.bDivEn = TRUE;
    stcVcGeneralCfg.enDivVref = VcDivVrefAvcc;
    stcVcGeneralCfg.u8DivVal = 0x20;

    stcVcChannelCfg.enVcChannel = genChannel;
    stcVcChannelCfg.enVcCmpDly = VcDelayoff;
    stcVcChannelCfg.enVcBiasCurrent = VcBias1200na;
    stcVcChannelCfg.enVcFilterTime = VcFilter14us;
    stcVcChannelCfg.enVcInPin_P = VcInPCh1;
    //stcVcChannelCfg.enVcInPin_N = AiBg1p2;
    stcVcChannelCfg.enVcInPin_N = ResDivOut;
    stcVcChannelCfg.enVcOutConfig = VcOutBrake;
    stcVcChannelCfg.enVcIrqSel = VcIrqHigh;
    stcVcChannelCfg.pfnAnalogCmpCb = VcIrqCallback;

    Vc_GeneralInit(&stcVcGeneralCfg);               //VC模块初始化
    Vc_ChannelInit(genChannel, &stcVcChannelCfg);   //VC通道初始化
    Vc_EnableFilter(genChannel);                    //滤波使能
    Vc_ConfigIrq(stcVcChannelCfg.enVcChannel, stcVcChannelCfg.enVcIrqSel);//VC中断配置
    Vc_EnableIrq(stcVcChannelCfg.enVcChannel);      //VC中断使能
    Vc_EnableChannel(stcVcChannelCfg.enVcChannel);  //VC通道使能
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
    en_adt_compare_t enAdtCompareB;
    uint16_t u16CompareB;
    stc_adt_basecnt_cfg_t stcAdtBaseCntCfg;
    stc_adt_CHxX_port_cfg_t stcAdtTIM4ACfg;
    stc_adt_CHxX_port_cfg_t stcAdtTIM4BCfg;
    
    DDL_ZERO_STRUCT(stcAdtBaseCntCfg);
    DDL_ZERO_STRUCT(stcAdtTIM4ACfg);
    DDL_ZERO_STRUCT(stcAdtTIM4BCfg);
    
    Clk_SwitchTo(ClkXTH);
    
    Clk_SetPeripheralGate(ClkPeripheralUart0, TRUE);//串口0外设时钟使能
    Clk_SetPeripheralGate(ClkPeripheralBt, TRUE);   //基础时钟外设时钟使能
    Clk_SetPeripheralGate(ClkPeripheralGpio, TRUE); //端口外设时钟使能
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
    #ifdef DEBUG_PRINT
        printf("\nADT VC Break test start!\n");
    #endif

    stcAdtBaseCntCfg.enCntMode = AdtSawtoothMode;
    stcAdtBaseCntCfg.enCntDir = AdtCntUp;
    stcAdtBaseCntCfg.enCntClkDiv = AdtClkPClk0Div4;
    Adt_Init(enAdt, &stcAdtBaseCntCfg);                      //ADT载波、计数模式、时钟配置
    
    u16Period = 0xC000;
    Adt_SetPeriod(enAdt, u16Period);                         //周期设置
    
    enAdtCompareA = AdtCompareA;
    u16CompareA = 0x6000;
    Adt_SetCompareValue(enAdt, enAdtCompareA, u16CompareA);  //通用比较基准值寄存器A设置
    
    enAdtCompareB = AdtCompareB;
    u16CompareB = 0x6000;
    Adt_SetCompareValue(enAdt, enAdtCompareB, u16CompareB);  //通用比较基准值寄存器B设置
    
    stcAdtTIM4ACfg.enCap = AdtCHxCompareOutput;
    stcAdtTIM4ACfg.bOutEn = TRUE;
    stcAdtTIM4ACfg.enPerc = AdtCHxPeriodLow;
    stcAdtTIM4ACfg.enCmpc = AdtCHxCompareHigh;
    stcAdtTIM4ACfg.enStaStp = AdtCHxStateSelSS;
    stcAdtTIM4ACfg.enStaOut = AdtCHxPortOutLow;
    stcAdtTIM4ACfg.enDisSel = AdtCHxDisSel0;
    stcAdtTIM4ACfg.enDisVal = AdtTIMxDisValLow;
    Adt_CHxXPortConfig(enAdt, AdtCHxA, &stcAdtTIM4ACfg);   //端口CHA配置
    
    stcAdtTIM4BCfg.enCap = AdtCHxCompareOutput;
    stcAdtTIM4BCfg.bOutEn = TRUE;
    stcAdtTIM4BCfg.enPerc = AdtCHxPeriodInv;
    stcAdtTIM4BCfg.enCmpc = AdtCHxCompareInv;
    stcAdtTIM4BCfg.enStaStp = AdtCHxStateSelSS;
    stcAdtTIM4BCfg.enStaOut = AdtCHxPortOutLow;
    stcAdtTIM4BCfg.enDisSel = AdtCHxDisSel0;
    stcAdtTIM4BCfg.enDisVal = AdtTIMxDisValLow;
    Adt_CHxXPortConfig(enAdt, AdtCHxB, &stcAdtTIM4BCfg);   //端口CHB配置
    
    Adt_StartCount(enAdt);
    
    ConfigVc();
    
    while(1);
}

/******************************************************************************
 * EOF (not truncated)
 ******************************************************************************/


