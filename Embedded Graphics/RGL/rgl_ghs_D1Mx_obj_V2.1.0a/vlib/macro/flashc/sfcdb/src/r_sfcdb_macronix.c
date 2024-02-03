/*
****************************************************************************
PROJECT : SFMA driver
FILE    : $Id: r_sfcdb_macronix.c 9233 2016-06-30 09:18:10Z florian.zimmermann $
============================================================================
DESCRIPTION
Serial flash command table for a macronix flash device
============================================================================
                            C O P Y R I G H T
============================================================================
                           Copyright (c) 2013 - 2014
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
  Title: Serial flash command table

  Note: This file defines the macro define and the flash command table for
        Macronix MX25L51245G serial flash device.
        If you use other serial flash device of Macronix, change setting
        parameters according to the model number of the target.

*/


/***************************************************************************
  Section: Includes
*/
#include "r_typedefs.h"
#include "r_sfma_api.h"

/***************************************************************************
  Section: Global Variables
*/
const r_sfma_FlashCommand_t r_sfc_Macronix_32bitAddrCmd_40MHz =
{
    {                            /* Register infomation (QuadIoMode)      */
        R_SFMA_STATUS_REG,       /* Register                              */
        0x40,                    /* Bit position                          */
    },
    {                            /* Register infomation (BlockProtect)    */
        R_SFMA_STATUS_REG,       /* Register                              */
        0x3C,                    /* Bit position                          */
    },
    {                            /* Register infomation (WriteInProgress) */
        R_SFMA_STATUS_REG,       /* Register                              */
        0x01,                    /* Bit position                          */
    },
    {                            /* Register Set (DummyCycle)             */
        R_SFMA_CONFIG_REG,       /* Register                              */
        0xC0,                    /* Bit Mask                              */
        0x40,                    /* Bit Set                               */
    },
    R_SFMA_DUMMY_6CYC,           /* ReadSdrDummyCycle         */
    R_SFMA_DUMMY_6CYC,           /* ReadSdrIoDualDummyCycle   */
    R_SFMA_DUMMY_4CYC,           /* ReadSdrIoQuadDummyCycle   */
    R_SFMA_DUMMY_6CYC,           /* ReadDdrDummyCycle         */
    R_SFMA_DUMMY_6CYC,           /* ReadDdrIoDualDummyCycle   */
    R_SFMA_DUMMY_4CYC,           /* ReadDdrIoQuadDummyCycle   */
    R_SFMA_ADDRESS_SIZE_1BIT,    /* Address bit size          */
    0x0C,                        /* ReadSdr        */
    0x3C,                        /* ReadSdrDual    */
    0x6C,                        /* ReadSdrQuad    */
    0xBC,                        /* ReadSdrIoDual  */
    0xEC,                        /* ReadSdrIoQuad  */
    0x0E,                        /* ReadDdr        */
    0xFF,                        /* ReadDdrDual    */
    0xFF,                        /* ReadDdrQuad    */
    0xBE,                        /* ReadDdrIoDual  */
    0xEE,                        /* ReadDdrioQuad  */
    0x12,                        /* WriteSdr       */
    0xFF,                        /* WriteSdrDual   */
    0x3E,                        /* WriteSdrQuad   */
    0xFF,                        /* WriteDdr       */
    0xFF,                        /* WriteDdrDual   */
    0xFF,                        /* WriteDdrQuad   */
    0x06,                        /* WriteEnable    */
    0x21,                        /* Erase          */
    0x05,                        /* ReadStatus1    */
    0x15,                        /* ReadStatus2    */
    0x01,                        /* WriteStatus    */
    0xFF,                        /* Exit external address space */
    0x66,                        /* ResetEnable    */
    0x99,                         /* Reset          */
    0xA5                         /* Performance enhance mode indicator */
};


