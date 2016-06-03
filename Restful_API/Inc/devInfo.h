/******************************************************************************
 * Copyright (c), 2012~2016 iot.10086.cn All Rights Reserved
 * @file 		
 * @brief 
 * @author 		宋伟<songwei1@iot.chinamobile.com>
 * @date 		2016/05/13
 * @version 	1.0.0
 * @par Revision History:
 * 		Date			Author		Revised.Ver		Notes
 * 		2015/5/13		宋伟		  1.0.0			file created
 ******************************************************************************/
#ifndef __DEVINFO_H__
#define __DEVINFO_H__

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





/*****************************************************************************/
/* External Structures, Enum and Typedefs                                    */
/*****************************************************************************/

/*****************************************************************************/
/* External Function Prototypes                                              */
/*****************************************************************************/
int32 DevInfo_SetDeviceID(int8 *deviceID);
int32 DevInfo_GetDeviceID(int8 *deviceID);
int32 DevInfo_SetMasterKey(int8 *masterKey);
int32 DevInfo_GetMasterKey(int8 *masterKey);
/*************************************************************************
 * @brief 检查当前设备是否已接入云端。首先匹配当前保存的masterkey是否和本次
 *        操作一致，如果一致，则直接返回当前保存的deviceID；如果不一致，则
 *        说明是将该设备接入一个新项目，应重新执行新增设备操作，覆盖原deviceID，
 *        暂不考虑先删除之前的
 *
 * @Param :masterKey 
 *
 * @Returns :
 *************************************************************************/
boolValue DevInfo_CheckIfAdded(int8 *masterKey);


#ifdef _cplusplus
}
#endif   

#endif

