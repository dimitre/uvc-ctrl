cmake_minimum_required(VERSION 3.16)
project(uvc_ctrl CXX)


set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(uvc_ctrl uvc-ctrl.cpp)

target_compile_definitions(uvc_ctrl PRIVATE
    _WIN32_WINNT=0x0A00
    WINVER=0x0A00
)

target_link_libraries(uvc_ctrl PRIVATE
    mfplat mf mfreadwrite mfuuid
    strmiids shlwapi ole32 oleaut32
)
