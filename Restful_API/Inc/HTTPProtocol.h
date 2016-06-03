/******************************************************************************
* Copyright (c), 2012~2015 iot.10086.cn All Rights Reserved
* @file             HTTPProtocol.h 
* @brief          this file provide http protocol pack/unpack APIs
* @details       define common device manager structures and functions
* @author               DengXing 
* @date			2015/7/30
* @version		1.0.0
* @par Revision History:
*      Date			Author		Revised.Ver		Notes
*      2015/6/30	DengXing	1.0.0			file created
******************************************************************************/

#ifndef __HTTP_PROTOCOL_H__
#define __HTTP_PROTOCOL_H__

/*****************************************************************************/
/* Includes                                                                  */
/*****************************************************************************/
#include <osAdaption/osAdaption.h>

#ifdef _cplusplus
extern "C"{
#endif   

/*****************************************************************************/
/* External Definition（Constant and Macro )                                 */
/*****************************************************************************/
#define HTTP_HEADER_KEY 31
#define HTTP_HEADER_VALUE 63
#define URL_LEN 31
#define URLPARAM_LEN 127
#define URL_PATH_LEN 127
#define ERROR_STRING_LEN 127
#define BODY_LEN 1024
#define RESP_BODY_LEN 4096
#define CONTENT_LEN 2048
#define HTTP_HEADER_PAIR_NUM 10

///< http packet
#define HTTP_PACKET_GET_INSTANCE(pInstance)  \
            do { \
                    pInstance = (pHTTPPacketGenerator)osAdaptionMemoryAlloc(sizeof(httpPacketGenerator));    \
                    assert(pInstance);   \
                    memset(pInstance, 0, sizeof(httpPacketGenerator));	\
                    pInstance->constructor = httpConstructor; \
                } while (0)

#define HTTP_PACKET_DESTROTY_INSTANCE(pInstance)    do { pInstance->destructor(pInstance); SAFE_FREE(pInstance); } while(0)

///< http response parser
#define HTTP_RES_PARSER_GET_INSTANCE(pInstance)  \
            do { \
                    pInstance = (pHttpParser)osAdaptionMemoryAlloc(sizeof(httpParser));    \
                    assert(pInstance);   \
                    pInstance->constructor = httpParserConstructor; \
                } while (0)

#define HTTP_RES_PARSER_DESTROTY_INSTANCE(pInstance)    do { pInstance->destructor(pInstance); SAFE_FREE(pInstance); } while(0)

/*****************************************************************************/
/* External Structures, Enum and Typedefs                                    */
/*****************************************************************************/
typedef enum
{
    HTTP_METHOD_POST,
    HTTP_METHOD_PUT,
    HTTP_METHOD_GET,
    HTTP_METHOD_DELETE
} httpMethod;

typedef enum
{
    CLOUD_RES_TYPE_USER,
    CLOUD_RES_TYPE_DEVICE,
    CLOUD_RES_TYPE_DATASTREAM,
    CLOUD_RES_TYPE_DATAPOINT,
    CLOUD_RES_TYPE_TRIGGER, 
    CLOUD_RES_TYPE_APIKEY, 
    CLOUD_RES_TYPE_BINDATA,  
    CLOUD_RES_TYPE_CMD,
    CLOUD_RES_TYPE_APILOG,  
    CLOUD_RES_TYPE_HISTORYDATA
} cloudResType;

typedef struct
{
        int8 key[HTTP_HEADER_KEY+1];
        int8 value[HTTP_HEADER_VALUE+1];
} httpHeaderPair, *pHttpHeaderPair;

typedef struct
{
    int32  errorCode;
    int8    errorString[ERROR_STRING_LEN+1];
} httpResponseCode, *pHttpResponseCode;

typedef struct
{
    ///< constructor & destructor
    void (*constructor)(void* this, int8* firstID, int8* secondID);
    void (*destructor)(void* this);
    ///< method
    void (*setUrl)(void* this, int8* url);
    void (*setMethod)(void* this, httpMethod method); ///< refer to enum httpMethod
    void (*setResType)(void* this, cloudResType type);
    void (*addHeader)(void* this, int8* key, int8* value);
    void (*addBody)(void* this, int8* buf, uint32 bodyLen);
    void (*addUrlParam)(void* this, int8* paramStr);
    void (*packetGen)(void* this);
    int8* (*getContent)(void* this);
    ///< private data
    httpMethod method;
    cloudResType resType;
    int8*   firstID;
    int8*   secondID;
    int8    url[URL_LEN+1];
    int8    urlParam[URLPARAM_LEN+1];
    int8    path[URL_PATH_LEN+1];
    int8    body[BODY_LEN+1];
    uint32 bodyLen;
    int8    content[CONTENT_LEN+1];
    httpHeaderPair array[HTTP_HEADER_PAIR_NUM];
    struct httpPacketGenerator* pthis;
} httpPacketGenerator, *pHTTPPacketGenerator;

typedef struct
{
     ///< constructor & destructor
    void (*constructor)(void* this);
    void (*destructor)(void* this);
    ///< method
    void (*startParse)(void* this, int8* buf, int32 len);
    httpResponseCode (*getResponseCode)(void* this);
    int8* (*getContentLength)(void* this);
    int8* (*getContentType)(void* this);
    int8* (*getContentEncoding)(void* this);
    int8* (*getContentBody)(void* this);
    //private data
    httpResponseCode  responseCode;
    int8    body[RESP_BODY_LEN+1];
    httpHeaderPair array[HTTP_HEADER_PAIR_NUM];
    struct httpParser* pthis;
} httpParser, *pHttpParser;
/*****************************************************************************/
/* External Function Prototypes                                                   */
/*****************************************************************************/
extern void httpConstructor(void* this, int8* firstID, int8* secondID);
extern void httpParserConstructor(void* this);
#ifdef _cplusplus
}
#endif
#endif /*__HTTP_PROTOCOL_H__*/
