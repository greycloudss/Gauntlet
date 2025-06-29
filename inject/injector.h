#pragma once
#include <stdio.h>
#include <iostream>
#include <windows.h>
#include <tlhelp32.h>

namespace INJ {
    class Injector {
        private:
            LPCSTR processName, dllName;
            HANDLE pHandle, tHandle;
            DWORD PID;

            int injectorResponse;
        public:
            Injector(LPCSTR process);
            Injector() = default;
            void setProcessName(const char* dllName);
            void setDLL(const char* pName);
            int inject();
            DWORD getPid();

            ~Injector() {
                processName = NULL;
                dllName = NULL;
                pHandle = NULL;
                tHandle = NULL;
                PID = {};
            }
    };
};