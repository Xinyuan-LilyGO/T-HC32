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
#include "bt.h"
#include "gpio.h"

/******************************************************************************
 * Local pre-processor symbols/macros ('#define')                            
 ******************************************************************************/
#define TEST_PORT    0
#define TEST_PIN    3

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
static stc_adt_cntstate_cfg_t stcAdt4CntState;
static stc_adt_cntstate_cfg_t stcAdt5CntState;
static stc_adt_cntstate_cfg_t stcAdt6CntState;
static uint32_t u32Exp = 0;

/*****************************************************************************
 * Function implementation - global ('extern') and local ('static')
 ******************************************************************************/
 
void Bt0Int(void)
{
    Bt_ClearIntFlag(TIM0);
    Bt_Stop(TIM0);
}

void Bt1Int(void)
{
    int i = 0;
    Bt_ClearIntFlag(TIM1);
    Bt_Stop(TIM1);
    Adt_GetCntState(AdTIM4, &stcAdt4CntState);
    Adt_GetCntState(AdTIM5, &stcAdt5CntState);
    Adt_GetCntState(AdTIM6, &stcAdt6CntState);
    
    if ((stcAdt4CntState.u16Counter == u32Exp)
        && (stcAdt5CntState.u16Counter == u32Exp)
        && (stcAdt6CntState.u16Counter == u32Exp))
    {
        while(1)
        {
            if ((i++)%2)
            {
                Gpio_SetIO(TEST_PORT, TEST_PIN, FALSE);
            }
            else
            {
                Gpio_SetIO(TEST_PORT, TEST_PIN, TRUE);
            }
            delay1ms(1000);
        }
    }    
}

