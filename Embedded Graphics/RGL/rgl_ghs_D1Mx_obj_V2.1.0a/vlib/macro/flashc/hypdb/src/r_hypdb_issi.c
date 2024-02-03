/*
****************************************************************************
PROJECT : HYPB driver
FILE    : $Id: r_hypdb_issi.c 10908 2016-11-01 08:05:38Z shinya.tomari $
============================================================================
DESCRIPTION
Hyper flash command table for ISSI flash device
============================================================================
                            C O P Y R I G H T
============================================================================
                           Copyright (c) 2016
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
  Title: Hyper flash command table

  Note: This file defines the macro define and the flash command table for
        ISSI IS26KS128S hyper flash device and IS66WVH16M8BLL hyper RAM device.
        If you use other hyper flash device, change setting
        parameters according to the model number of the target.

*/


/***************************************************************************
  Section: Includes
*/
#include "r_typedefs.h"
#include "r_hypb_api.h"

/***************************************************************************
  Section: Local defines for Hyper Flash
*/

/***************************************************************************
  Constants: Command flag of Hyper Flash

  Description:
  Command flag of Hyper Flash command. It is set to CA[47:45].

  Values:
  LOC_HF_FLAG_R - Read transaction. 
  LOC_HF_FLAG_W - Write transaction. 
*/

#define LOC_HF_FLAG_R             (0xA0) /* CA[47:45]= 101b */
#define LOC_HF_FLAG_W             (0x20) /* CA[47:45]= 001b */

/***************************************************************************
  Constants: Latency clock of Hyper Flash

  Description:
  Latency clock cycles for each frequency.

  Values:
  LOC_HF_LTC_052MHZ - Latency clock cycles for 52MHz
  LOC_HF_LTC_062MHZ - Latency clock cycles for 62MHz
  LOC_HF_LTC_072MHZ - Latency clock cycles for 72MHz
  LOC_HF_LTC_083MHZ - Latency clock cycles for 83MHz
  LOC_HF_LTC_093MHZ - Latency clock cycles for 93MHz
  LOC_HF_LTC_104MHZ - Latency clock cycles for 104MHz
  LOC_HF_LTC_114MHZ - Latency clock cycles for 114MHz
  LOC_HF_LTC_125MHZ - Latency clock cycles for 125MHz
  LOC_HF_LTC_135MHZ - Latency clock cycles for 135MHz
  LOC_HF_LTC_145MHZ - Latency clock cycles for 145MHz
  LOC_HF_LTC_156MHZ - Latency clock cycles for 156MHz
  LOC_HF_LTC_166MHZ - Latency clock cycles for 166MHz
*/

#define LOC_HF_LTC_052MHZ         ( 5)   /*  5 Clock */
#define LOC_HF_LTC_062MHZ         ( 6)   /*  6 Clock */
#define LOC_HF_LTC_072MHZ         ( 7)   /*  7 Clock */
#define LOC_HF_LTC_083MHZ         ( 8)   /*  8 Clock */
#define LOC_HF_LTC_093MHZ         ( 9)   /*  9 Clock */
#define LOC_HF_LTC_104MHZ         (10)   /* 10 Clock */
#define LOC_HF_LTC_114MHZ         (11)   /* 11 Clock */
#define LOC_HF_LTC_125MHZ         (12)   /* 12 Clock */
#define LOC_HF_LTC_135MHZ         (13)   /* 13 Clock */
#define LOC_HF_LTC_145MHZ         (14)   /* 14 Clock */
#define LOC_HF_LTC_156MHZ         (15)   /* 15 Clock */
#define LOC_HF_LTC_166MHZ         (16)   /* 16 Clock */

/***************************************************************************
  Constants: Clock cycles for Latency of Hyper Flash

  Description:
  Latency clock cycles for each frequency.

  Values:
  LOC_HF_LTC_DEF - Default clock.
  LOC_HF_LTC_CFG - Configured clock cycles.
  LOC_HF_LTC_R1  - Clock cycles for latency at the first read access.
                   This value is dependent on default latency code.
                   It is set in Non-Volatile Configuration Register. 
  LOC_HF_LTC_R2  - Clock cycles for latency after the second read access.
                   This value is dependent on the value of the configured Latency code.
  LOC_HF_LTC_W   - Clock cycles for latency for write access.
  
*/

