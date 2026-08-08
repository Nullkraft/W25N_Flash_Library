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

const static uint16_t CmdSelectLo1 = 0x01FFU;
const static uint16_t CmdSelectLo2 = 0x02FFU;
const static uint16_t CmdSelectLo3 = 0x03FFU;
const static uint16_t CmdFlagMask = 0x00FFU;
const static uint16_t CmdRefOff = 0x04FFU;
const static uint16_t CmdSelectAdc1 = 0x05FFU;
const static uint16_t CmdEnterCommand = 0x06FFU;
const static uint16_t CmdLedOff = 0x07FFU;
const static uint16_t CmdAttenuator = 0x08FFU;
const static uint16_t CmdRef1 = 0x0CFFU;
const static uint16_t CmdSelectAdc2 = 0x0DFFU;
const static uint16_t CmdEnterFmn = 0x0EFFU;
const static uint16_t CmdLedOn = 0x0FFFU;
const static uint16_t CmdRef2 = 0x14FFU;
const static uint16_t CmdSelectRam = 0x15FFU;
const static uint16_t CmdEnterDirect = 0x16FFU;
const static uint16_t CmdMessageRequest = 0x17FFU;
const static uint16_t CmdBeginSweep = 0x1FFFU;
const static uint16_t CmdEndSweep = 0x27FFU;
const static uint16_t CmdResetHardwareReportPllStatus = 0x2FFFU;
const static uint16_t CmdBeginMacro = 0x37FFU;
const static uint16_t CmdEndMacro = 0x3FFFU;
const static uint16_t CmdSquelchLevel = 0x47FFU;
const static uint16_t CmdFlashId = 0x48FFU;
const static uint16_t CmdFlashRegisterReport = 0x49FFU;
const static uint16_t CmdDivaModeLo2 = 0x4AFFU;
const static uint16_t CmdDivaModeLo3 = 0x4BFFU;
const static uint32_t CmdSerialAsciiWord = 0x000106FFUL;
const static uint32_t CmdSerialBinaryWord = 0x000206FFUL;
