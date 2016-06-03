/******************************************************************************
 * Copyright (c), 2012~2016 iot.10086.cn All Rights Reserved
 * @file 		RFStream.c
 * @brief       数据流操作接口封装
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
#include <assert.h>

#include <osAdaption/OSAErrno.h>
#include <osAdaption/OSASocket.h>
#include <cJSON.h>
#include <HTTPProtocol.h>
#include <HTTPComm.h>
#include <RFStream.h>
#include <common.h>
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
static int32 StreamJsonBodyGen(int8 *body, RFStreamConfig *streamConfig, uint32 flag)
{
    cJSON *pRoot = NULL;
    int8* p = NULL;

    pRoot = cJSON_CreateObject();
    if(NULL == pRoot)
        return OSAERR_NOMEM;

    if(flag & STREAMINFO_MASK_ID)
        cJSON_AddStringToObject(pRoot, "id", streamConfig->streamID);
    if(flag & STREAMINFO_MASK_TAG)
        AddListToJsonArray(pRoot, "tags", streamConfig->streamTagList);
    if(flag & STREAMINFO_MASK_UINT)
        cJSON_AddStringToObject(pRoot, "unit", streamConfig->unit);
    if(flag & STREAMINFO_MASK_UNITSYMBOL)
        cJSON_AddStringToObject(pRoot, "unit_symbol", streamConfig->unitSymbol);

    p = cJSON_Print(pRoot);
    strcpy((char *)body, p);
    SAFE_FREE(p);
    cJSON_Delete(pRoot);
    
    return OSASUCCESS;
}

static int32 ParseJsonToRFStreamInfoList(cJSON *pRoot, RFStreamInfo **streamList)
{
    cJSON *pSub = NULL;
    cJSON *pSubSub = NULL;
    uint32 streamCnt = 0;
    uint32 i = 0;

    streamCnt = cJSON_GetArraySize(pRoot);

    if(streamCnt > 0)
    {
        *streamList = osAdaptionMemoryAlloc(streamCnt * sizeof(RFStreamInfo));
        if(*streamList)
        {
            for(i = 0; i < streamCnt; i++)
            {
                pSub = cJSON_GetArrayItem(pRoot, i);
                if(pSub)
                {
                    pSubSub = cJSON_GetObjectItem(pSub, "id");
                    if(pSubSub)
                        strcpy((*streamList)[i].config.streamID, pSubSub->valuestring);
                    pSubSub = cJSON_GetObjectItem(pSub, "uuid");
                    if(pSubSub)
                        strcpy((*streamList)[i].streamUUID, pSubSub->valuestring);
                    pSubSub = cJSON_GetObjectItem(pSub, "tags");
                    if(pSubSub)
                        JsonArrayToList(pSubSub, (*streamList)[i].config.streamTagList);
                    pSubSub = cJSON_GetObjectItem(pSub, "unit");
                    if(pSubSub)
                        strcpy((*streamList)[i].config.unit, pSubSub->valuestring);
                    pSubSub = cJSON_GetObjectItem(pSub, "unit_symbol");
                    if(pSubSub)
                        strcpy((*streamList)[i].config.unitSymbol, pSubSub->valuestring);
                    pSubSub = cJSON_GetObjectItem(pSub, "create_time");
                    if(pSubSub)
                        (*streamList)[i].cTime = TimeStrToTimeStamp(pSubSub->valuestring);
                    pSubSub = cJSON_GetObjectItem(pSub, "update_at");
                    if(pSubSub)
                        (*streamList)[i].uTime = TimeStrToTimeStamp(pSubSub->valuestring);
                }

            }
        }
    }
    
    return streamCnt;
}

/*************************************************************************
 * @brief 创建数据流
 *
 * @Param :streamConfig 新数据流的配置信息
 * @Param :APIKey 有权限创建数据流的APIKey
 * @Param :streamUUID 返回创建成功的数据流的UUID
 *
 * @Returns : 参考OSAErrno.h
 *************************************************************************/
int32 RFStream_Create(RFStreamConfig *streamConfig, int8 *APIKey, int8 *streamUUID)
{
    int8 deviceID[16] = {0};
    int8 *pJsonBody = NULL;
    int8 *pRespBody = NULL;
    int32 ret = OSAERR;

    cJSON *pResp = NULL;
    cJSON *pSub = NULL;

    /* */
    if(OSASUCCESS != DevInfo_GetDeviceID(deviceID))
        return OSAERR_DEVNOTFOUND;

    HTTPComm_CreateDefaultHeader(HTTP_METHOD_POST, CLOUD_RES_TYPE_DATASTREAM, APIKey, deviceID, NULL);
    pJsonBody = osAdaptionMemoryAlloc(1024);
    if(pJsonBody)
    {
        StreamJsonBodyGen(pJsonBody, streamConfig, STREAMINFO_MASK_ALL);

        HTTPComm_Connect("api.heclouds.com", 80);
        pRespBody = HTTPComm_Process(pJsonBody, strlen((const char *)pJsonBody), NULL);
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
                        pSub = cJSON_GetObjectItem(pSub, "ds_uuid");
                        if(pSub)
                        {
                            strcpy(streamUUID, pSub->valuestring);
                            ret = OSASUCCESS;
                        }
                    }
                }
                cJSON_Delete(pResp);
            }
            SAFE_FREE(pRespBody);
        }
        SAFE_FREE(pJsonBody);
    }

    return ret;
}

