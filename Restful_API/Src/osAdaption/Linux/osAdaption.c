/******************************************************************************
* Copyright (c), 2012~2015 iot.10086.cn All Rights Reserved
* @file         osAdaption.c 
* @brief        os adaption layer function implementation
* @details      os adaption layer function implementation
* @author       DengXing, Ji Shanyang 
* @date			2015/7/4 
* @version		1.0.0
* @par Revision History:
*      Date			Author		Revised.Ver		Notes
*      2015/6/30	DengXing	1.0.0			file created
******************************************************************************/

/*****************************************************************************/
/* Includes                                                                  */
/*****************************************************************************/
#ifdef WIN32
#include "stdio.h"
#include "stdlib.h"
#endif

#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#if 0
#ifdef LITEOS
#include <tcp-socket.h>
#else
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/msg.h>
#include <sys/stat.h>
#include <arpa/inet.h>
#include <time.h>
#include <netdb.h>
#include <errno.h>
#include <signal.h>
#include <pthread.h>
#include <mqueue.h>
#endif
#endif
#include <osAdaption/osAdaption.h>

/*****************************************************************************/
/* Local Definitions （Constant and Macro )                                  */
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
#if 0
#ifdef LITEOS

#else	//linux
static osMulTimerManage_t* MulTimerManage=NULL;	
#endif

/*****************************************************************************/
/* External Functions and Variables                                              */
/*****************************************************************************/
extern int pthread_getattr_np(pthread_t thread, pthread_attr_t *attr);
/*****************************************************************************/
/* Function Implementation                                                         */
/*****************************************************************************/

#define OS_ADAPTION_MEMSORY_SECTION
#endif
/*
** Memory Section 
*/
void* osAdaptionMemoryAlloc(uint32 size)
{
    return malloc(size);
}

void osAdaptionMemoryFree(void* ptr)
{
    free(ptr);
}
#if 0
#define OS_ADAPTION_TIMER_SECTION
/*
** Timer Section 
*/
#ifndef LITEOS

static void osAdaptionTimerSigFunc(int32 signo){
	osMulTimerEntry_t* timerPtr = MulTimerManage->etrHead;
	osMulTimerEntry_t* tmpPtr=NULL;
	while(timerPtr != NULL){
		if(timerPtr->isUse == 1){
			timerPtr->elapse++;
			if(timerPtr->interval == timerPtr->elapse){
				timerPtr->elapse = 0;
				if(timerPtr->timerType == OS_TIMER_ONCE){
					timerPtr->isUse = 0;
				}
				//timerPtr->timerProc(timerPtr->arg);
				tmpPtr = timerPtr;	///<加tmpPtr后可安全地在定时器回调函数中删除定时器
				timerPtr = timerPtr->next;
				tmpPtr->timerProc(tmpPtr->arg);
				continue;
			}
		}
		timerPtr = timerPtr->next;
	}
}

static osMulTimerManage_t* osAdaptionInitMulTimer(void){
	struct itimerval val;

	MulTimerManage = (osMulTimerManage_t*)malloc(sizeof(osMulTimerManage_t));
	if(MulTimerManage == NULL){
		return NULL;
	}
	memset(MulTimerManage, 0, sizeof(osMulTimerManage_t));
	
	if(signal(SIGALRM, osAdaptionTimerSigFunc) == SIG_ERR){
		free(MulTimerManage);
		MulTimerManage = NULL;
		return NULL;
	}

	MulTimerManage->etrHead = NULL;
	MulTimerManage->timerID = 0;
	val.it_interval.tv_sec = 0;
	val.it_interval.tv_usec = 1000;
	val.it_value.tv_sec = 0;
	val.it_value.tv_usec = 1000;
	if(setitimer(ITIMER_REAL, &val, NULL)){
		free(MulTimerManage);
		MulTimerManage = NULL;
		return NULL;
	}
	pthread_mutex_init(&MulTimerManage->mutex, NULL);
	return MulTimerManage;
}


