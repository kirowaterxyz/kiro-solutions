#pragma once

#include "json.hpp"
using json = nlohmann::json;


namespace settings {

	// general
	namespace runtime {
		inline int version_major = 3;
		inline int version_minor = 2;
		inline bool criticalPriority = true;
		inline bool headless = false; // no console
		inline bool windowless = false; // no window (to fix)
		inline bool graphicsOnly = false; // just the menu
		inline std::vector<std::string> configList = { "rage.txt", "legit.txt", "other.txt" };
		inline int selectedConfigIndex = 0;
		inline std::string configFile = configList[selectedConfigIndex];

		// internal
		inline bool hotKeys = false;
	}

	// window settings
	namespace window {
		inline LPCWSTR ClassName = L"Castiel";
		inline LPCWSTR WindowName = L"Castiel";
		inline int Width = 1920;
		inline int Height = 1080;
		inline json toJson() {
			return {
				{"Width", Width},
				{"Height", Height}
			};
		}
		inline void fromJson(const json& j) {
			if (j.contains("Width")) Width = j["Width"];
			if (j.contains("Height")) Height = j["Height"];
		}
	}

	// cheat internal settings
	namespace cheat {
		inline bool GData = true;
		inline bool Camera = true;
		inline bool PlayerList = true;
		inline bool Players = true;
	}

	// dma settings
	namespace dma {
		inline bool MemoryMap = true;

		inline json toJson() {
			return {
				{"MemoryMap", MemoryMap}
			};
		}
		inline void fromJson(const json& j) {
			if (j.contains("MemoryMap")) MemoryMap = j["MemoryMap"];
		}

	}

	// menus
	namespace menu {
		// temp
		inline bool open = true;

		// menus
		inline bool AdvancedDebug = false;
		inline bool PlayerList = false;
		inline bool Statistics = false;
		inline bool Internals = false;

		inline json toJson() {
			return {
				{"AdvancedDebug", AdvancedDebug},
				{"PlayerList", PlayerList},
				{"Statistics", Statistics},
				{"Internals", Internals}
			};
		}
		inline void fromJson(const json& j) {
			if (j.contains("AdvancedDebug")) AdvancedDebug = j["AdvancedDebug"];
			if (j.contains("PlayerList")) PlayerList = j["PlayerList"];
			if (j.contains("Statistics")) Statistics = j["Statistics"];
			if (j.contains("Internals")) Internals = j["Internals"];
		}
	}

	// kmbox
	namespace kmbox {

		inline bool SerialKmbox = false;
		inline bool NetKmbox = false;

		namespace net {
			inline std::string ip = "192.168.2.188";
			inline std::string port = "1234";
			inline std::string uuid = "1234ABCD";

			inline json toJson() {
				return {
					{"KmboxNetIp", ip},
					{"KmboxNetPort", port},
					{"KmboxNetUuid", uuid}
				};
			}
			inline void fromJson(const json& j) {
				if (j.contains("KmboxNetIp")) ip = j["KmboxNetIp"];
				if (j.contains("KmboxNetPort")) port = j["KmboxNetPort"];
				if (j.contains("KmboxNetUuid")) uuid = j["KmboxNetUuid"];
			}

			inline bool saveConfig() {
				std::ifstream file(settings::runtime::configFile);
				if (file.is_open()) {
					json combinedConfig;
					file >> combinedConfig;
					file.close();
					std::ofstream fileOut(settings::runtime::configFile);
					if (fileOut.is_open()) {
						combinedConfig["KmboxNet"] = kmbox::net::toJson();
						fileOut << combinedConfig.dump(4);
						fileOut.close();
					}
				}
				else {
					std::ofstream fileOut(settings::runtime::configFile);
					if (fileOut.is_open()) {
						json combinedConfig;
						combinedConfig["KmboxNet"] = kmbox::net::toJson();
						fileOut << combinedConfig.dump(4);
						fileOut.close();
					}
					return false;
				}

				return true;
			}

			inline bool loadConfig() {
				std::ifstream file(settings::runtime::configFile);
				if (file.is_open()) {
					json combinedConfig;
					file >> combinedConfig;
					file.close();
					if (combinedConfig.contains("KmboxNet")) {
						kmbox::net::fromJson(combinedConfig["KmboxNet"]);
					}
				}
				else {
					std::cout << "(/) Failed to open configuration file" << std::endl;
					return false;
				}

				return true;
			}
		}
	}

	namespace config {
		// misc
		inline bool Topmost = true;
		inline bool ExitCheat = false;
		inline bool Fuser = false;
		inline bool vSync = false;
		inline bool Crosshair = false;
		inline float CrosshairColor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };

		// render distance
		inline bool EnableRenderDistance = false;
		inline float RenderDistance = 300.0f;

		// watermark colors
		inline float WatermarkBackgroundColor[4] = { 0.07, 0.07, 0.09, 1.00f };
		inline float WatermarkAccentColor[4] = { 0.80f, 0.80f, 0.83f, 0.88f };

		// aim
		inline float FovColor[4] = { 1.0f, 1.0f, 1.0f, 1.0f }; 
		inline bool RainbowFov = false;
		inline bool Aimbot = true;
		inline bool AimbotVisCheck = false;
		inline bool AimbotIgnoreknocked = false;
		inline int AimBone = 0;
		inline int AimFov = 100;
		inline bool ShowAimFov = false;
		inline float AimSmoothing = 10;
		inline int AimKey = 2;
		inline int AimKey2 = 2;

		// Pistol
		inline bool AimbotPistol = true;
		inline bool AimbotVisCheckPistol = false;
		inline bool AimbotIgnoreknockedPistol = false;
		inline int AimBonePistol = 0;
		inline int AimFovPistol = 100;
		inline bool ShowAimFovPistol = false;
		inline float AimSmoothingPistol = 10;
		inline int AimKeyPistol = 2;
		inline int AimKey2Pistol = 2;

		// Shotgun
		inline bool AimbotShotgun = true;
		inline bool AimbotVisCheckShotgun = false;
		inline bool AimbotIgnoreknockedShotgun = false;
		inline int AimBoneShotgun = 0;
		inline int AimFovShotgun = 100;
		inline bool ShowAimFovShotgun = false;
		inline float AimSmoothingShotgun = 10;
		inline int AimKeyShotgun = 2;
		inline int AimKey2Shotgun = 2;

		// Rifle
		inline bool AimbotRifle = true;
		inline bool AimbotVisCheckRifle = false;
		inline bool AimbotIgnoreknockedRifle = false;
		inline int AimBoneRifle = 0;
		inline int AimFovRifle = 100;
		inline bool ShowAimFovRifle = false;
		inline float AimSmoothingRifle = 10;
		inline int AimKeyRifle = 2;
		inline int AimKey2Rifle = 2;

