#include "../inject/injector.h"
#include "../disasm/dynAsm/disasm.h"
#include "../disasm/statAsm/disasm.h"

DWORD WINAPI statScanThread(LPVOID params) {
    ASM::StatDisasm* sAsm = (ASM::StatDisasm*) params;
    sAsm->disassemble();
    return 0;
}
DWORD WINAPI dynScanThread(LPVOID params) {
    ASM::DynDisasm* dAsm = (ASM::DynDisasm*) params;
    do {
        dAsm->scan();
        Sleep(10000);
    } while (true);
    return 0;
}
DWORD WINAPI injectionThread(LPVOID params) {
    INJ::Injector* injector = (INJ::Injector*) params;
    std::cout << injector->inject();
    return 0;
}