/*************************************************************************
 * @brief 更新数据流信息
 *
 * @Param :streamID 指定需要更新的数据流ID
 * @Param :APIKey 有权限修改数据流信息的APIKey
 * @Param :streamConfig 新的数据流信息
 * @Param :updateFlag 指定需要更新的数据流信息选项，信息内容由streamConfig
 *                    对应成员值确定
 *
 * @Returns : 参考OSAErrno.h
 *************************************************************************/
int32 RFStream_UpdateInfo(int8 *streamID, int8 *APIKey, RFStreamConfig *streamConfig, uint32 updateFlag)
{
    int8 deviceID[16] = {0};
    int8 *pJsonBody = NULL;
    int8 *pRespBody = NULL;
    int32 ret = OSAERR;

    cJSON *pResp = NULL;
    cJSON *pSub = NULL;

    if(OSASUCCESS != DevInfo_GetDeviceID(deviceID))
        return OSAERR_DEVNOTFOUND;

    HTTPComm_CreateDefaultHeader(HTTP_METHOD_PUT, CLOUD_RES_TYPE_DATASTREAM, APIKey, deviceID, streamID);
    pJsonBody = osAdaptionMemoryAlloc(1024);
    if(pJsonBody)
    {
        StreamJsonBodyGen(pJsonBody, streamConfig, updateFlag);

        HTTPComm_Connect("api.heclouds.com", 80);
        pRespBody = HTTPComm_Process(pJsonBody, strlen((const char *)pJsonBody), NULL);
        if(pRespBody)
        {
            pResp = cJSON_Parse(pRespBody);
            if(pResp)
            {
                pSub = cJSON_GetObjectItem(pResp, "errno");
                if(pSub && (0 == pSub->valueint))
                    ret = OSASUCCESS; 

				cJSON_Delete(pResp);
            }
            SAFE_FREE(pRespBody);
        }
        SAFE_FREE(pJsonBody);
    }
    
    return ret;
}

/*************************************************************************
 * @brief 查询数据流信息，只限本设备的数据流
 *
 * @Param :streamIDList 指定需要查询的目标数据流ID，多个数据流以逗号隔开。为空指针
 *                      或长度为0则表示查询本设备的所有数据流
 * @Param :APIKey 具有查询权限的APIKey
 * @Param :streamList 返回RFStreamInfo类型的结构体数组指针，用户以“streamList[i]”
 *                    的形式查看各条数据流信息，使用后空间需释放
 *
 * @Returns : 大于等于0 - streamList中结构体成员个数
 *            小于0 - 参考OSAErrno.h
 *************************************************************************/
int32 RFStream_View(int8 *streamIDList, int8 *APIKey, RFStreamInfo **streamList)
{
    int8 deviceID[16] = {0};
    int8 *pRespBody = NULL;
    int32 ret = OSAERR;
    int8 urlParam[63] = {0};

    cJSON *pResp = NULL;
    cJSON *pSub = NULL;

    if(OSASUCCESS != DevInfo_GetDeviceID(deviceID))
        return OSAERR_DEVNOTFOUND;

    HTTPComm_CreateDefaultHeader(HTTP_METHOD_GET, CLOUD_RES_TYPE_DATASTREAM, APIKey, deviceID, NULL);
    if(streamIDList && (0 != strlen(streamIDList)))
    {
        sprintf(urlParam, "?datastream_ids=%s", streamIDList);
        HTTPComm_AddUrlParam(urlParam);
    }

    ret = OSAERR;
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
                ret = ParseJsonToRFStreamInfoList(pSub, streamList);
            }
            cJSON_Delete(pResp);
        }
        SAFE_FREE(pRespBody);
    }
    
    return ret;
}

/*************************************************************************
 * @brief 删除指定数据流
 *
 * @Param :APIKey 有权限删除数据流的APIKey
 * @Param :streamID 指定需要删除的数据流ID
 *
 * @Returns : 参考OSAErrno.h
 *************************************************************************/
int32 RFStream_Delete(int8 *APIKey, int8 *streamID)
{
    int8 deviceID[16] = {0};
    int32 ret = OSAERR;
    int8 *pRespBody = NULL;

    cJSON *pResp = NULL;
    cJSON *pSub = NULL;

    if(OSASUCCESS != DevInfo_GetDeviceID(deviceID))
        return OSAERR_DEVNOTFOUND;

    HTTPComm_CreateDefaultHeader(HTTP_METHOD_DELETE, CLOUD_RES_TYPE_DATASTREAM, APIKey, deviceID, streamID);

    HTTPComm_Connect("api.heclouds.com", 80);
    pRespBody = HTTPComm_Process(NULL, 0, NULL);
    if(pRespBody)
    {
        pResp = cJSON_Parse(pRespBody);
        if(pResp)
        {
            pSub = cJSON_GetObjectItem(pResp, "errno");
            if(pSub && (0 == pSub->valueint))
                ret = OSASUCCESS; 
			cJSON_Delete(pResp);
        }
        SAFE_FREE(pRespBody);
    }

    return ret;
}

