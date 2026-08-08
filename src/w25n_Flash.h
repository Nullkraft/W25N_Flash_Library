#include <Arduino.h>
#include <stdint.h>
#include "w25n_command_codes.h"

class W25N_Flash {
  private:
    uint8_t manufID = 0xF;
    uint16_t deviceID = 0xFF;
    uint8_t PRValue = 0x42;
    uint8_t CRValue = 0x69;
    uint8_t SRValue = 0x67;

    void readManufID(uint16_t ID);
    void readProtReg(uint8_t ID);
    void readConfReg(uint8_t ID);
    void readStatusReg(uint8_t ID);

  public:
    W25N_Flash();
    void begin();
    uint8_t getManufID();
    uint16_t getDeviceID();
    uint8_t getProtReg();
    uint8_t getConfReg();
    uint8_t getStatReg();

    void spiWriteRegister(uint8_t sel_pin, uint32_t value);

    void loadStatusReg(uint8_t instructionCode);

    void loadProtectRegister(uint8_t instructionCode);

    void loadConfigRegister(uint8_t instructionCode);
};
