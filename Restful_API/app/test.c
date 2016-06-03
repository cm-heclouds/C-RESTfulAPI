/******************************************************************************
 * Copyright (c), 2012~2016 iot.10086.cn All Rights Reserved
 * @file 		
 * @brief 
 * @author 		宋伟<songwei1@iot.chinamobile.com>
 * @date 		2016/05/23
 * @version 	1.0.0
 * @par Revision History:
 * 		Date			Author		Revised.Ver		Notes
 * 		2015/5/23		宋伟		  1.0.0			file created
 ******************************************************************************/


//gcc -o test test.c -L../Lib -I../Inc -lRestFulLinux -losAdaption -lm -g


/*****************************************************************************/
/* Includes                                                                  */
/*****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <osAdaption/OSAErrno.h>
#include <RFDevice.h>
#include <RFStream.h>
#include <RFDataPoint.h>
#include <RFAPIKey.h>
#include <RFDeviceDetails.h>
#include <RFTrigger.h>

/*****************************************************************************/
/* Local Definitions ( Constant and Macro )                                  */
/*****************************************************************************/
#define DEVICE_TITLE ""
#define MASTER_KEY ""
#define TAGLIST	""


/*****************************************************************************/
/* Local Function Prototype                                                  */
/*****************************************************************************/

/*****************************************************************************/
/* Local Variables                                                           */
/*****************************************************************************/
static LocationInfo loc = {500, 29.521, 106.542};



/*****************************************************************************/
/* Global Variables                                                          */
/*****************************************************************************/


/*****************************************************************************/
/* External Functions and Variables                                          */
/*****************************************************************************/



/*****************************************************************************/
/* Function Implementation                                                   */
/*****************************************************************************/

int main(int argc, char **argv[])
{
    RFDeviceConfig config;
    RFStreamConfig sConfig;
    RFDeviceDetails details;
    RFDataPoint data[5];
    RFAPIKeyInfo apiKey;
	RFTriggerConfig tConfig;
    RFAPIKeyInfo *testInfo = NULL;
    char deviceid[64] = {0};
    char streamUUID[64] = {0};
    char binIndex[64] = {0};
	char triggerId[64] = {0};
    char testBuf[4096] = {0};
    int i = 0;
    int ret = 0;

    /* 根据实际情况完善MASTER_KEY, DEVICE_TITLE, TAGLIST的宏定义*/
    memset(&config, 0, sizeof(config));
    strcpy(config.APIKey, MASTER_KEY); 
    strcpy(config.deviceTitle, DEVICE_TITLE);
    strcpy(config.deviceDesc, "whatever");
    strcpy(config.deviceTagList, TAGLIST);
	memcpy(&(config.locInfo), &loc, sizeof(LocationInfo));
    config.isPrivate = true;
    
	/* 向云端添加设备*/
	if(OSASUCCESS == (ret = RFDevice_Create(&config, deviceid)))
	{
		printf("Create Device Success, deviceID is %s\n", deviceid);
		//RFDevice_Init(deviceid, config.APIKey);
		sleep(5);
		memset(&(config.locInfo), 0, sizeof(LocationInfo));
		/* 更新设备信息*/
		if(OSASUCCESS == (ret = RFDevice_UpdateInfo(&config, DEVICEINFO_MASK_LOCATION)))
		{
			printf("Clear Location success !\n");
			sleep(5);
			memset(&sConfig, 0, sizeof(sConfig));
			strcpy(sConfig.streamID, "TestStreamOps");
			strcpy(sConfig.streamTagList, TAGLIST);
			strcpy(sConfig.unit, "Meter");
			strcpy(sConfig.unitSymbol, "M");
			/* 增加数据流*/
			if(OSASUCCESS == (ret = RFStream_Create(&sConfig, MASTER_KEY, streamUUID)))
			{
				printf("Create Stream Success, UUID is %s\n", streamUUID);
				sleep(5);
				strcpy(sConfig.streamTagList, "tag3,tag2,tag1");
				/* 更新数据流信息*/
				if(OSASUCCESS == (ret = RFStream_UpdateInfo(sConfig.streamID, MASTER_KEY, &sConfig, STREAMINFO_MASK_TAG)))
				{
					printf("Update StreamInfo Success !\n");
					for(i = 0; i < 5; i++)
					{
						strcpy(data[i].streamID, "TestStreamOps");
						data[i].time = time(NULL);
						data[i].valueType = DATAPOINT_VALUE_TYPE_NUMBER;
						*((double *)(data[i].value)) = i*3;
					}
					/* 向云端增加数据点*/
					if(OSASUCCESS == (ret = RFDataPoint_Create(config.APIKey, data, 5)))
					{
						printf("Data Upload success !\n");
						sleep(5);
						strcpy(apiKey.title, "all"); 
						apiKey.permsFlag = APIKEY_PERMISSION_FLAG_GET;
						/* 创建API Key*/
						if(OSASUCCESS == (ret = RFAPIKey_Create(&apiKey)))
						{
							printf("Create API Key %s\n", apiKey.key);
							sleep(5);
							apiKey.permsFlag = APIKEY_PERMISSION_FLAG_PUT;
							/* 更新API Key信息*/
							if(OSASUCCESS == (ret = RFAPIKey_UpdateInfo(&apiKey)))
							{
								printf("Update API Key Success !\n");
								sleep(5);
								/* 查看API Key信息*/
								if(0 < (ret = RFAPIKey_View(&testInfo)))
								{
									for(i = 0; i < ret; i++)
									{
										printf("Title %s, C Time %d   perm %d\n", testInfo[i].title, testInfo[i].createTime, testInfo[i].permsFlag);
									}
									free(testInfo);
								}
							}
							/* 删除API Key*/
							RFAPIKey_Delete(apiKey.key);
							printf("Delete API Key \n");
							sleep(5);
						}

					}

				}
				RFStream_Delete(MASTER_KEY, sConfig.streamID);
				printf("Delete stream \n");
				sleep(5);
			}

		}
		RFDevice_Delete(MASTER_KEY);
		printf("Delete Device !\n");
	}
    return 0;
}


