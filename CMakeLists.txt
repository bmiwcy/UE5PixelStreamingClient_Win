# Project name and version
cmake_minimum_required(VERSION 3.14)
project(PixelStreamingClient)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)

# Set compiler warning and optimization options
if (MSVC)
    add_compile_options(/W4)
else ()
    add_compile_options(-Wall -Wextra -pedantic -Werror)
endif ()

# Include directories for header files
include_directories(
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/third_party/WebRTC/5414/Include
    ${CMAKE_SOURCE_DIR}/third_party/WebRTC/5414/Include/third_party/abseil-cpp
    ${CMAKE_SOURCE_DIR}/third_party/WebRTC/CompatInclude
    ${CMAKE_SOURCE_DIR}/third_party/libWebSockets/include/Win64/VS2015
    ${CMAKE_SOURCE_DIR}/third_party/OpenSSL/1.1.1t/include/Win64/VS2015
    ${CMAKE_SOURCE_DIR}/third_party/jsoncpp/include
    ${CMAKE_SOURCE_DIR}/third_party/libvpx/include
    ${CMAKE_SOURCE_DIR}/third_party/opus/include
)

# Add library directories
link_directories(
    ${CMAKE_SOURCE_DIR}/third_party/WebRTC/5414/Lib/Win64/Release
    ${CMAKE_SOURCE_DIR}/third_party/libWebSockets/lib/Win64/VS2015/Release
    ${CMAKE_SOURCE_DIR}/third_party/OpenSSL/1.1.1t/lib/Win64/VS2015/Release
    ${CMAKE_SOURCE_DIR}/third_party/jsoncpp/lib
    ${CMAKE_SOURCE_DIR}/third_party/libvpx/lib/x64
    ${CMAKE_SOURCE_DIR}/third_party/libOpus/lib/x64
)

# Add preprocessor definitions
add_definitions(
    -DWIN32
    -D_WINDOWS
    -DWEBRTC_WIN
    -DNOMINMAX
    -D_CRT_SECURE_NO_WARNINGS
    -D_ITERATOR_DEBUG_LEVEL=0
)

# Define the executable file
add_executable(
    PixelStreamingClient 
    src/main.cpp
    src/signaling_client.cpp
    src/webrtc_manager.cpp
    src/video_stream_configuration.cpp
)

# Link necessary libraries
target_link_libraries(PixelStreamingClient
    webrtc.lib
    websockets_static.lib
    libssl.lib
    libcrypto.lib
    ws2_32
    Crypt32.lib
    Iphlpapi.lib
    jsoncpp.lib
    winmm
    Secur32
    vpx.lib
    opus.lib
)

add_custom_command(TARGET PixelStreamingClient POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "${CMAKE_SOURCE_DIR}/third_party/libvpx/bin/x64/vpx.dll"
    $<TARGET_FILE_DIR:PixelStreamingClient>
)

add_custom_command(TARGET PixelStreamingClient POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "${CMAKE_SOURCE_DIR}/third_party/libOpus/bin/x64/opus.dll"
    $<TARGET_FILE_DIR:PixelStreamingClient>
)
