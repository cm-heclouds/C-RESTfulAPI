/**
 * Copyright (c), 2012~2016 iot.10086.cn All Rights Reserved
 * @file 		RFDataPoint.h
 * @brief       数据点操作接口封装，提供上报数据点的功能
 * @author 		宋伟<songwei1@iot.chinamobile.com>
 * @date 		2016/05/24
 * @version 	1.0.0
 * @par Revision History:
 * 		Date			Author		Revised.Ver		Notes\n
 * 		2015/5/24		宋伟		  1.0.0			file created\n
 */
#ifndef __RFDATAPOINT_H__
#define __RFDATAPOINT_H__

/*****************************************************************************/
/* Includes                                                                  */
/*****************************************************************************/
#include <osAdaption/osDataType.h>
#include <RFStream.h>

#ifdef _cplusplus
extern "C"{
#endif   


/*****************************************************************************/
/* External Definition（Constant and Macro )                                 */
/*****************************************************************************/
/* 数据点值长度定义*/
#define DATAPOINT_VALUE_LEN_MAX 15

/**
 * @name 数据点类型定义 
 * @{ */
/** 字符串型*/
#define DATAPOINT_VALUE_TYPE_STRING 0
/** 整型*/
#define DATAPOINT_VALUE_TYPE_NUMBER 1
/**  @} */



/*****************************************************************************/
/* External Structures, Enum and Typedefs                                    */
/*****************************************************************************/
/**
 * @brief 数据点属性结构体定义
 */
typedef struct
{
    /** 数据点所属的数据流名称*/
    int8 streamID[STREAM_ID_LEN_MAX + 1];
    /** 数据点值*/
    int8 value[DATAPOINT_VALUE_LEN_MAX + 1];
    /** 数据点数据类型，参见DATAPOINT_VALUE_TYPE_XXX，目前支持字符串和整型两种*/
    int32 valueType;
    /** 数据点创建时间，从公元1970年1月1日0时0分0秒的UTC时间算起所经过的秒数。
     * 设为0表示不添加时间参数，由云端确定*/
    uint32 time;
} RFDataPoint, *pRFDataPoint;

/*****************************************************************************/
/* External Function Prototypes                                              */
/*****************************************************************************/
/**
 * @brief 向云端新增数据点。使用示例如下：\n
 *      　RFDataPoint dataPoint[5];\n
 *        setValue(dataPoint); //对结构体数组各成员进行赋值\n
 *        RFDataPoint_Create(APIKey, dataPoint, 5);\n
 *
 * @param APIKey 有权限执行新增数据点操作的API Key
 * @param data 需要增加的数据。该参数为结构体数组头指针，数组成员个数由count确定
 * @param count　指定需要增加的数据点个数
 *
 * @return  参考@ref OSAErrno.h
 */
int32 RFDataPoint_Create(int8 *APIKey, RFDataPoint *data, int32 count);


#ifdef _cplusplus
}
#endif   

#endif