static osTimerID osAdaptionAddMulTimer(osMulTimerManage_t* timerManage, timerCB timerIsr, osTimerType type, void* arg){
	osMulTimerEntry_t* timerEntry=NULL;
	osMulTimerEntry_t* ptr=NULL;
	if(timerManage == NULL){
		return -1;	///<error
	}

	timerEntry = (osMulTimerEntry_t*)malloc(sizeof(osMulTimerEntry_t));
	if(timerEntry == NULL){
		return -1;
	}
	///<initial timer entry
	memset(timerEntry, 0, sizeof(osMulTimerEntry_t));
	timerEntry->isUse = 0;
	timerEntry->timerType = type;
	timerEntry->timerID = timerManage->timerID;
	timerManage->timerID++;
	timerEntry->interval = 0;
	timerEntry->elapse = 0;
	timerEntry->timerProc = timerIsr;
	timerEntry->arg = arg;
	timerEntry->next = NULL;

	///<add timer entry into timer manage
	pthread_mutex_lock(&timerManage->mutex);
	if(timerManage->etrHead == NULL){
		timerManage->etrHead = timerEntry;
	}
	else{
		ptr = timerManage->etrHead;
		while(ptr->next != NULL){
			ptr = ptr->next;
		}
		ptr->next = timerEntry;
	}
	pthread_mutex_unlock(&timerManage->mutex);
	return timerEntry->timerID;
}


static osMulTimerEntry_t* osAdaptionGetMulTimer(osMulTimerManage_t* timerManage, osTimerID id){
	if(timerManage==NULL || id<0){
		return NULL;
	}
	osMulTimerEntry_t* ptr = timerManage->etrHead;
	while(ptr != NULL){
		if(ptr->timerID == id){
			return ptr;
		}
		ptr = ptr->next;
	}
	return ptr;
}
/*
static void osStartMulTimer(osMulTimerEntry_t* timerEntry, uint32 milliSec){
	timerEntry->interval = milliSec;
	timerEntry->elapse = 0;
	timerEntry->isUse = 1;
}
static void osStopMulTimer(osMulTimerEntry_t* timerEntry){
	timerEntry->isUse = 0;
}
*/

#define osAdaptionStartMulTimer(timerEntry, milliSec)  \
				do{(timerEntry)->interval=(milliSec); \
					(timerEntry)->elapse=0; \
					(timerEntry)->isUse=1;} while(0)
												
#define osAdaptionStopMulTimer(timerEntry)  \
				do{(timerEntry)->isUse=0;} while(0)

static int32 osAdaptionDelMulTimer(osMulTimerManage_t* timerManage, osMulTimerEntry_t* timerEntry){
	osMulTimerEntry_t* ptr=NULL;
	if(NULL==timerEntry || NULL==timerManage){
		return -1;	///<error
	}
	pthread_mutex_lock(&timerManage->mutex);
	if(timerEntry == timerManage->etrHead){
		timerManage->etrHead = timerEntry->next;
		memset(timerEntry, 0, sizeof(osMulTimerEntry_t));
		free(timerEntry);
		pthread_mutex_unlock(&timerManage->mutex);
		return 0;	///<success
	}
	
	ptr = timerManage->etrHead;
	while(ptr->next != NULL){
		if(ptr->next == timerEntry){
			break;
		}
		ptr = ptr->next;
	}
	if(NULL == ptr->next){
		pthread_mutex_unlock(&timerManage->mutex);
		return -1;	///<error
	}
	ptr->next = timerEntry->next;
	free(timerEntry);
	pthread_mutex_unlock(&timerManage->mutex);
	return 0;
}

#endif

extern osTimerID osAdaptionCreateTimer(timerCB timerIsr, osTimerType type, void* arg)
{
#ifdef LITEOS

#else	///<linux
	osTimerID id=0;
	if(MulTimerManage==NULL){
		if(osAdaptionInitMulTimer()==NULL){
			return -1;	///<error
		}
	}
	id = osAdaptionAddMulTimer(MulTimerManage,timerIsr,type,arg);
	return id;
#endif

}