#define LOC_HF_LTC_DEF            (LOC_HF_LTC_166MHZ)
#define LOC_HF_LTC_CFG            (LOC_HF_LTC_083MHZ)

#define LOC_HF_LTC_R1             (LOC_HF_LTC_DEF)
#define LOC_HF_LTC_R2             (LOC_HF_LTC_CFG)
#define LOC_HF_LTC_W              (0) /* No latency is needed. */

/***************************************************************************
  Constants: Latency code of Hyper Flash

  Description:
  Latency code. This value is written to Configuration register.

*/
#define LOC_HF_LTC_CODE           (LOC_HF_LTC_CFG - 5)

/***************************************************************************
  Section: Local defines for Hyper RAM
*/

/***************************************************************************
  Constants: Commnad flag of Hyper RAM

  Description:
  Command flag of Hyper RAM command. It is set to CA[47:45].

  Values:
  LOC_HR_FLAG_MEM_R - Memory read transaction.    
  LOC_HR_FLAG_MEM_W - Memory write transaction.   
  LOC_HR_FLAG_REG_R - Register read transaction.  
  LOC_HR_FLAG_REG_W - Register write transaction. 
*/

#define LOC_HR_FLAG_MEM_R         (0xA0) /* CA[47:45]= 101b */
#define LOC_HR_FLAG_MEM_W         (0x20) /* CA[47:45]= 001b */
#define LOC_HR_FLAG_REG_R         (0xE0) /* CA[47:45]= 111b */
#define LOC_HR_FLAG_REG_W         (0x60) /* CA[47:45]= 011b */

/***************************************************************************
  Constants: Latency clock of Hyper RAM

  Description:
  Latency clock cycles for each frequency.

  Values:
  LOC_HR_LTC_083MHZ - Latency clock cycles for 83MHz
  LOC_HR_LTC_100MHZ - Latency clock cycles for 100MHz
  LOC_HR_LTC_133MHZ - Latency clock cycles for 133MHz
  LOC_HR_LTC_166MHZ - Latency clock cycles for 166MHz
*/

#define LOC_HR_LTC_083MHZ         ( 6)   /* 3 * 2 Clock (Fixed 2 times) */
#define LOC_HR_LTC_100MHZ         ( 8)   /* 4 * 2 Clock (Fixed 2 times) */
#define LOC_HR_LTC_133MHZ         (10)   /* 5 * 2 Clock (Fixed 2 times) */
#define LOC_HR_LTC_166MHZ         (12)   /* 6 * 2 Clock (Fixed 2 times) */

/***************************************************************************
  Constants: Clock cycles for Latency of Hyper Flash

  Description:
  Latency clock cycles for each frequency.

  Values:
  LOC_HR_LTC_DEF    - Default clock.
                      This value is dependent on default latency code.
                      It is set in Non-Volatile Configuration Register. 
  LOC_HR_LTC_CFG    - Configurable clock cycles.
  
  LOC_HR_LTC_REG_R1 - Clock cycles for latency at the first register read access.
                      This value is dependent on default latency code.
  LOC_HR_LTC_REG_R2 - Clock cycles for latency after the second register read access.
                      This value is dependent on the value of the configured Latency code.
  LOC_HR_LTC_REG_W  - Clock cycles for latency for register write access.
  LOC_HR_LTC_MEM_R  - Clock cycles for latency for memory read access.
                       This value is dependent on the value of the configured Latency code.
  LOC_HR_LTC_MEM_W  - Clock cycles for latency for memory write access.
                      This value is dependent on the value of the configured Latency code.
  
*/
#define LOC_HR_LTC_DEF            (LOC_HR_LTC_166MHZ)
#define LOC_HR_LTC_CFG            (LOC_HR_LTC_100MHZ)

#define LOC_HR_LTC_REG_R1         (LOC_HR_LTC_DEF)
#define LOC_HR_LTC_REG_R2         (LOC_HR_LTC_CFG)
#define LOC_HR_LTC_REG_W          (0) /* No latency is needed. */
#define LOC_HR_LTC_MEM_R          (LOC_HR_LTC_CFG)
#define LOC_HR_LTC_MEM_W          (LOC_HR_LTC_CFG)

