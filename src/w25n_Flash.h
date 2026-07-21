#include <stdint.h>

class W25N_Flash {
  private:
    W25N_Flash();
    static uint32_t manufID;
  public:
    uint8_t jedecIDcmd = 0x9F;
    uint32_t readManufID(uint8_t ID);
    uint32_t getJedecID();
};
