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
 * TRIM监测模式测试
 ******************************************************************************/
en_result_t TrimMonTest(void)
{
    stc_trim_config_t stcConfig;
    en_result_t       enResult = Error;
    uint32_t          u32RefConData = 320000; //监测时间10s
    uint32_t          u32CalConData = 300000000;  //溢出时间小于10S
    volatile uint32_t u32Index = 0x10000;
    
    
    //INT ENABLE
    stcConfig.pfnTrimCb = TrimInt;
    EnableNvic(31, 3, TRUE);
    Trim_EnableIrq();
    
    //Monitor normal (X32MHz)
    stcConfig.enMON    = MonEnable;
    stcConfig.enCALCLK = CALXTAL32M;
    stcConfig.enREFCLK = RefIRC32K;
    
    if (Ok != Trim_Init(&stcConfig))
    {
        enResult = Error;
    }
    
    Trim_RefConSet(u32RefConData);
    Trim_CalConSet(u32CalConData);
    
    Trim_Run();
    
    //进入中断……
    while(u32Index--)
    {
        if (0x04 == (u32TrimTestFlag&0x04))
        {
            //32M fault
            enResult = Error;
            break;
        }
    }
    
    //Monitor Abort (X32MHz)
    stcConfig.enCALCLK = CALXTAL32M;
    stcConfig.enREFCLK = RefIRC32K;
    if (Ok != Trim_Init(&stcConfig))
    {
        enResult = Error;
    }
    
    Trim_RefConSet(u32RefConData);
    Trim_CalConSet(u32CalConData);
    Trim_EnableIrq();
    Trim_Run();
    
    //关闭外部高速时钟，模拟时钟异常
    u32Index = 0x10000;
    while(u32Index--);
    Clk_Enable(ClkXTH, FALSE);
    
    while(1)
    {
        if (0x08 == (u32TrimTestFlag&0x08))
        {
            //32M fault
            enResult = Ok;
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
    Clk_Enable(ClkRCL, TRUE);
    Clk_Enable(ClkXTH, TRUE);  
    
    Clk_SetPeripheralGate(ClkPeripheralClkTrim, TRUE);

    if(Ok != TrimMonTest())
    {
        u8TestFlag |= 0x02;
    }
     
    while (1);
}

/******************************************************************************
 * EOF (not truncated)
 ******************************************************************************/


