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
#include "ddl.h"
#include "uart.h"
#include "gpio.h"
#include "adc.h"
#include "i2c.h"
#include "bt.h"
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
void RGB_LED_Write1(void);
void RGB_LED_Write0(void);
void RGB_LED_Write_24_Byte(uint32_t rgb);
void SET_RGB(uint32_t rgb);
void RGB_LED_Reset(void);
uint32_t  rainbow(uint32_t value);
void factory_test(void);
/******************************************************************************
 * Local variable definitions ('static')                                      *
 ******************************************************************************/

/******************************************************************************
 * Local pre-processor symbols/macros ('#define')
 ******************************************************************************/
#define     T1_PORT                 (3)
#define     T1_PIN                  (3)
#define     RGB_LED_HIGH   *((volatile uint32_t *)((uint32_t)&M0P_GPIO->P0OUT)) |= (0x2);
#define     RGB_LED_LOW    *((volatile uint32_t *)((uint32_t)&M0P_GPIO->P0OUT)) &= (0xFFFFFFFD);

uint8_t I2C_SLAVEADDR;

uint32_t ws2812_led[33];

const uint8_t pin_arr[13][2] = {{2, 6}, {3, 5}, {3, 6}, {0, 2}, {2, 5}, {0, 0}, {0, 1}, {2, 4}, {2, 3}, {1, 4}, {1, 5}, {2, 7}, {3, 1}};
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
    stc_i2c_config_t stcI2cCfg;
    stc_clk_config_t stcCfg;
    stc_adc_cfg_t      stcAdcCfg;
    stc_adc_norm_cfg_t stcAdcNormCfg;

    DDL_ZERO_STRUCT(stcCfg);
    DDL_ZERO_STRUCT(stcI2cCfg);
    DDL_ZERO_STRUCT(stcAdcCfg);
    DDL_ZERO_STRUCT(stcAdcNormCfg);

    //时钟部分配置
    stcCfg.enClkSrc = ClkRCH;
    stcCfg.enHClkDiv = ClkDiv1;
    stcCfg.enPClkDiv = ClkDiv1;
    Clk_Init(&stcCfg);

    factory_test();

    Gpio_SetAnalog(2, 4, TRUE);
    Gpio_SetAnalog(2, 6, TRUE);

    Adc_Enable();
    M0P_BGR->CR_f.BGR_EN = 0x1u;//BGR必须使能
    M0P_BGR->CR_f.TS_EN = 0x0u;

    delay100us(1);

    stcAdcCfg.enAdcOpMode = AdcNormalMode;          //单次采样模式
    stcAdcCfg.enAdcClkSel = AdcClkSysTDiv1;         //PCLK
    stcAdcCfg.enAdcSampTimeSel = AdcSampTime4Clk;   //4个采样时钟
    stcAdcCfg.enAdcRefVolSel = RefVolSelInBgr2p5;   //参考电压:内部2.5V(avdd>3V,SPS<=200kHz)  SPS速率 = ADC时钟 / (采样时钟 + 16CLK)
    stcAdcCfg.bAdcInBufEn = FALSE;                  //电压跟随器如果使能，SPS采样速率 <=200K
    stcAdcCfg.u32AdcRegHighThd = 0u;                //比较阈值上门限
    stcAdcCfg.u32AdcRegLowThd = 0u;                 //比较阈值下门限
    stcAdcCfg.enAdcTrig0Sel = AdcTrigDisable;       //ADC转换自动触发设置
    stcAdcCfg.enAdcTrig1Sel = AdcTrigDisable;
    Adc_Init(&stcAdcCfg);

    //LED
    Gpio_InitIOExt(0, 1, GpioDirOut, FALSE, FALSE, FALSE, FALSE);

    //I2C
    Gpio_InitIOExt(2, 5, GpioDirOut, FALSE, FALSE, FALSE, FALSE);
    Gpio_InitIOExt(2, 6, GpioDirOut, FALSE, FALSE, FALSE, FALSE);

    uint32_t x = 0;

    while(1)
    {
        if(x == 193)
            x = 0;
        else
            x++;

        for(int i = 0; i < 33; i++)
        {
            ws2812_led[i] = rainbow(x);
        }

        RGB_LED_Reset();
        delay1ms(200);

        for(int i = 0; i < 33; i++)
        {
            SET_RGB(ws2812_led[i]);
        }
    }
}

/******************************************************************************
 * EOF (not truncated)
 ******************************************************************************/
void delay(uint16_t u16cnt)
{
    uint16_t i;

    for(i = 0; i < u16cnt; i++)
    {
        __NOP();
    }
}

void RGB_LED_Write0(void)
{
    RGB_LED_HIGH;
    __nop();
    __nop();
    RGB_LED_LOW;
    __nop();
}

/********************************************************/
//
/********************************************************/

