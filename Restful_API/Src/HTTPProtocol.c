/******************************************************************************
* Copyright (c), 2012~2015 iot.10086.cn All Rights Reserved
* @file         DeviceManager.c 
* @brief        device manager logic implementation
* @details      device manager logic implementation
* @author       DengXing 
* @date			2015/7/5 
* @version		1.0.0
* @par Revision History:
*      Date			Author		Revised.Ver		Notes
*      2015/6/30	DengXing	1.0.0			file created
******************************************************************************/

/*****************************************************************************/
/* Includes                                                                  */
/*****************************************************************************/
#include "string.h"
#include "assert.h"
#include "stdlib.h"

#include <osAdaption/osAdaption.h>
#include <HTTPProtocol.h>

/*****************************************************************************/
/* Local Definitions （Constant and Macro )                                  */
/*****************************************************************************/


/*****************************************************************************/
/* Local Function Prototype                                                         */
/*****************************************************************************/
///< http packet method
static void httpDestructor(void* this);
static void httpSetUrl(void* this, int8* url);
static void httpSetMethod(void* this, uint32 method); 
static void httpSetResType(void* this, cloudResType type);
static void httpAddHeader(void* this, int8* key, int8* value);
static void httpAddBody(void* this, int8* buf, uint32 bodyLen);
static void httpAddUrlParam(void* this, int8* paramStr);
static void httpPacketGen(void* this);
static int8* getHttpContent(void* this);
///< http parser method
static void httpParserDestructor(void* this);
static void httpParserStartProcess(void* this, int8* buf, int32 len);
static httpResponseCode httpParserGetResponseCode(void* this);
static int8* httpParserGetContentLength(void* this);
static int8* httpParserGetContentType(void* this);
static int8* httpParserGetContentEncoding(void* this);
static int8* httpParserGetContentBody(void* this);
static int8* httpParserGetSpecificHeader(void* this, int8* headerKey);
static void httpParseResCode(void* this, int8* buf, int32 len);
static void httpParseHeaderSingleLine(void* this, uint32 count, int8* pline, int32 len);
static void httpParseHeader(void* this, int8* buf, int32 len);

///< genernal function
static void generateGetMethodPath(int8* path, int8* firstID, 
                int8* secondID, cloudResType type);
static void generatePostMethodPath(int8* path, int8* firstID, 
                int8* secondID, cloudResType type);
static void generatePutMethodPath(int8* path, int8* firstID, 
                int8* secondID, cloudResType type);
static void generateDeleteMethodPath(int8* path, int8* firstID, 
                int8* secondID, cloudResType type);

/*****************************************************************************/
/* Local Variables                                                                       */
/*****************************************************************************/

/*****************************************************************************/
/* Global Variables                                                                     */
/*****************************************************************************/

/*****************************************************************************/
/* External Functions and Variables                                               */
/*****************************************************************************/

/*****************************************************************************/
/* Function Implementation                                                          */
/*****************************************************************************/
#define HTTP_PACKET_METHOD
void httpConstructor(void* this, int8* firstID, int8* secondID)
{
    pHTTPPacketGenerator pthis = (pHTTPPacketGenerator)this;

    assert(pthis);
    pthis->destructor = httpDestructor;
    pthis->setUrl = httpSetUrl;
    pthis->setMethod = httpSetMethod;
    pthis->setResType = httpSetResType;
    pthis->addHeader = httpAddHeader;
    pthis->addBody = httpAddBody;
    pthis->addUrlParam = httpAddUrlParam;
    pthis->packetGen = httpPacketGen;
    pthis->getContent = getHttpContent;

     if (firstID != NULL)
    {
        pthis->firstID = (int8*)osAdaptionMemoryAlloc(strlen((int8*)firstID)+1);
        if (pthis->firstID)
            strcpy(pthis->firstID, firstID);
    }

    if (secondID != NULL)
    {
        pthis->secondID = (int8*)osAdaptionMemoryAlloc(strlen((int8*)secondID)+1);
        if (pthis->secondID)
            strcpy(pthis->secondID, secondID);
    }

    memset(pthis->array, 0, sizeof(httpHeaderPair)*HTTP_HEADER_PAIR_NUM);
    memset(pthis->urlParam, 0, (URLPARAM_LEN + 1));
}

