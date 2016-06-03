/******************************************************************************
 * Copyright (c), 2012~2016 iot.10086.cn All Rights Reserved
 * @file 		RFCmd.h
 * @brief       命令操作接口封装 
 * @author 		宋伟<songwei1@iot.chinamobile.com>
 * @date 		2016/05/25
 * @version 	1.0.0
 * @par Revision History:
 * 		Date			Author		Revised.Ver		Notes
 * 		2015/5/25		宋伟		  1.0.0			file created
 ******************************************************************************/

/*****************************************************************************/
/* Includes                                                                  */
/*****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <osAdaption/OSAErrno.h>
#include <HTTPComm.h>
#include <devInfo.h>
#include <RFCmd.h>
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
 * @brief 向指定设备发送命令
 *
 * @Param :remoteDevID 目标设备ID
 * @Param :data 需要跟随命令发送的数据，可以为用户自定义JSON或其他数据
 * @Param :dataLen data缓冲区有效数据长度
 * @Param :cmdUUID 返回该命令的UUID
 *
 * @Returns : 参考RFErrno.h
 *************************************************************************/
int32 RFCmd_Send(int8 *remoteDevID, int8 *data, uint32 dataLen, int8 *cmdUUID)
{
    int8 *pRespBody = NULL;
    int32 ret = OSAERR;
    int8 urlParam[127] = {0};

    cJSON *pResp = NULL;
    cJSON *pSub = NULL;
	int8 masterKey[63] = {0};

    if(OSASUCCESS != DevInfo_GetMasterKey(masterKey))
        return OSAERR;

    HTTPComm_CreateDefaultHeader(HTTP_METHOD_POST, CLOUD_RES_TYPE_CMD, masterKey, NULL, NULL);
    sprintf(urlParam, "?device_id=%s", remoteDevID);
    HTTPComm_AddUrlParam(urlParam);

    HTTPComm_Connect("api.heclouds.com", 80);
    pRespBody = HTTPComm_Process(data, dataLen, NULL);
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
                    pSub = cJSON_GetObjectItem(pSub, "cmd_uuid");
                    if(pSub)
                    {
                        strcpy(cmdUUID, pSub->valuestring);
                        ret = OSASUCCESS;
                    }
                }
            }
            else
                ret = OSAERR_DEVNOTFOUND;
            cJSON_Delete(pResp);
        }
        SAFE_FREE(pRespBody);
    }
    
    return ret;

}

/*************************************************************************
 * @brief 获取指定命令的当前状态
 *
 * @Param :cmdUUID 指定需要查看的命令的UUID
 *
 * @Returns : 状态码，参考RFCMD_STATUS_XXX定义
 *************************************************************************/
int32 RFCmd_GetStatus(int8 *cmdUUID)
{
    int8 *pRespBody = NULL;
    int32 ret = OSAERR;

    cJSON *pResp = NULL;
    cJSON *pSub = NULL;
	int8 masterKey[63] = {0};

    if(OSASUCCESS != DevInfo_GetMasterKey(masterKey))
        return OSAERR;

    HTTPComm_CreateDefaultHeader(HTTP_METHOD_GET, CLOUD_RES_TYPE_CMD, masterKey, cmdUUID, NULL);

    HTTPComm_Connect("api.heclouds.com", 80);
    pRespBody = HTTPComm_Process(NULL, 0, NULL);
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
                    pSub = cJSON_GetObjectItem(pSub, "status");
                    if(pSub)
                        ret = pSub->valueint;
                }
            }
            cJSON_Delete(pResp);
        }
        SAFE_FREE(pRespBody);
    }
    
    return ret;
}

/*************************************************************************
 * @brief 获取指定命令的响应内容，暂时只支持不超过4K的响应数据获取（包括HTTP头）
 *
 * @Param :cmdUUID 指定需要查看响应的命令UUID
 * @Param :respData 用于返回响应数据的缓冲区地址，传入前需初始化
 * @Param :respDataLen 返回收到的响应数据长度
 *
 * @Returns : 参考RFErrno.h
 *************************************************************************/
int32 RFCmd_GetResponse(int8 *cmdUUID, int8 *respData, uint32 *respDataLen)
{
    int8 *pRespBody = NULL;
    int32 ret = OSAERR;
    int8 urlParam[127] = {0};
	int8 masterKey[63] = {0};

    if(OSASUCCESS != DevInfo_GetMasterKey(masterKey))
        return OSAERR;

    HTTPComm_CreateDefaultHeader(HTTP_METHOD_GET, CLOUD_RES_TYPE_CMD, masterKey, cmdUUID, NULL);
    strcpy(urlParam, "/resp");
    HTTPComm_AddUrlParam(urlParam);

    HTTPComm_Connect("api.heclouds.com", 80);
    pRespBody = HTTPComm_Process(NULL, 0, respDataLen);
    if(pRespBody)
    {
        memcpy(respData, pRespBody, *respDataLen);
        ret = OSASUCCESS;
        SAFE_FREE(pRespBody);
    }
    
    return ret;
}

