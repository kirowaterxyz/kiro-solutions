#include "menu.h"

#include <fstream>
#include <iostream>
#include <string>
#include <format>
#include <numeric>

#include "../kmbox/kmboxNet.h"
#include "../kmbox/HidTable.h"
#include "../kmbox/kmbox_interface.h"

#include "../cheat/global.h"
#include "../performance.h"
#include "../rendering/Font.h"
#include "../rendering/images.h"
#include "../settings.h"
#include "../kmbox/kmbox_util.h"

#include "../dma/DMAHandler.h"

#include <imgui/imgui.h>
#include <imgui/imgui_impl_dx11.h>
#include <imgui/imgui_impl_win32.h>

#include "../cheat/utils.h"
#include <imgui/imgui_internal.h>

#include <random>
#include <chrono>

// Snowflake structure
struct Snowflake {
	float x;
	float y;
	float speed;
	float size;
	float angle;
};

// Global vector to store snowflakes
static std::vector<Snowflake> snowflakes;
static bool initialized = false;

// Initialize snowflakes
void InitSnowflakes(int count, float windowWidth, float windowHeight) {
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<float> xDist(0, windowWidth);
	std::uniform_real_distribution<float> yDist(-50, windowHeight);
	std::uniform_real_distribution<float> speedDist(1.0f, 2.0f);
	std::uniform_real_distribution<float> sizeDist(2.0f, 4.0f);
	std::uniform_real_distribution<float> angleDist(0, 360);

	snowflakes.clear();
	for (int i = 0; i < count; i++) {
		snowflakes.push_back({
			xDist(gen),
			yDist(gen),
			speedDist(gen),
			sizeDist(gen),
			angleDist(gen)
		});
	}
}

// Update and draw snowflakes
void UpdateSnowflakes(ImDrawList* drawList, ImVec2 pos, ImVec2 size, float deltaTime) {
	for (auto& flake : snowflakes) {
		// Update position - only move down
		flake.y += flake.speed * settings::colors::SnowflakeSpeed * deltaTime * 50.0f;

		// Reset if out of bounds
		if (flake.y > size.y) {
			flake.y = -20;
			flake.x = std::rand() % (int)size.x;
		}

		// Draw snowflake
		float alpha = settings::colors::SnowflakeColor[3];
		if (flake.y < 0) {
			alpha *= (1.0f + flake.y / 50.0f);
		}

		// Draw only if within bounds
		if (flake.x >= 0 && flake.x <= size.x && flake.y >= -20 && flake.y <= size.y) {
			drawList->AddCircleFilled(
				ImVec2(pos.x + flake.x, pos.y + flake.y),
				flake.size,
				ImColor(
					settings::colors::SnowflakeColor[0],
					settings::colors::SnowflakeColor[1],
					settings::colors::SnowflakeColor[2],
					alpha
				)
			);
		}
	}
}

// what is this you ask? well, ask chatgpt - sfYass
static int StringInputTextCallback(ImGuiInputTextCallbackData* data) {
	if (data->EventFlag == ImGuiInputTextFlags_CallbackResize) {
		std::string* str = static_cast<std::string*>(data->UserData);
		str->resize(data->BufTextLen);
		data->Buf = &(*str)[0];
	}
	return 0;
}

constexpr float RadiansToDegrees(float radians) {
	return radians * (180.0f / M_PI);
}

Rotation targetRotation(const Vector3& currentPosition, const Vector3& targetPosition) {
	float directionX = targetPosition.x - currentPosition.x;
	float directionY = targetPosition.y - currentPosition.y;
	float directionZ = targetPosition.z - currentPosition.z;

	float yawRadians = std::atan2(directionY, directionX);
	float yawDegrees = RadiansToDegrees(yawRadians);

	float distanceXY = std::sqrt(directionX * directionX + directionY * directionY); // Horizontal distance
	float pitchRadians = std::atan2(directionZ, distanceXY);
	float pitchDegrees = RadiansToDegrees(pitchRadians);

	return { yawDegrees, pitchDegrees };
}


