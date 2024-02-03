/*
****************************************************************************
PROJECT : VLIB
============================================================================ 
DESCRIPTION
Predefined data types
============================================================================
                            C O P Y R I G H T                                    
============================================================================
                           Copyright (c) 2012 - 2019
                                  by 
                       Renesas Electronics (Europe) GmbH. 
                           Arcadiastrasse 10
                          D-40472 Duesseldorf
                               Germany
                          All rights reserved.
============================================================================

DISCLAIMER

LICENSEE has read, understood and accepted the terms and conditions defined in
the license agreement, especially the usage rights. In any case, it is
LICENSEE's responsibility to make sure that any user of the software complies
with the terms and conditions of the signed license agreement.

****************************************************************************
*/

#ifndef TYPEDEF_H_
#define TYPEDEF_H_

/*******************************************************************************
*
*                   Defines
*
************************************************************
*/
#pragma ghs startnomisra  /* PRQA S 3116 */

#define R_STR(s)           # s
#define R_XSTR(s)          R_STR(s)
#define R_DOT(a,b)         a ## . ## b
#define R_XDOT(a,b)        R_DOT(a,b)
        
#define R_SPACE            " "
#define R_VERSIONPREFIX    "@(#)Version "    /* PRQA S 286 */
#define R_VERSIONPOSTFIX   " - "

#define R_VERSION(macro, hi, lo)  { "@(#)" # macro " driver version "  R_XSTR(hi) "."  R_XSTR(lo)}  /* PRQA S 286 */

#if defined (R_DBG_PRINT_MSG)
#ifndef R_DBG_MSG_LEVEL
    #define R_DBG_MSG_LEVEL 0
#endif

#define R_HALT_ON_ERROR(txt)    {R_DBG_PRINT(R_DBG_MSG_ERR, txt); for ( ; ; ){}}
#define R_DISPLAY_ERROR(txt)     R_DBG_PRINT(R_DBG_MSG_ERR, txt)
#define R_LOG(...)              printf(__VA_ARGS__)  /* PRQA S 1030 */

#else
#define R_HALT_ON_ERROR(txt)    {for ( ; ; ){}}
#define R_DISPLAY_ERROR(txt)    
#define R_LOG(...)              /* PRQA S 1030  */
#endif

#define R_INLINE inline


#pragma ghs endnomisra /* PRQA S 3116  */



/*******************************************************************************
*
*                   Enumerators 
*
************************************************************
*/

typedef enum
{
  R_ERR_OK = 0u,
  R_NG,
  R_PARAMETER_MISSING,
  R_PARAMETER_RANGE,
  R_PARAMETER_INCORRECT,
  R_DATA_MISMATCH
} r_Error_t;


/*******************************************************************************
*
*                   Types
*
************************************************************
*/

/* Define ISO conform exact types, if the compiler does not
   support them,
*/

#ifndef uint8_t
typedef char               char_t;
typedef signed char        int8_t;
typedef signed short       int16_t;  
typedef signed int         int32_t; 
typedef signed long long   int64_t;  
typedef unsigned char      uint8_t;  
typedef unsigned short     uint16_t; 
typedef unsigned int       uint32_t; 
typedef unsigned long long uint64_t;  
typedef float              float32_t;  
typedef double             float64_t;  
#endif /* uint8_t */


typedef void* handle_t;  

/* Version info */
typedef struct 
{
    uint16_t major; 
    uint16_t minor; 
} r_version_t;     


#ifndef R_NULL
#define R_NULL ((void*)0)
#endif /* R_NULL */

#ifndef R_TRUE
#define R_TRUE 1
#endif /* R_TRUE */

#ifndef R_FALSE
#define R_FALSE 0
#endif /* R_FALSE */

#define R_EXTERN extern


#endif /* TYPEDEF_H_ */

