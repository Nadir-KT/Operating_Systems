/*
****************************************************************************
PROJECT : Device Driver 
FILE    : $Id: r_dev_init.c 16747 2019-05-20 05:26:41Z shinya.tomari.wj@renesas.com $
============================================================================
DESCRIPTION
RH850/D1x device initialization
============================================================================
                            C O P Y R I G H T
============================================================================
                       Copyright (c) 2013 - 2016
                                  by 
                       Renesas Electronics (Europe) GmbH.
                           Arcadiastrasse 10
                          D-40472 Duesseldorf
                               Germany
                          All rights reserved.
============================================================================
Purpose: only for testing, not for mass production

DISCLAIMER

LICENSEE has read, understood and accepted the terms and conditions defined in
the license agreement, especially the usage rights. In any case, it is
LICENSEE's responsibility to make sure that any user of the software complies
with the terms and conditions of the signed license agreement.

SAMPLE CODE is not part of the licensed software, as such it must not be used in
mass-production applications. It can only be used for evaluation and
demonstration purposes at customer's premises listed in the signed license
agreement.

****************************************************************************
*/


/*******************************************************************************
  Title: device general (error handling, init/prepare) functions

  
*/

/*******************************************************************************
  Section: Includes
*/

#include "r_typedefs.h"
#include "r_dev_api.h"

#ifdef R_DBG_PRINT_MSG
   #include <stdio.h> 
   #include "r_dbg_api.h"
#endif

/*******************************************************************************
  Section: Local types 
*/

typedef struct 
{
    r_dev_IntSel_t IntIc; 
    uint8_t        Prio; 
} loc_IntCtrl_t;  

/*******************************************************************************
  Section: Local Constants 
*/

/*******************************************************************************
  Constant: LOC_FCCTL0
  
  Addrees of the flash cache configuration register 
*/

#define LOC_FCCTL0              0xffff6480u

/*******************************************************************************
  Constant: LOC_APFSGDPROT_INTC2
  
  Addresses of the APB Guard registers for the INTC and DMA   
*/
#define LOC_APFSGDPROT_INTC2    0xFFC4C000u
#define LOC_APFSGDPROT_PDMA     0xFFC4C004u

/*******************************************************************************
  Constant: LOC_APFSGDPROT_INTC2
  
  Addresses of the PEG registers needed to enable RAM access for AXI Bus masters   
*/

#define LOC_REG_PEGSP           0XFFFEE60CUL
#define LOC_REG_PEGG0MK         0XFFFEE680UL
#define LOC_REG_PEGG0BA         0XFFFEE684UL


/*******************************************************************************
  Constant: loc_IntCtrl
  
  Priority setting for all implemented/used/required interrupts  
*/
static const loc_IntCtrl_t loc_IntCtrl[] = 
{
 /*  Index/name         Set Prio      */
 {R_DEV_INT_P1         , 7      },
 {R_DEV_INT_P2         , 7      },
 {R_DEV_INT_P3         , 7      },
 {R_DEV_INT_P4         , 7      },
 {R_DEV_INT_P5         , 7      },
 {R_DEV_INT_P6         , 7      },
 {R_DEV_INT_P7         , 7      },
 {R_DEV_INT_P8         , 7      },
 {R_DEV_INT_P9         , 7      },
 {R_DEV_INT_P10        , 7      },
 {R_DEV_INT_OSTM0      , 1      },
 {R_DEV_INT_TAUJ0I0    , 2      },
 {R_DEV_INT_TAUJ0I1    , 2      },
 {R_DEV_INT_TAUJ0I2    , 2      },
 {R_DEV_INT_TAUJ0I3    , 2      },
 {R_DEV_INT_DAVEHD0PAUSE , 2      },
 {R_DEV_INT_DAVEHD0SYNC  , 2      },
 {R_DEV_INT_DAVEHD0SP    , 2      },

{R_DEV_INT_VDCE0ERR            ,2},
{R_DEV_INT_VDCE0GR3VBLANK      ,2},
{R_DEV_INT_VDCE0S0VIVSYNC      ,2},
{R_DEV_INT_VDCE0S0LOVSYNC      ,2},
{R_DEV_INT_VDCE0GR3VLINE       ,2},
{R_DEV_INT_VDCE0S0VFIELD       ,2},
{R_DEV_INT_VDCE0S1LOVSYNC      ,2},
{R_DEV_INT_VDCE0OIRVIVSYNC     ,2},
{R_DEV_INT_VDCE0OIRLOVSYNC     ,2},
{R_DEV_INT_VDCE0IRVLINE        ,2},

{R_DEV_INT_VDCE1ERR            ,2},
{R_DEV_INT_VDCE1GR3VBLANK      ,2},
{R_DEV_INT_VDCE1S0VIVSYNC      ,2},
{R_DEV_INT_VDCE1S0LOVSYNC      ,2},
{R_DEV_INT_VDCE1GR3VLINE       ,2},
{R_DEV_INT_VDCE1S0VFIELD       ,2},
{R_DEV_INT_VDCE1S1LOVSYNC      ,2},
 {R_DEV_INT_IMRD0    , 2      }, 
/* delimiter, do not remove !!! */
 {R_DEV_INT_LAST       , 7      }
};


