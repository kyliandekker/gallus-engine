#include <windows.h>

#include "engine/Engine.h"

int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE, _In_ LPWSTR, _In_ int nShowCmd)
{
    coopscoop::engine::ENGINE.Initialize(hInstance, 1920, 1080, L"CoopScoop");
    coopscoop::engine::ENGINE.Destroy();
    return 0;
}