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
#include "uart.h"
#include "bt.h"
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
//uint8_t u8TxData[10] = {0x99, 0x88, 0x77, 0x66, 0x55, 0x44, 0x33, 0x22, 0x11, 0x00}; //"HUADA MCU!";
uint8_t u8TxData[] = {"HELLO LILYGO!\n"}; //"HUADA MCU!";


int32_t main(void)
{
    uint16_t timer, i = 0;
    uint32_t pclk=0;
    stc_clk_config_t stcCfg;
    stc_uart_config_t  stcConfig;
    stc_uart_irq_cb_t stcUartIrqCb;
    stc_uart_multimode_t stcMulti;
    stc_uart_baud_config_t stcBaud;
    stc_bt_config_t stcBtConfig;

	
    en_uart_mmdorck_t enTb8;

    DDL_ZERO_STRUCT(stcConfig);
    DDL_ZERO_STRUCT(stcUartIrqCb);
    DDL_ZERO_STRUCT(stcMulti);
    DDL_ZERO_STRUCT(stcBaud);
    DDL_ZERO_STRUCT(stcBtConfig);
    //时钟部分配置
//    stcCfg.enClkSrc = ClkXTH;
//    stcCfg.enHClkDiv = ClkDiv1;
//    stcCfg.enPClkDiv = ClkDiv1;
//    Clk_Init(&stcCfg);

    Gpio_InitIOExt(3, 5, GpioDirOut, TRUE, FALSE, FALSE, FALSE);
    Gpio_InitIOExt(3, 6, GpioDirOut, TRUE, FALSE, FALSE, FALSE);

    //通道端口配置
    Gpio_SetFunc_UART1TX_P35();
    Gpio_SetFunc_UART1RX_P36();

    //外设时钟使能
    Clk_SetPeripheralGate(ClkPeripheralBt, TRUE);
    Clk_SetPeripheralGate(ClkPeripheralUart1, TRUE);


    stcUartIrqCb.pfnRxIrqCb = NULL;
    stcUartIrqCb.pfnTxIrqCb = NULL;
    stcUartIrqCb.pfnRxErrIrqCb = NULL;
    stcConfig.pstcIrqCb = &stcUartIrqCb;
    stcConfig.bTouchNvic = FALSE;
    #if 1//多主机模式测试
    stcConfig.enRunMode = UartMode1;
    stcMulti.enMulti_mode = UartNormal;
//    enTb8 = Addr;
//    Uart_SetMMDOrCk(UARTCH1, enTb8);
    stcConfig.pstcMultiMode = &stcMulti;

    stcBaud.bDbaud = 0u;
    stcBaud.u32Baud = 9600;
    stcBaud.u8Mode = UartMode1; //计算波特率需要模式参数
    pclk = Clk_GetPClkFreq();
    timer = Uart_SetBaudRate(UARTCH1, pclk, &stcBaud);

    stcBtConfig.enMD = BtMode2;
    stcBtConfig.enCT = BtTimer;
    Bt_Init(TIM1, &stcBtConfig);//调用basetimer1设置函数产生波特率
    Bt_ARRSet(TIM1, timer);
    Bt_Cnt16Set(TIM1, timer);
    Bt_Run(TIM1);
    #endif
    Uart_Init(UARTCH1, &stcConfig);
    Uart_ClrStatus(UARTCH1, UartTxEmpty); //清所有中断请求
    Uart_EnableFunc(UARTCH1, UartRx);
    #if 1
//    Uart_SendData(UARTCH1, 0xC0);
//    enTb8 = Data;
//    Uart_SetMMDOrCk(UARTCH1, enTb8);
//    Uart_ClrStatus(UARTCH1, UartTxEmpty);

//    while(i < 10)
//    {
//        Uart_SendData(UARTCH1, u8TxData[i]);
//        i++;
//    }

    #endif

    while(1)
    {
			delay1ms(500);
//				Uart_SetTb8(UARTCH1,Even,0xC0);
//        Uart_SendData(UARTCH1, 0xC0);
//        enTb8 = Data;
//        Uart_SetMMDOrCk(UARTCH1, enTb8);
//        Uart_ClrStatus(UARTCH1, UartTxEmpty);
        while(i < sizeof(u8TxData)/sizeof(uint8_t))
        {
            Uart_SendData(UARTCH1, u8TxData[i]);
            i++;
        }
				i = 0;


    }
}

/******************************************************************************
 * EOF (not truncated)
 ******************************************************************************/


