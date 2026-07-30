#include <SPI.h>
#include "w25n_Flash.h"

W25N_Flash::W25N_Flash() {
  // ctor
}

void W25N_Flash::begin(uint8_t flash_sel) {
  readManufID(jedecIDcmd, flash_sel);
}

void W25N_Flash::readManufID(uint16_t ID, uint8_t flash_sel) {
  SPI.beginTransaction(SPISettings(16000000, MSBFIRST, SPI_MODE0));
  SPI.transfer16(jedecIDcmd);
  manufID = SPI.transfer(0x00);
  deviceIDHigh = SPI.transfer(0x00);
  deviceIDLow = SPI.transfer(0x00);
  SPI.endTransaction();
}

uint8_t W25N_Flash::getManufID() {
  return W25N_Flash::manufID;    // Returns 0 if read failed
}

void W25N_Flash::spiWriteRegister(uint8_t sel_pin, uint32_t value) {
  // ToDo: Implement flash writes
}

uint16_t W25N_Flash::getDeviceID() {
  uint16_t deviceID = (deviceIDHigh << 8 | deviceIDLow);
  return deviceID;
}
