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
typedef struct {
    uint8_t u8Port;
    uint8_t u8Pin;
}stc_gpio_list_t;
/******************************************************************************
 * Local function prototypes ('static')
 ******************************************************************************/

/******************************************************************************
 * Local variable definitions ('static')                                      *
 ******************************************************************************/
const stc_gpio_list_t gGpiolist[] =
{
    /*{ 0, 0 }, */{ 0, 1 }, { 0, 2 }, /*{ 0, 3 },*/
    { 1, 4 }, { 1, 5 },
    { 2, 3 }, { 2, 4 }, { 2, 5 }, { 2, 6 },
    { 2, 7 },
    { 3, 1 },
    { 3, 2 }, { 3, 3 }, { 3, 4 }, { 3, 5 }, { 3, 6 },
};


/******************************************************************************
 * Local pre-processor symbols/macros ('#define')
 ******************************************************************************/
#define SK_SW2_INIT()       Gpio_InitIO(3, 3, GpioDirIn)
#define SK_SW2_GET()        Gpio_GetIO(3,3)

#define NextStep()          { while (TRUE == Gpio_GetIO(3,3));\
                                delay1ms(2000);\
                            }

#define SK_LED_INIT()       Gpio_InitIO(0, 3, GpioDirOut)
#define SK_LED_SET(x)       Gpio_SetIO(0,3,(x))

/*****************************************************************************
 * Function implementation - global ('extern') and local ('static')
 ******************************************************************************/
/**
 * \brief   Gpio_IRQHandler 
 *          any interrput blinking a led.
 *  
 * \param   [in]  u8Param            port index
 * \retval  void             
 */


void Gpio_IRQHandler(uint8_t u8Param)
{
    *((uint32_t *)((uint32_t)&M0P_GPIO->P0ICLR + u8Param * 0x40)) = 0;
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
    //警告！！！为防止程序进入低功耗无法调试，请勿删除以下两行代码
    SK_SW2_INIT();
    NextStep();

    SK_LED_INIT();
    SK_LED_SET(1);

    //配置P33（SW2）为中断输入端口，下降沿触发
    Gpio_InitIOExt(3, 3, GpioDirIn, TRUE, FALSE, FALSE, FALSE);
    Gpio_EnableIrq(3, 3, GpioIrqFalling);
    
    EnableNvic(PORT3_IRQn, DDL_IRQ_LEVEL_DEFAULT, TRUE);

    M0P_GPIO->CTRL0_f.IESEL = 1;

    Clk_SetFunc(ClkFuncWkupRCH, TRUE);
    SCB->SCR |= 0x4;       //sleepdeep
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    __NOP();

    while (1)
    {
        SK_LED_SET(0);
        delay1ms(2000);
        SK_LED_SET(1);
        delay1ms(2000);
        __WFI();
    }
}

/******************************************************************************
 * EOF (not truncated)
 ******************************************************************************/