/*******************************************************************************
  Constant: loc_DevName
  
  Human readable device Names   
*/
static const char *loc_DevName[] = 
{
    "CAN-FD Unknown device",
    "CAN-FD D1L1",         
    "CAN-FD D1L2",         
    "CAN-FD D1L2H",        
    "CAN-FD D1M1_3.75M",   
    "CAN-FD D1M1_5M",      
    "CAN-FD D1M1H_3.75M",  
    "CAN-FD D1M1H_5M",     
    "CAN-FD D1M2_3.75M",   
    "CAN-FD R7F701409",    
    "CAN-FD D1M2_5M",      
    "CAN-FD D1M2H_3.75M",  
    "CAN-FD D1M2H_5M",      
    "CAN-FD Unknown device",
    "CAN-FD Unknown device",
    "CAN-FD Unknown device",
    "CAN-FD Unknown device",
    "CAN-FD R7F701417",
    "CAN-FD R7F701418",
    "CAN-FD Unknown device",
    "CAN-FD Unknown device",
    "CAN-FD Unknown device",
    "CAN-FD Unknown device",
    "CAN-FD Unknown device",
    "CAN-FD Unknown device",
    "CAN-FD Unknown device",
    "CAN-FD Unknown device",
    "CAN-FD Unknown device",
    "CAN-FD Unknown device",
    "CAN-FD Unknown device",
    "CAN-FD Unknown device",
    "CAN-FD Unknown device",
    "CAN-FD Unknown device",
    "CAN-FD Unknown device",
    "CAN-FD Unknown device",
    "CAN-FD Unknown device",
    "CAN-FD Unknown device",
    "CAN-FD Unknown device",
    "CAN-FD Unknown device",
    "CAN-FD Unknown device",
    "CAN-FD Unknown device",
    "CAN-FD D1M1A",
    "CAN-FD D1M1-V2",
    "CAN-FD Unknown device",
    "CAN-FD Unknown device",
    "CAN-FD Unknown device",
    "CAN-FD Unknown device",
    "CAN-FD Unknown device",
    "CAN-FD Unknown device",
    "CAN-FD Unknown device",
    "CAN-FD Unknown device",
    "CAN-FD Unknown device",
    "CAN-FD Unknown device",
    "CAN-FD Unknown device",
    "CAN-FD Unknown device",
    "CAN-FD Unknown device",
    "CAN-FD Unknown device",
    "CAN-FD Unknown device",
    "CAN-FD Unknown device",
    "CAN-FD Unknown device",
    "CAN-FD R7F701460"
};


/*******************************************************************************
  Section: Local Variables
*/

/*******************************************************************************
  Variable: loc_DevNum

  loc_DevNum holds the device number that is identified by
  the first function call of R_DEV_GetDev().
  Further calls to R_DEV_GetDev() will immediatly return loc_DevNum.   
*/

#if defined DEV_OVERRIDE
  #warning "Device identification changed by DEV_OVERRIDE!"
  static r_dev_Device_t loc_DevNum = DEV_OVERRIDE;
#else
static r_dev_Device_t loc_DevNum = R_DEV_UNKNOWN_DEVICE;
#endif

/*******************************************************************************
  Variable: loc_DevHasCanFD

  in case the device has CAH-FD (instead of a standard can, this flag is setting
   
*/

static uint32_t loc_DevHasCanFD = 0;

/*******************************************************************************
  Variable: loc_ErrorHandler

  A function pointer to a user callback (Applications error handler)  
  
  The value is assigned by the application using the driver 
*/

static void (*loc_ErrorHandler )(uint32_t Unit, uint32_t Error) = 0;

