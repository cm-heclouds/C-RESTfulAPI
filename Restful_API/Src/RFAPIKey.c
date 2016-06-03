/******************************************************************************
 * Copyright (c), 2012~2016 iot.10086.cn All Rights Reserved
 * @file 		
 * @brief 
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
#include <assert.h>

#include <osAdaption/OSAErrno.h>
#include <osAdaption/osAdaption.h>
#include <HTTPProtocol.h>
#include <HTTPComm.h>
#include <devInfo.h>
#include <common.h>
#include <RFAPIKey.h>

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
static int32 APIKeyJsonBodyGen(int8 *body, int8 *deviceID, RFAPIKeyInfo *info)
{
    int8 *p = NULL;
    cJSON *pRoot = NULL;
    cJSON *pSub = NULL;
    cJSON *pSubSub = NULL;
    cJSON *pArray = NULL;

    pRoot = cJSON_CreateObject();

    if(pRoot)
    {
        cJSON_AddStringToObject(pRoot, "title", info->title);    

        pSub = cJSON_CreateObject();

        pArray = cJSON_CreateArray();
        pSubSub = cJSON_CreateObject();
        cJSON_AddStringToObject(pSubSub, "dev_id", deviceID);
        cJSON_AddItemToArray(pArray, pSubSub);
        cJSON_AddItemToObject(pSub, "resources", pArray);

        /* 如果所有操作都允许，可以不要access_methods字段*/
        if(0x000F != (info->permsFlag & 0x000F))
        {
            pArray  = cJSON_CreateArray();
            if(info->permsFlag & APIKEY_PERMISSION_FLAG_POST)
                cJSON_AddItemToArray(pArray, cJSON_CreateString("post"));
            if(info->permsFlag & APIKEY_PERMISSION_FLAG_PUT)
                cJSON_AddItemToArray(pArray, cJSON_CreateString("put"));
            if(info->permsFlag & APIKEY_PERMISSION_FLAG_GET)
                cJSON_AddItemToArray(pArray, cJSON_CreateString("get"));
            if(info->permsFlag & APIKEY_PERMISSION_FLAG_DELETE)
                cJSON_AddItemToArray(pArray, cJSON_CreateString("delete"));
            cJSON_AddItemToObject(pSub, "access_methods", pArray);
        }
        
        pSubSub = cJSON_CreateArray();
        cJSON_AddItemToArray(pSubSub, pSub);
        cJSON_AddItemToObject(pRoot, "permissions", pSubSub);

        p = cJSON_Print(pRoot);
        strcpy(body, p);
        SAFE_FREE(p);
        cJSON_Delete(pRoot);
    }
    
    return OSASUCCESS;
}

/*************************************************************************
 * @brief 解析查看APIKey操作返回的JSON字符串
 *
 * @Param :pRoot
 * @Param :infoList
 *
 * @Returns :
 *************************************************************************/
static int32 ParseJsonToRFAPIKeyInfoList(cJSON *pRoot, RFAPIKeyInfo **infoList)
{
    cJSON *pArray = NULL;
    cJSON *pSubSub = NULL;
	cJSON *pSubArray = NULL;
    cJSON *pTmp = NULL;
    int32 ret = OSAERR;
    int32 i = 0;
    int32 j, n = 0;

    pArray = cJSON_GetObjectItem(pRoot, "keys");
    if(pArray)
    {
        ret = cJSON_GetArraySize(pArray);
        if(ret > 0)
        {
            *infoList = osAdaptionMemoryAlloc(ret * sizeof(RFAPIKeyInfo));
            if(*infoList)
            {
                for(i = 0; i < ret; i++)
                {
                    pSubSub = cJSON_GetArrayItem(pArray, i);
                    pTmp = cJSON_GetObjectItem(pSubSub, "title");
                    if(pTmp)
                        strcpy((*infoList)[i].title, pTmp->valuestring);
                    pTmp = cJSON_GetObjectItem(pSubSub, "key");
                    if(pTmp)
                        strcpy((*infoList)[i].key, pTmp->valuestring);
                    pTmp = cJSON_GetObjectItem(pSubSub, "create_time");
                    if(pTmp)
                        (*infoList)[i].createTime = TimeStrToTimeStamp(pTmp->valuestring);
					pSubArray = cJSON_GetObjectItem(pSubSub, "permissions");
					if(pSubArray)
					{
						pTmp = cJSON_GetArrayItem(pSubArray, 0);
						if(pTmp)
						{
							pSubArray = cJSON_GetObjectItem(pTmp, "access_methods");
							if(pSubArray)
							{
								n = cJSON_GetArraySize(pSubArray);
								for(j = 0; j < n; j++)
								{
									pTmp = cJSON_GetArrayItem(pSubArray, j);
									if(0 == strcmp(pTmp->valuestring, "post"))
										(*infoList)[i].permsFlag |= APIKEY_PERMISSION_FLAG_POST;
									else if(0 == strcmp(pTmp->valuestring, "put"))
										(*infoList)[i].permsFlag |= APIKEY_PERMISSION_FLAG_PUT;
									else if(0 == strcmp(pTmp->valuestring, "get"))
										(*infoList)[i].permsFlag |= APIKEY_PERMISSION_FLAG_GET;
									else
										(*infoList)[i].permsFlag |= APIKEY_PERMISSION_FLAG_DELETE;
								}
							}
						}
					}
                }
            }
        }
    }
    return ret;
}

