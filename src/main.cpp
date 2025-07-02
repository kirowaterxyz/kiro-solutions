// dont move
#include "kmbox/kmboxNet.h"
#include "kmbox/HidTable.h"
//

#include <Windows.h>
#include <dwmapi.h>
#include <d3d11.h>
#include <chrono>
#include <fstream>
#include <iostream>
#include <string>

#include "performance.h"
#include "settings.h"

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include <imgui/imgui_impl_dx11.h>
#include <imgui/imgui_impl_win32.h>

#include "rendering/Font.h"
#define STB_IMAGE_IMPLEMENTATION
#include "rendering/stb_image.h"
#include "rendering/images.h"

#include "menu/menu.h"

#include "rendering/window.h"

#include "dma/DMAHandler.h"
#include "kmbox/kmbox_interface.h"

#include "kmbox/kmbox_util.h"

#include "cheat/cheat.h"
#include "cheat/aim.h"
#include "cheat/radar.h"
#include "cheat/esp.h"

#include <wininet.h>
#pragma comment(lib, "wininet.lib")			

void CheckForUpdates() {
	HINTERNET hInternet = InternetOpenA("UpdateCheck", INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);
	if (!hInternet) return;

	HINTERNET hFile = InternetOpenUrlA(
		hInternet,
		"https://raw.githubusercontent.com/kirowater/kiro-solution-cheat/main/version.txt", // Change to your URL
		NULL, 0, INTERNET_FLAG_RELOAD, 0);

	if (hFile) {
		char buffer[16] = { 0 };
		DWORD bytesRead;
		if (InternetReadFile(hFile, buffer, sizeof(buffer) - 1, &bytesRead) && bytesRead > 0) {
			buffer[bytesRead] = '\0';
			std::string versionStr(buffer);
			versionStr.erase(versionStr.find_last_not_of(" \n\r\t") + 1); // Trim trailing whitespace
			float latestVersion = std::stof(versionStr);
			float currentVersion = settings::runtime::version_major + settings::runtime::version_minor / 10.0f;
			std::cout << "Current version: " << currentVersion << "\nLatest version: " << latestVersion << std::endl; // Debug print
			if (latestVersion > currentVersion) {
				int result = MessageBoxA(NULL, "A new version is available!\nDownload Yes, Ignore No/Cancel", "kiro-solutions updater", MB_YESNOCANCEL | MB_ICONQUESTION);
				if (result == IDYES) {
					ShellExecuteA(NULL, "open", "https://github.com/kirowater/kiro-solution-cheat/releases", NULL, NULL, SW_SHOWNORMAL);
					system("cls");
				}
				system("cls");
			}		
		}
		system("cls");
		InternetCloseHandle(hFile);
	}
	InternetCloseHandle(hInternet);
}

vector<feature> memoryList;
vector<feature> mainList;

void memRefreshLight() {
	if (!point::Base) {
		std::cout << "(!) point::Base is null in memRefreshLight" << std::endl;
		return;
	}
	try {
		mem.RefreshLight();
	}
	catch (const std::exception& ex) {
		std::cout << "(!) Exception in memRefreshLight: " << ex.what() << std::endl;
	}
}


void memKeysUpdate() {
	// features that need the keys updated
	if (!settings::config::Aimbot && !settings::config::TriggerBot)
		return;

	mem.UpdateKeys();
}

