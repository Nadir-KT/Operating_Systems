/****************************************************************************
 *  License : All rights reserved for TES Electronic Solutions GmbH
 *            See included /docs/license.txt for details
 *  Project : D/AVE HD
 *  Purpose : HW Register Interface defines for STC (Group 0) registers
 ****************************************************************************
 * Version Control Information :
 *  $Revision: 3935 $
 *  $Date: 2014-09-23 17:52:32 +0900 (火, 23 9 2014) $
 *  $LastChangedBy: golczewskim $
 ****************************************************************************
 * Change History (autogenerated):
 ****************************************************************************/   

#ifndef DAVEHD_STCREGISTERMAP_H_INCLUDED
#define DAVEHD_STCREGISTERMAP_H_INCLUDED

#include "davehd_types.h"

/*----------------------------------------------------------------------------------------------------------*/
/* All register offsets                                                                                     */
/*                                                                                                          */
            
#define DHD_REG_STC_VERSION                  0x00   /* Hardware Version Information*/
#define DHD_REG_STC_CONFIG_1                 0x01   /* Hardware Configuration 1*/
#define DHD_REG_STC_CONFIG_2                 0x02   /* Hardware Configuration 2*/
#define DHD_REG_STC_CONFIG_3                 0x03   /* Hardware Configuration 3*/
#define DHD_REG_STC_BUSY_STATUS              0x04   
#define DHD_REG_STC_CONTROL                  0x05
#define DHD_REG_STC_INTERRUPT_STATUS         0x06
#define DHD_REG_STC_INTERRUPT_CLEAR          0x06
#define DHD_REG_STC_STREAM_ADDRESS           0x07
#define DHD_REG_STC_RING_PAUSE_ADDRESS       0x08
#define DHD_REG_STC_CURRENT_RING_ADDRESS     0x09
#define DHD_REG_STC_CURRENT_STREAM_ADDRESS   0x0A
#define DHD_REG_STC_CHECKSUM                 0x0B
#define DHD_REG_STC_SYNC_ID_1                0x0C   /* Synchronization ID for driver usage (used for jobid)*/
#define DHD_REG_STC_SYNC_ID_2                0x0D   /* Synchronization ID for driver usage (used for clrid)*/
#define DHD_REG_STC_SYNC_ID_3                0x0E   /* Synchronization ID for driver usage (used for arbitrary writes)*/
#define DHD_REG_STC_CALL_STACK_POINTER       0x0F
#define DHD_REG_STC_CALL_STACK_ENTRY_0       0x10   
#define DHD_REG_STC_CALL_STACK_ENTRY_1       0x12
#define DHD_REG_STC_CALL_STACK_ENTRY_2       0x14
#define DHD_REG_STC_CALL_STACK_ENTRY_3       0x16
#define DHD_REG_STC_CALL_STACK_ENTRY_4       0x18
#define DHD_REG_STC_CALL_STACK_ENTRY_5       0x1A
#define DHD_REG_STC_CALL_STACK_ENTRY_6       0x1C
#define DHD_REG_STC_CALL_STACK_ENTRY_7       0x1E
#define DHD_REG_STC_PERFORMANCE_COUNTER      0x20  /* 0x20 .. 0x3f up to 32 performance counter values*/

#define DHD_STC_REGFILE_SIZE                 64    /* number of stc registers*/

/*----------------------------------------------------------------------------------------------------------*/
/* All register layouts                                                                                     */
/*                                                                                                          */

/*----------------------------------------------------------------------------------------------------------*/
/* REG_STC_Version layout ( Hardware Version Information )*/
/**/
typedef union tagdhd_reg_stc_version_t {
  struct tagdhd_reg_stc_version_fields_t {
    dhd_uint32_t m_hw_revision    : 8;    /* HW revision (for bug fixes etc.)*/
    dhd_uint32_t m_hw_min_version : 8;    /* HW version minor*/
    dhd_uint32_t m_hw_maj_version : 8;    /* HW version major*/
    dhd_uint32_t m_dave_id        : 8;    /* magic constant 0xD4                              */
  } m_fields;
  dhd_uint32_t m_value;
} dhd_reg_stc_version_t;

/*----------------------------------------------------------------------------------------------------------*/
/* REG_STC_Config_1 layout ( Hardware Configuration 1 )*/
/**/
typedef union tagdhd_reg_stc_config_1_t {
  struct tagdhd_reg_stc_config_1_fields_t {
    dhd_uint32_t m_mbi_num_bytes     : 4;    /* Log2 of number of bytes in MBI data*/
    dhd_uint32_t m_str_line_length   : 4;    /* Length of line in the STR FIFO*/
    dhd_uint32_t m_str_num_lines     : 4;    /* Number of lines in the STR FIFO*/
    dhd_uint32_t m_reserved1         : 4;    
    dhd_uint32_t m_max_texture_units : 8;    /* Available number of texture units*/
    dhd_uint32_t m_max_color_passes  : 8;    /* Available number of color passes*/
  } m_fields;
  dhd_uint32_t m_value;
} dhd_reg_stc_config_1_t;

