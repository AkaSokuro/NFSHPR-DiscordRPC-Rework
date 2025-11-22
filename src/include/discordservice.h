#pragma once

#include "../../pch.h"

class DiscordService
{
private:
	bool initialized;
	discord::Core* instance;
	discord::ActivityManager* activityManager;

public:
	DiscordService();
	~DiscordService();

	bool Initialize();
	void Destroy();

	bool RunCallbacks();
	bool Update(discord::Activity activity);
};