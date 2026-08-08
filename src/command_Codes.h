#pragma once
#include <stdint.h>

const static uint16_t CmdJededId = 0x9F00;
const static uint16_t jedecIDcmd = CmdJededId;

const static uint8_t RegAddrProtect = 0xA0;
const static uint8_t RegAddrConfigure = 0xB0;
const static uint8_t RegAddrStatus = 0xC0;

const static uint8_t protectRegisterAddress = RegAddrProtect;
const static uint8_t configureRegisterAddress = RegAddrConfigure;
const static uint8_t statusRegisterAddress = RegAddrStatus;

const static uint8_t CmdReadRegisters = 0x05;
const static uint8_t readRegisters = CmdReadRegisters;
