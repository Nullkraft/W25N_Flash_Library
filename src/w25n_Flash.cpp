#include "w25n_Flash.h"

W25N_Flash::W25N_Flash(uint8_t csPin, SPIClass& spi)
    : _transport(csPin, spi)
{
}

void W25N_Flash::begin()
{
    _transport.begin();
}

void W25N_Flash::readJedecId(uint8_t& manufacturerId, uint16_t& deviceId)
{
    const uint8_t command[] = {
        static_cast<uint8_t>(CmdJededId >> 8),
        static_cast<uint8_t>(CmdJededId),
    };
    uint8_t id[3];

    _transport.transfer(command, sizeof(command), nullptr, 0U, id, sizeof(id));
    manufacturerId = id[0];
    deviceId = (static_cast<uint16_t>(id[1]) << 8) | id[2];
}

void W25N_Flash::readStatus(uint8_t registerAddress, uint8_t* value)
{
    const uint8_t command[] = {CmdReadRegisters, registerAddress};

    _transport.transfer(command, sizeof(command), nullptr, 0U, value, 1U);
}
