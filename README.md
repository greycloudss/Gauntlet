# Gauntlet

**Gauntlet** is a C++ reverse engineering utility that supports both static and dynamic disassembly, along with DLL injection into external processes. Built for modularity and performance, it provides core building blocks for memory manipulation, opcode decoding, and instruction tracing.

**Note:** This project is a work in progress and under active development.

---

## Project Structure

```
Gauntlet/
├── inject/             # DLL injection system
│   ├── injector.h
│   └── injector.cpp
├── disasm/
│   ├── dynAsm/         # Dynamic disassembler (hooking live processes)
│   │   ├── disasm.h
│   │   └── disasm.cpp
│   └── statAsm/        # Static disassembler (binary analysis)
│       ├── disasm.h
│       └── disasm.cpp
├── Mnemonics.h         # Opcode and addressing mode tables
├── triple.h            # Utility template for grouped values
├── main.cpp            # Entry point
├── main.h              # Runtime and threading logic
└── README.md
```

---

## Features

- DLL injection via `CreateRemoteThread` + `LoadLibrary`
- Static disassembly of binary files
- Dynamic scanning of running processes
- Opcode decoding and ModRM analysis
- Modular architecture
- Multi-threaded command-line parsing and execution

---

## Usage

### Build (Linux/MinGW/Windows)
Requires a C++20 compatible compiler.

```bash
# Example using g++
g++ -std=c++20 -g \
    main.cpp \
    inject/injector.cpp \
    disasm/dynAsm/disasm.cpp \
    disasm/statAsm/disasm.cpp \
    -o gauntlet.exe
```

### Run
```bash
./gauntlet.exe -inj target.exe -sAsm binary.bin -dAsm process.exe
```

- `-inj <exe>`: inject DLL into a target process
- `-sAsm <bin>`: run static disassembly on a file
- `-dAsm <exe>`: attach to and scan a live process

---

## Notes

- The current implementation assumes the DLL is named `debugger.dll`.
- Argument parsing is thread-dispatched; each action runs in a separate thread.
- Output is logged to standard output.

---

## Roadmap

- Memory patching and live write-back
- Improved mnemonic resolution
- Plugin-style instruction handlers
- UI overlay for disassembly visualization

---

## License

MIT License — feel free to fork, modify, and contribute.

