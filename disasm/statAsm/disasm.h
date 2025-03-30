#pragma once
#include "../Mnemonics.h"

namespace ASM {
    class StatDisasm {
        private:
            LPCSTR fName;
            std::vector<int> binary;
            std::ofstream ot;

            inline int getMod(int val);
            inline int getReg(int val);
            inline int getRM(int val);
            std::string lookup(const std::vector<std::pair<byte, std::string>>& vec, byte key);

            void ReadBinary();
            int CalculateTargetAddress(int i, int cur);
            
            bool printer(size_t& i, const std::string& operation, std::string firstArg, std::string secondArg);

            bool isInterrupt(int val, int intNum, int i);
            bool isMov(uint8_t opcode, size_t& i, const std::string& operation);
            int GetOffsetSize(uint8_t opcode, bool isFarJump, bool isProtectedMode);
            
            
        public:
            StatDisasm(LPCSTR fName);
            void disassemble();
            std::vector<std::pair<byte, std::string>>* GetModTable(int val);
            ~StatDisasm();
    };
};