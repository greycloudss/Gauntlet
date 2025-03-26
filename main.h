#pragma once
#include "inject\injector.h"
#include "disasm\disasm.h"
#include <iostream>
#include <vector>
#include <windows.h>
#include <stdio.h>



INJ::Injector* injector = nullptr;
ASM::StatDisasm* sAsm = nullptr;

namespace MAIN {
    DWORD WINAPI scanThread(LPVOID params) {
        sAsm = new ASM::StatDisasm((LPCSTR)params);
        return 0;
    }

    DWORD WINAPI injectionThread(LPVOID params) {
        injector = new INJ::Injector((LPCSTR)params);
        return 0;
    }


    class Payload {
        private:
            std::vector<LPCSTR> vArgs;
            std::vector<HANDLE> threads;

            void convertArgs(int argc, const char* args_arr[]) {
                for (int i = 0; i < argc; ++i)
                    this->vArgs.push_back(LPCSTR(args_arr[i]));
            }

        public:
            Payload(int argc, const char* args[]) {
                convertArgs(argc, args);
            //     threads.push_back(CreateThread(NULL, 0, injectionThread, (LPVOID)vArgs.at(1), 0, NULL));
            //     threads.push_back(CreateThread(NULL, 0, scanThread, (LPVOID)vArgs.at(1), 0, NULL));
            }

            ~Payload() {
                vArgs.clear();
                threads.clear();
            }
    };
};