/*
****************************************************************************
PROJECT : OCTA driver
FILE    : $Id: r_octacdb_macromix.c 13935 2017-06-15 10:30:54Z shinya.tomari $
============================================================================
DESCRIPTION
Octa flash command table for MACRONIX flash device
============================================================================
                            C O P Y R I G H T
============================================================================
                           Copyright (c) 2017
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

/***************************************************************************
  Title: Octa flash command table

  Note: This file defines the macro define and the flash command table for
        Macronix MX25LW51245G octa flash device and JSC JSC64SSU8AGDY octa RAM device.
        If you use other octa flash device, change setting
        parameters according to the model number of the target.

*/


/***************************************************************************
  Section: Includes
*/
#include "r_typedefs.h"
#include "r_octa_api.h"

/***************************************************************************
  Section: Local defines for Octa Flash
*/

/***************************************************************************
  Constants: Dummy Length clock of Octa Flash

  Description:
  Dummy Length clock cycles for each frequency.

  Values:
  LOC_OF_DL_066MHZ - Dummy Length clock cycles for 66MHz
  LOC_OF_DL_084MHZ - Dummy Length clock cycles for 84MHz
  LOC_OF_DL_104MHZ - Dummy Length clock cycles for 104MHz
  LOC_OF_DL_133MHZ - Dummy Length clock cycles for 133MHz
*/

#define LOC_OF_DL_066MHZ         ( 6)   /*  6 Clock */
#define LOC_OF_DL_084MHZ         ( 8)   /*  8 Clock */
#define LOC_OF_DL_104MHZ         (10)   /* 10 Clock */
#define LOC_OF_DL_133MHZ         (14)   /* 14 Clock */


/***************************************************************************
  Constants: Clock cycles for Dummy Length of Octa Flash

  Description:
  Dummy Length clock cycles for register / memory access.

  Values:
  LOC_OF_DL_CFG     - Configured clock cycles.
  LOC_OF_DL_REG_R   - Clock cycles of status / configuration / configuration2 register access.
  LOC_OF_DL_MSP_R   - Clock cycles of memory read access on SPI mode.
  LOC_OF_DL_MOP_R   - Clock cycles of memory read access on OPI / DOPI mode.
  
*/

#define LOC_OF_DL_CFG            (LOC_OF_DL_084MHZ)

#define LOC_OF_DL_REG_R          (4)
#define LOC_OF_DL_MSP_R          (8)
#define LOC_OF_DL_MOP_R          (LOC_OF_DL_CFG)

/***************************************************************************
  Constants: Dummy Length code of Octa Flash

  Description:
  Dummy Length code. This value is written to Configuration register.

*/
#define LOC_OF_DL_CODE           (10 - ((LOC_OF_DL_CFG) / 2))

/***************************************************************************
  Section: Local defines for Octa RAM
*/
/***************************************************************************
  Constants: Dummy Length clock of Octa RAM

  Description:
  Dummy Length clock cycles for each frequency.

  Values:
  LOC_OR_DL_083MHZ - Dummy Length clock cycles for 83MHz
  LOC_OR_DL_100MHZ - Dummy Length clock cycles for 100MHz
  LOC_OR_DL_133MHZ - Dummy Length clock cycles for 133MHz
*/

#define LOC_OR_DL_083MHZ         (3)   /* 3 Clock */
#define LOC_OR_DL_100MHZ         (4)   /* 4 Clock */
#define LOC_OR_DL_133MHZ         (5)   /* 5 Clock */

/***************************************************************************
  Constants: Clock cycles for Dummy Length of Octa RAM

  Description:
  Dummy Length clock cycles for each frequency.

  Values:
  LOC_OR_DL_CFG    - Configurable clock cycles.
  
  LOC_OR_DL_REG_R  - Clock cycles for Dummy Length for configuration register access.
  LOC_OR_DL_MEM_R  - Clock cycles for Dummy Length for memory read access.
  LOC_OR_DL_MEM_W  - Clock cycles for Dummy Length for memory write access.
  
*/
#define LOC_OR_DL_CFG            (LOC_OR_DL_100MHZ)