void menu::Menu() {
	// Apply custom colors
	ImGuiStyle& style = ImGui::GetStyle();
	auto& colors = style.Colors;

	// Apply colors from settings
	colors[ImGuiCol_WindowBg] = ImVec4(settings::colors::WindowBg[0], settings::colors::WindowBg[1], settings::colors::WindowBg[2], settings::colors::WindowBg[3]);
	colors[ImGuiCol_ChildBg] = ImVec4(settings::colors::ChildBg[0], settings::colors::ChildBg[1], settings::colors::ChildBg[2], settings::colors::ChildBg[3]);
	colors[ImGuiCol_PopupBg] = ImVec4(settings::colors::PopupBg[0], settings::colors::PopupBg[1], settings::colors::PopupBg[2], settings::colors::PopupBg[3]);
	colors[ImGuiCol_Border] = ImVec4(settings::colors::Border[0], settings::colors::Border[1], settings::colors::Border[2], settings::colors::Border[3]);
	colors[ImGuiCol_BorderShadow] = ImVec4(settings::colors::BorderShadow[0], settings::colors::BorderShadow[1], settings::colors::BorderShadow[2], settings::colors::BorderShadow[3]);
	colors[ImGuiCol_Text] = ImVec4(settings::colors::Text[0], settings::colors::Text[1], settings::colors::Text[2], settings::colors::Text[3]);
	colors[ImGuiCol_TextDisabled] = ImVec4(settings::colors::TextDisabled[0], settings::colors::TextDisabled[1], settings::colors::TextDisabled[2], settings::colors::TextDisabled[3]);
	colors[ImGuiCol_TextSelectedBg] = ImVec4(settings::colors::TextSelectedBg[0], settings::colors::TextSelectedBg[1], settings::colors::TextSelectedBg[2], settings::colors::TextSelectedBg[3]);
	colors[ImGuiCol_Header] = ImVec4(settings::colors::Header[0], settings::colors::Header[1], settings::colors::Header[2], settings::colors::Header[3]);
	colors[ImGuiCol_HeaderHovered] = ImVec4(settings::colors::HeaderHovered[0], settings::colors::HeaderHovered[1], settings::colors::HeaderHovered[2], settings::colors::HeaderHovered[3]);
	colors[ImGuiCol_HeaderActive] = ImVec4(settings::colors::HeaderActive[0], settings::colors::HeaderActive[1], settings::colors::HeaderActive[2], settings::colors::HeaderActive[3]);
	colors[ImGuiCol_Button] = ImVec4(settings::colors::Button[0], settings::colors::Button[1], settings::colors::Button[2], settings::colors::Button[3]);
	colors[ImGuiCol_ButtonHovered] = ImVec4(settings::colors::ButtonHovered[0], settings::colors::ButtonHovered[1], settings::colors::ButtonHovered[2], settings::colors::ButtonHovered[3]);
	colors[ImGuiCol_ButtonActive] = ImVec4(settings::colors::ButtonActive[0], settings::colors::ButtonActive[1], settings::colors::ButtonActive[2], settings::colors::ButtonActive[3]);
	colors[ImGuiCol_FrameBg] = ImVec4(settings::colors::FrameBg[0], settings::colors::FrameBg[1], settings::colors::FrameBg[2], settings::colors::FrameBg[3]);
	colors[ImGuiCol_FrameBgHovered] = ImVec4(settings::colors::FrameBgHovered[0], settings::colors::FrameBgHovered[1], settings::colors::FrameBgHovered[2], settings::colors::FrameBgHovered[3]);
	colors[ImGuiCol_FrameBgActive] = ImVec4(settings::colors::FrameBgActive[0], settings::colors::FrameBgActive[1], settings::colors::FrameBgActive[2], settings::colors::FrameBgActive[3]);

	// New colors
	colors[ImGuiCol_TitleBg] = ImVec4(settings::colors::TitleBg[0], settings::colors::TitleBg[1], settings::colors::TitleBg[2], settings::colors::TitleBg[3]);
	colors[ImGuiCol_TitleBgActive] = ImVec4(settings::colors::TitleBgActive[0], settings::colors::TitleBgActive[1], settings::colors::TitleBgActive[2], settings::colors::TitleBgActive[3]);
	colors[ImGuiCol_TitleBgCollapsed] = ImVec4(settings::colors::TitleBgCollapsed[0], settings::colors::TitleBgCollapsed[1], settings::colors::TitleBgCollapsed[2], settings::colors::TitleBgCollapsed[3]);
	colors[ImGuiCol_MenuBarBg] = ImVec4(settings::colors::MenuBarBg[0], settings::colors::MenuBarBg[1], settings::colors::MenuBarBg[2], settings::colors::MenuBarBg[3]);
	colors[ImGuiCol_ScrollbarBg] = ImVec4(settings::colors::ScrollbarBg[0], settings::colors::ScrollbarBg[1], settings::colors::ScrollbarBg[2], settings::colors::ScrollbarBg[3]);
	colors[ImGuiCol_ScrollbarGrab] = ImVec4(settings::colors::ScrollbarGrab[0], settings::colors::ScrollbarGrab[1], settings::colors::ScrollbarGrab[2], settings::colors::ScrollbarGrab[3]);
	colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(settings::colors::ScrollbarGrabHovered[0], settings::colors::ScrollbarGrabHovered[1], settings::colors::ScrollbarGrabHovered[2], settings::colors::ScrollbarGrabHovered[3]);
	colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(settings::colors::ScrollbarGrabActive[0], settings::colors::ScrollbarGrabActive[1], settings::colors::ScrollbarGrabActive[2], settings::colors::ScrollbarGrabActive[3]);
	colors[ImGuiCol_CheckMark] = ImVec4(settings::colors::CheckMark[0], settings::colors::CheckMark[1], settings::colors::CheckMark[2], settings::colors::CheckMark[3]);
	colors[ImGuiCol_SliderGrab] = ImVec4(settings::colors::SliderGrab[0], settings::colors::SliderGrab[1], settings::colors::SliderGrab[2], settings::colors::SliderGrab[3]);
	colors[ImGuiCol_SliderGrabActive] = ImVec4(settings::colors::SliderGrabActive[0], settings::colors::SliderGrabActive[1], settings::colors::SliderGrabActive[2], settings::colors::SliderGrabActive[3]);
	colors[ImGuiCol_ResizeGrip] = ImVec4(settings::colors::ResizeGrip[0], settings::colors::ResizeGrip[1], settings::colors::ResizeGrip[2], settings::colors::ResizeGrip[3]);
	colors[ImGuiCol_ResizeGripHovered] = ImVec4(settings::colors::ResizeGripHovered[0], settings::colors::ResizeGripHovered[1], settings::colors::ResizeGripHovered[2], settings::colors::ResizeGripHovered[3]);
	colors[ImGuiCol_ResizeGripActive] = ImVec4(settings::colors::ResizeGripActive[0], settings::colors::ResizeGripActive[1], settings::colors::ResizeGripActive[2], settings::colors::ResizeGripActive[3]);
	colors[ImGuiCol_PlotLines] = ImVec4(settings::colors::PlotLines[0], settings::colors::PlotLines[1], settings::colors::PlotLines[2], settings::colors::PlotLines[3]);
	colors[ImGuiCol_PlotLinesHovered] = ImVec4(settings::colors::PlotLinesHovered[0], settings::colors::PlotLinesHovered[1], settings::colors::PlotLinesHovered[2], settings::colors::PlotLinesHovered[3]);
	colors[ImGuiCol_PlotHistogram] = ImVec4(settings::colors::PlotHistogram[0], settings::colors::PlotHistogram[1], settings::colors::PlotHistogram[2], settings::colors::PlotHistogram[3]);
	colors[ImGuiCol_PlotHistogramHovered] = ImVec4(settings::colors::PlotHistogramHovered[0], settings::colors::PlotHistogramHovered[1], settings::colors::PlotHistogramHovered[2], settings::colors::PlotHistogramHovered[3]);

	ImVec2 size = ImVec2(50 * (settings::window::Width / 100), 50 * (settings::window::Height / 100) + 90);
	ImGui::SetNextWindowSize(size);
	ImGui::PushFont((ImFont*)fonts::notosans_font);

	// Initialize snowflakes if needed
	if (!initialized && settings::colors::EnableSnowflakes) {
		// Initialize with larger area if fullscreen
		if (settings::colors::FullscreenSnowflakes) {
			InitSnowflakes(settings::colors::SnowflakeCount, settings::window::Width, settings::window::Height);
		} else {
			InitSnowflakes(settings::colors::SnowflakeCount, size.x, size.y);
		}
		initialized = true;
	}

	// Draw fullscreen snowflakes before the menu if enabled
	if (settings::colors::EnableSnowflakes && settings::colors::FullscreenSnowflakes) {
		static auto lastTime = std::chrono::high_resolution_clock::now();
		auto currentTime = std::chrono::high_resolution_clock::now();
		float deltaTime = std::chrono::duration<float>(currentTime - lastTime).count();
		lastTime = currentTime;

		ImVec2 screenPos = ImVec2(0, 0);
		ImVec2 screenSize = ImVec2(settings::window::Width, settings::window::Height);
		UpdateSnowflakes(ImGui::GetForegroundDrawList(), screenPos, screenSize, deltaTime);
	}

	enum Section { AIM, VISUALS, RADAR, MISC, COLOR, CONFIG };
	static Section activeSection = AIM;

	// Main window
	if (ImGui::Begin("##MainWindow", NULL, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar)) {
		// Get the actual menu position and size
		ImVec2 menuPos = ImGui::GetWindowPos();
		ImVec2 menuSize = ImGui::GetWindowSize();

		ImGui::SetCursorPos({ 10, 10 });

		// --- HEADER BAR ---
		ImGui::BeginChild("Header", ImVec2(size.x - 20, 30), false);

		// Left-aligned: app name and version
		ImGui::Text("kiro-solutions v%d.%d", settings::runtime::version_major, settings::runtime::version_minor);
		ImGui::SameLine();

		// Center-ish: Discord link
		ImGui::SetCursorPosX(size.x / 2.5f);
		ImGui::Text("discord.gg/d4ZhmGTMNW");

		// Right-aligned: tab buttons
		ImGui::PushFont((ImFont*)fonts::icons_font);
		ImGui::SameLine(size.x - 170);
		if (ImGui::Button("A" "")) activeSection = AIM;
		ImGui::SameLine();
		if (ImGui::Button("B" "")) activeSection = VISUALS;
		ImGui::SameLine();
		if (ImGui::Button("C" "")) activeSection = RADAR;
		ImGui::SameLine();
		if (ImGui::Button("D" "")) activeSection = MISC;
		ImGui::SameLine();
		if (ImGui::Button("F" "")) activeSection = COLOR;
		ImGui::SameLine();
		if (ImGui::Button("E" "")) activeSection = CONFIG;
		ImGui::PopFont();
		ImGui::EndChild();

		// --- MAIN CONTENT ---
		ImGui::SetCursorPos({ 10, 40 });
		ImGui::BeginChild("MainContent", ImVec2(size.x - 20, size.y - 60), ImGuiChildFlags_Border);

		switch (activeSection) {
		case AIM: {
			ImGui::Text("Aim");
			//static bool selectingAim = false;
			static bool selectingTrigger = false;
			static bool selectingTrigger2 = false;
			// Weapon type selection for aimbot settings
			static int selectedWeapon = 0;
			const char* weaponTypes[] = { "Global", "Pistol", "Shotgun", "Rifle", "Sniper" };
			const char* aimBones[] = { "Head", "Top", "Middle" };
			ImGui::Combo("Weapon Type", &selectedWeapon, weaponTypes, IM_ARRAYSIZE(weaponTypes));

			// Key selection state for each weapon type
			static bool selectingAimKey[5] = { false, false, false, false, false };
			static bool selectingAimKey2[5] = { false, false, false, false, false };

#define AIM_SETTING(TYPE, Type) \
    ImGui::Checkbox("Aimbot " TYPE, &settings::config::Aimbot##Type); \
    ImGui::Checkbox("Visible Check " TYPE, &settings::config::AimbotVisCheck##Type); \
    ImGui::Checkbox("Ignore Knocked " TYPE, &settings::config::AimbotIgnoreknocked##Type); \
    ImGui::Checkbox("Show FOV " TYPE, &settings::config::ShowAimFov##Type); /* Only global, remove ##Type if not per-weapon */ \
	ImGui::ColorEdit4("FOV Color " TYPE, settings::config::FovColor); \
	ImGui::Checkbox("Rainbow FOV" TYPE, &settings::config::RainbowFov); \
    ImGui::SliderFloat("Smooth " TYPE, &settings::config::AimSmoothing##Type, 1.1f, 50.f); \
    ImGui::SliderInt("FOV " TYPE, &settings::config::AimFov##Type, 1, settings::window::Height / 2); \
    if (ImGui::Button(selectingAimKey[selectedWeapon] ? "Press key" : std::format("Aim Key " TYPE ": {:d}", settings::config::AimKey##Type).c_str())) \
        selectingAimKey[selectedWeapon] = true; \
    if (selectingAimKey[selectedWeapon]) { \
        for (int i = 0; i < 256; i++) { \
            if (mem.IsKeyDown(i)) { \
                settings::config::AimKey##Type = i; \
                selectingAimKey[selectedWeapon] = false; \
            } \
        } \
    }\
	if (ImGui::Button(selectingAimKey2[selectedWeapon] ? "Press key" : std::format("Aim Key 2 " TYPE ": {:d}", settings::config::AimKey2##Type).c_str())) \
		selectingAimKey2[selectedWeapon] = true; \
	if (selectingAimKey2[selectedWeapon]) { \
		for (int i = 0; i < 256; i++) { \
			if (mem.IsKeyDown(i)) { \
				settings::config::AimKey2##Type = i; \
				selectingAimKey2[selectedWeapon] = false; \
			} \
		} \
	} 
	//if (ImGui::Button("Copy AimKey to AimKey2 " TYPE)) { \
	settings::config::AimKey2##Type = settings::config::AimKey##Type; \
	}

			switch (selectedWeapon) {
			case 0: // Global
				ImGui::Combo("Aim Bone", &settings::config::AimBone, aimBones, IM_ARRAYSIZE(aimBones));
				AIM_SETTING("", )
					break;
			case 1: // Pistol
				ImGui::Combo("Aim Bone", &settings::config::AimBonePistol, aimBones, IM_ARRAYSIZE(aimBones));
				AIM_SETTING("", Pistol)
					break;
			case 2: // Shotgun
				ImGui::Combo("Aim Bone", &settings::config::AimBoneShotgun, aimBones, IM_ARRAYSIZE(aimBones));
				AIM_SETTING("", Shotgun)
					break;
			case 3: // Rifle
				ImGui::Combo("Aim Bone", &settings::config::AimBoneRifle, aimBones, IM_ARRAYSIZE(aimBones));
				AIM_SETTING("", Rifle)
					break;
			case 4: // Sniper
				ImGui::Combo("Aim Bone", &settings::config::AimBoneSniper, aimBones, IM_ARRAYSIZE(aimBones));
				AIM_SETTING("", Sniper)
					break;
			}
			ImGui::Separator();


			ImGui::Text("Trigger");
			ImGui::Checkbox("TriggerBot", &settings::config::TriggerBot);
			ImGui::Checkbox("TriggerBot All", &settings::config::TriggerBotAll);
			ImGui::Checkbox("TriggerBot Pistol", &settings::config::TriggerBotPistol);
			ImGui::Checkbox("TriggerBot Shotgun", &settings::config::TriggerBotShotgun);
			ImGui::Checkbox("TriggerBot Rifle", &settings::config::TriggerBotRifle);
			ImGui::Checkbox("TriggerBot Sniper", &settings::config::TriggerBotSniper);
			ImGui::Checkbox("Ignore knocked##3", &settings::config::TriggerBotIgnoreknocked);
			ImGui::Checkbox("Always On", &settings::config::TriggerBotAlwaysOn);
			ImGui::SliderFloat("TriggerBot Max Distance", &settings::config::TriggerBotMaxDistance, 0.0f, 100.0f, "%.1f");
			ImGui::SliderInt("Delay", &settings::config::TriggerDelay, 1, 100);

			if (ImGui::Button(selectingTrigger ? "Press key" : std::format("Trg Key: {:d}", settings::config::TriggerKey).c_str()))
					selectingTrigger = !selectingTrigger;

			if (selectingTrigger) {
				for (int i = 0; i < 256; i++) {
					if (mem.IsKeyDown(i)) { // doesnt read mem ofc
						settings::config::TriggerKey = i;
						selectingTrigger = false;
					}
				}
			}

			if (ImGui::Button(selectingTrigger2 ? "Press key" : std::format("Trg Key 2: {:d}", settings::config::TriggerKey2).c_str()))
				selectingTrigger2 = !selectingTrigger2;

			if (selectingTrigger2) {
				for (int i = 0; i < 256; i++) {
					if (mem.IsKeyDown(i)) {
						settings::config::TriggerKey2 = i;
						selectingTrigger2 = false;
					}
				}
			}

			ImGui::Separator();
			break;
		}

		case VISUALS:
			ImGui::Text("Visuals");
			ImGui::Checkbox("ESP Visible Check", &settings::config::isVis);
			ImGui::Checkbox("Box", &settings::config::Box);
			ImGui::SliderFloat("Roundness", &settings::config::BoxRoundness, 0.0, 10.f);
			ImGui::SliderFloat("Thickness##Box", &settings::config::BoxThickness, 0.0, 10.f);
			ImGui::ColorEdit4("Not Visible##Box", settings::config::boxColor);
			ImGui::ColorEdit4("Visible##Box", settings::config::boxColorVisible);

			ImGui::Checkbox("Box Corners", &settings::config::CornerBox);
			ImGui::ColorEdit4("Not Visible##Box Corners", settings::config::CornerboxColor);
			ImGui::ColorEdit4("Visible##Box Corners", settings::config::CornerboxColorVisible);

			ImGui::Checkbox("Skeleton", &settings::config::Skeleton);
			ImGui::SliderFloat("Thickness##Skeleton", &settings::config::SkeletonThickness, 0.1f, 3.0f);
			ImGui::ColorEdit4("Not Visible##Skeleton", settings::config::skeletonColor);
			ImGui::ColorEdit4("Visible##Skeleton", settings::config::skeletonColorVisible);

			ImGui::Checkbox("Distance", &settings::config::Distance);
			ImGui::ColorEdit4("Not Visible##Distance", settings::config::DistanceColor);
			ImGui::ColorEdit4("Visible##Distance", settings::config::DistanceColorVisible);
			ImGui::Checkbox("Snaplines", &settings::config::Snaplines);
			if (settings::config::Snaplines) {
				ImGui::Checkbox("Snapline from Middle", &settings::config::SnaplineMiddle);
				ImGui::Checkbox("Snapline from Top", &settings::config::SnaplineTop);
			}
			ImGui::ColorEdit4("Not Visible##Snaplines", settings::config::SnapColor);
			ImGui::ColorEdit4("Visible##Snaplines", settings::config::SnapColorVisible);
			ImGui::Checkbox("Name", &settings::config::Name);;
			ImGui::ColorEdit4("Not Visible##Name", settings::config::NameColor);
			ImGui::ColorEdit4("Visible##Name", settings::config::NameColorVisible);
			ImGui::Checkbox("Platform", &settings::config::Platform);
			ImGui::ColorEdit4("Not Visible##Platform", settings::config::PlatformColor);
			ImGui::ColorEdit4("Visible##Platform", settings::config::PlatformColorVisible);
			ImGui::Checkbox("Weapon", &settings::config::Weaponname);
			ImGui::ColorEdit4("Not Visible##Weapon", settings::config::WeaponColor);
			ImGui::ColorEdit4("Visible##Weapon", settings::config::WeaponColorVisible);
			ImGui::Checkbox("Rank", &settings::config::Rank);
			ImGui::ColorEdit4("Not Visible##Rank", settings::config::RankColor);
			ImGui::ColorEdit4("Visible##Rank", settings::config::RankColorVisible);
			ImGui::Checkbox("Fov Arrows", &settings::config::FovArrows);
			ImGui::SliderInt("Fov Arrows Slider", &settings::config::FovArrowsSlider, 0.0f, 540.f);
			ImGui::ColorEdit4("Not Visible##FovArrows", settings::config::FovArrowsColor);
			ImGui::ColorEdit4("Visible##FovArrows", settings::config::FovArrowsColorVisible);
			ImGui::Checkbox("Kill Score", &settings::config::KillScore);
			ImGui::ColorEdit4("Not Visible##KillScore", settings::config::KillScoreColor);
			ImGui::ColorEdit4("Visible##KillScore", settings::config::KillScoreColorVisible);
			ImGui::Checkbox("Ignore NPC", &settings::config::Ignorenpc);
			ImGui::Checkbox("Spectators", &settings::config::Spectators);
			break;
		case RADAR:
			ImGui::Text("Radar");
			ImGui::Checkbox("Radar", &settings::config::Radar);
			ImGui::Checkbox("Radar Fuser", &settings::config::RadarFuser);
			ImGui::Checkbox("Radar Draw Center", &settings::config::RadarDrawCenter);
			ImGui::Checkbox("Show Distance", &settings::config::RadarDistance);
			settings::config::RadarXSize = settings::config::RadarYSize;
			ImGui::SliderFloat("Zoom", &settings::config::RadarZoom, 0.0f, 10.f);
			ImGui::Separator();
			ImGui::Text("Render Distance");
			ImGui::Checkbox("Enable Render Distance", &settings::config::EnableRenderDistance);
			ImGui::SliderFloat("Max Distance (m)", &settings::config::RenderDistance, 50.0f, 500.0f, "%.1f");
			ImGui::Separator();
			ImGui::Text("Watermark Colors");
			ImGui::ColorEdit4("Accent Color", settings::config::WatermarkAccentColor);
			ImGui::ColorEdit4("Background Color", settings::config::WatermarkBackgroundColor);
			ImGui::Separator();
			ImGui::Text("Crosshair");
			ImGui::Checkbox("Show Crosshair", &settings::config::Crosshair);
			ImGui::ColorEdit4("Crosshair Color", settings::config::CrosshairColor);
			break;

		case MISC:
			ImGui::Text("Misc");
			ImGui::Checkbox("Topmost", &settings::config::Topmost);
			ImGui::Checkbox("Fuser Mode", &settings::config::Fuser);
			ImGui::Checkbox("vSync", &settings::config::vSync);
			ImGui::Checkbox("Exit End Key", &settings::config::ExitCheat);
			ImGui::Separator();
			ImGui::Text("Custom Resolution");
			ImGui::InputInt("Window Width", &settings::window::Width);
			ImGui::InputInt("Window Height", &settings::window::Height);
			ImGui::Separator();
			ImGui::Text("Debug");
			ImGui::Checkbox("Statistics", &settings::menu::Statistics);
			ImGui::Checkbox("Advanced Debug", &settings::menu::AdvancedDebug);
			ImGui::Checkbox("Player List Viewer", &settings::menu::PlayerList);
			ImGui::Checkbox("Internals", &settings::menu::Internals);
			if (ImGui::Button("Close Cheat")) { // future niggas who see this, note I was bored...
				if (settings::config::Fuser = true) {
					settings::config::Fuser = false;
					DMAHandler::CloseDMA();
					std::cout << "Thanks for choosing kirowater.xyz / kiro-solutons!\n";
					std::cout << "Closing in 5s... Goodbye!";
					Sleep(5000);
					exit(0);
				}
				else
				{
					settings::config::Fuser = false;
					DMAHandler::CloseDMA();
					std::cout << "Thanks for choosing kirowater.xyz / kiro-solutons!\n";
					std::cout << "Closing in 5s... Goodbye!";
					Sleep(5000);
					exit(0);
				}
			settings::config::Fuser = false;
			DMAHandler::CloseDMA();
			std::cout << "Thanks for choosing kirowater.xyz / kiro-solutons!\n";
			std::cout << "Closing in 5s... Goodbye!";
			Sleep(5000);
			exit(0);
			}

			ImGui::Separator();
			ImGui::Text("Connection Test");
			if (ImGui::Button("Kmbox/Makcu test move"))
			{
				km_move(50, 50);
			}
			if (ImGui::Button("Kmbox/Makcu test shoot"))
			{
				km_click();
				km_unclick();
			}
			if (ImGui::Button("Kmbox net test move"))
			{
				kmNet_mouse_move(50, 50);
			}
			if (ImGui::Button("Kmbox net test shoot"))
			{
				kmNet_mouse_left(true);
				kmNet_mouse_left(false);
			}
			ImGui::Separator();
			ImGui::Text("Kmbox net");
			// on that chatgpt xD
			ImGui::InputText(
				"Ip",
				&settings::kmbox::net::ip[0], settings::kmbox::net::ip.capacity() + 1,
				ImGuiInputTextFlags_CallbackResize,
				StringInputTextCallback,
				&settings::kmbox::net::ip
			);
			ImGui::InputText(
				"Port",
				&settings::kmbox::net::port[0], settings::kmbox::net::port.capacity() + 1,
				ImGuiInputTextFlags_CallbackResize,
				StringInputTextCallback,
				&settings::kmbox::net::port
			);
			ImGui::InputText(
				"Uuid",
				&settings::kmbox::net::uuid[0], settings::kmbox::net::uuid.capacity() + 1,
				ImGuiInputTextFlags_CallbackResize,
				StringInputTextCallback,
				&settings::kmbox::net::uuid
			);
			if (ImGui::Button("Connect")) {
				if (connect_net_kmbox()) {
					settings::kmbox::NetKmbox = true;
					settings::kmbox::net::saveConfig();
					kmNet_lcd_picture_bottom((unsigned char*)images::mini_supernatural_image);
				}
			}
			break;
		case COLOR:
			ImGui::Text("Color Settings");
			ImGui::Separator();
			ImGui::Text("Window Colors");
			ImGui::ColorEdit4("Window Background", settings::colors::WindowBg);
			ImGui::ColorEdit4("Child Background", settings::colors::ChildBg);
			ImGui::ColorEdit4("Popup Background", settings::colors::PopupBg);
			ImGui::ColorEdit4("Border", settings::colors::Border);
			ImGui::ColorEdit4("Border Shadow", settings::colors::BorderShadow);

			// Text Colors
			ImGui::Text("Text Colors");
			ImGui::ColorEdit4("Text", settings::colors::Text);
			ImGui::ColorEdit4("Text Disabled", settings::colors::TextDisabled);
			ImGui::ColorEdit4("Text Selected Background", settings::colors::TextSelectedBg);

			// Title Colors
			ImGui::Text("Title Colors");
			ImGui::ColorEdit4("Title Background", settings::colors::TitleBg);
			ImGui::ColorEdit4("Title Background Active", settings::colors::TitleBgActive);
			ImGui::ColorEdit4("Title Background Collapsed", settings::colors::TitleBgCollapsed);

			// Menu Bar Colors
			ImGui::Text("Menu Bar Colors");
			ImGui::ColorEdit4("Menu Bar Background", settings::colors::MenuBarBg);

			// Scrollbar Colors
			ImGui::Text("Scrollbar Colors");
			ImGui::ColorEdit4("Scrollbar Background", settings::colors::ScrollbarBg);
			ImGui::ColorEdit4("Scrollbar Grab", settings::colors::ScrollbarGrab);
			ImGui::ColorEdit4("Scrollbar Grab Hovered", settings::colors::ScrollbarGrabHovered);
			ImGui::ColorEdit4("Scrollbar Grab Active", settings::colors::ScrollbarGrabActive);

			// Check Mark Colors
			ImGui::Text("Check Mark Colors");
			ImGui::ColorEdit4("Check Mark", settings::colors::CheckMark);

			// Slider Colors
			ImGui::Text("Slider Colors");
			ImGui::ColorEdit4("Slider Grab", settings::colors::SliderGrab);
			ImGui::ColorEdit4("Slider Grab Active", settings::colors::SliderGrabActive);

			// Header Colors
			ImGui::Text("Header Colors");
			ImGui::ColorEdit4("Header", settings::colors::Header);
			ImGui::ColorEdit4("Header Hovered", settings::colors::HeaderHovered);
			ImGui::ColorEdit4("Header Active", settings::colors::HeaderActive);

			// Button Colors
			ImGui::Text("Button Colors");
			ImGui::ColorEdit4("Button", settings::colors::Button);
			ImGui::ColorEdit4("Button Hovered", settings::colors::ButtonHovered);
			ImGui::ColorEdit4("Button Active", settings::colors::ButtonActive);

			// Frame Colors
			ImGui::Text("Frame Colors");
			ImGui::ColorEdit4("Frame Background", settings::colors::FrameBg);
			ImGui::ColorEdit4("Frame Background Hovered", settings::colors::FrameBgHovered);
			ImGui::ColorEdit4("Frame Background Active", settings::colors::FrameBgActive);

			// Resize Grip Colors
			ImGui::Text("Resize Grip Colors");
			ImGui::ColorEdit4("Resize Grip", settings::colors::ResizeGrip);
			ImGui::ColorEdit4("Resize Grip Hovered", settings::colors::ResizeGripHovered);
			ImGui::ColorEdit4("Resize Grip Active", settings::colors::ResizeGripActive);

			// Plot Colors
			ImGui::Text("Plot Colors");
			ImGui::ColorEdit4("Plot Lines", settings::colors::PlotLines);
			ImGui::ColorEdit4("Plot Lines Hovered", settings::colors::PlotLinesHovered);
			ImGui::ColorEdit4("Plot Histogram", settings::colors::PlotHistogram);
			ImGui::ColorEdit4("Plot Histogram Hovered", settings::colors::PlotHistogramHovered);

			// Snowflake Settings
			ImGui::Separator();
			ImGui::Text("Snowflake Settings");
			if (ImGui::Checkbox("Enable Snowflakes", &settings::colors::EnableSnowflakes)) {
				if (settings::colors::EnableSnowflakes) {
					if (settings::colors::FullscreenSnowflakes) {
						InitSnowflakes(settings::colors::SnowflakeCount, settings::window::Width, settings::window::Height);
					} else {
						InitSnowflakes(settings::colors::SnowflakeCount, size.x, size.y);
					}
					initialized = true;
				}
			}
			if (settings::colors::EnableSnowflakes) {
				if (ImGui::Checkbox("Fullscreen Mode", &settings::colors::FullscreenSnowflakes)) {
					// Reinitialize snowflakes with new dimensions when switching modes
					if (settings::colors::FullscreenSnowflakes) {
						InitSnowflakes(settings::colors::SnowflakeCount, settings::window::Width, settings::window::Height);
					} else {
						InitSnowflakes(settings::colors::SnowflakeCount, size.x, size.y);
					}
				}
				ImGui::ColorEdit4("Snowflake Color", settings::colors::SnowflakeColor);
				ImGui::SliderFloat("Snowflake Speed", &settings::colors::SnowflakeSpeed, 0.1f, 5.0f);
				if (ImGui::SliderInt("Snowflake Count", &settings::colors::SnowflakeCount, 10, 200)) {
					if (settings::colors::FullscreenSnowflakes) {
						InitSnowflakes(settings::colors::SnowflakeCount, settings::window::Width, settings::window::Height);
					} else {
						InitSnowflakes(settings::colors::SnowflakeCount, size.x, size.y);
					}
				}
			}
			break;
		case CONFIG:
			ImGui::Text("Configs");
			if (ImGui::BeginCombo("Select Config", settings::runtime::configList[settings::runtime::selectedConfigIndex].c_str())) {
				for (int n = 0; n < settings::runtime::configList.size(); n++) {
					bool isSelected = (settings::runtime::selectedConfigIndex == n);
					if (ImGui::Selectable(settings::runtime::configList[n].c_str(), isSelected)) {
						settings::runtime::selectedConfigIndex = n;
						settings::runtime::configFile = settings::runtime::configList[settings::runtime::selectedConfigIndex];
					}
					if (isSelected)
						ImGui::SetItemDefaultFocus();
				}
				ImGui::EndCombo();
			}
			if (ImGui::Button("Save"))
				settings::saveConfig();
			if (ImGui::Button("Load"))
				settings::loadConfig();
			break;
		}

		ImGui::EndChild();

		// Draw menu-only snowflakes if enabled and not in fullscreen mode
		if (settings::colors::EnableSnowflakes && !settings::colors::FullscreenSnowflakes) {
			static auto lastTime = std::chrono::high_resolution_clock::now();
			auto currentTime = std::chrono::high_resolution_clock::now();
			float deltaTime = std::chrono::duration<float>(currentTime - lastTime).count();
			lastTime = currentTime;

			UpdateSnowflakes(ImGui::GetForegroundDrawList(), menuPos, menuSize, deltaTime);
		}

		ImGui::End();
	}

	ImGui::PopFont();

	if (settings::menu::AdvancedDebug)
		AdvancedDebugWindow();

	if (settings::menu::PlayerList)
		PlayerListWindow();

	if (settings::menu::Statistics)
		StatisticsWindow();

	if (settings::menu::Internals)
		InternalsWindow();
}

void menu::InternalsWindow() {
	ImVec2 size = ImVec2(500, 350);
	ImGui::SetNextWindowSize(size);
	if (ImGui::Begin("Internals (Dev Only)", NULL, ImGuiWindowFlags_NoResize)) {

		ImGui::Separator();
		ImGui::Checkbox("GData", &settings::cheat::GData);
		ImGui::Separator();
		ImGui::Checkbox("Camera", &settings::cheat::Camera);
		ImGui::Separator();
		ImGui::Checkbox("PlayerList", &settings::cheat::PlayerList);
		ImGui::Separator();
		ImGui::Checkbox("Players", &settings::cheat::Players);
		ImGui::Separator();

	} ImGui::End();
}

void menu::StatisticsWindow() {
	ImVec2 size = ImVec2(500, 500);
	ImGui::SetNextWindowSize(size);
	if (ImGui::Begin("Statistics", NULL, ImGuiWindowFlags_NoResize)) {

		// MainUpdate
		const auto& PlayersValues = stats::updatePlayersData.getValues();
		float PlayersAvgMs = std::accumulate(PlayersValues.begin(), PlayersValues.end(), 0.0f) / PlayersValues.size();
		float PlayersMaxMs = PlayersValues.empty() ? 0.0f : *std::max_element(PlayersValues.begin(), PlayersValues.end());

		ImGui::Text("Main Update performance Graph (us per execution)");
		ImGui::PlotLines("##Performance", PlayersValues.data(), PlayersValues.size(), 0, nullptr, FLT_MAX, FLT_MAX, ImVec2(0, 100));
		ImGui::Text("Average: %.2f us Max: %.2f us", PlayersAvgMs, PlayersMaxMs);

		ImGui::Separator();

		// main thread
		const auto& MainValues = stats::mainThreadData.getValues();
		float mainAvgMs = std::accumulate(MainValues.begin(), MainValues.end(), 0.0f) / MainValues.size();
		float mainFPS = (mainAvgMs > 0) ? (1000.0f / mainAvgMs) : 0.0f;

		ImGui::Text("Main thread performance Graph (ms per loop)");
		ImGui::PlotLines("##Performance", MainValues.data(), MainValues.size(), 0, nullptr, FLT_MAX, FLT_MAX, ImVec2(0, 100));
		ImGui::Text("Average: %.2f ms (%.1f FPS)", mainAvgMs, mainFPS);

		ImGui::Separator();

		static bool advanced = false;

		ImGui::Checkbox("Advanced view", &advanced);

		ImGui::Separator();

		if (advanced) {
			// GData
			const auto& GDataValues = stats::GDataData.getValues();
			float GDataAvgMs = std::accumulate(GDataValues.begin(), GDataValues.end(), 0.0f) / GDataValues.size();
			float GDataMaxMs = GDataValues.empty() ? 0.0f : *std::max_element(GDataValues.begin(), GDataValues.end());

			ImGui::Text("GDatad performance Graph (ms per execution)");
			ImGui::PlotLines("##Performance", GDataValues.data(), GDataValues.size(), 0, nullptr, FLT_MAX, FLT_MAX, ImVec2(0, 100));
			ImGui::Text("Average: %.2f ms Max: %.2f ms", GDataAvgMs, GDataMaxMs);

			ImGui::Separator();

			// PlayerList
			const auto& PlayerListValues = stats::updatePlayerListData.getValues();
			float PlayerListAvgMs = std::accumulate(PlayerListValues.begin(), PlayerListValues.end(), 0.0f) / PlayerListValues.size();
			float PlayerListMaxMs = PlayerListValues.empty() ? 0.0f : *std::max_element(PlayerListValues.begin(), PlayerListValues.end());

			ImGui::Text("PlayerList performance Graph (ms per execution)");
			ImGui::PlotLines("##Performance", PlayerListValues.data(), PlayerListValues.size(), 0, nullptr, FLT_MAX, FLT_MAX, ImVec2(0, 100));
			ImGui::Text("Average: %.2f ms Max: %.2f ms", PlayerListAvgMs, PlayerListMaxMs);
		}

	} ImGui::End();
}

void menu::PlayerListWindow() {
	ImVec2 size = ImVec2(510, 750);

	std::unordered_map<uintptr_t, PlayerCache> PlayerList = secondPlayerList;
	static uintptr_t selectedPlayer = 0;

	// player list
	ImGui::SetNextWindowSize(size);

	if (ImGui::Begin("Player List", NULL, ImGuiWindowFlags_NoResize)) {
		ImGui::SetCursorPos(ImVec2(10, 30));
		ImGui::BeginChild("Player List", ImVec2(490, 710), ImGuiChildFlags_Border);

		for (auto it : PlayerList) {

			bool isSelected = (selectedPlayer == it.first);
			bool invalid = (!it.second.Pawn || !it.second.BoneArray);

			ImGui::PushStyleColor(ImGuiCol_Text, invalid ? ImColor(255, 0, 0, 255).Value : ImColor(0, 255, 0, 255).Value);

			if (ImGui::Selectable(std::format("PlayerState -> 0x{:X} {} {} ", it.first, invalid ? "(invalid)" : "(valid)", it.first == point::PlayerState ? "(me)" : "").c_str())) {
				selectedPlayer = it.first;
			}

			ImGui::PopStyleColor();

		}

		ImGui::EndChild();
	}ImGui::End();

	// details window
	if (selectedPlayer && PlayerList.find(selectedPlayer) != PlayerList.end()) {
		ImGui::SetNextWindowSize(ImVec2(400, 350));
		if (ImGui::Begin(std::format("Player Details for -> 0x{:X}", selectedPlayer).c_str(), NULL, ImGuiWindowFlags_NoResize)) {
			ImGui::Separator();
			ImGui::Text(std::format("Pawn -> 0x{:X}", PlayerList[selectedPlayer].Pawn).c_str());
			ImGui::Text(std::format("Mesh -> 0x{:X}", PlayerList[selectedPlayer].Mesh).c_str());
			uintptr_t BoneArray = PlayerList[selectedPlayer].BoneArrays[0].data;
			if (!BoneArray) BoneArray = PlayerList[selectedPlayer].BoneArrays[1].data;
			ImGui::Text(std::format("BoneArray -> 0x{:X}", BoneArray).c_str());
			ImGui::Text(std::format("RootComponent -> 0x{:X}", PlayerList[selectedPlayer].RootComponent).c_str());
			ImGui::Separator();
			ImGui::Text(std::format("Team -> {:d}", PlayerList[selectedPlayer].TeamId).c_str());
			//ImGui::Text(std::format("isDying -> {}", PlayerList[selectedPlayer].isDying & 0x10 ? "true" : "false").c_str());
			ImGui::Text(std::format("isDying -> {}", (PlayerList[selectedPlayer].isDying >> 5) & 1 ? "true" : "false").c_str());
			//ImGui::Text(std::format("isDBNO -> {}", PlayerList[selectedPlayer].isDBNO & 0x10 ? "true" : "false").c_str());
			ImGui::Text(std::format("isDBNO -> {}", (PlayerList[selectedPlayer].isDBNO >> 7) & 1 ? "true" : "false").c_str());
			ImGui::Text(std::format("isBot -> {}", (PlayerList[selectedPlayer].isBot >> 3) & 1 ? "true" : "false").c_str());
			//ImGui::Text("isDying -> %s", (PlayerList[selectedPlayer].isDying & 0x20) ? "true" : "false");
		//	ImGui::Text("isDBNO  -> %s", (PlayerList[selectedPlayer].isDBNO & 0x40) ? "true" : "false");
			//ImGui::Text("isBot   -> %s", (PlayerList[selectedPlayer].isBot ? "true" : "false"));
			ImGui::Separator();
			ImGui::Text(std::format("Velocity -> x: {:.2f} y: {:.2f} z: {:.2f}", PlayerList[selectedPlayer].Velocity.x, PlayerList[selectedPlayer].Velocity.y, PlayerList[selectedPlayer].Velocity.z).c_str());
			ImGui::Text(std::format("Last Render -> {:.2f}", PlayerList[selectedPlayer].last_render).c_str());
			ImGui::Separator();
			ImGui::Text("World Postions");
			ImGui::Text(std::format("Head -> x: {:.2f} y: {:.2f} z: {:.2f}", PlayerList[selectedPlayer].Head3D.x, PlayerList[selectedPlayer].Head3D.y, PlayerList[selectedPlayer].Head3D.z).c_str());
			ImGui::Text(std::format("Bottom -> x: {:.2f} y: {:.2f} z: {:.2f}", PlayerList[selectedPlayer].Bottom3D.x, PlayerList[selectedPlayer].Bottom3D.y, PlayerList[selectedPlayer].Bottom3D.z).c_str());
			ImGui::Text(std::format("Hip -> x: {:.2f} y: {:.2f} z: {:.2f}", PlayerList[selectedPlayer].Hip3D.x, PlayerList[selectedPlayer].Hip3D.y, PlayerList[selectedPlayer].Hip3D.z).c_str());
			ImGui::Text(std::format("Neck -> x: {:.2f} y: {:.2f} z: {:.2f}", PlayerList[selectedPlayer].Neck3D.x, PlayerList[selectedPlayer].Neck3D.y, PlayerList[selectedPlayer].Neck3D.z).c_str());
			ImGui::Text(std::format("UpperArmLeft -> x: {:.2f} y: {:.2f} z: {:.2f}", PlayerList[selectedPlayer].UpperArmLeft3D.x, PlayerList[selectedPlayer].UpperArmLeft3D.y, PlayerList[selectedPlayer].UpperArmLeft3D.z).c_str());
			ImGui::Text(std::format("UpperArmRight -> x: {:.2f} y: {:.2f} z: {:.2f}", PlayerList[selectedPlayer].UpperArmRight3D.x, PlayerList[selectedPlayer].UpperArmRight3D.y, PlayerList[selectedPlayer].UpperArmRight3D.z).c_str());
			ImGui::Text(std::format("LeftHand -> x: {:.2f} y: {:.2f} z: {:.2f}", PlayerList[selectedPlayer].LeftHand3D.x, PlayerList[selectedPlayer].LeftHand3D.y, PlayerList[selectedPlayer].LeftHand3D.z).c_str());
			ImGui::Text(std::format("RightHand -> x: {:.2f} y: {:.2f} z: {:.2f}", PlayerList[selectedPlayer].RightHand3D.x, PlayerList[selectedPlayer].RightHand3D.y, PlayerList[selectedPlayer].RightHand3D.z).c_str());
			ImGui::Text(std::format("LeftHand2 -> x: {:.2f} y: {:.2f} z: {:.2f}", PlayerList[selectedPlayer].LeftHandT3D.x, PlayerList[selectedPlayer].LeftHandT3D.y, PlayerList[selectedPlayer].LeftHandT3D.z).c_str());
			ImGui::Text(std::format("RightHand2 -> x: {:.2f} y: {:.2f} z: {:.2f}", PlayerList[selectedPlayer].RightHandT3D.x, PlayerList[selectedPlayer].RightHandT3D.y, PlayerList[selectedPlayer].RightHandT3D.z).c_str());
			ImGui::Text(std::format("RightThigh -> x: {:.2f} y: {:.2f} z: {:.2f}", PlayerList[selectedPlayer].RightThigh3D.x, PlayerList[selectedPlayer].RightThigh3D.y, PlayerList[selectedPlayer].RightThigh3D.z).c_str());
			ImGui::Text(std::format("LeftThigh -> x: {:.2f} y: {:.2f} z: {:.2f}", PlayerList[selectedPlayer].LeftThigh3D.x, PlayerList[selectedPlayer].LeftThigh3D.y, PlayerList[selectedPlayer].LeftThigh3D.z).c_str());
			ImGui::Text(std::format("RightCalf -> x: {:.2f} y: {:.2f} z: {:.2f}", PlayerList[selectedPlayer].RightCalf3D.x, PlayerList[selectedPlayer].RightCalf3D.y, PlayerList[selectedPlayer].RightCalf3D.z).c_str());
			ImGui::Text(std::format("LeftCalf -> x: {:.2f} y: {:.2f} z: {:.2f}", PlayerList[selectedPlayer].LeftCalf3D.x, PlayerList[selectedPlayer].LeftCalf3D.y, PlayerList[selectedPlayer].LeftCalf3D.z).c_str());
			ImGui::Text(std::format("LeftFoot -> x: {:.2f} y: {:.2f} z: {:.2f}", PlayerList[selectedPlayer].LeftFoot3D.x, PlayerList[selectedPlayer].LeftFoot3D.y, PlayerList[selectedPlayer].LeftFoot3D.z).c_str());
			ImGui::Text(std::format("RightFoot -> x: {:.2f} y: {:.2f} z: {:.2f}", PlayerList[selectedPlayer].RightFoot3D.x, PlayerList[selectedPlayer].RightFoot3D.y, PlayerList[selectedPlayer].RightFoot3D.z).c_str());
			ImGui::Separator();
			ImGui::Text("Screen Postions");
			ImGui::Text(std::format("Head -> x: {:.2f} y: {:.2f} z: {:.2f}", PlayerList[selectedPlayer].Head2D.x, PlayerList[selectedPlayer].Head2D.y, PlayerList[selectedPlayer].Head2D.z).c_str());
			ImGui::Text(std::format("Bottom -> x: {:.2f} y: {:.2f} z: {:.2f}", PlayerList[selectedPlayer].Bottom2D.x, PlayerList[selectedPlayer].Bottom2D.y, PlayerList[selectedPlayer].Bottom2D.z).c_str());
			ImGui::Text(std::format("Hip -> x: {:.2f} y: {:.2f} z: {:.2f}", PlayerList[selectedPlayer].Hip2D.x, PlayerList[selectedPlayer].Hip2D.y, PlayerList[selectedPlayer].Hip2D.z).c_str());
			ImGui::Text(std::format("Neck -> x: {:.2f} y: {:.2f} z: {:.2f}", PlayerList[selectedPlayer].Neck2D.x, PlayerList[selectedPlayer].Neck2D.y, PlayerList[selectedPlayer].Neck2D.z).c_str());
			ImGui::Text(std::format("UpperArmLeft -> x: {:.2f} y: {:.2f} z: {:.2f}", PlayerList[selectedPlayer].UpperArmLeft2D.x, PlayerList[selectedPlayer].UpperArmLeft2D.y, PlayerList[selectedPlayer].UpperArmLeft2D.z).c_str());
			ImGui::Text(std::format("UpperArmRight -> x: {:.2f} y: {:.2f} z: {:.2f}", PlayerList[selectedPlayer].UpperArmRight2D.x, PlayerList[selectedPlayer].UpperArmRight2D.y, PlayerList[selectedPlayer].UpperArmRight2D.z).c_str());
			ImGui::Text(std::format("LeftHand -> x: {:.2f} y: {:.2f} z: {:.2f}", PlayerList[selectedPlayer].LeftHand2D.x, PlayerList[selectedPlayer].LeftHand2D.y, PlayerList[selectedPlayer].LeftHand2D.z).c_str());
			ImGui::Text(std::format("RightHand -> x: {:.2f} y: {:.2f} z: {:.2f}", PlayerList[selectedPlayer].RightHand2D.x, PlayerList[selectedPlayer].RightHand2D.y, PlayerList[selectedPlayer].RightHand2D.z).c_str());
			ImGui::Text(std::format("LeftHand2 -> x: {:.2f} y: {:.2f} z: {:.2f}", PlayerList[selectedPlayer].LeftHandT2D.x, PlayerList[selectedPlayer].LeftHandT2D.y, PlayerList[selectedPlayer].LeftHandT2D.z).c_str());
			ImGui::Text(std::format("RightHand2 -> x: {:.2f} y: {:.2f} z: {:.2f}", PlayerList[selectedPlayer].RightHandT2D.x, PlayerList[selectedPlayer].RightHandT2D.y, PlayerList[selectedPlayer].RightHandT2D.z).c_str());
			ImGui::Text(std::format("RightThigh -> x: {:.2f} y: {:.2f} z: {:.2f}", PlayerList[selectedPlayer].RightThigh2D.x, PlayerList[selectedPlayer].RightThigh2D.y, PlayerList[selectedPlayer].RightThigh2D.z).c_str());
			ImGui::Text(std::format("LeftThigh -> x: {:.2f} y: {:.2f} z: {:.2f}", PlayerList[selectedPlayer].LeftThigh2D.x, PlayerList[selectedPlayer].LeftThigh2D.y, PlayerList[selectedPlayer].LeftThigh2D.z).c_str());
			ImGui::Text(std::format("RightCalf -> x: {:.2f} y: {:.2f} z: {:.2f}", PlayerList[selectedPlayer].RightCalf2D.x, PlayerList[selectedPlayer].RightCalf2D.y, PlayerList[selectedPlayer].RightCalf2D.z).c_str());
			ImGui::Text(std::format("LeftCalf -> x: {:.2f} y: {:.2f} z: {:.2f}", PlayerList[selectedPlayer].LeftCalf2D.x, PlayerList[selectedPlayer].LeftCalf2D.y, PlayerList[selectedPlayer].LeftCalf2D.z).c_str());
			ImGui::Text(std::format("LeftFoot -> x: {:.2f} y: {:.2f} z: {:.2f}", PlayerList[selectedPlayer].LeftFoot2D.x, PlayerList[selectedPlayer].LeftFoot2D.y, PlayerList[selectedPlayer].LeftFoot2D.z).c_str());
			ImGui::Text(std::format("RightFoot -> x: {:.2f} y: {:.2f} z: {:.2f}", PlayerList[selectedPlayer].RightFoot2D.x, PlayerList[selectedPlayer].RightFoot2D.y, PlayerList[selectedPlayer].RightFoot2D.z).c_str());
			ImGui::Separator();
		}ImGui::End();
	}
}

void menu::AdvancedDebugWindow() {
	ImVec2 size = ImVec2(800, 500);
	ImGui::SetNextWindowSize(size);
	if (ImGui::Begin("Advanced Debug"), NULL, ImGuiWindowFlags_NoResize) {

		int x = 0, y = 0;

		// first row
		x = 10; y = 30;

		ImGui::SetCursorPos(ImVec2(x, y += 20));
		ImGui::Text(std::format("Base -> 0x{:X}", point::Base).c_str());
		ImGui::SetCursorPos(ImVec2(x, y += 20));
		ImGui::Text(std::format("va_text -> 0x{:X}", point::va_text).c_str());
		ImGui::SetCursorPos(ImVec2(x, y += 20));
		ImGui::Text(std::format("Uworld -> 0x{:X}", point::Uworld).c_str());
		ImGui::SetCursorPos(ImVec2(x, y += 20));
		ImGui::Text(std::format("GameInstance -> 0x{:X}", point::GameInstance).c_str());
		ImGui::SetCursorPos(ImVec2(x, y += 20));
		ImGui::Text(std::format("PersistentLevel -> 0x{:X}", point::PersistentLevel).c_str());
		ImGui::SetCursorPos(ImVec2(x, y += 20));
		ImGui::Text(std::format("GameState -> 0x{:X}", point::GameState).c_str());
		ImGui::SetCursorPos(ImVec2(x, y += 20));
		ImGui::Text(std::format("LocalPlayers -> 0x{:X}", point::LocalPlayers).c_str());
		ImGui::SetCursorPos(ImVec2(x, y += 20));
		ImGui::Text(std::format("LocalPlayer -> 0x{:X}", point::LocalPlayer).c_str());
		ImGui::SetCursorPos(ImVec2(x, y += 20));
		ImGui::Text(std::format("PlayerController -> 0x{:X}", point::PlayerController).c_str());
		ImGui::SetCursorPos(ImVec2(x, y += 20));
		ImGui::Text(std::format("LocalPawn -> 0x{:X}", point::Player).c_str());
		ImGui::SetCursorPos(ImVec2(x, y += 20));
		ImGui::Text(std::format("PlayerCameraManager -> 0x{:X}", point::PlayerCameraManager).c_str());
		ImGui::SetCursorPos(ImVec2(x, y += 20));
		ImGui::Text(std::format("PlayerState -> 0x{:X}", point::PlayerState).c_str());

		ImGui::SetCursorPos(ImVec2(x, y += 40));
		ImGui::Text(std::format("Players Rendered -> {:d}", info::render::playersRendered).c_str());
		ImGui::SetCursorPos(ImVec2(x, y += 20));
		ImGui::Text(std::format("Players Looped -> {:d}", info::render::playersLooped).c_str());
		ImGui::SetCursorPos(ImVec2(x, y += 20));
		ImGui::Text(std::format("Valid Players Looped -> {:d}", info::render::validPlayersLooped).c_str());
		ImGui::SetCursorPos(ImVec2(x, y += 20));
		ImGui::Text(std::format("Invalid Players Looped -> {:d}", info::render::invalidPlayersLooped).c_str());
		ImGui::SetCursorPos(ImVec2(x, y += 20));
		ImGui::Text(std::format("Valid Bots Looped -> {:d}", info::render::validBots).c_str());
		ImGui::SetCursorPos(ImVec2(x, y += 20));
		ImGui::Text(std::format("Teammates Skipped -> {:d}", info::render::teammatesSkipped).c_str());

		// second row

		x = 310; y = 30;

		ImGui::SetCursorPos(ImVec2(x, y += 20));
		ImGui::Text(std::format("LocationPointer -> 0x{:X}", point::LocationPointer).c_str());
		ImGui::SetCursorPos(ImVec2(x, y += 20));
		ImGui::Text(std::format("RotationPointer -> 0x{:X}", point::RotationPointer).c_str());
		ImGui::SetCursorPos(ImVec2(x, y += 20));
		ImGui::Text(std::format("Camera Location -> x: {:.2f} y: {:.2f} z: {:.2f}", mainCamera.Location.x, mainCamera.Location.y, mainCamera.Location.z).c_str());
		ImGui::SetCursorPos(ImVec2(x, y += 20));
		ImGui::Text(std::format("Camera Rotation -> x: {:.2f} y: {:.2f}", mainCamera.Rotation.x, mainCamera.Rotation.y).c_str());
		ImGui::SetCursorPos(ImVec2(x, y += 20));
		ImGui::Text(std::format("Camera FieldOfView -> {:.2f} ", mainCamera.FieldOfView).c_str());
		ImGui::SetCursorPos(ImVec2(x, y += 20));
		ImGui::Text(std::format("Location Under Reticle -> x: {:.2f} y: {:.2f} z: {:.2f}", mainCamera.LocationUnderReticle.x, mainCamera.LocationUnderReticle.y, mainCamera.LocationUnderReticle.z).c_str());
		ImGui::SetCursorPos(ImVec2(x, y += 20));
		ImGui::Text(std::format("Seconds -> {:.2f}", point::Seconds).c_str());
		ImGui::SetCursorPos(ImVec2(x, y += 20));
		ImGui::Text(std::format("Mouse Sens X -> {:.4f}", point::MouseSensX).c_str());
		ImGui::SetCursorPos(ImVec2(x, y += 20));
		ImGui::Text(std::format("Mouse Sens Y -> {:.4f}", point::MouseSensY).c_str());
		ImGui::SetCursorPos(ImVec2(x, y += 40));
		ImGui::Text(std::format("Local Team -> {:d}", local_player::localTeam).c_str());
		//ImGui::SetCursorPos(ImVec2(x, y += 20));
		//ImGui::Text(std::format("LocalPlayers -> 0x{:X}", point::LocalPlayers).c_str());
		//ImGui::SetCursorPos(ImVec2(x, y += 20));
		//ImGui::Text(std::format("LocalPlayer -> 0x{:X}", point::LocalPlayer).c_str());
		ImGui::SetCursorPos(ImVec2(x, y += 40));
		ImGui::Text(std::format("PlayerArray -> 0x{:X}", point::PlayerArray.data).c_str());
		ImGui::SetCursorPos(ImVec2(x, y += 20));
		ImGui::Text(std::format("PlayerArray Count -> {:d}", point::PlayerArray.count).c_str());

	} ImGui::End();


}
