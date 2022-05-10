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

void genClkIn(void)
{
    uint32_t i;
    stc_adt_cntstate_cfg_t stcAdtCntState;
    boolean_t bAin[17] = {FALSE, TRUE, TRUE, FALSE,
                        FALSE, TRUE, TRUE, FALSE,
                        FALSE,FALSE,
                        FALSE, TRUE, TRUE, FALSE,
                        FALSE, TRUE, TRUE};
    boolean_t bBin[17] = {TRUE, TRUE, FALSE, FALSE,
                        TRUE, TRUE, FALSE, FALSE,
                        TRUE, TRUE, FALSE, FALSE,
                        TRUE, TRUE, FALSE, FALSE,
                        TRUE};

    DDL_ZERO_STRUCT(stcAdtCntState);

    for (i = 0; i < 17; i++)
    {
        Adt_GetCntState(AdTIM5, &stcAdtCntState);
        #ifdef DEBUG_PRINT
            printf("Cnt = %d\n", stcAdtCntState.u16Counter);
        #endif
        Gpio_SetIO(2, 4, bAin[i]);
        Gpio_SetIO(2, 5, bBin[i]);
        delay1ms(1000);
    }
    Adt_GetCntState(AdTIM5, &stcAdtCntState);
    #ifdef DEBUG_PRINT
        printf("Cnt = %d\n", stcAdtCntState.u16Counter);
    #endif
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
    stc_adt_CHxX_port_cfg_t stcAdtTIM5ACfg;
    stc_adt_CHxX_port_cfg_t stcAdtTIM5BCfg;

    DDL_ZERO_STRUCT(stcAdtTIM5ACfg);
    DDL_ZERO_STRUCT(stcAdtTIM5BCfg);

    Clk_SwitchTo(ClkXTH);

    Clk_SetPeripheralGate(ClkPeripheralUart0, TRUE);//串口0外设时钟使能
    Clk_SetPeripheralGate(ClkPeripheralBt, TRUE);   //基础时钟外设时钟使能
    Clk_SetPeripheralGate(ClkPeripheralGpio, TRUE); //端口外设时钟使能
    #ifdef DEBUG_PRINT
    Debug_UartInit();
    #endif

    Gpio_SetFunc_TIM5_CHA_P34();
    Gpio_SetFunc_TIM5_CHB_P33();

    // P24 AIN, P25 BIN
    Gpio_InitIO(2, 4, GpioDirOut);
    Gpio_InitIO(2, 5, GpioDirOut);

    if (Ok != Clk_SetPeripheralGate(ClkPeripheralAdt, TRUE))//ADT外设时钟使能
    {
        return Error;
    }

    stcAdtTIM5ACfg.bFltEn = TRUE;
    stcAdtTIM5ACfg.enFltClk = AdtFltClkPclk0Div64;
    Adt_CHxXPortConfig(AdTIM5, AdtCHxA, &stcAdtTIM5ACfg);//端口CHA默认输入，滤波使能

    stcAdtTIM5BCfg.bFltEn = TRUE;
    stcAdtTIM5BCfg.enFltClk = AdtFltClkPclk0Div64;
    Adt_CHxXPortConfig(AdTIM5, AdtCHxB, &stcAdtTIM5BCfg);//端口CHB默认输入，滤波使能

    Gpio_SetIO(2, 4, FALSE);
    Gpio_SetIO(2, 5, TRUE);
    delay1ms(1000);

    #ifdef DEBUG_PRINT
        printf("*************************************\n");
        printf("ADT phase count 1x\n");
    #endif
    Adt_ClearHwCntUp(AdTIM5);
    Adt_ClearHwCntDwn(AdTIM5);
    Adt_ConfigHwCntUp(AdTIM5, AdtHwCntTimxBHighTimxARise);//硬件递加计数条件：CHxB端口为高电平时，CHxA端口上采样到上升沿
    Adt_ConfigHwCntDwn(AdTIM5, AdtHwCntTimxBLowTimxARise);//硬件递减计数条件：CHxB端口为低电平时，CHxA端口上采样到上升沿
    Adt_SetCount(AdTIM5, 3);
    Adt_StartCount(AdTIM5);
    genClkIn();
    Adt_StopCount(AdTIM5);
    Adt_ClearCount(AdTIM5);
    #ifdef DEBUG_PRINT
        printf("\n");
    #endif

    Gpio_SetIO(2, 4, FALSE);
    Gpio_SetIO(2, 5, TRUE);
    delay1ms(1000);

    #ifdef DEBUG_PRINT
        printf("*************************************\n");
        printf("ADT phase count 2x\n");
    #endif
    Adt_ClearHwCntUp(AdTIM5);
    Adt_ClearHwCntDwn(AdTIM5);
    Adt_ConfigHwCntUp(AdTIM5, AdtHwCntTimxBHighTimxARise);  //硬件递加计数条件：CHxB端口为高电平时，CHxA端口上采样到上升沿
    Adt_ConfigHwCntUp(AdTIM5, AdtHwCntTimxBLowTimxAFall);   //硬件递加计数条件：CHxB端口为低电平时，CHxA端口上采样到下降沿
    Adt_ConfigHwCntDwn(AdTIM5, AdtHwCntTimxAHighTimxBRise); //硬件递减计数条件：CHxA端口为高电平时，CHxB端口上采样到上升沿
    Adt_ConfigHwCntDwn(AdTIM5, AdtHwCntTimxALowTimxBFall);  //硬件递减计数条件：CHxA端口为低电平时，CHxB端口上采样到下降沿
    Adt_SetCount(AdTIM5, 3);
    Adt_StartCount(AdTIM5);
    genClkIn();
    Adt_StopCount(AdTIM5);
    Adt_ClearCount(AdTIM5);
    #ifdef DEBUG_PRINT
        printf("\n");
    #endif

    Gpio_SetIO(2, 4, FALSE);
    Gpio_SetIO(2, 5, TRUE);
    delay1ms(1000);

    #ifdef DEBUG_PRINT
        printf("*************************************\n");
        printf("ADT phase count 4x\n");
    #endif
    Adt_ClearHwCntUp(AdTIM5);
    Adt_ClearHwCntDwn(AdTIM5);
    Adt_ConfigHwCntUp(AdTIM5, AdtHwCntTimxBHighTimxARise);  //硬件递加计数条件：CHxB端口为高电平时，CHxA端口上采样到上升沿
    Adt_ConfigHwCntUp(AdTIM5, AdtHwCntTimxBLowTimxAFall);   //硬件递加计数条件：CHxB端口为低电平时，CHxA端口上采样到下降沿
    Adt_ConfigHwCntUp(AdTIM5, AdtHwCntTimxAHighTimxBFall);  //硬件递加计数条件：CHxA端口为高电平时，CHxB端口上采样到下降沿
    Adt_ConfigHwCntUp(AdTIM5, AdtHwCntTimxALowTimxBRise);   //硬件递加计数条件：CHxA端口为低电平时，CHxB端口上采样到上升沿
    Adt_ConfigHwCntDwn(AdTIM5, AdtHwCntTimxAHighTimxBRise); //硬件递减计数条件：CHxA端口为高电平时，CHxB端口上采样到上升沿
    Adt_ConfigHwCntDwn(AdTIM5, AdtHwCntTimxALowTimxBFall);  //硬件递减计数条件：CHxA端口为低电平时，CHxB端口上采样到下降沿
    Adt_ConfigHwCntDwn(AdTIM5, AdtHwCntTimxBHighTimxAFall); //硬件递减计数条件：CHxB端口为高电平时，CHxA端口上采样到下降沿
    Adt_ConfigHwCntDwn(AdTIM5, AdtHwCntTimxBLowTimxARise);  //硬件递减计数条件：CHxB端口为低电平时，CHxA端口上采样到上升沿
    Adt_SetCount(AdTIM5, 3);
    Adt_StartCount(AdTIM5);
    genClkIn();
    Adt_StopCount(AdTIM5);
    Adt_ClearCount(AdTIM5);

    while(1);
}

/******************************************************************************
 * EOF (not truncated)
 ******************************************************************************/


