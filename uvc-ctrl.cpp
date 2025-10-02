// cl /EHsc /std:c++17 uvc_ctrl.cpp  mfplat.lib mf.lib mfreadwrite.lib mfuuid.lib shlwapi.lib
#include <windows.h>
#include <mfapi.h>
#include <mfidl.h>
#include <mfreadwrite.h>
#include <dshow.h>        // IAMVideoProcAmp, IAMCameraControl

#pragma comment(lib, "mfplat.lib")
#pragma comment(lib, "mf.lib")
#pragma comment(lib, "mfreadwrite.lib")
#pragma comment(lib, "mfuuid.lib")
#pragma comment(lib, "shlwapi.lib")

template <class T> void SafeRelease(T** pp) {
    if (*pp) { (*pp)->Release(); *pp = nullptr; }
}

int main() {
    CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
    MFStartup(MF_VERSION);

    IMFMediaSource* source = nullptr;
    IMFAttributes*  attrs  = nullptr;
    IMFActivate**   acts   = nullptr;
    UINT32          count  = 0;

    MFCreateAttributes(&attrs, 1);
    attrs->SetGUID(MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE,
                   MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_VIDCAP_GUID);
    MFEnumDeviceSources(attrs, &acts, &count);

    if (count == 0) return 0;
    acts[0]->ActivateObject(__uuidof(IMFMediaSource), (void**)&source);

    IAMVideoProcAmp*  vpa = nullptr;
    IAMCameraControl* cam = nullptr;
    source->QueryInterface(&vpa);
    source->QueryInterface(&cam);

    /* ----------  example: set brightness to 150 (0-255)  ---------- */
    long val, flags;
    vpa->GetRange(VideoProcAmp_Brightness, &min, &max, &step, &def, &caps);
    vpa->Set(VideoProcAmp_Brightness, 150, VideoProcAmp_Flags_Manual);

    /* ----------  example: fix exposure to -6 (logarithmic)  ------- */
    if (cam) {
        cam->GetRange(CameraControl_Exposure, &min, &max, &step, &def, &caps);
        cam->Set(CameraControl_Exposure, -6, CameraControl_Flags_Manual);
    }

    /* ----------  read back ----------------------------------------- */
    vpa->Get(VideoProcAmp_Brightness, &val, &flags);
    printf("brightness = %ld\n", val);

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
