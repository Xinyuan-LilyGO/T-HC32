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
 **   - 2017-06-28 LiuHL    First Version
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
//#define DEBUG_PRINT

#define OUTPUT_PORT       0
#define OUTPUT_PIN        3

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
    stc_adt_disable_3_cfg_t stcAdtDisable3;
    
    DDL_ZERO_STRUCT(stcAdtBaseCntCfg);
    DDL_ZERO_STRUCT(stcAdtTIM4ACfg);
    DDL_ZERO_STRUCT(stcAdtTIM4BCfg);
    DDL_ZERO_STRUCT(stcAdtDisable3);
    
    Clk_SwitchTo(ClkXTH);
    
    #ifdef DEBUG_PRINT
    Clk_SetPeripheralGate(ClkPeripheralUart0, TRUE);//串口0外设时钟使能
    Clk_SetPeripheralGate(ClkPeripheralBt, TRUE);   //基础时钟外设时钟使能
    #endif
    
    Clk_SetPeripheralGate(ClkPeripheralGpio, TRUE); //端口外设时钟使能
    
    #ifdef DEBUG_PRINT
        Debug_UartInit();
    #endif
        
    Gpio_InitIO(3, 4, GpioDirIn);    // port break
    
    Gpio_SetIO(OUTPUT_PORT, OUTPUT_PIN, FALSE);
    Gpio_InitIO(OUTPUT_PORT, OUTPUT_PIN, GpioDirOut);    // P03 -> P34

    Gpio_SetFunc_TIM4_CHA_P23();
    Gpio_SetFunc_TIM4_CHB_P24();

    if (Ok != Clk_SetPeripheralGate(ClkPeripheralAdt, TRUE))//ADT外设时钟使能
    {
        return Error;
    }
    enAdt = AdTIM4;

    #ifdef DEBUG_PRINT
        printf("\nADT Port Break test start!\n");
    #endif
    
    //config Port high break
    stcAdtDisable3.stcBrkPtCfg[AdtTrigxSelP34].bPortEn = TRUE;      //P34配置为刹车端口
    stcAdtDisable3.stcBrkPtCfg[AdtTrigxSelP34].enPol = AdtPtBrkHigh;//刹车电平：高电平有效
    stcAdtDisable3.bFltEn = TRUE;
    stcAdtDisable3.enFltClk = AdtFltClkPclk0Div4;
    stcAdtDisable3.bSwBrk = FALSE;
    Adt_Disable3Cfg(&stcAdtDisable3);                              //失效条件3：端口刹车初始化配置

    stcAdtBaseCntCfg.enCntMode = AdtSawtoothMode;
    stcAdtBaseCntCfg.enCntDir = AdtCntUp;
    stcAdtBaseCntCfg.enCntClkDiv = AdtClkPClk0Div4;
    Adt_Init(enAdt, &stcAdtBaseCntCfg);
    
    u16Period = 0xC000;
    Adt_SetPeriod(enAdt, u16Period);
    
    enAdtCompareA = AdtCompareA;
    u16CompareA = 0x6000;
    Adt_SetCompareValue(enAdt, enAdtCompareA, u16CompareA);
    
    enAdtCompareB = AdtCompareB;
    u16CompareB = 0x6000;
    Adt_SetCompareValue(enAdt, enAdtCompareB, u16CompareB);
    
    stcAdtTIM4ACfg.enCap = AdtCHxCompareOutput;
    stcAdtTIM4ACfg.bOutEn = TRUE;
    stcAdtTIM4ACfg.enPerc = AdtCHxPeriodLow;
    stcAdtTIM4ACfg.enCmpc = AdtCHxCompareHigh;
    stcAdtTIM4ACfg.enStaStp = AdtCHxStateSelSS;
    stcAdtTIM4ACfg.enStaOut = AdtCHxPortOutLow;
    stcAdtTIM4ACfg.enDisSel = AdtCHxDisSel3;
    stcAdtTIM4ACfg.enDisVal = AdtTIMxDisValLow;
    Adt_CHxXPortConfig(enAdt, AdtCHxA, &stcAdtTIM4ACfg);  //CHA端口配置
    
    stcAdtTIM4BCfg.enCap = AdtCHxCompareOutput;
    stcAdtTIM4BCfg.bOutEn = TRUE;
    stcAdtTIM4BCfg.enPerc = AdtCHxPeriodInv;
    stcAdtTIM4BCfg.enCmpc = AdtCHxCompareInv;
    stcAdtTIM4BCfg.enStaStp = AdtCHxStateSelSS;
    stcAdtTIM4BCfg.enStaOut = AdtCHxPortOutLow;
    
    stcAdtTIM4BCfg.enDisSel = AdtCHxDisSel3;
    stcAdtTIM4BCfg.enDisVal = AdtTIMxDisValHigh;
    Adt_CHxXPortConfig(enAdt, AdtCHxB, &stcAdtTIM4BCfg); //CHB端口配置
    
    Adt_StartCount(enAdt); 
    
    delay1ms(1000);
    #ifdef DEBUG_PRINT
        printf("****************************************\n");
    #endif
    if(Adt_GetPortBrakeFlag())
    {
        #ifdef DEBUG_PRINT
            printf("Break malfunction!\n");
            printf("input is %d\n", Gpio_GetIO(3, 4));
        #endif
    }
    #ifdef DEBUG_PRINT
        printf("Port High Break\n");
    #endif
    Gpio_SetIO(OUTPUT_PORT, OUTPUT_PIN, TRUE);
    delay1ms(1000);
    if(Adt_GetPortBrakeFlag())
    {
        #ifdef DEBUG_PRINT
            printf("Break!\n");
        #endif
    }
    delay1ms(1000);
    Gpio_SetIO(OUTPUT_PORT, OUTPUT_PIN, FALSE);
    // disable port break
    stcAdtDisable3.stcBrkPtCfg[AdtTrigxSelP34].bPortEn = FALSE;
    Adt_Disable3Cfg(&stcAdtDisable3);                          //端口刹车功能解除
    delay1ms(1000);
    Gpio_SetIO(OUTPUT_PORT, OUTPUT_PIN, TRUE);
    Adt_ClearPortBrakeFlag();
    delay1ms(1000);
    if(Adt_GetPortBrakeFlag())
    {
        #ifdef DEBUG_PRINT
            printf("Break malfunction!\n");
            printf("input is %d\n", Gpio_GetIO(3, 4));
        #endif
    }
    
    //config port low break
    stcAdtDisable3.stcBrkPtCfg[AdtTrigxSelP34].bPortEn = TRUE;     //P34配置为刹车端口
    stcAdtDisable3.stcBrkPtCfg[AdtTrigxSelP34].enPol = AdtPtBrkLow;//刹车电平：低电平有效
    stcAdtDisable3.bFltEn = TRUE;
    stcAdtDisable3.enFltClk = AdtFltClkPclk0Div4;
    stcAdtDisable3.bSwBrk = FALSE;
    Adt_Disable3Cfg(&stcAdtDisable3);                              //失效条件3：端口刹车初始化配置
    
    delay1ms(1000);
    #ifdef DEBUG_PRINT
        printf("****************************************\n");
    #endif
    if(Adt_GetPortBrakeFlag())
    {
        #ifdef DEBUG_PRINT
            printf("Break malfunction!\n");
            printf("input is %d\n", Gpio_GetIO(3, 4));
        #endif
    }
    #ifdef DEBUG_PRINT
        printf("Port Low Break\n");
    #endif
    Gpio_SetIO(OUTPUT_PORT, OUTPUT_PIN, FALSE);
    delay1ms(10);
    if(Adt_GetPortBrakeFlag())
    {
        #ifdef DEBUG_PRINT
            printf("Break!\n");
        #endif
    }
    delay1ms(1000);
    Gpio_SetIO(OUTPUT_PORT, OUTPUT_PIN, TRUE);
    
    // disable port break
    stcAdtDisable3.stcBrkPtCfg[AdtTrigxSelP34].bPortEn = FALSE;
    Adt_Disable3Cfg(&stcAdtDisable3);                          //端口刹车功能解除
    delay1ms(1000);
    Gpio_SetIO(OUTPUT_PORT, OUTPUT_PIN, FALSE);
    Adt_ClearPortBrakeFlag();
    delay1ms(1000);
    if(Adt_GetPortBrakeFlag())
    {
        #ifdef DEBUG_PRINT
            printf("Break malfunction!\n");
            printf("input is %d\n", Gpio_GetIO(3, 4));
        #endif
    }
    
    
    //config software break
    #ifdef DEBUG_PRINT
        printf("****************************************\n");
        printf("Software Break\n");
    #endif
    if(Adt_GetPortBrakeFlag())
    {
        #ifdef DEBUG_PRINT
            printf("Break malfunction!\n");
        #endif
    }
    
    stcAdtDisable3.bSwBrk = TRUE;        //软件刹车
    Adt_Disable3Cfg(&stcAdtDisable3);
    
    while(1);
}

/******************************************************************************
 * EOF (not truncated)
 ******************************************************************************/