/*************************************************************************
 * @brief 创建APIKey
 *
 * @Param :keyInfo API Key信息，该参数在创建APIKey时只需要tile和permsFlag，key
 *         用于返回创建成功的API Key字符串
 *
 * @Returns : 参考RFErrno.h
 *************************************************************************/
int32 RFAPIKey_Create(RFAPIKeyInfo *keyInfo)
{
    int8 deviceID[16] = {0};
	int8 masterKey[63] = {0};
	int8 *pJsonBody = NULL;
    int8 *pRespBody = NULL;
	int32 ret = OSAERR;
     

    cJSON *pResp = NULL;
    cJSON *pSub = NULL;

    if(OSASUCCESS != DevInfo_GetDeviceID(deviceID))
        return OSAERR_DEVNOTFOUND;

    if(OSASUCCESS != DevInfo_GetMasterKey(masterKey))
        return OSAERR;

    HTTPComm_CreateDefaultHeader(HTTP_METHOD_POST, CLOUD_RES_TYPE_APIKEY, masterKey, NULL, NULL);

	pJsonBody = osAdaptionMemoryAlloc(1024);
	assert(pJsonBody);
	ret = APIKeyJsonBodyGen(pJsonBody, deviceID, keyInfo);
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
                    pSub = cJSON_GetObjectItem(pSub, "key");
                    if(pSub)
                    {
                        strcpy(keyInfo->key, pSub->valuestring);
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
 * @brief 更新APIKey信息
 *
 * @Param :keyInfo API Key信息，跟RFAPIKey_Create不同，更新接口还需要用key
 *         参数指定需要更新的目标APIKey
 *
 * @Returns : 参考RFErrno.h
 *************************************************************************/
int32 RFAPIKey_UpdateInfo(RFAPIKeyInfo *keyInfo)
{
    int8 deviceID[16] = {0};
	int8 masterKey[63] = {0};
	int8 *pJsonBody = NULL;
    int8 *pRespBody = NULL;
	int32 ret = OSAERR;
     

    cJSON *pResp = NULL;
    cJSON *pSub = NULL;

    if(OSASUCCESS != DevInfo_GetDeviceID(deviceID))
        return OSAERR_DEVNOTFOUND;

    if(OSASUCCESS != DevInfo_GetMasterKey(masterKey))
        return OSAERR;

    HTTPComm_CreateDefaultHeader(HTTP_METHOD_PUT, CLOUD_RES_TYPE_APIKEY, masterKey, keyInfo->key, NULL);

	pJsonBody = osAdaptionMemoryAlloc(1024);
	assert(pJsonBody);
	ret = APIKeyJsonBodyGen(pJsonBody, deviceID, keyInfo);
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

/*************************************************************************
 * @brief 查看本设备的所有key
 *
 * @Param :keyInfoList 返回RFAPIKeyInfo类型的结构体数组指针。用户以“keyInfoList[i]”
 *         的形式进行查看，查看后需自行释放
 *
 * @Returns : 大于等于0 - keyInfoList中结构体成员个数
 *            小于0 - 参考OSAErrno.h
 *************************************************************************/
int32 RFAPIKey_View(RFAPIKeyInfo **keyInfoList)
{
    int8 deviceID[16] = {0};
	int8 masterKey[63] = {0};
    int8 *pRespBody = NULL;
    int32 ret = OSAERR;
    int8 urlParam[63] = {0};

    cJSON *pResp = NULL;
    cJSON *pSub = NULL;

    if(OSASUCCESS != DevInfo_GetDeviceID(deviceID))
        return OSAERR_DEVNOTFOUND;

    if(OSASUCCESS != DevInfo_GetMasterKey(masterKey))
        return OSAERR;

    HTTPComm_CreateDefaultHeader(HTTP_METHOD_GET, CLOUD_RES_TYPE_APIKEY, masterKey, NULL, NULL);
    sprintf(urlParam, "?device_id=%s", deviceID);
    HTTPComm_AddUrlParam(urlParam);

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
                ret = ParseJsonToRFAPIKeyInfoList(pSub, keyInfoList);
            }
            cJSON_Delete(pResp);
        }
        SAFE_FREE(pRespBody);
    }
    
    return ret;

}

/*************************************************************************
 * @brief 删除指定APIKey
 *
 * @Param :targetKey 指定需要删除的key字符串
 *
 * @Returns : 参考OSAErrno.h
 *************************************************************************/
int32 RFAPIKey_Delete(int8 *targetKey)
{
	int8 masterKey[63] = {0};
    int8 *pRespBody = NULL;
	int32 ret = OSAERR;
     

    cJSON *pResp = NULL;
    cJSON *pSub = NULL;

    if(OSASUCCESS != DevInfo_GetMasterKey(masterKey))
        return OSAERR;

    HTTPComm_CreateDefaultHeader(HTTP_METHOD_DELETE, CLOUD_RES_TYPE_APIKEY, masterKey, targetKey, NULL);

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
                ret = OSASUCCESS;
            }
            cJSON_Delete(pResp);
        }
        SAFE_FREE(pRespBody);
    }
    return ret;
}

