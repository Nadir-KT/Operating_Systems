/****************************************************************************
 *  License : All rights reserved for TES Electronic Solutions GmbH
 *            See included /docs/license.txt for details
 *  Project : D/AVE HD
 *  Purpose : Preprocessor defines for platform dependent keywords
 ****************************************************************************
 * Version Control Information :
 *  $Revision: 6749 $
 *  $Date: 2015-11-18 19:16:52 +0900 (水, 18 11 2015) $
 *  $LastChangedBy: florian.zimmermann $
 ****************************************************************************
 * Change History (autogenerated):
 ****************************************************************************/    

#ifndef DAVEHD_PLATFORM_H_INCLUDED
#define DAVEHD_PLATFORM_H_INCLUDED

/*----------------------------------------------------------------------------------------------------------*/
/* Function inlining*/
/**/

#ifdef PRQA
#pragma PRQA_MACRO_MESSAGES_OFF "DHD_INLINE" 1527       /* $Misra: Inline function have to appear in multiple translation units.$*/
#endif

/*----------------------------------------------------------------------------------------------------------*/
/* Interrupt service calling convention*/
/**/

#ifdef _MSC_VER
#define DHD_IRQ_CALL __stdcall                          /* Function specifier used for IRQ callbacks.*/
#else
#define DHD_IRQ_CALL 
#endif

/*----------------------------------------------------------------------------------------------------------*/
/* size_t type definition*/
/**/

#ifdef _MSC_VER
#ifndef _SIZE_T_DEFINED             
#include <stddef.h>                                      /* using stddef on windows*/
#endif
typedef size_t dhd_size_t;
#else
#ifdef __NIOS2__
#include <stddef.h>                                      /* using stddef on nios*/
typedef size_t dhd_size_t;
#else
#if defined(__ghs__)
typedef unsigned int dhd_size_t;                         /* using generic when using GHS*/
#else
#ifdef C5SOC_ARM
#include <stddef.h>                                      /* using stddef on linux*/
typedef size_t dhd_size_t;
#else
typedef unsigned int dhd_size_t;                         /* using a 32bit assumption on other platforms*/
#endif
#endif
#endif
#endif

/*----------------------------------------------------------------------------------------------------------*/
/* In case of coverage analysis, activate suppression macros and other tools.*/

#ifdef _BullseyeCoverage
/* begin a block that has to be suppressed for the coverage analysis.*/
#define DHD_NO_COVERAGE_BEGIN  #pragma BullseyeCoverage off
/* end such a block.*/
#define DHD_NO_COVERAGE_END    #pragma BullseyeCoverage on
#else
/* ignore these defines when coverage is off*/
#define DHD_NO_COVERAGE_BEGIN
#define DHD_NO_COVERAGE_END
#endif

/*----------------------------------------------------------------------------------------------------------*/

#endif /*DAVEHD_PLATFORM_H_INCLUDED*/
