#pragma once
#include "../Mnemonics.h"
#include "../triple.h"
#include "typeEnum.h"
#include <typeinfo>
#include <tlhelp32.h>
#include <stdio.h>
#include <ctype.h>
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
            LPCSTR process;
            HANDLE pHandle;
            DWORD pid;
            
            void getPid();
            BOOL isValidPointer(uintptr_t ptr);
            void AnalyzeMemory(HANDLE process, char* buffer, uintptr_t baseAddr, SIZE_T size);

        public:
            inline void scan();
            DynDisasm(LPCSTR process);
            template<typename valType> TypeE typeViaID(valType val);

            template<typename type> type rpm(uintptr_t address, type value);
            template<typename type> type* rpm(uintptr_t address, int offset, int count, type value);

            template<typename type> bool wpm(uintptr_t address, type value);
            bool decastPtr(triple<uintptr_t, void*, TypeE>& trpl);
            std::vector<bool> wpm(std::vector<triple<uintptr_t, void*, TypeE>> writes);

            ~DynDisasm();
    };
};