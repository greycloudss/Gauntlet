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
            int inject();
            DWORD getPid();
    };
};