// cl /EHsc /std:c++17 uvc_ctrl.cpp  mfplat.lib mf.lib mfreadwrite.lib strmiids.lib shlwapi.lib ole32.lib oleaut32.lib

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#include <mfapi.h>
#include <mfidl.h>
#include <mfreadwrite.h>
#include <dshow.h>
#include <shlwapi.h>
#include <cstdio>

#pragma comment(lib, "mfplat.lib")
#pragma comment(lib, "mf.lib")
#pragma comment(lib, "mfreadwrite.lib")
#pragma comment(lib, "strmiids.lib")
#pragma comment(lib, "shlwapi.lib")
#pragma comment(lib, "ole32.lib")
#pragma comment(lib, "oleaut32.lib")

template <class T> void SafeRelease(T** pp) { if (*pp) { (*pp)->Release(); *pp = nullptr; } }

void PrintPropRange(IAMVideoProcAmp* vpa, VideoProcAmpProperty id, const char* name)
{
    long mn, mx, step, def, caps;
    if (SUCCEEDED(vpa->GetRange(id, &mn, &mx, &step, &def, &caps)))
        printf("%-20s  min=%4ld  max=%4ld  step=%3ld  default=%4ld  flags=%#lx\n",
               name, mn, mx, step, def, caps);
}

void PrintCamRange(IAMCameraControl* cam, CameraControlProperty id, const char* name)
{
    long mn, mx, step, def, caps;
    if (SUCCEEDED(cam->GetRange(id, &mn, &mx, &step, &def, &caps)))
        printf("%-20s  min=%4ld  max=%4ld  step=%3ld  default=%4ld  flags=%#lx\n",
               name, mn, mx, step, def, caps);
}

int main()
{
    CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
    MFStartup(MF_VERSION);

    IMFAttributes* attrs = nullptr;
    IMFActivate** acts   = nullptr;
    UINT32         count = 0;
    MFCreateAttributes(&attrs, 1);
    attrs->SetGUID(MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE, MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_VIDCAP_GUID);
    MFEnumDeviceSources(attrs, &acts, &count);
    if (count == 0) { puts("No camera"); return 0; }

    IMFMediaSource* source = nullptr;
    acts[0]->ActivateObject(__uuidof(IMFMediaSource), (void**)&source);

    IAMVideoProcAmp*  vpa = nullptr;
    IAMCameraControl* cam = nullptr;
    source->QueryInterface(&vpa);
    source->QueryInterface(&cam);

    puts("\n=== VideoProcAmp (brightness, contrast, etc.) ===");
    if (vpa) {
        PrintPropRange(vpa, VideoProcAmp_Brightness, "Brightness");
        PrintPropRange(vpa, VideoProcAmp_Contrast,   "Contrast");
        PrintPropRange(vpa, VideoProcAmp_Hue,        "Hue");
        PrintPropRange(vpa, VideoProcAmp_Saturation, "Saturation");
        PrintPropRange(vpa, VideoProcAmp_Sharpness,  "Sharpness");
        PrintPropRange(vpa, VideoProcAmp_Gamma,      "Gamma");
        PrintPropRange(vpa, VideoProcAmp_Gain,       "Gain");
        PrintPropRange(vpa, VideoProcAmp_WhiteBalance, "WhiteBalance");

        // example: set brightness to mid-range
        long mn, mx, step, def, caps;
        vpa->GetRange(VideoProcAmp_Brightness, &mn, &mx, &step, &def, &caps);
        long mid = (mn + mx) / 2;
        vpa->Set(VideoProcAmp_Brightness, mid, VideoProcAmp_Flags_Manual);
        printf("\nSet brightness → %ld\n", mid);
    }

    puts("\n=== CameraControl (exposure, focus, zoom, etc.) ===");
    if (cam) {
        PrintCamRange(cam, CameraControl_Exposure, "Exposure");
        PrintCamRange(cam, CameraControl_Focus,    "Focus");
        PrintCamRange(cam, CameraControl_Zoom,     "Zoom");
        PrintCamRange(cam, CameraControl_Pan,      "Pan");
        PrintCamRange(cam, CameraControl_Tilt,     "Tilt");
        PrintCamRange(cam, CameraControl_Roll,     "Roll");
    }

    SafeRelease(&vpa);
    SafeRelease(&cam);
    SafeRelease(&source);
    for (UINT32 i = 0; i < count; ++i) acts[i]->Release();
    CoTaskMemFree(acts);
    SafeRelease(&attrs);
    MFShutdown();
    CoUninitialize();
    return 0;
}
