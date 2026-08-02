#include <Arduino.h>
#include <stdint.h>

class W25N_Flash {
  private:
    const static uint16_t jedecIDcmd = 0x9F00;
    uint8_t manufID = 0xF;
    uint16_t deviceID = 0xFF;
    uint8_t SRValue = 0xF;

    void readManufID(uint16_t ID);
    void readStatusReg(uint8_t instrCode);

  public:
    W25N_Flash();
    void begin();
    uint8_t getManufID();
    uint16_t getDeviceID();
    uint8_t getStatReg();

    void reportStatusReg(uint8_t cmd_value);

    void spiWriteRegister(uint8_t sel_pin, uint32_t value);
};
