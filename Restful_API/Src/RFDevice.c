/******************************************************************************
 * Copyright (c), 2012~2016 iot.10086.cn All Rights Reserved
 * @file 	    RFDevice.h	
 * @brief       设备操作接口封装
 * @author 		宋伟<songwei1@iot.chinamobile.com>
 * @date 		2016/05/18
 * @version 	1.0.0
 * @par Revision History:
 * 		Date			Author		Revised.Ver		Notes
 * 		2015/5/18		SongWei		  1.0.0			file created
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
#include <RFDevice.h>
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
/* 设备信息结构体，用于保存本机详细设备信息*/
//static RFDeviceInfo s_devInfo;


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
 * @brief 构造新增或更新设备的JSON字符串
 *
 * @Param :body 用于存放JSON字符串的缓冲区
 * @Param :devConfig 需要新增或更新的设备信息
 * @Param :flag	指定JSON字符串需要哪些参数项，参见DEVICEINFO_MASK_XXXX
 *
 * @Returns : 参见OSAErrno.h
 *************************************************************************/
static int32 DeviceJsonBodyGen(int8 *body, RFDeviceConfig *devConfig, uint32 flag)
{
	uint8 *p = NULL;
	cJSON *pJsonRoot = NULL;
	cJSON *pJsonLocInfo = NULL;

	assert(body);

	pJsonRoot = cJSON_CreateObject();
	if(NULL == pJsonRoot)
		return OSAERR_NOMEM;

	pJsonLocInfo = cJSON_CreateObject();
	if(NULL == pJsonLocInfo)
	{
		cJSON_Delete(pJsonRoot);
		return OSAERR_NOMEM;
	}

	if(flag & DEVICEINFO_MASK_TITLE)
		cJSON_AddStringToObject(pJsonRoot, "title", (const char *)(devConfig->deviceTitle));

	if(flag & DEVICEINFO_MASK_DESC)
		cJSON_AddStringToObject(pJsonRoot, "desc", (const char *)(devConfig->deviceDesc));

	if(flag & DEVICEINFO_MASK_LOCATION)
	{
		cJSON_AddNumberToObject(pJsonLocInfo, "ele", devConfig->locInfo.ele);
		cJSON_AddNumberToObject(pJsonLocInfo, "lat", devConfig->locInfo.lat);
		cJSON_AddNumberToObject(pJsonLocInfo, "lon", devConfig->locInfo.lon);
		cJSON_AddItemToObject(pJsonRoot, "location", pJsonLocInfo);
	}

	if(flag & DEVICEINFO_MASK_PRIVATE)
		cJSON_AddBoolToObject(pJsonRoot, "private", devConfig->isPrivate);

    if(flag & DEVICEINFO_MASK_TAGLIST)
        AddListToJsonArray(pJsonRoot, "tags", devConfig->deviceTagList);

	//if(flag & DEVICEINFO_MASK_PROTOCOL)
		cJSON_AddStringToObject(pJsonRoot, "protocol", "HTTP");

	p = (uint8 *)(cJSON_Print(pJsonRoot));
	strcpy((char *)body, (const char *)p);

	SAFE_FREE(p);
	cJSON_Delete(pJsonRoot);

	return OSASUCCESS;
}


