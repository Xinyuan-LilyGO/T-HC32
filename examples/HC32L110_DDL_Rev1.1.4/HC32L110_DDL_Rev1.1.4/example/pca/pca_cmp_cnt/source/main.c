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
#include "pca.h"
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

/*******************************************************************************
 * Local variable definitions ('static')
 ******************************************************************************/
static volatile uint32_t u32PcaTestFlag = 0;
static volatile uint16_t u16CcapData[8] = {0};

/*******************************************************************************
 * Local function prototypes ('static')
 ******************************************************************************/


/*******************************************************************************
 * Function implementation - global ('extern') and local ('static')
 ******************************************************************************/
/*******************************************************************************
 * Pca中断服务程序
 ******************************************************************************/
void PcaInt(void)
{
    if (TRUE == Pca_GetCntIntFlag())
    {
        Pca_ClearCntIntFlag();
        u32PcaTestFlag |= 0x20;
    }
    if (TRUE == Pca_GetIntFlag(Module0))
    {
        Pca_ClearIntFlag(Module0);
        u32PcaTestFlag |= 0x01;
    }
    if (TRUE == Pca_GetIntFlag(Module1))
    {
        Pca_ClearIntFlag(Module1);
        u32PcaTestFlag |= 0x02;
    }
    if (TRUE == Pca_GetIntFlag(Module2))
    {
        Pca_ClearIntFlag(Module2);
        u32PcaTestFlag |= 0x04;
    }
    if (TRUE == Pca_GetIntFlag(Module3))
    {
        Pca_ClearIntFlag(Module3);
        u32PcaTestFlag |= 0x08;
    }
    if (TRUE == Pca_GetIntFlag(Module4))
    {
        Pca_ClearIntFlag(Module4);
        u32PcaTestFlag |= 0x10;
    }
        
}

/*******************************************************************************
 * PCA 比较计数功能测试
 ******************************************************************************/
en_result_t PcaCmpCntTest(void)
{
    stc_pca_config_t stcConfig;
    stc_pca_capmodconfig_t stcModConfig;
    en_result_t      enResult = Error;
    uint16_t         u16InitCntData = 0;
    uint16_t         u16CcapData = 0xC0;
    
    stcConfig.enCIDL = IdleGoon; 
    stcConfig.enWDTE = PCAWDTDisable;
    stcConfig.enCPS  = PCAPCLKDiv3; 
    
    stcConfig.pfnPcaCb = PcaInt;
    
    stcModConfig.enECOM = ECOMEnable;//允许比较功能
    stcModConfig.enCAPP = CAPPDisable;
    stcModConfig.enCAPN = CAPNDisable;
    stcModConfig.enMAT  = MATEnable;//允许匹配
    stcModConfig.enTOG  = TOGDisable;
    stcModConfig.enPWM  = PCAPWMDisable;
     
    if (Ok != Pca_Init(&stcConfig))
    {
        enResult = Error;
    }
    if (Ok != Pca_CapModConfig(Module2, &stcModConfig))
    {
        enResult = Error;
    }
    
    //PCA 中断使能
    Pca_ClearIntFlag(Module2);
    Pca_EnableIrq(Module2);    
    EnableNvic(PCA_IRQn, 3, TRUE);

    Pca_CapData16Set(Module2, u16CcapData);//比较捕获寄存器设置
    Pca_Cnt16Set(u16InitCntData);
    Pca_Run();
    
    //进入中断……
    while(1)
    {
        //比较值匹配
        if (0x04 == (u32PcaTestFlag&0x04))
        {
            u32PcaTestFlag = (u32PcaTestFlag & (~0x04));
            Pca_Stop();
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
//PCA
    Clk_SetPeripheralGate(ClkPeripheralPca, TRUE);
    Clk_SetPeripheralGate(ClkPeripheralGpio, TRUE);
        
    if(Ok != PcaCmpCntTest())
    {
        u8TestFlag |= 0x04;
    } 
    
    while (1);
}

/******************************************************************************
 * EOF (not truncated)
 ******************************************************************************/


