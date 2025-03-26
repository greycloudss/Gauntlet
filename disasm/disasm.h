#pragma once
#include "Mnemonics.h"

namespace ASM {
    class StatDisasm{
        private:
            LPCSTR fName;
            std::vector<int> binary;

            inline int getMod(int val);
            inline int getReg(int val);
            inline int getRM(int val);
            void ReadBinary();
            std::string lookup(const std::vector<std::pair<byte, std::string>>& vec, byte key);
            bool isInterrupt(int val, int intNum, int i);
            int CalculateTargetAddress(int i, int cur);
            bool printer(size_t& i, const std::string& operation, std::string firstArg, std::string secondArg);
            bool isMov(uint8_t opcode, size_t& i, const std::string& operation);
            int GetOffsetSize(uint8_t opcode, bool isFarJump, bool isProtectedMode);
            
        public:
            StatDisasm(LPCSTR fName);
            void disassemble();
            std::vector<std::pair<byte, std::string>>* GetModTable(int val);
            ~StatDisasm();
    };


    class DynAsm {
        private:
            LPCSTR moduleName;
            HANDLE pHandle;
        public:
            DynAsm();
            ~DynAsm();
    };
};