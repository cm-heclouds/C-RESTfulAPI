/******************************************************************************
 * Copyright (c), 2012~2016 iot.10086.cn All Rights Reserved
 * @file        OSASocket.c 
 * @brief       网络Socket接口封装
 * @author      宋伟<songwei1@iot.chinamobile.com>
 * @date        2016/05/19
 * @version     1.0.0
 * @par Revision History:
 *      Date            Author      Revised.Ver     Notes
 *      2015/5/19       宋伟          1.0.0         file created
 ******************************************************************************/

/*****************************************************************************/
/* Includes                                                                  */
/*****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/time.h>
#include <unistd.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <errno.h>

#include <osAdaption/OSAErrno.h>
#include <osAdaption/OSASocket.h>

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
 * @brief 转换主机地址
 *
 * @Param :hostName 主机名，支持域名和点分十进制IP形式
 * @Param :port 主机端口
 * @Param :addr 返回转换后的主机地址结构
 *
 * @Returns : 参见OSAErrno.h
 *************************************************************************/
static int32 GetHostAddrByName(const int8 *hostName, uint16 port, struct sockaddr_in *addr)
{
    struct hostent *h = NULL;

    h = gethostbyname(hostName);
    if(NULL == h)
        return OSAERR_NOTFOUND;

    addr->sin_family = AF_INET;
    addr->sin_addr = *((struct in_addr*)h->h_addr);
    addr->sin_port = htons(port);
    
    return OSASUCCESS;
}

/*************************************************************************
 * @brief 创建系统网络socket
 *
 * @Param : socketType 指定Socket类型
 *
 * @Returns : 成功 - 返回socket号
 *            失败 - 返回错误码，参见OSAErrno.h
 *************************************************************************/
int32 OSASocket_Create(OSASocketType socketType)
{
    uint32 type = SOCK_STREAM;
    int32 fd = 0;
    int32 flags = 0;

    if(SOCKET_TYPE_UDP == socketType)
        type = SOCK_DGRAM;

    fd = socket(AF_INET, type, 0);
    
    if(0 <= fd)
    {
        /* 设置Socket为非阻塞模式*/
        flags = fcntl(fd, F_GETFL, 0);
        if(flags < 0)
        {
            close(fd);
            return OSAERR;
        }
        fcntl(fd, F_SETFL, flags|O_NONBLOCK);
    }

    return fd;
}


/*************************************************************************
 * @brief 连接指定主机端口，UDP和TCP均适用
 *
 * @Param :socket 本地socket号
 * @Param :host 需要连接的主机名，可为域名或点分十进制IP地址
 * @Param :port 需要连接的主机端口
 *
 * @Returns : 参见OSAErrno.h
 *************************************************************************/
int32 OSASocket_Connect(int32 socket, int8 *host, uint16 port)
{
    int32 ret = OSASUCCESS;
    struct sockaddr_in hostAddr;

    /* 先转换地址*/
    memset((void *)(&hostAddr), 0, sizeof(struct sockaddr_in));
    ret = GetHostAddrByName((const int8 *)host, port, &hostAddr);
    if(OSASUCCESS != ret)
        return ret;

    ret = connect(socket, (struct sockaddr *)(&hostAddr), sizeof(struct sockaddr));
    if(OSASUCCESS != ret)
        return OSAERR;

    return ret;
}

/*************************************************************************
 * @brief 通过socket发送数据，目标地址由OSASocket_Connect函数指定
 *
 * @Param :socket 本地socket号
 * @Param :sendBuf 需要发送的数据缓冲区
 * @Param :sendLen 需要发送的数据长度
 * @Param :timeout 发送超时时间，单位ms 
 *
 * @Returns : 成功 - 返回成功发送的数据长度
 *            失败 - 参见OSAErrno.h
 *************************************************************************/
int32 OSASocket_Send(uint32 socket, uint8 *sendBuf, uint16 sendLen, uint32 timeout)
{
    int32 ret = 0;
    /* 剩余timeout时间，单位us*/
    int32 timeoutRemain = timeout * 1000;
    uint32 dataLeft = sendLen;
    uint32 sendDataLen = 0;
    fd_set fs;
    struct timeval tv;
    struct timeval end;
    struct timeval curr;

    /* 计算超时的绝对时间*/
    gettimeofday(&end, NULL);
    end.tv_usec += (timeout * 1000);
    end.tv_sec += end.tv_usec / 1000000;
    end.tv_usec %= 1000000;

    /* 每次select超时时间为总剩余超时时间的1/2，当总剩余时间小于1ms则退出，不再轮询*/
    tv.tv_sec = (timeoutRemain / 2) / 1000000;
    tv.tv_usec = (timeoutRemain / 2) % 1000000;
    while(dataLeft)
    {
        FD_ZERO(&fs);
        FD_SET(socket, &fs);

        ret = select(socket + 1, NULL, &fs, NULL, &tv);
        if(-1 == ret)
        {
            return OSAERR;
        }
        else if(ret)
        {
            if(FD_ISSET(socket, &fs))
            {
                /* 发送数据*/
                sendDataLen = send(socket, (sendBuf + sendLen - dataLeft), dataLeft, 0);
                if(sendDataLen >= 0)
                {
                    dataLeft -= sendDataLen;
                }
                else if(EAGAIN != errno)
                {
                    return OSAERR;
                }
            }
        }

        /* 计算是否超时并确认下一次超时时间*/
        gettimeofday(&curr, NULL);
        timeoutRemain = ((end.tv_sec - curr.tv_sec) * 1000000 + (end.tv_usec - curr.tv_usec));
        if(timeoutRemain < 1000)
            break;
        else
        {
            tv.tv_sec = (timeoutRemain / 2) / 1000000;
            tv.tv_usec = (timeoutRemain / 2) % 1000000;
        }
    }

    return (sendLen - dataLeft);
}

/*************************************************************************
 * @brief 通过socket接收数据，来源地址由OSASocket_Connect函数指定
 *
 * @Param :socket 本地socket号
 * @Param :recvBuf 用于接收的数据缓冲区首地址
 * @Param :recvBufLen 数据接收缓冲区长度
 * @Param :timeout 接收超时时间，单位ms
 *
 * @Returns : 成功 - 返回成功接收的数据长度
 *            失败 - 参见OSAErrno.h
 *************************************************************************/
int32 OSASocket_Recv(uint32 socket, uint8 *recvBuf, uint16 recvBufLen, uint32 timeout)
{
    int32 ret = 0;
    fd_set fs;
    struct timeval tv;

    tv.tv_sec = (timeout / 1000);
    tv.tv_usec = (timeout % 1000) * 1000;
    FD_ZERO(&fs);
    FD_SET(socket, &fs);

    ret = select(socket + 1, &fs, NULL, NULL, &tv);
    if(-1 == ret)
    {
        return OSAERR;
    }
    else if(ret)
    {
        if(FD_ISSET(socket, &fs))
        {
            /* 接收数据*/
            ret = recv(socket, recvBuf, recvBufLen, 0);
        }
        else
            return OSAERR;
    }
    else
        ret = OSAERR_SOCKTIMEOUT;

    return ret;
}

/*************************************************************************
 * @brief 关闭指定socket
 *
 * @Param :socket 需要关闭的socket号
 *
 * @Returns : 参见OSAErrno.h
 *************************************************************************/
int32 OSASocket_Close(uint32 socket)
{
    int32 ret = OSASUCCESS;

    ret = close(socket);
    if(ret)
        ret = OSAERR_SOCKETERR;

    return ret;
}

