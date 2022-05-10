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
/*****************************************************************************/
/** \file lpm.h
 **
 ** Lpm 数据结构及API声明
 **  
 **
 ** History:
 **   - 2017-06-06   Lux     V1.0
 **
 *****************************************************************************/

#ifndef __LPM_H__
#define __LPM_H__

/*****************************************************************************
 * Include files
 *****************************************************************************/
#include "ddl.h"

#ifdef __cplusplus
extern "C"
{
#endif

/**
 *******************************************************************************
 ** \defgroup LpmGroup Low Power Management (LPM)
 **
 **
 ******************************************************************************/
//@{
    
/******************************************************************************
 ** Global pre-processor symbols/macros ('#define')                            
 ******************************************************************************/

/******************************************************************************
 * Global type definitions
 ******************************************************************************/

 /**
 ******************************************************************************
 ** \brief 事件挂起使能枚举重定义 (SEVONPEND)
 *****************************************************************************/
typedef enum en_lpm_sevonpend
{
    SevPndDisable = 0u,            ///< 事件挂起禁止
    SevPndEnable  = 1u,            ///< 事件挂起使能
}en_lpm_sevonpend_t; 
 
 /**
 ******************************************************************************
 ** \brief 深度睡眠使能枚举重定义 (SLEEPDEEP)
 *****************************************************************************/
typedef enum en_lpm_sleepdeep
{
    SlpDpDisable = 0u,            ///< 深度睡眠禁止
    SlpDpEnable  = 1u,            ///< 深度睡眠使能
}en_lpm_sleepdeep_t;

/**
 ******************************************************************************
 ** \brief 异常退出进睡眠使能枚举重定义 (SLEEPONEXIT)
 *****************************************************************************/
typedef enum en_lpm_sleeponexit
{
    SlpExtDisable = 0u,            ///< 异常退出进睡眠禁止
    SlpExtEnable  = 1u,            ///< 异常退出进睡眠使能
}en_lpm_sleeponexit_t;

/**
 ******************************************************************************
 ** \brief 唤醒模式选择枚举重定义 (WAKEUP_DPSP)
 *****************************************************************************/
typedef enum en_lpm_wakeupmode
{
    LpmWakeupUseNVIC        = 0u,       ///< 使用NVIC系统控制寄存器控制唤醒使能
    LpmWakeupUseWAKEUP_DPSP = 1u,       ///< 使用WAKEUP_DPSP寄存器控制唤醒使能
}en_lpm_wakeupmode_t;

/**
 ******************************************************************************
 ** \brief 深度休眠唤醒使能控制枚举重定义 (WAKEUP_DPSP)
 *****************************************************************************/
typedef enum en_lpm_wakeupen
{
    LpmWakeupDisable        = 0u,       ///< 唤醒功能禁止
    LpmWakeupEnable         = 1u,       ///< 唤醒功能使能
}en_lpm_wakeupen_t;

/**
 ******************************************************************************
 ** \brief Low Power Mode 配置结构体定义 
 *****************************************************************************/
typedef struct stc_lpm_config
{
    en_lpm_sevonpend_t   enSEVONPEND;        ///< 使能:每次新中断会产生一个事件,如果使用WFE休眠,则可用于唤醒处理器.
    en_lpm_sleepdeep_t   enSLEEPDEEP;        ///< 使能:执行WFI进入深度休眠;不使能:执行WFI进入休眠.        
    en_lpm_sleeponexit_t enSLEEPONEXIT;      ///< 使能:退出异常处理并返回时,处理器自动进入休眠模式;不使能:该特性禁止.
}stc_lpm_config_t;

/******************************************************************************
 * Global variable declarations ('extern', definition in C source)
 *****************************************************************************/

/******************************************************************************
 * Global function prototypes (definition in C source)
 *****************************************************************************/
//功能配置及操作函数
//低功耗模式配置
en_result_t Lpm_Config(stc_lpm_config_t* pstcConfig);
//进入低功耗模式
void Lpm_GotoLpmMode(void);

//@} // LpmGroup

#ifdef __cplusplus
#endif


#endif /* __LPM_H__ */
/******************************************************************************
 * EOF (not truncated)
 *****************************************************************************/


