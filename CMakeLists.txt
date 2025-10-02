cmake_minimum_required(VERSION 3.16)
project(uvc_ctrl CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(uvc_ctrl uvc_ctrl.cpp)

# Tell CMake to use the Windows 10 SDK that already contains headers/libs
target_compile_definitions(uvc_ctrl PRIVATE
    _WIN32_WINNT=0x0A00   # Windows 10
    WINVER=0x0A00
)

# Core MF + DirectShow libs (all ship with Windows SDK)
target_link_libraries(uvc_ctrl PRIVATE
    mfplat      # MFStartup, IMFAttributes, …
    mf          # IMFMediaSource, MFEnumDeviceSources, …
    mfreadwrite # MFCreateSourceReaderFromMediaSource (optional)
    mfuuid      # GUIDs
    strmiids    # DirectShow CLSIDs  (IAMVideoProcAmp, IAMCameraControl)
    shlwapi     # String helpers
    ole32       # CoInitializeEx
    oleaut32    # Variant helpers (some MF helpers)
)

# Make sure we link the *unicode* entry point
target_link_options(uvc_ctrl PRIVATE /ENTRY:wWinMainCRTStartup)
