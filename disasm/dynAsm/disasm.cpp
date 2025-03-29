#include "disasm.h"


namespace ASM {
    bool DynDisasm::hookProcess() {
        return {};
    }

    DynDisasm::DynDisasm(LPCSTR process) {

    }
            
    inline void DynDisasm::scan() {
        
    }

    template<typename type> type DynDisasm::rpm(uintptr_t address, type value) {
        return {};
    }

    template<typename type> type* DynDisasm::rpm(uintptr_t address, int offset, int count, type value) {
        return {};
    }

    template<typename type> bool DynDisasm::wpm(uintptr_t address, type value) {
        SIZE_T written;
        bool returnB = WriteProcessMemory(pHandle, (LPVOID)address, &value, sizeof(type), &written);
        
        if (written <= 0 || !returnB) std::cout << "[Dynamic Disasm] [Error] Write failed at address and value of: " << address << ", " << value << "\n\n";

        return returnB;
    }

    bool DynDisasm::decastPtr(triple<uintptr_t, void*, TypeE>& trpl) {
        switch(trpl.cVal) {
            case INTn: return       wpm(trpl.aVal, *(int*)trpl.bVal);
            case FLOATn: return     wpm(trpl.aVal, *(float*)trpl.bVal);
            case STRING: return     wpm(trpl.aVal, *(std::string*)trpl.bVal);
            case CHARn: return      wpm(trpl.aVal, *(char*)trpl.bVal);
            case CHARPTR: return    wpm(trpl.aVal, *((char**)(trpl.bVal)));
            case DOUBLEn: return    wpm(trpl.aVal, *(double*)trpl.bVal);
            case UNSIGNED: return   wpm(trpl.aVal, *(unsigned*)trpl.bVal);
            case BOOLn: return      wpm(trpl.aVal, *(bool*)trpl.bVal);
            default: return false;
        };
    }

    std::vector<bool> DynDisasm::wpm(std::vector<triple<uintptr_t, void*, TypeE>> writes) {
        std::vector<bool> writesOutput(writes.size(), false);
        int ctr = 0;

        for (triple<uintptr_t, void*, TypeE>& aho : writes) {
            bool anu = decastPtr(aho);
            bool bad = !anu;
            if (bad) {
                std::cout << "[Dynamic Disasm] [Error] Depointing incident occurred at " << aho.aVal
                          << " with type and value of: " << aho.cVal << ", " << anu << "\n\n";

                writesOutput[ctr++] = false;
                continue;
            }

            writesOutput[ctr++] = anu;
        }

        return writesOutput;
    }

    DynDisasm::~DynDisasm() {

    }
};