		// Sniper
		inline bool AimbotSniper = true;
		inline bool AimbotVisCheckSniper = false;
		inline bool AimbotIgnoreknockedSniper = false;
		inline int AimBoneSniper = 0;
		inline int AimFovSniper = 100;
		inline bool ShowAimFovSniper = false;
		inline float AimSmoothingSniper = 10;
		inline int AimKeySniper = 2;
		inline int AimKey2Sniper = 2;


		inline bool MoonlightAim = false;

		inline float StepPerDegreeX = 10;
		inline float StepPerDegreeY = -10;

		// trigger
		inline bool TriggerBot = true;
		inline bool TriggerBotAll = false; // Add this
		inline bool TriggerBotPistol = true;
		inline bool TriggerBotShotgun = true;
		inline bool TriggerBotRifle = true;
		inline bool TriggerBotSniper = true;
		inline bool TriggerBotIgnoreknocked = false;
		inline bool TriggerBotAlwaysOn = false;
		inline float TriggerBotMaxDistance = 100; // meters
		inline int TriggerDelay = 25; // ms
		inline int TriggerKey = 2;
		inline int TriggerKey2 = 2; // Add this

		// esp
		inline bool isVis = true;
		inline bool Skeleton = true;
		inline float SkeletonThickness = 0.8f;
		inline float skeletonColor[4] = { 1.0f, 0.0f, 0.0f, 1.0f };        // Default red
		inline float skeletonColorVisible[4] = { 0.0f, 1.0f, 0.0f, 1.0f }; // Default green
		inline bool Box = false;
		inline bool CornerBox = false;
		inline float CornerboxColor[4] = { 1.0f, 0.0f, 0.0f, 1.0f };        // Default red
		inline float CornerboxColorVisible[4] = { 0.0f, 1.0f, 0.0f, 1.0f }; // Default green
		inline float BoxRoundness = 10.f;
		inline float BoxThickness = 2.f;
		inline float boxColor[4] = { 1.0f, 0.0f, 0.0f, 1.0f };        // Default red
		inline float boxColorVisible[4] = { 0.0f, 1.0f, 0.0f, 1.0f }; // Default green
		inline bool Distance = true;
		inline float DistanceColor[4] = { 1.0f, 0.0f, 0.0f, 1.0f };        // Default red
		inline float DistanceColorVisible[4] = { 0.0f, 1.0f, 0.0f, 1.0f }; // Default green
		inline bool Snaplines = false;
		inline bool SnaplineMiddle = false;
		inline bool SnaplineTop = false;
		inline float SnapColor[4] = { 1.0f, 0.0f, 0.0f, 1.0f };        // Default red
		inline float SnapColorVisible[4] = { 0.0f, 1.0f, 0.0f, 1.0f }; // Default green
		inline bool Name = false;
		inline float NameColor[4] = { 1.0f, 0.0f, 0.0f, 1.0f };        // Default red
		inline float NameColorVisible[4] = { 0.0f, 1.0f, 0.0f, 1.0f }; // Default green
		inline bool Platform = false;
		inline float PlatformColor[4] = { 1.0f, 0.0f, 0.0f, 1.0f };        // Default red
		inline float PlatformColorVisible[4] = { 0.0f, 1.0f, 0.0f, 1.0f }; // Default green
		inline bool Weaponname = false;
		inline float WeaponColor[4] = { 1.0f, 0.0f, 0.0f, 1.0f };        // Default red
		inline float WeaponColorVisible[4] = { 0.0f, 1.0f, 0.0f, 1.0f }; // Default green
		inline bool Rank = false;
		inline float RankColor[4] = { 1.0f, 0.0f, 0.0f, 1.0f };        // Default red
		inline float RankColorVisible[4] = { 0.0f, 1.0f, 0.0f, 1.0f }; // Default green
		inline bool FovArrows = false;
		inline int FovArrowsSlider = 100;
		inline float FovArrowsColor[4] = { 1.0f, 0.0f, 0.0f, 1.0f };        // Default red
		inline float FovArrowsColorVisible[4] = { 0.0f, 1.0f, 0.0f, 1.0f }; // Default green
		inline bool KillScore = false;
		inline float KillScoreColor[4] = { 1.0f, 0.0f, 0.0f, 1.0f };        // Default red
		inline float KillScoreColorVisible[4] = { 0.0f, 1.0f, 0.0f, 1.0f }; // Default green
		inline bool Ignorenpc = false;
		inline bool Spectators = false;
		inline bool Bots = false;

		// radar
		inline bool Radar = true;
		inline bool RadarFuser = false;
		inline bool RadarDrawCenter = false;
		inline bool RadarDistance = true;
		inline int RadarX = 30;
		inline int RadarY = 30;
		inline int RadarXSize = 300;
		inline int RadarYSize = 300;
		inline int RadarScale = INT_MAX;
		inline float RadarZoom = 1.f;

