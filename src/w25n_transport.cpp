#include "w25n_transport.h"

W25nTransport::W25nTransport(SPIClass& spi)
    : _spi(spi)
{
}

void W25nTransport::begin()
{
}

void W25nTransport::transfer(const uint8_t* command, size_t commandLength,
                             const uint8_t* writeData, size_t writeLength,
                             uint8_t* readData, size_t readLength)
{
    for (size_t i = 0; i < commandLength; ++i) {
        _spi.transfer(command[i]);
    }
    for (size_t i = 0; i < writeLength; ++i) {
        _spi.transfer(writeData[i]);
    }
    for (size_t i = 0; i < readLength; ++i) {
        readData[i] = _spi.transfer(0U);
    }
}
