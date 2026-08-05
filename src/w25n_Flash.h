#include <Arduino.h>
#include <stdint.h>

class W25N_Flash {
  private:
    const static uint16_t jedecIDcmd = 0x9F00;
    uint8_t manufID = 0xF;
    uint16_t deviceID = 0xFF;
    uint8_t PRValue = 0xF;
    uint8_t CRValue = 0xF;
    uint8_t SRValue = 0xF;

    void readManufID(uint16_t ID);
    void readProtReg(uint8_t ID);
    void readStatusReg(uint8_t instrCode, uint8_t regAdd);

  public:
    W25N_Flash();
    void begin();
    uint8_t getManufID();
    uint16_t getDeviceID();
    uint8_t getProtReg();
    uint8_t getConfReg();
    uint8_t getStatReg();

    void reportStatusReg(uint8_t cmd_value, uint8_t regAdd);

    void spiWriteRegister(uint8_t sel_pin, uint32_t value);

    void loadProtectRegister();
};
