#include <stdint.h>

class W25N_Flash {
  private:
    const static uint32_t manufID = 0xAA21;

  public:
    W25N_Flash();
    uint8_t jedecIDcmd = 0x9F;
    uint32_t readManufID(uint8_t ID);
    uint32_t getJedecID();

    void spiWriteRegister(uint8_t sel_pin, uint32_t value);
};
