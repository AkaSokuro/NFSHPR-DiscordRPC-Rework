#pragma once

#include "../../pch.h"

class RPCManager
{
private:
	inline static DiscordService* g_discordService = nullptr;
	inline static discord::Activity activity{};
public:
	static void Initialize(DiscordService* discordService);
	static void Update();

	static void UpdateSingleplayerActivity(std::string& details, std::string& state,
		bool isInGarage, bool isInWorld,
		unsigned int trackId, const std::string& iconText,
		unsigned int loadedCarId, const std::string& carName);
	static void UpdateMultiplayerActivity(std::string& details, std::string& state, int& maxSize,
		bool isInGarage, bool isInWorld,
		unsigned int loadedCarId, const std::string& carName);
};