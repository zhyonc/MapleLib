# MapleLib
MapleLib is a lightweight library designed to simplify function hooking and memory editing for MapleStory Client

## Features
- AOBHelper: Hooks functions via `Detours`
- AOBScan: Matches byte patterns in memory regions
- Logger:  Outputs debug messages with `OutputDebugStringW`
- ProcessHelper: Provides process-level analysis
- Rosemary: Edits and scans memory
- StringHelper: Converts and formats string

## How to build
```powershell
git clone --recurse-submodules https://github.com/zhyonc/MapleLib.git
cd MapleLib
mkdir build
cd build
cmake .. -G "Visual Studio 17 2022" -A Win32
```
- Open `MapleLib.sln` via Visual Studio 2022
- Select `Release Win32` mode and build solution

## How to use
- Right-click on your project in Solution Explorer
- Open `Property Pages` and Select `VC++ Directories`
- Set `Include Directories` is `$(ProjectDir)include`
- Set `Library Directories` is `$(ProjectDir)lib`
- Copy `MapleLib.h` file to your project's `include` directory
- Copy `MapleLib.lib` file to your project's `lib` directory
- In your `pch.h` file or any other .cpp file import these codes
```cpp
// Set ture to show debug messages
#ifndef IS_DEBUG_MODE
#define IS_DEBUG_MODE true
#endif

// Include MapleLib and link its library
#include "MapleLib.h"
#pragma comment(lib, "MapleLib.lib")
```
- Recommend using [DebugView](https://learn.microsoft.com/en-us/sysinternals/downloads/debugview) to see debug log
## Credits
- [RiremitoTools](https://github.com/Riremito/tools)
- [Detours](https://github.com/microsoft/Detours)