#include "injector.h"

namespace INJ {
        int printErr(int errCode) {
            std::cout << "[ERROR] Error encountered: "<< errCode  << "\n";
            return errCode;
        }

        Injector::Injector(LPCSTR process) : processName(process), dllName("debugger.dll"), injectorResponse(-1) { }

        DWORD Injector::getPid() {
            HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

            PROCESSENTRY32 pEntry;

            pEntry.dwSize = sizeof(pEntry);

            if (Process32First(snapshot, &pEntry)) {
                do {
                    if (strcmp((const char*)pEntry.szExeFile, this->processName) == 0) {
                        CloseHandle(snapshot);
                        return pEntry.th32ProcessID;
                    }

                } while (Process32Next(snapshot, &pEntry));
            }

            CloseHandle(snapshot);
            return 0;
        }

        int Injector::inject() {
            PID = getPid();
            if (!PID) return injectorResponse = printErr(1);

            pHandle = OpenProcess(PROCESS_VM_OPERATION | PROCESS_VM_WRITE | PROCESS_CREATE_THREAD, FALSE, PID);
            if (!pHandle) return injectorResponse = printErr(2);

            char buff[MAX_PATH];
            GetFullPathNameA(dllName, MAX_PATH, buff, nullptr);
            std::string path(buff);

            LPVOID alloca = VirtualAllocEx(pHandle, NULL, path.length() + 1, MEM_COMMIT, PAGE_READWRITE); 
            if (!alloca) return injectorResponse = printErr(3);

            if (!WriteProcessMemory(pHandle, alloca, (void*)(path.c_str()), path.length() + 1, nullptr)) {
                VirtualFreeEx(pHandle, alloca, 0, MEM_RELEASE);
                return injectorResponse = printErr(5);
            }

            HANDLE hThread = CreateRemoteThread(pHandle, nullptr, 0, (LPTHREAD_START_ROUTINE)GetProcAddress(GetModuleHandleA("kernel32.dll"), "LoadLibraryA"), alloca, 0, nullptr);
            
            if (!hThread) {
                DWORD err = GetLastError();
                std::cout << "[Error] failed to create thread: " << err;
            }
            

            if (!hThread) {
                VirtualFreeEx(pHandle, alloca, 0, MEM_RELEASE);
                return injectorResponse = printErr(6);
            }
    
            WaitForSingleObject(hThread, INFINITE);

            return injectorResponse;
        }
}