bool on_initialize() {
	std::cout << "(+) Welcome to the kiro-solutions! credits: sfyass, kirowater.xyz" << std::endl;
	std::cout << "(+) This is a free cheat if you paid for this you got scammed!" << std::endl;
	std::cout << "(+) Join our discord server: discord.gg/d4ZhmGTMNW\n" << std::endl;
	if (connect_serial_kmbox()) {
		settings::kmbox::SerialKmbox = true;
	}

	// configs
	settings::loadConfig();

	// try to connect to kmbox net
	if (connect_net_kmbox()) {
		settings::kmbox::NetKmbox = true;
		kmNet_lcd_picture_bottom((unsigned char*)images::mini_supernatural_image);
	}
	
	if (mem.Init(L"FortniteClient-Win64-Shipping.exe", settings::dma::MemoryMap) < 0) {
		std::cout << "(!) Failed to initialize" << std::endl;
		return false;
	}

	std::cout << "(+) Initialized VMMDLL" << std::endl;

	mem.FixDTB();

	// idk why sometimes it fails
	point::Base = mem.GetBaseAddress();
	if (!point::Base)
	{
		std::cout <<  "(!) Failed to refresh process" << std::endl; // couldnt get base
		return false;
	}

	std::cout << "(+) Successfully refreshed process" << std::endl;

	if (!mem.SCreate()) {
		std::cout << "(!) Failed to initialize all handles" << std::endl;
		return false;
	}

	std::cout << "(+) Scatter handles Created" << std::endl;	
	
	if (!mem.InitKeyboard()) 
	{
		std::cout << "(/) Failed to initialize keyboard hotkeys" << std::endl;
	}
	else {
		settings::runtime::hotKeys = true;
		std::cout << "(+) Initialized keyboard hotkeys" << std::endl;
	}

	// no longer any offset (for now)
	point::va_text = point::Base;
	//if (!update_va_text()) {
		//std::cout << hue::red << "(!) " << hue::white << "Failed to get text_va" << std::endl;
		//return false;
	//}

	// memory features
	{
		// refresh memory LOW PRIORITY
		feature RefreshLight = { memRefreshLight , 1, 500 }; // this is needed without this esp,aimbot everything will not work can cause crashes also btw
		memoryList.push_back(RefreshLight);

		// update uworld and basics LOW PRIORITY / FAST
		feature GDataUpdate = { newInfo , 1, 1000 };
		memoryList.push_back(GDataUpdate);

		// update local weapon projectile stats MID PRIORITY
		feature WeaponUpdate = { weaponUpdate, 1, 500 };
		memoryList.push_back(WeaponUpdate);

		// update player list MID PRIORITY
		feature PlayerListUpdate = { updatePlayerList , 1, 1000 };
		memoryList.push_back(PlayerListUpdate);

		// update keys MID/HIGH PRIORITY
		feature KeysUpdate = { memKeysUpdate , 1, 5 };
		memoryList.push_back(KeysUpdate);

	}

	// main thread features
	{
		// aimbot
		feature Aimbot = { aim::updateAimbot, 1, 5 };
		mainList.push_back(Aimbot);

		// triggerbot
		feature Triggerbot = { aim::updateTriggerBot, 1, 5 };
		mainList.push_back(Triggerbot);

		// debugging drawings
		feature Debug = { esp::Debug, 1, -1 };
		mainList.push_back(Debug);

		// update camera and players location
		// update camera and players location
		feature PlayersUpdate = { MainUpdate , 1, -1 };
		mainList.push_back(PlayersUpdate);

		// esp 
		feature Walls = { esp::renderPlayers, 1, -1 };
		mainList.push_back(Walls);

		// radar
		feature Radar = { radar::Render, 1, -1 };
		mainList.push_back(Radar);
	}

	return true;
}

static void on_exit() {
	CloseHandle(hSerial);

	if (!settings::runtime::headless) {
		fclose(stdin);
		fclose(stdout);
		fclose(stderr);
	}
}

void memoryloop() {

	if (settings::runtime::criticalPriority) {
		// set thread priority
		if (!SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_TIME_CRITICAL))
			std::cout << "(/) Failed to set critical priority to memory thread" << std::endl;
		// set thread class
		if (!SetPriorityClass(GetCurrentProcess(), REALTIME_PRIORITY_CLASS))
			std::cout << "(/) Failed to set critical priority class to memory thread" << std::endl;
	}

	// never quit?
	while (true) {
		auto start = std::chrono::high_resolution_clock::now();

		for (feature& i : memoryList) {
			try {
				if ((chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now().time_since_epoch()).count() - i.lasttime) >= i.period) {
					i.lasttime = chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now().time_since_epoch()).count();
					i.func();
				}
			}
			catch (const std::exception& ex) {
				std::cout << "(!) Exception in memory feature: " << ex.what() << std::endl;
			}
		}


		__int64 elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start).count();
		stats::memoryThreadData.addValue(static_cast<float>(elapsed));
	}
}

void mainFeatures() {

	for (feature& i : mainList) {
		try {
			if ((chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now().time_since_epoch()).count() - i.lasttime) >= i.period) {
				i.lasttime = chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now().time_since_epoch()).count();
				i.func();
			}
		}
		catch (const std::exception& ex) {
			std::cout << "(!) Exception in main feature: " << ex.what() << std::endl;
		}
	}
}


