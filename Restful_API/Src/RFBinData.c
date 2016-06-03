/******************************************************************************
 * Copyright (c), 2012~2016 iot.10086.cn All Rights Reserved
 * @file 		RFBindata.c
 * @brief       二进制数据操作接口封装，主要提供上传和查看功能。 
 * @author 		宋伟<songwei1@iot.chinamobile.com>
 * @date 		2016/05/24
 * @version 	1.0.0
 * @par Revision History:
 * 		Date			Author		Revised.Ver		Notes
 * 		2015/5/24		宋伟		  1.0.0			file created
 ******************************************************************************/

/*****************************************************************************/
/* Includes                                                                  */
/*****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <osAdaption/OSAErrno.h>
#include <HTTPComm.h>
#include <HTTPProtocol.h>
#include <RFBinData.h>
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



/*****************************************************************************/
/* Global Variables                                                          */
/*****************************************************************************/


/*****************************************************************************/
/* External Functions and Variables                                          */
/*****************************************************************************/



/*****************************************************************************/
/* Function Implementation                                                   */
/*****************************************************************************/
/*************************************************************************
 * @brief 向云端添加二进制数据
 *
 * @Param :APIKey 有添加数据权限的APIKey
 * @Param :streamID 数据流名称（不是数据流UUID）
 * @Param :binData 需要添加到云端的二进制数据缓冲区
 * @Param :dataLen 需要添加到云端的二进制数据长度
 * @Param :index 上传成功后，返回该二进制数据在云端的序列号
 *
 * @Returns : 参考RFErrno.h
 *************************************************************************/
int32 RFBinData_Create(int8 *APIKey, int8 *streamID, int8 *binData, uint32 dataLen, int8 *index)
{
    int8 deviceID[16] = {0};
    int8 *pRespBody = NULL;
    int32 ret = OSAERR;
    int8 urlParam[127] = {0};

    cJSON *pResp = NULL;
    cJSON *pSub = NULL;

    if(OSASUCCESS != DevInfo_GetDeviceID(deviceID))
        return OSAERR_DEVNOTFOUND;

    HTTPComm_CreateDefaultHeader(HTTP_METHOD_POST, CLOUD_RES_TYPE_BINDATA, APIKey, NULL, NULL);
    sprintf(urlParam, "?device_id=%s&datastream_id=%s", deviceID, streamID);
    HTTPComm_AddUrlParam(urlParam);

    HTTPComm_Connect("api.heclouds.com", 80);
    pRespBody = HTTPComm_Process(binData, dataLen, NULL);
    if(pRespBody)
    {
        pResp = cJSON_Parse(pRespBody);
        if(pResp)
        {
            pSub = cJSON_GetObjectItem(pResp, "errno");
            if(pSub && (0 == pSub->valueint))
            {
                pSub = cJSON_GetObjectItem(pResp, "data");
                if(pSub)
                {
                    pSub = cJSON_GetObjectItem(pSub, "index");
                    if(pSub)
                        strcpy(index, pSub->valuestring);
                }
            }
            cJSON_Delete(pResp);
        }
        SAFE_FREE(pRespBody);
    }
    
    return ret;
}

/*************************************************************************
 * @brief 查看云端上指定的二进制数据
 *
 * @Param :APIKey 有查看数据权限的APIKey
 * @Param :index 创建二进制数据时生成的云端序列号
 * @Param :binData 返回获取到的二进制数据。暂时只支持不超过4K的数据获取（包括HTTP头）
 * @Param :dataLen 返回获取到的二进制数据长度
 *
 * @Returns : 参考OSAErrno.h
 *************************************************************************/
int32 RFBinData_View(int8 *APIKey, int8 *index, int8 *binData, uint32 *dataLen)
{
    int8 *pRespBody = NULL;
    int32 ret = OSAERR;


    HTTPComm_CreateDefaultHeader(HTTP_METHOD_GET, CLOUD_RES_TYPE_BINDATA, APIKey, index, NULL);

    HTTPComm_Connect("api.heclouds.com", 80);
    pRespBody = HTTPComm_Process(NULL, 0, dataLen);
    if(pRespBody)
    {
       memcpy(binData, pRespBody, *dataLen); 
       ret = OSASUCCESS;
       SAFE_FREE(pRespBody);
    }

    return ret;
}