/*----------------------------------------------------------------------------------------------------------*/
/* REG_STC_Config_2 layout ( Hardware Configuration 2 )*/
/**/
typedef union tagdhd_reg_stc_config_2_t {
  struct tagdhd_reg_stc_config_2_fields_t {    
    dhd_uint32_t m_num_const_colors  : 8;    /* Number of constant color registers*/
    dhd_uint32_t m_num_pixel_pipes   : 8;    /* Number of pixel pipelines*/
    dhd_uint32_t m_num_limiters      : 8;    /* Number of Limiters*/
    dhd_uint32_t m_num_perf_counters : 8;    /* Number of performance counter units*/
  } m_fields;
  dhd_uint32_t m_value;
} dhd_reg_stc_config_2_t;

/*----------------------------------------------------------------------------------------------------------*/
/* REG_STC_Config_3 layout ( Hardware Configuration 3 )*/
/**/
typedef union tagdhd_reg_stc_config_3_t {
  struct tagdhd_reg_stc_config_3_fields_t {    
    dhd_uint32_t m_tex_clut_available   : 1;    /* A CLUT is available in the texel processing unit*/
    dhd_uint32_t m_tex_clut_has_alpha   : 1;    /* The CLUT contains full ARGB32 values instead of RGB24 only*/
    dhd_uint32_t m_reserved1            : 2;    
    dhd_uint32_t m_tex_clut_depth       : 4;    /* Depth (number of entries) of the texture CLUT*/
    dhd_uint32_t m_stc_call_stack_depth : 8;    /* Depth of stream controller call stack*/
    dhd_uint32_t m_reserved2            : 16;    
  } m_fields;
  dhd_uint32_t m_value;
} dhd_reg_stc_config_3_t;


/*----------------------------------------------------------------------------------------------------------*/
/* REG_STC_Control layout */
/**/
typedef union tagdhd_reg_stc_control_t {
  struct tagdhd_reg_stc_control_fields_t {    
    dhd_uint32_t m_stop_irq_enable               : 1;  /* */
    dhd_uint32_t m_pause_irq_enable              : 1;  
    dhd_uint32_t m_stall_irq_enable              : 1;  
    dhd_uint32_t m_mbierror_irq_enable           : 1;  
    dhd_uint32_t m_sync_irq_enable               : 1;    
    dhd_uint32_t m_reserved1                     : 3;  
    dhd_uint32_t m_permanent_enable_clock        : 1;  /* disable clock gating when set*/
    dhd_uint32_t m_reserved2                     : 1;   
    dhd_uint32_t m_checksum_protection_mode      : 1;
    dhd_uint32_t m_reserved3                     : 1;
    dhd_uint32_t m_stall_request                 : 1;  /**/
    dhd_uint32_t m_stream_halt_request           : 1;
    dhd_uint32_t m_limit_prefetch_in_ring        : 1;
    dhd_uint32_t m_reserved4                     : 17;    
  } m_fields;
  dhd_uint32_t m_value;
} dhd_reg_stc_control_t;


/*----------------------------------------------------------------------------------------------------------*/
/* REG_STC_Control layout */
/**/
typedef union tagdhd_reg_stc_interrupt_status_t {
  struct tagdhd_reg_stc_interrupt_status_fields_t {    
    dhd_uint32_t m_stop_irq               : 1;   
    dhd_uint32_t m_pause_irq              : 1;  
    dhd_uint32_t m_stall_irq              : 1;  
    dhd_uint32_t m_mbierror_irq           : 1;  
    dhd_uint32_t m_sync_irq               : 1;    
    dhd_uint32_t m_reserved1              : 3;    
    dhd_uint32_t m_stop_code              : 3;    
    dhd_uint32_t m_reserved2              : 5;    
    dhd_uint32_t m_mbiread_error          : 1;  
    dhd_uint32_t m_mbiread_error_source   : 3;  
    dhd_uint32_t m_mbiwrite_error         : 1;  
    dhd_uint32_t m_mbiwrite_error_source  : 3;  
    dhd_uint32_t m_reserved3              : 8;    
  } m_fields;
  dhd_uint32_t m_value;
} dhd_reg_stc_interrupt_status_t;



/*----------------------------------------------------------------------------------------------------------*/
/* STC Register file*/
/**/
typedef union tagdhd_reg_stc_registerfile_t {
  struct tagdhd_reg_stc_regfile_fields_t {
    dhd_reg_stc_version_t          m_stc_version;
    dhd_reg_stc_config_1_t         m_stc_config1;
    dhd_reg_stc_config_2_t         m_stc_config2;
    dhd_reg_stc_config_3_t         m_stc_config3;
    dhd_uint32_t                   m_stc_busy_status;
    dhd_reg_stc_control_t          m_stc_control;
    dhd_reg_stc_interrupt_status_t m_stc_interrupt_status;
    dhd_uint32_t                   m_stc_stream_address;
    dhd_uint32_t                   m_stc_ring_pause_address;
    dhd_uint32_t                   m_stc_current_ring_address;
    dhd_uint32_t                   m_stc_current_stream_address;
    dhd_uint32_t                   m_stc_checksum;
    dhd_uint32_t                   m_stc_sync_id[3];
    dhd_uint32_t                   m_stc_call_stack_pointer;
    dhd_uint32_t                   m_stc_call_stack_entry[16];
    dhd_uint32_t                   m_stc_perf_count_value[32];
  } m_named;
  dhd_uint32_t m_all[ DHD_STC_REGFILE_SIZE ];
} dhd_reg_stc_registerfile_t;



#endif /*DAVEHD_STCREGISTERMAP_H_INCLUDED*/