static void httpDestructor(void* this)
{
    pHTTPPacketGenerator pthis = (pHTTPPacketGenerator)this;

    assert(pthis);
    SAFE_FREE(pthis->firstID);
    SAFE_FREE(pthis->secondID);
}

static void httpSetUrl(void* this, int8* url)
{
    pHTTPPacketGenerator pthis = (pHTTPPacketGenerator)this;

    assert(pthis);
    if (url != NULL)
    {
        strncpy(pthis->url, url, URL_LEN);
        pthis->url[URL_LEN] = 0x00;
    }
}

static void httpSetMethod(void* this, uint32 method)
{ 
     pHTTPPacketGenerator pthis = (pHTTPPacketGenerator)this;

     assert(pthis);
     pthis->method = method;
}

static void httpSetResType(void* this, cloudResType type)
{   
     pHTTPPacketGenerator pthis = (pHTTPPacketGenerator)this;

     assert(pthis);
     pthis->resType = type;
}

static void httpAddHeader(void* this, int8* key, int8* value)
{
    uint32 i = 0;
    pHTTPPacketGenerator pthis = (pHTTPPacketGenerator)this;

    assert(pthis);
    if (key != NULL && value != NULL)
    {
        while (i < HTTP_HEADER_PAIR_NUM)
        {
            if (pthis->array[i].key[0] != 0x00){
				i++;
                continue;
			}

            strncpy(pthis->array[i].key, key, HTTP_HEADER_KEY);
            pthis->array[i].key[HTTP_HEADER_KEY] = 0x00;
            
            strncpy(pthis->array[i].value, value, HTTP_HEADER_VALUE);
            pthis->array[i].value[HTTP_HEADER_VALUE+1] = 0x00;
			break;
        }
    }
}

static void httpAddBody(void* this, int8* buf, uint32 bodyLen)
{
    pHTTPPacketGenerator pthis = (pHTTPPacketGenerator)this;

    assert(pthis);
    if (buf != NULL)
    {
        memset(pthis->body, 0, BODY_LEN);
        pthis->bodyLen = bodyLen;
        memcpy(pthis->body, buf, bodyLen);
    }
}

static void httpAddUrlParam(void* this, int8* paramStr)
{
    pHTTPPacketGenerator pthis = (pHTTPPacketGenerator)this;

    assert(pthis);
    if (paramStr != NULL)
    {
        strcpy(pthis->urlParam, paramStr);
    }

}

static void httpPacketGen(void* this)
{
    int32 i = 0;
    pHTTPPacketGenerator pthis = (pHTTPPacketGenerator)this;
    
    assert(pthis);

    ///<step 1, gen path
    switch (pthis->method)
    {
        case HTTP_METHOD_POST:
            generatePostMethodPath(pthis->path, pthis->firstID, pthis->secondID, pthis->resType);
            break;
        case HTTP_METHOD_GET:
            generateGetMethodPath(pthis->path, pthis->firstID, pthis->secondID, pthis->resType);
            break;
        case HTTP_METHOD_PUT:
            generatePutMethodPath(pthis->path, pthis->firstID, pthis->secondID, pthis->resType);
            break;
        case HTTP_METHOD_DELETE:
            generateDeleteMethodPath(pthis->path, pthis->firstID, pthis->secondID, pthis->resType);
            break;
        default:
            break;
    }
    ///< step 2, append path
    memset(pthis->content, 0, CONTENT_LEN+1);
    strcpy(pthis->content, pthis->path);
    if(strlen(pthis->urlParam))
    {
        strcat(pthis->content, pthis->urlParam);
    }
    strcat(pthis->content, " HTTP/1.1\r\n");
    ///< step 3, append http header
    for (i = 0;  i < HTTP_HEADER_PAIR_NUM;  i++)
    {
        if (pthis->array[i].key[0] == 0x00)
                break;

        strcat(pthis->content, pthis->array[i].key);
        strcat(pthis->content, ":");
        strcat(pthis->content, pthis->array[i].value);
        strcat(pthis->content, "\r\n");
    }
    ///< step 4, append body
    strcat(pthis->content, "\r\n");
#if 0
    strcat(pthis->content, pthis->body);
#else
    memcpy(pthis->content + strlen(pthis->content), pthis->body, pthis->bodyLen);
    /*HTTP body可能不是字符串*/
#endif
}

