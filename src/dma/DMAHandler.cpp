#include "DMAHandler.h"

#include <fstream>
#include <chrono>
#include <thread>
#include <leechcore.h>
#include <filesystem>

uint64_t cbSize = 0x80000;
//callback for VfsFileListU
VOID cbAddFile(_Inout_ HANDLE h, _In_ LPCSTR uszName, _In_ ULONG64 cb, _In_opt_ PVMMDLL_VFS_FILELIST_EXINFO pExInfo) 
{
	if (strcmp(uszName, "dtb.txt") == 0)
		cbSize = cb;
}

struct Info {
	uint32_t index;
	uint32_t process_id;
	uint64_t dtb;
	uint64_t kernelAddr;
	std::string name;
};

void clearLine() {
	std::cout << "                                                                                                                \r" << std::flush;
}

//Directory Table Base requires: symsrv.dll, dbghelp.dll and info.db
int DMAHandler::FixDTB() // used and updated
{
	PVMMDLL_MAP_MODULEENTRY module_entry = NULL;
	bool result = VMMDLL_Map_GetModuleFromNameU(this->DMA_HANDLE, this->GetPID(), (LPSTR)this->GameName.c_str(), &module_entry, NULL);
	if (result) {
		//log("[+] DTB patch not needed, module already found.");
		std::cout << "(+) Dtb fix and tables caching was not needed" << std::endl;
		return 1;
	}

	if (!VMMDLL_InitializePlugins(this->DMA_HANDLE)) {
		//log("[-] Failed to initialize VMMDLL plugins");
		std::cout << "(/) Failed to initialize VMMDLL plugins\r" << std::flush;
		return -1;
	}

	//log("[+] Initializing plugins...");
	std::cout << "(+) Initializing plugins..\r" << std::flush;
	std::this_thread::sleep_for(std::chrono::milliseconds(500));

	while (true) {
		BYTE bytes[4] = { 0 };
		DWORD i = 0;
		auto nt = VMMDLL_VfsReadW(this->DMA_HANDLE, const_cast<LPWSTR>(L"\\misc\\procinfo\\progress_percent.txt"), bytes, 3, &i, 0);
		if (nt == VMMDLL_STATUS_SUCCESS && atoi(reinterpret_cast<LPSTR>(bytes)) == 100)
			break;

		if (nt == VMMDLL_STATUS_SUCCESS) {
			std::string content(reinterpret_cast<char*>(bytes));
			//log("[+] Initializing VMMDLL plugins.. (%s%%)", content.c_str());
			std::cout << "(+) Initializing VMMDLL plugins.. (" << content << "%)\r" << std::flush;
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}

	VMMDLL_VFS_FILELIST2 VfsFileList;
	VfsFileList.dwVersion = VMMDLL_VFS_FILELIST_VERSION;
	VfsFileList.h = 0;
	VfsFileList.pfnAddDirectory = 0;
	VfsFileList.pfnAddFile = cbAddFile;

	result = VMMDLL_VfsListU(this->DMA_HANDLE, const_cast<LPSTR>("\\misc\\procinfo\\"), &VfsFileList);
	if (!result) {
		//log("[-] Failed to list VFS files");
		return false;
	}

	const size_t buffer_size = cbSize;
	std::unique_ptr<BYTE[]> bytes(new BYTE[buffer_size]);
	DWORD j = 0;
	auto nt = VMMDLL_VfsReadW(this->DMA_HANDLE, const_cast<LPWSTR>(L"\\misc\\procinfo\\dtb.txt"), bytes.get(), buffer_size - 1, &j, 0);
	if (nt != VMMDLL_STATUS_SUCCESS) {
		//log("[-] Failed to read dtb.txt from VFS");
		return false;
	}

	//log("[+] Recovering possible DTBs...");
	std::cout << "(+) Recovering possible dtb's..\r" << std::flush;
	std::vector<uint64_t> possible_dtbs;
	std::string lines(reinterpret_cast<char*>(bytes.get()));
	std::istringstream iss(lines);
	std::string line;

	while (std::getline(iss, line)) {
		Info info = {};
		std::istringstream info_ss(line);
		if (info_ss >> std::hex >> info.index >> std::dec >> info.process_id >> std::hex >> info.dtb >> info.kernelAddr >> info.name) {
			if (info.process_id == 0)
				possible_dtbs.push_back(info.dtb);
			if (strstr(info.name.c_str(), this->GameName.c_str()))
				possible_dtbs.push_back(info.dtb);
		}
	}

	static ULONG64 pml4[512];
	DWORD readsize;
	VMMDLL_MemReadEx(this->DMA_HANDLE, -1, processInfo.pid, reinterpret_cast<PBYTE>(pml4), sizeof(pml4), (PDWORD)&readsize, VMMDLL_FLAG_NOCACHE | VMMDLL_FLAG_NOPAGING | VMMDLL_FLAG_ZEROPAD_ON_FAIL | VMMDLL_FLAG_NOPAGING_IO);
	VMMDLL_MemReadEx((VMM_HANDLE)-666, 333, (ULONG64)pml4, 0, 0, 0, 0);
	VMMDLL_ConfigSet(this->DMA_HANDLE, VMMDLL_OPT_PROCESS_DTB | this->GetPID(), 666);

	//log("[+] Trying possible DTBs...");
	std::cout << "(+) Trying possible dtb's..\r" << std::flush;
	for (size_t i = 0; i < possible_dtbs.size(); i++) {
		auto dtb = possible_dtbs[i];
		VMMDLL_ConfigSet(this->DMA_HANDLE, VMMDLL_OPT_PROCESS_DTB | this->GetPID(), dtb);
		bool result = VMMDLL_Map_GetModuleFromNameU(this->DMA_HANDLE, this->GetPID(), (LPSTR)this->GameName.c_str(), &module_entry, NULL);
		if (result) {
			//log("[+] Correct DTB found and patched.");
			std::cout << "(+) Correct dtb found\r" << std::flush;
			//processInfo.dtb = dtb;
			return true;
		}
	}

	//log("[-] Failed to patch module with any DTB.");
	std::cout << "(!) Failed to find correct dtb\r" << std::endl;
	return false;
}

ULONG64 DMAHandler::GetModuleAddress(std::wstring modulename) // used and updated needed
{
	if (DMAHandler::Modules.contains(modulename))
		return DMAHandler::Modules[modulename];
	DMAHandler::Modules[modulename] = VMMDLL_ProcessGetModuleBase(this->DMA_HANDLE, GetPID(), (LPWSTR)modulename.c_str());
	return DMAHandler::Modules[modulename];
} // used
void DMAHandler::log(const char* fmt, ...) // used
{
	//small check before processing
	if (strlen(fmt) > 2000) return;
	char logBuffer[2001] = { 0 };

	//time related
	const auto now_time_t = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
	std::tm time_info;
	localtime_s(&time_info, &now_time_t);
	// Format time as a string
	std::ostringstream oss;
	oss << std::put_time(&time_info, "%H:%M:%S");
	va_list args;
	va_start(args, fmt);
	vsprintf_s(logBuffer, 2000, fmt, args);
	printf("[DMA @ %s]: %s\n", oss.str().c_str(), logBuffer);
}

void DMAHandler::assertNoInit() const // not used only in write... readbool etc
{
	if (!DMA_HANDLE || !PROCESS_INITIALIZED)
	{
		//log("DMA or process not inizialized!");
		//throw new std::string("DMA not inizialized!");
	}

}

void DMAHandler::retrieveScatter(VMMDLL_SCATTER_HANDLE handle, void* buffer, void* target, SIZE_T size) // used update needed
{
	if (!handle) {
		//log("Invalid handle!");
		return;
	}
	DWORD bytesRead = 0;
	if (!VMMDLL_Scatter_Read(handle, reinterpret_cast<ULONG64>(target), size, static_cast<PBYTE>(buffer), &bytesRead))
		log("Scatter read for %p failed partly or full! Bytes written: %d/%d", target, bytesRead, size);
}

unsigned char abort2[4] = { 0x10, 0x00, 0x10, 0x00 };


int DMAHandler::Init(const wchar_t* wname, bool memMap) // used and updated
{
	WGameName = wname;
	GameName = std::string(WGameName.begin(), WGameName.end());

	if (!DMA_HANDLE)
	{
		//log("Loading libraries...");
		modules.VMM = LoadLibraryA("vmm.dll");
		modules.FTD3XX = LoadLibraryA("FTD3XX.dll");
		modules.LEECHCORE = LoadLibraryA("leechcore.dll");
		LoadLibraryA("dbghelp.dll");
		LoadLibraryA("symsrv.dll");

		/*if (!modules.VMM || !modules.FTD3XX || !modules.LEECHCORE)
		{
			log("vmm: %p", modules.VMM);
			log("ftd: %p", modules.FTD3XX);
			log("leech: %p", modules.LEECHCORE);
			log("[!] Could not load a required library");
			return -1;
		}*/

		if (!modules.VMM)
		{
			std::cout << "(!) Could not load vmm.dll\r" << std::endl;
			return -1;
		}

		if (!modules.VMM)
		{
			std::cout << "(!) Could not load FTD3XX.dll\r" << std::endl;
			return -1;
		}

		if (!modules.VMM)
		{
			std::cout << "(!) Could not load leechcore.dll\r" << std::endl;
			return -1;
		}

		//log("Successfully loaded libraries!");

		LPCSTR args[8] = { "", "-device", "fpga", "-norefresh", "", "", "", "" };
		DWORD argc = 4;

		std::string mmap_path;
		if (memMap)
		{
			auto temp_path = std::filesystem::temp_directory_path();
			mmap_path = temp_path.string() + "\\mmap.txt";
			bool dumped = false;
			if (!std::filesystem::exists(mmap_path))
				dumped = DumpMemoryMap();
			else
				dumped = true;

			//log("Dumping memory map to file...");
			if (!dumped)
			{
				//log("[!] ERROR: Could not dump memory map!");
				//log("Defaulting to no memory map!");
				std::cout << "(/) Failed to dump memory\r" << std::endl;
			}
			else
			{
				//log("Dumped memory map!");
				args[argc++] = "-memmap";
				args[argc++] = mmap_path.c_str();
			}
		}

		DMA_HANDLE = VMMDLL_Initialize(argc, args);
		if (!DMA_HANDLE)
		{
			//log("[!] Failed to connect to fpga device");
			std::cout << "(!) Failed to connect to fpga device\r" << std::endl;
			return -1;
		}

		ULONG64 FPGA_ID = 0, DEVICE_ID = 0;
		VMMDLL_ConfigGet(DMA_HANDLE, LC_OPT_FPGA_FPGA_ID, &FPGA_ID);
		VMMDLL_ConfigGet(DMA_HANDLE, LC_OPT_FPGA_DEVICE_ID, &DEVICE_ID);

		ULONG64 qwID = 0, qwVersionMajor = 0, qwVersionMinor = 0;
		if (!VMMDLL_ConfigGet(this->DMA_HANDLE, LC_OPT_FPGA_FPGA_ID, &qwID) && VMMDLL_ConfigGet(this->DMA_HANDLE, LC_OPT_FPGA_VERSION_MAJOR, &qwVersionMajor) && VMMDLL_ConfigGet(this->DMA_HANDLE, LC_OPT_FPGA_VERSION_MINOR, &qwVersionMinor))
		{
			//LOG("[!] Failed to lookup FPGA device, Attempting to proceed\n\n");
			return false;
		}

		//LOG("[+] VMMDLL_ConfigGet");
		//LOG(" ID = %lli", qwID);
		//LOG(" VERSION = %lli.%lli\n", qwVersionMajor, qwVersionMinor);

		if ((qwVersionMajor >= 4) && ((qwVersionMajor >= 5) || (qwVersionMinor >= 7)))
		{
			HANDLE handle;
			LC_CONFIG config = { .dwVersion = LC_CONFIG_VERSION, .szDevice = "existing" };
			handle = LcCreate(&config);
			if (!handle)
			{
				//LOG("[!] Failed to create FPGA device\n");
				return false;
			}

			LcCommand(handle, LC_CMD_FPGA_CFGREGPCIE_MARKWR | 0x002, 4, reinterpret_cast<PBYTE>(&abort2), NULL, NULL);
		//	LOG("[-] Register auto cleared\n");
			LcClose(handle);
		}

		//log("FPGA ID: %llu", FPGA_ID);
		//log("DEVICE ID: %llu", DEVICE_ID);
		//log("DMA initialization success!");
	}
	else
	{
		//log("DMA already initialized!");
	}

	// Convert the wide string to a standard string because VMMDLL_PidGetFromName expects LPSTR.
	std::wstring ws(wname);
	const std::string str(ws.begin(), ws.end());

	processInfo.name = str;
	processInfo.wname = wname;
	if (!VMMDLL_PidGetFromName(DMA_HANDLE, const_cast<char*>(processInfo.name.c_str()), &processInfo.pid))
	{
		//log("[!] Failed to find game process: %s", processInfo.name.c_str());
		std::cout << "(!) Failed to find game\r" << std::endl;
		return -1;
	}
	else
	{
		PROCESS_INITIALIZED = TRUE;
		//log("[+] Process initialized: %s (PID: %u)", processInfo.name.c_str(), processInfo.pid);
	}

	return 0;
}


void DMAHandler::RefreshLight() { // used not updated
	if (!DMA_HANDLE) return;
	VMMDLL_ConfigSet(DMA_HANDLE, VMMDLL_OPT_REFRESH_FREQ_MEM_PARTIAL, 1);
	VMMDLL_ConfigSet(DMA_HANDLE, VMMDLL_OPT_REFRESH_FREQ_TLB_PARTIAL, 1);
	//VMMDLL_ConfigSet(DMA_HANDLE, VMMDLL_OPT_REFRESH_FREQ_FAST, 1);
	// or
	//VMMDLL_ConfigSet(DMA_HANDLE, VMMDLL_OPT_REFRESH_ALL, 1);
}

bool DMAHandler::DumpMemoryMap() // used and updated
{
	LPCSTR args[8] = { "-device", "fpga", "-waitinitialize", "-norefresh", "", "", "", "" };
	int argc = 4;

	VMM_HANDLE handle = VMMDLL_Initialize(argc, args);
	if (!handle)
	{
		//log("[!] Failed to open a VMM Handle");
		return false;
	}

	PVMMDLL_MAP_PHYSMEM pPhysMemMap = nullptr;
	if (!VMMDLL_Map_GetPhysMem(handle, &pPhysMemMap))
	{
		//log("[!] Failed to get physical memory map");
		VMMDLL_Close(handle);
		return false;
	}

	if (pPhysMemMap->dwVersion != VMMDLL_MAP_PHYSMEM_VERSION)
	{
		//log("[!] Invalid VMM Map Version");
		VMMDLL_MemFree(pPhysMemMap);
		VMMDLL_Close(handle);
		return false;
	}

	if (pPhysMemMap->cMap == 0)
	{
		//log("[!] Failed to get physical memory map (empty map)");
		VMMDLL_MemFree(pPhysMemMap);
		VMMDLL_Close(handle);
		return false;
	}

	// Dump map to file in temp directory
	std::stringstream sb;
	for (DWORD i = 0; i < pPhysMemMap->cMap; i++)
	{
		sb << std::hex << pPhysMemMap->pMap[i].pa << " " << (pPhysMemMap->pMap[i].pa + pPhysMemMap->pMap[i].cb - 1) << std::endl;
	}

	auto temp_path = std::filesystem::temp_directory_path();
	std::ofstream nFile(temp_path.string() + "\\mmap.txt");
	nFile << sb.str();
	nFile.close();

	VMMDLL_MemFree(pPhysMemMap);
	//log("Successfully dumped memory map to file: %s\\mmap.txt", temp_path.string().c_str());
	// Little sleep to make sure it's written to file.
	Sleep(3000);
	VMMDLL_Close(handle);
	return true;
}


bool DMAHandler::IsInitialized() const // used not updated needed
{
	return DMA_HANDLE && PROCESS_INITIALIZED;
}

DWORD DMAHandler::GetPID() const // used not updated needed 
{
	//assertNoInit();
	return processInfo.pid;
}

ULONG64 DMAHandler::GetBaseAddress() // updated & used
{
	if (!processInfo.base) {
		VMMDLL_ConfigSet(DMA_HANDLE, VMMDLL_OPT_REFRESH_ALL, 1);

		// Use the processInfo.wname as the module name
		std::wstring module_name = processInfo.wname;
		PVMMDLL_MAP_MODULEENTRY module_info = nullptr;

		if (!VMMDLL_Map_GetModuleFromNameW(DMA_HANDLE, processInfo.pid, const_cast<LPWSTR>(module_name.c_str()), &module_info, VMMDLL_MODULE_FLAG_NORMAL)) {
			//log("[-] Couldn't find Base Address for %ws\n", module_name.c_str());
			return 0;
		}

		//log("[+] Found Base Address for %ws at 0x%p\n", module_name.c_str(), module_info->vaBase);
		processInfo.base = module_info->vaBase;
	}

	return processInfo.base;
}

bool DMAHandler::ReadBool(const ULONG64 address, const ULONG64 buffer, const SIZE_T size) const // not used but updated
{
	assertNoInit();
	DWORD dwBytesRead = 0;

#if COUNT_TOTAL_READSIZE
	readSize += size;
#endif

	bool status = VMMDLL_MemReadEx(DMA_HANDLE, processInfo.pid, address, reinterpret_cast<PBYTE>(buffer), size, &dwBytesRead, VMMDLL_FLAG_NOCACHE);

	if (dwBytesRead != size)
		//log("Didnt read all bytes requested! Only read %llu/%llu bytes!", dwBytesRead, size);
	return status;
}

void DMAHandler::Read(const ULONG64 address, const ULONG64 buffer, const SIZE_T size) // used and updated
{
	//assertNoInit();
	DWORD dwBytesRead = 0;

#if COUNT_TOTAL_READSIZE
	readSize += size;
#endif

	VMMDLL_MemReadEx(DMA_HANDLE, processInfo.pid, address, reinterpret_cast<PBYTE>(buffer), size, &dwBytesRead, VMMDLL_FLAG_NOCACHE);
	//if (dwBytesRead != size)
	//	log("Didnt read all bytes requested! Only read %llu/%llu bytes!", dwBytesRead, size);
}


bool DMAHandler::Write(const ULONG64 address, const ULONG64 buffer, const SIZE_T size) const // not used
{
	assertNoInit();
	return VMMDLL_MemWrite(DMA_HANDLE, processInfo.pid, address, reinterpret_cast<PBYTE>(buffer), size);
}

static const char* hexdigits = // used updated
"\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000"
"\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000"
"\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000"
"\000\001\002\003\004\005\006\007\010\011\000\000\000\000\000\000"
"\000\012\013\014\015\016\017\000\000\000\000\000\000\000\000\000"
"\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000"
"\000\012\013\014\015\016\017\000\000\000\000\000\000\000\000\000"
"\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000"
"\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000"
"\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000"
"\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000"
"\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000"
"\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000"
"\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000"
"\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000"
"\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000";

static uint8_t GetByte(const char* hex) // used updated
{
	return static_cast<uint8_t>((hexdigits[hex[0]] << 4) | (hexdigits[hex[1]]));
}

ULONG64 DMAHandler::PatternScan(const char* signature, uint64_t range_start, uint64_t range_end, int PID) // used updated
{
	if (!signature || signature[0] == '\0' || range_start >= range_end)
		return 0;

	if (PID == 0)
		PID = processInfo.pid;
	std::vector<uint8_t> buffer(range_end - range_start);
	if (!VMMDLL_MemReadEx(this->DMA_HANDLE, PID, range_start, buffer.data(), buffer.size(), 0, VMMDLL_FLAG_NOCACHE))
		return 0;

	const char* pat = signature;
	uint64_t first_match = 0;
	for (uint64_t i = range_start; i < range_end; i++)
	{
		if (*pat == '?' || buffer[i - range_start] == GetByte(pat))
		{
			if (!first_match)
				first_match = i;

			if (!pat[2])
				break;

			pat += (*pat == '?') ? 2 : 3;
		}
		else
		{
			pat = signature;
			first_match = 0;
		}
	}

	return first_match;
}

void DMAHandler::QueueScatterReadEx(VMMDLL_SCATTER_HANDLE handle, uint64_t addr, void* bffr, size_t size) const // used update not needed
{
	//assertNoInit();

	DWORD memoryPrepared = NULL;
	VMMDLL_Scatter_PrepareEx(handle, addr, size, static_cast<PBYTE>(bffr), &memoryPrepared); 
}

void DMAHandler::ExecuteScatterRead(VMMDLL_SCATTER_HANDLE handle) const // not used 
{
	//assertNoInit();

	VMMDLL_Scatter_ExecuteRead(handle);
	//Clear after using it
	VMMDLL_Scatter_Clear(handle, processInfo.pid, NULL);

	VMMDLL_Scatter_CloseHandle(handle);
}

void DMAHandler::QueueScatterWriteEx(VMMDLL_SCATTER_HANDLE handle, uint64_t addr, void* bffr, size_t size) const // not used
{
	assertNoInit();

	if (!VMMDLL_Scatter_PrepareWrite(handle, addr, static_cast<PBYTE>(bffr), size)) {
		log("failed to prepare scatter write at 0x%p\n", addr);
	}
}

void DMAHandler::ExecuteScatterWrite(VMMDLL_SCATTER_HANDLE handle) const // not used
{
	assertNoInit();

	if (!VMMDLL_Scatter_Execute(handle)) {
		log("failed to Execute Scatter write\n");
	}
	//Clear after using it
	if (!VMMDLL_Scatter_Clear(handle, processInfo.pid, NULL)) {
		log("failed to clear write Scatter\n");
	}
}

VMMDLL_SCATTER_HANDLE DMAHandler::CreateScatterHandle() const // not used 
{
	//assertNoInit();

	const VMMDLL_SCATTER_HANDLE ScatterHandle = VMMDLL_Scatter_Initialize(DMA_HANDLE, processInfo.pid, VMMDLL_FLAG_NOCACHE);
	//if (!ScatterHandle) log("failed to create scatter handle\n");
	return ScatterHandle;
}

void DMAHandler::CloseScatterHandle(VMMDLL_SCATTER_HANDLE& handle) const // not used
{
	//assertNoInit();

	VMMDLL_Scatter_CloseHandle(handle);

	handle = nullptr;
}

void DMAHandler::CloseDMA() // used on exit updated not needed
{
	//log("DMA closed!");
	DMA_HANDLE = nullptr;
	VMMDLL_Close(DMA_HANDLE);
}

#if COUNT_TOTAL_READSIZE

DWORD64 DMAHandler::GetTotalReadSize()
{
	return readSize;
}

void DMAHandler::ResetReadSize()
{
	log("Bytes read since last reset: %llu B, %llu KB, %llu MB", readSize, readSize / 1024, readSize / 1024 / 1024);
	readSize = 0;
}

#endif
