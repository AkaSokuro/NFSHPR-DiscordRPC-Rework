#include "../../pch.h"
#include "../include/discordservice.h"

DiscordService::DiscordService()
    : initialized(false), instance(nullptr), activityManager(nullptr) {
}

DiscordService::~DiscordService()
{
    Destroy();
}

bool DiscordService::Initialize()
{
    if (initialized)
        return true;

    auto result = discord::Core::Create(1316417194032238632, DiscordCreateFlags_Default, &instance);
    if (result != discord::Result::Ok || !instance)
    {
        initialized = false;
        return false;
    }

    initialized = true;
    activityManager = &instance->ActivityManager();
    activityManager->RegisterSteam(1328660);

    return true;
}

void DiscordService::Destroy()
{
    if (!initialized)
        return;

    if (instance)
    {
        delete instance;
        instance = nullptr;
    }

    initialized = false;
}

bool DiscordService::RunCallbacks()
{
    if (!initialized || !instance)
        return false;

    auto result = instance->RunCallbacks();
    if (result != discord::Result::Ok)
        return false;

    return true;
}

bool DiscordService::Update(discord::Activity activity)
{
    if (!initialized || !activityManager)
        return false;

    activityManager->UpdateActivity(activity, [](discord::Result result) {
        // error handling idk
        });

    return RunCallbacks();
}