static int8* getHttpContent(void* this)
{
    pHTTPPacketGenerator pthis = (pHTTPPacketGenerator)this;
    
    assert(pthis);

    return pthis->content;
}

#define HTTP_RESPONSE_PARSER_METHOD
void httpParserConstructor(void* this)
{
     pHttpParser pthis = (pHttpParser)this;
     assert(pthis);

     memset(pthis, 0, sizeof(httpParser));
     pthis->destructor = httpParserDestructor;
     pthis->startParse = httpParserStartProcess;
     pthis->getResponseCode = httpParserGetResponseCode;
     pthis->getContentLength = httpParserGetContentLength;
     pthis->getContentType = httpParserGetContentType;
     pthis->getContentEncoding = httpParserGetContentEncoding;
     pthis->getContentBody = httpParserGetContentBody;
     pthis->pthis = this;
}

void httpParserDestructor(void* this)
{
     ///< memory free or other clean operations
}

void httpParserStartProcess(void* this, int8* buf, int32 len)
{
    pHttpParser pthis = (pHttpParser)this;
    int8* pbufStart = buf;
    int8* pbufEnd = buf;
    int32 length = 0;
    int32 totalLen = 0;
    int32 bodyLen = 0;
    
    assert(pthis);

    if (pbufStart != NULL && pbufEnd != NULL)
    {
        ///< step 1, get response code and error string
        pbufEnd = strstr(pbufStart, "\r\n");
        pbufEnd++;
        length = pbufEnd - pbufStart + 1;
        totalLen += length;
        httpParseResCode(pthis, pbufStart, length);
        ///< step 2, get the response header
        pbufStart = pbufEnd+1;
        pbufEnd = strstr(pbufStart, "\r\n\r\n");
        if (pbufEnd != NULL) ///< if have body
        {
            pbufEnd += 3;
            length = pbufEnd - pbufStart + 1;
            totalLen += length;
            httpParseHeader(pthis, pbufStart, length);
            ///< copy the response body
            pbufStart = pbufEnd+1;
            memset(pthis->body, 0, RESP_BODY_LEN+1);
            bodyLen =( len - totalLen) > RESP_BODY_LEN  ? RESP_BODY_LEN: ( len - totalLen);
            memcpy(pthis->body, pbufStart, bodyLen);
        }
        else
        {
             httpParseHeader(pthis, pbufStart, len - totalLen);
        }
    }
}

httpResponseCode httpParserGetResponseCode(void* this)
{
    pHttpParser pthis = (pHttpParser)this;
    assert(pthis);

    return pthis->responseCode;
}

int8* httpParserGetContentLength(void* this)
{
   pHttpParser pthis = (pHttpParser)this;
    int8*   buf = NULL;

    assert(pthis);
    buf = httpParserGetSpecificHeader(pthis, "Content-Length");

    return buf;

    
}

int8* httpParserGetContentType(void* this)
{
    pHttpParser pthis = (pHttpParser)this;
    int8*   buf = NULL;

    assert(pthis);
    buf = httpParserGetSpecificHeader(pthis, "Content-Type");

    return buf;
}

int8* httpParserGetContentEncoding(void* this)
{
    pHttpParser pthis = (pHttpParser)this;
    int8*   buf = NULL;

    assert(pthis);
    buf = httpParserGetSpecificHeader(pthis, "Content-Encoding");

    return buf;
}

int8* httpParserGetContentBody(void* this)
{
    pHttpParser pthis = (pHttpParser)this;
    
    assert(pthis);

    return pthis->body;
}

