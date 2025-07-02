#pragma once

#include "global.h"
#include "utils.h"
#include "offsets.h"

enum class e_weapon_core_animation : std::uint8_t {
	melee = 0,
	pistol = 1,
	shotgun = 2,
	paper_blueprint = 3,
	rifle = 4,
	melee_one_hand = 5,
	machine_pistol = 6,
	rocket_launcher = 7,
	grenade_launcher = 8,
	going_commando = 9,
	assault_rifle = 10,
	tactical_shotgun = 11,
	sniper_rifle = 12,
	trap_placement = 13,
	shoulder_launcher = 14,
	ability_deco_tool = 15,
	crossbow = 16,
	c4 = 17,
	remote_control = 18,
	dual_wield = 19,
	ar_bullpup = 20,
	ar_forward_grip = 21,
	med_pack_paddles = 22,
	smg_p90 = 23,
	ar_drum_gun = 24,
	consumable_small = 25,
	consumable_large = 26,
	balloon = 27,
	mounted_turret = 28,
	creative_tool = 29,
	explosive_bow = 30,
	ashton_indigo = 31,
	ashton_chicago = 32,
	melee_dual_wield = 33,
	unarmed = 34,
	max = 35
};




bool update_va_text() {
	const int maxTries = 10000;
	for (int i = 0, e = 0; i < maxTries && e < 3; i++) {
		point::va_text = point::Base + i * 0x1000;
		auto uworld = mem.Read<uintptr_t>(point::va_text + offsets::Uworld);
		auto level = mem.Read<uintptr_t>(uworld + offsets::PersistentLevel);
		if (uworld && level && mem.Read<uintptr_t>(level + offsets::OwningWorld) == uworld) {
			return true;
		}
		if (i > 1000) { i = 0; e++; }
	}
	std::cout << "(!) update_va_text failed to find valid va_text after " << maxTries << " tries." << std::endl;
	return false;
}