extern void osAdaptionStartTimer(osTimerID id, uint32 milliSec)
{
#ifdef LITEOS

#else
	osMulTimerEntry_t* timerEntry = NULL;
	timerEntry = osAdaptionGetMulTimer(MulTimerManage, id);
	if(timerEntry == NULL){
		return;	///<error
	}
	osAdaptionStartMulTimer(timerEntry,milliSec);

#endif
}

extern void osAdaptionStopTimer(osTimerID id)
{
#ifdef LITEOS
	
#else
	osMulTimerEntry_t* timerEntry = NULL;
	timerEntry = osAdaptionGetMulTimer(MulTimerManage, id);
	if(timerEntry == NULL){
		return; ///<error
	}
	osAdaptionStopMulTimer(timerEntry);
	
#endif
}

extern void osAdaptionDeleteTimer(osTimerID id)
{
#ifdef LITEOS
		
#else
	osMulTimerEntry_t* timerEntry = NULL;
	timerEntry = osAdaptionGetMulTimer(MulTimerManage, id);
	if(timerEntry == NULL){
		return; ///<error
	}
	osAdaptionDelMulTimer(MulTimerManage, timerEntry);
		
#endif
}


#define OS_ADAPTION_FILE_SYSTEM_SECTION
/*
** File System Section 
*/

/*******************************************************************************
function: osAdaptionFileOpen
	parameter:
	return:
           	It return the new file descriptor.
           	returns -1, there has been an error in the open
*******************************************************************************/
osFileHandle osAdaptionFileOpen(int8* filePath, int32 flag)
{
#ifdef LIETOS

#else
	int32 oflag = 0;
	int32 rev = 0;
	if(flag & OS_FILE_READ_ONLY){	///read only
		oflag |= O_RDONLY;
	}
	if(flag & OS_FILE_WRITE_ONLY){	///write only
		oflag |= O_WRONLY;
	}
	if(flag & OS_FILE_READ_WRITE){	///read and write
		oflag |= O_RDWR;
	}
	if(flag & OS_FILE_APPEND){		///The file is opened in append mode
		oflag |= O_APPEND;
	}
	if(flag & OS_FILE_CREATE){		
		oflag |= O_CREAT;
	}
	if(flag & OS_FILE_TRUNCATE){	
		oflag |= O_TRUNC;
	}

	if(flag & OS_FILE_CREATE)
		///<If the file does not exist it will be created, and user has read & write permission
		rev = open(filePath, oflag, S_IRUSR|S_IWUSR); 
	else
		rev = open(filePath, oflag);

	if(rev==-1){
		if(errno==EACCES){
			return OS_FILE_RESULT_NOT_EXIST;
		}
		return OS_FILE_RESULT_ERROR;
	}
	return rev;

#endif
}
/*******************************************************************************
function: osAdaptionFileRead
	parameter:
	return:
           	It returns the number of bytes actually read.
           	returns 0, it had nothing to read; it reached the end of the file.
           	returns -1, there has been an error in the read
*******************************************************************************/
int32 osAdaptionFileRead(osFileHandle handle, void* buf, uint32 nReads)
{
#ifdef LIETOS

#else
	int32 rev = 0;

	rev = read(handle, buf, nReads);

	if(-1 == rev){
		if(errno == EBADF){
			return OS_FILE_RESULT_PARA_ERROR;
		}
		return OS_FILE_RESULT_ERROR;
	}
	return rev;
	
#endif
}

/*******************************************************************************
function: osAdaptionFileWrite
	parameter:
	return:
           	It returns the number of bytes actually written.
           	returns 0, it means no data was written.
           	returns -1, there has been an error in the write
*******************************************************************************/
int32 osAdaptionFileWrite(osFileHandle handle, void* buf, uint32 nWrites)
{
#ifdef LIETOS

#else
	int32 rev = 0;
	rev = write(handle, buf, nWrites);

	if(-1 == rev){
		if(errno == EBADF){
			return OS_FILE_RESULT_PARA_ERROR;
		}
		return OS_FILE_RESULT_ERROR;
	}
	return rev;
#endif
}