int8* httpParserGetSpecificHeader(void* this, int8* headerKey)
{
    pHttpParser pthis = (pHttpParser)this;
    uint32    i = 0;
    int8*  buf = NULL;
    assert(pthis);

    if (headerKey != NULL)
    {
        for (i = 0;  i< HTTP_HEADER_PAIR_NUM; i++)
        {
            if (strcmp((const int8*)headerKey, (const int8*)pthis->array[i].key) == 0x00)
            {
                break;
            }
        }

        if (i < HTTP_HEADER_PAIR_NUM)
        {
            buf = pthis->array[i].value;
        }
    }

    return buf;
}

void httpParseResCode(void* this, int8* buf, int32 len)
{
    pHttpParser pthis = (pHttpParser)this;
    int8* pbufStart = buf;
    int8* pbufEnd = buf;
    int8 result[128] = {0};
    int8* pResult = result;
    
    assert(pthis);

    if (pbufStart != NULL && pbufEnd != NULL)
    {
        ///< step 1, get response code and error string
        pbufStart = strstr(pbufStart, " "); 
        //pbufEnd = strstr(pbufStart, " ");
		pbufEnd = strstr(pbufStart+1, " ");	///<修正http resp code 解析错误
        pbufStart++; 
        while (pbufStart < pbufEnd)
        {
            *pResult++ = *pbufStart++;
        }
        pthis->responseCode.errorCode = atoi(result);

        pbufStart++;
        pbufEnd = strstr(pbufStart, "\r\n");
        memset(result, 0, 128);
        pResult = result;
         while (pbufStart < pbufEnd)
        {
            *pResult++ = *pbufStart++;
        }
        strcpy(pthis->responseCode.errorString, result);
    }
}

void httpParseHeader(void* this, int8* buf, int32 len)
{
    pHttpParser pthis = (pHttpParser)this;
    int8* pbufStart = buf;
    int8* pbufEnd = buf;
    int32 length = 0;
    int32 totalLen = 0;
    int32 i = 0;
    
    assert(pthis);
    
    if (pbufStart != NULL && pbufEnd != NULL)
    {
        for (i = 0;  i < HTTP_HEADER_PAIR_NUM && totalLen <  len;  i++)
        {
            pbufEnd = strstr(pbufStart, "\r\n");
			if(pbufEnd == pbufStart){	///<修正http header 解析错误
				return;
			}
            pbufEnd++;
            length = pbufEnd - pbufStart + 1;
            totalLen += length;
            httpParseHeaderSingleLine(pthis, i , pbufStart, length);
            pbufStart = pbufEnd + 1;
        }
    }
}

void httpParseHeaderSingleLine(void* this, uint32 count, int8* pline, int32 len)
{
    pHttpParser pthis = (pHttpParser)this;
    int8* pbufStart = pline;
    int8* pbufEnd = pline;
    int32 length = 0;
    int32 totalLen = 0;

    assert(pthis);
    
    if (pbufStart != NULL && pbufEnd != NULL)
    {
        ///< copy the header key
        pbufEnd = strstr(pbufStart, ": ");
        /* 不拷贝冒号*/
        length = pbufEnd - pbufStart;
        memcpy(pthis->array[count].key, pbufStart, length);
        /* 跳过": "*/
        totalLen += (length + 2);
        ///< copy the header value
#if 0
        pbufStart += 2;
        totalLen += 2;
#else
        pbufStart += totalLen;
#endif
        memcpy(pthis->array[count].value, pbufStart, (len - totalLen - 2)); ///< minus \r\n
    }
}

#define HTTP_GENERNAL_FUNCTION


