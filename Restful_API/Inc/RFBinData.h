/**
 * Copyright (c), 2012~2016 iot.10086.cn All Rights Reserved
 * @file 		RFBinData.h
 * @brief       二进制数据操作接口封装，主要提供上传和查看功能。
 * @author 		宋伟<songwei1@iot.chinamobile.com>
 * @date 		2016/05/24
 * @version 	1.0.0
 * @par Revision History:
 * 		Date			Author		Revised.Ver		Notes\n
 * 		2015/5/24		宋伟		  1.0.0			file created\n
 */
#ifndef __RFBINDATA_H__
#define __RFBINDATA_H__

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

/** 云端分配为二进制数据分配的索引长度*/
#define BINDATA_INDEX_LEN_MAX   63




/*****************************************************************************/
/* External Structures, Enum and Typedefs                                    */
/*****************************************************************************/

/*****************************************************************************/
/* External Function Prototypes                                              */
/*****************************************************************************/
/**
 * @brief 向云端添加二进制数据
 *
 * @param APIKey 有添加数据权限的APIKey
 * @param streamID 数据流名称（不是数据流UUID）
 * @param binData 需要添加到云端的二进制数据缓冲区
 * @param dataLen 需要添加到云端的二进制数据长度
 * @param index 上传成功后，返回该二进制数据在云端的序列号
 *
 * @return  参考@ref OSAErrno.h
 */
int32 RFBinData_Create(int8 *APIKey, int8 *streamID, int8 *binData, uint32 dataLen, int8 *index);

/**
 * @brief 查看云端上指定的二进制数据
 *
 * @param APIKey 有查看数据权限的APIKey
 * @param index 创建二进制数据时生成的云端序列号
 * @param binData 返回获取到的二进制数据。暂时只支持不超过4K的数据获取（包括HTTP头）
 * @param dataLen 返回获取到的二进制数据长度
 *
 * @return  参考@ref OSAErrno.h
 */
int32 RFBinData_View(int8 *APIKey, int8 *index, int8 *binData, uint32 *dataLen);


#ifdef _cplusplus
}
#endif   

#endif

