# Gauntlet

**Gauntlet** is a C++ reverse engineering utility that supports both static and dynamic disassembly, along with DLL injection into external processes. Built for modularity and performance, it provides core building blocks for memory manipulation, opcode decoding, and instruction tracing.

> **Note:** This project is a work in progress and under active development.

---
## Features

- **Static Disassembly (WIP)**  
  Analyze PE binaries through custom disassembly logic, resolving opcodes, ModRM bytes, and operand structures with smart filtering.

- **Dynamic Disassembly (Live Memory Scanner) (WIP)**  
  Attach to a running process and scan its memory for values interpreted dynamically as ints, floats, pointers, or strings. Results are printed with type heuristics and address ranges.

- **DLL Injection**  
  Inject external libraries into live processes using `CreateRemoteThread` + `LoadLibrary` strategy.

- **Memory Manipulation (WIP)**
  Read and write memory of target processes using `ReadProcessMemory` and `WriteProcessMemory`, with automatic protection flag handling via `VirtualProtectEx`.

- **Pattern Scanning (WIP)**  
  Byte-pattern search with future support for wildcards and pattern match masking.

- **Modular Architecture**  
  Disassemblers and injectors are organized into clearly separated modules (`statAsm`, `dynAsm`, `inject`) to enable extensibility.

- **Threaded Runtime**  
  All operations (injection, static analysis, dynamic scanning) are offloaded to independent threads, allowing concurrent or sequential workflows.

- **Debug Console Logging**  
  A dedicated debug console is automatically attached for all output, ensuring visibility of runtime logs across threads.

---

## Project Structure

```gauntlet/
├── .vscode/
│   ├── settings.json
│   └── tasks.json
├── disasm/
│   ├── dynAsm/
│   │   ├── disasm.cpp
│   │   ├── disasm.h
│   │   └── typeEnum.h
│   ├── statAsm/
│   │   ├── disasm.cpp
│   │   └── disasm.h
│   ├── Mnemonics.h
│   └── triple.h
├── gui/
│   ├── menu.cpp
│   ├── menu.h
│   ├── notification.h
│   ├── pane.h
│   └── tab.h
├── inject/
│   ├── injector.cpp
│   └── injector.h
├── LICENSE
├── main.cpp
├── main.h
└── README.md

```

## Usage

### Build (Linux/MinGW/Windows)
Requires a C++17 compatible compiler.

```bash
# Example build using MinGW-w64
g++ -std=c++17 -g \
    main.cpp \
    gui/menu.cpp \
    inject/injector.cpp \
    disasm/dynAsm/disasm.cpp \
    disasm/statAsm/disasm.cpp \
    -o gauntlet.exe \
    -static-libgcc -static-libstdc++
```

### Run
```bash
./gauntlet.exe -inj target.exe -sAsm binary.exe -dAsm process.exe
```

- `-inj <exe>`: Inject DLL into a target process
- `-sAsm <exe>`: Perform static disassembly on a PE binary
- `-dAsm <exe>`: (Planned) Attach to and scan a running process

---

## Output

- Disassembly is printed to both console and `disasm_<file>.txt`
- Only valid instructions in the `.text` section are processed
- Skips unknown or nonsensical byte sequences

---

## Roadmap

- Live memory patching + write-back
- Wildcard pattern matching for memory scanning
- Opcode/mnemonic plugin support
- Visualization UI for code flow
- Signature/pattern scanning utilities
- Lua support for modular extensions

---

## License

MIT License — fork, modify, and contribute freely.
