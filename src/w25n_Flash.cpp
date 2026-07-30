#include <SPI.h>
#include "w25n_Flash.h"

W25N_Flash::W25N_Flash() {
  // ctor
}

void W25N_Flash::begin() {
  readManufID(jedecIDcmd);
}

void W25N_Flash::readManufID(uint16_t ID) {
  SPI.beginTransaction(SPISettings(16000000, MSBFIRST, SPI_MODE0));
  SPI.transfer16(ID);
  manufID = SPI.transfer(0x00);
  deviceIDHigh = SPI.transfer(0x00);
  deviceIDLow = SPI.transfer(0x00);
  SPI.endTransaction();
}

void W25N_Flash::readStatusReg(uint8_t instrCode) {
  Serial.println("readStatusReg");
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
