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
/** \file trim.h
 **
 ** TRIM 数据结构及API声明
 **  
 **
 ** History:
 **   - 2017-05-10   Lux     V1.0
 **
 *****************************************************************************/

#ifndef __TRIM_H__
#define __TRIM_H__

/*****************************************************************************
 * Include files
 *****************************************************************************/
#include "ddl.h"
#include "interrupts_hc32l110.h"

#ifdef __cplusplus
extern "C"
{
#endif

/**
 ******************************************************************************
 ** \defgroup TrimGroup Clock Trimming (TRIM)
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
 ** \brief 监测模式使能枚举重定义    (MON_EN)
 *****************************************************************************/
typedef enum en_trim_monitor
{
    MonDisable = 0u,            ///< 禁止
    MonEnable  = 1u,            ///< 使能
}en_trim_monitor_t; 
 
 /**
 ******************************************************************************
 ** \brief 待校准/监测时钟选择枚举重定义 (CALCLK_SEL)
 *****************************************************************************/
typedef enum en_trim_calclksel
{
    CALIRC16M  = 0u,            ///< 内部16M
    CALXTAL32M = 1u,            ///< 外部32M
    CALIRC32K  = 2u,            ///< 内部32K
    CALXTAL32K = 3u,            ///< 外部32K
}en_trim_calclksel_t;

/**
 ******************************************************************************
 ** \brief 参考时钟选择枚举重定义 (REFCLK_SEL)
 *****************************************************************************/
typedef enum en_trim_refclksel
{
    RefIRC16M  = 0u,            ///< 内部16M
    RefXTAL32M = 1u,            ///< 外部32M
    RefIRC32K  = 2u,            ///< 内部32K
    RefXTAL32K = 3u,            ///< 外部32K
    RefIRC10K  = 4u,            ///< 内部10K
    ExtClkIn   = 5u,            ///< 外部输入时钟
}en_trim_refclksel_t;

/**
 ******************************************************************************
 ** \brief 中断标志类型枚举重定义
 *****************************************************************************/
typedef enum en_trim_inttype
{
    RefStop      = 0u,            ///< 参考计数器停止标志
    CalCntOf     = 1u,            ///< 校准计数器停止标志
    XTAL32KFault = 2u,            ///< XTAL32K 失效标志
    XTAL32MFault = 3u,            ///< XTAL32M 失效标志
}en_trim_inttype_t;

/**
 ******************************************************************************
 ** \brief TRIM 配置结构体定义 
 *****************************************************************************/
typedef struct stc_trim_config
{
    en_trim_monitor_t    enMON;         ///< 监测模式
    en_trim_calclksel_t  enCALCLK;      ///< 校准时钟选择
    en_trim_refclksel_t  enREFCLK;      ///< 参考时钟选择
    
    func_ptr_t           pfnTrimCb;     ///< TRIM 中断服务回调函数[void function(void)]
}stc_trim_config_t;

/******************************************************************************
 * Global variable declarations ('extern', definition in C source)
 *****************************************************************************/

/******************************************************************************
 * Global function prototypes (definition in C source)
 *****************************************************************************/
//中断操作相关函数
//中断标志获取
boolean_t Trim_GetIntFlag(en_trim_inttype_t enIntType);
//中断标志清除
en_result_t Trim_ClearIntFlag(en_trim_inttype_t enIntType);
//中断使能/禁止
en_result_t Trim_EnableIrq (void);
en_result_t Trim_DisableIrq(void);

//功能配置及操作函数
//Trim 配置及初始化
en_result_t Trim_Init(stc_trim_config_t* pstcConfig);
//校准/监测启动/停止
en_result_t Trim_Run(void);
en_result_t Trim_Stop(void);
//参考计数器初值配置
en_result_t Trim_RefConSet(uint32_t u32Data);
//校准计数器初值配置
en_result_t Trim_CalConSet(uint32_t u32Data);
//参考计数器计数值获取
uint32_t Trim_RefCntGet(void);
//校准计数器计数值获取
uint32_t Trim_CalCntGet(void);

//@} // TrimGroup

#ifdef __cplusplus
#endif


#endif /* __TRIM_H__ */
/******************************************************************************
 * EOF (not truncated)
 *****************************************************************************/


