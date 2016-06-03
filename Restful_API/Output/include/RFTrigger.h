/**
 * Copyright (c), 2012~2016 iot.10086.cn All Rights Reserved
 * @file 	    RFTrigger.h	
 * @brief       触发器操作接口封装。触发器主要用于设置某种条件，数据流上传的数据点满足
 *              条件时，自动向指定url发起POST请求。
 * @author 		宋伟<songwei1@iot.chinamobile.com>
 * @date 		2016/05/24
 * @version 	1.0.0
 * @par Revision History:
 * 		Date			Author		Revised.Ver		Notes\n
 * 		2015/5/24		宋伟		  1.0.0			file created\n
 */
#ifndef __RFTRIGGER_H__
#define __RFTRIGGER_H__

/*****************************************************************************/
/* Includes                                                                  */
/*****************************************************************************/
#include <osAdaption/osDataType.h>

#ifdef _cplusplus
extern "C"{
#endif   


/*****************************************************************************/
/* External Definition（Constant and Macro )                                 */
/*****************************************************************************/
/** 触发器名称长度定义*/
#define TRIGGER_TITILE_LEN_MAX  31
/** 数据流唯一标识符长度定义*/
#define TRIGGER_DSUUIDLIST_LEN_MAX 127
/** 触发器发送POST消息的目的URL长度定义*/
#define TRIGGER_URL_LEN_MAX 127
/** 触发参数长度定义*/
#define TRIGGER_PARAM_LEN_MAX   255

/**
 * @name 触发类型定义
 * @{ */

/**
 * @name 5种关系运算型触发
 * @brief 触发参数为指定数值（double型）
 * @{ */
/** 小于*/
#define TRIGGER_TYPE_LT   0
/** 小于等于*/
#define TRIGGER_TYPE_LE   1
/** 大于*/
#define TRIGGER_TYPE_GT   2
/** 大于等于*/
#define TRIGGER_TYPE_GE   3
/** 等于*/
#define TRIGGER_TYPE_EQ   4
/**  @} */

/**
 * @name 区间触发
 * @brief 触发参数(两个double型)用于指定触发区间下限（第一个double）和上限（第二个double）
 * @{ */
/** 进入区间时触发*/
#define TRIGGER_TYPE_IN     5
/** 离开区间时触发*/
#define TRIGGER_TYPE_OUT    6
/** 上述两种情况都触发*/
#define TRIGGER_TYPE_INOUT  7
/**  @} */

/**
 * @name 条件指定触发 
 * @brief 触发参数为字符串类型的条件表达式。\n $val[0]表示第一个数据流，$val[1]为第二个，以此类推
 * 例如，第一个数据流上报数据点格式为：{“temperature”:22, “humidity”:56},若需设置温度大于30，
 * 且湿度小于33时触发告警，则threshold中条件表达式可设置为：\n
 * “$val[0][‘temperature’] > 30 && $val[0][‘humidity’]< 33 ”
 * @{ */
#define TRIGGER_TYPE_EXP        8
/**  @} */

/**
 * @name 数据变化触发
 * @brief 无需触发参数，当上传的值有变化时触发
 * @{ */
#define TRIGGER_TYPE_CHANGE     9
/**  @} */

/**
 * @name 活动状态变化触发
 * @{ */
/** 触发参数为数值（double型），指定多少秒内未上报数据触发告警，同时被监控对象进入frozen状态*/
#define TRIGGER_TYPE_FROZEN     10
/** 无需触发参数，被监控对象在frozen状态下收到上报的数据点时，触发告警*/
#define TRIGGER_TYPE_LIVE       11
/**  @} */

/**
 * @name 未知触发类型
 * @{ */
#define TRIGGER_TYPE_UNKNOWN    20
/**  @} */

/**  @} */


/*****************************************************************************/
/* External Structures, Enum and Typedefs                                    */
/*****************************************************************************/
/**
 * @brief 触发器配置信息结构定义
 */
typedef struct
{
    /** 触发器名称*/
    int8 title[TRIGGER_TITILE_LEN_MAX + 1];
    /** 触发器需要监控的数据流的UUID，支持多个，以逗号隔开*/
    int8 dataStreamUUIDList[TRIGGER_DSUUIDLIST_LEN_MAX + 1];
    /** 满足触发条件后的POST消息目的url*/
    int8 remoteURL[TRIGGER_URL_LEN_MAX + 1];
    /** 触发类型，参考TRIGGER_TYPE_XXX*/
    int32 triggerType;
	/** 触发参数，参考TRIGGER_TYPE_XXX*/
    int8 triggerParam[TRIGGER_PARAM_LEN_MAX + 1];
} RFTriggerConfig, *pRFTriggerConfig;

/*****************************************************************************/
/* External Function Prototypes                                              */
/*****************************************************************************/
/**
 * @brief 创建触发器
 *
 * @param config 触发器参数
 * @param triggerID 触发器创建成功后返回触发器在云端分配的ID
 *
 * @return  参考@ref OSAErrno.h
 */
int32 RFTrigger_Create(RFTriggerConfig *config, int8 *triggerID);

/**
 * @brief 更新触发器信息
 *
 * @param triggerID 指定需要更新的触发器ID
 * @param config 需要更新的触发器信息
 *
 * @return  参考@ref OSAErrno.h
 */
int32 RFTrigger_UpdateInfo(int8 *triggerID, RFTriggerConfig *config);

/**
 * @brief 查看触发器
 *
 * @param triggerID 指定需要查看的触发器ID
 * @param config 用于返回触发器信息
 *
 * @return  参考@ref OSAErrno.h
 */
int32 RFTrigger_View(int8 *triggerID, RFTriggerConfig *config);

/**
 * @brief 删除触发器
 *
 * @param triggerID 指定需要删除的触发器ID
 *
 * @return  参考@ref OSAErrno.h
 */
int32 RFTrigger_Delete(int8 *triggerID);


#ifdef _cplusplus
}
#endif   

#endif

