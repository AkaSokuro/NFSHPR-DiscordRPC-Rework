#include "pch.h"
#include "src/include/discordservice.h"
#include "src/include/rpcmanager.h"

namespace {
    DiscordService g_DiscordService;
    std::atomic<bool> g_Running{ true };
}

DWORD WINAPI MainThread(LPVOID lpParam) {
    if (!g_DiscordService.Initialize()) {
        return 1;
    }

    RPCManager::Initialize(&g_DiscordService);

    while (g_Running.load(std::memory_order_relaxed)) {
        RPCManager::Update();
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    g_DiscordService.Destroy();
    return 0;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    {
        DisableThreadLibraryCalls(hModule);
        HANDLE hThread = CreateThread(nullptr, 0, MainThread, hModule, 0, nullptr);
        if (hThread) {
            CloseHandle(hThread);
        }
        break;
    }

    case DLL_PROCESS_DETACH:
        g_Running.store(false, std::memory_order_release);
        break;
    }

    return TRUE;
}