/*******************************************************************************
  Section: Local functions
*/

/*******************************************************************************
  Function: R_DEV_ErrorHandler
  
  see <r_dev_api.h> 
*/

void R_DEV_ErrorHandler(uint32_t Unit, uint32_t Error) 
{
    if (loc_ErrorHandler != 0) 
    {      
        /* we've got a registered error handler here ...*/
        loc_ErrorHandler(Unit, Error);      
    } 
    else 
    {
        for (;;) 
        {                          /* we don't have a handler */
        }                          /* so, let's wait here */
    }
}

/*******************************************************************************
  Section: Global functions
*/

/*******************************************************************************
  Function: R_DEV_SetErrorCallback

  see <r_dev_api.h>

*/

void R_DEV_SetErrorCallback(void(*ErrorCallback )(uint32_t Unit, uint32_t Error)) 
{

    loc_ErrorHandler    = ErrorCallback;

}

/******************************************************************************* 
  Function: R_DEV_Prepare
 
  see r_dev_api.h for details  
*/ 

void R_DEV_Prepare(void)
{
    uint16_t j      = 0u; 
    r_dev_IntSel_t i;  
    
    /* Disable the AT guard  */
    R_DEV_WRITE_REG(32, LOC_APFSGDPROT_INTC2, 0x07FFFFFF);
    R_DEV_WRITE_REG(32, LOC_APFSGDPROT_PDMA,  0x07FFFFFF);

    /* PEG: Open LRAM access for all */
    R_DEV_WRITE_REG(32, LOC_REG_PEGSP,   0x00000001u);  /* Enable access to PE ressources by external masters */
    R_DEV_WRITE_REG(32, LOC_REG_PEGG0MK, 0xFFFFF000u);  /* Allow the whole address range to be accessed */
    R_DEV_WRITE_REG(32, LOC_REG_PEGG0BA, 0x000000F7u);  /* Allow read and write access for all SPIDs */


    /* Set Table mode for all interrupts */
    for (i = R_DEV_INT_FIRST; i < R_DEV_INT_LAST; i++ )
    {
        R_DEV_IntSetMode(i, R_DEV_INT_TABLE); 
    }
    
    /* Config prio */
    while (R_DEV_INT_LAST > loc_IntCtrl[j].IntIc)
    {
        R_DEV_IntSetPrio(loc_IntCtrl[j].IntIc, loc_IntCtrl[j].Prio); 
        j++; 
    }
}

/*******************************************************************************
  Function: R_DEV_EnableFlashCache
  
  see r_dev_api.h for details  
*/

r_Error_t R_DEV_EnableFlashCache(uint8_t Enable)
{
    /* The code and data cache cannot be configured by SW */
    return R_ERR_OK; 
}


/*******************************************************************************
  Function: R_DEV_GetDev
  
  see r_dev_api.h for details  

*/

r_dev_Device_t R_DEV_GetDev(void) 
{
    uint32_t prdname2;
    uint32_t prdname3;
    
    /* when Device name is already known, return it directly */
    if (loc_DevNum != R_DEV_UNKNOWN_DEVICE)
    {
        return loc_DevNum;
    }
    
    /* get Device name from PRDNAME register */
    prdname2 = R_DEV_READ_REG(32,0xFFCD00D4u);
    prdname3 = R_DEV_READ_REG(32,0xFFCD00D8u) & 0xFu;
    
    /* Device name range: 1460 - R7F701460
        ((prdname2 == 0x36343130) && (prdname3 == 0x00000000))
        
       When R7F701460 POD is used the PRDNAME is
       automatically overwritten by the POD to the emulated device name.
       Thus PRDNAME of R7F701460 can be handled like for a regular device.
       
       Example: When the R7F701460 POD is used to emulate D1L2H,
                the PRDNAME is set to R7F401403 by the Emulation POD automatically.
    */
    
    /* Regular devices */
    /* Check for Can-FD */
    if ( (prdname2 & 0x02000000u) !=0)  /* devices 142x, 143x and 146x have CAN-FD */
    {
        /* Device has CAN-FD -> clear the bit ... */
        prdname2 &= 0xFDFFFFFFu; 
        /* ...and store the info */
        loc_DevHasCanFD = 1; 
    }
    else 
    {
        loc_DevHasCanFD = 0; 
    }
        
    /* Device name range: 1440 - 1449 (& 1461 - 1469) */
    if (prdname2 == 0x34343130) /* (144x) */
    {
        loc_DevNum = ((r_dev_Device_t)((uint32_t)R_DEV_R7F7014OFFS + 40 + prdname3));
        /* special case: D1M1A-alpha (1444)
            (Dedicated mode of R7F701460 emulating D1M1A, where prdname
             is not indicated as prdname of emulated device)
            -> Remap it to D1M1A device prdname (1441) */
        if (loc_DevNum == R_DEV_R7F701444)
        {
            loc_DevNum = R_DEV_R7F701441;   /* D1M1A */
        }
        return loc_DevNum;
    }
    /* Device name range: 1410 - 1419 (& 1430 - 1439) */
    if (prdname2 == 0x31343130) /* (141x) */
    {
        loc_DevNum = ((r_dev_Device_t)((uint32_t)R_DEV_R7F701410 + prdname3));
        return loc_DevNum;
    }
    /* Device name range: 1400 - 1409 (& 1420 - 1429) */
    if (prdname2 == 0x30343130) /* (140x) */
    {
        loc_DevNum = ((r_dev_Device_t)((uint32_t)R_DEV_R7F7014OFFS + prdname3));
        return loc_DevNum;
    }
    
    /* error device not found */
    return R_DEV_UNKNOWN_DEVICE;
}