static void ParseJsonToRFDetails(cJSON *pRoot, RFDeviceDetails *details)
{
    uint32 i = 0;
    uint32 arraySize = 0;
    cJSON *pObj = NULL;
    cJSON *pSub = NULL;
    cJSON *pSubSub = NULL;

    /* 填充设备信息*/
    pObj = cJSON_GetObjectItem(pRoot, "create_time");
    if(pObj)
        details->devInfo.createTime = TimeStrToTimeStamp(pObj->valuestring);
    pObj = cJSON_GetObjectItem(pRoot, "id");
    if(pObj)
        strcpy(details->devInfo.deviceID, pObj->valuestring);
    pObj = cJSON_GetObjectItem(pRoot, "title");
    if(pObj)
        strcpy(details->devInfo.devConfig.deviceTitle, pObj->valuestring);
    pObj = cJSON_GetObjectItem(pRoot, "desc");
    if(pObj)
        strcpy(details->devInfo.devConfig.deviceDesc, pObj->valuestring);
    pObj = cJSON_GetObjectItem(pRoot, "private");
    if(pObj)
        details->devInfo.devConfig.isPrivate = pObj->type;
    pObj = cJSON_GetObjectItem(pRoot, "tags");
    if(pObj)
        JsonArrayToList(pObj, details->devInfo.devConfig.deviceTagList);
    pObj = cJSON_GetObjectItem(pRoot, "location");
    if(pObj)
    {
        pSub = cJSON_GetObjectItem(pObj, "ele");
        if(pSub)
            details->devInfo.devConfig.locInfo.ele = pSub->valueint;
        pSub = cJSON_GetObjectItem(pObj, "lat");
        if(pSub)
            details->devInfo.devConfig.locInfo.lat = pSub->valueint;
        pSub = cJSON_GetObjectItem(pObj, "lon");
        if(pSub)
            details->devInfo.devConfig.locInfo.lon = pSub->valueint;
    }

    /* 填充二进制数据信息*/
    pObj = cJSON_GetObjectItem(pRoot, "binary");
    if(pObj)
    {
        arraySize = cJSON_GetArraySize(pObj);
        if(arraySize > 0)
        {
            details->binData.binDataList = osAdaptionMemoryAlloc(arraySize * sizeof(RFBinDataInfo));
            if(details->binData.binDataList)
            {
                for(i = 0; i < arraySize; i++)
                {
                    pSub = cJSON_GetArrayItem(pObj, i);
                    if(pSub)
                    {
                        pSubSub = cJSON_GetObjectItem(pSub, "index");
                        if(pSubSub)
                            strcpy(details->binData.binDataList[i].index, pSubSub->valuestring);
                        pSubSub = cJSON_GetObjectItem(pSub, "at");
                        if(pSubSub)
                            details->binData.binDataList[i].time = TimeStrToTimeStamp(pSubSub->valuestring);
                        pSubSub = cJSON_GetObjectItem(pSub, "size");
                        if(pSubSub)
                            details->binData.binDataList[i].size = pSubSub->valueint;
                    }
                }
                details->binData.binDataCnt = arraySize;
            }
        }
    }

    /* 填充APIKey信息*/
    pObj = cJSON_GetObjectItem(pRoot, "keys");
    if(pObj)
    {
        arraySize = cJSON_GetArraySize(pObj);
        if(arraySize > 0)
        {
            details->key.keyList = osAdaptionMemoryAlloc(arraySize * sizeof(RFAPIKeyInfo));
            if(details->key.keyList)
            {
                for(i = 0; i < arraySize; i++)
                {
                    pSub = cJSON_GetArrayItem(pObj, i);
                    if(pSub)
                    {
                        pSubSub = cJSON_GetObjectItem(pSub, "title");
                        if(pSubSub)
                            strcpy(details->key.keyList[i].title, pSubSub->valuestring);
                        pSubSub = cJSON_GetObjectItem(pSub, "key");
                        if(pSubSub)
                            strcpy(details->key.keyList[i].key, pSubSub->valuestring);
                    }
                }
                details->key.keyCnt = arraySize;
            }
        }
    }

    /* 填充数据流信息*/
    pObj = cJSON_GetObjectItem(pRoot, "datastreams");
    if(pObj)
    {
        arraySize = cJSON_GetArraySize(pObj);
        if(arraySize > 0)
        {
            details->stream.streamList = osAdaptionMemoryAlloc(arraySize * sizeof(RFStreamInfo));
            if(details->stream.streamList)
            {
                for(i = 0; i < arraySize; i++)
                {
                    pSub = cJSON_GetArrayItem(pObj, i);
                    if(pSub)
                    {
                        pSubSub = cJSON_GetObjectItem(pSub, "create_time");
                        if(pSubSub)
                            details->stream.streamList[i].cTime = TimeStrToTimeStamp(pSubSub->valuestring);
                        pSubSub = cJSON_GetObjectItem(pSub, "id");
                        if(pSubSub)
                            strcpy(details->stream.streamList[i].config.streamID, pSubSub->valuestring);
                        pSubSub = cJSON_GetObjectItem(pSub, "unit");
                        if(pSubSub)
                            strcpy(details->stream.streamList[i].config.unit, pSubSub->valuestring);
                        pSubSub = cJSON_GetObjectItem(pSub, "unit_symbol");
                        if(pSubSub)
                            strcpy(details->stream.streamList[i].config.unitSymbol, pSubSub->valuestring);
                        pSubSub = cJSON_GetObjectItem(pSub, "uuid");
                        if(pSubSub)
                            strcpy(details->stream.streamList[i].streamUUID, pSubSub->valuestring);
                    }
                }
                details->stream.streamCnt = arraySize;
            }
        }
    }
}

int32 RFDevice_Init(int8 *deviceID, int8 *masterKey)
{
	int32 ret = OSAERR;

	ret = DevInfo_SetDeviceID(deviceID);
	if(ret)
		return ret;

	ret = DevInfo_SetMasterKey(masterKey);

	return ret;
}

/*************************************************************************
 * @brief 新增设备
 *
 * @Param :devInfo 需要新增设备的设备信息
 * @Param :deviceID 返回设备ID。缓冲区传入前需初始化
 *
 * @Returns : 参见OSAErrno.h
 *************************************************************************/
