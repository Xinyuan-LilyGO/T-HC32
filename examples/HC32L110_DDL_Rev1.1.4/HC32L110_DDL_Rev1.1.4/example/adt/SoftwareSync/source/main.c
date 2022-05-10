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
    stc_adt_basecnt_cfg_t stcAdtBaseCntCfg;
    stc_adt_sw_sync_t stcAdtSwSync;
    stc_adt_sw_sync_t stcAdtSwSState;
    stc_adt_cntstate_cfg_t stcAdt4CntState;
    stc_adt_cntstate_cfg_t stcAdt5CntState;
    stc_adt_cntstate_cfg_t stcAdt6CntState;


    DDL_ZERO_STRUCT(stcAdtBaseCntCfg);
    DDL_ZERO_STRUCT(stcAdtSwSync);
    DDL_ZERO_STRUCT(stcAdtSwSState);
    DDL_ZERO_STRUCT(stcAdt4CntState);
    DDL_ZERO_STRUCT(stcAdt5CntState);
    DDL_ZERO_STRUCT(stcAdt6CntState);

    Clk_SwitchTo(ClkXTH);
    
    #ifdef DEBUG_PRINT
    Clk_SetPeripheralGate(ClkPeripheralUart0, TRUE);//串口0外设时钟使能
    Clk_SetPeripheralGate(ClkPeripheralBt, TRUE);   //基础时钟外设时钟使能
    #endif
    Clk_SetPeripheralGate(ClkPeripheralGpio, TRUE); //端口外设时钟使能
    
    #ifdef DEBUG_PRINT
        Debug_UartInit();
    #endif


    if (Ok != Clk_SetPeripheralGate(ClkPeripheralAdt, TRUE))//ADT外设时钟使能
    {
        return Error;
    }

    stcAdtBaseCntCfg.enCntMode = AdtTriangleModeB;
    stcAdtBaseCntCfg.enCntDir = AdtCntUp;
    stcAdtBaseCntCfg.enCntClkDiv = AdtClkPClk0Div1024;
    Adt_Init(AdTIM4, &stcAdtBaseCntCfg);
    Adt_Init(AdTIM5, &stcAdtBaseCntCfg);
    Adt_Init(AdTIM6, &stcAdtBaseCntCfg);                    //ADT载波、计数模式、时钟配置
    
    #ifdef DEBUG_PRINT
        printf("*************************************\n");
        printf("ADT 4,5,6 SW Sync start!\n");
    #endif
    stcAdtSwSync.bAdTim4 = TRUE;
    stcAdtSwSync.bAdTim5 = TRUE;
    stcAdtSwSync.bAdTim6 = TRUE;
    // start
    Adt_SwSyncStart(&stcAdtSwSync);                         //软件同步开始
    Adt_GetSwSyncState(&stcAdtSwSState);                    //读取软件同步状态
    #ifdef DEBUG_PRINT
        printf("Sync start! stat4 = 0x%X, stat5 = 0x%X, stat6 = 0x%X!\n", 
               stcAdtSwSState.bAdTim4, 
               stcAdtSwSState.bAdTim5, 
               stcAdtSwSState.bAdTim6);
    #endif
    delay1ms(1000);
    // stop
    Adt_SwSyncStop(&stcAdtSwSync);                          //软件同步停止
    delay1ms(1);
    Adt_GetCntState(AdTIM4, &stcAdt4CntState);              //读取状态寄存器
    Adt_GetCntState(AdTIM5, &stcAdt5CntState);
    Adt_GetCntState(AdTIM6, &stcAdt6CntState);
    Adt_GetSwSyncState(&stcAdtSwSState);                    //读取软件同步状态
    #ifdef DEBUG_PRINT
        printf("Sync stop! stat4 = 0x%X, stat5 = 0x%X, stat6 = 0x%X!\n", 
               stcAdtSwSState.bAdTim4, 
               stcAdtSwSState.bAdTim5, 
               stcAdtSwSState.bAdTim6);
        printf("cnt4 = 0x%X, cnt5 = 0x%X, cnt6 = 0x%X!\n", 
               stcAdt4CntState.u16Counter, 
               stcAdt5CntState.u16Counter, 
               stcAdt6CntState.u16Counter);
    #endif
    
    // clear
    Adt_SwSyncClear(&stcAdtSwSync);                          //软件同步清零
    delay1ms(5);
    Adt_GetCntState(AdTIM4, &stcAdt4CntState);              //读取状态寄存器
    Adt_GetCntState(AdTIM5, &stcAdt5CntState);
    Adt_GetCntState(AdTIM6, &stcAdt6CntState);
    
    #ifdef DEBUG_PRINT
        printf("Sync clear! cnt4 = 0x%X, cnt5 = 0x%X, cnt6 = 0x%X!\n", 
               stcAdt4CntState.u16Counter, 
               stcAdt5CntState.u16Counter, 
               stcAdt6CntState.u16Counter);
    
        printf("*************************************\n\n\n");
    
        printf("*************************************\n");
        printf("ADT 5,6 SW Sync start! ADT 4 stop!\n");
    #endif
    stcAdtSwSync.bAdTim4 = FALSE;
    stcAdtSwSync.bAdTim5 = TRUE;
    stcAdtSwSync.bAdTim6 = TRUE;
    // start
    Adt_SwSyncStart(&stcAdtSwSync);                         //软件同步开始
    Adt_GetSwSyncState(&stcAdtSwSState);                    //读取软件同步状态
    #ifdef DEBUG_PRINT
        printf("Sync start! stat4 = 0x%X, stat5 = 0x%X, stat6 = 0x%X!\n", 
               stcAdtSwSState.bAdTim4, 
               stcAdtSwSState.bAdTim5, 
               stcAdtSwSState.bAdTim6);
    #endif
    delay1ms(1000);
    // stop
    Adt_SwSyncStop(&stcAdtSwSync);                          //软件同步停止
    delay1ms(1);
    Adt_GetCntState(AdTIM4, &stcAdt4CntState);              //读取状态寄存器
    Adt_GetCntState(AdTIM5, &stcAdt5CntState);
    Adt_GetCntState(AdTIM6, &stcAdt6CntState);
    Adt_GetSwSyncState(&stcAdtSwSState);                    //读取软件同步状态
    #ifdef DEBUG_PRINT
        printf("Sync stop! stat4 = 0x%X, stat5 = 0x%X, stat6 = 0x%X!\n", 
               stcAdtSwSState.bAdTim4, 
               stcAdtSwSState.bAdTim5, 
               stcAdtSwSState.bAdTim6);
        printf("cnt4 = 0x%X, cnt5 = 0x%X, cnt6 = 0x%X!\n", 
               stcAdt4CntState.u16Counter, 
               stcAdt5CntState.u16Counter, 
               stcAdt6CntState.u16Counter);
    #endif
    // clear
    Adt_SwSyncClear(&stcAdtSwSync);                          //软件同步清零
    delay1ms(5);
    Adt_GetCntState(AdTIM4, &stcAdt4CntState);
    Adt_GetCntState(AdTIM5, &stcAdt5CntState);
    Adt_GetCntState(AdTIM6, &stcAdt6CntState);              //读取状态寄存器
    #ifdef DEBUG_PRINT
        printf("Sync clear! cnt4 = 0x%X, cnt5 = 0x%X, cnt6 = 0x%X!\n", 
               stcAdt4CntState.u16Counter, 
               stcAdt5CntState.u16Counter, 
               stcAdt6CntState.u16Counter);
        printf("*************************************\n\n\n");
    
        printf("*************************************\n");
        printf("ADT 4,6 SW Sync start! ADT 5 stop!\n");
    #endif
    stcAdtSwSync.bAdTim4 = TRUE;
    stcAdtSwSync.bAdTim5 = FALSE;
    stcAdtSwSync.bAdTim6 = TRUE;
    //start
    Adt_SwSyncStart(&stcAdtSwSync);                         //软件同步开始
    Adt_GetSwSyncState(&stcAdtSwSState);                    //读取软件同步状态
    #ifdef DEBUG_PRINT
    printf("Sync start! stat4 = 0x%X, stat5 = 0x%X, stat6 = 0x%X!\n", 
           stcAdtSwSState.bAdTim4, 
           stcAdtSwSState.bAdTim5, 
           stcAdtSwSState.bAdTim6);
    #endif
    delay1ms(1000);
    // stop
    Adt_SwSyncStop(&stcAdtSwSync);                          //软件同步停止
    delay1ms(1);
    Adt_GetCntState(AdTIM4, &stcAdt4CntState);
    Adt_GetCntState(AdTIM5, &stcAdt5CntState);
    Adt_GetCntState(AdTIM6, &stcAdt6CntState);              //读取状态寄存器
    Adt_GetSwSyncState(&stcAdtSwSState);                    //读取软件同步状态
    #ifdef DEBUG_PRINT
        printf("Sync stop! stat4 = 0x%X, stat5 = 0x%X, stat6 = 0x%X!\n", 
               stcAdtSwSState.bAdTim4, 
               stcAdtSwSState.bAdTim5, 
               stcAdtSwSState.bAdTim6);
        printf("cnt4 = 0x%X, cnt5 = 0x%X, cnt6 = 0x%X!\n", 
               stcAdt4CntState.u16Counter, 
               stcAdt5CntState.u16Counter, 
               stcAdt6CntState.u16Counter);
    #endif
    // clear
    Adt_SwSyncClear(&stcAdtSwSync);                          //软件同步清零
    delay1ms(5);
    Adt_GetCntState(AdTIM4, &stcAdt4CntState);
    Adt_GetCntState(AdTIM5, &stcAdt5CntState);
    Adt_GetCntState(AdTIM6, &stcAdt6CntState);              //读取状态寄存器
    #ifdef DEBUG_PRINT
        printf("Sync clear! cnt4 = 0x%X, cnt5 = 0x%X, cnt6 = 0x%X!\n", 
               stcAdt4CntState.u16Counter, 
               stcAdt5CntState.u16Counter, 
               stcAdt6CntState.u16Counter);
        printf("*************************************\n\n\n");
    
        printf("*************************************\n");
        printf("ADT 4,5 SW Sync start! ADT 6 stop!\n");
    #endif
    stcAdtSwSync.bAdTim4 = TRUE;
    stcAdtSwSync.bAdTim5 = TRUE;
    stcAdtSwSync.bAdTim6 = FALSE;
    // start
    Adt_SwSyncStart(&stcAdtSwSync);                         //软件同步开始
    Adt_GetSwSyncState(&stcAdtSwSState);                    //读取软件同步状态
    #ifdef DEBUG_PRINT
        printf("Sync start! stat4 = 0x%X, stat5 = 0x%X, stat6 = 0x%X!\n", 
               stcAdtSwSState.bAdTim4, 
               stcAdtSwSState.bAdTim5, 
               stcAdtSwSState.bAdTim6);
    #endif
    delay1ms(1000);
    // stop
    Adt_SwSyncStop(&stcAdtSwSync);                          //软件同步停止
    delay1ms(1);
    Adt_GetCntState(AdTIM4, &stcAdt4CntState);
    Adt_GetCntState(AdTIM5, &stcAdt5CntState);
    Adt_GetCntState(AdTIM6, &stcAdt6CntState);              //读取状态寄存器
    Adt_GetSwSyncState(&stcAdtSwSState);                    //读取软件同步状态
    #ifdef DEBUG_PRINT
        printf("Sync stop! stat4 = 0x%X, stat5 = 0x%X, stat6 = 0x%X!\n", 
               stcAdtSwSState.bAdTim4, 
               stcAdtSwSState.bAdTim5, 
               stcAdtSwSState.bAdTim6);
        printf("cnt4 = 0x%X, cnt5 = 0x%X, cnt6 = 0x%X!\n", 
               stcAdt4CntState.u16Counter, 
               stcAdt5CntState.u16Counter, 
               stcAdt6CntState.u16Counter);
    #endif
    // clear
    Adt_SwSyncClear(&stcAdtSwSync);                          //软件同步清零
    delay1ms(5);
    Adt_GetCntState(AdTIM4, &stcAdt4CntState);
    Adt_GetCntState(AdTIM5, &stcAdt5CntState);
    Adt_GetCntState(AdTIM6, &stcAdt6CntState);              //读取状态寄存器
    #ifdef DEBUG_PRINT
        printf("Sync clear! cnt4 = 0x%X, cnt5 = 0x%X, cnt6 = 0x%X!\n", 
               stcAdt4CntState.u16Counter, 
               stcAdt5CntState.u16Counter, 
               stcAdt6CntState.u16Counter);
        printf("*************************************\n\n\n");
    #endif
    
    while(1);
}

/******************************************************************************
 * EOF (not truncated)
 ******************************************************************************/