void mainloop() {

	// 5s exit if END key is pressed
	if (settings::config::ExitCheat) {
		if (GetAsyncKeyState(VK_END) & 1) {
			settings::config::Fuser = false;
			DMAHandler::CloseDMA();
			std::cout << "Thanks for choosing kirowater.xyz / kiro-solutons!\n";
			std::cout << "Closing in 5s... Goodbye!";
			Sleep(5000);
			on_exit();
			exit(0);
		}
		if (mem.IsKeyDown(VK_END) & 1) {
			settings::config::Fuser = false;
			DMAHandler::CloseDMA();
			std::cout << "Thanks for choosing kirowater.xyz / kiro-solutons!\n";
			std::cout << "Closing in 5s... Goodbye!";
			Sleep(5000);
			on_exit();
			exit(0);
		}
	}

	static bool prevInsertState = false;
	bool currentInsertState = mem.IsKeyDown(VK_INSERT);

	if (!settings::config::MoonlightAim) {
		if (ImGui::IsKeyPressed(ImGuiKey_Insert)) {
			settings::menu::open = !settings::menu::open;
		}

		// Debounced check from mem (main PC)
		if (currentInsertState && !prevInsertState) {
			settings::menu::open = !settings::menu::open;
		}
	}
	else {
		if (GetAsyncKeyState(VK_INSERT) & 1) {
			settings::menu::open = !settings::menu::open;
		}

		// Debounced check from mem (Moonlight branch)
		if (currentInsertState && !prevInsertState) {
			settings::menu::open = !settings::menu::open;
		}


		ImGuiIO& io = ImGui::GetIO();
		io.DeltaTime = 1.0f / 60.0f;
		POINT p;
		GetCursorPos(&p);
		io.MousePos.x = p.x;
		io.MousePos.y = p.y;
		if (GetAsyncKeyState(0x1)) {
			io.MouseDown[0] = true;
			io.MouseClicked[0] = true;
			io.MouseClickedPos[0].x = io.MousePos.x;
			io.MouseClickedPos[0].y = io.MousePos.y;
		}
		else
			io.MouseDown[0] = false;
	}

	prevInsertState = currentInsertState;

	if (settings::menu::open)
		menu::Menu();

	mainFeatures();

	return;
}

static INT APIENTRY WinMain(_In_ HINSTANCE instance, _In_opt_ HINSTANCE prev_instance, _In_ PSTR cmd_line, _In_ INT cmd_show) {  

   if (!settings::runtime::headless) {  
       AllocConsole();  
       freopen("CONIN$", "r", stdin);  
       freopen("CONOUT$", "w", stdout);  
       freopen("CONOUT$", "w", stderr);  
       SetConsoleTitleA(("kiro-solutions v" + std::to_string(settings::runtime::version_major) + "." + std::to_string(settings::runtime::version_minor)).c_str());
   }  
   CheckForUpdates();
   if (settings::runtime::criticalPriority) {  
       // set thread priority  
       if (!SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_TIME_CRITICAL))  
           std::cout << "(/) Failed to set critical priority to main thread" << std::endl;  
	   // set thread class
	   if (!SetPriorityClass(GetCurrentProcess(), REALTIME_PRIORITY_CLASS))
		   std::cout << "(/) Failed to set critical priority class to main thread" << std::endl;
   }  

   if (!settings::runtime::graphicsOnly) {  
       if (!on_initialize()) {  
           std::cout << "(/) Press enter to exit" << std::endl;  
           std::cin.get();  
           return 1;  
       }

		thread memoryThread(memoryloop);  
		memoryThread.detach();  
   }  

   // wanted to make it run without a window for debugging not sure if needed  
   if (!settings::runtime::windowless) {  
       InitWindow(instance, cmd_show);  

       while (UpdateWindow(mainloop)) {  

       }  

       DestroyRenderWindow();  
   } else {  
       while (true) {  
           auto start = std::chrono::high_resolution_clock::now();  

           mainFeatures();  

           __int64 elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start).count();  
           stats::mainThreadData.addValue(static_cast<float>(elapsed));  
       }  
   }  
}
