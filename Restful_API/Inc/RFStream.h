/**
 * Copyright (c), 2012~2016 iot.10086.cn All Rights Reserved
 * @file 	    RFStream.h	
 * @brief       数据流操作接口封装
 * @author 		宋伟<songwei1@iot.chinamobile.com>
 * @date 		2016/05/23
 * @version 	1.0.0
 * @par Revision History:
 * 		Date			Author		Revised.Ver		Notes\n
 * 		2015/5/23		宋伟		  1.0.0			file created\n
 */
#ifndef __RFSTREAM_H__ 
#define __RFSTREAM_H__

/*****************************************************************************/
/* Includes                                                                  */
/*****************************************************************************/
#include <osAdaption/osDataType.h>

#ifdef _cplusplus
extern "C"{
#endif   


/*****************************************************************************/
/* External Definition（Constant and Macro )                                 */
/*****************************************************************************/
/** 数据流名称长度定义*/
#define STREAM_ID_LEN_MAX 15
/** 数据流唯一标识符长度定义*/
#define STREAM_UUID_LEN_MAX 63
/** 数据流标签表长度定义*/
#define STREAM_TAGLIST_LEN_MAX 63
/** 数据流数据单位长度定义*/
#define STREAM_UNIT_LEN_MAX 15
/** 数据流数据单位符号长度定义*/
#define STREAM_UNITSYMBOL_LEN_MAX 15


/**
 * @name 数据流信息更新项选择标识
 * @{ */
/** 更新数据流名称*/
#define STREAMINFO_MASK_ID  0x0001
/** 更新数据流标签表*/
#define STREAMINFO_MASK_TAG 0x0002
/** 更新数据流数据单位*/
#define STREAMINFO_MASK_UINT    0x0004
/** 更新数据流数据单位符号*/
#define STREAMINFO_MASK_UNITSYMBOL  0x0008
/** 更新以上所有项*/
#define STREAMINFO_MASK_ALL 0xFFFF
/**  @} */

/*****************************************************************************/
/* External Structures, Enum and Typedefs                                    */
/*****************************************************************************/
/**
 * @brief 数据流配置信息结构体定义
 */
typedef struct
{
    /** 数据流名称*/
    int8 streamID[STREAM_ID_LEN_MAX + 1];
    /** 数据流标签表，可填写多个，以逗号隔开*/
    int8 streamTagList[STREAM_TAGLIST_LEN_MAX + 1];
    /** 数据流数据单位*/
    int8 unit[STREAM_UNIT_LEN_MAX + 1];
    /** 数据流数据单位符号*/
    int8 unitSymbol[STREAM_UNITSYMBOL_LEN_MAX + 1];
} RFStreamConfig, *pRFStreamConfig;

/**
 * @brief 数据流属性结构体定义
 */
typedef struct
{
    /** 数据流配置信息*/
    RFStreamConfig config;
    /** 创建时间*/
    uint32 cTime;
    /** 更新时间*/
    uint32 uTime;
    /** 数据流UUID*/
    int8 streamUUID[STREAM_UUID_LEN_MAX];
} RFStreamInfo, *pRFStreamInfo;
/*****************************************************************************/
/* External Function Prototypes                                              */
/*****************************************************************************/
/**
 * @brief 创建数据流
 *
 * @param streamConfig 新数据流的配置信息
 * @param APIKey 有权限创建数据流的APIKey
 * @param streamUUID 返回创建成功的数据流的UUID
 *
 * @return  参考@ref OSAErrno.h
 */
int32 RFStream_Create(RFStreamConfig *streamConfig, int8 *APIKey, int8 *streamUUID);

/**
 * @brief 更新数据流信息
 *
 * @param streamID 指定需要更新的数据流ID，非创建数据流时返回的UUID
 * @param APIKey 有权限修改数据流信息的APIKey
 * @param streamConfig 新的数据流信息
 * @param updateFlag 指定需要更新的数据流信息选项，信息内容由streamConfig
 *                    对应成员值确定
 *
 * @return  参考@ref OSAErrno.h
 */
int32 RFStream_UpdateInfo(int8 *streamID, int8 *APIKey, RFStreamConfig *streamConfig, uint32 updateFlag);

/**
 * @brief 查询数据流信息，只限本设备的数据流
 *
 * @param streamIDList 指定需要查询的目标数据流ID，多个数据流以逗号隔开。为空指针
 *                      或长度为0则表示查询本设备的所有数据流
 * @param APIKey 具有查询权限的APIKey
 * @param streamList 返回RFStreamInfo类型的结构体数组指针，用户以“streamList[i]”
 *                    的形式查看各条数据流信息，使用后空间需释放
 *
 * @return  大于等于0 - streamList中结构体成员个数
 *            小于0 - 参考@ref OSAErrno.h
 */
int32 RFStream_View(int8 *streamIDList, int8 *APIKey, RFStreamInfo **streamList);

/**
 * @brief 删除指定数据流
 *
 * @param APIKey 有权限删除数据流的APIKey
 * @param streamID 指定需要删除的数据流ID，非创建数据流时返回的UUID
 *
 * @return  参考@ref OSAErrno.h
 */
int32 RFStream_Delete(int8 *APIKey, int8 *streamID);


#ifdef _cplusplus
}
#endif   

#endif

