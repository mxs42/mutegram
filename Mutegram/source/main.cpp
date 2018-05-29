#include "stdafx.h"
#include "SignatureScan.h"

#define ERROR_CHECK(condition, description) \
	if(!(condition)) \
	{ \
		std::wcout << "[ERROR] " << description << ": " << GetLastError() << std::endl; \
		system("pause"); \
		return 1; \
	}

int main()
{
	std::wstring currentDir;
	currentDir.resize(MAX_PATH);
	GetCurrentDirectory(MAX_PATH, const_cast<wchar_t*>(currentDir.data()));;
	currentDir.resize(currentDir.find_first_of(L'\0'));
	std::wcout << "Current directory: " << currentDir << std::endl;

	std::wstring tgPath = currentDir + L"\\Telegram.exe";
	std::wstring tgBackupPath = currentDir + L"\\Telegram.backup.exe";

	const auto file = CreateFile(tgPath.c_str(), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, nullptr, OPEN_EXISTING, 0, nullptr);
	if (file == INVALID_HANDLE_VALUE)
	{
		const auto error = GetLastError();
		if (error == ERROR_FILE_NOT_FOUND)
		{
			std::wcout << "[ERROR] Can not find Telegram.exe at path " << tgPath << ".\nDid you place this tool into correct directory?.." << std::endl;
		}
		else if (error == ERROR_SHARING_VIOLATION)
		{
			std::wcout << "[ERROR] Close Telegram before using this tool." << std::endl;
		}
		else
		{
			std::wcout << "[ERROR] " << GetLastError() << std::endl;
		}

		system("pause");
		return 1;
	}

	auto hr = CopyFile(tgPath.c_str(), tgBackupPath.c_str(), false);
	ERROR_CHECK(SUCCEEDED(hr), "Can not create backup");
	std::wcout << "[OK] Backup created." << std::endl;

	const auto size = GetFileSize(file, nullptr);
	ERROR_CHECK(size != INVALID_FILE_SIZE, "GetFileSize");

	const auto fileMapping = CreateFileMappingA(file, nullptr, PAGE_READWRITE, 0, 0, nullptr);
	ERROR_CHECK(fileMapping, "CreateFileMappingA");

	auto map = MapViewOfFile(fileMapping, FILE_MAP_ALL_ACCESS, 0, 0, 0);
	ERROR_CHECK(map, "MapViewOfFile");

	// ------------------------------
	// PATCH BEGIN
	// ------------------------------

	// Telegram checks that its window is a foreground window. 
	// If this is TRUE then it doesn't call FlashWindowEx flash. 
	// So we have to force this check to be always TRUE.
	// Not sure about signature correctness for future updates though.

	//	8B 3D 00 E4 15 03                                   mov     edi, dword_315E400
	//	85 FF                                               test    edi, edi
	//	74 4F                                               jz      short loc_CF0948 <- patch this to jmp
	//	8B BF 94 00 00 00                                   mov     edi, [edi + 94h]
	//	85 FF                                               test    edi, edi
	//	74 45                                               jz      short loc_CF0948
	//	8B B7 34 01 00 00                                   mov     esi, [edi + 134h]
	//	FF 15 78 57 91 01                                   call    ds : GetForegroundWindow
	//	3B C6                                               cmp     eax, esi
	//	74 35                                               jz      short loc_CF0948
	//	; ----------------------------------------------
	//	C7 85 7C FF FF FF 14 00 00 00                       mov     dword ptr[ebp - 84h], 14h
	//	8B 87 34 01 00 00                                   mov     eax, [edi + 134h]
	//	89 45 80                                            mov[ebp - 80h], eax
	//	8D 85 7C FF FF FF                                   lea     eax, [ebp - 84h]
	//	50                                                  push    eax; pfwi
	//	C7 45 84 03 00 00 00                                mov     dword ptr[ebp - 7Ch], 3
	//	C7 45 8C 00 00 00 00                                mov     dword ptr[ebp - 74h], 0
	//	C7 45 88 01 00 00 00                                mov     dword ptr[ebp - 78h], 1
	//	FF 15 74 57 91 01                                   call    ds : FlashWindowEx

	const auto target = SignatureScan(map, size, "74 ? 8B BF ? ? ? ? 85 FF 74 ? 8B B7 ? ? ? ? FF 15 ? ? ? ?");
	if (!target)
	{
		std::wcout << "[ERROR] Can't patch Telegram executable. Maybe you've already patched it or this tool doesn't support current Telegram version." << std::endl;
		system("pause");
		return 1;
	}

	// replace jz with jmp
	*reinterpret_cast<uint8_t*>(target) = 0xEB;

	// ------------------------------
	// PATCH END
	// ------------------------------
	hr = UnmapViewOfFile(map);
	ERROR_CHECK(SUCCEEDED(hr), "UnmapViewOfFile");

	hr = CloseHandle(fileMapping);
	ERROR_CHECK(SUCCEEDED(hr), "CloseHandle(fileMapping)");

	hr = CloseHandle(file);
	ERROR_CHECK(SUCCEEDED(hr), "CloseHandle(file)");
	
	std::wcout << "[OK] Patch successfully applied at 0x" << std::hex << std::uppercase << target << " (offset 0x" << std::hex << std::uppercase << (target - reinterpret_cast<DWORD>(map)) << ")." << std::endl;
	std::wcout << "In case of errors delete Telegram.exe, rename Telegram.backup.exe to Telegram.exe, then you can try again." << std::endl;
	std::wcout << "\nSupport: https://github.com/mxs42/mutegram" << std::endl;
	system("pause");

	return 0;
}