# Gauntlet

**Gauntlet** is a C++ reverse engineering utility that supports both static and dynamic disassembly, along with DLL injection into external processes. Built for modularity and performance, it provides core building blocks for memory manipulation, opcode decoding, and instruction tracing.

**Note:** This project is a work in progress and under active development.

---

## Project Structure

```
Gauntlet/
├── inject/                 # DLL injection system
│   ├── injector.h
│   └── injector.cpp
├── disasm/
│   ├── Mnemonics.h         # Opcode and addressing mode tables
│   ├── triple.h            # Utility template for grouped values
│   ├── dynAsm/             # Dynamic disassembler (hooking live processes)
│   │   ├── typeEnum.h
│   │   ├── disasm.h
│   │   └── disasm.cpp
│   └── statAsm/            # Static disassembler (binary analysis)
│       ├── disasm.h
│       └── disasm.cpp
├── main.cpp                # Entry point
├── main.h                  # Runtime and threading logic
└── README.md
```

---

## Features

- DLL injection via `CreateRemoteThread` + `LoadLibrary`
- Static disassembly of binary `.text` sections only
- Dynamic scanning of live processes (planned)
- Opcode decoding and ModRM resolution
- Smart decoding with junk filtering
- Multi-threaded argument parsing
- Modular, pluggable architecture

---

## Usage

### Build (Linux/MinGW/Windows)
Requires a C++17 compatible compiler.

```bash
# Build example
g++ -std=c++17 -g main.cpp inject/injector.cpp disasm/dynAsm/disasm.cpp disasm/statAsm/disasm.cpp -o gauntlet.exe -static-libgcc -static-libstdc++
```

### Run
```bash
./gauntlet.exe -inj target.exe -sAsm binary.exe -dAsm process.exe
```

- `-inj <exe>`: Inject DLL into a target process
- `-sAsm <exe>`: Run static disassembly on a PE binary
- `-dAsm <exe>`: (Planned) Attach and scan a live process

---

## Output

- Static disassembly will print to console and output to `disasm_<file>.txt`
- Only valid x86/x64 instructions from the `.text` section are processed
- Invalid opcodes or excessive junk are skipped

---

## Roadmap

- Live memory patching and write-back
- Advanced opcode/mnemonic plugins
- Visual frontend for instruction tracing
- Signature scanning + pattern matching

---

## License

MIT License — feel free to fork, modify, and contribute.
