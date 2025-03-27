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
        return {};
    }

    template<typename type> bool* DynDisasm::wpm(uintptr_t* addressList, int* offsets, type* values) {
        return {};
    }

    DynDisasm::~DynDisasm() {

    }
};