#define LOC_OR_DL_REG_R          (LOC_OR_DL_CFG)
#define LOC_OR_DL_MEM_R          (LOC_OR_DL_CFG)
#define LOC_OR_DL_MEM_W          (LOC_OR_DL_CFG)

/***************************************************************************
  Constants: Dummy Length code of Octa RAM

  Description:
  Dummy Length code. This value is written to Configuration register.

*/
#define LOC_OR_DL_CODE           (LOC_OR_DL_CFG - 3)


/***************************************************************************
  Section: Global Variables for Octa Flash
*/

/*******************************************************************************
  Variable: r_octacdb_FlashCmdTbl

  This value stores the Octa flash command table.
  
*/

const r_octa_Command_t  r_octacdb_FlashCmdTbl = { 

    /* Reg              , Address   , AddressLength, BitPosition */
    { R_OCTA_STATUS_REG , 0         , 0            , 0x003C     }, /* BlockProtect        */
    { R_OCTA_STATUS_REG , 0         , 0            , 0x0002     }, /* WriteEnableLatch    */
    { R_OCTA_STATUS_REG , 0         , 0            , 0x0001     }, /* WriteInProgress     */
    { R_OCTA_CONFIG2_REG, 0x00000200, 4            , 0x0001     }, /* PreCycle            */
    { R_OCTA_CONFIG2_REG, 0x00000000, 4            , 0x0003     }, /* FlashTransferType   */
    { R_OCTA_CONFIG2_REG, 0x00000200, 4            , 0x0002     }, /* DOS                 */
    
    /* Reg              , Address   , AddressLength, BitMask, BitSet */
    { R_OCTA_CONFIG2_REG, 0x00000300, 4            , 0x07   , LOC_OF_DL_CODE }, /* DummyCycle     */
    
    /* Low  , High  , Between */
    { R_OCTA_LOWPERIOD_2 , R_OCTA_HIGHPERIOD_4, R_OCTA_BETWEENPERIOD_5 }, /* ReadTiming          */
    { R_OCTA_LOWPERIOD_2 , R_OCTA_HIGHPERIOD_5, R_OCTA_BETWEENPERIOD_7 }, /* WriteTiming         */
    { R_OCTA_LOWPERIOD_2 , R_OCTA_HIGHPERIOD_5, R_OCTA_BETWEENPERIOD_7 }, /* CfgTiming           */
    
    /* Cmd  , Address   , CmdLength, AddressLength, DummyLength    , DataLength, OpeFlags */
    { 0x000B, 0         , 1        , 3            , LOC_OF_DL_MSP_R, 0         , R_OCTA_CMD_READ8      }, /* ReadSPI3B           */
    { 0x000C, 0         , 1        , 4            , LOC_OF_DL_MSP_R, 0         , R_OCTA_CMD_READ8      }, /* ReadSPI4B           */
    { 0xEC13, 0         , 2        , 4            , LOC_OF_DL_MOP_R, 0         , R_OCTA_CMD_READ8      }, /* ReadOPI             */
    { 0xEE11, 0         , 2        , 4            , LOC_OF_DL_MOP_R, 0         , R_OCTA_CMD_READ16     }, /* ReadDOPI           */
    { 0x0002, 0         , 1        , 3            , 0              , 0         , R_OCTA_CMD_WRITE8     }, /* WriteSPI3B          */
    { 0x0012, 0         , 1        , 4            , 0              , 0         , R_OCTA_CMD_WRITE8     }, /* WriteSPI4B          */
    { 0x12ED, 0         , 2        , 4            , 0              , 0         , R_OCTA_CMD_WRITE8     }, /* WriteOPI            */
    { 0x12ED, 0         , 2        , 4            , 0              , 0         , R_OCTA_CMD_WRITE16    }, /* WriteDOPI          */
    { 0x0022, 0         , 1        , 4            , 0              , 0         , R_OCTA_CMD_WRITE_NONE }, /* WriteBufInitialSPI  */
    { 0x22DD, 0         , 2        , 4            , 0              , 0         , R_OCTA_CMD_WRITE_NONE }, /* WriteBufInitialOPI  */
    { 0x0024, 0         , 1        , 4            , 0              , 0         , R_OCTA_CMD_WRITE8     }, /* WriteBufContinueSPI */
    { 0x24DB, 0         , 2        , 4            , 0              , 0         , R_OCTA_CMD_WRITE8     }, /* WriteBufContinueOPI */
    { 0x0031, 0         , 1        , 0            , 0              , 0         , R_OCTA_CMD_WRITE_NONE }, /* WriteBufConfirmSPI  */
    { 0x31CE, 0         , 2        , 0            , 0              , 0         , R_OCTA_CMD_WRITE_NONE }, /* WriteBufConfirmOPI  */
    { 0x0006, 0         , 1        , 0            , 0              , 0         , R_OCTA_CMD_WRITE_NONE }, /* WriteEnableSPI      */
    { 0x06F9, 0         , 2        , 0            , 0              , 0         , R_OCTA_CMD_WRITE_NONE }, /* WriteEnableOPI      */
    { 0x0020, 0         , 1        , 3            , 0              , 0         , R_OCTA_CMD_WRITE_NONE }, /* EraseSPI3B          */
    { 0x0021, 0         , 1        , 4            , 0              , 0         , R_OCTA_CMD_WRITE_NONE }, /* EraseSPI4B          */
    { 0x21DE, 0         , 2        , 4            , 0              , 0         , R_OCTA_CMD_WRITE_NONE }, /* EraseOPI            */
    { 0x0005, 0         , 1        , 0            , 0              , 1         , R_OCTA_CMD_READ8      }, /* ReadStsSPI          */
    { 0x05FA, 0x00000000, 2        , 4            , LOC_OF_DL_REG_R, 1         , R_OCTA_CMD_READ8      }, /* ReadStsOPI          */
    { 0x0015, 0         , 1        , 0            , 0              , 1         , R_OCTA_CMD_READ8      }, /* ReadCfgSPI          */
    { 0x15EA, 0x00000001, 2        , 4            , LOC_OF_DL_REG_R, 1         , R_OCTA_CMD_READ8      }, /* ReadCfgOPI          */
    { 0x0071, 0         , 1        , 0            , 0              , 1         , R_OCTA_CMD_READ8      }, /* ReadCfg2SPI         */
    { 0x718E, 0         , 2        , 0            , LOC_OF_DL_REG_R, 1         , R_OCTA_CMD_READ8      }, /* ReadCfg2OPI         */
    { 0x0001, 0         , 1        , 0            , 0              , 2         , R_OCTA_CMD_WRITE16    }, /* WriteStsCfgSPI      */
    { 0x01FE, 0x00000000, 2        , 4            , 0              , 1         , R_OCTA_CMD_WRITE8     }, /* WriteStsOPI         */
    { 0x01FE, 0x00000001, 2        , 4            , 0              , 1         , R_OCTA_CMD_WRITE8     }, /* WriteCfgOPI         */
    { 0x0072, 0         , 1        , 0            , 0              , 1         , R_OCTA_CMD_WRITE8     }, /* WriteCfg2SPI        */
    { 0x728D, 0         , 2        , 0            , 0              , 1         , R_OCTA_CMD_WRITE8     }, /* WriteCfg2OPI        */
    { 0x0066, 0         , 1        , 0            , 0              , 0         , R_OCTA_CMD_WRITE_NONE }, /* ResetEnableSPI      */
    { 0x6699, 0         , 2        , 0            , 0              , 0         , R_OCTA_CMD_WRITE_NONE }, /* ResetEnableOPI      */
    { 0x0099, 0         , 1        , 0            , 0              , 0         , R_OCTA_CMD_WRITE_NONE }, /* ResetSPI            */
    { 0x9966, 0         , 2        , 0            , 0              , 0         , R_OCTA_CMD_WRITE_NONE }, /* ResetOPI            */
    { 0x009F, 0         , 1        , 0            , 0              , 3         , R_OCTA_CMD_READ8      }, /* ReadIDSPI           */
    { 0x9F60, 0x00000000, 2        , 4            , LOC_OF_DL_REG_R, 3         , R_OCTA_CMD_READ8      }  /* ReadIDOPI           */
};

