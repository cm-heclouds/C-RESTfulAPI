/**
 * Copyright (c), 2012~2016 iot.10086.cn All Rights Reserved
 * @file        RFDevice.h
 * @brief       设备操作接口封装
 * @author      宋伟<songwei1@iot.chinamobile.com>
 * @date        2016/05/18
 * @version     1.0.0
 * @par Revision History:
 *      Date            Author      Revised.Ver     Notes\n
 *      2015/5/18       SongWei       1.0.0         file created\n
 */
#ifndef __RFDEVICE_H__ 
#define __RFDEVICE_H__

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
/** 设备名长度定义*/
#define DEVICE_NAME_LEN_MAX 15
/** 设备描述信息长度定义*/
#define DEVICE_DESC_LEN_MAX 127
/** 设备标签长度定义*/
#define DEVICE_TAGLIST_LEN_MAX 63
/** 云端分配的设备ID长度定义*/
#define DEVICE_ID_LEN_MAX   15
/** API Key长度定义*/
#define DEVICE_API_KEY_LEN_MAX 63

/**
 * @name 设备信息更新项选择标识
 * @{ */
/** 更新设备名*/
#define DEVICEINFO_MASK_TITLE   0x0001
/** 更新设备描述信息*/
#define DEVICEINFO_MASK_DESC    0x0002
/** 更新设备是否属于私有设备*/
#define DEVICEINFO_MASK_PRIVATE 0x0004
/** 更新设备标签信息*/
#define DEVICEINFO_MASK_TAGLIST 0x0008
/** 更新设备位置信息*/
#define DEVICEINFO_MASK_LOCATION    0x0010
/** 更新以上所有信息*/
#define DEVICEINFO_MASK_ALL     0xFFFF
/**  @} */

#if 0
/* 指定模糊查询设备时使用的参数项*/
#define DEVICEVIEW_MASK_KEYWORD 0x0001
#define DEVICEVIEW_MASK_TAG     0x0002
#define DEVICEVIEW_MASK_PRIVATE 0x0004
#define DEVICEVIEW_MASK_PAGE    0x0008
#define DEVICEVIEW_MASK_PERPAGE 0x0010
#define DEVICEVIEW_MASK_DEVICEID    0x0020
#define DEVICEVIEW_MASK_ALL     0xFFFF
#endif

/*****************************************************************************/
/* External Structures, Enum and Typedefs                                    */
/*****************************************************************************/
/**
 * @brief 位置信息定义
 */
typedef struct
{
    /** 海拔高度*/
    float ele;
    /** 纬度*/
    float lat;
    /** 经度*/
    float lon;
} LocationInfo, *pLocationInfo;

/**
 * @brief 设备配置信息定义
 */
typedef struct
{
    /** 设备名，用户范围内应唯一*/
    int8 deviceTitle[DEVICE_NAME_LEN_MAX + 1];
    /** 设备描述*/
    int8 deviceDesc[DEVICE_DESC_LEN_MAX + 1];
    /** 设备标签表，可填写多个标签，以逗号隔开*/
    int8 deviceTagList[DEVICE_TAGLIST_LEN_MAX + 1];
    /** 具有操作权限的APIKey。对于新增设备必须使用MasterKey*/
    int8 APIKey[DEVICE_API_KEY_LEN_MAX + 1];
    /** 设备位置信息*/
    LocationInfo locInfo;
    /** 是否为私有设备*/
    boolValue isPrivate;
} RFDeviceConfig, *pRFDeviceConfig;

/**
 * @brief 设备属性定义
 */
typedef struct
{
    /** 设备配置信息*/
    RFDeviceConfig devConfig;
    /** 设备ID*/
    int8 deviceID[DEVICE_ID_LEN_MAX + 1];
    /** 从公元1970年1月1日0时0分0秒的UTC时间算起所经过的秒数。新增或更新设备时不使用*/
    uint32 createTime;
} RFDeviceInfo, *pRFDeviceInfo;

#include <RFDeviceDetails.h>

/*****************************************************************************/
/* External Function Prototypes                                              */
/*****************************************************************************/

/**
 * @brief 新增设备。设备创建成功后，用户应保存返回的设备ID，当下一次上线时，
 *     	  无需重新调用设备创建接口，直接使用@ref RFDevice_Init 即可。
 *     	  本接口包含@ref RFDevice_Init 的功能。
 *
 * @param devInfo 需要新增设备的设备信息
 * @param deviceID 返回设备ID。缓冲区传入前需初始化
 *
 * @return  参见@ref OSAErrno.h
 */
int32 RFDevice_Create(RFDeviceConfig *devConfig, int8 *deviceID);

/**
 * @brief 设备信息初始化，和@ref RFDevice_Create 接口只需调用其中一个。需要向云端
 * 		  添加设备时用Create接口，其它情况用Init接口
 *
 * @param deviceID 设备创建时云端返回的ID
 * @param masterKey 云端项目的MasterKey
 *
 * @return  参考OSAErrno.h
 */
int32 RFDevice_Init(int8 *deviceID, int8 *masterKey);

/**
 * @brief 更新设备信息
 *
 * @param devConfig 需要更新设备的设备信息。
 * @param updateFlag 指定需要更新的项，可同时更新多项内容。Flag指定的设备
 *                    信息项，需要在devConfig中正确填写
 *
 * @return  参见@ref OSAErrno.h
 */
int32 RFDevice_UpdateInfo(RFDeviceConfig *devConfig, uint32 updateFlag);


/**
 * @brief 查询设备，只限查询本设备
 *
 * @param APIKey 有权限查看设备的Key
 * @param details 返回设备详细信息
 *
 * @return  参见@ref OSAErrno.h
 */
int32 RFDevice_View(int8 *APIKey, RFDeviceDetails *details);

/**
 * @brief 执行@ref RFDevice_View 操作后，必须调用该接口释放空间
 *
 * @param details 传入View接口的设备详细信息指针
 */
void RFDevice_FreeDetails(RFDeviceDetails *details);

/**
 * @brief 从云端删除指定设备
 *
 * @param APIKey 有权限进行删除操作的APIKey
 *
 * @return  参见@ref OSAErrno.h
 */
int32 RFDevice_Delete(int8 *APIKey);

#ifdef _cplusplus
}
#endif   

#endif

