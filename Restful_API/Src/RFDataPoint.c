/******************************************************************************
 * Copyright (c), 2012~2016 iot.10086.cn All Rights Reserved
 * @file 		RFDataPoint.c
 * @brief       数据点操作接口封装，提供上报数据点的功能
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
#include <time.h>

#include <cJSON.h>
#include <osAdaption/OSAErrno.h>
#include <HTTPProtocol.h>
#include <HTTPComm.h>
#include <RFDataPoint.h>
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
static cJSON *DataPointJsonBodyGetDataPointsItem(cJSON *dsArrays, int8* streamID)
{
    int32 i = 0;
    int32 dsCnt = 0;
    int32 found = 0;
    cJSON *ds = NULL;
    cJSON *dp = NULL;
    cJSON *tmp = NULL;

    /*　检查当前stream是否已加入json结构*/
    found = 0;
    dsCnt = cJSON_GetArraySize(dsArrays);
    for(i = 0; i < dsCnt; i++)
    {
        ds = cJSON_GetArrayItem(dsArrays, i);         
        if(ds)
        {
            dp = cJSON_GetObjectItem(ds, "id");
            if(dp && (0 == strcmp((const char *)streamID, (const char *)(dp->valuestring))))
            {
                found = 1;
                tmp = cJSON_GetObjectItem(ds, "datapoints");
                break;
            }

        }
    }
    if(0 == found)
    {
        ds = cJSON_CreateObject();
        if(ds)
        {
            cJSON_AddStringToObject(ds, "id", (const char *)streamID);
            tmp = cJSON_CreateArray();
            cJSON_AddItemToObject(ds, "datapoints", tmp);
            cJSON_AddItemToArray(dsArrays, ds);
        }
    }

    return tmp;
}

static void DataPointJsonBodyAddPoint(cJSON *item, RFDataPoint *data)
{
    int8 timeStr[32] = {0};
    int8 *tmp = NULL;
    cJSON *value = NULL;
    struct tm *t = NULL;

    value = cJSON_CreateObject();
    if(value)
    {
        if(0 != data->time)
        {
            t = localtime((time_t *)(&(data->time)));
            sprintf(timeStr, "%d-%02d-%02dT%02d:%02d:%02d", (1900 + t->tm_year), (t->tm_mon + 1), t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec);
            cJSON_AddStringToObject(value, "at", timeStr);
        }
        if(DATAPOINT_VALUE_TYPE_STRING == data->valueType)
            cJSON_AddStringToObject(value, "value", data->value);
        else
        {
            tmp = data->value;
            cJSON_AddNumberToObject(value, "value", *((double *)tmp));
        }
        cJSON_AddItemToArray(item, value);
    }
}

static int32 DataPointJsonBodyGen(int8 *body, RFDataPoint *data, int32 count)
{
    int32 i = 0;
    cJSON *pRoot = NULL;
    cJSON *content = NULL;
    cJSON *dp = NULL;
    int8 *p = NULL;

#if 0
    pRoot = cJSON_CreateObject();
    if(NULL == pRoot)
        return OSAERR_NOMEM;
#endif
    content = cJSON_CreateArray();
    if(NULL == content)
    {
        return OSAERR_NOMEM;
    }

    for(i = 0; i < count; i++)
    {
        /*　检查当前stream是否已加入json结构*/
        dp = DataPointJsonBodyGetDataPointsItem(content, data[i].streamID);
        DataPointJsonBodyAddPoint(dp, &(data[i]));
    }

    pRoot = cJSON_CreateObject();
    cJSON_AddItemToObject(pRoot, "datastreams", content);
    p = (int8 *)cJSON_Print(pRoot);
    strcpy(body, (const char *)p);

    SAFE_FREE(p);
    cJSON_Delete(pRoot);
    return OSASUCCESS;
}

/*************************************************************************
 * @brief 向云端新增数据点。使用示例如下：
 *      　RFDataPoint dataPoint[5];
 *        
 *        setValue(dataPoint); ///对结构体数组各成员进行赋值
 *        RFDataPoint_Create(APIKey, dataPoint, 5);
 *
 * @Param :APIKey 有权限执行新增数据点操作的API Key
 * @Param :data 需要增加的数据。该参数为结构体数组头指针，数组成员个数由count确定
 * @Param :count　指定需要增加的数据点个数
 *
 * @Returns :
 *************************************************************************/
int32 RFDataPoint_Create(int8 *APIKey, RFDataPoint *data, int32 count)
{
    int32 ret = OSAERR;
    int8 deviceID[16] = {0};
    int8 *pJsonBody = NULL;
    int8 *pRespBody = NULL;
    
    cJSON *pResp = NULL;
    cJSON *pSub = NULL;

    if(OSASUCCESS != DevInfo_GetDeviceID(deviceID))
        return OSAERR_DEVNOTFOUND;

    HTTPComm_CreateDefaultHeader(HTTP_METHOD_POST, CLOUD_RES_TYPE_DATAPOINT, APIKey, deviceID, NULL);
    pJsonBody = osAdaptionMemoryAlloc(1024);
    if(pJsonBody)
    {
        DataPointJsonBodyGen(pJsonBody, data, count);

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
#if 0 /* 由于数据点查看的选项和内容过多，且实际应用中设备终端几乎不需要此功能，暂不实现*/
int32 RFDataPoint_View()
{
    return OSASUCCESS;
}
#endif

