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


/******************************************************************************
 * Local variable definitions ('static')                                      *
 ******************************************************************************/
static stc_adt_cntstate_cfg_t stcAdt4CntState;
static stc_adt_cntstate_cfg_t stcAdt5CntState;
static stc_adt_cntstate_cfg_t stcAdt6CntState;

/*****************************************************************************
 * Function implementation - global ('extern') and local ('static')
 ******************************************************************************/

 void Adt4CaptureACalllback(void)
{
    static uint16_t u16Tim4Capture;
    
    Adt_GetCaptureValue(AdTIM4, AdtCHxA, &u16Tim4Capture);
}

void Adt5CaptureACalllback(void)
{
    static uint16_t u16Tim5Capture;
    
    Adt_GetCaptureValue(AdTIM5, AdtCHxA, &u16Tim5Capture);
}

void Adt6CaptureACalllback(void)
{
    static uint16_t u16Tim6Capture;
    
    Adt_GetCaptureValue(AdTIM6, AdtCHxA, &u16Tim6Capture);
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
    int32_t i;
    uint16_t u16Period;
    stc_adt_basecnt_cfg_t stcAdtBaseCntCfg;
    stc_adt_port_trig_cfg_t stcAdtPortTrig;
    
    DDL_ZERO_STRUCT(stcAdtBaseCntCfg);
    DDL_ZERO_STRUCT(stcAdtPortTrig);

    DDL_ZERO_STRUCT(stcAdt4CntState);
    DDL_ZERO_STRUCT(stcAdt5CntState);
    DDL_ZERO_STRUCT(stcAdt6CntState);
    
    Clk_SwitchTo(ClkXTH);
    
    Clk_SetPeripheralGate(ClkPeripheralGpio, TRUE); //端口外设时钟使能
    
    // P24 -> P23 Rise -> Time TrigA Rise -> HW Start
    // P24 -> P23 Fall -> Time TrigA Fall -> HW Stop
    // P25 -> P32 Fall -> Time TrigC Fall -> HW Clear
    // P25 -> P32 Rise -> Time TrigC Rise -> HW Capture
    Gpio_InitIO(2, 3, GpioDirIn);                        //P23配置为输入端口
    Gpio_InitIO(3, 2, GpioDirIn);                        //P32配置为输入端口
    
    Gpio_InitIO(2, 4, GpioDirOut);                       //P24配置为输出端口
    Gpio_SetIO(2, 4, FALSE);                             //P24输出Low-Level
    Gpio_InitIO(2, 5, GpioDirOut);                       //P25配置为输出端口
    Gpio_SetIO(2, 5, FALSE);                             //P25输出Low-Level
    
    if (Ok != Clk_SetPeripheralGate(ClkPeripheralAdt, TRUE))//ADT外设时钟使能
    {
        return Error;
    }
    
    stcAdtBaseCntCfg.enCntMode = AdtSawtoothMode;
    stcAdtBaseCntCfg.enCntDir = AdtCntUp;
    stcAdtBaseCntCfg.enCntClkDiv = AdtClkPClk0Div256;
    Adt_Init(AdTIM4, &stcAdtBaseCntCfg);
    Adt_Init(AdTIM5, &stcAdtBaseCntCfg);
    Adt_Init(AdTIM6, &stcAdtBaseCntCfg);                      //ADT载波、计数模式、时钟配置

    u16Period = 0xC000;
    Adt_SetPeriod(AdTIM4, u16Period);
    Adt_SetPeriod(AdTIM5, u16Period);
    Adt_SetPeriod(AdTIM6, u16Period);                         //周期设置
    
    stcAdtPortTrig.enTrigSrc = AdtTrigxSelP23;
    stcAdtPortTrig.bFltEn = TRUE;
    stcAdtPortTrig.enFltClk = AdtFltClkPclk0Div64;
    Adt_PortTrigConfig(AdtTrigA, &stcAdtPortTrig);           //TRIGA输入配置
    
    stcAdtPortTrig.enTrigSrc = AdtTrigxSelP32;
    stcAdtPortTrig.bFltEn = TRUE;
    stcAdtPortTrig.enFltClk = AdtFltClkPclk0Div64;
    Adt_PortTrigConfig(AdtTrigC, &stcAdtPortTrig);           //TRIGC输入配置
    
    Adt_ConfigHwStart(AdTIM4, AdtHwTrigTimTriARise);         //硬件开始条件：TIMTRIA端口上采样到上升沿
    Adt_ConfigHwStop(AdTIM4, AdtHwTrigTimTriAFall);          //硬件停止条件：TIMTRIA端口上采样到下降沿
    Adt_ConfigHwClear(AdTIM4, AdtHwTrigTimTriCFall);         //硬件清零条件：TIMTRIC端口上采样到下降沿
    Adt_ConfigHwCaptureA(AdTIM4, AdtHwTrigTimTriCRise);      //硬件捕获：TIMTRIC端口上采样到上升沿
    Adt_EnableHwStart(AdTIM4);
    Adt_EnableHwStop(AdTIM4);
    Adt_EnableHwClear(AdTIM4);
    
    Adt_ConfigHwStart(AdTIM5, AdtHwTrigTimTriARise);
    Adt_ConfigHwStop(AdTIM5, AdtHwTrigTimTriAFall);
    Adt_ConfigHwClear(AdTIM5, AdtHwTrigTimTriCFall);
    Adt_ConfigHwCaptureA(AdTIM5, AdtHwTrigTimTriCRise);
    Adt_EnableHwStart(AdTIM5);
    Adt_EnableHwStop(AdTIM5);
    Adt_EnableHwClear(AdTIM5);
    
    Adt_ConfigHwStart(AdTIM6, AdtHwTrigTimTriARise);
    Adt_ConfigHwStop(AdTIM6, AdtHwTrigTimTriAFall);
    Adt_ConfigHwClear(AdTIM6, AdtHwTrigTimTriCFall);
    Adt_ConfigHwCaptureA(AdTIM6, AdtHwTrigTimTriCRise);
    Adt_EnableHwStart(AdTIM6);
    Adt_EnableHwStop(AdTIM6);
    Adt_EnableHwClear(AdTIM6);
    
    Adt_ConfigIrq(AdTIM4, AdtCMAIrq, TRUE, Adt4CaptureACalllback);
    Adt_ConfigIrq(AdTIM5, AdtCMAIrq, TRUE, Adt5CaptureACalllback);
    Adt_ConfigIrq(AdTIM6, AdtCMAIrq, TRUE, Adt6CaptureACalllback);//配置捕获中断
    
    Gpio_SetIO(2, 4, TRUE);    //HW start
        
    for (i = 0; i < 10; i++)
    {
        delay1ms(1000);        // TODO delay1ms need update
        Adt_GetCntState(AdTIM4, &stcAdt4CntState);
        Adt_GetCntState(AdTIM5, &stcAdt5CntState);
        Adt_GetCntState(AdTIM6, &stcAdt6CntState);

    }
    delay1ms(1000);

    Gpio_SetIO(2, 5, TRUE);   //HW capture
    delay1ms(1000);
    
    Gpio_SetIO(2, 4, FALSE);  //HW stop

    for (i = 0; i < 3; i++)
    {
        delay1ms(1000);
        Adt_GetCntState(AdTIM4, &stcAdt4CntState);
        Adt_GetCntState(AdTIM5, &stcAdt5CntState);
        Adt_GetCntState(AdTIM6, &stcAdt6CntState);
    }
    
    Gpio_SetIO(2, 5, FALSE);  //HW clear
    delay1ms(1000);
    
    for (i = 0; i < 3; i++) 
    {
        delay1ms(1000);
        Adt_GetCntState(AdTIM4, &stcAdt4CntState);
        Adt_GetCntState(AdTIM5, &stcAdt5CntState);
        Adt_GetCntState(AdTIM6, &stcAdt6CntState);
    }
    
    while(1);
}

/******************************************************************************
 * EOF (not truncated)
 ******************************************************************************/


