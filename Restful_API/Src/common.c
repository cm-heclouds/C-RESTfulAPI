/******************************************************************************
 * Copyright (c), 2012~2016 iot.10086.cn All Rights Reserved
 * @file 		common.c
 * @brief       一些通用接口封装
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
#include <time.h>

#include <common.h>

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
 * @brief 将固定格式的列表("item1,item2,item3")添加到JSON数值结构
 *
 * @Param :pRoot
 * @Param :name
 * @Param :list
 *************************************************************************/
void AddListToJsonArray(cJSON *pRoot, int8 *name, int8 *list)
{
    int8 *tmpPtr = NULL; 
    int8 tmpBuf[16] = {0};
    cJSON *pArray = NULL;
    
    if((NULL == list) || (0 == strlen((const char *)list)))
    {
        return;
    }
    
    pArray = cJSON_CreateArray();
    if(pArray)
    {
        tmpPtr = strstr((const char *)list, ","); 
        if(tmpPtr)
        {
            do 
            {
                memset((void *)tmpBuf, 0, 16); 
                memcpy((void *)tmpBuf, (const void *)list, (tmpPtr - list));
                cJSON_AddItemToArray(pArray, cJSON_CreateString(tmpBuf));
                list = tmpPtr + 1;
                tmpPtr = strstr((const char *)list, ",");
            } while(tmpPtr);
        }

        cJSON_AddItemToArray(pArray, cJSON_CreateString(list));

        cJSON_AddItemToObject(pRoot, name, pArray);
    }
}

void JsonArrayToList(cJSON *pArray, int8 *list)
{
    uint32 i = 0;
    uint32 itemSize = cJSON_GetArraySize(pArray);
    cJSON *item = NULL;

    if(itemSize > 0)
    {
        for(i = 0; i < itemSize; i++)
        {
            item = cJSON_GetArrayItem(pArray, i);
            if(item)
            {
                strcat(list, item->valuestring);
                if(i != (itemSize - 1))
                    strcat(list, ",");
            }
        }
    }
}

/*************************************************************************
 * @brief 将JSON中的时间字段转换为linux timestamp
 *
 * @Param :timeStr
 *
 * @Returns :
 *************************************************************************/
uint32 TimeStrToTimeStamp(int8 *timeStr)
{
    struct tm tm;

    sscanf(timeStr, "%d-%02d-%02d %02d:%02d:%02d",
            &(tm.tm_year), &(tm.tm_mon), &(tm.tm_mday), 
            &(tm.tm_hour), &(tm.tm_min), &(tm.tm_sec));
    tm.tm_year -= 1900;
    tm.tm_mon -= 1;
    return mktime(&tm);
}

