/**
 * Copyright (c), 2012~2016 iot.10086.cn All Rights Reserved
 * @file 		RFAPIKey.h
 * @brief       APIKey操作接口封装。
 * @author 		宋伟<songwei1@iot.chinamobile.com>
 * @date 		2016/05/25
 * @version 	1.0.0
 * @par Revision History:
 * 		Date			Author		Revised.Ver		Notes\n
 * 		2015/5/25		宋伟		  1.0.0			file created\n
 */
#ifndef __RFAPIKEY_H__
#define __RFAPIKEY_H__

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

/** APIKey命名最大长度*/
#define APIKEY_TITILE_LEN_MAX   31
/** APIKey字符串最大长度*/
#define APIKEY_KEY_LEN_MAX      63

/**
 * @name APIKey
 * @brief API Key的操作权限，允许某项操作，则设定为对应Flag；允许多个操作，则设定为（Flag1 | Flag2）；都允许，则可以设为0xFFFF
 * @{ */ 

/** POST权限*/
#define APIKEY_PERMISSION_FLAG_POST 0x0001
/** PUT权限*/
#define APIKEY_PERMISSION_FLAG_PUT  0x0002
/** GET权限*/
#define APIKEY_PERMISSION_FLAG_GET  0x0004
/** DELETE权限*/
#define APIKEY_PERMISSION_FLAG_DELETE   0x0008
/** 所有权限*/
#define APIKEY_PERMISSION_FLAG_ALL  0xFFFF

/**  @} */



/*****************************************************************************/
/* External Structures, Enum and Typedefs                                    */
/*****************************************************************************/
/**
 * @brief APIKey信息结构体
 */
typedef struct
{
    /** APIKey名称*/
    int8 title[APIKEY_TITILE_LEN_MAX + 1];
    /** 该APIKey的权限定义*/
    uint32 permsFlag;
    /** APIKey字符串*/
    int8 key[APIKEY_KEY_LEN_MAX + 1];
    /** 从公元1970年1月1日0时0分0秒的UTC时间算起所经过的秒数。新增或更新设备时不使用*/
    uint32 createTime; 
} RFAPIKeyInfo, *pRFAPIKeyInfo;

/*****************************************************************************/
/* External Function Prototypes                                              */
/*****************************************************************************/
/**
 * @brief 创建APIKey
 *
 * @param keyInfo API Key信息，该参数在创建APIKey时只需要tile和permsFlag，key
 *         用于返回创建成功的API Key字符串
 *
 * @return 参考@ref OSAErrno.h
 */
int32 RFAPIKey_Create(RFAPIKeyInfo *keyInfo);

/**
 * @brief 更新APIKey信息
 *
 * @param keyInfo API Key信息，跟@ref RFAPIKey_Create 不同，更新接口还需要用key
 *         参数指定需要更新的目标APIKey
 *
 * @return  参考@ref OSAErrno.h
 */
int32 RFAPIKey_UpdateInfo(RFAPIKeyInfo *keyInfo);

/**
 * @brief 查看本设备的所有key
 *
 * @param keyInfoList 返回@ref RFAPIKeyInfo 类型的结构体数组指针。用户以“keyInfoList[i]”
 *         的形式进行查看，查看后需自行释放
 *
 * @return  大于等于0 - keyInfoList 中结构体成员个数
 * @return      小于0 - 参考@ref OSAErrno.h
 */
int32 RFAPIKey_View(RFAPIKeyInfo **keyInfoList);

/**
 * @brief 删除指定APIKey
 *
 * @param targetKey 指定需要删除的key字符串
 *
 * @return  参考@ref OSAErrno.h
 */
int32 RFAPIKey_Delete(int8 *targetKey);

#ifdef _cplusplus
}
#endif   

#endif

