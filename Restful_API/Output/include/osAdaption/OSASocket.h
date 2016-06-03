/**
 * Copyright (c), 2012~2016 iot.10086.cn All Rights Reserved
 * @file        OSASocket.h       
 * @brief       网络Socket接口封装 
 * @author      宋伟<songwei1@iot.chinamobile.com>
 * @date        2016/05/19
 * @version     1.0.0
 * @par Revision History:
 *      Date            Author      Revised.Ver     Notes\n
 *      2015/5/19       宋伟          1.0.0         file created\n
 */
#ifndef __OSASOCKET_H__ 
#define __OSASOCKET_H__

/*****************************************************************************/
/* Includes                                                                  */
/*****************************************************************************/
#include "osDataType.h"

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
 * @brief Socket类型定义
 */
typedef enum
{
	/** TCP连接*/
    SOCKET_TYPE_TCP = 0,
	/** UDP连接*/
    SOCKET_TYPE_UDP
} OSASocketType;

/*****************************************************************************/
/* External Function Prototypes                                              */
/*****************************************************************************/

/**
 * @brief 创建系统网络Socket 
 *
 * @param type 指定Socket类型
 *
 * @retval 成功 - 返回Socket号 
 * @retval 失败 - 返回错误码，参见@ref OSAErrno.h 
 */
int32 OSASocket_Create(OSASocketType type);


/**
 * @brief 连接指定主机端口，UDP和TCP均适用
 *
 * @param socket 本地socket号
 * @param host 需要连接的主机名，可为域名或点分十进制IP地址
 * @param port 需要连接的主机端口
 *
 * @return 参见@ref OSAErrno.h
 */
int32 OSASocket_Connect(int32 socket, int8 *host, uint16 port);

/**
 * @brief 通过socket发送数据，目标地址由@ref OSASocket_Connect 函数指定
 *
 * @param socket 本地socket号
 * @param sendBuf 需要发送的数据缓冲区
 * @param sendLen 需要发送的数据长度
 * @param timeout 发送超时时间，单位ms
 *
 * @retval 成功 - 返回成功发送的数据长度
 * @retval 失败 - 参见@ref OSAErrno.h
 */
int32 OSASocket_Send(uint32 socket, uint8 *sendBuf, uint16 sendLen, uint32 timeout);

/**
 * @brief 通过socket接收数据，来源地址由@ref OSASocket_Connect 函数指定
 *
 * @param socket 本地socket号
 * @param recvBuf 用于接收的数据缓冲区首地址
 * @param recvBufLen 数据接收缓冲区长度
 * @param timeout 接收超时时间，单位ms
 *
 * @retval 成功 - 返回成功接收的数据长度
 * @retval 失败 - 参见@ref OSAErrno.h
 */
int32 OSASocket_Recv(uint32 socket, uint8 *recvBuf, uint16 recvBufLen, uint32 timeout);

/**
 * @brief 关闭指定socket
 *
 * @param socket 需要关闭的socket号
 *
 * @return 参见@ref OSAErrno.h
 */
int32 OSASocket_Close(uint32 socket);

#ifdef _cplusplus
}
#endif   

#endif

