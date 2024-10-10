// Copyright 2024 MACKI, Krzysztof Gliwiński

#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

typedef enum {
  TMC_2209_REG_GCONF = 0x00,
  TMC_2209_REG_GSTAT = 0x01,
  TMC_2209_REG_IFCNT = 0x02,
  TMC_2209_REG_SLAVECONF = 0x03,
  TMC_2209_REG_OTP_PROG = 0x04,
  TMC_2209_REG_OTP_READ = 0x05,
  TMC_2209_REG_IOIN = 0x06,
  TMC_2209_REG_FACTORY_CONF = 0x07,

  TMC_2209_REG_IHOLD_IRUN = 0x10,
  TMC_2209_REG_TPOWERDOWN = 0x11,
  TMC_2209_REG_TSTEP = 0x12,
  TMC_2209_REG_TPWMTHRS = 0x13,
  TMC_2209_REG_VACTUAL = 0x22,
  TMC_2209_REG_TCOOLTHRS = 0x14,
  TMC_2209_REG_SGTHRS = 0x40,
  TMC_2209_REG_SG_RESULT = 0x41,
  TMC_2209_REG_COOLCONF = 0x42,

  TMC_2209_REG_MSCNT = 0x6A,
  TMC_2209_REG_MSCURACT = 0x6B,
  TMC_2209_REG_CHOPCONF = 0x6C,
  TMC_2209_REG_DRV_STATUS = 0x6F,
  TMC_2209_REG_PWMCONF = 0x70,
  TMC_2209_REG_PWM_SCALE = 0x71,
  TMC_2209_REG_PWM_AUTO = 0x72,
  TMC_2209_REG_LAST_ADDR = TMC_2209_REG_PWM_AUTO
} tmc2209_registers_t;

typedef union {
  struct {
    uint8_t I_scale_analog : 1;
    uint8_t internal_Rsense : 1;
    uint8_t en_spreadCycle : 1;
    uint8_t shaft : 1;
    uint8_t index_otpw : 1;
    uint8_t index_step : 1;
    uint8_t pdn_disable : 1;
    uint8_t mstep_reg_select : 1;
    uint8_t multistep_filt : 1;
    uint8_t test_mode : 1;
    uint32_t reserved : 22;
  };
  uint32_t raw;
} tmc2209_reg_gconf_t;

typedef union {
  struct {
    uint8_t reset : 1;
    uint8_t drv_err : 1;
    uint8_t uv_cp : 1;
    uint32_t reserved : 29;
  };
  uint32_t raw;
} tmc2209_reg_gstat_t;

typedef struct {
  uint32_t raw;
} tmc2209_reg_ifcnt_t;

typedef union {
  struct {
    uint8_t reserved_bottom : 8;
    uint8_t send_delay : 3;
    uint32_t reserved_top : 21;
  };
  uint32_t raw;
} tmc2209_reg_nodeconf_t;

typedef union {
  struct {
    uint8_t otpbit : 2;
    uint8_t reserved_bottom : 2;
    uint8_t otpbyte : 2;
    uint8_t reserved_middle : 2;  // x kurwa d
    uint8_t otp_magic : 8;
    uint16_t reserved_top : 16;
  };
  uint32_t raw;
} tmc2209_reg_otp_prog_t;

// TODO(Glibus): decide if needed, if not remove
// typedef union {
//   struct {
//     uint8_t otp00_04 : 5;  // reset default for FCLKTRIM
//     uint8_t otp05 : 1;     // reset default for OTTTRIM
//     uint8_t otp06 : 1;     // reset default for GCONF.internal_Rsense
//     uint8_t otp07 : 1;     // reset default for TBL
//   };
//   uint8_t raw;
// } tmc2209_reg_otp_read0_read_t;

// typedef union {
//   struct {
//     uint8_t otp10_13 : 4;  // reset default for TBL
//   };
//   uint8_t raw;
// } tmc2209_reg_otp_read1_read_t;

typedef union {
  struct {
    uint8_t otp0 : 8;
    uint8_t otp1 : 8;
    uint8_t otp2 : 8;
    uint32_t reserved : 8;
  };
  uint32_t raw;
} tmc2209_reg_otp_read_t;

typedef union {
  struct {
    uint8_t enn : 1;
    uint8_t zero_0 : 1;
    uint8_t ms1 : 1;
    uint8_t ms2 : 1;
    uint8_t diag : 1;
    uint8_t zero_1 : 1;
    uint8_t pdn_uart : 1;
    uint8_t step : 1;
    uint8_t spread_en : 1;
    uint8_t dir : 1;
    uint16_t reserved : 14;
    uint8_t version : 8;
  };
  uint32_t raw;
} tmc2209_reg_ioin_t;

typedef union {
  struct {
    uint8_t fclktrim : 4;
    uint8_t ottrim : 4;
    uint32_t reserved : 24;
  };
  uint32_t raw;
} tmc2209_reg_factory_conf_t;

typedef union {
  struct {
    uint8_t ihold : 5;
    uint8_t reserved_bottowm : 3;
    uint8_t irun : 5;
    uint8_t reserved_top : 3;
    uint8_t iholddelay : 4;
    uint16_t reserved : 12;
  };
  uint32_t raw;
} tmc2209_reg_ihold_irun_t;

typedef union {
  struct {
    uint8_t tpowerdown : 8;
    uint32_t reserved : 24;
  };
  uint32_t raw;
} tmc2209_reg_tpowerdown_t;

typedef union {
  struct {
    uint32_t tstep : 20;
    uint16_t reserved : 12;
  };
  uint32_t raw;
} tmc2209_reg_tstep_t;

typedef union {
  struct {
    uint32_t tpwmthrs : 20;
    uint16_t reserved : 12;
  };
  uint32_t raw;
} tmc2209_reg_tpwmthrs_t;

typedef union {
  struct {
    uint32_t vactual : 24;
    uint8_t reserved : 8;
  };
  uint32_t raw;
} tmc2209_reg_vactual_t;
