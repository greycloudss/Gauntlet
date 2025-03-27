#include "disasm.h"


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
                std::cout << "[Error] Unknown mod\n";
                return nullptr;
        }
    }

    int StatDisasm::GetOffsetSize(uint8_t opcode, bool isFarJump = false, bool isProtectedMode = false) {
        if ((opcode >= 0x70 && opcode <= 0x7F) || opcode == 0xEB)return 1;
        if (opcode == 0xE9 || opcode == 0xE8) return isProtectedMode ? 4 : 2;
        if (opcode == 0xEA || isFarJump) return 8;
        return 0;
    }


    int StatDisasm::CalculateTargetAddress(int i, int cur) {
        if (i >= static_cast<int>(binary.size()) - 1) return cur;
    
        uint8_t opcode = static_cast<uint8_t>(binary[i]);
        int offsetSize = GetOffsetSize(opcode);
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
    
        while (!file.eof()) {
            char byte;
            file.read(&byte, 1);
            if (!file.eof()) binary.push_back(static_cast<unsigned char>(byte));
        }

        file.close();
    }

    std::string StatDisasm::lookup(const std::vector<std::pair<byte, std::string>>& vec, byte key) {
        for (const auto& p : vec) {
            if (p.first == key)
                return p.second;
        }
        return "UNKNOWN";
    }

    bool StatDisasm::printer(size_t& i, const std::string& operation, std::string firstArg, std::string secondArg) {
        std::cout << "CS:" << i << "   " << operation << " " << firstArg << ", " << secondArg << std::endl;
        return true;
    }

    bool StatDisasm::isMov(uint8_t opcode, size_t& i, const std::string& operation) {
        int modrm = binary[++i];
        int mod = getMod(modrm);
        int reg = getReg(modrm);
        int rm = getRM(modrm);
    
        std::string firstArg = lookup(mod11, 0xC0 + reg);
        std::string secondArg;
    
        if (isInterrupt(opcode, binary[i + 1], i)) i++;
    
        if (mod == 0x03) {
            secondArg = lookup(mod11, rm);
            return printer(i, operation, firstArg, secondArg);
        }
    
        if (mod == 0x00 && rm == 0x06) {
            int disp16 = binary[++i] | (binary[++i] << 8);
            firstArg = lookup(mod11, 0xC0 + reg);
            secondArg = "[0x" + std::to_string(disp16) + "]";
            return printer(i, operation, firstArg, secondArg);
        }
    
        const std::vector<std::pair<byte, std::string>>* modTable = nullptr;
        if (mod == 0x00) modTable = &mod00;
        if (mod == 0x01) modTable = &mod01;
        if (mod == 0x02) modTable = &mod10;
    
        if (!modTable) return false;
    
        secondArg = lookup(*modTable, rm);
    
        if (mod == 0x01) {
            int disp8 = binary[++i];
            secondArg += " + 0x" + std::to_string(disp8);
        }
    
        if (mod == 0x02) {
            int disp16 = binary[++i] | (binary[++i] << 8);
            secondArg += " + 0x" + std::to_string(disp16);
        }
    
        return printer(i, operation, firstArg, secondArg);
    }
    

    void StatDisasm::disassemble() {
        ReadBinary();

        for (size_t i = 0; i < binary.size(); i++) {
            uint8_t opcode = static_cast<uint8_t>(binary[i]);
            if (opcode == 0x00) {
                ++i;
                continue;
            }
    
            bool foundOp = false;
            for (const auto& op : opcodes) {
                if (op.first == opcode) {
                    foundOp = true;

                    if (op.second == "MOV" && i + 1 < binary.size()) isMov(opcode, i, op.second);

                    break;
                }
            }
    
            if (foundOp) continue;
            for (const auto& inst : instants) {
                if (inst.first != opcode) continue;

                if ((opcode >= 0x70 && opcode <= 0x7F) || opcode == 0xEB || opcode == 0xE9 || opcode == 0xEA) {
                    int targetAddress = CalculateTargetAddress(i, static_cast<int>(i));
                    std::cout << "CS:" << i << "   " << inst.second << " 0x" << std::hex << targetAddress << std::endl;
                    continue;
                }

                std::cout << "CS:" << i << "   " << inst.second << std::endl;
                
                break;

            }
            
        }
    }
};