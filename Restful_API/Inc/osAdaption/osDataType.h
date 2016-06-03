/**
* Copyright (c), 2012~2015 iot.10086.cn All Rights Reserved
* @file         osDataType.h
* @brief        general data type definition . 
* @details      general data type definition .
* @author       DengXing 
* @date			2015/6/30 
* @version		1.0.0
* @par Revision History:
*      Date			Author		Revised.Ver		Notes\n
*      2015/6/30	DengXing	1.0.0			file created\n
*/
#ifndef __DATA_TYPE_H__
#define __DATA_TYPE_H__

/*****************************************************************************/
/* Includes                                                                  */
/*****************************************************************************/

#ifdef _cplusplus
extern "C"{
#endif
/*****************************************************************************/
/* External Definition£¨Constant and Macro )                                 */
/*****************************************************************************/
#define	true	1
#define false	0
/*****************************************************************************/
/* External Structures, Enum and Typedefs                                    */
/*****************************************************************************/

/**
 * @name 数据类型定义
 * @{ */
typedef unsigned char 		uint8;
typedef char				int8;
typedef unsigned short 	    uint16;
typedef short 			    int16;
typedef unsigned int 		uint32;
typedef int 				int32;
typedef unsigned char       boolValue;
/**  @} */

/*****************************************************************************/
/* External Function Prototypes                                              */
/*****************************************************************************/
#define DEBUGON
#ifdef DEBUGON
#define DEBUG(format,...)	do{printf("%s %d:"format"\n",__FILE__, __LINE__,  ##__VA_ARGS__ );}while(0)
#else
#define DEBUG(format,...)
#endif


#ifdef _cplusplus
}
#endif
#endif /*__DATA_TYPE_H__*/