/* Latency code of Hyper RAM */
#if (LOC_HR_LTC_CFG == LOC_HR_LTC_083MHZ)
#define LOC_HR_LTC_CODE           (0x0E)
#elif (LOC_HR_LTC_CFG == LOC_HR_LTC_100MHZ)
#define LOC_HR_LTC_CODE           (0x0F)
#elif (LOC_HR_LTC_CFG == LOC_HR_LTC_133MHZ)
#define LOC_HR_LTC_CODE           (0x00)
#else
#define LOC_HR_LTC_CODE           (0x01)
#endif

/***************************************************************************
  Section: Global Variables for Hyper Flash
*/

/*******************************************************************************
  Variable: loc_FlashCmdReset

  This value stores the Hyper flash Reset command sequence.
*/
static const r_hypb_CmdTransaction_t  loc_FlashCmdReset[]  = { 
    /* 1. Software Reset */
    /* Address, Data    , CmdFlags     , Latency      , OpeFlags   */
    { 0       , 0x00F0uL, LOC_HF_FLAG_W, LOC_HF_LTC_W , R_HYPB_CMD_WRITE16 },
    
    /* 2. Status Register Clear */
    { 0x0555uL, 0x0071uL, LOC_HF_FLAG_W, LOC_HF_LTC_W , R_HYPB_CMD_WRITE16 },
};


/*******************************************************************************
  Variable: loc_FlashCmdRead

  This value stores the Hyper flash Read command sequence.
*/
static const r_hypb_CmdTransaction_t  loc_FlashCmdRead[]  = { 
    /* Address, Data    , CmdFlags     , Latency      , OpeFlags   */
    { 0       , 0       , LOC_HF_FLAG_R, LOC_HF_LTC_R2, R_HYPB_CMD_READ64  | 
                                                        R_HYPB_CMD_SET_ADDR }
};

/*******************************************************************************
  Variable: loc_FlashCmdWrite

  This value stores the Hyper flash Write directly command sequence.
*/
static const r_hypb_CmdTransaction_t  loc_FlashCmdWrite[] = {
    /* Address, Data    , CmdFlags     , Latency      , OpeFlags   */
    { 0x0555uL, 0x00AAuL, LOC_HF_FLAG_W, LOC_HF_LTC_W , R_HYPB_CMD_WRITE16   },
    { 0x02AAuL, 0x0055uL, LOC_HF_FLAG_W, LOC_HF_LTC_W , R_HYPB_CMD_WRITE16   },
    { 0x0555uL, 0x00A0uL, LOC_HF_FLAG_W, LOC_HF_LTC_W , R_HYPB_CMD_WRITE16   },
    { 0       , 0       , LOC_HF_FLAG_W, LOC_HF_LTC_W , R_HYPB_CMD_WRITE_A  |
                                                        R_HYPB_CMD_SET_ADDR | 
                                                        R_HYPB_CMD_SET_DATA  }
};

/*******************************************************************************
  Variable: loc_FlashCmdWriteBuf

  This value stores the Hyper flash Write via buffer command sequence.
*/
static const r_hypb_CmdTransaction_t  loc_FlashCmdWriteBuf[] = {
    /* 1. Write to Buffer */
    /* Address, Data    , CmdFlags     , Latency      , OpeFlags   */
    { 0x0555uL, 0x00AAuL, LOC_HF_FLAG_W, LOC_HF_LTC_W , R_HYPB_CMD_WRITE16   },
    { 0x02AAuL, 0x0055uL, LOC_HF_FLAG_W, LOC_HF_LTC_W , R_HYPB_CMD_WRITE16   },
    { 0       , 0x0025  , LOC_HF_FLAG_W, LOC_HF_LTC_W , R_HYPB_CMD_WRITE16  |
                                                        R_HYPB_CMD_SET_SA    },
    { 0       , 0       , LOC_HF_FLAG_W, LOC_HF_LTC_W , R_HYPB_CMD_WRITE16  |
                                                        R_HYPB_CMD_SET_SA   | 
                                                        R_HYPB_CMD_SET_COUNT },
    { 0       , 0       , LOC_HF_FLAG_W, LOC_HF_LTC_W , R_HYPB_CMD_WRITE16  |
                                                        R_HYPB_CMD_SET_ADDR | 
                                                        R_HYPB_CMD_SET_DATA | 
                                                        R_HYPB_CMD_LOOP      },
    /* 2. Program Buffer to Flash  */
    { 0       , 0x0029  , LOC_HF_FLAG_W, LOC_HF_LTC_W , R_HYPB_CMD_WRITE16  |
                                                        R_HYPB_CMD_SET_SA    },
};

