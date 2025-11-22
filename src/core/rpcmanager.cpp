#include "../../pch.h"
#include "../include/discordservice.h"
#include "../include/rpcmanager.h"

void RPCManager::Initialize(DiscordService* discordService)
{
    g_discordService = discordService;
    activity = discord::Activity();

    activity.GetAssets().SetLargeImage("nfshp");
    activity.GetAssets().SetLargeText("NFS: Hot Pursuit Remastered");
}

void RPCManager::Update()
{
    if (!g_discordService || !g_discordService->RunCallbacks())
        return;

    Game::Player localPlayer = Game::Player::GetLocalPlayer();
    std::string playerName = localPlayer.GetName();

    // PlayerName will always show up even if the game is loaded in offline.
    // So if it doesn't show up, that means player is still in the intro sequence.
    if (playerName.empty()) {
        activity.SetDetails("Booting Up");
        g_discordService->Update(activity);
        return;
    }

    activity.GetAssets().SetSmallImage("nfshp-redicon");
    activity.GetAssets().SetSmallText(localPlayer.GetName().c_str());

    bool isInLobby = localPlayer.IsInLobby();
    bool isInGarage = localPlayer.IsInGarage();
    bool isInWorld = Game::World::IsInWorld();

    unsigned int loadedCarId = localPlayer.GetLoadedVehicleID();
    unsigned int carId = localPlayer.GetCurrentVehicleID();
    std::string carName = localPlayer.GetCurrentVehicleName();

    unsigned int trackId = Game::Lobby::CurrentTrackID(isInLobby);
    std::string trackName = Game::Lobby::CurrentTrackName(isInLobby);
	std::string iconText = (trackId && isInLobby) || (loadedCarId != 0x0 && trackId) 
        ? trackName 
        : "NFS: Hot Pursuit Remastered";

    std::string details;
    std::string state;
    int maxSize = 0;

    if (isInLobby) {
        UpdateMultiplayerActivity(details, state, maxSize, isInGarage, isInWorld,
            loadedCarId, carName);
    }
    else {
        UpdateSingleplayerActivity(details, state, isInGarage, isInWorld,
            trackId, iconText, loadedCarId, carName);
    }

    activity.SetDetails(details.c_str());
    activity.SetState(state.c_str());
    activity.GetAssets().SetLargeText(iconText.c_str());
    activity.GetParty().GetSize().SetMaxSize(maxSize);

    g_discordService->Update(activity);
}

void RPCManager::UpdateSingleplayerActivity(std::string& details, std::string& state,
    bool isInGarage, bool isInWorld,
    unsigned int trackId, const std::string& iconText,
    unsigned int loadedCarId, const std::string& carName)
{
    // Just in case the player is in garage without being in career mode.
    if (isInGarage) {
        details = "In Garage";
    }

    bool isInCareerMode = (trackId != 0x0 && iconText != "NFS: Hot Pursuit Remastered");
    // This is a bit retarded way to check but it just works. kinda.
    if (isInCareerMode && loadedCarId != 0x0) {
        details = isInWorld ? carName : "Preparing";
        state = "Singleplayer Career";
    }

    details = details.empty() ? "Browsing Menu" : details;
    state = state.empty() ? "Autolog" : state;

    activity.GetParty().GetSize().SetCurrentSize(0);
}

void RPCManager::UpdateMultiplayerActivity(std::string& details, std::string& state, int& maxSize,
    bool isInGarage, bool isInWorld,
    unsigned int loadedCarId, const std::string& carName)
{
    std::string series = Game::Lobby::CurrentSeriesName();
    std::string gamemode = Game::Lobby::CurrentGamemodeName();
    state = series + " - " + gamemode;

    if (isInGarage) {
        details = "Choosing Car";
    }
    else {
        details = isInWorld ? carName : "In Lobby";

        if (!isInWorld && loadedCarId != 0x0) {
            details = "Waiting";
        }
    }

    std::string lobbyType = Game::Lobby::GetLobbyType();
    int numPlayers = Game::Lobby::GetSize();

    // I don't even know if this is a good idea but I'll leave it as is.
    if (lobbyType == "Invite-Only") {
        activity.GetParty().GetSize().SetCurrentSize(1);
        maxSize = 1;
    }
    else {
        activity.GetParty().GetSize().SetCurrentSize(numPlayers);
        maxSize = (gamemode == "Interceptor") ? 2 : 8;
    }
}