void RGB_LED_Write1(void)
{
    RGB_LED_HIGH;
    __nop();
    __nop();
    __nop();
    __nop();
    __nop();
    __nop();
    __nop();
    __nop();
    __nop();
    RGB_LED_LOW;
}

void RGB_LED_Reset(void)
{
    RGB_LED_LOW;
    delay100us(3);
}

void RGB_LED_Write_24_Byte(uint32_t rgb)
{
    uint8_t i;
    uint32_t tmp = rgb;

    for(i = 0; i < 24; i++)
    {
        if(tmp & 0x80000)
        {
            RGB_LED_Write1();
        }
        else
        {
            RGB_LED_Write0();
        }

        tmp <<= 1;
    }
}

void SET_RGB(uint32_t rgb)
{
    uint8_t r = (rgb & 0xff0000) >> 16;
    uint8_t g = (rgb & 0xff00) >> 8;
    uint8_t b =  rgb & 0xff;

    uint32_t res = g << 8;
    res = res << 8;
    res |= r << 8 | b;

    RGB_LED_Write_24_Byte(res);
}

uint32_t my_rainbow(uint8_t brightness, uint32_t value)
{
    uint8_t red   = 0; // Red is the top 5 bits of a 16 bit colour value
    uint8_t green = 0; // Green is the middle 6 bits, but only top 5 bits used here
    uint8_t blue  = 0; // Blue is the bottom 5 bits
    uint8_t sector = 0;
    uint8_t amplit = 0;
    uint8_t tmp = value % (brightness * 6);
    uint32_t res = 0;
    sector = tmp >> 5;
    amplit = tmp & brightness;
    switch (sector)
    {
        case 0:
            red   = brightness;
            green = amplit; // Green ramps up
            blue  = 0;
            break;

        case 1:
            red   = brightness - amplit; // Red ramps down
            green = brightness;
            blue  = 0;
            break;

        case 2:
            red   = 0;
            green = brightness;
            blue  = amplit; // Blue ramps up
            break;

        case 3:
            red   = 0;
            green = brightness - amplit; // Green ramps down
            blue  = brightness;
            break;

        case 4:
            red   = amplit; // Red ramps up
            green = 0;
            blue  = brightness;
            break;

        case 5:
            red   = brightness;
            green = 0;
            blue  = brightness - amplit; // Blue ramps down
            break;
    }
    res = red << 8;
    res = res << 8;
    res |= green << 8 | blue;
    return res;
}

uint32_t  rainbow(uint32_t value)
{
    uint8_t red   = 0; // Red is the top 5 bits of a 16 bit colour value
    uint8_t green = 0; // Green is the middle 6 bits, but only top 5 bits used here
    uint8_t blue  = 0; // Blue is the bottom 5 bits
    uint8_t sector = 0;
    uint8_t amplit = 0;
    uint8_t tmp = value % 192;
    uint32_t res = 0;
    sector = tmp >> 5;
    amplit = tmp & 0x1F;

    switch (sector)
    {
        case 0:
            red   = 0x1F;
            green = amplit; // Green ramps up
            blue  = 0;
            break;

        case 1:
            red   = 0x1F - amplit; // Red ramps down
            green = 0x1F;
            blue  = 0;
            break;

        case 2:
            red   = 0;
            green = 0x1F;
            blue  = amplit; // Blue ramps up
            break;

        case 3:
            red   = 0;
            green = 0x1F - amplit; // Green ramps down
            blue  = 0x1F;
            break;

        case 4:
            red   = amplit; // Red ramps up
            green = 0;
            blue  = 0x1F;
            break;

        case 5:
            red   = 0x1F;
            green = 0;
            blue  = 0x1F - amplit; // Blue ramps down
            break;
    }

    res = red << 8;
    res = res << 8;
    res |= green << 8 | blue;
    return res;
}


void factory_test(void)
{

    for(uint8_t i = 0; i < 13; i++)//初始化
    {
        Gpio_InitIOExt(pin_arr[i][0], pin_arr[i][1], GpioDirOut, FALSE, FALSE, FALSE, FALSE);
    }

    for(uint8_t i = 0; i < 13; i++)//熄灭
    {
        Gpio_SetIO(pin_arr[i][0], pin_arr[i][1], 0);
    }

    delay1ms(500);


    for(uint8_t i = 0; i < 13; i++)//顺序点亮
    {
        Gpio_SetIO(pin_arr[i][0], pin_arr[i][1], 1);
        delay1ms(200);
    }

    for(uint8_t i = 0; i < 13; i++)//顺序熄灭
    {
        Gpio_SetIO(pin_arr[i][0], pin_arr[i][1], 0);
        delay1ms(200);
    }


    for(uint8_t j = 0; j < 5; j++)
    {
        for(uint8_t i = 0; i < 13; i++)
        {
            Gpio_SetIO(pin_arr[i][0], pin_arr[i][1], j % 2);
        }
        delay1ms(500);
    }

}