const r_sfma_FlashCommand_t r_sfc_Macronix_32bitAddrCmd_80MHz =
{
    {                            /* Register infomation (QuadIoMode)      */
        R_SFMA_STATUS_REG,       /* Register                              */
        0x40,                    /* Bit position                          */
    },
    {                            /* Register infomation (BlockProtect)    */
        R_SFMA_STATUS_REG,       /* Register                              */
        0x3C,                    /* Bit position                          */
    },
    {                            /* Register infomation (WriteInProgress) */
        R_SFMA_STATUS_REG,       /* Register                              */
        0x01,                    /* Bit position                          */
    },
    {                            /* Register Set (DummyCycle)             */
        R_SFMA_CONFIG_REG,       /* Register                              */
        0xC0,                    /* Bit Mask                              */
        0x80,                    /* Bit Set                               */
    },
    R_SFMA_DUMMY_8CYC,           /* ReadSdrDummyCycle         */
    R_SFMA_DUMMY_8CYC,           /* ReadSdrIoDualDummyCycle   */
    R_SFMA_DUMMY_8CYC,           /* ReadSdrIoQuadDummyCycle   */
    R_SFMA_DUMMY_8CYC,           /* ReadDdrDummyCycle         */
    R_SFMA_DUMMY_8CYC,           /* ReadDdrIoDualDummyCycle   */
    R_SFMA_DUMMY_8CYC,           /* ReadDdrIoQuadDummyCycle   */
    R_SFMA_ADDRESS_SIZE_1BIT,    /* Address bit size          */
    0x0C,                        /* ReadSdr        */
    0x3C,                        /* ReadSdrDual    */
    0x6C,                        /* ReadSdrQuad    */
    0xBC,                        /* ReadSdrIoDual  */
    0xEC,                        /* ReadSdrIoQuad  */
    0x0E,                        /* ReadDdr        */
    0xFF,                        /* ReadDdrDual    */
    0xFF,                        /* ReadDdrQuad    */
    0xBE,                        /* ReadDdrIoDual  */
    0xEE,                        /* ReadDdrioQuad  */
    0x12,                        /* WriteSdr       */
    0xFF,                        /* WriteSdrDual   */
    0x3E,                        /* WriteSdrQuad   */
    0xFF,                        /* WriteDdr       */
    0xFF,                        /* WriteDdrDual   */
    0xFF,                        /* WriteDdrQuad   */
    0x06,                        /* WriteEnable    */
    0x21,                        /* Erase          */
    0x05,                        /* ReadStatus1    */
    0x15,                        /* ReadStatus2    */
    0x01,                        /* WriteStatus    */
    0xFF,                        /* Exit external address space */
    0x66,                        /* ResetEnable    */
    0x99,                        /* Reset          */
    0xA5                         /* Performance enhance mode indicator */
};


const r_sfma_FlashCommand_t r_sfc_Macronix_24bitAddrCmd_40MHz =
{
    {                            /* Register infomation (QuadIoMode)      */
        R_SFMA_STATUS_REG,       /* Register                              */
        0x40,                    /* Bit position                          */
    },
    {                            /* Register infomation (BlockProtect)    */
        R_SFMA_STATUS_REG,       /* Register                              */
        0x3C,                    /* Bit position                          */
    },
    {                            /* Register infomation (WriteInProgress) */
        R_SFMA_STATUS_REG,       /* Register                              */
        0x01,                    /* Bit position                          */
    },
    {                            /* Register Set (DummyCycle)             */
        R_SFMA_CONFIG_REG,       /* Register                              */
        0xC0,                    /* Bit Mask                              */
        0x40,                    /* Bit Set                               */
    },
    R_SFMA_DUMMY_6CYC,           /* ReadSdrDummyCycle         */
    R_SFMA_DUMMY_6CYC,           /* ReadSdrIoDualDummyCycle   */
    R_SFMA_DUMMY_4CYC,           /* ReadSdrIoQuadDummyCycle   */
    R_SFMA_DUMMY_6CYC,           /* ReadDdrDummyCycle         */
    R_SFMA_DUMMY_6CYC,           /* ReadDdrIoDualDummyCycle   */
    R_SFMA_DUMMY_4CYC,           /* ReadDdrIoQuadDummyCycle   */
    R_SFMA_ADDRESS_SIZE_1BIT,    /* Address bit size          */
    0x0B,                        /* ReadSdr        */
    0x3B,                        /* ReadSdrDual    */
    0x6B,                        /* ReadSdrQuad    */
    0xBB,                        /* ReadSdrIoDual  */
    0xEB,                        /* ReadSdrIoQuad  */
    0x0D,                        /* ReadDdr        */
    0xFF,                        /* ReadDdrDual    */
    0xFF,                        /* ReadDdrQuad    */
    0xBD,                        /* ReadDdrIoDual  */
    0xED,                        /* ReadDdrioQuad  */
    0x02,                        /* WriteSdr       */
    0xFF,                        /* WriteSdrDual   */
    0x38,                        /* WriteSdrQuad   */
    0xFF,                        /* WriteDdr       */
    0xFF,                        /* WriteDdrDual   */
    0xFF,                        /* WriteDdrQuad   */
    0x06,                        /* WriteEnable    */
    0x20,                        /* Erase          */
    0x05,                        /* ReadStatus1    */
    0x15,                        /* ReadStatus2    */
    0x01,                        /* WriteStatus    */
    0xFF,                        /* Exit external address space */
    0x66,                        /* ResetEnable    */
    0x99,                        /* Reset          */
    0xA5                         /* Performance enhance mode indicator */
};


