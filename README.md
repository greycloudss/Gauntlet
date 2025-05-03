# Gauntlet

**Gauntlet** is a C++ reverse engineering utility that supports both static and dynamic disassembly, along with DLL injection into external processes. Built for modularity and performance, it provides core building blocks for memory manipulation, opcode decoding, and instruction tracing.

> Part of the **Armourer** series of projects.  
> **Note:** This project is a under active development. Will not be able to fix or do anything towards this project until I finish my exam season :)

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

- **ImGui GUI Interface**  
  Fully interactive interface built with ImGui, including tabbed panels, list boxes, editable fields, and color theming.

- **Theme Customization**  
  Full color customization system using `ImGui::ColorEdit4`, with save/load to config file and runtime application.

- **Type-Agnostic Value Conversion**  
  Read/write logic automatically converts between `int`, `float`, `std::string`, and `uintptr_t` based on selected type.

- **Command-Line Flag Support**  
  Launch modes configurable via flags like `-sAsm`, `-dAsm`, `-m`, `-nc` to toggle between static/dynamic/injection/menu/console modes.
  
- **Runtime Logging to UI List**  
  Static and dynamic disassembly results are displayed directly in the UI with live updates.

---

## Dependencies

The project depends on the following libraries and system APIs:

| Dependency         | Description                                                                 |
|--------------------|-----------------------------------------------------------------------------|
| **ImGui**          | Immediate Mode GUI library used for rendering the interface (`gui/imgui/`)  |
| **DirectX 11**     | Used as the backend for ImGui rendering                                     |
| **Windows API**    | Core functionality like window management, threading, process/memory access |
| **C++17 STL**      | Includes `<thread>`, `<mutex>`, `<vector>`, `<string>`, `<filesystem>`, etc |

### Build-Time Requirements
- **g++** with C++17 support (tested with MinGW-w64)
- Windows SDK (for `d3d11.lib`, `dxgi.lib`, `user32.lib`, etc.)

### Optional Tools
- **Visual Studio Code** + C++ extension for editing
- **MSYS2 / MinGW** environment for easy Windows builds

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
├── inject/
│   ├── injector.cpp
│   └── injector.h
├── gui/
│   ├── colours.h
│   ├── menu.cpp
│   ├── menu.h
│   └── imgui/
│       ├── imconfig.h
│       ├── imgui.cpp
│       ├── imgui.h
│       ├── imgui_demo.cpp
│       ├── imgui_draw.cpp
│       ├── imgui_impl_dx11.cpp
│       ├── imgui_impl_dx11.h
│       ├── imgui_impl_win32.cpp
│       ├── imgui_impl_win32.h
│       ├── imgui_internal.h
│       ├── imgui_tables.cpp
│       ├── imgui_widgets.cpp
│       ├── imstb_rectpack.h
│       ├── imstb_textedit.h
│       └── imstb_truetype.h
├── .gitignore
├── imgui.ini
├── LICENSE
├── main.cpp
├── main.h
├── README.md
└── userconfig.cfg

```

## Usage

### Build (Linux/MinGW/Windows)
Requires a C++17 compatible compiler.

```bash
# Example build using MinGW-w64
g++ -std=c++17 -g main.cpp inject/injector.cpp disasm/dynAsm/disasm.cpp
 disasm/statAsm/disasm.cpp gui/menu.cpp gui/imgui/imgui.cpp
 gui/imgui/imgui_draw.cpp gui/imgui/imgui_widgets.cpp
 gui/imgui/imgui_tables.cpp gui/imgui/imgui_demo.cpp gui/imgui/imgui_impl_dx11.cpp
 gui/imgui/imgui_impl_win32.cpp -o gauntlet.exe -static-libgcc -static-libstdc++
 -ld3d11 -ldxgi -ldxguid -ldinput8 -ld3dcompiler -ldwmapi -luser32 -lgdi32 -mwindows
```

### Run
```bash
./gauntlet.exe -inj target.exe -sAsm binary.exe -dAsm process.exe -m -nc
```

- `-inj <exe>`: Inject DLL into a target process
- `-sAsm <exe>`: Perform static disassembly on a PE binary
- `-dAsm <exe>`: Attach to and scan a running process
- `-m`: Will turn on GUI
- `-nc`: Will not open up a console for the user. (it is bugged and will likely crash the program)
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
- Signature/pattern scanning utilities
- Lua support for modular extensions

---

## License

MIT License — fork, modify, and contribute freely.
