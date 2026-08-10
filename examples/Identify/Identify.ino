#include <SPI.h>
#include <w25n_Flash.h>

constexpr uint8_t FLASH_CS_PIN   = 10;
constexpr uint8_t FLASH_MOSI_PIN = MOSI;
constexpr uint8_t FLASH_MISO_PIN = MISO;
constexpr uint8_t FLASH_SCK_PIN  = SCK;

W25N_Flash flash(FLASH_CS_PIN, SPI);

void setup()
{
    uint8_t manufacturerId;
    uint16_t deviceId;
    uint8_t protection;
    uint8_t configuration;

    Serial.begin(115200);
    SPI.begin();
    flash.begin();
    flash.readJedecId(manufacturerId, deviceId);
    flash.readStatus(RegAddrProtect, protection);
    flash.readStatus(RegAddrConfigure, configuration);

    Serial.print("Manufacturer ID: 0x");
    Serial.println(manufacturerId, HEX);
    Serial.print("Device ID: 0x");
    Serial.println(deviceId, HEX);
    Serial.print("Protection register: 0x");
    Serial.println(protection, HEX);
    Serial.print("Configuration register: 0x");
    Serial.println(configuration, HEX);
}

void loop()
{
}