void newInfo()
{
	auto start = std::chrono::high_resolution_clock::now();

	if (!settings::cheat::GData)
		goto END;

	if (point::Uworld || point::GameInstance || point::GameState) {

		mem.SClear(mem.hS);

		mem.SPrepare(mem.hS, point::va_text + offsets::Uworld, sizeof(uintptr_t));

		if (point::Uworld) {
			mem.SPrepare(mem.hS, point::Uworld + offsets::CameraLocationPointer, sizeof(uintptr_t));
			mem.SPrepare(mem.hS, point::Uworld + offsets::CameraRotationPointer, sizeof(uintptr_t));

			mem.SPrepare(mem.hS, point::Uworld + offsets::GameInstance, sizeof(uintptr_t));
			mem.SPrepare(mem.hS, point::Uworld + offsets::PersistentLevel, sizeof(uintptr_t));
			mem.SPrepare(mem.hS, point::Uworld + offsets::GameState, sizeof(uintptr_t));
		}

		if (point::GameInstance) {
			mem.SPrepare(mem.hS, point::GameInstance + offsets::LocalPlayers, sizeof(uintptr_t));
		}

		if (point::LocalPlayers) {
			mem.SPrepare(mem.hS, point::LocalPlayers, sizeof(uintptr_t));
		}

		if (point::LocalPlayer) {
			mem.SPrepare(mem.hS, point::LocalPlayer + offsets::PlayerController, sizeof(uintptr_t));
			mem.SPrepare(mem.hS, point::LocalPlayer + offsets::MouseStuff, sizeof(uintptr_t));
		}
		
		if (point::PlayerController) {
			mem.SPrepare(mem.hS, point::PlayerController + offsets::LocalPawn, sizeof(uintptr_t));
			mem.SPrepare(mem.hS, point::PlayerController + offsets::Cameramanager, sizeof(uintptr_t));
		}

		if (point::Player) {
			mem.SPrepare(mem.hS, point::Player + offsets::PlayerState, sizeof(uintptr_t));
		}

		if (point::GameState) {
			mem.SPrepare(mem.hS, point::GameState + offsets::PlayerArray, sizeof(tarray));
		}

		if (point::PlayerState) {
			mem.SPrepare(mem.hS, point::PlayerState + offsets::TeamId, sizeof(uint32_t));
		}

		mem.ExecuteMemoryReads(mem.hS);

		point::Uworld = mem.SReads<uintptr_t>(mem.hS, point::va_text + offsets::Uworld);

		if (point::Uworld) {
			point::LocationPointer = mem.SReads<uintptr_t>(mem.hS, point::Uworld + offsets::CameraLocationPointer);
			point::RotationPointer = mem.SReads<uintptr_t>(mem.hS, point::Uworld + offsets::CameraRotationPointer);
			point::GameInstance = mem.SReads<uintptr_t>(mem.hS, point::Uworld + offsets::GameInstance);
			point::PersistentLevel = mem.SReads<uintptr_t>(mem.hS, point::Uworld + offsets::PersistentLevel);
			point::GameState = mem.SReads<uintptr_t>(mem.hS, point::Uworld + offsets::GameState);
		}

		if (point::GameInstance) {
			point::LocalPlayers = mem.SReads<uintptr_t>(mem.hS, point::GameInstance + offsets::LocalPlayers);
		}

		if (point::LocalPlayers) {
			point::LocalPlayer = mem.SReads<uintptr_t>(mem.hS, point::LocalPlayers);
		}

		if (point::LocalPlayer) {
			point::PlayerController = mem.SReads<uintptr_t>(mem.hS, point::LocalPlayer + offsets::PlayerController);
			point::Settings = mem.SReads<uintptr_t>(mem.hS, point::LocalPlayer + offsets::MouseStuff);
			if (point::Settings) {
				point::MouseSensX = mem.Read<float>(point::Settings + offsets::MouseSensitivityX);
				point::MouseSensY = mem.Read<float>(point::Settings + offsets::MouseSensitivityY);
			}
		}

		if (point::PlayerController) {
			point::Player = mem.SReads<uintptr_t>(mem.hS, point::PlayerController + offsets::LocalPawn);
			point::PlayerCameraManager = mem.SReads<uintptr_t>(mem.hS, point::PlayerController + offsets::Cameramanager);
		}

		if (point::Player) {
			point::PlayerState = mem.SReads<uintptr_t>(mem.hS, point::Player + offsets::PlayerState);
		}

		if (point::GameState) {
			point::PlayerArray = mem.SReads<tarray>(mem.hS, point::GameState + offsets::PlayerArray);
		}

		if (point::PlayerState) {
			local_player::localTeam = mem.SReads<uint32_t>(mem.hS, point::PlayerState + offsets::TeamId);
		}

	}
	else {

		point::Uworld = mem.Read<uintptr_t>(point::va_text + offsets::Uworld);

		if (!point::Uworld) goto END;

		mem.SClear(mem.hS);

		mem.SPrepare(mem.hS, point::Uworld + offsets::CameraLocationPointer, sizeof(uintptr_t));
		mem.SPrepare(mem.hS, point::Uworld + offsets::CameraRotationPointer, sizeof(uintptr_t));

		mem.SPrepare(mem.hS, point::Uworld + offsets::GameInstance, sizeof(uintptr_t));
		mem.SPrepare(mem.hS, point::Uworld + offsets::PersistentLevel, sizeof(uintptr_t));
		mem.SPrepare(mem.hS, point::Uworld + offsets::GameState, sizeof(uintptr_t));

		mem.ExecuteMemoryReads(mem.hS);

		point::LocationPointer = mem.SReads<uintptr_t>(mem.hS, point::Uworld + offsets::CameraLocationPointer);
		point::RotationPointer = mem.SReads<uintptr_t>(mem.hS, point::Uworld + offsets::CameraRotationPointer);
		point::GameInstance = mem.SReads<uintptr_t>(mem.hS, point::Uworld + offsets::GameInstance);
		point::PersistentLevel = mem.SReads<uintptr_t>(mem.hS, point::Uworld + offsets::PersistentLevel);
		point::GameState = mem.SReads<uintptr_t>(mem.hS, point::Uworld + offsets::GameState);

		if (!point::GameState) goto END;

		if (point::GameInstance) point::LocalPlayers = mem.Read<uintptr_t>(point::GameInstance + offsets::LocalPlayers);

		if (!point::LocalPlayers) goto END;

		point::LocalPlayer = mem.Read<uintptr_t>(point::LocalPlayers);

		if (!point::LocalPlayer) goto END;

		point::PlayerController = mem.Read<uintptr_t>(point::LocalPlayer + offsets::PlayerController);


		if (point::PlayerController) point::Player = mem.Read<uintptr_t>(point::PlayerController + offsets::LocalPawn);

		if (point::PlayerController) point::PlayerCameraManager = mem.Read<uintptr_t>(point::PlayerController + offsets::Cameramanager);

		if (point::Player) point::PlayerState = mem.Read<uintptr_t>(point::Player + offsets::PlayerState);

		point::PlayerArray = mem.Read<tarray>(point::GameState + offsets::PlayerArray);

		if (point::PlayerState) local_player::localTeam = mem.Read<uint32_t>(point::PlayerState + offsets::TeamId);
	}

END:

	__int64 elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start).count();
	stats::GDataData.addValue(static_cast<float>(elapsed));

	if (elapsed > 1000)
	std::cout << "(/) Warning GData Update took " << elapsed << " ms" << std::endl;

}