/*******************************************************************************
  Variable: loc_FlashCmdErase

  This value stores the Hyper flash Erase command sequence.
*/

static const r_hypb_CmdTransaction_t  loc_FlashCmdErase[] = {
    /* Address, Data    , CmdFlags     , Latency      , OpeFlags   */
    { 0x0555uL, 0x00AAuL, LOC_HF_FLAG_W, LOC_HF_LTC_W , R_HYPB_CMD_WRITE16  },
    { 0x02AAuL, 0x0055uL, LOC_HF_FLAG_W, LOC_HF_LTC_W , R_HYPB_CMD_WRITE16  },
    { 0x0555uL, 0x0080uL, LOC_HF_FLAG_W, LOC_HF_LTC_W , R_HYPB_CMD_WRITE16  },
    { 0x0555uL, 0x00AAuL, LOC_HF_FLAG_W, LOC_HF_LTC_W , R_HYPB_CMD_WRITE16  },
    { 0x02AAuL, 0x0055uL, LOC_HF_FLAG_W, LOC_HF_LTC_W , R_HYPB_CMD_WRITE16  },
    { 0       , 0x0030uL, LOC_HF_FLAG_W, LOC_HF_LTC_W , R_HYPB_CMD_WRITE16 |
                                                        R_HYPB_CMD_SET_SA   }
};

/*******************************************************************************
  Variable: loc_FlashCmdProtect

  This value stores the Hyper flash Protect command sequence.
*/

static const r_hypb_CmdTransaction_t  loc_FlashCmdProtect[] = {
    /* 1. DYB ASO Entry  */
    /* Address, Data    , CmdFlags     , Latency      , OpeFlags   */
    { 0x0555uL, 0x00AAuL, LOC_HF_FLAG_W, LOC_HF_LTC_W , R_HYPB_CMD_WRITE16   },
    { 0x02AAuL, 0x0055uL, LOC_HF_FLAG_W, LOC_HF_LTC_W , R_HYPB_CMD_WRITE16   },
    { 0x0555uL, 0x00E0uL, LOC_HF_FLAG_W, LOC_HF_LTC_W , R_HYPB_CMD_WRITE16   },
    /* 2. DYB Set / Clear  */
    { 0      ,  0x00A0uL, LOC_HF_FLAG_W, LOC_HF_LTC_W , R_HYPB_CMD_WRITE16   },
    { 0      ,  0       , LOC_HF_FLAG_W, LOC_HF_LTC_W , R_HYPB_CMD_WRITE16  |
                                                        R_HYPB_CMD_SET_SA   | 
                                                        R_HYPB_CMD_SET_DATA  },
    /* 3. Reset / ASO Exit  */
    { 0       , 0x00F0uL, LOC_HF_FLAG_W, LOC_HF_LTC_W , R_HYPB_CMD_WRITE16   },
};

/*******************************************************************************
  Variable: loc_FlashCmdReadCfg1

  This value stores the Hyper flash Read configuration register command sequence
  at the first time.
  At the first time, read from Non-Volatile Configuration Register.
  
*/

static const r_hypb_CmdTransaction_t  loc_FlashCmdReadCfg1[] = {  
    /* Address, Data    , CmdFlags     , Latency      , OpeFlags   */
    { 0x0555uL, 0x00AAuL, LOC_HF_FLAG_W, LOC_HF_LTC_W , R_HYPB_CMD_WRITE16 },
    { 0x02AAuL, 0x0055uL, LOC_HF_FLAG_W, LOC_HF_LTC_W , R_HYPB_CMD_WRITE16 },
    { 0x0555uL, 0x00C6uL, LOC_HF_FLAG_W, LOC_HF_LTC_W , R_HYPB_CMD_WRITE16 },
    { 0       , 0       , LOC_HF_FLAG_R, LOC_HF_LTC_R1, R_HYPB_CMD_READ16  }
};

/*******************************************************************************
  Variable: loc_FlashCmdReadCfg2

  This value stores the Hyper flash Read confiruration register command sequence
  after the second time.
  After the second time, read from Volatile Configuration Register.
  
*/

