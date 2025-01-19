#ifndef __DRIVER_MPR121_H
#define __DRIVER_MPR121_H
//#include "i2c.h"
#include <stdint.h>

#include "stm32f1xx_hal.h"
#define B10000000 128
// I2C Handle
#define MPR121_I2CADDR_DEFAULT 0x5A        ///< default I2C address
#define MPR121_TOUCH_THRESHOLD_DEFAULT 12  ///< default touch threshold value
#define MPR121_RELEASE_THRESHOLD_DEFAULT 6 ///< default relese threshold value
extern I2C_HandleTypeDef hi2c1;
enum {
  MPR121_TOUCHSTATUS_L = 0x00,
  MPR121_TOUCHSTATUS_H = 0x01,
  MPR121_FILTDATA_0L = 0x04,
  MPR121_FILTDATA_0H = 0x05,
  MPR121_BASELINE_0 = 0x1E,
  MPR121_MHDR = 0x2B,
  MPR121_NHDR = 0x2C,
  MPR121_NCLR = 0x2D,
  MPR121_FDLR = 0x2E,
  MPR121_MHDF = 0x2F,
  MPR121_NHDF = 0x30,
  MPR121_NCLF = 0x31,
  MPR121_FDLF = 0x32,
  MPR121_NHDT = 0x33,
  MPR121_NCLT = 0x34,
  MPR121_FDLT = 0x35,

  MPR121_TOUCHTH_0 = 0x41,
  MPR121_RELEASETH_0 = 0x42,
  MPR121_DEBOUNCE = 0x5B,
  MPR121_CONFIG1 = 0x5C,
  MPR121_CONFIG2 = 0x5D,
  MPR121_CHARGECURR_0 = 0x5F,
  MPR121_CHARGETIME_1 = 0x6C,
  MPR121_ECR = 0x5E,
  MPR121_AUTOCONFIG0 = 0x7B,
  MPR121_AUTOCONFIG1 = 0x7C,
  MPR121_UPLIMIT = 0x7D,
  MPR121_LOWLIMIT = 0x7E,
  MPR121_TARGETLIMIT = 0x7F,

  MPR121_GPIODIR = 0x76,
  MPR121_GPIOEN = 0x77,
  MPR121_GPIOSET = 0x78,
  MPR121_GPIOCLR = 0x79,
  MPR121_GPIOTOGGLE = 0x7A,

  MPR121_SOFTRESET = 0x80,
};

// MPR121 I2C Address
#define MPR121_I2C_ADDR    (0x5A << 1)

#define MPR_NUM_0  (1<<7)
#define MPR_NUM_1  (1<<8)
#define MPR_NUM_2  (1<<4)
#define MPR_NUM_3  (1<<0)
#define MPR_NUM_4  (1<<9)
#define MPR_NUM_5  (1<<5)
#define MPR_NUM_6  (1<<1)
#define MPR_NUM_7 (1<<10)
#define MPR_NUM_8  (1<<6)
#define MPR_NUM_9  (1<<2)
#define MPR_NUM_D (1<<11)
#define MPR_NUM_O  (1<<3)


// MPR121 Registers
// #define MPR121_TOUCH_STATUS_L   0x00
// #define MPR121_TOUCH_STATUS_H   0x01

uint8_t MPR121_Init(void);
// uint8_t MPR121_IsTouched(void);
uint8_t readRegister8(uint8_t reg);
uint16_t readRegister16(uint8_t reg);
void writeRegister(uint8_t reg, uint8_t cmd);
void setThresholds(uint8_t touch, uint8_t release);
uint16_t filteredData(uint8_t t);
uint16_t touched(void);

#endif /* __DRIVER_MPR121_H */