const r_sfma_FlashCommand_t r_sfc_Macronix_24bitAddrCmd_80MHz =
{
    {                            /* Register infomation (QuadIoMode)      */
        R_SFMA_STATUS_REG,       /* Register                              */
        0x40,                    /* Bit position                          */
    },
    {                            /* Register infomation (BlockProtect)    */
        R_SFMA_STATUS_REG,       /* Register                              */
        0x3C,                    /* Bit position                          */
    },
    {                            /* Register infomation (WriteInProgress) */
        R_SFMA_STATUS_REG,       /* Register                              */
        0x01,                    /* Bit position                          */
    },
    {                            /* Register Set (DummyCycle)             */
        R_SFMA_CONFIG_REG,       /* Register                              */
        0xC0,                    /* Bit Mask                              */
        0x80,                    /* Bit Set                               */
    },
    R_SFMA_DUMMY_8CYC,           /* ReadSdrDummyCycle         */
    R_SFMA_DUMMY_8CYC,           /* ReadSdrIoDualDummyCycle   */
    R_SFMA_DUMMY_8CYC,           /* ReadSdrIoQuadDummyCycle   */
    R_SFMA_DUMMY_8CYC,           /* ReadDdrDummyCycle         */
    R_SFMA_DUMMY_8CYC,           /* ReadDdrIoDualDummyCycle   */
    R_SFMA_DUMMY_8CYC,           /* ReadDdrIoQuadDummyCycle   */
    R_SFMA_ADDRESS_SIZE_1BIT,    /* Address bit size          */
    0x0B,                        /* ReadSdr        */
    0x3B,                        /* ReadSdrDual    */
    0x6B,                        /* ReadSdrQuad    */
    0xBB,                        /* ReadSdrIoDual  */
    0xEB,                        /* ReadSdrIoQuad  */
    0x0D,                        /* ReadDdr        */
    0xFF,                        /* ReadDdrDual    */
    0xFF,                        /* ReadDdrQuad    */
    0xBD,                        /* ReadDdrIoDual  */
    0xED,                        /* ReadDdrioQuad  */
    0x02,                        /* WriteSdr       */
    0xFF,                        /* WriteSdrDual   */
    0x38,                        /* WriteSdrQuad   */
    0xFF,                        /* WriteDdr       */
    0xFF,                        /* WriteDdrDual   */
    0xFF,                        /* WriteDdrQuad   */
    0x06,                        /* WriteEnable    */
    0x20,                        /* Erase          */
    0x05,                        /* ReadStatus1    */
    0x15,                        /* ReadStatus2    */
    0x01,                        /* WriteStatus    */
    0xFF,                        /* Exit external address space */
    0x66,                        /* ResetEnable    */
    0x99,                        /* Reset          */
    0xA5                         /* Performance enhance mode indicator */
};

