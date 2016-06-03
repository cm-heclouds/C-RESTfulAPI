/******************************************************************************
 * Copyright (c), 2012~2016 iot.10086.cn All Rights Reserved
 * @file        HTTPComm.h      
 * @brief       HTTP协议数据处理 
 * @author      宋伟<songwei1@iot.chinamobile.com>
 * @date        2016/05/20
 * @version     1.0.0
 * @par Revision History:
 *      Date            Author      Revised.Ver     Notes
 *      2015/5/20       宋伟          1.0.0         file created
 ******************************************************************************/
#ifndef __HTTPCOMM_H__
#define __HTTPCOMM_H__

/*****************************************************************************/
/* Includes                                                                  */
/*****************************************************************************/
#include <osAdaption/osDataType.h>

#include <HTTPProtocol.h>
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
 * @brief 创建默认HTTP头部
 *
 * @Param :method 设定HTTP方法
 * @Param :type 设定操作的资源类型
 * @Param :APIKey 设定有操作权限的APIKey
 * @Param :firstID HTTP地址中的一级id
 * @Param :secondID HTTP地址中的二级id
 *
 * @Returns : 参见OSAErrno.h
 *************************************************************************/
int32 HTTPComm_CreateDefaultHeader(httpMethod method, cloudResType type, int8 *APIKey, int8 *firstID, int8 *secondID);

/*************************************************************************
 * @brief 增加指定HTTP头部
 *
 * @Param :header 头域名
 * @Param :value 值
 *
 * @Returns : 参见OSAErrno.h
 *************************************************************************/
int32 HTTPComm_AddHeader(int8 *header, int8* value);

int32 HTTPComm_AddUrlParam(int8* value);
/*************************************************************************
 * @brief 建立到指定主机的HTTP
 *
 * @Param :host 主机名，支持域名和点分十进制IP地址
 * @Param :port 端口
 *
 * @Returns : 参见OSAErrno.h
 *************************************************************************/
int HTTPComm_Connect(int8 *host, uint16 port);

/*************************************************************************
 * @brief 发送构造的Http Body，并获取响应。如果响应正常则返回响应中的JSON
 *        字符串（使用后需释放），否则返回空指针
 *
 * @Param :sendHttpBody 需要发送的Body缓冲区
 * @Param :bodyLen 需要发送的数据长度
 * @Param :respLen 返回接收到的响应数据长度
 *
 * @Returns :
 *************************************************************************/
int8* HTTPComm_Process(int8 *sendHttpBody, uint32 bodyLen, uint32 *respLen);

#ifdef _cplusplus
}
#endif   

#endif