/*******************************************************************************
  Function: R_DEV_GetVersion
  
  see r_dev_api.h for details  

*/

uint32_t R_DEV_GetVersion(void) 
{
    /* RH850/D1x product version register PRDVER
        PRDVER access is 32bit address is 0xFFCD00C8.
        PRDVER bits [27:16] show the device revision,
               and bits [31:28 + 15:0] are undefined indicated by "X":
            
            RH850/D1x revision: 0xXRRRXXXX
                                   ^^^ bits 27 to 16 relevant for revision
            
            R7F701417-V1 V1.0 (#Yx1):  0xX100XXXX    (D1L1 die based marketing product)
            R7F701417-V2 V2.0 (#Yx2):  0xX200XXXX    (acutal R7F701417 die product)
            
            D1L1    V1.0 (#Yx1):  0xX100XXXX
            D1L1    V1.1 (#Yx2):  0xX200XXXX
            
            D1L2(H) V1.0 (#Yx1):  0xX100XXXX
            D1L2(H) V2.0 (#Yx2):  0xX200XXXX
            D1L2(H) V2.1 (#Yx3):  0xX300XXXX
            D1L2(H) V2.2 (#Yx4):  0xX400XXXX
            
            D1M1-V2 V1.0 (#Yx1):  0xX100XXXX
            
            D1M1(H) V1.0 (#Yx1):  0xX100XXXX
            D1M1(H) V1.2 (#Yx2):  0xX200XXXX
            
            D1M1A   V1.0 (#Yx1):  0xX100XXXX
            
            D1M2(H) V1.0 (#Yx1):  0xX100XXXX
            D1M2(H) V2.0 (#Yx2):  0xX200XXXX
            D1M2(H) V3.0 (#Yx3):  0xX300XXXX
            
            R7F701460 POD      : 0xXFFFXXXX (special revision used to identify Emulation POD)
            
        Devise version the function R_DEV_GetVersion() returns is
        mapped to the format 0xAABB,
        where AA is the mayor revision number
        and BB is the minor revision number.
            e.g. "0x0102" is the device revision "V1.2"
    */
    uint32_t        version;
    r_dev_Device_t  device;

    version = R_DEV_READ_REG(32,0xFFCD00C8u);
    version = ((version >> 16uL) & 0x0FFFu);
    
    device  =  R_DEV_GetDev();
    
    /* R7F701460 POD revision mapping
       (It has special revision to distinguish Emulation POD from real device.) */
    if (version == 0xFFF)
    {
        version = R_DEV_REVISION_EMU;    /* "0xFFF" => Emulation POD */
        return version;
    }
    
    /* R7F701417 revision mapping */
    if ((device >= R_DEV_R7F701416) && (device <= R_DEV_R7F701417))    /* R7F701417 */
    {
        if (version == 0x100)
        {
            version = R_DEV_REVISION_1V0;    /* V1.0 => R7F701417-V1 (D1L1 die based marketing product) */
        } else if (version == 0x200)
        {
            version = R_DEV_REVISION_2V0;    /* V2.0 => R7F701417-V2 (acutal R7F701417 die product) */
        } else
        {
            version = R_DEV_UNKNOWN_DEVICE_REVISION;    /* error, version not supported */
        }
        return version;
    }
    /* D1L1 revision mapping */
    if ((device >= R_DEV_R7F701400) && (device <= R_DEV_R7F701401))    /* D1L1 */
    {
        if (version == 0x100)
        {
            version = R_DEV_REVISION_1V0;    /* V1.0 */
        } else if (version == 0x200)
        {
            version = R_DEV_REVISION_1V1;    /* V1.1 */
        } else
        {
            version = R_DEV_UNKNOWN_DEVICE_REVISION;    /* error, version not supported */
        }
        return version;
    }
    /* D1L2(H) revision mapping */
    if ((device >= R_DEV_R7F701402) && (device <= R_DEV_R7F701403))    /* D1L2(H) */
    {
        if (version == 0x100)
        {
            version = R_DEV_REVISION_1V0;    /* V1.0 */
        } else if (version == 0x200)
        {
            version = R_DEV_REVISION_2V0;    /* V2.0 */
        } else if (version == 0x300)
        {
            version = R_DEV_REVISION_2V1;    /* V2.1 */
        } else if (version == 0x400)
        {
            version = R_DEV_REVISION_2V2;    /* V2.2 */
        } else
        {
            version = R_DEV_UNKNOWN_DEVICE_REVISION;    /* error, version not supported */
        }
        return version;
    }

    /* D1M1-V2 revision mapping */
    if (device == R_DEV_R7F701442)    /* D1M1-V2 */
    {
        if (version == 0x100)
        {
            version = R_DEV_REVISION_1V0;    /* V1.0 */
        } else if (version == 0x200)
        {
            version = R_DEV_REVISION_2V0;    /* V2.0 */
        } else
        {
            version = R_DEV_UNKNOWN_DEVICE_REVISION;    /* error, version not supported */
        }
        return version;
    }

    /* D1M1(H) revision mapping */
    if ((device >= R_DEV_R7F701404) && (device <= R_DEV_R7F701407))    /* D1M1(H) */
    {
        if (version == 0x100)
        {
            version = R_DEV_REVISION_1V0;    /* V1.0 */
        } else if (version == 0x200)
        {
            version = R_DEV_REVISION_1V2;    /* V1.2 */
        } else
        {
            version = R_DEV_UNKNOWN_DEVICE_REVISION;    /* error, version not supported */
        }
        return version;
    }
    
    /* D1M1A revision mapping */
    if ((device == R_DEV_R7F701441) || (device == R_DEV_R7F701418))    /* D1M1A */
    {
        if (version == 0x100)
        {
            version = R_DEV_REVISION_1V0;    /* V1.0 */
        } else if (version == 0x200)
        {
            version = R_DEV_REVISION_1V1;    /* V1.1 */
        } else
        {
            version = R_DEV_UNKNOWN_DEVICE_REVISION;    /* error, version not supported */
        }
        return version;
    }
    
    /* D1M2(H) revision mapping */
    if ((device >= R_DEV_R7F701408) && (device <= R_DEV_R7F701412))    /* D1M2(H) */
    {
        if (version == 0x100)
        {
            version = R_DEV_REVISION_1V0;    /* V1.0 */
        } else if (version == 0x200)
        {
            version = R_DEV_REVISION_2V0;    /* V2.0 */
        } else if (version == 0x300)
        {
            version = R_DEV_REVISION_3V0;    /* V3.0 */
        } else
        {
            version = R_DEV_UNKNOWN_DEVICE_REVISION;    /* error, version not supported */
        }
        return version;
    }
    
    /* failure, version is not supported. */
    version = R_DEV_UNKNOWN_DEVICE_REVISION;
    return version;
}


/*******************************************************************************
  Function: R_DEV_GetDevName
  
  see r_dev_api.h for details  

*/

const char *R_DEV_GetDevName(void) 
{
    uint32_t device = (uint32_t) R_DEV_GetDev(); 
    const char * dev_name; 
    
    if (device > R_DEV_R7F7014OFFS)
    {
       device -= R_DEV_R7F7014OFFS;
    }
    else 
    {
        device = R_DEV_UNKNOWN_DEVICE; 
    }
    dev_name = loc_DevName[device]; 
    if (0 == loc_DevHasCanFD)
    {
        /* skip leading CAN-FD in the string */
        dev_name += 7; 
    }
    return dev_name; 
} 




