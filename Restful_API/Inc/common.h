/******************************************************************************
 * Copyright (c), 2012~2016 iot.10086.cn All Rights Reserved
 * @file 		common.h
 * @brief       一些通用接口封装
 * @author 		宋伟<songwei1@iot.chinamobile.com>
 * @date 		2016/05/23
 * @version 	1.0.0
 * @par Revision History:
 * 		Date			Author		Revised.Ver		Notes
 * 		2015/5/23		宋伟		  1.0.0			file created
 ******************************************************************************/
#ifndef __COMMON_H__
#define __COMMON_H__

/*****************************************************************************/
/* Includes                                                                  */
/*****************************************************************************/
#include <osAdaption/osDataType.h>
#include <cJSON.h>

#ifdef _cplusplus
extern "C"{
#endif   


/*****************************************************************************/
/* External Definition（Constant and Macro )                                 */
/*****************************************************************************/





/*****************************************************************************/
/* External Structures, Enum and Typedefs                                    */
/*****************************************************************************/

/*****************************************************************************/
/* External Function Prototypes                                              */
/*****************************************************************************/

/*************************************************************************
 * @brief 将固定格式的列表("item1,item2,item3")添加到JSON数值结构
 *
 * @Param :pRoot
 * @Param :name
 * @Param :list
 *************************************************************************/
void AddListToJsonArray(cJSON *pRoot, int8 *name, int8 *list);

void JsonArrayToList(cJSON *pArray, int8 *list);

uint32 TimeStrToTimeStamp(int8 *timeStr);

#ifdef _cplusplus
}
#endif   

#endif