static const r_hypb_CmdTransaction_t  loc_FlashCmdReadCfg2[] = {  
    /* Address, Data    , CmdFlags     , Latency      , OpeFlags   */
    { 0x0555uL, 0x00AAuL, LOC_HF_FLAG_W, LOC_HF_LTC_W , R_HYPB_CMD_WRITE16 },
    { 0x02AAuL, 0x0055uL, LOC_HF_FLAG_W, LOC_HF_LTC_W , R_HYPB_CMD_WRITE16 },
    { 0x0555uL, 0x00C7uL, LOC_HF_FLAG_W, LOC_HF_LTC_W , R_HYPB_CMD_WRITE16 },
    { 0       , 0       , LOC_HF_FLAG_R, LOC_HF_LTC_R2, R_HYPB_CMD_READ16  }
};

/*******************************************************************************
  Variable: loc_FlashCmdWriteCfg

  This value stores the Hyper flash Write confiruration register command sequence
  Write to Volatile Configuration Register.
  
*/

static const r_hypb_CmdTransaction_t  loc_FlashCmdWriteCfg[]  = { 
    /* Address, Data    , CmdFlags     , Latency      , OpeFlags   */
    { 0x0555uL, 0x00AAuL, LOC_HF_FLAG_W, LOC_HF_LTC_W , R_HYPB_CMD_WRITE16  },
    { 0x02AAuL, 0x0055uL, LOC_HF_FLAG_W, LOC_HF_LTC_W , R_HYPB_CMD_WRITE16  },
    { 0x0555uL, 0x0038  , LOC_HF_FLAG_W, LOC_HF_LTC_W , R_HYPB_CMD_WRITE16  },
    { 0       , 0       , LOC_HF_FLAG_W, LOC_HF_LTC_W , R_HYPB_CMD_WRITE16 | 
                                                        R_HYPB_CMD_SET_DATA }
};


/*******************************************************************************
  Variable: loc_FlashCmdReadSts

  This value stores the Hyper falsh Read status register command sequence
  
*/

static const r_hypb_CmdTransaction_t  loc_FlashCmdReadSts[] = {  
    /* Address, Data    , CmdFlags     , Latency      , OpeFlags   */
    { 0x0555uL, 0x0070uL, LOC_HF_FLAG_W, LOC_HF_LTC_W , R_HYPB_CMD_WRITE16 },
    { 0       , 0       , LOC_HF_FLAG_R, LOC_HF_LTC_R2, R_HYPB_CMD_READ16  },
};

/*******************************************************************************
  Variable: r_hypdb_FlashCmdTbl

  This value stores the Hyper flash command table.
  
*/

const r_hypb_Command_t  r_hypdb_FlashCmdTbl = { 
    /* Code          , NextAcc, NagDelay, ClkDelay */
    { LOC_HF_LTC_CODE, 1      , 1       , 1        }, /* Latency     */
    
    /* Transaction                                  , Cycle                  */
    { (r_hypb_CmdTransaction_t*)loc_FlashCmdReset   , 2 }, /* Open           */
    { (r_hypb_CmdTransaction_t*)loc_FlashCmdReset   , 2 }, /* Close          */
    { (r_hypb_CmdTransaction_t*)loc_FlashCmdRead    , 1 }, /* Read           */
    { (r_hypb_CmdTransaction_t*)loc_FlashCmdWrite   , 4 }, /* Write          */
    { (r_hypb_CmdTransaction_t*)loc_FlashCmdWriteBuf, 6 }, /* WriteBufFlash  */
    { (r_hypb_CmdTransaction_t*)loc_FlashCmdErase   , 6 }, /* Erase          */
    { (r_hypb_CmdTransaction_t*)loc_FlashCmdProtect , 6 }, /* Protect        */
    { (r_hypb_CmdTransaction_t*)loc_FlashCmdReadCfg1, 4 }, /* ReadCfg1       */
    { (r_hypb_CmdTransaction_t*)loc_FlashCmdReadCfg2, 4 }, /* ReadCfg2       */
    { (r_hypb_CmdTransaction_t*)loc_FlashCmdWriteCfg, 4 }, /* WriteCfg1      */
    { (r_hypb_CmdTransaction_t*)loc_FlashCmdWriteCfg, 4 }, /* WriteCfg2      */
    { (r_hypb_CmdTransaction_t*)loc_FlashCmdReadSts , 2 }, /* ReadSts        */
};

