#include "MPR121.h"


uint8_t MPR121_Init(void) {
    
    writeRegister(MPR121_SOFTRESET, 0x63);
    HAL_Delay(1);
    writeRegister(MPR121_ECR, 0x0);

  uint8_t c = readRegister8(MPR121_CONFIG2);

  if (c != 0x24)
    return c;

  setThresholds(MPR121_TOUCH_THRESHOLD_DEFAULT, MPR121_RELEASE_THRESHOLD_DEFAULT);
  writeRegister(MPR121_MHDR, 0x01);
  writeRegister(MPR121_NHDR, 0x01);
  writeRegister(MPR121_NCLR, 0x0E);
  writeRegister(MPR121_FDLR, 0x00);

  writeRegister(MPR121_MHDF, 0x01);
  writeRegister(MPR121_NHDF, 0x05);
  writeRegister(MPR121_NCLF, 0x01);
  writeRegister(MPR121_FDLF, 0x00);

  writeRegister(MPR121_NHDT, 0x00);
  writeRegister(MPR121_NCLT, 0x00);
  writeRegister(MPR121_FDLT, 0x00);

  writeRegister(MPR121_DEBOUNCE, 0);
  writeRegister(MPR121_CONFIG1, 0x10); // default, 16uA charge current
  writeRegister(MPR121_CONFIG2, 0x20); // 0.5uS encoding, 1ms period
    uint8_t ECR_SETTING =
      B10000000 + 12; // 5 bits for baseline tracking & proximity disabled + X
                      // amount of electrodes running (12)
  writeRegister(MPR121_ECR, ECR_SETTING); // start with above ECR setting

    return c; 
}


// uint8_t MPR121_IsTouched(void) {
//     uint8_t touchStatus[2] = {0};

    
//     if (HAL_I2C_Mem_Read(&hi2c1, MPR121_I2C_ADDR, MPR121_TOUCH_STATUS_L, I2C_MEMADD_SIZE_8BIT, touchStatus, 2, HAL_MAX_DELAY) != HAL_OK) {
//         return 0; 
//     }

    
//     uint16_t touchData = (touchStatus[1] << 8) | touchStatus[0];
//     return (touchData != 0) ? 1 : 0;
// }

void writeRegister(uint8_t reg, uint8_t cmd) {
	// uint8_t arr[2] = { reg, cmd };
	// HAL_I2C_Master_Transmit(&hi2c1, MPR121_I2C_ADDR, arr, 2, HAL_MAX_DELAY);
    HAL_I2C_Mem_Write(&hi2c1, MPR121_I2C_ADDR|0x01,reg,1,&cmd,1,HAL_MAX_DELAY);
}

/**
 * @brief Reads from a specific register.
 * @param reg Address of register to read from.
 * @return Byte read.
 */
uint8_t readRegister8(uint8_t reg) {
	// HAL_I2C_Master_Transmit(&hi2c1, MPR121_I2C_ADDR, &reg, 1, HAL_MAX_DELAY);
	uint8_t result;
	// HAL_I2C_Master_Receive(&hi2c1, MPR121_I2C_ADDR, &result, 1, HAL_MAX_DELAY);
    HAL_I2C_Mem_Read(&hi2c1,MPR121_I2C_ADDR,reg,1,&result,1,HAL_MAX_DELAY);
	return result;
}

uint16_t readRegister16(uint8_t reg) {
	// HAL_I2C_Master_Transmit(&hi2c1, MPR121_I2C_ADDR, &reg, 1, HAL_MAX_DELAY);
	// uint8_t result[2];
	// HAL_I2C_Master_Receive(&hi2c1, MPR121_I2C_ADDR, (uint8_t*)&result, 2, HAL_MAX_DELAY);
	// return result[0]|(result[1]<<8);
	uint16_t result;
    HAL_I2C_Mem_Read(&hi2c1,MPR121_I2C_ADDR,reg,1,(uint8_t*)&result,2,HAL_MAX_DELAY);
    return result;
}

void setThresholds(uint8_t touch, uint8_t release) {
  // set all thresholds (the same)
  for (uint8_t i = 0; i < 12; i++) {
    writeRegister(MPR121_TOUCHTH_0 + 2 * i, touch);
    writeRegister(MPR121_RELEASETH_0 + 2 * i, release);
  }
}

uint16_t filteredData(uint8_t t) {
  if (t > 12)
    return 0;
  return readRegister16(MPR121_FILTDATA_0L + t * 2);
}

uint16_t touched(void) {
  uint16_t t = readRegister16(MPR121_TOUCHSTATUS_L);
  return t & 0x0FFF;
}
