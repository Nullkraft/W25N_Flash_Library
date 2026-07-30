#include <Arduino.h>
#include <stdint.h>

class W25N_Flash {
  private:
    const static uint16_t jedecIDcmd = 0x9F00;
    uint8_t manufID = 0;
    uint8_t deviceIDHigh = 0;
    uint8_t deviceIDLow = 0;

    void readManufID(uint16_t ID);

  public:
    W25N_Flash();
    void begin();
    uint8_t getManufID();
    uint16_t getDeviceID();

    void spiWriteRegister(uint8_t sel_pin, uint32_t value);
};