/*******************************************************************************
function: osAdaptionFileWrite
	parameter:
	return:
		returns the resulting offset location as measured in bytes from the beginning of the file. 
		On error, the value (off_t) -1 is returned 
*******************************************************************************/
int32 osAdaptionFileSeek(osFileHandle handle, uint32 offset, int32 whence)
{
#ifdef LITEOS

#else
	int32 rev = 0;
	int32 wflag = 0;
	if(whence & OS_FILE_SEEK_SET){
		wflag = SEEK_SET;
	}
	else if(whence & OS_FILE_SEEK_CUR){
		wflag = SEEK_CUR;
	}
	else if(whence & OS_FILE_SEEK_END){
		wflag = SEEK_END;
	}
	rev = lseek(handle, offset, wflag);
	
	if(-1 == rev){
		if(errno == EBADF){
			return OS_FILE_RESULT_PARA_ERROR;
		}
		return OS_FILE_RESULT_ERROR;
	}
	return rev;
#endif
}
/*******************************************************************************
function: osAdaptionFileClose
	parameter:
	return:
			It returns 0 if successful and -1 on error.
*******************************************************************************/
int32 osAdaptionFileClose(osFileHandle handle)
{
#ifdef LIETOS

#else
	int32 rev = 0;
	rev = close(handle);
	if(-1 == rev){
		if(errno == EBADF){
			return OS_FILE_RESULT_PARA_ERROR;
		}
		return OS_FILE_RESULT_ERROR;
	}
	return OS_FILE_RESULT_OK;

#endif
}

int32 osAdaptionFileDelete(int8* filePath)
{
#ifdef LITEOS

#else
	int32 rev = 0;
	rev = remove(filePath);
	if(-1 == rev){
		if(errno == EBADF){
			return OS_FILE_RESULT_PARA_ERROR;
		}
		return OS_FILE_RESULT_ERROR;
	}
	return OS_FILE_RESULT_OK;
#endif
}

int32 osAdaptionAccess(const int8* filePath, int32 mode){

#ifdef LITEOS

#else
	int32 tmpmode = 0;
	if(mode&OS_FILE_ACCESS_MODE_F){tmpmode |= F_OK;}
	if(mode&OS_FILE_ACCESS_MODE_R){tmpmode |= R_OK;}
	if(mode&OS_FILE_ACCESS_MODE_W){tmpmode |= W_OK;}
	if(mode&OS_FILE_ACCESS_MODE_X){tmpmode |= X_OK;}
	return access(filePath, tmpmode);
#endif

}



#define OS_ADAPTION_THREAD_SECTION
/*
** Thread Management Section 
*/
osThreadHandle osAdaptionThreadCreate(const osThreadDef *threadDef, void *arg)
{
    pthread_t *pthread = NULL;
    pthread_attr_t attr;
    osThreadHandle handle = -1;

    if ( threadDef != NULL )
    {
        pthread = (pthread_t*)malloc(sizeof(pthread_t));
        if (pthread !=NULL )
        {
            pthread_attr_init(&attr);
            pthread_attr_setscope(&attr, PTHREAD_SCOPE_SYSTEM);
            pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
            pthread_attr_setstacksize(&attr, threadDef->stackSize);
            if (pthread_create(pthread, NULL, threadDef->funcName , arg) == 0)
            {
                handle = (osThreadHandle)pthread;
            }

            pthread_attr_destroy(&attr);
        }
    }
	
    return handle;
}


///<add : 2015/08/06  Ji Shanyang
///<description : sends a cancellation request to the specified thread.

osStatusCode osAdaptionThreadSetCancelState(void){
#ifdef LITEOS
	return OS_STATUS_OK;
#else
	pthread_setcancelstate(PTHREAD_CANCEL_ENABLE,NULL); 
	return OS_STATUS_OK;
#endif
}
osStatusCode osAdaptionThreadSetCancelType(void){
#ifdef LITEOS
	return OS_STATUS_OK;
#else
	pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
	return OS_STATUS_OK;
#endif
}

