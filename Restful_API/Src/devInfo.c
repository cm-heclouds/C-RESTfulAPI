/******************************************************************************
 * Copyright (c), 2012~2016 iot.10086.cn All Rights Reserved
 * @file 		devInfo.c
 * @brief       设备信息访问接口
 * @author 		宋伟<songwei1@iot.chinamobile.com>
 * @date 		2016/05/23
 * @version 	1.0.0
 * @par Revision History:
 * 		Date			Author		Revised.Ver		Notes
 * 		2015/5/23		宋伟		  1.0.0			file created
 ******************************************************************************/

/*****************************************************************************/
/* Includes                                                                  */
/*****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <osAdaption/OSAErrno.h>
#include <devInfo.h>

/*****************************************************************************/
/* Local Definitions ( Constant and Macro )                                  */
/*****************************************************************************/


/*****************************************************************************/
/* Local Function Prototype                                                  */
/*****************************************************************************/

/*****************************************************************************/
/* Local Variables                                                           */
/*****************************************************************************/
static int8 s_devID[64];
static int8 s_masterKey[64];



/*****************************************************************************/
/* Global Variables                                                          */
/*****************************************************************************/


/*****************************************************************************/
/* External Functions and Variables                                          */
/*****************************************************************************/



/*****************************************************************************/
/* Function Implementation                                                   */
/*****************************************************************************/

int32 DevInfo_SetDeviceID(int8 *deviceID)
{
	if(deviceID && (0 != strlen(deviceID)))
	{
		strcpy(s_devID, deviceID);
		return OSASUCCESS;
	}

	return OSAERR;
}

int32 DevInfo_GetDeviceID(int8 *deviceID)
{
	if(0 == strlen(s_devID))
		return OSAERR_DEVNOTFOUND;

	if(deviceID)
	{
		strcpy(deviceID, s_devID);
		return OSASUCCESS;
	}
	return OSAERR_NOMEM;

}

int32 DevInfo_SetMasterKey(int8 *masterKey)
{
	if(masterKey && (0 != strlen(masterKey)))
	{
		strcpy(s_masterKey, masterKey);
		return OSASUCCESS;
	}

	return OSAERR;
}

int32 DevInfo_GetMasterKey(int8 *masterKey)
{
	if(masterKey && (0 != strlen(s_masterKey)))
	{
		strcpy(masterKey, s_masterKey);
		return OSASUCCESS;
	}
	return OSAERR_NOMEM;
}

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
boolValue DevInfo_CheckIfAdded(int8 *masterKey)
{
    int8 currKey[64] = {0};
    int32 ret = OSAERR;

    ret = DevInfo_GetMasterKey(currKey);
    if((OSASUCCESS != ret) || (0 != strcmp(masterKey, currKey)))
        return false;

    return true;
}