#include <mutex>
std::mutex listLock;

//GetPlayerName from PlayerState
std::string GetPlayerName(uintptr_t player_state) {
	if (!player_state) return "BOT";
	__int64 FString = mem.Read<__int64>(player_state + offsets::PlayerName);
	if (!FString) return "BOT";
	int Length = mem.Read<int>(FString + 16);
	if (Length <= 0 || Length > 64) return "BOT";
	uintptr_t FText = mem.Read<__int64>(FString + 8);
	if (!FText) return "BOT";
	__int64 v6 = Length;
	if (!v6) return std::string("BOT");

	wchar_t* NameBuffer = new wchar_t[Length];
	if (!mem.ReadA((ULONG64)FText, NameBuffer, Length * sizeof(wchar_t))) {
		delete[] NameBuffer;
		return "BOT";
	}

	char v21;
	int v22;
	int i;
	int v25;
	WORD* v23;

	v21 = v6 - 1;
	if (!(DWORD)v6)
		v21 = 0;
	v22 = 0;
	v23 = (WORD*)NameBuffer;
	for (i = (v21) & 3; ; *v23++ += i & 7)
	{
		v25 = v6 - 1;
		if (!(DWORD)v6)
			v25 = 0;
		if (v22 >= v25)
			break;
		i += 3;
		++v22;
	}

	std::wstring wbuffer(NameBuffer, Length);
	delete[] NameBuffer;
	return std::string(wbuffer.begin(), wbuffer.end());
}
std::vector<std::string> spectatorNamesCache;
std::chrono::steady_clock::time_point lastSpectatorUpdate = std::chrono::steady_clock::now();
// Helper struct for FFortSpectatorZoneItem
struct FFortSpectatorZoneItem {
	char pad[0x10];
	uintptr_t PlayerState; // pointer to PlayerState
};

std::string get_weapon_name(uintptr_t pawn)
{
	if (!pawn) return "No Weapon";
	uint64_t CurrentWeapon = mem.Read<uint64_t>(pawn + offsets::CurrentWeapon);
	if (!CurrentWeapon) return "No Weapon";
	uint64_t WeaponData = mem.Read<uint64_t>(CurrentWeapon + offsets::WeaponData);
	if (!WeaponData) return "No Weapon";
	uint64_t ItemName = mem.Read<uint64_t>(WeaponData + 0x40);
	if (!ItemName) return "No Weapon";

	uint64_t fData = mem.Read<uint64_t>(ItemName + 0x20);
	int fLength = mem.Read<int>(ItemName + 0x28);

	if (fLength > 0 && fLength < 50 && fData) {
		std::vector<wchar_t> buffer(fLength);
		mem.Read(fData, (uint64_t)buffer.data(), fLength * sizeof(wchar_t));
		std::wstring wstr_buf(buffer.data(), fLength);
		std::string result(wstr_buf.begin(), wstr_buf.end());
		if (!result.empty()) return result;
	}

	return "No Weapon";
}