static void generateGetMethodPath(int8* path, int8* firstID, int8* secondID, cloudResType type)
{
    //assert(path&&firstID&&secondID);
    
    strcpy(path, "GET ");

    switch(type)
    {
        case CLOUD_RES_TYPE_USER:
            strcat(path, "/user");
            break;

        case CLOUD_RES_TYPE_DEVICE:
            strcat(path, "/devices/");
            strcat(path, firstID);
            break;

        case CLOUD_RES_TYPE_DATASTREAM:
            strcat(path, "/devices/");
            strcat(path, firstID);
            strcat(path, "/datastreams/");
            strcat(path, secondID);
            break;

        case CLOUD_RES_TYPE_DATAPOINT:
            strcat(path, "/devices/");
            strcat(path, firstID);
            strcat(path, "/datapoints");
            break;

        case CLOUD_RES_TYPE_TRIGGER:
            strcat(path, "/triggers/");
            strcat(path, firstID);
            break;

        case CLOUD_RES_TYPE_APIKEY:
            strcat(path, "/keys");
            break;

        case CLOUD_RES_TYPE_BINDATA:
            strcat(path, "/bindata/");
            strcat(path, firstID);
            break;

        case CLOUD_RES_TYPE_CMD:
            strcat(path, "/cmds/");
            strcat(path, firstID);
            break;
        case CLOUD_RES_TYPE_APILOG:
            strcat(path, "/logs/");
            strcat(path, firstID);
            break;

        case CLOUD_RES_TYPE_HISTORYDATA:
            strcat(path, "/datapoints");
            break;

        default:
            break;
    }
}

static void generatePostMethodPath(int8* path, int8* firstID, int8* secondID, cloudResType type)
{
    //assert(path&&firstID&&secondID);

    strcpy(path, "POST ");

    switch(type)
    {
        case CLOUD_RES_TYPE_DEVICE:
			assert(path);
            strcat(path, "/devices");
            break;

        case CLOUD_RES_TYPE_DATASTREAM:
			assert(path&&firstID);
            strcat(path, "/devices/");
            strcat(path, firstID);
            strcat(path, "/datastreams");
            break;

        case CLOUD_RES_TYPE_DATAPOINT:
			assert(path&&firstID);
            strcat(path, "/devices/");
            strcat(path, firstID);
            strcat(path, "/datapoints");
            break;

        case CLOUD_RES_TYPE_TRIGGER:
			assert(path);
            strcat(path, "/triggers");
            break;

        case CLOUD_RES_TYPE_APIKEY:
			assert(path);
            strcat(path, "/keys");
            break;

        case CLOUD_RES_TYPE_BINDATA:
			assert(path);
            strcat(path, "/bindata");
            break;
        case CLOUD_RES_TYPE_CMD:
            assert(path);
            strcat(path, "/cmds");
            break;

        default:
            break;
    }
}

static void generatePutMethodPath(int8* path, int8* firstID, int8* secondID, cloudResType type)
{
   //assert(path&&firstID&&secondID);   

    strcpy(path, "PUT ");

    switch(type)
    {
        case CLOUD_RES_TYPE_DEVICE:
            strcat(path, "/devices/");
            strcat(path, firstID);
            break;

        case CLOUD_RES_TYPE_DATASTREAM:
            strcat(path, "/devices/");
            strcat(path, firstID);
            strcat(path, "/datastreams/");
            strcat(path, secondID);
            break;

        case CLOUD_RES_TYPE_TRIGGER:
            strcat(path, "/triggers/");
            strcat(path, firstID);
            break;

        case CLOUD_RES_TYPE_APIKEY:
            strcat(path, "/keys/");
            strcat(path, firstID);
            break;

        default:
            break;
    }
}

static void generateDeleteMethodPath(int8* path, int8* firstID, int8* secondID, cloudResType type)

{
   //assert(path&&firstID&&secondID);   

    strcpy(path, "DELETE ");

    switch(type)
    {
        case CLOUD_RES_TYPE_DEVICE:
            strcat(path, "/devices/");
            strcat(path, firstID);
            break;

        case CLOUD_RES_TYPE_DATASTREAM:
            strcat(path, "/devices/");
            strcat(path, firstID);
            strcat(path, "/datastreams/");
            strcat(path, secondID);
            break;

        case CLOUD_RES_TYPE_DATAPOINT:
            strcat(path, "/devices/");
            strcat(path, firstID);
            strcat(path, "/datapoints");
            break;

        case CLOUD_RES_TYPE_TRIGGER:
            strcat(path, "/triggers/");
            strcat(path, firstID);
            break;

        case CLOUD_RES_TYPE_APIKEY:
            strcat(path, "/keys/");
            strcat(path, firstID);
            break;

        case CLOUD_RES_TYPE_BINDATA:
            strcat(path,  "/bindata/");
            break;
        default:
            break;
    }
}