osStatusCode osAdaptionThreadCancel(osThreadHandle handle){
#ifdef LITEOS
	
#else

	pthread_t* thread = (pthread_t*)handle;
	if(0==pthread_cancel(*thread))
		return OS_STATUS_OK;
	else
		return OS_STATUS_ERROR;
		
#endif
}
///<add : 2015/08/06  Ji Shanyang
///<description : wait for the thread identified by handle to terminate
osStatusCode osAdaptionThreadJoin(osThreadHandle handle){
#ifdef LITEOS
		
#else
	
	pthread_t* thread = (pthread_t*)handle;
	if(0==pthread_join(*thread, NULL))
		return OS_STATUS_OK;
	else
		return OS_STATUS_ERROR;
		
#endif
}


osThreadHandle osAdaptionThreadGetHandle(void)
{
    osThreadHandle handle = 0;
    pthread_t *pthread = NULL;
    
    *pthread = pthread_self();
    handle = (osThreadHandle)pthread;

    return handle;
}

osStatusCode osAdaptionThreadTerminate(osThreadHandle handle)
{
    pthread_t *pthread = (pthread_t*)handle;
    
    SAFE_FREE(pthread);
    pthread_exit(0);

    return OS_STATUS_OK;
}


///<add : 2015/08/06  Ji Shanyang
///<description : free the memory allocate by osAdaptionThreadCreate function
osStatusCode osAdaptionThreadHandleFree(osThreadHandle handle){
	pthread_t *pthread = (pthread_t*)handle;
    
	SAFE_FREE(pthread);
	return OS_STATUS_OK;
}


osStatusCode osAdaptionThreadSetPriority(osThreadHandle handle, osThreadPriority priority)
{
    pthread_attr_t attr;
    struct sched_param param;
    osStatusCode ret = OS_STATUS_ERROR;
    
    if (pthread_getattr_np((*(pthread_t*)handle), &attr))
    {
        switch (priority)
        {
            case OS_THREAD_PRIORITY_LOW:
                param.sched_priority = 0;
                break;
            case OS_THREAD_PRIORITY_MEDIUM:
                param.sched_priority = 30;
                break;
            case OS_THREAD_PRIORITY_HIGH:
                param.sched_priority = 60;
                break;
            default:
                break;
        }
        pthread_attr_setschedparam(&attr, &param);
        
        ret = OS_STATUS_OK;
    }

    return ret;
}

osThreadPriority osAdaptionThreadGetPriority(osThreadHandle handle)
{
    pthread_attr_t attr;
    struct sched_param param;
    osThreadPriority priority = OS_THREAD_PRIORITY_LOW;
    
    pthread_getattr_np((*(pthread_t*)handle), &attr);
    pthread_attr_getschedparam(&attr, &param);

    if ((param.sched_priority - 60)>= 0)
    {
        priority = OS_THREAD_PRIORITY_HIGH;
    }
    else if ((param.sched_priority - 30)>= 0)
    {
        priority = OS_THREAD_PRIORITY_MEDIUM;
    }

    pthread_attr_destroy(&attr);
    return priority;
}

osStatusCode osAdaptionThreadYield(void)
{
    sched_yield();   

    return OS_STATUS_OK;
}

#define OS_ADAPTION_MUTEX_SECTION
/*
** Mutex Section 
*/
osMutexID osAdaptionMutexCreate(const osMutexDef *mutexDef)
{
    pthread_mutex_t * pmutex = NULL;
    osMutexID id = 0;
    
    if (mutexDef != NULL)
    {
        pmutex = (pthread_mutex_t*)malloc(sizeof(pthread_mutex_t));
        if (pmutex != NULL)
        {
            if (pthread_mutex_init(pmutex, NULL) != 0) ///< init error code 大于0， 返回0 代表 ok
            {
                id = -1;
            }
            else
            {
                id = (osMutexID)pmutex;
            }
        }
    }
    
    return id;
}