std::string get_rank(int32_t Tier)
{

	if (Tier == 0)
		return std::string("Bronze 1");
	else if (Tier == 1)
		return std::string("Bronze 2");
	else if (Tier == 2)
		return std::string("Bronze 3");
	else if (Tier == 3)
		return std::string("Silver 1");
	else if (Tier == 4)
		return std::string("Silver 2");
	else if (Tier == 5)
		return std::string("Silver 3");
	else if (Tier == 6)
		return std::string("Gold 1");
	else if (Tier == 7)
		return std::string("Gold 2");
	else if (Tier == 8)
		return std::string("Gold 3");
	else if (Tier == 9)
		return std::string("Platinum 1");
	else if (Tier == 10)
		return std::string("Platinum 2");
	else if (Tier == 11)
		return std::string("Platinum 3");
	else if (Tier == 12)
		return std::string("Diamond 1");
	else if (Tier == 13)
		return std::string("Diamond 2");
	else if (Tier == 14)
		return std::string("Diamond 3");
	else if (Tier == 15)
		return std::string("Elite");
	else if (Tier == 16)
		return std::string("Champion");
	else if (Tier == 17)
		return std::string("Unreal");
	else
		return std::string("Unranked");
}

std::string GetRankText(uintptr_t player_state) {
	uintptr_t habenero = mem.Read<uintptr_t>(player_state + offsets::HabaneroComponent);
	if (habenero) {
		int32_t RankProgress = mem.Read<int32_t>(habenero + offsets::RankedProgress + offsets::CurrentRank);
		std::string ranktype = get_rank(RankProgress);
		return ranktype;
	}
	return "Unknown";
}

std::vector<std::string> GetSpectatorNamesCached() {
	auto now = std::chrono::steady_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastSpectatorUpdate);

	if (duration.count() < 500)
		return spectatorNamesCache;


	std::vector<std::string> names;
	if (!point::PlayerState) return names;

	uintptr_t array_addr = mem.Read<uintptr_t>(point::PlayerState + offsets::Spectators + offsets::SpectatorArray);
	int32_t count = mem.Read<int32_t>(point::PlayerState + offsets::Spectators + offsets::SpectatorArray + 0x8);

	if (!array_addr || count <= 0 || count > 32) {
		spectatorNamesCache = names;
		lastSpectatorUpdate = now;
		return names;
	}

	for (int i = 0; i < count; ++i) {
		FFortSpectatorZoneItem item = mem.Read<FFortSpectatorZoneItem>(array_addr + i * sizeof(FFortSpectatorZoneItem));
		if (item.PlayerState) {
			std::string name = GetPlayerName(item.PlayerState);
			names.push_back(name);
		}
	}

	spectatorNamesCache = names;
	lastSpectatorUpdate = now;
	return names;
}



// Updated GetPlatformText function to show full platform names
std::string GetPlatformText(uintptr_t player_state) {
	if (!player_state) return "Unknown";
	uintptr_t platform_ptr = mem.Read<uintptr_t>(player_state + offsets::Platform);
	if (!platform_ptr)
		return "Unknown";

	wchar_t platform_wchar[64] = { 0 };
	if (!mem.ReadA(platform_ptr, platform_wchar, sizeof(platform_wchar))) {
		return "Unknown";
	}

	std::wstring platform_wstring(platform_wchar);

	if (platform_wstring.find(L"XBL") != std::wstring::npos || platform_wstring.find(L"XSX") != std::wstring::npos)
		return "Xbox";
	if (platform_wstring.find(L"PSN") != std::wstring::npos || platform_wstring.find(L"PS5") != std::wstring::npos)
		return "PS";
	if (platform_wstring.find(L"WIN") != std::wstring::npos)
		return "Win";
	if (platform_wstring.find(L"SWT") != std::wstring::npos)
		return "Switch";
	if (platform_wstring.find(L"AND") != std::wstring::npos)
		return "Android";
	if (platform_wstring.find(L"MAC") != std::wstring::npos)
		return "macOS";
	if (platform_wstring.find(L"LNX") != std::wstring::npos)
		return "Linux";
	if (platform_wstring.find(L"IOS") != std::wstring::npos)
		return "iOS";

	if (!platform_wstring.empty()) {
		std::string platform_str(platform_wstring.begin(), platform_wstring.end());
		if (!platform_str.empty())
			return platform_str;
	}

	return "Unknown";
}