/* changed to 8 dummy cycles since this configuration is shared 

    Macronix R_SFMA_CONFIG_REG 0x40 can be either 4 or 6 dummy cycles depending on
    the mode.
*/
const r_sfma_FlashCommand_t r_sfc_Macronix_32bitAddrQuadCmd_40MHz =
{
    {                            /* Register infomation (QuadIoMode)      */
        R_SFMA_STATUS_REG,       /* Register                              */
        0x40,                    /* Bit position                          */
    },
    {                            /* Register infomation (BlockProtect)    */
        R_SFMA_STATUS_REG,       /* Register                              */
        0x3C,                    /* Bit position                          */
    },
    {                            /* Register infomation (WriteInProgress) */
        R_SFMA_STATUS_REG,       /* Register                              */
        0x01,                    /* Bit position                          */
    },
    {                            /* Register Set (DummyCycle)             */
        R_SFMA_CONFIG_REG,       /* Register                              */
        0xC0,                    /* Bit Mask                              */
        0x80,                    /* Bit Set                               */
    },
    R_SFMA_DUMMY_8CYC,           /* ReadSdrDummyCycle         */
    R_SFMA_DUMMY_8CYC,           /* ReadSdrIoDualDummyCycle   */
    R_SFMA_DUMMY_8CYC,           /* ReadSdrIoQuadDummyCycle   */
    R_SFMA_DUMMY_8CYC,           /* ReadDdrDummyCycle         */
    R_SFMA_DUMMY_8CYC,           /* ReadDdrIoDualDummyCycle   */
    R_SFMA_DUMMY_8CYC,           /* ReadDdrIoQuadDummyCycle   */
    R_SFMA_ADDRESS_SIZE_4BIT,    /* Address bit size          */
    0x0C,                        /* ReadSdr        */
    0x3C,                        /* ReadSdrDual    */
    0x6C,                        /* ReadSdrQuad    */
    0xBC,                        /* ReadSdrIoDual  */
    0xEC,                        /* ReadSdrIoQuad  */
    0x0E,                        /* ReadDdr        */
    0xFF,                        /* ReadDdrDual    */
    0xFF,                        /* ReadDdrQuad    */
    0xBE,                        /* ReadDdrIoDual  */
    0xEE,                        /* ReadDdrioQuad  */
    0x12,                        /* WriteSdr       */
    0xFF,                        /* WriteSdrDual   */
    0x3E,                        /* WriteSdrQuad   */
    0xFF,                        /* WriteDdr       */
    0xFF,                        /* WriteDdrDual   */
    0xFF,                        /* WriteDdrQuad   */
    0x06,                        /* WriteEnable    */
    0x21,                        /* Erase          */
    0x05,                        /* ReadStatus1    */
    0x15,                        /* ReadStatus2    */
    0x01,                        /* WriteStatus    */
    0xFF,                        /* Exit external address space */
    0x66,                        /* ResetEnable    */
    0x99,                        /* Reset          */
    0xA5                         /* Performance enhance mode indicator */
};


const r_sfma_FlashCommand_t r_sfc_Macronix_32bitAddrQuadCmd_80MHz =
{
    {                            /* Register infomation (QuadIoMode)      */
        R_SFMA_STATUS_REG,       /* Register                              */
        0x40,                    /* Bit position                          */
    },
    {                            /* Register infomation (BlockProtect)    */
        R_SFMA_STATUS_REG,       /* Register                              */
        0x3C,                    /* Bit position                          */
    },
    {                            /* Register infomation (WriteInProgress) */
        R_SFMA_STATUS_REG,       /* Register                              */
        0x01,                    /* Bit position                          */
    },
    {                            /* Register Set (DummyCycle)             */
        R_SFMA_CONFIG_REG,       /* Register                              */
        0xC0,                    /* Bit Mask                              */
        0x80,                    /* Bit Set                               */
    },
    R_SFMA_DUMMY_8CYC,           /* ReadSdrDummyCycle         */
    R_SFMA_DUMMY_8CYC,           /* ReadSdrIoDualDummyCycle   */
    R_SFMA_DUMMY_8CYC,           /* ReadSdrIoQuadDummyCycle   */
    R_SFMA_DUMMY_8CYC,           /* ReadDdrDummyCycle         */
    R_SFMA_DUMMY_8CYC,           /* ReadDdrIoDualDummyCycle   */
    R_SFMA_DUMMY_8CYC,           /* ReadDdrIoQuadDummyCycle   */
    R_SFMA_ADDRESS_SIZE_4BIT,    /* Address bit size          */
    0x0C,                        /* ReadSdr        */
    0x3C,                        /* ReadSdrDual    */
    0x6C,                        /* ReadSdrQuad    */
    0xBC,                        /* ReadSdrIoDual  */
    0xEC,                        /* ReadSdrIoQuad  */
    0x0E,                        /* ReadDdr        */
    0xFF,                        /* ReadDdrDual    */
    0xFF,                        /* ReadDdrQuad    */
    0xBE,                        /* ReadDdrIoDual  */
    0xEE,                        /* ReadDdrioQuad  */
    0x12,                        /* WriteSdr       */
    0xFF,                        /* WriteSdrDual   */
    0x3E,                        /* WriteSdrQuad   */
    0xFF,                        /* WriteDdr       */
    0xFF,                        /* WriteDdrDual   */
    0xFF,                        /* WriteDdrQuad   */
    0x06,                        /* WriteEnable    */
    0x21,                        /* Erase          */
    0x05,                        /* ReadStatus1    */
    0x15,                        /* ReadStatus2    */
    0x01,                        /* WriteStatus    */
    0xFF,                        /* Exit external address space */
    0x66,                        /* ResetEnable    */
    0x99,                        /* Reset          */
    0xA5                         /* Performance enhance mode indicator */
};

