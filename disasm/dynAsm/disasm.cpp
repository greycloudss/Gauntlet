#include "disasm.h"
#include <stdio.h>
#include <stdint.h>
#include <ctype.h>

#define isascii_text(c) (isprint(c) || (c) == '\n' || (c) == '\r' || (c) == '\t')

namespace ASM {
    BOOL DynDisasm::isValidPointer(uintptr_t ptr) {
        MEMORY_BASIC_INFORMATION memInfo;
        if (VirtualQueryEx(pHandle, (LPCVOID)ptr, &memInfo, sizeof(memInfo)) == 0) return FALSE;
        return memInfo.State == MEM_COMMIT && (memInfo.Protect & (PAGE_READWRITE | PAGE_EXECUTE_READWRITE | PAGE_READONLY));
    }

    void DynDisasm::AnalyzeMemory(char* buffer, uintptr_t baseAddr, SIZE_T size) {
        SIZE_T i = 0;
    
        while (i < size) {
            if (isascii_text(buffer[i]) && isascii_text(buffer[i + 1])) {
                SIZE_T strLen = 0;

                while (i + strLen < size && isascii_text(buffer[i + strLen]) && strLen < 256) strLen++;

                printf("0x%p |       size = %llu | [STRING] \"%.*s\"\n", (void*)(baseAddr + i), strLen, (int)strLen, buffer + i);
                i += strLen;
                continue;
            }

            if (i + 8 <= size) {
                uintptr_t ptr;
                memcpy(&ptr, buffer + i, sizeof(ptr));
                if (isValidPointer( ptr)) {
                    printf("0x%p |       size = 8 | [POINTER] 0x%p\n", (void*)(baseAddr + i + 8));
                    i += 8;
                    continue;
                }
            }

            if (i + 4 <= size) {
                int val;
                memcpy(&val, buffer + i, 4);
                if (val >= -1000000 && val <= 1000000 && val != 0) {
                    printf("0x%p |       size = 4 | [INT] %d\n", (void*)(baseAddr + i), val);
                    i += 4;
                    continue;
                }
            }
    

            if (i + 4 <= size) {
                float f;
                memcpy(&f, buffer + i, 4);
                if (f > -1e6 && f < 1e6 && f == f && f != 0.0f) {
                    printf("0x%p |       size = 4 | [FLOAT] %.3f\n", (void*)(baseAddr + i), f);
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
                PVOID regionBase = memInfo.BaseAddress;
                char* buffer = (char*)malloc(regionSize);

                SIZE_T bytesRead;

                

                if (ReadProcessMemory(pHandle, regionBase, buffer, regionSize, &bytesRead)) AnalyzeMemory(buffer, reinterpret_cast<uintptr_t>(regionBase), regionSize);
                
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




    /* really ugly and i dont want to use this but for now this is a temp fix. im really sorry*/
    template<typename valType> TypeE typeViaID(valType val) {
        const std::type_info& t = typeid(val);
        if (t == typeid(int))                                       return INTn;
        if (t == typeid(float))                                     return FLOATn;
        if (t == typeid(double))                                    return DOUBLEn;
        if (t == typeid(short))                                     return SHORTn;
        if (t == typeid(long))                                      return LONGn;
        if (t == typeid(unsigned short))                            return SHORTn;
        if (t == typeid(unsigned int))                              return UNSIGNED;
        if (t == typeid(unsigned long))                             return ULONGn;
        if (t == typeid(int64_t))                                   return INT64n;
        if (t == typeid(uint64_t))                                  return UINT64n;
        if (t == typeid(char))                                      return CHARn;
        if (t == typeid(wchar_t))                                   return WCHARn;
        if (t == typeid(const char*) || t == typeid(char*))         return CHARPTR;
        if (t == typeid(const wchar_t*) || t == typeid(wchar_t*))   return WCHARPTR;
        if (t == typeid(void*))                                     return VOIDPTR;
        if (t == typeid(bool))                                      return BOOLn;
        if (t == typeid(std::string))                               return STRING;
    
        return UNKNOWN;
    }

    template<typename type> triple<uintptr_t, void*, TypeE>* DynDisasm::rpm(uintptr_t address[], int offsets[], int count) {
        triple<uintptr_t, void*, TypeE>* returnT = (triple<uintptr_t, void*, TypeE>*) malloc(sizeof(triple<uintptr_t, void*, TypeE>) * count);

        
        for (int i = 0; i < count; ++i) {
            if (!address[i] || offsets == nullptr) break;

            type val = rpm<triple<uintptr_t, void*, TypeE>>(address[i] + offsets[i]);

            returnT[i] = triple(address[i] + offsets[i], (void*)val, typeViaID(val));
        }
        
        return returnT;
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