void updatePlayerList()
{
	auto start = std::chrono::high_resolution_clock::now();

	if (!settings::cheat::PlayerList) {
		__int64 elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start).count();
		stats::updatePlayerListData.addValue(static_cast<float>(elapsed));
		return;
	}

	// check if we can update players
	if (!point::Uworld && !point::GameState && (!point::PlayerArray.data || point::PlayerArray.count == 0 || point::PlayerArray.count >= 150)) {
		__int64 elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start).count();
		stats::updatePlayerListData.addValue(static_cast<float>(elapsed));
		return;
	}


	// we will save the data used for this update (so they dont change while we are using them)
	tarray playerArray = point::PlayerArray;

	std::unique_ptr<uintptr_t[]> players(new uintptr_t[playerArray.count]); // this is an array of player states

	// read the whole player list
	mem.ReadA(playerArray.data, players.get(), playerArray.count * sizeof(uintptr_t));

	// Collect keys to remove outside the lock
	std::vector<uintptr_t> toRemove;
	for (const auto& entry : mainPlayerList) {
		bool found = false;
		for (uint32_t i = 0; i < playerArray.count; ++i) {
			if (players[i] == entry.first) {
				found = true;
				break;
			}
		}
		if (!found) {
			toRemove.push_back(entry.first);
		}
	}

	// Remove keys in a single lock
	listLock.lock();
	for (uintptr_t key : toRemove) {
		mainPlayerList.erase(key);
	}
	listLock.unlock();

	// add people that havent been cached yet
	std::vector<PlayerCache> playersToAdd;
	for (int i = 0; i < playerArray.count; i++) {
		if (!players[i])
			continue;
		auto it = mainPlayerList.find(players[i]);
		if (it == mainPlayerList.end()) {
			PlayerCache newPlayer;
			newPlayer.PlayerState = players[i];
			playersToAdd.push_back(newPlayer);
		}
	}

	// are there new players to add?
	if (playersToAdd.size() > 0) {
		// start looping over the new players to prepare data
		mem.SClear(mem.hS3);
		for (int i = 0; i < playersToAdd.size(); i++) {
			mem.SPrepare(mem.hS3, playersToAdd[i].PlayerState + offsets::PawnPrivate, sizeof(uintptr_t));
			mem.SPrepare(mem.hS3, playersToAdd[i].PlayerState + offsets::TeamId, sizeof(uint32_t));
			mem.SPrepare(mem.hS3, playersToAdd[i].PlayerState + offsets::bIsABot, sizeof(BYTE));
			mem.SPrepare(mem.hS3, playersToAdd[i].PlayerState + offsets::KillScore, sizeof(int));
		}
		mem.ExecuteMemoryReads(mem.hS3);
		// read the prepared data
		for (int i = 0; i < playersToAdd.size(); i++) {
			playersToAdd[i].Pawn = mem.SReads<uintptr_t>(mem.hS3, playersToAdd[i].PlayerState + offsets::PawnPrivate);
			playersToAdd[i].TeamId = mem.SReads<uint32_t>(mem.hS3, playersToAdd[i].PlayerState + offsets::TeamId);
			playersToAdd[i].isBot = mem.SReads<BYTE>(mem.hS3, playersToAdd[i].PlayerState + offsets::bIsABot) & 0x08; // it's good
			playersToAdd[i].KillScore = mem.SReads<int>(mem.hS3, playersToAdd[i].PlayerState + offsets::KillScore);
			playersToAdd[i].PlayerName = GetPlayerName(playersToAdd[i].PlayerState);
			playersToAdd[i].Platform = GetPlatformText(playersToAdd[i].PlayerState);
			playersToAdd[i].Rank = GetRankText(playersToAdd[i].PlayerState);
		}

		// now that we have everything we just add them to the cache
		listLock.lock();
		for (int i = 0; i < playersToAdd.size(); i++) {
			mainPlayerList[playersToAdd[i].PlayerState] = playersToAdd[i];
		}
		listLock.unlock();
	}