/***************************************************************************
  Section: Global Variables for Octa RAM 
*/

/*******************************************************************************
  Variable: r_octacdb_RamCmdTabl

  This value stores the Octa RAM command table.
  
*/

const r_octa_Command_t  r_octacdb_RamCmdTabl = { 

    /* Reg              , Address   , AddressLength, BitPosition */
    { R_OCTA_NONE_REG   , 0         , 0            , 0          }, /* BlockProtect        */
    { R_OCTA_NONE_REG   , 0         , 0            , 0          }, /* WriteEnableLatch    */
    { R_OCTA_NONE_REG   , 0         , 0            , 0          }, /* WriteInProgress     */
    { R_OCTA_CONFIG_REG , 0x00040000, 4            , 0x0100     }, /* PreCycle            */
    { R_OCTA_NONE_REG   , 0         , 0            , 0          }, /* FlashTransferType   */
    { R_OCTA_NONE_REG   , 0         , 0            , 0          }, /* DOS                 */
    
    /* Reg              , Address   , AddressLength, BitMask, BitSet */ 
    { R_OCTA_CONFIG_REG , 0x00040000, 4            , 0x00F0 , ((uint16_t)LOC_OR_DL_CODE << 4) }, /* DummyCycle     */
    
    /* Low  , High  , Between */
    { R_OCTA_LOWPERIOD_2 , R_OCTA_HIGHPERIOD_4, R_OCTA_BETWEENPERIOD_5 }, /* ReadTiming          */
    { R_OCTA_LOWPERIOD_2 , R_OCTA_HIGHPERIOD_5, R_OCTA_BETWEENPERIOD_7 }, /* WriteTiming         */
    { R_OCTA_LOWPERIOD_2 , R_OCTA_HIGHPERIOD_5, R_OCTA_BETWEENPERIOD_7 }, /* CfgTiming           */
    
    /* Cmd  , Address   , CmdLength, AddressLength, DummyLength    , DataLength, OpeFlags */
    { 0xFFFF, 0         , 0        , 0            , 0              , 0         , R_OCTA_CMD_NONE    }, /* ReadSPI3B           */
    { 0xFFFF, 0         , 0        , 0            , 0              , 0         , R_OCTA_CMD_NONE    }, /* ReadSPI4B           */
    { 0xFFFF, 0         , 0        , 0            , 0              , 0         , R_OCTA_CMD_NONE    }, /* ReadOPI             */
    { 0xA000, 0         , 2        , 4            , LOC_OR_DL_MEM_R, 0         , R_OCTA_CMD_READ16  }, /* ReadDOPI           */
    { 0xFFFF, 0         , 0        , 0            , 0              , 0         , R_OCTA_CMD_NONE    }, /* WriteSPI3B          */
    { 0xFFFF, 0         , 0        , 0            , 0              , 0         , R_OCTA_CMD_NONE    }, /* WriteSPI4B          */
    { 0xFFFF, 0         , 0        , 0            , 0              , 0         , R_OCTA_CMD_NONE    }, /* WriteOPI            */
    { 0x2000, 0         , 2        , 4            , LOC_OR_DL_MEM_W, 0         , R_OCTA_CMD_WRITE16 }, /* WriteDOPI          */
    { 0xFFFF, 0         , 0        , 0            , 0              , 0         , R_OCTA_CMD_NONE    }, /* WriteBufInitialSPI  */
    { 0xFFFF, 0         , 0        , 0            , 0              , 0         , R_OCTA_CMD_NONE    }, /* WriteBufInitialOPI  */
    { 0xFFFF, 0         , 0        , 0            , 0              , 0         , R_OCTA_CMD_NONE    }, /* WriteBufContinueSPI */
    { 0xFFFF, 0         , 0        , 0            , 0              , 0         , R_OCTA_CMD_NONE    }, /* WriteBufContinueOPI */
    { 0xFFFF, 0         , 0        , 0            , 0              , 0         , R_OCTA_CMD_NONE    }, /* WriteBufConfirmSPI  */
    { 0xFFFF, 0         , 0        , 0            , 0              , 0         , R_OCTA_CMD_NONE    }, /* WriteBufConfirmOPI  */
    { 0xFFFF, 0         , 0        , 0            , 0              , 0         , R_OCTA_CMD_NONE    }, /* WriteEnableSPI      */
    { 0xFFFF, 0         , 0        , 0            , 0              , 0         , R_OCTA_CMD_NONE    }, /* WriteEnableOPI      */
    { 0xFFFF, 0         , 0        , 0            , 0              , 0         , R_OCTA_CMD_NONE    }, /* EraseSPI3B          */
    { 0xFFFF, 0         , 0        , 0            , 0              , 0         , R_OCTA_CMD_NONE    }, /* EraseSPI4B          */
    { 0xFFFF, 0         , 0        , 0            , 0              , 0         , R_OCTA_CMD_NONE    }, /* EraseOPI            */
    { 0xFFFF, 0         , 0        , 0            , 0              , 0         , R_OCTA_CMD_NONE    }, /* ReadStsSPI          */
    { 0xFFFF, 0         , 0        , 0            , 0              , 0         , R_OCTA_CMD_NONE    }, /* ReadStsOPI          */
    { 0xFFFF, 0         , 0        , 0            , 0              , 0         , R_OCTA_CMD_NONE    }, /* ReadCfgSPI          */
    { 0xC000, 0x00040000, 2        , 4            , LOC_OR_DL_REG_R, 2         , R_OCTA_CMD_READ16  }, /* ReadCfgOPI          */
    { 0xFFFF, 0         , 0        , 0            , 0              , 0         , R_OCTA_CMD_NONE    }, /* ReadCfg2SPI         */
    { 0xFFFF, 0         , 0        , 0            , 0              , 0         , R_OCTA_CMD_NONE    }, /* ReadCfg2OPI         */
    { 0xFFFF, 0         , 0        , 0            , 0              , 0         , R_OCTA_CMD_NONE    }, /* WriteStsCfgSPI      */
    { 0xFFFF, 0         , 0        , 0            , 0              , 0         , R_OCTA_CMD_NONE    }, /* WriteStsOPI         */
    { 0x4000, 0x00040000, 2        , 4            , 0              , 2         , R_OCTA_CMD_WRITE16 }, /* WriteCfgOPI         */
    { 0xFFFF, 0         , 0        , 0            , 0              , 0         , R_OCTA_CMD_NONE    }, /* WriteCfg2SPI        */
    { 0xFFFF, 0         , 0        , 0            , 0              , 0         , R_OCTA_CMD_NONE    }, /* WriteCfg2OPI        */
    { 0xFFFF, 0         , 0        , 0            , 0              , 0         , R_OCTA_CMD_NONE    }, /* ResetEnableSPI      */
    { 0xFFFF, 0         , 0        , 0            , 0              , 0         , R_OCTA_CMD_NONE    }, /* ResetEnableOPI      */
    { 0xFFFF, 0         , 0        , 0            , 0              , 0         , R_OCTA_CMD_NONE    }, /* ResetSPI            */
    { 0xFFFF, 0         , 0        , 0            , 0              , 0         , R_OCTA_CMD_NONE    }, /* ResetOPI            */
    { 0xFFFF, 0         , 0        , 0            , 0              , 0         , R_OCTA_CMD_NONE    }, /* ReadIDSPI           */
    { 0xC000, 0x00000000, 2        , 4            , LOC_OR_DL_REG_R, 2         , R_OCTA_CMD_READ16  }  /* ReadIDOPI           */
};
/* End Of file */
