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
	GetCurrentDirectory(MAX_PATH, const_cast<wchar_t*>(currentDir.data()));
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

	//	.text : 00D4458D FF 15 70 77 C1 01	call    ds : GetForegroundWindow
	//	.text : 00D44593 3B C6				cmp     eax, esi
	//	.text : 00D44595 74 3B				jz      short loc_D445D2

	const auto target = SignatureScan(map, size, "FF 15 ? ? ? ? 3B C6 74 ?");
	if (!target)
	{
		std::wcout << "[ERROR] Can't patch Telegram executable. Maybe you've already patched it or this tool doesn't support current Telegram version." << std::endl;
		system("pause");
		return 1;
	}

	// replace jz with jmp
	*reinterpret_cast<uint8_t*>(target + 0x8) = 0xEB;

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