#pragma once
#include <iostream>
#include <windows.h>
#include <vector>
#include <string>
#include <fstream>
#include <stdexcept>
#include <cstdint>
#include <utility>

using byte = unsigned char;


struct ModEntry {
    byte opcode;
    std::string mnemonic;
};

static std::vector<std::pair<byte, std::string>> mod00 = {
    { 0x00, "[RAX]" },
    { 0x01, "[RCX]" },
    { 0x02, "[RDX]" },
    { 0x03, "[RBX]" },
    { 0x04, "SIB" },
    { 0x05, "disp32" },
    { 0x06, "[RSI]" },
    { 0x07, "[RDI]" }
};

static std::vector<std::pair<byte, std::string>> mod01 = {
    { 0x00, "[RAX + disp8]" },
    { 0x01, "[RCX + disp8]" },
    { 0x02, "[RDX + disp8]" },
    { 0x03, "[RBX + disp8]" },
    { 0x04, "SIB + disp8" },
    { 0x05, "[RBP + disp8]" },
    { 0x06, "[RSI + disp8]" },
    { 0x07, "[RDI + disp8]" }
};

static std::vector<std::pair<byte, std::string>> mod10 = {
    { 0x00, "[RAX + disp32]" },
    { 0x01, "[RCX + disp32]" },
    { 0x02, "[RDX + disp32]" },
    { 0x03, "[RBX + disp32]" },
    { 0x04, "SIB + disp32" },
    { 0x05, "[RBP + disp32]" },
    { 0x06, "[RSI + disp32]" },
    { 0x07, "[RDI + disp32]" }
};

static std::vector<std::pair<byte, std::string>> mod11 = {
    { 0x00, "RAX" },
    { 0x01, "RCX" },
    { 0x02, "RDX" },
    { 0x03, "RBX" },
    { 0x04, "RSP" },
    { 0x05, "RBP" },
    { 0x06, "RSI" },
    { 0x07, "RDI" }
};

static std::vector<std::pair<byte, std::string>> opcodes = {
    { 0x88, "MOV" },
    { 0x89, "MOV" },
    { 0x8A, "MOV" },
    { 0x8B, "MOV" },
    { 0xB8, "MOV" },
    { 0xC6, "MOV" },
    { 0xC7, "MOV" }
};

static std::vector<std::pair<byte, std::string>> instants = {
    { 0x90, "NOP" },
    { 0x98, "CBW" },
    { 0x99, "CWD" },
    { 0x9B, "WAIT" },
    { 0x9C, "PUSHF" },
    { 0x9D, "POPF" },
    { 0x9E, "SAHF" },
    { 0x9F, "LAHF" },
    { 0xC3, "RET" },
    { 0xCB, "RETF" },
    { 0xCC, "INT 3" },
    { 0xCE, "INTO" },
    { 0xCF, "IRET" },
    { 0xD7, "XLAT" },
    { 0xF0, "LOCK" },
    { 0xF2, "REPNZ" },
    { 0xF3, "REP" },
    { 0xF4, "HLT" },
    { 0xF5, "CMC" },
    { 0xF8, "CLC" },
    { 0xF9, "STC" },
    { 0xFA, "CLI" },
    { 0xFB, "STI" },
    { 0xFC, "CLD" },
    { 0xFD, "STD" },
    { 0x70, "JO" },
    { 0x71, "JNO" },
    { 0x72, "JB" },
    { 0x73, "JAE" },
    { 0x74, "JZ" },
    { 0x75, "JNZ" },
    { 0x76, "JBE" },
    { 0x77, "JA" },
    { 0x78, "JS" },
    { 0x79, "JNS" },
    { 0x7A, "JP" },
    { 0x7B, "JNP" },
    { 0x7C, "JL" },
    { 0x7D, "JGE" },
    { 0x7E, "JLE" },
    { 0x7F, "JG" },
    { 0xEB, "JMP (short)" },
    { 0xE9, "JMP (near)" },
    { 0xEA, "JMP (far)" }
};