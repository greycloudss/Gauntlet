#pragma once
#include "../Mnemonics.h"
#include "../triple.h"
#include "typeEnum.h"
#include <typeinfo>
#include <tlhelp32.h>
#include <stdio.h>
#include <ctype.h>


namespace ASM {
    class DynDisasm {
        private:
            LPCSTR process;
            HANDLE pHandle;
            DWORD pid;
            
            void getPid();
            BOOL isValidPointer(uintptr_t ptr);
            void AnalyzeMemory(char* buffer, uintptr_t baseAddr, SIZE_T size);

        public:
            std::vector<std::string> toPrint;
            std::mutex toPrintMutex;

            DynDisasm() = default;

            inline void scan() {
                if (!pHandle) {
                    getPid();
        
                    pHandle = OpenProcess(PROCESS_VM_WRITE | PROCESS_VM_OPERATION | PROCESS_VM_READ | PROCESS_QUERY_INFORMATION | PROCESS_CREATE_THREAD, FALSE, pid);
                }

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
            
            DynDisasm(LPCSTR process);
            template<typename valType> TypeE typeViaID(valType val);

            template<typename type> type rpm(uintptr_t address) {
                type buffer{};
                SIZE_T bytesRead;
                SIZE_T written;
                DWORD oldProtect;
        
                VirtualProtectEx(pHandle, (LPVOID)address, sizeof(type), PAGE_READWRITE, &oldProtect);
                bool returnB = ReadProcessMemory(pHandle, (LPVOID)address, &buffer, sizeof(buffer), &bytesRead);
                VirtualProtectEx(pHandle, (LPVOID)address, sizeof(type), oldProtect, &oldProtect);
                
                if (bytesRead <= 0 || !returnB) {
                    std::cout << "[Dynamic Disasm] [Error] Write failed at address and value of: " << address << "\n\n";
                    return type();
                }
        
                return buffer;
            }
        
            template<typename type> bool wpm(uintptr_t address, type value) {
                SIZE_T written;
                DWORD oldProtect;
                VirtualProtectEx(pHandle, (LPVOID)address, sizeof(type), PAGE_READWRITE, &oldProtect);
                bool returnB = WriteProcessMemory(pHandle, (LPVOID)address, &value, sizeof(type), &written);
                VirtualProtectEx(pHandle, (LPVOID)address, sizeof(type), oldProtect, &oldProtect);
                
                if (written <= 0 || !returnB) std::cout << "[Dynamic Disasm] [Error] Write failed at address and value of: " << address << ", " << value << "\n\n";
        
                return returnB;
            }

            template<typename type> triple<uintptr_t, void*, TypeE>* rpm(uintptr_t address[], int offsets[], int count);

            bool decastPtr(triple<uintptr_t, void*, TypeE>& trpl);
            std::vector<bool> wpm(std::vector<triple<uintptr_t, void*, TypeE>> writes);

            void setPName(LPCSTR process);  

            ~DynDisasm() {
                process = NULL;
                pHandle = NULL;

                pid = {};

                toPrint.clear();
                toPrint.shrink_to_fit();
            }
    };
};