int32 RFDevice_Create(RFDeviceConfig *devConfig, int8 *deviceID)
{
	int8 *pJsonBody = NULL;
    int8 *pRespBody = NULL;
	int32 ret = OSAERR;
     

    cJSON *pResp = NULL;
    cJSON *pSub = NULL;

    /* 检查当前设备是否已添加到云端，且获取到了DeviceID*/
#if 0
	if(true == CheckDeviceIfExist(devConfig->deviceTitle, deviceID))
	{
        /*　新增设备操作变更为更新设备信息操作*/
        ret = RFDevice_UpdateInfo(deviceID, devConfig, DEVICEINFO_MASK_ALL);
		return ret;
	}
#endif
	/* 本地暂不做检查设备是否已添加到云端*/

    HTTPComm_CreateDefaultHeader(HTTP_METHOD_POST, CLOUD_RES_TYPE_DEVICE, devConfig->APIKey, NULL, NULL);

	pJsonBody = osAdaptionMemoryAlloc(1024);
	assert(pJsonBody);
	ret = DeviceJsonBodyGen(pJsonBody, devConfig, DEVICEINFO_MASK_ALL);
	if(OSASUCCESS != ret)
		return ret;

    ret = OSAERR;
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
                    pSub = cJSON_GetObjectItem(pSub, "device_id");
                    if(pSub)
                    {
                        strcpy(deviceID, pSub->valuestring);
						DevInfo_SetDeviceID(deviceID);
						DevInfo_SetMasterKey(devConfig->APIKey);
                        ret = OSASUCCESS;
                    }
                }
            }
            cJSON_Delete(pResp);
        }
        SAFE_FREE(pRespBody);
    }

	SAFE_FREE(pJsonBody);
	return ret;
}

/*************************************************************************
 * @brief 更新设备信息
 *
 * @Param :deviceID 指定需要更新的设备。
 * @Param :devConfig 需要更新设备的设备信息。
 * @Param :updateFlag 指定需要更新的项，可同时更新多项内容。Flag指定的设备
 * 					  信息项，需要在devConfig中正确填写
 *
 * @Returns : 参见OSAErrno.h
 *************************************************************************/
int32 RFDevice_UpdateInfo(RFDeviceConfig *devConfig, uint32 updateFlag)
{
	int8 *pJsonBody = NULL;
    int8 *pRespBody = NULL;
	int32 ret = OSAERR;
	int8 deviceID[16] = {0};

    cJSON *pResp  = NULL;
    cJSON *pSub = NULL;

    if(OSASUCCESS != DevInfo_GetDeviceID(deviceID))
        return OSAERR_DEVNOTFOUND;

    HTTPComm_CreateDefaultHeader(HTTP_METHOD_PUT, CLOUD_RES_TYPE_DEVICE, devConfig->APIKey, deviceID, NULL);

	pJsonBody = osAdaptionMemoryAlloc(1024);
	assert(pJsonBody);
	ret = DeviceJsonBodyGen(pJsonBody, devConfig, updateFlag);
	if(OSASUCCESS != ret)
		return ret;

    ret = OSAERR;
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
                ret = OSASUCCESS;
            }
            cJSON_Delete(pResp);
        }
        SAFE_FREE(pRespBody);
    }

	SAFE_FREE(pJsonBody);

	return ret;
}

void RFDevice_FreeDetails(RFDeviceDetails *details)
{
    if(details)
    {
        if(details->binData.binDataList)
            SAFE_FREE(details->binData.binDataList);
        if(details->key.keyList)
            SAFE_FREE(details->key.keyList);
        if(details->stream.streamList)
            SAFE_FREE(details->stream.streamList);
    }
}

int32 RFDevice_View(int8 *APIKey, RFDeviceDetails *details)
{
    int8 deviceID[16] = {0};
	int32 ret = OSAERR;
    int8 *pRespBody = NULL;

    cJSON *pResp = NULL;
    cJSON *pSub = NULL;

    if(OSASUCCESS != DevInfo_GetDeviceID(deviceID))
        return OSAERR_DEVNOTFOUND;

    HTTPComm_CreateDefaultHeader(HTTP_METHOD_GET, CLOUD_RES_TYPE_DEVICE, APIKey, deviceID, NULL);

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
                ParseJsonToRFDetails(pSub, details);
                ret = OSASUCCESS;
                
            }
            cJSON_Delete(pResp);
        }
        SAFE_FREE(pRespBody);
    }
    else
        ret = OSAERR;

	return ret;
}

int32 RFDevice_Delete(int8 *APIKey)
{
    int8 deviceID[16] = {0};
	int32 ret = OSAERR;
    int8 *pRespBody = NULL;

    cJSON *pResp = NULL;
    cJSON *pSub = NULL;

    /* 只能将自己从云端删除*/
    if(OSASUCCESS != DevInfo_GetDeviceID(deviceID))
        return OSAERR_DEVNOTFOUND;

    HTTPComm_CreateDefaultHeader(HTTP_METHOD_DELETE, CLOUD_RES_TYPE_DEVICE, APIKey, deviceID, NULL);

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
                ret = OSASUCCESS;
            }
            cJSON_Delete(pResp);
        }
        SAFE_FREE(pRespBody);
    }
    else
        ret = OSAERR;

	return ret;
}