const r_sfma_FlashCommand_t r_sfc_Macronix_24bitAddrQuadCmd_40MHz =
{
    {                            /* Register infomation (QuadIoMode)      */
        R_SFMA_STATUS_REG,       /* Register                              */
        0x40,                    /* Bit position                          */
    },
    {                            /* Register infomation (BlockProtect)    */
        R_SFMA_STATUS_REG,       /* Register                              */
        0x3C,                    /* Bit position                          */
    },
    {                            /* Register infomation (WriteInProgress) */
        R_SFMA_STATUS_REG,       /* Register                              */
        0x01,                    /* Bit position                          */
    },
    {                            /* Register Set (DummyCycle)             */
        R_SFMA_CONFIG_REG,       /* Register                              */
        0xC0,                    /* Bit Mask                              */
        0x40,                    /* Bit Set                               */
    },
    R_SFMA_DUMMY_6CYC,           /* ReadSdrDummyCycle         */
    R_SFMA_DUMMY_6CYC,           /* ReadSdrIoDualDummyCycle   */
    R_SFMA_DUMMY_4CYC,           /* ReadSdrIoQuadDummyCycle   */
    R_SFMA_DUMMY_6CYC,           /* ReadDdrDummyCycle         */
    R_SFMA_DUMMY_6CYC,           /* ReadDdrIoDualDummyCycle   */
    R_SFMA_DUMMY_4CYC,           /* ReadDdrIoQuadDummyCycle   */
    R_SFMA_ADDRESS_SIZE_4BIT,    /* Address bit size          */
    0x0B,                        /* ReadSdr        */
    0x3B,                        /* ReadSdrDual    */
    0x6B,                        /* ReadSdrQuad    */
    0xBB,                        /* ReadSdrIoDual  */
    0xEB,                        /* ReadSdrIoQuad  */
    0x0D,                        /* ReadDdr        */
    0xFF,                        /* ReadDdrDual    */
    0xFF,                        /* ReadDdrQuad    */
    0xBD,                        /* ReadDdrIoDual  */
    0xED,                        /* ReadDdrioQuad  */
    0x02,                        /* WriteSdr       */
    0xFF,                        /* WriteSdrDual   */
    0x38,                        /* WriteSdrQuad   */
    0xFF,                        /* WriteDdr       */
    0xFF,                        /* WriteDdrDual   */
    0xFF,                        /* WriteDdrQuad   */
    0x06,                        /* WriteEnable    */
    0x20,                        /* Erase          */
    0x05,                        /* ReadStatus1    */
    0x15,                        /* ReadStatus2    */
    0x01,                        /* WriteStatus    */
    0xFF,                        /* Exit external address space */
    0x66,                        /* ResetEnable    */
    0x99,                        /* Reset          */
    0xA5                         /* Performance enhance mode indicator */
};

