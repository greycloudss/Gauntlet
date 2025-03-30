#include "disasm.h"
#include <stdio.h>
#include <stdint.h>
#include <ctype.h>

namespace ASM {
    BOOL DynDisasm::isValidPointer(uintptr_t ptr) {
        MEMORY_BASIC_INFORMATION memInfo;
        if (VirtualQueryEx(pHandle, (LPCVOID)ptr, &memInfo, sizeof(memInfo)) == 0) return FALSE;
        return memInfo.State == MEM_COMMIT && (memInfo.Protect & (PAGE_READWRITE | PAGE_EXECUTE_READWRITE | PAGE_READONLY));
    }

    void DynDisasm::AnalyzeMemory(HANDLE process, char* buffer, uintptr_t baseAddr, SIZE_T size) {
        SIZE_T i = 0;
    
        while (i < size) {
            if (isprint(buffer[i]) && isprint(buffer[i + 1])) {
                SIZE_T strLen = 0;
                while (i + strLen < size && isprint(buffer[i + strLen]))
                    strLen++;
    
                printf("0x%p - 0x%p  [STRING %llu bytes] \"%.*s\"\n",
                    (void*)(baseAddr + i), (void*)(baseAddr + i + strLen), strLen, (int)strLen, buffer + i);
                i += strLen;
                continue;
            }

            if (i + 8 <= size) {
                uintptr_t ptr;
                memcpy(&ptr, buffer + i, sizeof(ptr));
                if (isValidPointer( ptr)) {
                    printf("0x%p - 0x%p  [POINTER] 0x%p\n",
                        (void*)(baseAddr + i), (void*)(baseAddr + i + 8), (void*)ptr);
                    i += 8;
                    continue;
                }
            }

            if (i + 4 <= size) {
                int val;
                memcpy(&val, buffer + i, 4);
                if (val >= -1000000 && val <= 1000000) {
                    printf("0x%p - 0x%p  [INT] %d\n",
                        (void*)(baseAddr + i), (void*)(baseAddr + i + 4), val);
                    i += 4;
                    continue;
                }
            }
    

            if (i + 4 <= size) {
                float f;
                memcpy(&f, buffer + i, 4);
                if (f > -1e6 && f < 1e6 && f == f) {
                    printf("0x%p - 0x%p  [FLOAT] %.3f\n",
                        (void*)(baseAddr + i), (void*)(baseAddr + i + 4), f);
                    i += 4;
                    continue;
                }
            }

            i++;
        }
    }

    inline void DynDisasm::scan() {
        SYSTEM_INFO sysInfo;
        GetSystemInfo(&sysInfo);

        char* basePtr = (char*)sysInfo.lpMinimumApplicationAddress;
        char* maxAddr = (char*)sysInfo.lpMaximumApplicationAddress;

        MEMORY_BASIC_INFORMATION memInfo;

        while (basePtr < maxAddr && VirtualQueryEx(pHandle, basePtr, &memInfo, sizeof(memInfo))) {
            if (memInfo.State == MEM_COMMIT && (memInfo.Protect & PAGE_READWRITE)) {
                SIZE_T regionSize = memInfo.RegionSize;
                char* regionBase = (char*)memInfo.BaseAddress;
                char* buffer = (char*)malloc(regionSize);

                SIZE_T bytesRead;
                if (ReadProcessMemory(pHandle, regionBase, buffer, regionSize, &bytesRead)) {
                    for (SIZE_T i = 0; i + 4 <= bytesRead; i += 4) {
                        int iVal;
                        float fVal;
                        memcpy(&iVal, buffer + i, sizeof(int));
                        memcpy(&fVal, buffer + i, sizeof(float));
                        
                        if (iVal == 0 && fVal == 0) continue;

                        uintptr_t addr = (uintptr_t)regionBase + i;
                        printf("0x%p  int: %d  float: %.3f\n", (void*)addr, iVal, fVal);
                    }
                }

                free(buffer);
            }

            basePtr = (char*)memInfo.BaseAddress + memInfo.RegionSize;
        }
    }

    DynDisasm::DynDisasm(LPCSTR process) : process(process) {
        getPid();
        
        pHandle = OpenProcess(PROCESS_VM_WRITE | PROCESS_VM_OPERATION | PROCESS_VM_READ | PROCESS_QUERY_INFORMATION | PROCESS_CREATE_THREAD, FALSE, pid);


        do {
            scan();
            Sleep(10000);
        } while (true);
    }

    void DynDisasm::getPid() {
        HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

        PROCESSENTRY32 pEntry;

        pEntry.dwSize = sizeof(pEntry);

        if (Process32First(snapshot, &pEntry)) {
            do {
                if (strcmp((const char*)pEntry.szExeFile, this->process) == 0) {
                    CloseHandle(snapshot);
                    pid = pEntry.th32ProcessID;
                    break;
                }

            } while (Process32Next(snapshot, &pEntry));
        }

        CloseHandle(snapshot);
    }

    template<typename type> type DynDisasm::rpm(uintptr_t address, type value) {
        type buffer{};
        SIZE_T bytesRead;

        bool returnB = ReadProcessMemory(pHandle, (LPVOID)address, &buffer, sizeof(buffer), &bytesRead);

        if (bytesRead <= 0 || !returnB) {
            std::cout << "[Dynamic Disasm] [Error] Write failed at address and value of: " << address << "\n\n";
            return type();
        }

        return buffer;
    }

    template<typename type> type* DynDisasm::rpm(uintptr_t address, int offset, int count, type value) {
        return {};
    }

    template<typename type> bool DynDisasm::wpm(uintptr_t address, type value) {
        SIZE_T written;
        DWORD oldProtect;
        VirtualProtectEx(pHandle, (LPVOID)address, sizeof(type), PAGE_READWRITE, &oldProtect);
        bool returnB = WriteProcessMemory(pHandle, (LPVOID)address, &value, sizeof(type), &written);
        VirtualProtectEx(pHandle, (LPVOID)address, sizeof(type), oldProtect, &oldProtect);
        
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