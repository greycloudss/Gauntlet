#include "disasm.h"
#include <fstream>
#include <windows.h>

namespace ASM {
    StatDisasm::StatDisasm(LPCSTR fName) : fName(fName) {  }

    inline int StatDisasm::getMod(int val) { return (val >> 6) & 0b11; }
    inline int StatDisasm::getReg(int val) { return (val >> 3) & 0b111; }
    inline int StatDisasm::getRM(int val)  { return val & 0b111; }

    std::vector<std::pair<byte, std::string>>* StatDisasm::GetModTable(int val) {
        int mod = getMod(val);
        switch (mod) {
            case 0x00: return &mod00;
            case 0x01: return &mod01;
            case 0x02: return &mod10;
            case 0x03: return &mod11;
            default:
                std::cout << "[Static Disasm] [Error] Unknown mod\n";
                return nullptr;
        }
    }

    int StatDisasm::GetOffsetSize(uint8_t opcode, bool isFarJump, bool isProtectedMode) {
        if ((opcode >= 0x70 && opcode <= 0x7F) || opcode == 0xEB) return 1;
        if (opcode == 0xE9 || opcode == 0xE8) return isProtectedMode ? 4 : 2;
        if (opcode == 0xEA || isFarJump) return 8;
        return 0;
    }

    int StatDisasm::CalculateTargetAddress(int i, int cur) {
        if (i >= static_cast<int>(binary.size()) - 1) return cur;

        uint8_t opcode = static_cast<uint8_t>(binary[i]);
        int offsetSize = GetOffsetSize(opcode, false, false);
        if (offsetSize == 0) return cur;

        int offset = 0;
        for (int j = 0; j < offsetSize; j++)
            offset |= (binary[i + 1 + j] << (j * 8));

        if (offsetSize == 1) offset = static_cast<int8_t>(offset);
        else if (offsetSize == 2) offset = static_cast<int16_t>(offset);

        return cur + offset + offsetSize + 1;
    }

    bool StatDisasm::isInterrupt(int val, int intNum, int i) {
        if (val == 0xCD) {
            std::cout << "CS:" << i << "   INT 0x" << std::hex << binary[intNum + 1] << std::endl;
            return true;
        }
        return false;
    }

    void StatDisasm::ReadBinary() {
        std::ifstream file(fName, std::ios::binary);
        if (!file.is_open()) return;

        char byte;
        while (file.get(byte)) {
            binary.push_back(static_cast<unsigned char>(byte));
        }
        file.close();
    }

    DWORD findTextSectionOffset(LPCSTR filename, DWORD& sizeOut) {
        std::ifstream file(filename, std::ios::binary);
        if (!file.is_open()) return 0;

        IMAGE_DOS_HEADER dosHeader;
        file.read(reinterpret_cast<char*>(&dosHeader), sizeof(dosHeader));
        file.seekg(dosHeader.e_lfanew, std::ios::beg);

        IMAGE_NT_HEADERS ntHeaders;
        file.read(reinterpret_cast<char*>(&ntHeaders), sizeof(ntHeaders));

        IMAGE_SECTION_HEADER sectionHeader;
        for (int i = 0; i < ntHeaders.FileHeader.NumberOfSections; i++) {
            file.read(reinterpret_cast<char*>(&sectionHeader), sizeof(sectionHeader));
            if (strncmp((char*)sectionHeader.Name, ".text", 5) == 0) {
                sizeOut = sectionHeader.Misc.VirtualSize;
                return sectionHeader.PointerToRawData;
            }
        }

        return 0;
    }

    std::string StatDisasm::lookup(const std::vector<std::pair<byte, std::string>>& vec, byte key) {
        for (const auto& p : vec) {
            if (p.first == key)
                return p.second;
        }
        return "UNKNOWN";
    }

    bool StatDisasm::printer(size_t& i, const std::string& operation, std::string firstArg, std::string secondArg) {
        std::cout << "CS:" << std::hex << i << "   " << operation << " " << firstArg << ", " << secondArg << "\n";
        ot << "CS:" << std::hex << i << "   " << operation << " " << firstArg << ", " << secondArg << "\n";
        return true;
    }