const r_sfma_FlashCommand_t r_sfc_Macronix_24bitAddrQuadCmd_80MHz =
{
    {                            /* Register infomation (QuadIoMode)      */
        R_SFMA_STATUS_REG,       /* Register                              */
        0x40,                    /* Bit position                          */
    },
    {                            /* Register infomation (BlockProtect)    */
        R_SFMA_STATUS_REG,       /* Register                              */
        0x3C,                    /* Bit position                          */
    },
    {                            /* Register infomation (WriteInProgress) */
        R_SFMA_STATUS_REG,       /* Register                              */
        0x01,                    /* Bit position                          */
    },
    {                            /* Register Set (DummyCycle)             */
        R_SFMA_CONFIG_REG,       /* Register                              */
        0xC0,                    /* Bit Mask                              */
        0x80,                    /* Bit Set                               */
    },
    R_SFMA_DUMMY_8CYC,           /* ReadSdrDummyCycle         */
    R_SFMA_DUMMY_8CYC,           /* ReadSdrIoDualDummyCycle   */
    R_SFMA_DUMMY_8CYC,           /* ReadSdrIoQuadDummyCycle   */
    R_SFMA_DUMMY_8CYC,           /* ReadDdrDummyCycle         */
    R_SFMA_DUMMY_8CYC,           /* ReadDdrIoDualDummyCycle   */
    R_SFMA_DUMMY_8CYC,           /* ReadDdrIoQuadDummyCycle   */
    R_SFMA_ADDRESS_SIZE_4BIT,    /* Address bit size          */
    0x0B,                        /* ReadSdr        */
    0x3B,                        /* ReadSdrDual    */
    0x6B,                        /* ReadSdrQuad    */
    0xBB,                        /* ReadSdrIoDual  */
    0xEB,                        /* ReadSdrIoQuad  */
    0x0D,                        /* ReadDdr        */
    0xFF,                        /* ReadDdrDual    */
    0xFF,                        /* ReadDdrQuad    */
    0xBD,                        /* ReadDdrIoDual  */
    0xED,                        /* ReadDdrioQuad  */
    0x02,                        /* WriteSdr       */
    0xFF,                        /* WriteSdrDual   */
    0x38,                        /* WriteSdrQuad   */
    0xFF,                        /* WriteDdr       */
    0xFF,                        /* WriteDdrDual   */
    0xFF,                        /* WriteDdrQuad   */
    0x06,                        /* WriteEnable    */
    0x20,                        /* Erase          */
    0x05,                        /* ReadStatus1    */
    0x15,                        /* ReadStatus2    */
    0x01,                        /* WriteStatus    */
    0xFF,                        /* Exit external address space */
    0x66,                        /* ResetEnable    */
    0x99,                        /* Reset          */
    0xA5                         /* Performance enhance mode indicator */
};



/* Performance Enhance mode with 8 dummy cycles */
const r_sfma_FlashCommand_t r_sfc_Macronix_32bitAddrCmdPerformanceEnhance_40MHz =
{
    {                            /* Register infomation (QuadIoMode)      */
        R_SFMA_STATUS_REG,       /* Register                              */
        0x40,                    /* Bit position                          */
    },
    {                            /* Register infomation (BlockProtect)    */
        R_SFMA_STATUS_REG,       /* Register                              */
        0x3C,                    /* Bit position                          */
    },
    {                            /* Register infomation (WriteInProgress) */
        R_SFMA_STATUS_REG,       /* Register                              */
        0x01,                    /* Bit position                          */
    },
    {                            /* Register Set (DummyCycle)             */
        R_SFMA_CONFIG_REG,       /* Register                              */
        0xC0,                    /* Bit Mask                              */
        0x80,                    /* Bit Set 8 dummy cycles                */
    },
    R_SFMA_DUMMY_8CYC,           /* ReadSdrDummyCycle         Performance enhance not supported           */
    R_SFMA_DUMMY_8CYC,           /* ReadSdrIoDualDummyCycle   Performance enhance not supported           */
    R_SFMA_DUMMY_8CYC,           /* ReadSdrIoQuadDummyCycle   Performance enhance supported               */
    R_SFMA_DUMMY_8CYC,           /* ReadDdrDummyCycle         Performance enhance not supported           */
    R_SFMA_DUMMY_8CYC,           /* ReadDdrIoDualDummyCycle   Performance enhance not supported           */
    R_SFMA_DUMMY_8CYC,           /* ReadDdrIoQuadDummyCycle   Performance enhance supported               */
    R_SFMA_ADDRESS_SIZE_1BIT,    /* Address bit size          */
    0x0C,                        /* ReadSdr        */
    0x3C,                        /* ReadSdrDual    */
    0x6C,                        /* ReadSdrQuad    */
    0xBC,                        /* ReadSdrIoDual  */
    0xEC,                        /* ReadSdrIoQuad  */
    0x0E,                        /* ReadDdr        */
    0xFF,                        /* ReadDdrDual    */
    0xFF,                        /* ReadDdrQuad    */
    0xBE,                        /* ReadDdrIoDual  */
    0xEE,                        /* ReadDdrioQuad  */
    0x12,                        /* WriteSdr       */
    0xFF,                        /* WriteSdrDual   */
    0xFF,                        /* WriteSdrQuad   */
    0xFF,                        /* WriteDdr       */
    0xFF,                        /* WriteDdrDual   */
    0xFF,                        /* WriteDdrQuad   */
    0x06,                        /* WriteEnable    */
    0x21,                        /* Erase          */
    0x05,                        /* ReadStatus1    */
    0x15,                        /* ReadStatus2    */
    0x01,                        /* WriteStatus    */
    0xFF,                        /* Exit external address space */
    0x66,                        /* ResetEnable    */
    0x99,                        /* Reset          */
    0xA5                         /* Performance enhance mode indicator */
};


