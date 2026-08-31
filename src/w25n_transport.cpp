#include "w25n_transport.h"

namespace {

constexpr uint32_t W25N_SPI_CLOCK_HZ = 16000000UL;

} // namespace

W25nTransport::W25nTransport(uint8_t csPin, SPIClass& spi)
    : _csPin(csPin), _spi(spi)
{
}

void W25nTransport::begin()
{
    pinMode(_csPin, OUTPUT);
    digitalWrite(_csPin, HIGH);
}

void W25nTransport::transfer(const uint8_t* command, size_t commandLength,
                             const uint8_t* writeData, size_t writeLength,
                             uint8_t* readData, size_t readLength)
{
    _spi.beginTransaction(SPISettings(W25N_SPI_CLOCK_HZ, MSBFIRST, SPI_MODE0));
    digitalWrite(_csPin, LOW);

    for (size_t i = 0; i < commandLength; ++i) {
        _spi.transfer(command[i]);
    }
    for (size_t i = 0; i < writeLength; ++i) {
        _spi.transfer(writeData[i]);
    }
    for (size_t i = 0; i < readLength; ++i) {
        readData[i] = _spi.transfer(0U);
    }

    digitalWrite(_csPin, HIGH);
    _spi.endTransaction();
}
