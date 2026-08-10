#pragma once

#include <stdint.h>

#include "w25n_command_codes.h"
#include "w25n_transport.h"

class W25N_Flash {
public:
    explicit W25N_Flash(uint8_t csPin, SPIClass& spi = SPI);

    void begin();
    void readJedecId(uint8_t& manufacturerId, uint16_t& deviceId);
    void readStatus(uint8_t registerAddress, uint8_t& value);

private:
    W25nTransport _transport;
};