END:

	__int64 elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start).count();
	stats::updatePlayerListData.addValue(static_cast<float>(elapsed));

	if (elapsed > 1000)
		std::cout << "(/) Warning Players Update took " << elapsed << " ms" << std::endl;
}

#include <execution>

void MainUpdate()
{
	static std::chrono::steady_clock::time_point lastRefresh = std::chrono::steady_clock::now();
	auto start = std::chrono::high_resolution_clock::now();
	auto currentTime = std::chrono::steady_clock::now();
	bool shouldRefresh = std::chrono::duration_cast<std::chrono::seconds>(currentTime - lastRefresh).count() >= 5;

	if (!settings::cheat::Players) {
		goto END;
	}

	listLock.lock();

	secondPlayerList = mainPlayerList;

	if (shouldRefresh) {
		lastRefresh = currentTime;
	}

	// Reset ignore flags at the start of each update
	for (auto& it : secondPlayerList) {
		// Reset ignore if it's time for periodic refresh or if it's a valid player
		if (shouldRefresh || (it.second.Pawn && !(point::Player && it.second.TeamId == local_player::localTeam))) {
			it.second.ignore = false;
		}
	}

	mem.SClear(mem.hS4);

	// camera
	FNRot rotation;
	mem.SPrepareEx(mem.hS4, point::RotationPointer, sizeof(FNRot), &rotation);
	mem.SPrepareEx(mem.hS4, point::LocationPointer, sizeof(Vector3), &mainCamera.Location);
	mem.SPrepareEx(mem.hS4, point::LocalPlayer + offsets::FieldOfView, sizeof(float), &mainCamera.FieldOfView);

	// while we are at it update seconds for vischeck
	mem.SPrepareEx(mem.hS4, point::Uworld + offsets::Seconds, sizeof(double), &point::Seconds);

	// and location under reticle
	if (point::PlayerController)
		mem.SPrepareEx(mem.hS4, point::PlayerController + offsets::LocationUnderReticle, sizeof(Vector3), &mainCamera.LocationUnderReticle);

	// prepare all the reads in parallel
	std::for_each(std::execution::par, secondPlayerList.begin(), secondPlayerList.end(), [&](auto& it) {
		if (!it.second.Pawn || point::Player && it.second.TeamId == local_player::localTeam) {
			it.second.ignore = true;
			return;
		}

		// First, check if we need to update root component position
		if (it.second.RootComponent) {
			mem.SPrepareEx(mem.hS4, it.second.RootComponent + offsets::RelativeLocation, sizeof(Vector3), &it.second.RootLocation);
		}
	});

	// Execute initial reads to get positions
	mem.ExecuteMemoryReads(mem.hS4);

	// Check distances
	std::for_each(std::execution::par, secondPlayerList.begin(), secondPlayerList.end(), [&](auto& it) {
		if (!it.second.ignore && it.second.RootComponent) {
			// Check render distance using root location
			if (settings::config::EnableRenderDistance) {
				float distanceMeters = mainCamera.Location.Distance(it.second.RootLocation) / 100.0f; // Convert to meters
				
				if (distanceMeters > settings::config::RenderDistance) {
					it.second.ignore = true;
					return;
				}
			}
		}
	});

	// Prepare detailed reads only for players in range
	mem.SClear(mem.hS4);
	std::for_each(std::execution::par, secondPlayerList.begin(), secondPlayerList.end(), [&](auto& it) {
		if (!it.second.ignore) {
			if (it.second.BoneArray) {
				mem.SPrepareEx(mem.hS4, it.second.BoneArray, (82 * 0x60) + sizeof(FTransform), it.second.Bones);
			}

			if (it.second.Mesh) {
				mem.SPrepareEx(mem.hS4, it.second.Mesh + offsets::BoneArray, sizeof(it.second.BoneArrays), it.second.BoneArrays);
				mem.SPrepareEx(mem.hS4, it.second.Mesh + offsets::ComponentToWorld, sizeof(FTransform), &it.second.component_to_world);
				mem.SPrepareEx(mem.hS4, it.second.Mesh + offsets::LastRenderTime, sizeof(float), &it.second.last_render);
			}

			if (it.second.Pawn && std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - it.second.lastMesh).count() > 500) {
				mem.SPrepareEx(mem.hS4, it.second.Pawn + offsets::Mesh, sizeof(uintptr_t), &it.second.Mesh);
				mem.SPrepareEx(mem.hS4, it.second.Pawn + offsets::RootComponent, sizeof(uintptr_t), &it.second.RootComponent);
				mem.SPrepareEx(mem.hS4, it.second.Pawn + offsets::isDying, sizeof(BYTE), &it.second.isDying);
				mem.SPrepareEx(mem.hS4, it.second.Pawn + offsets::bIsABot, sizeof(BYTE), &it.second.isBot);
				mem.SPrepareEx(mem.hS4, it.second.Pawn + offsets::IsDBNO, sizeof(BYTE), &it.second.isDBNO);
				it.second.lastMesh = std::chrono::system_clock::now();
			}

			if (it.second.RootComponent && std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - it.second.lastVelocity).count() > 100) {
				mem.SPrepareEx(mem.hS4, it.second.RootComponent + offsets::Velocity, sizeof(Vector3), &it.second.Velocity);
				it.second.lastVelocity = std::chrono::system_clock::now();
			}
		}

		if (it.second.PlayerState && std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - it.second.lastPawn).count() > 500) {
			mem.SPrepareEx(mem.hS4, it.second.PlayerState + offsets::PawnPrivate, sizeof(uintptr_t), &it.second.Pawn);
			it.second.lastPawn = std::chrono::system_clock::now();
		}

		if (it.second.PlayerState && std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - it.second.lastTeamId).count() > 2500) {
			mem.SPrepareEx(mem.hS4, it.second.PlayerState + offsets::TeamId, sizeof(uint32_t), &it.second.TeamId);
			it.second.lastTeamId = std::chrono::system_clock::now();
		}
		if (it.second.PlayerState) {
			mem.SPrepareEx(mem.hS4, it.second.PlayerState + offsets::KillScore, sizeof(int), &it.second.KillScore);
		}
	});

	// Execute the reads
	mem.ExecuteMemoryReads(mem.hS4);

	// camera rotation
	mainCamera.Rotation.x = asin(rotation.c) * (180.0 / M_PI);
	mainCamera.Rotation.y = atan2(rotation.a * -1, rotation.b) * (180.0 / M_PI);

	// Process bone data for players in range
	std::for_each(std::execution::par, secondPlayerList.begin(), secondPlayerList.end(), [&](auto& it) {
		if (!it.second.ignore)
		{
			it.second.lastUpdate = chrono::system_clock::now();

			if (it.second.BoneArray) {
				// get bones world position
				it.second.Head3D = CalcMatrix(it.second.Bones[static_cast<int>(BoneID::HeadBone)].Bone, it.second.component_to_world);
				it.second.Bottom3D = CalcMatrix(it.second.Bones[static_cast<int>(BoneID::BottomBone)].Bone, it.second.component_to_world);
				it.second.Hip3D = CalcMatrix(it.second.Bones[static_cast<int>(BoneID::HipBone)].Bone, it.second.component_to_world);
				it.second.Neck3D = CalcMatrix(it.second.Bones[static_cast<int>(BoneID::NeckBone)].Bone, it.second.component_to_world);
				it.second.UpperArmLeft3D = CalcMatrix(it.second.Bones[static_cast<int>(BoneID::UpperArmLeftBone)].Bone, it.second.component_to_world);
				it.second.UpperArmRight3D = CalcMatrix(it.second.Bones[static_cast<int>(BoneID::UpperArmRightBone)].Bone, it.second.component_to_world);
				it.second.LeftHand3D = CalcMatrix(it.second.Bones[static_cast<int>(BoneID::LeftHandBone)].Bone, it.second.component_to_world);
				it.second.RightHand3D = CalcMatrix(it.second.Bones[static_cast<int>(BoneID::RightHandBone)].Bone, it.second.component_to_world);
				it.second.LeftHandT3D = CalcMatrix(it.second.Bones[static_cast<int>(BoneID::LeftHandTBone)].Bone, it.second.component_to_world);
				it.second.RightHandT3D = CalcMatrix(it.second.Bones[static_cast<int>(BoneID::RightHandTBone)].Bone, it.second.component_to_world);
				it.second.RightThigh3D = CalcMatrix(it.second.Bones[static_cast<int>(BoneID::RightThighBone)].Bone, it.second.component_to_world);
				it.second.LeftThigh3D = CalcMatrix(it.second.Bones[static_cast<int>(BoneID::LeftThighBone)].Bone, it.second.component_to_world);
				it.second.RightCalf3D = CalcMatrix(it.second.Bones[static_cast<int>(BoneID::RightCalfBone)].Bone, it.second.component_to_world);
				it.second.LeftCalf3D = CalcMatrix(it.second.Bones[static_cast<int>(BoneID::LeftCalfBone)].Bone, it.second.component_to_world);
				it.second.LeftFoot3D = CalcMatrix(it.second.Bones[static_cast<int>(BoneID::LeftFootBone)].Bone, it.second.component_to_world);
				it.second.RightFoot3D = CalcMatrix(it.second.Bones[static_cast<int>(BoneID::RightFootBone)].Bone, it.second.component_to_world);

				// and do world to screen
				it.second.Head2D = w2s(it.second.Head3D);
				it.second.Top2D = w2s(Vector3(it.second.Head3D.x, it.second.Head3D.y, it.second.Head3D.z + 15.f));
				it.second.Bottom2D = w2s(it.second.Bottom3D);
				it.second.Hip2D = w2s(it.second.Hip3D);
				it.second.Neck2D = w2s(it.second.Neck3D);
				it.second.UpperArmLeft2D = w2s(it.second.UpperArmLeft3D);
				it.second.UpperArmRight2D = w2s(it.second.UpperArmRight3D);
				it.second.LeftHand2D = w2s(it.second.LeftHand3D);
				it.second.RightHand2D = w2s(it.second.RightHand3D);
				it.second.LeftHandT2D = w2s(it.second.LeftHandT3D);
				it.second.RightHandT2D = w2s(it.second.RightHandT3D);
				it.second.RightThigh2D = w2s(it.second.RightThigh3D);
				it.second.LeftThigh2D = w2s(it.second.LeftThigh3D);
				it.second.RightCalf2D = w2s(it.second.RightCalf3D);
				it.second.LeftCalf2D = w2s(it.second.LeftCalf3D);
				it.second.LeftFoot2D = w2s(it.second.LeftFoot3D);
				it.second.RightFoot2D = w2s(it.second.RightFoot3D);
			}

			it.second.bisDying = (it.second.isDying >> 5) & 1;
			it.second.bisBot = (it.second.isBot >> 3) & 1;
			it.second.bisDBNO = (it.second.isDBNO >> 7) & 1;

			it.second.BoneArray = it.second.BoneArrays[0].data;
			if (!it.second.BoneArray) it.second.BoneArray = it.second.BoneArrays[1].data;
		}

		if (it.second.Pawn &&
			std::chrono::duration_cast<std::chrono::milliseconds>(
				std::chrono::system_clock::now() - it.second.lastWeaponUpdate
			).count() > 500)
		{
			it.second.Weaponname = get_weapon_name(it.second.Pawn);
			it.second.lastWeaponUpdate = std::chrono::system_clock::now();
		}
	});

	mainPlayerList = secondPlayerList;
	listLock.unlock();

END:

	__int64 elapsed = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - start).count();
	stats::updatePlayersData.addValue(static_cast<float>(elapsed));

	if (elapsed > 1000000)
		std::cout << "(/) Warning Main Update took " << elapsed / 1000 << " ms" << std::endl;
}

void weaponUpdate()
{

	// features that need the weapon updated
	if (!settings::config::Aimbot && !settings::config::TriggerBot)
		return;

	mem.SClear(mem.hS5);

	if (point::Player)
		mem.SPrepare(mem.hS5, point::Player + offsets::CurrentWeapon, sizeof(uintptr_t));

	mem.ExecuteMemoryReads(mem.hS5);

	float projectileSpeed = 0;

	if (point::Player)
		point::CurrentWeapon = mem.SReads<uintptr_t>(mem.hS5, point::Player + offsets::CurrentWeapon);
}
