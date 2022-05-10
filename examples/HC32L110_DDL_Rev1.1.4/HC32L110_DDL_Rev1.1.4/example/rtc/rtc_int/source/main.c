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
 **   - 2016-02-16  1.0  XYZ First version for Device Driver Library of Module.
 **
 ******************************************************************************/

/******************************************************************************
 * Include files
 ******************************************************************************/
#include "rtc.h"
#include "lpm.h"
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

/******************************************************************************
 * Local pre-processor symbols/macros ('#define')                             
 ******************************************************************************/
#define     T1_PORT                 (0)
#define     T1_PIN                  (3)
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
uint8_t cyccnt=0;
uint8_t u8Alarmcnt=0;
uint8_t flg=0;
stc_rtc_time_t  stcReadTime;
static void RtcCycCb(void)
{
    cyccnt++;
    flg = ~flg;
    Gpio_SetIO(T1_PORT,T1_PIN,flg);
}
static void RtcAlarmCb(void)
{
  u8Alarmcnt++;
}
int32_t main(void)
{
    stc_rtc_config_t stcRtcConfig; 
    stc_rtc_irq_cb_t stcIrqCb;
    stc_rtc_time_t  stcTime;
    stc_rtc_alarmset_t stcAlarm;
    stc_rtc_cyc_sel_t   stcCycSel;
    stc_lpm_config_t stcLpmCfg;
    
    DDL_ZERO_STRUCT(stcRtcConfig);
    DDL_ZERO_STRUCT(stcIrqCb);
    DDL_ZERO_STRUCT(stcAlarm);
    DDL_ZERO_STRUCT(stcTime);
    DDL_ZERO_STRUCT(stcCycSel);
    DDL_ZERO_STRUCT(stcLpmCfg);
    
    stcLpmCfg.enSLEEPDEEP = SlpDpEnable;//SlpDpDisable;//
    stcLpmCfg.enSLEEPONEXIT = SlpExtEnable;
    
    Clk_SetPeripheralGate(ClkPeripheralGpio,TRUE);//使能GPIO时钟
    Gpio_InitIO(T1_PORT,T1_PIN,GpioDirOut);
    Gpio_SetIO(T1_PORT,T1_PIN,1);
    Gpio_InitIO(3,3,GpioDirIn);
    
    Clk_Enable(ClkXTL, TRUE);//开启外部晶振32.768   
    Clk_SetPeripheralGate(ClkPeripheralRtc,TRUE);//使能rtc时钟
    
    stcRtcConfig.enClkSel = RtcClk32768;//RtcClkHxt1024;//RtcClk32;//
    stcRtcConfig.enAmpmSel = Rtc24h;//Rtc12h;//
   
    stcCycSel.enCyc_sel = RtcPrads;
    stcCycSel.enPrds_sel = Rtc_1S;
#if 0 
    stcCycSel.enCyc_sel = RtcPradx;
    stcCycSel.u8Prdx = 1u;
#endif
    stcRtcConfig.pstcCycSel = &stcCycSel;
#if 1
    Rtc_DisableFunc(RtcCount);
    stcAlarm.u8Minute = 0x59;
    stcAlarm.u8Hour = 0x12;
    stcAlarm.u8Week = 0x08;
    Rtc_DisableFunc(RtcAlarmEn);
    Rtc_EnAlarmIrq(Rtc_AlarmInt_Enable);
    Rtc_SetAlarmTime(&stcAlarm);
    Rtc_EnableFunc(RtcAlarmEn);
#endif
    stcTime.u8Year = 0x17;
    stcTime.u8Month = 0x06;
    stcTime.u8Day = 0x07;
    stcTime.u8Hour = 0x12;
    stcTime.u8Minute = 0x58;
    stcTime.u8Second = 0x55;
    stcTime.u8DayOfWeek = Rtc_CalWeek(&stcTime.u8Day);
    stcRtcConfig.pstcTimeDate = &stcTime;
    
    stcIrqCb.pfnAlarmIrqCb = RtcAlarmCb;
    stcIrqCb.pfnTimerIrqCb = RtcCycCb;
    stcRtcConfig.pstcIrqCb = &stcIrqCb;
    stcRtcConfig.bTouchNvic = TRUE;
    
    Rtc_DisableFunc(RtcCount);
    Rtc_Init(&stcRtcConfig); 
    
    Rtc_EnableFunc(RtcCount);
#if 0
    while(1 == Gpio_GetIO(3,3));//注意：此处用户不能屏蔽，否则进入深度休眠模式导致SWD连接不上。    
    Lpm_Config(&stcLpmCfg);
    Lpm_GotoLpmMode();
#endif    
    while (1)
    {
        if(cyccnt)
        {
            Rtc_ReadDateTime(&stcReadTime);
        }
    }

}

/******************************************************************************
 * EOF (not truncated)
 ******************************************************************************/