    bool StatDisasm::isMov(uint8_t opcode, size_t& i, const std::string& operation) {
        if (opcode >= 0xB8 && opcode <= 0xBF) {
            std::string reg = lookup(mod11, opcode - 0xB8);
            int imm = binary[++i] | (binary[++i] << 8) | (binary[++i] << 16) | (binary[++i] << 24);
            std::string immStr = "0x" + std::to_string(imm);
            return printer(i, "MOV", reg, immStr);
        }

        if (i + 1 >= binary.size()) return false;
        int modrm = binary[++i];
        int mod = getMod(modrm);
        int reg = getReg(modrm);
        int rm = getRM(modrm);

        std::string firstArg = lookup(mod11, reg);
        std::string secondArg;

        if (isInterrupt(opcode, binary[i + 1], i)) i++;

        if (mod == 0x03) {
            secondArg = lookup(mod11, rm);
            return printer(i, operation, firstArg, secondArg);
        }

        if (mod == 0x00 && rm == 0x06 && i + 2 < binary.size()) {
            int disp16 = binary[++i] | (binary[++i] << 8);
            secondArg = "[0x" + std::to_string(disp16) + "]";
            return printer(i, operation, firstArg, secondArg);
        }

        const auto* modTable = GetModTable(modrm);
        if (!modTable) return false;

        secondArg = lookup(*modTable, rm);

        if (mod == 0x01 && i + 1 < binary.size()) {
            int disp8 = binary[++i];
            secondArg += " + 0x" + std::to_string(disp8);
        }
        if (mod == 0x02 && i + 2 < binary.size()) {
            int disp16 = binary[++i] | (binary[++i] << 8);
            secondArg += " + 0x" + std::to_string(disp16);
        }

        return printer(i, operation, firstArg, secondArg);
    }

    void StatDisasm::disassemble() {
        DWORD textSize = 0;
        DWORD textOffset = findTextSectionOffset(fName, textSize);

        ReadBinary();
        ot = std::ofstream("disasm_" + std::string(fName) + ".txt");

        size_t end = textOffset + textSize;
        if (end > binary.size()) end = binary.size();

        int count = 0;
        int junkStreak = 0;
        const int junkThreshold = 16;

        for (size_t i = textOffset; i < end; i++) {
            uint8_t opcode = static_cast<uint8_t>(binary[i]);

            if (opcode == 0x00 || opcode == 0xCC) {
                junkStreak++;
                if (junkStreak >= junkThreshold) break;
                continue;
            }

            bool foundOp = false;
            for (const auto& op : opcodes) {
                if (op.first == opcode) {
                    foundOp = true;
                    if (op.second == "MOV" && i + 1 < binary.size()) {
                        if (!isMov(opcode, i, op.second)) {
                            junkStreak++;
                            break;
                        }
                    }
                    count++;
                    junkStreak = 0;
                    break;
                }
            }

            if (foundOp) continue;
            for (const auto& inst : instants) {
                if (inst.first != opcode) continue;

                if ((opcode >= 0x70 && opcode <= 0x7F) || opcode == 0xEB || opcode == 0xE9 || opcode == 0xEA) {
                    int targetAddress = CalculateTargetAddress(i, static_cast<int>(i));
                    std::cout << "CS:" << std::hex << i << "   " << inst.second << " 0x" << targetAddress << std::endl;
                    ot << "CS:" << std::hex << i << "   " << inst.second << " 0x" << targetAddress << std::endl;
                } else {
                    std::cout << "CS:" << std::hex << i << "   " << inst.second << std::endl;
                    ot << "CS:" << std::hex << i << "   " << inst.second << std::endl;
                }
                count++;
                junkStreak = 0;
                break;
            }
        }

        std::cout << "[Static Disasm] [Info] Disassembled instruction count: " << count << std::endl;
        ot.close();
    }
}