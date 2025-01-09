
#include <windows.h>

#include "core/Engine.h"

int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE, _In_ LPWSTR, _In_ int nShowCmd)
{
    coopscoop::core::ENGINE.Initialize(hInstance, 1920, 1080, "CoopScoop");
    coopscoop::core::ENGINE.Destroy();
    return 0;
}