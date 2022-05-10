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
#include "lpuart.h"
#include "bt.h"
#include "clk.h"
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
#define     T1_PORT                 (3)
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
uint8_t u8TxData[10] = {0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55};//"HUADA MCU!";
uint8_t u8RxData[10];
uint32_t data;
uint8_t u8TxCnt=0,u8RxCnt=0;
uint8_t ledon = 0;

void TxIntCallback(void)
{
    if(u8TxCnt<5)
    {
        M0P_LPUART->SBUF = u8RxData[u8TxCnt];

        u8TxCnt++;
    }
    else //if(u8TxCnt>10)
    {
        u8TxCnt = 0;
        u8RxCnt = 0;
        LPUart_ClrStatus(LPUartTxEmpty);   
        LPUart_DisableIrq(LPUartTxIrq);
        LPUart_EnableIrq(LPUartRxIrq);
    }
    
}
void RxIntCallback(void)
{
    if(u8RxCnt<5)
    {
        u8RxData[u8RxCnt]=LPUart_ReceiveData();
		if(LPUart_CheckEvenOrOdd(Even,u8RxData[u8RxCnt])!=Ok)
		{
			u8RxCnt=0;
		}
		else
		{
			u8RxCnt++;
		}
    }
    else 
    {
        u8RxCnt = 0;
    }
}
void ErrIntCallback(void)
{
  
}
int32_t main(void)
{    

    stc_lpuart_config_t  stcConfig;
    stc_lpuart_irq_cb_t stcLPUartIrqCb;
    stc_lpuart_multimode_t stcMulti;
    stc_lpuart_sclk_sel_t  stcLpuart_clk;
    stc_lpuart_mode_t       stcRunMode;

    stc_bt_config_t stcBtConfig;
    stc_lpm_config_t stcLpmCfg;
    
    DDL_ZERO_STRUCT(stcConfig);
    DDL_ZERO_STRUCT(stcLPUartIrqCb);
    DDL_ZERO_STRUCT(stcMulti);
    DDL_ZERO_STRUCT(stcBtConfig);
    DDL_ZERO_STRUCT(stcLpmCfg);
    

    stcLpmCfg.enSLEEPDEEP = SlpDpEnable;//SlpDpDisable;//
    stcLpmCfg.enSLEEPONEXIT = SlpExtDisable;//唤醒后不自动进入睡眠
   
    Clk_Enable(ClkRCL, TRUE);
    Clk_SetRCLFreq(ClkFreq38_4K);//内部38.4K 
    
    Clk_SetPeripheralGate(ClkPeripheralLpUart,TRUE);//使能LPUART时钟
    Clk_SetPeripheralGate(ClkPeripheralBt,TRUE);
    
    //通道端口配置
    Gpio_InitIOExt(2,5,GpioDirOut,TRUE,FALSE,FALSE,FALSE);
    Gpio_InitIOExt(2,6,GpioDirOut,TRUE,FALSE,FALSE,FALSE);

    Gpio_SetFunc_UART2RX_P25();
    Gpio_SetFunc_UART2TX_P26();

    Gpio_InitIO(T1_PORT,T1_PIN,GpioDirIn);
    Gpio_InitIO(0,3,GpioDirOut);
    

    stcLpuart_clk.enSclk_sel = LPUart_Rcl;//LPUart_Pclk;//
    
    stcLpuart_clk.enSclk_Prs = LPUartDiv1;
    stcConfig.pstcLpuart_clk = &stcLpuart_clk;

    stcRunMode.enLpMode = LPUartLPMode;//正常工作模式或低功耗工作模式配置
    stcRunMode.enMode   = LPUartMode3;
    stcConfig.pstcRunMode = &stcRunMode;

    stcLPUartIrqCb.pfnRxIrqCb = RxIntCallback;
    stcLPUartIrqCb.pfnTxIrqCb = TxIntCallback;
    stcLPUartIrqCb.pfnRxErrIrqCb = ErrIntCallback;
    stcConfig.pstcIrqCb = &stcLPUartIrqCb;
    stcConfig.bTouchNvic = TRUE;

    stcMulti.enMulti_mode = LPUartNormal;//只有模式2/3才有多主机模式

    stcConfig.pstcMultiMode = &stcMulti; 
    LPUart_EnableIrq(LPUartRxIrq);
    LPUart_Init(&stcConfig);

    LPUart_EnableFunc(LPUartRx);
    Gpio_SetIO(0,3,1);
#if 0
    while(1 == Gpio_GetIO(T1_PORT,T1_PIN));//注意：此处用户不能屏蔽，否则进入深度休眠模式导致SWD连接不上
    Lpm_Config(&stcLpmCfg);
    Lpm_GotoLpmMode();
#endif    
    while (1)
    {
      
       if((u8RxData[0] == 0x55)&&(u8RxData[1] == 0x44)&&(u8RxData[2] == 0x33)&&(u8RxData[3] == 0x22)&&(u8RxData[4] == 0x11))
       {
            u8RxCnt = 0;
            Gpio_SetIO(0,3,0);
       }

    }
}

/******************************************************************************
 * EOF (not truncated)
 ******************************************************************************/


