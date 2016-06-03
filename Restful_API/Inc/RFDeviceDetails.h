/**
 * Copyright (c), 2012~2016 iot.10086.cn All Rights Reserved
 * @file 		RFDeviceDetails.h	
 * @brief 		设备明细结构定义
 * @author 		宋伟<songwei1@iot.chinamobile.com>
 * @date 		2016/05/19
 * @version 	1.0.0
 * @par Revision History:
 * 		Date			Author		Revised.Ver		Notes\n
 * 		2015/5/19		宋伟		  1.0.0			file created\n
 */
#ifndef __RFDEVICEDETAILS_H__ 
#define __RFDEVICEDETAILS_H__

/*****************************************************************************/
/* Includes                                                                  */
/*****************************************************************************/
#include <RFDevice.h>
#include <RFStream.h>
#include <RFAPIKey.h>
#include <RFBinData.h>

#ifdef _cplusplus
extern "C"{
#endif   


/*****************************************************************************/
/* External Definition（Constant and Macro )                                 */
/*****************************************************************************/





/*****************************************************************************/
/* External Structures, Enum and Typedefs                                    */
/*****************************************************************************/
/**
 * @brief 数据流信息表
 */
typedef struct
{
	/** 数据流信息结构体数组头指针*/
    RFStreamInfo *streamList;
	/** 指定数据流信息结构体的数量*/
    uint32 streamCnt;
} RFDataStreamList, *pRFDataStreamList;

/**
 * @brief API Key信息表
 */
typedef struct
{
	/** API Key信息结构体数组头指针*/
    RFAPIKeyInfo *keyList;
	/** 指定API Key信息结构体数量*/
    uint32 keyCnt;
} RFAPIKeyList, *pRFAPIKeyList;

/**
 * @brief 二进制数据信息
 */
typedef struct
{
	/** 云端为二进制数据分配的索引*/
    int8 index[BINDATA_INDEX_LEN_MAX + 1];
	/** 二进制数据的上传时间*/
    uint32 time;
	/** 二进制数据长度*/
    uint32 size;
} RFBinDataInfo, *pRFBinDataInfo;

/**
 * @brief 设备二进制数据信息表
 */
typedef struct
{
	/** 二进制信息结构体数组头指针*/
    RFBinDataInfo *binDataList;
	/** 指定二进制数据信息结构体数量*/
    uint32 binDataCnt;
} RFBinDataList, *pRFBinDataList;

/**
 * @brief 设备详细信息结构定义
 */
typedef struct
{
	/** 设备属性*/
	RFDeviceInfo devInfo;
	/** 设备所有数据流信息*/
    RFDataStreamList stream;
	/** 设备所有API Key信息*/
    RFAPIKeyList key;
	/** 设备所有二进制数据信息*/
    RFBinDataList binData;
} RFDeviceDetails, *pRFDeviceDetails;

/*****************************************************************************/
/* External Function Prototypes                                              */
/*****************************************************************************/


#ifdef _cplusplus
}
#endif   

#endif