/* Performance Enhance mode with 10 dummy cycles */
const r_sfma_FlashCommand_t r_sfc_Macronix_32bitAddrCmdPerformanceEnhance_80MHz =
{
    {                            /* Register infomation (QuadIoMode)      */
        R_SFMA_STATUS_REG,       /* Register                              */
        0x40,                    /* Bit position                          */
    },
    {                            /* Register infomation (BlockProtect)    */
        R_SFMA_STATUS_REG,       /* Register                              */
        0x3C,                    /* Bit position                          */
    },
    {                            /* Register infomation (WriteInProgress) */
        R_SFMA_STATUS_REG,       /* Register                              */
        0x01,                    /* Bit position                          */
    },
    {                            /* Register Set (DummyCycle)             */
        R_SFMA_CONFIG_REG,       /* Register                              */
        0xC0,                    /* Bit Mask                              */
        0xC0 ,                   /* Bit Set  10 dummy cycles              */
    },
    R_SFMA_DUMMY_10CYC,          /* ReadSdrDummyCycle         Performance enhance not supported           */
    R_SFMA_DUMMY_10CYC,          /* ReadSdrIoDualDummyCycle   Performance enhance not supported           */
    R_SFMA_DUMMY_10CYC,          /* ReadSdrIoQuadDummyCycle   Performance enhance supported               */
    R_SFMA_DUMMY_10CYC,          /* ReadDdrDummyCycle         Performance enhance not supported           */
    R_SFMA_DUMMY_10CYC,          /* ReadDdrIoDualDummyCycle   Performance enhance not supported           */
    R_SFMA_DUMMY_10CYC,          /* ReadDdrIoQuadDummyCycle   Performance enhance supported               */
    R_SFMA_ADDRESS_SIZE_1BIT,    /* Address bit size (write)  */
    0x0C,                        /* ReadSdr        */
    0x3C,                        /* ReadSdrDual    */
    0x6C,                        /* ReadSdrQuad    */
    0xBC,                        /* ReadSdrIoDual  */
    0xEC,                        /* ReadSdrIoQuad  */
    0x0E,                        /* ReadDdr        */
    0xFF,                        /* ReadDdrDual    */
    0xFF,                        /* ReadDdrQuad    */
    0xBE,                        /* ReadDdrIoDual  */
    0xEE,                        /* ReadDdrioQuad  */
    0x12,                        /* WriteSdr       */
    0xFF,                        /* WriteSdrDual   */
    0x3E,                        /* WriteSdrQuad   */
    0xFF,                        /* WriteDdr       */
    0xFF,                        /* WriteDdrDual   */
    0xFF,                        /* WriteDdrQuad   */
    0x06,                        /* WriteEnable    */
    0x21,                        /* Erase          */
    0x05,                        /* ReadStatus1    */
    0x15,                        /* ReadStatus2    */
    0x01,                        /* WriteStatus    */
    0xFF,                        /* Exit external address space */
    0x66,                        /* ResetEnable    */
    0x99,                        /* Reset          */
    0xA5                         /* Performance enhance mode indicator */
};