		inline json toJson() {
			return {
				{"Topmost", Topmost},
				{"ExitCheat", ExitCheat},
				{"Fuser", Fuser},
				{"vSync", vSync},
				{"Crosshair", Crosshair},
				{"CrosshairColor", std::vector<float>(CrosshairColor, CrosshairColor + 4)},
				{"EnableRenderDistance", EnableRenderDistance},
				{"RenderDistance", RenderDistance},
				{"WatermarkBackgroundColor", std::vector<float>(WatermarkBackgroundColor, WatermarkBackgroundColor + 4)},
				{"WatermarkAccentColor", std::vector<float>(WatermarkAccentColor, WatermarkAccentColor + 4)},
				{"FovColor", std::vector<float>(FovColor, FovColor + 4)},
				{"RainbowFov", RainbowFov},
				{"Aimbot", Aimbot},
				{"AimbotVisCheck", AimbotVisCheck},
				{"AimbotIgnoreknocked", AimbotIgnoreknocked},
				{"AimBone", AimBone},
				{"AimFov", AimFov},
				{"ShowAimFov", ShowAimFov},
				{"AimSmoothing", AimSmoothing},
				{"AimKey", AimKey},
				{"AimKey2", AimKey2},

				// Pistol
				{"AimbotPistol", AimbotPistol},
				{"AimbotVisCheckPistol", AimbotVisCheckPistol},
				{"AimbotIgnoreknockedPistol", AimbotIgnoreknockedPistol},
				{"AimBonePistol", AimBonePistol},
				{"AimFovPistol", AimFovPistol},
				{"AimSmoothingPistol", AimSmoothingPistol},
				{"AimKeyPistol", AimKeyPistol},
				{"AimKey2Pistol", AimKey2Pistol},

				// Shotgun
				{"AimbotShotgun", AimbotShotgun},
				{"AimbotVisCheckShotgun", AimbotVisCheckShotgun},
				{"AimbotIgnoreknockedShotgun", AimbotIgnoreknockedShotgun},
				{"AimBoneShotgun", AimBoneShotgun},
				{"AimFovShotgun", AimFovShotgun},
				{"AimSmoothingShotgun", AimSmoothingShotgun},
				{"AimKeyShotgun", AimKeyShotgun},
				{"AimKey2Shotgun", AimKey2Shotgun},

				// Rifle
				{"AimbotRifle", AimbotRifle},
				{"AimbotVisCheckRifle", AimbotVisCheckRifle},
				{"AimbotIgnoreknockedRifle", AimbotIgnoreknockedRifle},
				{"AimBoneRifle", AimBoneRifle},
				{"AimFovRifle", AimFovRifle},
				{"AimSmoothingRifle", AimSmoothingRifle},
				{"AimKeyRifle", AimKeyRifle},
				{"AimKey2Rifle", AimKey2Rifle},

				// Sniper
				{"AimbotSniper", AimbotSniper},
				{"AimbotVisCheckSniper", AimbotVisCheckSniper},
				{"AimbotIgnoreknockedSniper", AimbotIgnoreknockedSniper},
				{"AimBoneSniper", AimBoneSniper},
				{"AimFovSniper", AimFovSniper},
				{"AimSmoothingSniper", AimSmoothingSniper},
				{"AimKeySniper", AimKeySniper},
				{"AimKey2Sniper", AimKey2Sniper},


				{"StepPerDegreeX", StepPerDegreeX},
				{"StepPerDegreeY", StepPerDegreeY},
				{"MoonlightAim", MoonlightAim},
				{"TriggerBot", TriggerBot},
				{"TriggerBotAll", TriggerBotAll},
				{"TriggerBotPistol", TriggerBotPistol},
				{"TriggerBotShotgun", TriggerBotShotgun},
				{"TriggerBotRifle", TriggerBotRifle},
				{"TriggerBotSniper", TriggerBotSniper},
				{"TriggerBotIgnoreknocked", TriggerBotIgnoreknocked},
				{"TriggerBotAlwaysOn", TriggerBotAlwaysOn},
				{"TriggerBotMaxDistance", TriggerBotMaxDistance},
				{"TriggerDelay", TriggerDelay},
				{"TriggerKey", TriggerKey},
				{"TriggerKey2", TriggerKey2},
				{"isVis", isVis},
				{"Skeleton", Skeleton},
				{"SkeletonThickness", SkeletonThickness},
				{"skeletonColor", std::vector<float>(skeletonColor, skeletonColor + 4)},
				{"skeletonColorVisible", std::vector<float>(skeletonColorVisible, skeletonColorVisible + 4)},
				{"Box", Box},
				{"CornerBox", CornerBox},
				{"CornerboxColor", std::vector<float>(CornerboxColor, CornerboxColor + 4)},
				{"CornerboxColorVisible", std::vector<float>(CornerboxColorVisible, CornerboxColorVisible + 4)},
				{"BoxRoundness", BoxRoundness},
				{"BoxThickness", BoxThickness},
				{"boxColor", std::vector<float>(boxColor, boxColor + 4)},
				{"boxColorVisible", std::vector<float>(boxColorVisible, boxColorVisible + 4)},
				{"Distance", Distance},
				{"DistanceColor", std::vector<float>(DistanceColor, DistanceColor + 4)},
				{"DistanceColorVisible", std::vector<float>(DistanceColorVisible, DistanceColorVisible + 4)},
				{"Snaplines", Snaplines},
				{"SnapColor", std::vector<float>(SnapColor, SnapColor + 4)},
				{"SnaplineMiddle", SnaplineMiddle},
				{"SnaplineTop", SnaplineTop},
				{"SnapColorVisible", std::vector<float>(SnapColorVisible, SnapColorVisible + 4)},
				{"Name", Name},
				{"NameColor", std::vector<float>(NameColor, NameColor + 4)},
				{"NameColorVisible", std::vector<float>(NameColorVisible, NameColorVisible + 4)},
				{"Platform", Platform},
				{"PlatformColor", std::vector<float>(PlatformColor, PlatformColor + 4)},
				{"PlatformColorVisible", std::vector<float>(PlatformColorVisible, PlatformColorVisible + 4)},
				{"Weaponname", Weaponname},
				{"WeaponColor", std::vector<float>(WeaponColor, WeaponColor + 4)},
				{"WeaponColorVisible", std::vector<float>(WeaponColorVisible, WeaponColorVisible + 4)},
				{"Rank", Rank},
				{"RankColor", std::vector<float>(RankColor, RankColor + 4)},
				{"RankColorVisible", std::vector<float>(RankColorVisible, RankColorVisible + 4)},
				{ "FovArrows", FovArrows },
				{ "FovArrowsSlider", FovArrowsSlider },
				{ "FovArrowsColor", std::vector<float>(FovArrowsColor, FovArrowsColor + 4) },
				{ "FovArrowsColorVisible", std::vector<float>(FovArrowsColorVisible, FovArrowsColorVisible + 4) },
				{ "KillScore", KillScore },
				{ "KillScoreColor", std::vector<float>(KillScoreColor, KillScoreColor + 4) },
				{ "KillScoreColorVisible", std::vector<float>(KillScoreColorVisible, KillScoreColorVisible + 4) },
				{"Ignorenpc", Ignorenpc},
				{"Spectators", Spectators},
				{"Bots", Bots},
				{"RadarX", RadarX},
				{"RadarY", RadarY},
				{"Radar", Radar},
				{"RadarFuser", RadarFuser},
				{"RadarDistance", RadarDistance},
				{"RadarDrawCenter", RadarDrawCenter},
				{"RadarXSize", RadarXSize},
				{"RadarYSize", RadarYSize},
				{"radarScale", RadarScale},
				{"RadarZoom", RadarZoom}
			};
		}
		inline void fromJson(const json& j) {
			if (j.contains("Topmost")) Topmost = j["Topmost"];
			if (j.contains("ExitCheat")) ExitCheat = j["ExitCheat"];
			if (j.contains("Fuser")) Fuser = j["Fuser"];
			if (j.contains("vSync")) vSync = j["vSync"];
			if (j.contains("Crosshair")) Crosshair = j["Crosshair"];
			if (j.contains("CrosshairColor") && j["CrosshairColor"].is_array() && j["CrosshairColor"].size() == 4)
				for (int i = 0; i < 4; ++i)
					CrosshairColor[i] = j["CrosshairColor"][i];
			if (j.contains("EnableRenderDistance")) EnableRenderDistance = j["EnableRenderDistance"];
			if (j.contains("RenderDistance")) RenderDistance = j["RenderDistance"];
			if (j.contains("WatermarkBackgroundColor") && j["WatermarkBackgroundColor"].is_array() && j["WatermarkBackgroundColor"].size() == 4)
				for (int i = 0; i < 4; ++i)
					WatermarkBackgroundColor[i] = j["WatermarkBackgroundColor"][i];
			if (j.contains("WatermarkAccentColor") && j["WatermarkAccentColor"].is_array() && j["WatermarkAccentColor"].size() == 4)
				for (int i = 0; i < 4; ++i)
					WatermarkAccentColor[i] = j["WatermarkAccentColor"][i];
			if (j.contains("FovColor") && j["FovColor"].is_array() && j["FovColor"].size() == 4)
				for (int i = 0; i < 4; ++i)
					FovColor[i] = j["FovColor"][i];
			if (j.contains("RainbowFov")) RainbowFov = j["RainbowFov"];
			if (j.contains("Aimbot")) Aimbot = j["Aimbot"];
			if (j.contains("AimbotVisCheck")) AimbotVisCheck = j["AimbotVisCheck"];
			if (j.contains("AimbotIgnoreknocked")) AimbotIgnoreknocked = j["AimbotIgnoreknocked"];
			if (j.contains("AimBone")) AimBone = j["AimBone"];
			if (j.contains("AimFov")) AimFov = j["AimFov"];
			if (j.contains("ShowAimFov")) ShowAimFov = j["ShowAimFov"];
			if (j.contains("AimSmoothing")) AimSmoothing = j["AimSmoothing"];
			if (j.contains("AimKey")) AimKey = j["AimKey"];
			if (j.contains("AimKey2")) AimKey2 = j["AimKey2"];

			// Pistol
			if (j.contains("AimbotPistol")) AimbotPistol = j["AimbotPistol"];
			if (j.contains("AimbotVisCheckPistol")) AimbotVisCheckPistol = j["AimbotVisCheckPistol"];
			if (j.contains("AimbotIgnoreknockedPistol")) AimbotIgnoreknockedPistol = j["AimbotIgnoreknockedPistol"];
			if (j.contains("AimBonePistol")) AimBonePistol = j["AimBonePistol"];
			if (j.contains("AimFovPistol")) AimFovPistol = j["AimFovPistol"];
			if (j.contains("AimSmoothingPistol")) AimSmoothingPistol = j["AimSmoothingPistol"];
			if (j.contains("AimKeyPistol")) AimKeyPistol = j["AimKeyPistol"];
			if (j.contains("AimKey2Pistol")) AimKey2Pistol = j["AimKey2Pistol"];

			// Shotgun
			if (j.contains("AimbotShotgun")) AimbotShotgun = j["AimbotShotgun"];
			if (j.contains("AimbotVisCheckShotgun")) AimbotVisCheckShotgun = j["AimbotVisCheckShotgun"];
			if (j.contains("AimbotIgnoreknockedShotgun")) AimbotIgnoreknockedShotgun = j["AimbotIgnoreknockedShotgun"];
			if (j.contains("AimBoneShotgun")) AimBoneShotgun = j["AimBoneShotgun"];
			if (j.contains("AimFovShotgun")) AimFovShotgun = j["AimFovShotgun"];
			if (j.contains("AimSmoothingShotgun")) AimSmoothingShotgun = j["AimSmoothingShotgun"];
			if (j.contains("AimKeyShotgun")) AimKeyShotgun = j["AimKeyShotgun"];
			if (j.contains("AimKey2Shotgun")) AimKey2Shotgun = j["AimKey2Shotgun"];

			// Rifle
			if (j.contains("AimbotRifle")) AimbotRifle = j["AimbotRifle"];
			if (j.contains("AimbotVisCheckRifle")) AimbotVisCheckRifle = j["AimbotVisCheckRifle"];
			if (j.contains("AimbotIgnoreknockedRifle")) AimbotIgnoreknockedRifle = j["AimbotIgnoreknockedRifle"];
			if (j.contains("AimBoneRifle")) AimBoneRifle = j["AimBoneRifle"];
			if (j.contains("AimFovRifle")) AimFovRifle = j["AimFovRifle"];
			if (j.contains("AimSmoothingRifle")) AimSmoothingRifle = j["AimSmoothingRifle"];
			if (j.contains("AimKeyRifle")) AimKeyRifle = j["AimKeyRifle"];
			if (j.contains("AimKey2Rifle")) AimKeyRifle = j["AimKey2Rifle"];

			// Sniper
			if (j.contains("AimbotSniper")) AimbotSniper = j["AimbotSniper"];
			if (j.contains("AimbotVisCheckSniper")) AimbotVisCheckSniper = j["AimbotVisCheckSniper"];
			if (j.contains("AimbotIgnoreknockedSniper")) AimbotIgnoreknockedSniper = j["AimbotIgnoreknockedSniper"];
			if (j.contains("AimBoneSniper")) AimBoneSniper = j["AimBoneSniper"];
			if (j.contains("AimFovSniper")) AimFovSniper = j["AimFovSniper"];
			if (j.contains("AimSmoothingSniper")) AimSmoothingSniper = j["AimSmoothingSniper"];
			if (j.contains("AimKeySniper")) AimKeySniper = j["AimKeySniper"];
			if (j.contains("AimKey2Sniper")) AimKey2Sniper = j["AimKey2Sniper"];


			if (j.contains("StepPerDegreeX")) StepPerDegreeX = j["StepPerDegreeX"];
			if (j.contains("StepPerDegreeY")) StepPerDegreeY = j["StepPerDegreeY"];
			if (j.contains("MoonlightAim")) MoonlightAim = j["MoonlightAim"];
			if (j.contains("TriggerBot")) TriggerBot = j["TriggerBot"];
			if (j.contains("TriggerBotAll")) TriggerBotAll = j["TriggerBotAll"];
			if (j.contains("TriggerBotPistol")) TriggerBotPistol = j["TriggerBotPistol"];
			if (j.contains("TriggerBotShotgun")) TriggerBotShotgun = j["TriggerBotShotgun"];
			if (j.contains("TriggerBotRifle")) TriggerBotRifle = j["TriggerBotRifle"];
			if (j.contains("TriggerBotSniper")) TriggerBotSniper = j["TriggerBotSniper"];
			if (j.contains("TriggerBotIgnoreknocked")) TriggerBotIgnoreknocked = j["TriggerBotIgnoreknocked"];
			if (j.contains("TriggerBotAlwaysOn")) TriggerBotAlwaysOn = j["TriggerBotAlwaysOn"];
			if (j.contains("TriggerBotMaxDistance")) TriggerBotMaxDistance = j["TriggerBotMaxDistance"];
			if (j.contains("TriggerDelay")) TriggerDelay = j["TriggerDelay"];
			if (j.contains("TriggerKey")) TriggerKey = j["TriggerKey"];
			if (j.contains("TriggerKey2")) TriggerKey2 = j["TriggerKey2"];
			if (j.contains("AimFov")) AimFov = j["AimFov"];
			if (j.contains("isVis")) isVis = j["isVis"];
			if (j.contains("Skeleton")) Skeleton = j["Skeleton"];
			if (j.contains("SkeletonThickness")) SkeletonThickness = j["SkeletonThickness"];
			if (j.contains("skeletonColor") && j["skeletonColor"].is_array() && j["skeletonColor"].size() == 4)
				for (int i = 0; i < 4; ++i)
					skeletonColor[i] = j["skeletonColor"][i];

			if (j.contains("skeletonColorVisible") && j["skeletonColorVisible"].is_array() && j["skeletonColorVisible"].size() == 4)
				for (int i = 0; i < 4; ++i)
					skeletonColorVisible[i] = j["skeletonColorVisible"][i];

			if (j.contains("Box")) Box = j["Box"];
			if (j.contains("CornerBox")) CornerBox = j["CornerBox"];
			if (j.contains("CornerboxColor") && j["CornerboxColor"].is_array() && j["CornerboxColor"].size() == 4)
				for (int i = 0; i < 4; ++i)
					CornerboxColor[i] = j["CornerboxColor"][i];

			if (j.contains("CornerboxColorVisible") && j["CornerboxColorVisible"].is_array() && j["CornerboxColorVisible"].size() == 4)
				for (int i = 0; i < 4; ++i)
					CornerboxColorVisible[i] = j["CornerboxColorVisible"][i];
			if (j.contains("BoxRoundness")) BoxRoundness = j["BoxRoundness"];
			if (j.contains("BoxThickness")) BoxThickness = j["BoxThickness"];
			if (j.contains("boxColor") && j["boxColor"].is_array() && j["boxColor"].size() == 4)
				for (int i = 0; i < 4; ++i)
					boxColor[i] = j["boxColor"][i];

			if (j.contains("boxColorVisible") && j["boxColorVisible"].is_array() && j["boxColorVisible"].size() == 4)
				for (int i = 0; i < 4; ++i)
					boxColorVisible[i] = j["boxColorVisible"][i];
			if (j.contains("Distance")) Distance = j["Distance"];
			if (j.contains("DistanceColor") && j["DistanceColor"].is_array() && j["DistanceColor"].size() == 4)
				for (int i = 0; i < 4; ++i)
					DistanceColor[i] = j["DistanceColor"][i];

			if (j.contains("DistanceColorVisible") && j["DistanceColorVisible"].is_array() && j["DistanceColorVisible"].size() == 4)
				for (int i = 0; i < 4; ++i)
					DistanceColorVisible[i] = j["DistanceColorVisible"][i];
			if (j.contains("Snaplines")) Snaplines = j["Snaplines"];
			if (j.contains("SnaplineMiddle")) SnaplineMiddle = j["SnaplineMiddle"];
			if (j.contains("SnaplineTop")) SnaplineTop = j["SnaplineTop"];
			if (j.contains("SnapColor") && j["SnapColor"].is_array() && j["SnapColor"].size() == 4)
				for (int i = 0; i < 4; ++i)
					SnapColor[i] = j["SnapColor"][i];

			if (j.contains("SnapColorVisible") && j["SnapColorVisible"].is_array() && j["SnapColorVisible"].size() == 4)
				for (int i = 0; i < 4; ++i)
					SnapColorVisible[i] = j["SnapColorVisible"][i];
			if (j.contains("Name")) Name = j["Name"];
			if (j.contains("NameColor") && j["NameColor"].is_array() && j["NameColor"].size() == 4)
				for (int i = 0; i < 4; ++i)
					NameColor[i] = j["NameColor"][i];

			if (j.contains("NameColorVisible") && j["NameColorVisible"].is_array() && j["NameColorVisible"].size() == 4)
				for (int i = 0; i < 4; ++i)
					NameColorVisible[i] = j["NameColorVisible"][i];
			if (j.contains("Platform")) Platform = j["Platform"];
			if (j.contains("PlatformColor") && j["PlatformColor"].is_array() && j["PlatformColor"].size() == 4)
				for (int i = 0; i < 4; ++i)
					PlatformColor[i] = j["PlatformColor"][i];

			if (j.contains("PlatformColorVisible") && j["PlatformColorVisible"].is_array() && j["PlatformColorVisible"].size() == 4)
				for (int i = 0; i < 4; ++i)
					PlatformColorVisible[i] = j["PlatformColorVisible"][i];
			if (j.contains("Weaponname")) Weaponname = j["Weaponname"];
			if (j.contains("WeaponColor") && j["WeaponColor"].is_array() && j["WeaponColor"].size() == 4)
				for (int i = 0; i < 4; ++i)
					WeaponColor[i] = j["WeaponColor"][i];

			if (j.contains("WeaponColorVisible") && j["WeaponColorVisible"].is_array() && j["WeaponColorVisible"].size() == 4)
				for (int i = 0; i < 4; ++i)
					WeaponColorVisible[i] = j["WeaponColorVisible"][i];
			if (j.contains("Rank")) Rank = j["Rank"];
			if (j.contains("RankColor") && j["RankColor"].is_array() && j["RankColor"].size() == 4)
				for (int i = 0; i < 4; ++i)
					RankColor[i] = j["RankColor"][i];

			if (j.contains("RankColorVisible") && j["RankColorVisible"].is_array() && j["RankColorVisible"].size() == 4)
				for (int i = 0; i < 4; ++i)
					RankColorVisible[i] = j["RankColorVisible"][i];
			if (j.contains("FovArrows")) FovArrows = j["FovArrows"];
			if (j.contains("FovArrowsSlider")) FovArrowsSlider = j["FovArrowsSlider"];
			if (j.contains("FovArrowsColor") && j["FovArrowsColor"].is_array() && j["FovArrowsColor"].size() == 4)
				for (int i = 0; i < 4; ++i)
					FovArrowsColor[i] = j["FovArrowsColor"][i];
			if (j.contains("FovArrowsColorVisible") && j["FovArrowsColorVisible"].is_array() && j["FovArrowsColorVisible"].size() == 4)

				if (j.contains("KillScore")) KillScore = j["KillScore"];
			if (j.contains("KillScoreColor") && j["KillScoreColor"].is_array() && j["KillScoreColor"].size() == 4)
				for (int i = 0; i < 4; ++i)
					KillScoreColor[i] = j["KillScoreColor"][i];

			if (j.contains("KillScoreColorVisible") && j["KillScoreColorVisible"].is_array() && j["KillScoreColorVisible"].size() == 4)
				for (int i = 0; i < 4; ++i)
					KillScoreColorVisible[i] = j["KillScoreColorVisible"][i];
			if (j.contains("Ignorenpc")) Ignorenpc = j["Ignorenpc"];
			if (j.contains("Spectators")) Spectators = j["Spectators"];
			if (j.contains("Bots")) Bots = j["Bots"];
			if (j.contains("Radar")) Radar = j["Radar"];
			if (j.contains("RadarFuser")) RadarFuser = j["RadarFuser"];
			if (j.contains("RadarDrawCenter")) RadarDrawCenter = j["RadarDrawCenter"];
			if (j.contains("RadarDistance")) RadarDistance = j["RadarDistance"];
			if (j.contains("RadarX")) RadarX = j["RadarX"];
			if (j.contains("RadarY")) RadarY = j["RadarY"];
			if (j.contains("RadarXSize")) RadarXSize = j["RadarXSize"];
			if (j.contains("RadarYSize")) RadarYSize = j["RadarYSize"];
			if (j.contains("RadarScale")) RadarScale = j["RadarScale"];
			if (j.contains("RadarZoom")) RadarZoom = j["RadarZoom"];
		}

	}
	
	namespace colors {
		// Window colors
		inline float WindowBg[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
		inline float ChildBg[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
		inline float PopupBg[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
		inline float Border[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
		inline float BorderShadow[4] = { 0.0f, 0.0f, 0.0f, 1.0f };

		// Text colors
		inline float Text[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
		inline float TextDisabled[4] = { 0.8f, 0.8f, 0.831f, 0.31f };
		inline float TextSelectedBg[4] = { 0.8f, 0.8f, 0.831f, 0.31f };

		// Header colors
		inline float Header[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
		inline float HeaderHovered[4] = { 0.8f, 0.8f, 0.831f, 0.31f };
		inline float HeaderActive[4] = { 0.8f, 0.8f, 0.831f, 0.31f };

		// Button colors
		inline float Button[4] = { 0.8f, 0.8f, 0.831f, 0.31f };
		inline float ButtonHovered[4] = { 0.8f, 0.8f, 0.831f, 0.31f };
		inline float ButtonActive[4] = { 0.8f, 0.8f, 0.831f, 1.0f };

		// Frame colors
		inline float FrameBg[4] = { 0.8f, 0.8f, 0.831f, 0.31f };
		inline float FrameBgHovered[4] = { 0.8f, 0.8f, 0.831f, 0.31f };
		inline float FrameBgActive[4] = { 0.8f, 0.8f, 0.831f, 0.31f };

		// Title colors
		inline float TitleBg[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
		inline float TitleBgActive[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
		inline float TitleBgCollapsed[4] = { 1.0f, 1.0f, 1.0f, 1.0f };

		// Menu Bar colors
		inline float MenuBarBg[4] = { 1.0f, 1.0f, 1.0f, 1.0f };

		// Scrollbar colors
		inline float ScrollbarBg[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
		inline float ScrollbarGrab[4] = { 0.8f, 0.8f, 0.831f, 1.0f };
		inline float ScrollbarGrabHovered[4] = { 0.8f, 0.8f, 0.831f, 1.0f };
		inline float ScrollbarGrabActive[4] = { 0.8f, 0.8f, 0.831f, 1.0f };

		// Check Mark colors
		inline float CheckMark[4] = { 0.8f, 0.8f, 0.831f, 1.0f };

		// Slider colors
		inline float SliderGrab[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
		inline float SliderGrabActive[4] = { 1.0f, 1.0f, 1.0f, 1.0f };

		// Resize Grip colors
		inline float ResizeGrip[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
		inline float ResizeGripHovered[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
		inline float ResizeGripActive[4] = { 1.0f, 1.0f, 1.0f, 1.0f };

		// Plot colors
		inline float PlotLines[4] = { 0.8f, 0.8f, 0.831f, 1.0f };
		inline float PlotLinesHovered[4] = { 0.8f, 0.8f, 0.831f, 1.0f };
		inline float PlotHistogram[4] = { 0.8f, 0.8f, 0.831f, 1.0f };
		inline float PlotHistogramHovered[4] = { 0.8f, 0.8f, 0.831f, 1.0f };

		// Snowflakes animation
		inline bool EnableSnowflakes = true;
		inline bool FullscreenSnowflakes = true;  // New setting for fullscreen mode
		inline float SnowflakeColor[4] = { 0.8f, 0.8f, 0.831f, 0.584f }; // White with slight transparency
		inline float SnowflakeSpeed = 1.0f;
		inline int SnowflakeCount = 200;


		inline json toJson() {
			return {
				{"WindowBg", std::vector<float>(WindowBg, WindowBg + 4)},
				{"ChildBg", std::vector<float>(ChildBg, ChildBg + 4)},
				{"PopupBg", std::vector<float>(PopupBg, PopupBg + 4)},
				{"Border", std::vector<float>(Border, Border + 4)},
				{"BorderShadow", std::vector<float>(BorderShadow, BorderShadow + 4)},
				{"Text", std::vector<float>(Text, Text + 4)},
				{"TextDisabled", std::vector<float>(TextDisabled, TextDisabled + 4)},
				{"Header", std::vector<float>(Header, Header + 4)},
				{"HeaderHovered", std::vector<float>(HeaderHovered, HeaderHovered + 4)},
				{"HeaderActive", std::vector<float>(HeaderActive, HeaderActive + 4)},
				{"Button", std::vector<float>(Button, Button + 4)},
				{"ButtonHovered", std::vector<float>(ButtonHovered, ButtonHovered + 4)},
				{"ButtonActive", std::vector<float>(ButtonActive, ButtonActive + 4)},
				{"FrameBg", std::vector<float>(FrameBg, FrameBg + 4)},
				{"FrameBgHovered", std::vector<float>(FrameBgHovered, FrameBgHovered + 4)},
				{"FrameBgActive", std::vector<float>(FrameBgActive, FrameBgActive + 4)},
				{"EnableSnowflakes", EnableSnowflakes},
				{"FullscreenSnowflakes", FullscreenSnowflakes},
				{"SnowflakeColor", std::vector<float>(SnowflakeColor, SnowflakeColor + 4)},
				{"SnowflakeSpeed", SnowflakeSpeed},
				{"SnowflakeCount", SnowflakeCount},
				{"TextSelectedBg", std::vector<float>(TextSelectedBg, TextSelectedBg + 4)},
				{"TitleBg", std::vector<float>(TitleBg, TitleBg + 4)},
				{"TitleBgActive", std::vector<float>(TitleBgActive, TitleBgActive + 4)},
				{"TitleBgCollapsed", std::vector<float>(TitleBgCollapsed, TitleBgCollapsed + 4)},
				{"MenuBarBg", std::vector<float>(MenuBarBg, MenuBarBg + 4)},
				{"ScrollbarBg", std::vector<float>(ScrollbarBg, ScrollbarBg + 4)},
				{"ScrollbarGrab", std::vector<float>(ScrollbarGrab, ScrollbarGrab + 4)},
				{"ScrollbarGrabHovered", std::vector<float>(ScrollbarGrabHovered, ScrollbarGrabHovered + 4)},
				{"ScrollbarGrabActive", std::vector<float>(ScrollbarGrabActive, ScrollbarGrabActive + 4)},
				{"CheckMark", std::vector<float>(CheckMark, CheckMark + 4)},
				{"SliderGrab", std::vector<float>(SliderGrab, SliderGrab + 4)},
				{"SliderGrabActive", std::vector<float>(SliderGrabActive, SliderGrabActive + 4)},
				{"ResizeGrip", std::vector<float>(ResizeGrip, ResizeGrip + 4)},
				{"ResizeGripHovered", std::vector<float>(ResizeGripHovered, ResizeGripHovered + 4)},
				{"ResizeGripActive", std::vector<float>(ResizeGripActive, ResizeGripActive + 4)},
				{"PlotLines", std::vector<float>(PlotLines, PlotLines + 4)},
				{"PlotLinesHovered", std::vector<float>(PlotLinesHovered, PlotLinesHovered + 4)},
				{"PlotHistogram", std::vector<float>(PlotHistogram, PlotHistogram + 4)},
				{"PlotHistogramHovered", std::vector<float>(PlotHistogramHovered, PlotHistogramHovered + 4)}
			};
		}

		inline void fromJson(const json& j) {
			if (j.contains("WindowBg") && j["WindowBg"].is_array() && j["WindowBg"].size() == 4)
				for (int i = 0; i < 4; ++i)
					WindowBg[i] = j["WindowBg"][i];
			if (j.contains("ChildBg") && j["ChildBg"].is_array() && j["ChildBg"].size() == 4)
				for (int i = 0; i < 4; ++i)
					ChildBg[i] = j["ChildBg"][i];
			if (j.contains("PopupBg") && j["PopupBg"].is_array() && j["PopupBg"].size() == 4)
				for (int i = 0; i < 4; ++i)
					PopupBg[i] = j["PopupBg"][i];
			if (j.contains("Border") && j["Border"].is_array() && j["Border"].size() == 4)
				for (int i = 0; i < 4; ++i)
					Border[i] = j["Border"][i];
			if (j.contains("BorderShadow") && j["BorderShadow"].is_array() && j["BorderShadow"].size() == 4)
				for (int i = 0; i < 4; ++i)
					BorderShadow[i] = j["BorderShadow"][i];
			if (j.contains("Text") && j["Text"].is_array() && j["Text"].size() == 4)
				for (int i = 0; i < 4; ++i)
					Text[i] = j["Text"][i];
			if (j.contains("TextDisabled") && j["TextDisabled"].is_array() && j["TextDisabled"].size() == 4)
				for (int i = 0; i < 4; ++i)
					TextDisabled[i] = j["TextDisabled"][i];
			if (j.contains("Header") && j["Header"].is_array() && j["Header"].size() == 4)
				for (int i = 0; i < 4; ++i)
					Header[i] = j["Header"][i];
			if (j.contains("HeaderHovered") && j["HeaderHovered"].is_array() && j["HeaderHovered"].size() == 4)
				for (int i = 0; i < 4; ++i)
					HeaderHovered[i] = j["HeaderHovered"][i];
			if (j.contains("HeaderActive") && j["HeaderActive"].is_array() && j["HeaderActive"].size() == 4)
				for (int i = 0; i < 4; ++i)
					HeaderActive[i] = j["HeaderActive"][i];
			if (j.contains("Button") && j["Button"].is_array() && j["Button"].size() == 4)
				for (int i = 0; i < 4; ++i)
					Button[i] = j["Button"][i];
			if (j.contains("ButtonHovered") && j["ButtonHovered"].is_array() && j["ButtonHovered"].size() == 4)
				for (int i = 0; i < 4; ++i)
					ButtonHovered[i] = j["ButtonHovered"][i];
			if (j.contains("ButtonActive") && j["ButtonActive"].is_array() && j["ButtonActive"].size() == 4)
				for (int i = 0; i < 4; ++i)
					ButtonActive[i] = j["ButtonActive"][i];
			if (j.contains("FrameBg") && j["FrameBg"].is_array() && j["FrameBg"].size() == 4)
				for (int i = 0; i < 4; ++i)
					FrameBg[i] = j["FrameBg"][i];
			if (j.contains("FrameBgHovered") && j["FrameBgHovered"].is_array() && j["FrameBgHovered"].size() == 4)
				for (int i = 0; i < 4; ++i)
					FrameBgHovered[i] = j["FrameBgHovered"][i];
			if (j.contains("FrameBgActive") && j["FrameBgActive"].is_array() && j["FrameBgActive"].size() == 4)
				for (int i = 0; i < 4; ++i)
					FrameBgActive[i] = j["FrameBgActive"][i];
			if (j.contains("EnableSnowflakes")) EnableSnowflakes = j["EnableSnowflakes"];
			if (j.contains("FullscreenSnowflakes")) FullscreenSnowflakes = j["FullscreenSnowflakes"];
			if (j.contains("SnowflakeColor") && j["SnowflakeColor"].is_array() && j["SnowflakeColor"].size() == 4)
				for (int i = 0; i < 4; ++i)
					SnowflakeColor[i] = j["SnowflakeColor"][i];
			if (j.contains("SnowflakeSpeed")) SnowflakeSpeed = j["SnowflakeSpeed"];
			if (j.contains("SnowflakeCount")) SnowflakeCount = j["SnowflakeCount"];
			if (j.contains("TextSelectedBg") && j["TextSelectedBg"].is_array() && j["TextSelectedBg"].size() == 4)
				for (int i = 0; i < 4; ++i) TextSelectedBg[i] = j["TextSelectedBg"][i];
			if (j.contains("TitleBg") && j["TitleBg"].is_array() && j["TitleBg"].size() == 4)
				for (int i = 0; i < 4; ++i) TitleBg[i] = j["TitleBg"][i];
			if (j.contains("TitleBgActive") && j["TitleBgActive"].is_array() && j["TitleBgActive"].size() == 4)
				for (int i = 0; i < 4; ++i) TitleBgActive[i] = j["TitleBgActive"][i];
			if (j.contains("TitleBgCollapsed") && j["TitleBgCollapsed"].is_array() && j["TitleBgCollapsed"].size() == 4)
				for (int i = 0; i < 4; ++i) TitleBgCollapsed[i] = j["TitleBgCollapsed"][i];
			if (j.contains("MenuBarBg") && j["MenuBarBg"].is_array() && j["MenuBarBg"].size() == 4)
				for (int i = 0; i < 4; ++i) MenuBarBg[i] = j["MenuBarBg"][i];
			if (j.contains("ScrollbarBg") && j["ScrollbarBg"].is_array() && j["ScrollbarBg"].size() == 4)
				for (int i = 0; i < 4; ++i) ScrollbarBg[i] = j["ScrollbarBg"][i];
			if (j.contains("ScrollbarGrab") && j["ScrollbarGrab"].is_array() && j["ScrollbarGrab"].size() == 4)
				for (int i = 0; i < 4; ++i) ScrollbarGrab[i] = j["ScrollbarGrab"][i];
			if (j.contains("ScrollbarGrabHovered") && j["ScrollbarGrabHovered"].is_array() && j["ScrollbarGrabHovered"].size() == 4)
				for (int i = 0; i < 4; ++i) ScrollbarGrabHovered[i] = j["ScrollbarGrabHovered"][i];
			if (j.contains("ScrollbarGrabActive") && j["ScrollbarGrabActive"].is_array() && j["ScrollbarGrabActive"].size() == 4)
				for (int i = 0; i < 4; ++i) ScrollbarGrabActive[i] = j["ScrollbarGrabActive"][i];
			if (j.contains("CheckMark") && j["CheckMark"].is_array() && j["CheckMark"].size() == 4)
				for (int i = 0; i < 4; ++i) CheckMark[i] = j["CheckMark"][i];
			if (j.contains("SliderGrab") && j["SliderGrab"].is_array() && j["SliderGrab"].size() == 4)
				for (int i = 0; i < 4; ++i) SliderGrab[i] = j["SliderGrab"][i];
			if (j.contains("SliderGrabActive") && j["SliderGrabActive"].is_array() && j["SliderGrabActive"].size() == 4)
				for (int i = 0; i < 4; ++i) SliderGrabActive[i] = j["SliderGrabActive"][i];
			if (j.contains("ResizeGrip") && j["ResizeGrip"].is_array() && j["ResizeGrip"].size() == 4)
				for (int i = 0; i < 4; ++i) ResizeGrip[i] = j["ResizeGrip"][i];
			if (j.contains("ResizeGripHovered") && j["ResizeGripHovered"].is_array() && j["ResizeGripHovered"].size() == 4)
				for (int i = 0; i < 4; ++i) ResizeGripHovered[i] = j["ResizeGripHovered"][i];
			if (j.contains("ResizeGripActive") && j["ResizeGripActive"].is_array() && j["ResizeGripActive"].size() == 4)
				for (int i = 0; i < 4; ++i) ResizeGripActive[i] = j["ResizeGripActive"][i];
			if (j.contains("PlotLines") && j["PlotLines"].is_array() && j["PlotLines"].size() == 4)
				for (int i = 0; i < 4; ++i) PlotLines[i] = j["PlotLines"][i];
			if (j.contains("PlotLinesHovered") && j["PlotLinesHovered"].is_array() && j["PlotLinesHovered"].size() == 4)
				for (int i = 0; i < 4; ++i) PlotLinesHovered[i] = j["PlotLinesHovered"][i];
			if (j.contains("PlotHistogram") && j["PlotHistogram"].is_array() && j["PlotHistogram"].size() == 4)
				for (int i = 0; i < 4; ++i) PlotHistogram[i] = j["PlotHistogram"][i];
			if (j.contains("PlotHistogramHovered") && j["PlotHistogramHovered"].is_array() && j["PlotHistogramHovered"].size() == 4)
				for (int i = 0; i < 4; ++i) PlotHistogramHovered[i] = j["PlotHistogramHovered"][i];
		}
	}

	inline bool saveConfig() {
		json combinedConfig;

		combinedConfig["Dma"] = dma::toJson();
		combinedConfig["Menu"] = menu::toJson();
		combinedConfig["KmboxNet"] = kmbox::net::toJson();
		combinedConfig["Config"] = config::toJson();
		combinedConfig["Window"] = window::toJson();
		combinedConfig["Colors"] = colors::toJson();

		std::ofstream file(settings::runtime::configFile);
		if (file.is_open()) {
			file << combinedConfig.dump(4);
			file.close();
			std::cout << "(+) Saved configuration file" << std::endl;
		}
		else {
			std::cout << "(/) Failed to open configuration file" << std::endl;
			return false;
		}

		return true;
	}

	inline bool loadConfig() {
		std::ifstream file(settings::runtime::configFile);
		if (file.is_open()) {
			json combinedConfig;
			file >> combinedConfig;
			file.close();

			if (combinedConfig.contains("Dma")) {
				dma::fromJson(combinedConfig["Dma"]);
			}
			if (combinedConfig.contains("Menu")) {
				menu::fromJson(combinedConfig["Menu"]);
			}
			if (combinedConfig.contains("KmboxNet")) {
				kmbox::net::fromJson(combinedConfig["KmboxNet"]);
			}
			if (combinedConfig.contains("Config")) {
				config::fromJson(combinedConfig["Config"]);
			}
			if (combinedConfig.contains("Window")) {
				window::fromJson(combinedConfig["Window"]);
			}
			if (combinedConfig.contains("Colors")) {
				colors::fromJson(combinedConfig["Colors"]);
			}

			std::cout <<"(+) Loaded configuration file" << std::endl;
		}
		else {
			std::cout << "(/) Failed to open configuration file" << std::endl;
			return false;
		}

		return true;
	}

}

struct feature {
	void (*func)();
	__int64 lasttime;
	__int64 period;
};