osStatusCode osAdaptionMutexLock(osMutexID id, uint32 millisec)
{
    osStatusCode ret = OS_STATUS_OK;
    pthread_mutex_t * pmutex = ( pthread_mutex_t *)id;

    if (pthread_mutex_lock(pmutex) != 0)
    {
        ret = OS_STATUS_ERROR;
    }
    
    return ret;
}

osStatusCode osAdaptionMutexUnlock(osMutexID id)
{
    osStatusCode ret = OS_STATUS_OK;
    pthread_mutex_t * pmutex = ( pthread_mutex_t *)id;

    if (pthread_mutex_unlock(pmutex) != 0)
    {
        ret = OS_STATUS_ERROR;
    }
    
    return ret;
}

osStatusCode osAdaptionMutexDelete(osMutexID id)
{
    osStatusCode ret = OS_STATUS_OK;
    pthread_mutex_t * pmutex = ( pthread_mutex_t *)id;

    if (pthread_mutex_destroy(pmutex) != 0)
    {
        SAFE_FREE(pmutex);
    }
    else
    {
        ret = OS_STATUS_ERROR;
    }
    
    return ret;
}

#define OS_ADAPTION_MSG_QUEUE_SECTION
/*
** Message Queue Section 
*/
osMsgQueueID osAdaptionMsgQueueCreate(const osMsgQueueDef *queueDef, osThreadHandle handle)
{
    int8 qname[16] = {'/'};
    mqd_t mqID = 0;
    osMsgQueueID id = -1;
     ///< Linux default setting  modify from /proc/sys/fs/mqueue# cat msg_max  /proc/sys/fs/mqueue# cat msgsize_max
    struct mq_attr attr = {0, 10, 8192, 0};
    
    if (queueDef != NULL)
    {
        attr.mq_maxmsg = queueDef->queueSize; 
        attr.mq_msgsize = queueDef->itemSize;
        strncat(qname, queueDef->name, sizeof(qname)-2);
        ///< 成功返回0，出错返回-1, 
        mqID = mq_open(qname,  O_RDWR |O_CREAT, 0666, &attr );

        id = (osMsgQueueID)mqID;
       
    }
    return id;
}

osStatusCode osAdaptionMsgSend(osMsgQueueID id, osMsgEvent msg, uint32 millisec)
{
    mqd_t mqID = (mqd_t)id;
    osStatusCode ret = OS_STATUS_OK;

    ///< 成功返回0，出错返回-1, priority 数值越大，优先级越高, default 为0 不需要设定优先级
    if (mq_send(mqID, (const char*)&msg, sizeof(msg), 0) == -1)
    {
        ret = OS_STATUS_ERROR;
    }
    
    return ret;
}

osMsgEvent osAdaptionMsgReceive(osMsgQueueID id, uint32 millisec)
{
    mqd_t mqID = (mqd_t)id;
    osMsgEvent msg;
    
    ///<成功返回接收到消息的字节数，出错返回-1
    if (mq_receive(mqID, (char*)&msg, sizeof(msg), NULL) != -1)
    {
        msg.msgID = -1;
        msg.param = NULL;
    }
    return msg;
}


#define OS_ADAPTION_SOCKET_SECTION
/*
** TCP Socket Section
*/

/*******************************************************************************
function: osHtons
	16-bit数据主机字节序到网络字节序的转换
	in parameter:
		host_uint16 		:	16-bit主机字节序数据
	return:
			16-bit网络字节序数据
*******************************************************************************/
uint16 osAdaptionHtons(uint16 host_uint16)
{
#ifdef LITEOS


#else	//<linux
	return htons(host_uint16);
#endif
}

/*******************************************************************************
function: osHtonl
	32-bit数据主机字节序到网络字节序的转换
	in parameter:
		host_uint16			:	32-bit主机字节序数据
	return:
           	32-bit网络字节序数据
*******************************************************************************/
uint32 osAdaptionHtonl(uint32 host_uint32)
{
#ifdef LITEOS


#else	//linux
	return htonl(host_uint32);
#endif
}


