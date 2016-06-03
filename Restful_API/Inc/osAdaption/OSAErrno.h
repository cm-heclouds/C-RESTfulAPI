/**
 * Copyright (c), 2012~2016 iot.10086.cn All Rights Reserved
 * @file        OSAErrno.h  
 * @brief       系统错误码定义
 * @author      宋伟<songwei1@iot.chinamobile.com>
 * @date        2016/05/18
 * @version     1.0.0
 * @par Revision History:
 *      Date            Author      Revised.Ver     Notes\n
 *      2015/5/18       SongWei       1.0.0         file created\n
 */
#ifndef __OSAERRNO_H__ 
#define __OSAERRNO_H__

/*****************************************************************************/
/* Includes                                                                  */
/*****************************************************************************/

#ifdef _cplusplus
extern "C"{
#endif   

/*****************************************************************************/
/* External Definition（Constant and Macro )                                 */
/*****************************************************************************/
#define OSASUCCESS          0
#define OSAERR              -1
#define OSAERR_INVALID      -2
#define OSAERR_NOMEM        -3
#define OSAERR_DEVEXISTED   -4
#define OSAERR_DEVNOTFOUND  -5

#define OSAERR_SOCKETERR    -10
#define OSAERR_SOCKTIMEOUT  -11

#define OSAERR_NOTFOUND     -20

#define OSAERR_TRIGINVALID  -31
/*****************************************************************************/
/* External Structures, Enum and Typedefs                                    */
/*****************************************************************************/

/*****************************************************************************/
/* External Function Prototypes                                              */
/*****************************************************************************/


#ifdef _cplusplus
}
#endif   

#endif

