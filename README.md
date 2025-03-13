# MediaLock DRM Simulator

![License](https://img.shields.io/badge/license-MIT-blue.svg)
![Version](https://img.shields.io/badge/version-1.0-green.svg)

## Overview

**MediaLock** is a Digital Rights Management (DRM) simulator built in C++ using the Crypto++ library for encryption and Qt for a graphical user interface (GUI). It demonstrates a basic DRM system by allowing users to:
- Generate a machine-specific license key.
- Encrypt media files (e.g., text, video) to protect them.
- Decrypt files only on the licensed machine.

The project started as a command-line tool and evolved into a GUI application, showcasing both cryptographic security and user-friendly design. It’s a proof-of-concept for learning about DRM, encryption, and software development.

## Features

- **License Generation**: Creates a unique license tied to the machine’s disk serial number (or a random ID if unavailable), secured with HMAC-SHA256.
- **File Encryption/Decryption**: Uses AES-CBC encryption with keys stored securely in the license file, encrypted with a secret key.
- **Graphical Interface**: Built with Qt, featuring buttons for generating licenses, encrypting, and decrypting files, plus a log for status updates.
- **Machine Binding**: Ties licenses to a machine ID, simulating real DRM behavior.
- **Error Handling**: Robust checks for invalid licenses, file errors, and decryption failures.

## Why I Built This

I created **MediaLock** to:
- **Learn Cryptography**: Dive into practical encryption with Crypto++ (AES, HMAC) and understand how DRM protects content.
- **Explore C++**: Strengthen my skills in modern C++ (C++17), CMake, and GUI development with Qt.
- **Simulate Real-World Systems**: Replicate a simplified version of commercial DRM (e.g., Widevine, FairPlay) to grasp their mechanics.
- **Portfolio Project**: Build something tangible to showcase problem-solving, coding, and tool integration.

It began as a fun experiment with file encryption and grew into a full-fledged simulator as I added licensing, machine binding, and a GUI. The journey taught me about secure key management, UI design, and integrating third-party libraries.

## Prerequisites

To build and run **MediaLock**, you’ll need:
1. **Visual Studio 2022**: With C++ Desktop Development workload (includes MSVC and NMake).
2. **CMake**: Version 3.10 or higher (download from [cmake.org](https://cmake.org/download/)).
3. **Crypto++**: Cryptographic library for encryption (instructions below) 
4. **Qt 6**: For the GUI (e.g., 6.6.2 with MinGW 64-bit kit, download from [qt.io](https://www.qt.io/download-qt-installer)).
5. **Windows**: Tested on Windows 10/11 (uses Windows APIs for machine ID).

## Building the Project

### 1. Set Up Crypto++
1. Download Crypto++ from [cryptopp.com](https://www.cryptopp.com/#download).
2. Extract to `C:\cryptopp`.
3. Build the library:
   - Open `cryptlib.vcxproj` in Visual Studio 2022.
   - Set configuration to **Debug**, platform to **x64**.
   - Build the solution (outputs `cryptlib.lib` in `C:\cryptopp\x64\Output\Debug`).

### 2. Install Qt
1. Run the Qt installer from [qt.io](https://www.qt.io/download-qt-installer).
2. Select Qt 6.6.2 (or latest) with the MinGW 64-bit kit.
3. Install to `C:\Qt` (e.g., `C:\Qt\6.6.2\mingw_64`).
4. Add `C:\Qt\6.6.2\mingw_64\bin` to your system PATH.

### 3. Clone the Repository
```bash
git clone https://github.com/JOEL/MediaLock.git
cd MediaLock.

cd C:\Users\JOEL\Documents\GitHub\MediaLock\build
del /Q *           # Clean build directory
cmake .. -G "NMake Makefiles"
nmake

Running MediaLock
MediaLock.exe