/*******************************************************************************
function: osNtohs
	16-bit数据网络字节序到主机字节序的转换
	in parameter:
		host_uint16			:	16-bit网络字节序数据
	return:
           	16-bit主机字节序数据
*******************************************************************************/
uint16 osAdaptionNtohs(uint16 net_uint16)
{
#ifdef LITEOS
	
	
#else	///<linux
	return ntohs(net_uint16);
#endif
}


/*******************************************************************************
function: osNtohl
	32-bit数据网络字节序到主机字节序的转换
	in parameter:
		host_uint16			:	32-bit网络字节序数据
	return:
           	32-bit主机字节序数据
*******************************************************************************/
uint32 osAdaptionNtohl(uint32 net_uint32)
{
#ifdef LITEOS
	
	
#else	///<linux
	return ntohl(net_uint32);
#endif
}


#ifdef LITEOS
///<liteos TCP data callback function.
static int32 losDataCallback(struct tcp_socket *s, void *ptr, const uint8 *input_data_ptr, int32 input_data_len)
{

}

///<liteos TCP event callback function.
static void losEventCallback(struct tcp_socket *s, void *ptr, tcp_socket_event_t event)
{
	osTcpSocket_t* skt = (osTcpSocket_t*)ptr;
	if(TCP_SOCKET_CLOSED == event){
		skt->TcpSktState |= LOS_TCPSKT_CLOSED;
	}
	else if(TCP_SOCKET_TIMEDOUT == event){
		skt->TcpSktState |= LOS_TCPSKT_TIMEOUT;
	}
	else if(TCP_SOCKET_ABORTED == event){
		skt->TcpSktState |= LOS_TCPSKT_ABORTED;
	}
}
#endif

/*******************************************************************************
function: osIpAddrNtop
	二进制形式IP地址到字符串形式IP地址转换
	in parameter:
		src					:	二进制形式地址
		dest				:	字符形式地址
		bufSize				:	dest所指向的buffer大小
	return:
           	success				:	指向dest的指针
           	failed				:	NULL
*******************************************************************************/
int8* osAdaptionIpAddrNtop(const osIpAddr_t* src, int8* dest, uint16 bufSize){
	int8 strAddr[16]={0};
	uint8 strAddrLen = 0;
	sprintf(strAddr, "%d.%d.%d.%d", (*src)[0],(*src)[1],(*src)[2],(*src)[3]);
	strAddrLen = strlen(strAddr);
	if(strAddrLen >= bufSize){
		return NULL;
	}
	else{
		return(strcpy(dest, strAddr));
	}

}


/*******************************************************************************
function: osIpAddrNtop
	字符串形式IP地址到二进制形式IP地址转换
	in parameter:
		src					:	字符形式地址
		dest				:	二进制形式地址
	return:
           	success				:	0
           	failed				:	-1
*******************************************************************************/
int16 osAdaptionIpAddrPton(const int8* src, osIpAddr_t* dest){
	const int8* ptr = src;
	uint8 count = 0;
	(*dest)[0] = 0;
	(*dest)[1] = 0;
	(*dest)[2] = 0;
	(*dest)[3] = 0;
	while(*ptr != '\0'){
		if(*ptr >= '0' && *ptr <= '9'){
			(*dest)[count] = ((*dest)[count])*10 + (*ptr - '0');
			if((*dest)[count] > 255){
				return -1;
			}
		}
		else if(*ptr == '.'){
			count++;
			if(count > 3){
				return -1;
			}
		}
		else{
			return -1; ///<error
		}
		ptr++;
	}
	return 0;
}


int8* osAdaptionGetLocalTime(void){
	static int8 tmBuf[20];
	time_t now;
	struct tm *tmlocal;
	time(&now);
	tmlocal = localtime(&now);
	sprintf(tmBuf,"%04d-%02d-%02d %02d:%02d:%02d", 
		tmlocal->tm_year+1900, tmlocal->tm_mon+1, tmlocal->tm_mday, 
		tmlocal->tm_hour, tmlocal->tm_min, tmlocal->tm_sec);
	return tmBuf;
}

#endif

