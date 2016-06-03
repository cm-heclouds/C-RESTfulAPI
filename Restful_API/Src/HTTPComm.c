/******************************************************************************
 * Copyright (c), 2012~2016 iot.10086.cn All Rights Reserved
 * @file 		HTTPComm.c	
 * @brief		HTTP协议数据处理 
 * @author 		宋伟<songwei1@iot.chinamobile.com>
 * @date 		2016/05/19
 * @version 	1.0.0
 * @par Revision History:
 * 		Date			Author		Revised.Ver		Notes
 * 		2015/5/19		宋伟		  1.0.0			file created
 ******************************************************************************/

/*****************************************************************************/
/* Includes                                                                  */
/*****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <osAdaption/OSAErrno.h>
#include <HTTPProtocol.h>
#include <osAdaption/OSASocket.h>
#include <HTTPComm.h>

/*****************************************************************************/
/* Local Definitions ( Constant and Macro )                                  */
/*****************************************************************************/


/*****************************************************************************/
/* Local Function Prototype                                                  */
/*****************************************************************************/

/*****************************************************************************/
/* Local Variables                                                           */
/*****************************************************************************/
static pHTTPPacketGenerator s_pHTTPInst = NULL;
static int32 HTTPSocket = -1;



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
 * @brief 创建默认HTTP头部
 *
 * @Param :method 设定HTTP方法
 * @Param :type 设定操作的资源类型
 * @Param :APIKey 设定有操作权限的APIKey
 * @Param :firstID HTTP地址中的一级id
 * @Param :secondID HTTP地址中的二级id
 *
 * @Returns : 参见OSAErrno.h
 *************************************************************************/
int32 HTTPComm_CreateDefaultHeader(httpMethod method, cloudResType type, int8 *APIKey, int8 *firstID, int8 *secondID)
{
    if(NULL != s_pHTTPInst)
    {
        HTTP_PACKET_DESTROTY_INSTANCE(s_pHTTPInst);
        s_pHTTPInst = NULL;
    }
	HTTP_PACKET_GET_INSTANCE(s_pHTTPInst);

    s_pHTTPInst->constructor(s_pHTTPInst, firstID, secondID);
    s_pHTTPInst->setUrl(s_pHTTPInst, "api.heclouds.com");
    s_pHTTPInst->setMethod(s_pHTTPInst, method);
    s_pHTTPInst->setResType(s_pHTTPInst, type);
    s_pHTTPInst->addHeader(s_pHTTPInst, "api-key", APIKey);
    s_pHTTPInst->addHeader(s_pHTTPInst, "Host", "api.heclouds.com");

    return OSASUCCESS;
}

/*************************************************************************
 * @brief 增加指定HTTP头部
 *
 * @Param :header 头域名
 * @Param :value 值
 *
 * @Returns : 参见OSAErrno.h
 *************************************************************************/
int32 HTTPComm_AddHeader(int8 *header, int8* value)
{
    if(NULL != s_pHTTPInst)
    {
        s_pHTTPInst->addHeader(s_pHTTPInst, header, value);
        return OSASUCCESS;
    }
    return OSAERR;
}

int32 HTTPComm_AddUrlParam(int8* value)
{
    if(NULL != s_pHTTPInst)
    {
        s_pHTTPInst->addUrlParam(s_pHTTPInst, value);
        return OSASUCCESS;
    }
    return OSAERR;
}

/*************************************************************************
 * @brief 建立到指定主机的HTTP
 *
 * @Param :host 主机名，支持域名和点分十进制IP地址
 * @Param :port 端口
 *
 * @Returns : 参见OSAErrno.h
 *************************************************************************/
int HTTPComm_Connect(int8 *host, uint16 port)
{
    int32 ret = OSASUCCESS;

    HTTPSocket = OSASocket_Create(SOCKET_TYPE_TCP);

	if(0 <= HTTPSocket)
	{
		ret = OSASocket_Connect(HTTPSocket, host, port); 
	}

    return ret;
}

/*************************************************************************
 * @brief 发送构造的Http Body，并获取响应。如果响应正常则返回响应中的JSON
 *        字符串（使用后需释放），否则返回空指针
 *
 * @Param :sendHttpBody 需要发送的Body缓冲区
 * @Param :bodyLen 需要发送的数据长度
 * @Param :respLen 返回接收到的响应数据长度
 *
 * @Returns : 操作成功 - 返回HTTP响应的JSON Body
 *************************************************************************/
int8* HTTPComm_Process(int8 *sendHttpBody, uint32 bodyLen, uint32 *respLen)
{
    int8 bodyLenStr[8] = {0};
    int8 *sendBuf = NULL;
    int8 *contentLen = NULL;
    uint32 sendLen = 0;
	int8 respDataBuf[4096] = {0};
	int32 ret = OSASUCCESS;
    boolValue largeBody = false;

	pHttpParser pParser = NULL;
	int8 *JsonBody = NULL;
    int8 *pRespBody = NULL;

    /* 如果发送的数据过大，HTTP封装内部buffer不够，需要采用另外的方式*/
    if(bodyLen > BODY_LEN)
        largeBody = true;
    /* 补全HTTP头部的Content-Length域*/
    sprintf((char *)bodyLenStr, "%d", bodyLen);
    s_pHTTPInst->addHeader(s_pHTTPInst, "Content-Length", bodyLenStr);

    /* 插入JSON Body到HTTP头部后，并生成发送数据缓冲区*/
    if(sendHttpBody && (false == largeBody))
        s_pHTTPInst->addBody(s_pHTTPInst, sendHttpBody, bodyLen);
    s_pHTTPInst->packetGen(s_pHTTPInst);

    sendBuf = s_pHTTPInst->getContent(s_pHTTPInst);
    sendLen = strlen(sendBuf);

    /* 发送HTTP请求*/
    ret = OSASocket_Send(HTTPSocket, (uint8 *)sendBuf, sendLen, 500);
    if(largeBody)
        ret = OSASocket_Send(HTTPSocket, (uint8 *)sendHttpBody, bodyLen, 500);
    if(ret)
    {
        /* 接收HTTP响应*/
        ret = OSASocket_Recv(HTTPSocket, (uint8 *)respDataBuf, 4096, 1000);
        if(0 < ret)
        {
            /* 解析HTTP响应，确认HTTP返回码为200*/
            HTTP_RES_PARSER_GET_INSTANCE(pParser);
            pParser->constructor(pParser);
            pParser->startParse(pParser, respDataBuf, ret);
            if(200 == pParser->getResponseCode(pParser).errorCode)
            {
                /* 获取HTTP响应的JSON字符串*/
                JsonBody = pParser->getContentBody(pParser);
                if(JsonBody)
                {
                    pRespBody = osAdaptionMemoryAlloc(strlen(JsonBody) + 1);
                    if(respLen)
                    {
                        contentLen = pParser->getContentLength(pParser);
                        if(contentLen)
                            *respLen = atoi(contentLen);
                    }
                    if(pRespBody)
                        strcpy(pRespBody, JsonBody);
                }
            }
            HTTP_RES_PARSER_DESTROTY_INSTANCE(pParser);
        }
    }

    /* 销毁资源*/
    ret = OSASocket_Close(HTTPSocket);
    HTTPSocket = -1;

    if(NULL != s_pHTTPInst)
    {
        HTTP_PACKET_DESTROTY_INSTANCE(s_pHTTPInst);
        s_pHTTPInst = NULL;
    }

    if(OSASUCCESS == ret)
        return pRespBody;
    else
        return NULL;
}
