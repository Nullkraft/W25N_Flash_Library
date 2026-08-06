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
  manufID = SPI.transfer(0x0);
  deviceID = SPI.transfer16(0x0);
  SPI.endTransaction();
}

void W25N_Flash::readProtReg(uint8_t instrCode) {
  SPI.beginTransaction(SPISettings(16000000, MSBFIRST, SPI_MODE0));
  SPI.transfer(instrCode);
  SPI.transfer(protectRegisterAddress);
  PRValue = SPI.transfer(0x0);
  SPI.endTransaction();
}

void W25N_Flash::readConfReg(uint8_t instrCode) {
  SPI.beginTransaction(SPISettings(16000000, MSBFIRST, SPI_MODE0));
  SPI.transfer(instrCode);
  SPI.transfer(configureRegisterAddress);
  CRValue = SPI.transfer(0x0);
  SPI.endTransaction();
}

void W25N_Flash::readStatusReg(uint8_t instrCode) {
  SPI.beginTransaction(SPISettings(16000000, MSBFIRST, SPI_MODE0));
  SPI.transfer(instrCode);
  SPI.transfer(statusRegisterAddress);
  SRValue = SPI.transfer(0x0);
  SPI.endTransaction();
}

void W25N_Flash::loadProtectRegister() {
  readProtReg(0x05);
}

void W25N_Flash::loadConfigRegister() {
  readConfReg(0x05);
}

void W25N_Flash::loadStatusReg() {
  readStatusReg(0x05);
}

uint8_t W25N_Flash::getManufID() {
  return manufID;    // Returns 0 if read failed
}

uint8_t W25N_Flash::getProtReg() {
  return PRValue;
}

uint8_t W25N_Flash::getConfReg() {
  return CRValue;
}

uint8_t W25N_Flash::getStatReg() {
  return SRValue;
}

void W25N_Flash::spiWriteRegister(uint8_t sel_pin, uint32_t value) {
  // ToDo: Implement flash writes
}

uint16_t W25N_Flash::getDeviceID() {
  return deviceID;
}
