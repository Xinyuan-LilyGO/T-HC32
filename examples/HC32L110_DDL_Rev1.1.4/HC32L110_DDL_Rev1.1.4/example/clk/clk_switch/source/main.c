/******************************************************************************
* Copyright (C) 2016, Huada Semiconductor Co.,Ltd All rights reserved.
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
#include "clk.h"
#include "gpio.h"

/******************************************************************************
 * Local pre-processor symbols/macros ('#define')
 ******************************************************************************/
#define SK_SW2_INIT()       Gpio_InitIO(3, 3, GpioDirIn);
#define SK_SW2_GET()        Gpio_GetIO(3,3)

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

/*****************************************************************************
 * Function implementation - global ('extern') and local ('static')
 ******************************************************************************/
/**
 ******************************************************************************
 ** \brief  Main function of project
 **
 ** \return uint32_t return value, if needed
 **
 ** check Pxx to verify the clock frequency.
 **
 ******************************************************************************/
int32_t main(void)
{
    volatile uint32_t u32Val = 0;
    //SW2控制程序是否继续运行
    SK_SW2_INIT();
    while (TRUE == SK_SW2_GET());
    //设置P24为HCLK输出
    Gpio_SetFunc_HCLKOUT_P24();
    u32Val = Clk_GetHClkFreq();
    
    //系统时钟频率设置与切换
    //RCH 4MHz
    Clk_SwitchTo(ClkRCL);
    Clk_SetRCHFreq(ClkFreq4Mhz);
    Clk_SwitchTo(ClkRCH);
    u32Val = Clk_GetHClkFreq();

    //RCH 8MHz
    Clk_SwitchTo(ClkRCL);
    Clk_SetRCHFreq(ClkFreq8Mhz);
    Clk_SwitchTo(ClkRCH);
    u32Val = Clk_GetHClkFreq();

    //RCH 16MHz
    Clk_SwitchTo(ClkRCL);
    Clk_SetRCHFreq(ClkFreq16Mhz);
    Clk_SwitchTo(ClkRCH);
    u32Val = Clk_GetHClkFreq();

    //RCH 22.12MHz
    Clk_SwitchTo(ClkRCL);
    Clk_SetRCHFreq(ClkFreq22_12Mhz);
    Clk_SwitchTo(ClkRCH);
    u32Val = Clk_GetHClkFreq();

    //RCH 24MHz
    Clk_SwitchTo(ClkRCL);
    Clk_SetRCHFreq(ClkFreq24Mhz);
    Clk_SwitchTo(ClkRCH);
    u32Val = Clk_GetHClkFreq();

    //XTH
    Clk_SwitchTo(ClkXTH);
    u32Val = Clk_GetHClkFreq();

    //XTL
    Clk_SwitchTo(ClkXTL);
    u32Val = Clk_GetHClkFreq();
    
    //RCL   38.4K
    Clk_SetRCLFreq(ClkFreq38_4K);
    Clk_SwitchTo(ClkRCL);
    u32Val = Clk_GetHClkFreq();

    //RCL 32768
    Clk_SetRCLFreq(ClkFreq32768);
    u32Val = Clk_GetHClkFreq();

    ///< \todo check 2nd version 
    Clk_SwitchTo(ClkRCL);
    Clk_SetRCHFreq(ClkFreq4Mhz);
    Clk_SwitchTo(ClkRCH);
    u32Val = Clk_GetHClkFreq();

    while (1);

}

/******************************************************************************
 * EOF (not truncated)
 ******************************************************************************/



