/**
* Copyright (c), 2012~2015 iot.10086.cn All Rights Reserved
* @file         osAdaption.h
* @brief        this is the OS adaption layer. 
* @details      we define general OS API to hide difference of a variety of IOT
*               OS.
* @author       DengXing 
* @date			2015/7/1
* @version		1.0.0
* @par Revision History:
*      Date			Author		Revised.Ver		Notes\n
*      2015/6/30	DengXing	1.0.0			file created\n
*/
#ifndef __OS_ADAPTION__
#define __OS_ADAPTION__

/*****************************************************************************/
/* Includes                                                                  */
/*****************************************************************************/
#include "osDataType.h"

#ifdef _cplusplus
extern "C"{
#endif
/*
 *Definition
*/

/*
 *External Structures, Enum and Typedefs
*/

/*****************************************************************************/
/* Memory Management Section                                                 */
/*****************************************************************************/
/*
 *Definition
*/
#define SAFE_FREE(p) do { if(p) { osAdaptionMemoryFree(p); (p)=NULL; } }while(0)
/*
 *External Structures, Enum and Typedefs
*/

/*
 *External API
*/
/**
 * @brief 内存分配函数
 *
 * @param size 指定需要分配的内存大小
 *
 * @return 成功 - 返回分配的内存区域头指针
 * @return 失败 - 空指针
 */
void* osAdaptionMemoryAlloc(uint32 size);

/**
 * @brief 释放内存
 *
 * @param ptr 需要释放的内存区域头指针
 */
void osAdaptionMemoryFree(void* ptr);

#ifdef _cplusplus
}
#endif
#endif /*__OS_ADAPTION__*/
