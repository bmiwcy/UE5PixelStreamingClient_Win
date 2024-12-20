# UE5PixelStreamingClient_Win

This project demonstrates a Pixel Streaming Client for Unreal Engine 5 using WebRTC and WebSocket technologies. It includes precompiled libraries and dependencies to streamline the setup process for Windows users.

## Table of Contents

- [Introduction](#introduction)
- [Requirements](#requirements)
- [Features](#features)
- [Build Process](#build-process)
- [Usage](#usage)
- [License](#license)

---

## Introduction

The `UE5PixelStreamingClient_Win` project is designed to work with Unreal Engine's Pixel Streaming technology. It enables communication with a signaling server and supports WebRTC video streaming for interactive applications.

---

## Requirements

Before building the project, ensure you have the following:

- **Operating System**: Windows 10 or later
- **Compiler**: Visual Studio 2022 (with C++ development tools installed)
- **CMake**: Version 3.14 or later
- **Dependencies**: Precompiled libraries are included in the `third_party` directory:
  - WebRTC (5414)
  - libWebSockets
  - OpenSSL (1.1.1t)
  - JsonCpp

---

## Features

- Communicate with a signaling server via WebSocket.
- Integrate WebRTC for real-time video and audio streaming.
- Utilize precompiled libraries for faster setup.
- Compatible with Unreal Engine's Pixel Streaming server.

---

## Build Process

Follow these steps to build the project:

### 1. Clone the Repository

Clone this repository to your local machine:
```bash
git clone https://github.com/bmiwcy/UE5PixelStreamingClient_Win.git
cd UE5PixelStreamingClient_Win
```

### 2. Install CMake
Ensure CMake is installed and available in your system's PATH. You can download it from CMake Official Website.

### 3. Configure the Project
Create a build directory and configure the project using CMake:
```bash
mkdir build
cd build
cmake .. -G "Visual Studio 17 2022" -A x64
```

### 4. Build the Project
Compile the project in Release mode:
```bash
cmake --build . --config Release
```

---

## Usage 
### Run the Executable
Navigate to the build/Release directory and run the executable:
```bash
.\PixelStreamingClient.exe
```
### Expected Output

If the setup is correct, you should see the following:

- **OpenSSL Version**: The version of OpenSSL being used (e.g., `OpenSSL 1.1.1t`).
- **libWebSockets Version**: The version of libWebSockets being used (e.g., `3.0.0`).

---

## Notes

- **Signaling Server:** Ensure you have a signaling server running (e.g., Unreal Engine Pixel Streaming Signaling Server) and update the SIGNALING_SERVER_ADDRESS in the source code if necessary.
- **Dependencies:** The third_party directory contains precompiled libraries required for the project. Do not remove or modify these files unless necessary.

---

## License
This project is licensed under the MIT License. See the LICENSE file for details.

---

If you need further assistance or encounter issues, feel free to open an issue in the repository or contact the project maintainer. 😊