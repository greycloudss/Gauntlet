#pragma once
#include <iostream>
#include <vector>
#include <windows.h>




namespace MAIN {
    DWORD WINAPI scanThread(LPVOID params) {


        return 0;
    }

    DWORD WINAPI injectionThread(LPVOID params) {


        return 0;
    }

    class Payload {
        private:
            std::vector<std::string> args;
            std::vector<HANDLE> threads;


            void convertArgs(int argc, const char* args_arr[]) {
                for (int i = 0; i < argc; ++i)
                    this->args.push_back(std::string(args_arr[i]));
                
            }

        public:
            Payload(int argc, const char* args[]) {
                threads.push_back(CreateThread(NULL, 0, scanThread, NULL, 0, NULL));
                convertArgs(argc, args);
            }

            ~Payload() {
                args.clear();
                threads.clear();
            }
    };
};