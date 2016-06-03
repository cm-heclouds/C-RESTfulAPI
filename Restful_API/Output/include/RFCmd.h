/**
 * Copyright (c), 2012~2016 iot.10086.cn All Rights Reserved
 * @file 	    RFCmd.h	
 * @brief       命令操作接口封装
 * @author 		宋伟<songwei1@iot.chinamobile.com>
 * @date 		2016/05/25
 * @version 	1.0.0
 * @par Revision History:
 * 		Date			Author		Revised.Ver		Notes\n
 * 		2015/5/25		宋伟		  1.0.0			file created\n
 */
#ifndef __RFCMD_H__
#define __RFCMD_H__

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
/**
 * @name 命令状态定义
 * @{ */

/** 目标设备不在线*/
#define RFCMD_STATUS_OFFLINE    0
/** 命令已创建*/
#define RFCMD_STATUS_CREATED    1
/** 命令已发往目标设备*/
#define RFCMD_STATUS_SENT       2
/** 命令发往目标设备失败*/
#define RFCMD_STATUS_SENDFAIL   3
/** 目标设备正常响应*/
#define RFCMD_STATUS_RESPRECV   4
/** 目标设备响应超时*/
#define RFCMD_STATUS_RESPTIMEOUT    5
/** 目标设备响应过长*/
#define RFCMD_STATUS_RESPTOOLARGE   6
/**  @} */

/*****************************************************************************/
/* External Structures, Enum and Typedefs                                    */
/*****************************************************************************/

/*****************************************************************************/
/* External Function Prototypes                                              */
/*****************************************************************************/
/**
 * @brief 向指定设备发送命令
 *
 * @param remoteDevID 目标设备ID
 * @param data 需要跟随命令发送的数据，可以为用户自定义JSON或其他数据
 * @param dataLen data缓冲区有效数据长度
 * @param cmdUUID 返回该命令的UUID
 *
 * @return  参考@ref OSAErrno.h
 */
int32 RFCmd_Send(int8 *remoteDevID, int8 *data, uint32 dataLen, int8 *cmdUUID);

/**
 * @brief 获取指定命令的当前状态
 *
 * @param cmdUUID 指定需要查看的命令的UUID
 *
 * @return  状态码，参考@ref RFCMD_STATUS_XXX定义
 */
int32 RFCmd_GetStatus(int8 *cmdUUID);

/**
 * @brief 获取指定命令的响应内容，暂时只支持不超过4K的响应数据获取（包括HTTP头）
 *
 * @param cmdUUID 指定需要查看响应的命令UUID
 * @param respData 用于返回响应数据的缓冲区地址，传入前需初始化
 * @param respDataLen 返回收到的响应数据长度
 *
 * @return  参考@ref OSAErrno.h
 */
int32 RFCmd_GetResponse(int8 *cmdUUID, int8 *respData, uint32 *respDataLen);

#ifdef _cplusplus
}
#endif   

#endif

