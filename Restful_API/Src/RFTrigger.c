/******************************************************************************
 * Copyright (c), 2012~2016 iot.10086.cn All Rights Reserved
 * @file 	    RFTrigger.c	
 * @brief       触发器操作接口封装。触发器主要用于设置某种条件，数据流上传的数据点满足
 *              条件时，自动向指定url发起POST请求。
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
#include <HTTPProtocol.h>
#include <HTTPComm.h>
#include <cJSON.h>
#include <devInfo.h>
#include <common.h>
#include <RFTrigger.h>
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
static int32 TriggerTypeStringToType(int8 *typeStr)
{
    if(!strncmp(typeStr, "<=", 2))
        return TRIGGER_TYPE_LE;
    else if(*(typeStr) == '<')
        return TRIGGER_TYPE_LT;
    else if(!strncmp(typeStr, ">=", 2))
        return TRIGGER_TYPE_GE;
    else if(*(typeStr) == '>')
        return TRIGGER_TYPE_GT;
    else if(!strncmp(typeStr, "==", 2))
        return TRIGGER_TYPE_EQ;
    else if(!strncmp(typeStr, "inout", 5))
        return TRIGGER_TYPE_INOUT;
    else if(!strncmp(typeStr, "out", 3))
        return TRIGGER_TYPE_OUT;
    else if(!strncmp(typeStr, "in", 2))
        return TRIGGER_TYPE_IN;
    else if(!strncmp(typeStr, "exp", 3))
        return TRIGGER_TYPE_EXP;
    else if(!strncmp(typeStr, "change", 6))
        return TRIGGER_TYPE_CHANGE;
    else if(!strncmp(typeStr, "frozen", 6))
        return TRIGGER_TYPE_FROZEN;
    else if(!strncmp(typeStr, "live", 4))
        return TRIGGER_TYPE_FROZEN;
    
    return TRIGGER_TYPE_UNKNOWN;
}

static void AddTriggerTypeToJson(cJSON *pJson, int32 triggerType, int8 *triggerParam)
{
    double *param = NULL;
    cJSON *pSub = NULL;
   
    param = (double *)triggerParam;

    switch(triggerType)
    {
        case TRIGGER_TYPE_LT:
            cJSON_AddStringToObject(pJson, "type", "<");
            cJSON_AddNumberToObject(pJson, "threshold", *param);
            break;
        case TRIGGER_TYPE_LE:
            cJSON_AddStringToObject(pJson, "type", "<=");
            cJSON_AddNumberToObject(pJson, "threshold", *param);
            break;
        case TRIGGER_TYPE_GT:
            cJSON_AddStringToObject(pJson, "type", ">");
            cJSON_AddNumberToObject(pJson, "threshold", *param);
            break;
        case TRIGGER_TYPE_GE:
            cJSON_AddStringToObject(pJson, "type", ">=");
            cJSON_AddNumberToObject(pJson, "threshold", *param);
            break;
        case TRIGGER_TYPE_EQ:
            cJSON_AddStringToObject(pJson, "type", "==");
            cJSON_AddNumberToObject(pJson, "threshold", *param);
            break;
        case TRIGGER_TYPE_IN:
            cJSON_AddStringToObject(pJson, "type", "in");
            pSub = cJSON_CreateObject();
            if(pSub)
            {
                cJSON_AddNumberToObject(pSub, "lolmt", *param);
                cJSON_AddNumberToObject(pSub, "uplmt", *(param + 1));
                cJSON_AddItemToObject(pJson, "threshold", pSub);
            }
            break;
        case TRIGGER_TYPE_OUT:
            cJSON_AddStringToObject(pJson, "type", "out");
            pSub = cJSON_CreateObject();
            if(pSub)
            {
                cJSON_AddNumberToObject(pSub, "lolmt", *param);
                cJSON_AddNumberToObject(pSub, "uplmt", *(param + 1));
                cJSON_AddItemToObject(pJson, "threshold", pSub);
            }
            break;
        case TRIGGER_TYPE_INOUT:
            cJSON_AddStringToObject(pJson, "type", "inout");
            pSub = cJSON_CreateObject();
            if(pSub)
            {
                cJSON_AddNumberToObject(pSub, "lolmt", *param);
                cJSON_AddNumberToObject(pSub, "uplmt", *(param + 1));
                cJSON_AddItemToObject(pJson, "threshold", pSub);
            }
            break;
        case TRIGGER_TYPE_EXP:
            cJSON_AddStringToObject(pJson, "type", "exp");
            cJSON_AddStringToObject(pJson, "threshold", triggerParam);
            break;
        case TRIGGER_TYPE_CHANGE:
            cJSON_AddStringToObject(pJson, "type", "change");
            break;
        case TRIGGER_TYPE_FROZEN:
            cJSON_AddStringToObject(pJson, "type", "frozen");
            cJSON_AddNumberToObject(pJson, "threshold", *param);
            break;
        case TRIGGER_TYPE_LIVE:
            cJSON_AddStringToObject(pJson, "type", "live");
            break;
        default:
            break;
    }
}

static void TriggerJsonBodyGen(int8 *body, RFTriggerConfig *config, int8 *deviceID)
{
    cJSON *pRoot = NULL;
    cJSON *pSub = NULL;
    int8 *p = NULL;

    pRoot = cJSON_CreateObject();
    if(pRoot)
    {
        cJSON_AddStringToObject(pRoot, "title", config->title);
        pSub = cJSON_CreateArray();
        if(pSub)
        {
            cJSON_AddItemToArray(pSub, cJSON_CreateString(deviceID));
            cJSON_AddItemToObject(pRoot, "dev_ids", pSub);
        }
        AddListToJsonArray(pRoot, "ds_uuids", config->dataStreamUUIDList);

        cJSON_AddStringToObject(pRoot, "url", config->remoteURL);

        AddTriggerTypeToJson(pRoot, config->triggerType, config->triggerParam);    

        p = (int8 *)cJSON_Print(pRoot);
        strcpy(body, (const char *)p);

        SAFE_FREE(p);
        cJSON_Delete(pRoot);
    }
}

int32 RFTrigger_Create(RFTriggerConfig *config, int8 *triggerID)
{
    int8 deviceID[16] = {0};
	int8 APIKey[63] = {0};
    int8 *pJsonBody = NULL;
    int8 *pRespBody = NULL;
    int32 ret = OSAERR;

    cJSON *pResp = NULL;
    cJSON *pSub = NULL;

    if(OSASUCCESS != DevInfo_GetDeviceID(deviceID))
        return OSAERR_DEVNOTFOUND;

    if(OSASUCCESS != DevInfo_GetMasterKey(APIKey))
        return OSAERR;

    HTTPComm_CreateDefaultHeader(HTTP_METHOD_POST, CLOUD_RES_TYPE_TRIGGER, APIKey, deviceID, NULL);
    pJsonBody = osAdaptionMemoryAlloc(1024);
    if(pJsonBody)
    {
        TriggerJsonBodyGen(pJsonBody, config, deviceID);

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
                        pSub = cJSON_GetObjectItem(pSub, "trigger_id");
                        if(pSub)
                        {
                            strcpy(triggerID, pSub->valuestring);
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

int32 RFTrigger_UpdateInfo(int8 *triggerID, RFTriggerConfig *config)
{
    int8 deviceID[16] = {0};
	int8 APIKey[63] = {0};
    int8 *pJsonBody = NULL;
    int8 *pRespBody = NULL;
    int32 ret = OSAERR;

    cJSON *pResp = NULL;
    cJSON *pSub = NULL;

    if(OSASUCCESS != DevInfo_GetDeviceID(deviceID))
        return OSAERR_DEVNOTFOUND;

    if(OSASUCCESS != DevInfo_GetMasterKey(APIKey))
        return OSAERR;

    HTTPComm_CreateDefaultHeader(HTTP_METHOD_PUT, CLOUD_RES_TYPE_TRIGGER, APIKey, triggerID, NULL);
    pJsonBody = osAdaptionMemoryAlloc(1024);
    if(pJsonBody)
    {
        TriggerJsonBodyGen(pJsonBody, config, deviceID);

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
    }
    return ret;
}

static int32 ParseJsonToTriggerConfig(cJSON *pRoot, RFTriggerConfig *config)
{
    cJSON *pSub = NULL;
    cJSON *pTmp = NULL;
    cJSON *pTmpSub = NULL;
    double *param = (double *)(config->triggerParam);
    int32 ret = OSAERR;
    int32 trigType = TRIGGER_TYPE_UNKNOWN;

    memset((void *)config, 0, sizeof(config));
    
    pSub = cJSON_GetObjectItem(pRoot, "title");
    if(pSub)
        strcpy(config->title, pSub->valuestring);

    pSub = cJSON_GetObjectItem(pRoot, "url");
    if(pSub)
        strcpy(config->remoteURL, pSub->valuestring);

    pSub = cJSON_GetObjectItem(pRoot, "invalid");
    if(pSub)
    {
        if(cJSON_True == pSub->type)
            ret = OSAERR_TRIGINVALID;
    }
    
    pSub = cJSON_GetObjectItem(pRoot, "type");
    if(pSub)
    {
        pTmp = cJSON_GetObjectItem(pRoot, "threshold");

        trigType = TriggerTypeStringToType(pSub->valuestring);

        if(((TRIGGER_TYPE_LT <= trigType) && (TRIGGER_TYPE_EQ >= trigType))
                || (TRIGGER_TYPE_FROZEN == trigType))
        {
            *param = pTmp->valueint;
        }
        else if((TRIGGER_TYPE_IN <= trigType) && (TRIGGER_TYPE_INOUT >= trigType))
        {
            pTmpSub = cJSON_GetObjectItem(pTmp, "lolmt");
            *param = pTmpSub->valueint;
            pTmpSub = cJSON_GetObjectItem(pTmp, "uplmt");
            *(param + 1) = pTmpSub->valueint;
        }
        else
            strcpy(config->triggerParam, pSub->valuestring);

        ret = OSASUCCESS;
    }

    return ret;
}

int32 RFTrigger_View(int8 *triggerID, RFTriggerConfig *config)
{
	int8 APIKey[63] = {0};
    int8 *pRespBody = NULL;
    int32 ret = OSAERR;

    cJSON *pResp = NULL;
    cJSON *pSub = NULL;

    if(OSASUCCESS != DevInfo_GetMasterKey(APIKey))
        return OSAERR;

    HTTPComm_CreateDefaultHeader(HTTP_METHOD_GET, CLOUD_RES_TYPE_TRIGGER, APIKey, triggerID, NULL);

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
                ParseJsonToTriggerConfig(pSub, config);
                ret = OSASUCCESS;
            }
            cJSON_Delete(pResp);
        }
        SAFE_FREE(pRespBody);
    }
    
    return ret;

}

int32 RFTrigger_Delete(int8 *triggerID)
{
	int8 APIKey[63] = {0};
    int8 *pRespBody = NULL;
    int32 ret = OSAERR;

    cJSON *pResp = NULL;
    cJSON *pSub = NULL;

    if(OSASUCCESS != DevInfo_GetMasterKey(APIKey))
        return OSAERR;

    HTTPComm_CreateDefaultHeader(HTTP_METHOD_DELETE, CLOUD_RES_TYPE_TRIGGER, APIKey, triggerID, NULL);

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