void Bt2Int(void)
{
    Bt_ClearIntFlag(TIM2);
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
    uint32_t u32Cnt1;
    uint32_t u32Cnt2;
    uint16_t u16Period;
    uint16_t u16Cnt;
    uint16_t u16Reload;

    stc_adt_basecnt_cfg_t stcAdtBaseCntCfg;
    stc_adt_aos_trig_cfg_t stcAdtAosTrigCfg;
    stc_bt_config_t   stcTm0Config;
    stc_bt_config_t   stcTm1Config;
    stc_bt_config_t   stcTm2Config;

    DDL_ZERO_STRUCT(stcAdtBaseCntCfg);
    DDL_ZERO_STRUCT(stcAdtAosTrigCfg);

    DDL_ZERO_STRUCT(stcAdt4CntState);
    DDL_ZERO_STRUCT(stcAdt5CntState);
    DDL_ZERO_STRUCT(stcAdt6CntState);
    
    DDL_ZERO_STRUCT(stcTm0Config);
    DDL_ZERO_STRUCT(stcTm1Config);
    DDL_ZERO_STRUCT(stcTm2Config);

    Clk_SwitchTo(ClkXTH);

    Clk_SetPeripheralGate(ClkPeripheralUart0, TRUE);//串口0外设时钟使能
    Clk_SetPeripheralGate(ClkPeripheralBt, TRUE);   //基础时钟外设时钟使能
    Clk_SetPeripheralGate(ClkPeripheralGpio, TRUE); //端口外设时钟使能
    #ifdef DEBUG_PRINT
        //Debug_UartInit();
    #endif

    // TM0 IRQ -> AOS0 trigger-> HW Start
    // TM1 IRQ -> AOS1 trigger -> HW Stop
    // TM2 IRQ -> AOS2 trigger -> HW count up clock

    Gpio_InitIO(TEST_PORT, TEST_PIN, GpioDirOut);
    Gpio_SetIO(TEST_PORT, TEST_PIN, FALSE);
    
    if (Ok != Clk_SetPeripheralGate(ClkPeripheralAdt, TRUE)) //ADT外设时钟使能
    {
        return Error;
    }
    
    stcAdtBaseCntCfg.enCntMode = AdtSawtoothMode;
    // stcAdtBaseCntCfg.enCntDir = AdtCntUp;
    // stcAdtBaseCntCfg.enCntClkDiv = AdtClkPClk0Div256;
    Adt_Init(AdTIM4, &stcAdtBaseCntCfg);
    Adt_Init(AdTIM5, &stcAdtBaseCntCfg);
    Adt_Init(AdTIM6, &stcAdtBaseCntCfg);                      //ADT载波配置
    
    Adt_StopCount(AdTIM4);
    Adt_StopCount(AdTIM5);
    Adt_StopCount(AdTIM6);
    Adt_ClearCount(AdTIM4);
    Adt_ClearCount(AdTIM5);
    Adt_ClearCount(AdTIM6);

    u16Period = 0xC000;
    Adt_SetPeriod(AdTIM4, u16Period);
    Adt_SetPeriod(AdTIM5, u16Period);
    Adt_SetPeriod(AdTIM6, u16Period);                         //周期设置

    // AOS trigger config
    stcAdtAosTrigCfg.enAos0TrigSrc = AdtAosxTrigSelTim0Int;
    stcAdtAosTrigCfg.enAos1TrigSrc = AdtAosxTrigSelTim1Int;
    stcAdtAosTrigCfg.enAos2TrigSrc = AdtAosxTrigSelTim2Int;
    Adt_AosTrigConfig(&stcAdtAosTrigCfg);                     //AOS事件源配置
                                   
    Adt_ConfigHwStart(AdTIM4, AdtHwTrigAos0);                 //Timer4硬件启动 条件配置
    Adt_ConfigHwStop(AdTIM4, AdtHwTrigAos1);                  //Timer4硬件停止 条件配置
    Adt_ConfigHwCntUp(AdTIM4, AdtHwCntAos2);                  //Timer4硬件递加计数 条件配置
    Adt_EnableHwStart(AdTIM4);
    Adt_EnableHwStop(AdTIM4);                                 //Timer4硬件启动和停止使能
    
    Adt_ConfigHwStart(AdTIM5, AdtHwTrigAos0);
    Adt_ConfigHwStop(AdTIM5, AdtHwTrigAos1);
    Adt_ConfigHwCntUp(AdTIM5, AdtHwCntAos2);
    Adt_EnableHwStart(AdTIM5);
    Adt_EnableHwStop(AdTIM5);
    
    Adt_ConfigHwStart(AdTIM6, AdtHwTrigAos0);
    Adt_ConfigHwStop(AdTIM6, AdtHwTrigAos1);
    Adt_ConfigHwCntUp(AdTIM6, AdtHwCntAos2);
    Adt_EnableHwStart(AdTIM6);
    Adt_EnableHwStop(AdTIM6);
    
    // Adt_ConfigIrq(AdTIM4, AdtCMAIrq, TRUE, Adt4CaptureACalllback);
    // Adt_ConfigIrq(AdTIM5, AdtCMAIrq, TRUE, Adt5CaptureACalllback);
    // Adt_ConfigIrq(AdTIM6, AdtCMAIrq, TRUE, Adt6CaptureACalllback);
    
    Bt_Stop(TIM2);
    Bt_Stop(TIM0);
    Bt_Stop(TIM1);
    
    stcTm0Config.enGateP = BtPositive;
    stcTm0Config.enGate  = BtGateDisable;
    stcTm0Config.enPRS   = BtPCLKDiv16;
    stcTm0Config.enTog   = BtTogDisable;
    stcTm0Config.enCT    = BtTimer;
    stcTm0Config.enMD    = BtMode1;            //32位自由计数
    stcTm0Config.pfnTim0Cb = Bt0Int;
    Bt_Init(TIM0, &stcTm0Config);              //Timer0初始化
    u32Cnt1 = 0xFFFF0000;
    Bt_Cnt32Set(TIM0, u32Cnt1);                //Timer0计数值初始设置

    stcTm1Config.enGateP = BtPositive;
    stcTm1Config.enGate  = BtGateDisable;
    stcTm1Config.enPRS   = BtPCLKDiv16;
    stcTm1Config.enTog   = BtTogDisable;
    stcTm1Config.enCT    = BtTimer;
    stcTm1Config.enMD    = BtMode1;            //32位自由计数
    stcTm1Config.pfnTim1Cb = Bt1Int;
    Bt_Init(TIM1, &stcTm1Config);
    u32Cnt2 = 0xFFF00000;
    Bt_Cnt32Set(TIM1, u32Cnt2);
    
    stcTm2Config.enGateP = BtPositive;
    stcTm2Config.enGate  = BtGateDisable;
    stcTm2Config.enPRS   = BtPCLKDiv16;
    stcTm2Config.enTog   = BtTogDisable;
    stcTm2Config.enCT    = BtTimer;
    stcTm2Config.enMD    = BtMode2;            //16位重载计数
    stcTm2Config.pfnTim2Cb = Bt2Int;
    Bt_Init(TIM2, &stcTm2Config);
    u16Cnt = 0xFF00;
    u16Reload = 0xFF00;
    Bt_ARRSet(TIM2, u16Reload);
    Bt_Cnt16Set(TIM2, u16Cnt);
    
    u32Exp = (u32Cnt1-u32Cnt2)/(0x10000-u16Reload);
    
    EnableNvic(TIM0_IRQn, 3, TRUE);
    EnableNvic(TIM1_IRQn, 3, TRUE);
    EnableNvic(TIM2_IRQn, 3, TRUE);
    Bt_EnableIrq (TIM2);        // Tim2 IRQ -> count clock
    Bt_EnableIrq (TIM0);        // Tim0 IRQ -> start
    Bt_EnableIrq (TIM1);        // Tim1 IRQ -> stop
    
    Bt_Run(TIM2);
    Bt_Run(TIM0);
    Bt_Run(TIM1);
    
    while (1);
}

/******************************************************************************
 * EOF (not truncated)
 ******************************************************************************/


