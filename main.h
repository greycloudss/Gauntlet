#pragma once
#include "inject\injector.h"
#include "disasm\dynAsm\disasm.h"
#include "disasm\statAsm\disasm.h"
#include "gui\menu.h"
#include <iostream>
#include <vector>
#include <windows.h>
#include <stdio.h>

namespace MAIN {
    INJ::Injector* injector = nullptr;
    ASM::StatDisasm* sAsm = nullptr;
    ASM::DynDisasm* dAsm = nullptr;

    DWORD WINAPI statScanThread(LPVOID params) {
        sAsm = new ASM::StatDisasm(((std::vector<LPCSTR>*)params)->at(0));
        sAsm->disassemble();
        return 0;
    }

    DWORD WINAPI dynScanThread(LPVOID params) {
        dAsm = new ASM::DynDisasm(((std::vector<LPCSTR>*)params)->at(0));

        return 0;
    }

    DWORD WINAPI injectionThread(LPVOID params) {
        injector = new INJ::Injector(((std::vector<LPCSTR>*)params)->at(0));
        std::cout << injector->inject();
        return 0;
    }


    /* ((std::vector<LPCSTR>*)params)->at(0) looks nasty but for now itll do as a placeholder */

    class Payload {
        private:

            std::vector<LPCSTR>* injArgs = new std::vector<LPCSTR>();
            std::vector<LPCSTR>* statArgs = new std::vector<LPCSTR>();
            std::vector<LPCSTR>* dynArgs = new std::vector<LPCSTR>();

            HANDLE iHandle, dHandle, sHandle;
        
            void parser(int argc, const char* args[]) {
                char mode = {};

                for (int i = 1; i < argc; ++i) {
                    std::string arg(args[i]);

                    std::cout << "[DEBUG] Argument check: argv[" << i << "]: " << args[i] << "\n";
                    if (arg == "-sAsm") {mode = 0; continue;}
                    if (arg == "-dAsm") {mode = 1; continue;}
                    if (arg == "-inj") {mode = 2; continue;}
                
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
            
                OpenDebugConsole();
                
                if (injArgs->size() != 0) {
                    iHandle = CreateThread(NULL, 0, injectionThread, (LPVOID)injArgs, 0, NULL);
                    WaitForSingleObject(iHandle, INFINITE);
                }
            
                if (statArgs->size() != 0) sHandle = CreateThread(NULL, 0, statScanThread, (LPVOID)statArgs, 0, NULL);
                
            
                if (dynArgs->size() != 0) dHandle = CreateThread(NULL, 0, dynScanThread, (LPVOID)dynArgs, 0, NULL);
                
                //MENU::dAsm = dAsm;
                //MENU::sAsm = sAsm;
                //MENU::injector = injector;
//
                //CreateThread(nullptr, 0, [](LPVOID) -> DWORD {
                //    MENU::Main(GetModuleHandle(nullptr));
                //    return 0;
                //}, nullptr, 0, nullptr);
                

                WaitForSingleObject(dHandle, INFINITE);
                WaitForSingleObject(sHandle, INFINITE);
                WaitForSingleObject(iHandle, INFINITE);
            }

            ~Payload() {
                delete injArgs;
                delete statArgs;
                delete dynArgs;
            }
            
    };
};