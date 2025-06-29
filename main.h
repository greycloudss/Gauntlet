#pragma once
#include "inject\injector.h"
#include "disasm\dynAsm\disasm.h"
#include "disasm\statAsm\disasm.h"
#include "gui\menu.h"
#include <iostream>
#include <vector>
#include <windows.h>
#include <stdio.h>
#include "util/threads.h"

namespace MAIN {
    INJ::Injector* injector = new INJ::Injector();
    ASM::StatDisasm* sAsm = new ASM::StatDisasm();
    ASM::DynDisasm* dAsm = new ASM::DynDisasm();

    /* ((std::vector<LPCSTR>*)params)->at(0) looks nasty but for now itll do as a placeholder */

    class Payload {
        private:

            std::vector<LPCSTR>* injArgs = new std::vector<LPCSTR>();
            std::vector<LPCSTR>* statArgs = new std::vector<LPCSTR>();
            std::vector<LPCSTR>* dynArgs = new std::vector<LPCSTR>();

            HANDLE iHandle, dHandle, sHandle, mHandle;

            bool menuMode = false;
            bool noConsole = false;

            void parser(int argc, const char* args[]) {
                char mode = {};

                for (int i = 1; i < argc; ++i) {
                    std::string arg(args[i]);

                    std::cout << "[DEBUG] Argument check: argv[" << i << "]: " << args[i] << "\n";
                    if (arg == "-sAsm") {mode = 0; continue;}
                    if (arg == "-dAsm") {mode = 1; continue;}
                    if (arg == "-inj") {mode = 2; continue;}
                    if (arg == "-m") {menuMode = true; continue;}
                    if (arg == "-nc") {noConsole = true; continue;}
                
                    switch (mode) {
                        case 0:
                            statArgs->push_back(args[i]);
                            break;

                        case 1:
                            dynArgs->push_back(args[i]);
                            break;

                        case 2:
                            injArgs->push_back(args[i]);
                            break;

                        default: continue;
                    }
                }
            }
        public:

            void OpenDebugConsole() {
                AllocConsole();
                FILE* f;
                freopen_s(&f, "CONOUT$", "w", stdout);
                freopen_s(&f, "CONOUT$", "w", stderr);
                std::cout << "[DEBUG] Console allocated.\n";
            }
        
            Payload(int argc, const char* args[]) {
                parser(argc, args);
                
                if (!noConsole) OpenDebugConsole();

                if (injArgs->size() != 0) {
                    delete injector;
                    injector = new INJ::Injector(injArgs->at(0));
                    iHandle = CreateThread(NULL, 0, injectionThread, (LPVOID)injector, 0, NULL);
                }
            
                if (statArgs->size() != 0) {
                    delete sAsm;
                    sAsm = new ASM::StatDisasm(statArgs->at(0));
                    sHandle = CreateThread(NULL, 0, statScanThread, (LPVOID)dAsm, 0, NULL);
                }
            
                if (dynArgs->size() != 0) {
                    delete dAsm;
                    dAsm = new ASM::DynDisasm(dynArgs->at(0));
                    dHandle = CreateThread(NULL, 0, dynScanThread, (LPVOID)dAsm, 0, NULL);
                }
                
                if (menuMode) {
                    MENU::setObjects(injector, sAsm, dAsm);
                    mHandle = CreateThread(nullptr, 0, [](LPVOID) -> DWORD { MENU::Main(GetModuleHandle(nullptr)); return 0; }, nullptr, 0, nullptr);
                }

                if (dHandle) WaitForSingleObject(dHandle, INFINITE);
                if (sHandle) WaitForSingleObject(sHandle, INFINITE);
                if (iHandle) WaitForSingleObject(iHandle, INFINITE);
                if (mHandle) WaitForSingleObject(mHandle, INFINITE);
            }

            ~Payload() {
                delete injArgs;
                delete statArgs;
                delete dynArgs;
            }
            
    };
};