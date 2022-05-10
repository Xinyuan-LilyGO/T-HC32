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
#include "trim.h"
#include "lpm.h"

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

/*******************************************************************************
 * Local variable definitions ('static')
 ******************************************************************************/
static volatile uint32_t u32TrimTestFlag   = 0;
static volatile uint32_t u32CntRefer       = 0;
static volatile uint32_t u32CntCalibration = 0;
/*******************************************************************************
 * Local function prototypes ('static')
 ******************************************************************************/


/*******************************************************************************
 * Function implementation - global ('extern') and local ('static')
 ******************************************************************************/

/*******************************************************************************
 * TRIM 中断服务程序
 ******************************************************************************/   
void TrimInt(void)
{
    if (TRUE == Trim_GetIntFlag(RefStop))
    { 
        u32CntRefer       = Trim_RefCntGet();
        u32CntCalibration = Trim_CalCntGet();
        Trim_ClearIntFlag(RefStop);
        u32TrimTestFlag   = 0x01;               ///< stop
    }
    if (TRUE == Trim_GetIntFlag(CalCntOf))
    {
        Trim_ClearIntFlag(CalCntOf);
        u32TrimTestFlag |= 0x02;               ///< calcnt_of
    }
    if (TRUE == Trim_GetIntFlag(XTAL32KFault))
    {
        Trim_ClearIntFlag(XTAL32KFault);
        u32TrimTestFlag |= 0x04;
    }
    if (TRUE == Trim_GetIntFlag(XTAL32MFault))
    {
        Trim_ClearIntFlag(XTAL32MFault);
        u32TrimTestFlag |= 0x08;
    }
    
    Trim_Stop();
    
}
   
/*******************************************************************************
 * TRIM 校准模式测试
 ******************************************************************************/
en_result_t TrimCalTest(void)
{
    stc_trim_config_t stcConfig;
    en_result_t       enResult = Error;
    uint32_t          u32RefConData = 3200000u; //100ms
    
    stcConfig.enMON     = MonDisable;
    stcConfig.enCALCLK  = CALIRC16M;
    stcConfig.enREFCLK  = RefXTAL32M;
    
    stcConfig.pfnTrimCb = TrimInt;
    
    if (Ok != Trim_Init(&stcConfig))
    {
        enResult = Error;
    }
    
    //配置参考初值，中断使能，开启校准
    Trim_RefConSet(u32RefConData);
    Trim_EnableIrq();
    EnableNvic(CLK_TRIM_IRQn, 3, TRUE);
    
    Trim_Run();
    
    //进入中断……
    while(1)
    {
        if (0x01 == (u32TrimTestFlag&0x01))
        {
            u32TrimTestFlag = (u32TrimTestFlag & (~0x01));
            //获取待校准时钟的计数器值，与参考时钟的初值进行比较。
            //该值可用于待校准时钟校准时作为参考
            if ((u32CntRefer == 0) && (u32CntCalibration > 0))
            {
                enResult = Ok;
            }
            break;
        }
    } 
    
    return enResult;
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
    volatile uint8_t u8TestFlag = 0;
    
    //CLK Enable    
    Clk_SetRCHFreq(ClkFreq16Mhz);    
    Clk_Enable(ClkXTH, TRUE);
    
    Clk_SetPeripheralGate(ClkPeripheralClkTrim, TRUE);
    if(Ok != TrimCalTest())
    {
       u8TestFlag |= 0x01;
    }
     
    while (1);
}

/******************************************************************************
 * EOF (not truncated)
 ******************************************************************************/


