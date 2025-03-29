#pragma once
#include "../Mnemonics.h"
#include "../triple.h"
#include "typeEnum.h"

/*
**  this thing will manipulate memory of the hooked process
**
**  Hook this program to the wanted process
**  
**  Try to make this with the idea of multiple program scanning
*/

namespace ASM {
    class DynDisasm {
        private:
            LPCSTR moduleName;
            HANDLE pHandle;
            
            
            bool hookProcess();

        public:
            DynDisasm(LPCSTR process);
            
            inline void scan();

            template<typename type> type rpm(uintptr_t address, type value);
            template<typename type> type* rpm(uintptr_t address, int offset, int count, type value);

            template<typename type> bool wpm(uintptr_t address, type value);
            bool decastPtr(triple<uintptr_t, void*, TypeE>& trpl);
            std::vector<bool> wpm(std::vector<triple<uintptr_t, void*, TypeE>> writes);

            ~DynDisasm();
    };
};