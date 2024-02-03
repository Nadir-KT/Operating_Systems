/****************************************************************************
 *  License : All rights reserved for TES Electronic Solutions GmbH
 *            See included /docs/license.txt for details
 *  Project : D/AVE HD
 *  Purpose : Hardware specific compiletime settings
 ****************************************************************************
 * Version Control Information :
 *  $Revision: 14857 $
 *  $Date: 2017-10-11 18:50:27 +0900 (水, 11 10 2017) $
 *  $LastChangedBy: florian.zimmermann $
 ****************************************************************************
 * Change History (autogenerated):
 ****************************************************************************/
/*----------------------------------------------------------------------------------------------------------*/
/* Renesas D1x Platform gets platform constrained registermap, default config for all other*/
/**/
#ifndef DAVEHD_REGISTERMAP_H_INCLUDED
#define DAVEHD_REGISTERMAP_H_INCLUDED

#ifdef RENESAS_D1X
  #include "davehd_registermap_d1x.h"
#else
  #include "davehd_registermap_default.h"
#endif

#endif /* DAVEHD_REGISTERMAP_H_INCLUDED */