/***************************************************************************
  Section: Global Variables for Hyper RAM 
*/

/*******************************************************************************
  Variable: loc_RamCmdRead

  This value stores the Hyper RAM Read command sequence.
*/
static const r_hypb_CmdTransaction_t  loc_RamCmdRead[] = {
    /* Address, Data    , CmdFlags         , Latency          , OpeFlags   */
    { 0       , 0       , LOC_HR_FLAG_MEM_R, LOC_HR_LTC_MEM_R , R_HYPB_CMD_READ64  | 
                                                                R_HYPB_CMD_SET_ADDR },
};

/*******************************************************************************
  Variable: loc_RamCmdReadCfg1

  This value stores the Hyper RAM Read confiruration register command sequence.
  Read from Configuration Register of die0.
  
*/
static const r_hypb_CmdTransaction_t  loc_RamCmdReadCfg1[] = {
    /* Address, Data    , CmdFlags         , Latency          , OpeFlags   */
    { 0x0800uL, 0       , LOC_HR_FLAG_REG_R, LOC_HR_LTC_REG_R1, R_HYPB_CMD_READ16 },
};

/*******************************************************************************
  Variable: loc_RamCmdReadCfg2

  This value stores the Hyper RAM Read confiruration register command sequence.
  Read from Configuration Register of die0.
  
*/
static const r_hypb_CmdTransaction_t  loc_RamCmdReadCfg2[] = {
    /* Address, Data    , CmdFlags         , Latency          , OpeFlags   */
    { 0x0800uL, 0       , LOC_HR_FLAG_REG_R, LOC_HR_LTC_REG_R2, R_HYPB_CMD_READ16 },
};

/*******************************************************************************
  Variable: loc_RamCmdWriteCfg

  This value stores the Hyper RAM Write confiruration register command sequence.
  Write to Configuration Register of die0 and die1.
  
*/
static const r_hypb_CmdTransaction_t  loc_RamCmdWriteCfg[] = {
    /* Address  , Data    , CmdFlags         , Latency          , OpeFlags   */
    { 0x000800uL, 0       , LOC_HR_FLAG_REG_W, LOC_HR_LTC_REG_W , R_HYPB_CMD_WRITE16 | 
                                                                  R_HYPB_CMD_SET_DATA },
    { 0x400800uL, 0       , LOC_HR_FLAG_REG_W, LOC_HR_LTC_REG_W , R_HYPB_CMD_WRITE16 | 
                                                                  R_HYPB_CMD_SET_DATA },
};

/*******************************************************************************
  Variable: r_hypdb_RamCmdTabl

  This value stores the Hyper RAM command table.
  
*/
const r_hypb_Command_t  r_hypdb_RamCmdTabl =
{
    /* Code          , NextAcc, NagDelay, ClkDelay */
    { LOC_HR_LTC_CODE, 1      , 1       , 1         }, /* Latency     */
    
    /* Transaction                                , Cycle                    */
    { (r_hypb_CmdTransaction_t*)R_NULL            , 0 }, /* Open             */
    { (r_hypb_CmdTransaction_t*)R_NULL            , 0 }, /* Close            */
    { (r_hypb_CmdTransaction_t*)loc_RamCmdRead    , 1 }, /* * Read           */
    { (r_hypb_CmdTransaction_t*)R_NULL            , 0 }, /* * Write          */
    { (r_hypb_CmdTransaction_t*)R_NULL            , 0 }, /* * WriteBufFlash  */
    { (r_hypb_CmdTransaction_t*)R_NULL            , 0 }, /* * Erase          */
    { (r_hypb_CmdTransaction_t*)R_NULL            , 0 }, /* * Protect        */
    { (r_hypb_CmdTransaction_t*)loc_RamCmdReadCfg1, 1 }, /* * ReadCfg1       */
    { (r_hypb_CmdTransaction_t*)loc_RamCmdReadCfg2, 1 }, /* * ReadCfg2       */
    { (r_hypb_CmdTransaction_t*)loc_RamCmdWriteCfg, 2 }, /* * WriteCfg1      */
    { (r_hypb_CmdTransaction_t*)loc_RamCmdWriteCfg, 2 }, /* * WriteCfg2      */
    { (r_hypb_CmdTransaction_t*)R_NULL            , 0 }, /* * ReadSts        */
};


/* End Of file */
