#pragma once
#include <stdint.h>

const static uint16_t jedecIDcmd = 0x9F00;

const static uint8_t protectRegisterAddress = 0xA0;
const static uint8_t configureRegisterAddress = 0xB0;
const static uint8_t statusRegisterAddress = 0xC0;

const static uint8_t readRegisters = 0x05;
