#include "stdafx.h"

const char* exportedNames[] = {
"CloseDriver",
"DefDriverProc",
"DriverCallback",
"DrvGetModuleHandle",
"GetDriverModuleHandle",
"NotifyCallbackData",
"OpenDriver",
"PlaySound",
"PlaySoundA",
"PlaySoundW",
"SendDriverMessage",
"WOW32DriverCallback",
"WOW32ResolveMultiMediaHandle",
"WOWAppExit",
"aux32Message",
"auxGetDevCapsA",
"auxGetDevCapsW",
"auxGetNumDevs",
"auxGetVolume",
"auxOutMessage",
"auxSetVolume",
"joy32Message",
"joyConfigChanged",
"joyGetDevCapsA",
"joyGetDevCapsW",
"joyGetNumDevs",
"joyGetPos",
"joyGetPosEx",
"joyGetThreshold",
"joyReleaseCapture",
"joySetCapture",
"joySetThreshold",
"mci32Message",
"mciDriverNotify",
"mciDriverYield",
"mciExecute",
"mciFreeCommandResource",
"mciGetCreatorTask",
"mciGetDeviceIDA",
"mciGetDeviceIDFromElementIDA",
"mciGetDeviceIDFromElementIDW",
"mciGetDeviceIDW",
"mciGetDriverData",
"mciGetErrorStringA",
"mciGetErrorStringW",
"mciGetYieldProc",
"mciLoadCommandResource",
"mciSendCommandA",
"mciSendCommandW",
"mciSendStringA",
"mciSendStringW",
"mciSetDriverData",
"mciSetYieldProc",
"mid32Message",
"midiConnect",
"midiDisconnect",
"midiInAddBuffer",
"midiInClose",
"midiInGetDevCapsA",
"midiInGetDevCapsW",
"midiInGetErrorTextA",
"midiInGetErrorTextW",
"midiInGetID",
"midiInGetNumDevs",
"midiInMessage",
"midiInOpen",
"midiInPrepareHeader",
"midiInReset",
"midiInStart",
"midiInStop",
"midiInUnprepareHeader",
"midiOutCacheDrumPatches",
"midiOutCachePatches",
"midiOutClose",
"midiOutGetDevCapsA",
"midiOutGetDevCapsW",
"midiOutGetErrorTextA",
"midiOutGetErrorTextW",
"midiOutGetID",
"midiOutGetNumDevs",
"midiOutGetVolume",
"midiOutLongMsg",
"midiOutMessage",
"midiOutOpen",
"midiOutPrepareHeader",
"midiOutReset",
"midiOutSetVolume",
"midiOutShortMsg",
"midiOutUnprepareHeader",
"midiStreamClose",
"midiStreamOpen",
"midiStreamOut",
"midiStreamPause",
"midiStreamPosition",
"midiStreamProperty",
"midiStreamRestart",
"midiStreamStop",
"mixerClose",
"mixerGetControlDetailsA",
"mixerGetControlDetailsW",
"mixerGetDevCapsA",
"mixerGetDevCapsW",
"mixerGetID",
"mixerGetLineControlsA",
"mixerGetLineControlsW",
"mixerGetLineInfoA",
"mixerGetLineInfoW",
"mixerGetNumDevs",
"mixerMessage",
"mixerOpen",
"mixerSetControlDetails",
"mmDrvInstall",
"mmGetCurrentTask",
"mmTaskBlock",
"mmTaskCreate",
"mmTaskSignal",
"mmTaskYield",
"mmioAdvance",
"mmioAscend",
"mmioClose",
"mmioCreateChunk",
"mmioDescend",
"mmioFlush",
"mmioGetInfo",
"mmioInstallIOProcA",
"mmioInstallIOProcW",
"mmioOpenA",
"mmioOpenW",
"mmioRead",
"mmioRenameA",
"mmioRenameW",
"mmioSeek",
"mmioSendMessage",
"mmioSetBuffer",
"mmioSetInfo",
"mmioStringToFOURCCA",
"mmioStringToFOURCCW",
"mmioWrite",
"mmsystemGetVersion",
"mod32Message",
"mxd32Message",
"sndPlaySoundA",
"sndPlaySoundW",
"tid32Message",
"timeBeginPeriod",
"timeEndPeriod",
"timeGetDevCaps",
"timeGetSystemTime",
"timeGetTime",
"timeKillEvent",
"timeSetEvent",
"waveInAddBuffer",
"waveInClose",
"waveInGetDevCapsA",
"waveInGetDevCapsW",
"waveInGetErrorTextA",
"waveInGetErrorTextW",
"waveInGetID",
"waveInGetNumDevs",
"waveInGetPosition",
"waveInMessage",
"waveInOpen",
"waveInPrepareHeader",
"waveInReset",
"waveInStart",
"waveInStop",
"waveInUnprepareHeader",
"waveOutBreakLoop",
"waveOutClose",
"waveOutGetDevCapsA",
"waveOutGetDevCapsW",
"waveOutGetErrorTextA",
"waveOutGetErrorTextW",
"waveOutGetID",
"waveOutGetNumDevs",
"waveOutGetPitch",
"waveOutGetPlaybackRate",
"waveOutGetPosition",
"waveOutGetVolume",
"waveOutMessage",
"waveOutOpen",
"waveOutPause",
"waveOutPrepareHeader",
"waveOutReset",
"waveOutRestart",
"waveOutSetPitch",
"waveOutSetPlaybackRate",
"waveOutSetVolume",
"waveOutUnprepareHeader",
"waveOutWrite",
"wid32Message",
"wod32Message",
};

constexpr auto exportsCount = sizeof(exportedNames) / 4;

FARPROC addresses[exportsCount];

BOOL WINAPI hkFlashWindowEx(PFLASHWINFO pfwi)
{
	return 1;
}

BOOL WINAPI DllMain(HINSTANCE hInst, DWORD reason, LPVOID)
{
	static HINSTANCE originalDll;

	if (reason == DLL_PROCESS_ATTACH)
	{
		std::wstring systemDir;
		systemDir.resize(MAX_PATH);

		if (!GetSystemDirectory(const_cast<wchar_t*>(systemDir.data()), MAX_PATH))
			return FALSE;

		systemDir.resize(systemDir.find_first_of(L'\0'));

		originalDll = LoadLibrary((systemDir + L"\\winmm.dll").c_str());
		if (!originalDll) return false;

		for (unsigned int i = 0; i < exportsCount; ++i)
		{
			addresses[i] = GetProcAddress(originalDll, exportedNames[i]);
		}

		HOOK_TRACE_INFO hHook = { NULL };

		NTSTATUS result = LhInstallHook(
			GetProcAddress(GetModuleHandle(L"user32"), "FlashWindowEx"),
			hkFlashWindowEx,
			NULL,
			&hHook);

		ULONG ACLEntries[1] = { 0 };
		LhSetInclusiveACL(ACLEntries, 1, &hHook);
	}

	if (reason == DLL_PROCESS_DETACH)
		FreeLibrary(originalDll);
	return TRUE;
}

_declspec(naked) void _CloseDriver()
{
	__asm jmp dword ptr[addresses + 0 * 4];
}

_declspec(naked) void _DefDriverProc()
{
	__asm jmp dword ptr[addresses + 1 * 4];
}

_declspec(naked) void _DriverCallback()
{
	__asm jmp dword ptr[addresses + 2 * 4];
}

_declspec(naked) void _DrvGetModuleHandle()
{
	__asm jmp dword ptr[addresses + 3 * 4];
}

_declspec(naked) void _GetDriverModuleHandle()
{
	__asm jmp dword ptr[addresses + 4 * 4];
}

_declspec(naked) void _NotifyCallbackData()
{
	__asm jmp dword ptr[addresses + 5 * 4];
}

_declspec(naked) void _OpenDriver()
{
	__asm jmp dword ptr[addresses + 6 * 4];
}

_declspec(naked) void _PlaySound()
{
	__asm jmp dword ptr[addresses + 7 * 4];
}

_declspec(naked) void _PlaySoundA()
{
	__asm jmp dword ptr[addresses + 8 * 4];
}

_declspec(naked) void _PlaySoundW()
{
	__asm jmp dword ptr[addresses + 9 * 4];
}

_declspec(naked) void _SendDriverMessage()
{
	__asm jmp dword ptr[addresses + 10 * 4];
}

_declspec(naked) void _WOW32DriverCallback()
{
	__asm jmp dword ptr[addresses + 11 * 4];
}

_declspec(naked) void _WOW32ResolveMultiMediaHandle()
{
	__asm jmp dword ptr[addresses + 12 * 4];
}

_declspec(naked) void _WOWAppExit()
{
	__asm jmp dword ptr[addresses + 13 * 4];
}

_declspec(naked) void _aux32Message()
{
	__asm jmp dword ptr[addresses + 14 * 4];
}

_declspec(naked) void _auxGetDevCapsA()
{
	__asm jmp dword ptr[addresses + 15 * 4];
}

_declspec(naked) void _auxGetDevCapsW()
{
	__asm jmp dword ptr[addresses + 16 * 4];
}

_declspec(naked) void _auxGetNumDevs()
{
	__asm jmp dword ptr[addresses + 17 * 4];
}

_declspec(naked) void _auxGetVolume()
{
	__asm jmp dword ptr[addresses + 18 * 4];
}

_declspec(naked) void _auxOutMessage()
{
	__asm jmp dword ptr[addresses + 19 * 4];
}

_declspec(naked) void _auxSetVolume()
{
	__asm jmp dword ptr[addresses + 20 * 4];
}

_declspec(naked) void _joy32Message()
{
	__asm jmp dword ptr[addresses + 21 * 4];
}

_declspec(naked) void _joyConfigChanged()
{
	__asm jmp dword ptr[addresses + 22 * 4];
}

_declspec(naked) void _joyGetDevCapsA()
{
	__asm jmp dword ptr[addresses + 23 * 4];
}

_declspec(naked) void _joyGetDevCapsW()
{
	__asm jmp dword ptr[addresses + 24 * 4];
}

_declspec(naked) void _joyGetNumDevs()
{
	__asm jmp dword ptr[addresses + 25 * 4];
}

_declspec(naked) void _joyGetPos()
{
	__asm jmp dword ptr[addresses + 26 * 4];
}

_declspec(naked) void _joyGetPosEx()
{
	__asm jmp dword ptr[addresses + 27 * 4];
}

_declspec(naked) void _joyGetThreshold()
{
	__asm jmp dword ptr[addresses + 28 * 4];
}

_declspec(naked) void _joyReleaseCapture()
{
	__asm jmp dword ptr[addresses + 29 * 4];
}

_declspec(naked) void _joySetCapture()
{
	__asm jmp dword ptr[addresses + 30 * 4];
}

_declspec(naked) void _joySetThreshold()
{
	__asm jmp dword ptr[addresses + 31 * 4];
}

_declspec(naked) void _mci32Message()
{
	__asm jmp dword ptr[addresses + 32 * 4];
}

_declspec(naked) void _mciDriverNotify()
{
	__asm jmp dword ptr[addresses + 33 * 4];
}

_declspec(naked) void _mciDriverYield()
{
	__asm jmp dword ptr[addresses + 34 * 4];
}

_declspec(naked) void _mciExecute()
{
	__asm jmp dword ptr[addresses + 35 * 4];
}

_declspec(naked) void _mciFreeCommandResource()
{
	__asm jmp dword ptr[addresses + 36 * 4];
}

_declspec(naked) void _mciGetCreatorTask()
{
	__asm jmp dword ptr[addresses + 37 * 4];
}

_declspec(naked) void _mciGetDeviceIDA()
{
	__asm jmp dword ptr[addresses + 38 * 4];
}

_declspec(naked) void _mciGetDeviceIDFromElementIDA()
{
	__asm jmp dword ptr[addresses + 39 * 4];
}

_declspec(naked) void _mciGetDeviceIDFromElementIDW()
{
	__asm jmp dword ptr[addresses + 40 * 4];
}

_declspec(naked) void _mciGetDeviceIDW()
{
	__asm jmp dword ptr[addresses + 41 * 4];
}

_declspec(naked) void _mciGetDriverData()
{
	__asm jmp dword ptr[addresses + 42 * 4];
}

_declspec(naked) void _mciGetErrorStringA()
{
	__asm jmp dword ptr[addresses + 43 * 4];
}

_declspec(naked) void _mciGetErrorStringW()
{
	__asm jmp dword ptr[addresses + 44 * 4];
}

_declspec(naked) void _mciGetYieldProc()
{
	__asm jmp dword ptr[addresses + 45 * 4];
}

_declspec(naked) void _mciLoadCommandResource()
{
	__asm jmp dword ptr[addresses + 46 * 4];
}

_declspec(naked) void _mciSendCommandA()
{
	__asm jmp dword ptr[addresses + 47 * 4];
}

_declspec(naked) void _mciSendCommandW()
{
	__asm jmp dword ptr[addresses + 48 * 4];
}

_declspec(naked) void _mciSendStringA()
{
	__asm jmp dword ptr[addresses + 49 * 4];
}

_declspec(naked) void _mciSendStringW()
{
	__asm jmp dword ptr[addresses + 50 * 4];
}

_declspec(naked) void _mciSetDriverData()
{
	__asm jmp dword ptr[addresses + 51 * 4];
}

_declspec(naked) void _mciSetYieldProc()
{
	__asm jmp dword ptr[addresses + 52 * 4];
}

_declspec(naked) void _mid32Message()
{
	__asm jmp dword ptr[addresses + 53 * 4];
}

_declspec(naked) void _midiConnect()
{
	__asm jmp dword ptr[addresses + 54 * 4];
}

_declspec(naked) void _midiDisconnect()
{
	__asm jmp dword ptr[addresses + 55 * 4];
}

_declspec(naked) void _midiInAddBuffer()
{
	__asm jmp dword ptr[addresses + 56 * 4];
}

_declspec(naked) void _midiInClose()
{
	__asm jmp dword ptr[addresses + 57 * 4];
}

_declspec(naked) void _midiInGetDevCapsA()
{
	__asm jmp dword ptr[addresses + 58 * 4];
}

_declspec(naked) void _midiInGetDevCapsW()
{
	__asm jmp dword ptr[addresses + 59 * 4];
}

_declspec(naked) void _midiInGetErrorTextA()
{
	__asm jmp dword ptr[addresses + 60 * 4];
}

_declspec(naked) void _midiInGetErrorTextW()
{
	__asm jmp dword ptr[addresses + 61 * 4];
}

_declspec(naked) void _midiInGetID()
{
	__asm jmp dword ptr[addresses + 62 * 4];
}

_declspec(naked) void _midiInGetNumDevs()
{
	__asm jmp dword ptr[addresses + 63 * 4];
}

_declspec(naked) void _midiInMessage()
{
	__asm jmp dword ptr[addresses + 64 * 4];
}

_declspec(naked) void _midiInOpen()
{
	__asm jmp dword ptr[addresses + 65 * 4];
}

_declspec(naked) void _midiInPrepareHeader()
{
	__asm jmp dword ptr[addresses + 66 * 4];
}

_declspec(naked) void _midiInReset()
{
	__asm jmp dword ptr[addresses + 67 * 4];
}

_declspec(naked) void _midiInStart()
{
	__asm jmp dword ptr[addresses + 68 * 4];
}

_declspec(naked) void _midiInStop()
{
	__asm jmp dword ptr[addresses + 69 * 4];
}

_declspec(naked) void _midiInUnprepareHeader()
{
	__asm jmp dword ptr[addresses + 70 * 4];
}

_declspec(naked) void _midiOutCacheDrumPatches()
{
	__asm jmp dword ptr[addresses + 71 * 4];
}

_declspec(naked) void _midiOutCachePatches()
{
	__asm jmp dword ptr[addresses + 72 * 4];
}

_declspec(naked) void _midiOutClose()
{
	__asm jmp dword ptr[addresses + 73 * 4];
}

_declspec(naked) void _midiOutGetDevCapsA()
{
	__asm jmp dword ptr[addresses + 74 * 4];
}

_declspec(naked) void _midiOutGetDevCapsW()
{
	__asm jmp dword ptr[addresses + 75 * 4];
}

_declspec(naked) void _midiOutGetErrorTextA()
{
	__asm jmp dword ptr[addresses + 76 * 4];
}

_declspec(naked) void _midiOutGetErrorTextW()
{
	__asm jmp dword ptr[addresses + 77 * 4];
}

_declspec(naked) void _midiOutGetID()
{
	__asm jmp dword ptr[addresses + 78 * 4];
}

_declspec(naked) void _midiOutGetNumDevs()
{
	__asm jmp dword ptr[addresses + 79 * 4];
}

_declspec(naked) void _midiOutGetVolume()
{
	__asm jmp dword ptr[addresses + 80 * 4];
}

_declspec(naked) void _midiOutLongMsg()
{
	__asm jmp dword ptr[addresses + 81 * 4];
}

_declspec(naked) void _midiOutMessage()
{
	__asm jmp dword ptr[addresses + 82 * 4];
}

_declspec(naked) void _midiOutOpen()
{
	__asm jmp dword ptr[addresses + 83 * 4];
}

_declspec(naked) void _midiOutPrepareHeader()
{
	__asm jmp dword ptr[addresses + 84 * 4];
}

_declspec(naked) void _midiOutReset()
{
	__asm jmp dword ptr[addresses + 85 * 4];
}

_declspec(naked) void _midiOutSetVolume()
{
	__asm jmp dword ptr[addresses + 86 * 4];
}

_declspec(naked) void _midiOutShortMsg()
{
	__asm jmp dword ptr[addresses + 87 * 4];
}

_declspec(naked) void _midiOutUnprepareHeader()
{
	__asm jmp dword ptr[addresses + 88 * 4];
}

_declspec(naked) void _midiStreamClose()
{
	__asm jmp dword ptr[addresses + 89 * 4];
}

_declspec(naked) void _midiStreamOpen()
{
	__asm jmp dword ptr[addresses + 90 * 4];
}

_declspec(naked) void _midiStreamOut()
{
	__asm jmp dword ptr[addresses + 91 * 4];
}

_declspec(naked) void _midiStreamPause()
{
	__asm jmp dword ptr[addresses + 92 * 4];
}

_declspec(naked) void _midiStreamPosition()
{
	__asm jmp dword ptr[addresses + 93 * 4];
}

_declspec(naked) void _midiStreamProperty()
{
	__asm jmp dword ptr[addresses + 94 * 4];
}

_declspec(naked) void _midiStreamRestart()
{
	__asm jmp dword ptr[addresses + 95 * 4];
}

_declspec(naked) void _midiStreamStop()
{
	__asm jmp dword ptr[addresses + 96 * 4];
}

_declspec(naked) void _mixerClose()
{
	__asm jmp dword ptr[addresses + 97 * 4];
}

_declspec(naked) void _mixerGetControlDetailsA()
{
	__asm jmp dword ptr[addresses + 98 * 4];
}

_declspec(naked) void _mixerGetControlDetailsW()
{
	__asm jmp dword ptr[addresses + 99 * 4];
}

_declspec(naked) void _mixerGetDevCapsA()
{
	__asm jmp dword ptr[addresses + 100 * 4];
}

_declspec(naked) void _mixerGetDevCapsW()
{
	__asm jmp dword ptr[addresses + 101 * 4];
}

_declspec(naked) void _mixerGetID()
{
	__asm jmp dword ptr[addresses + 102 * 4];
}

_declspec(naked) void _mixerGetLineControlsA()
{
	__asm jmp dword ptr[addresses + 103 * 4];
}

_declspec(naked) void _mixerGetLineControlsW()
{
	__asm jmp dword ptr[addresses + 104 * 4];
}

_declspec(naked) void _mixerGetLineInfoA()
{
	__asm jmp dword ptr[addresses + 105 * 4];
}

_declspec(naked) void _mixerGetLineInfoW()
{
	__asm jmp dword ptr[addresses + 106 * 4];
}

_declspec(naked) void _mixerGetNumDevs()
{
	__asm jmp dword ptr[addresses + 107 * 4];
}

_declspec(naked) void _mixerMessage()
{
	__asm jmp dword ptr[addresses + 108 * 4];
}

_declspec(naked) void _mixerOpen()
{
	__asm jmp dword ptr[addresses + 109 * 4];
}

_declspec(naked) void _mixerSetControlDetails()
{
	__asm jmp dword ptr[addresses + 110 * 4];
}

_declspec(naked) void _mmDrvInstall()
{
	__asm jmp dword ptr[addresses + 111 * 4];
}

_declspec(naked) void _mmGetCurrentTask()
{
	__asm jmp dword ptr[addresses + 112 * 4];
}

_declspec(naked) void _mmTaskBlock()
{
	__asm jmp dword ptr[addresses + 113 * 4];
}

_declspec(naked) void _mmTaskCreate()
{
	__asm jmp dword ptr[addresses + 114 * 4];
}

_declspec(naked) void _mmTaskSignal()
{
	__asm jmp dword ptr[addresses + 115 * 4];
}

_declspec(naked) void _mmTaskYield()
{
	__asm jmp dword ptr[addresses + 116 * 4];
}

_declspec(naked) void _mmioAdvance()
{
	__asm jmp dword ptr[addresses + 117 * 4];
}

_declspec(naked) void _mmioAscend()
{
	__asm jmp dword ptr[addresses + 118 * 4];
}

_declspec(naked) void _mmioClose()
{
	__asm jmp dword ptr[addresses + 119 * 4];
}

_declspec(naked) void _mmioCreateChunk()
{
	__asm jmp dword ptr[addresses + 120 * 4];
}

_declspec(naked) void _mmioDescend()
{
	__asm jmp dword ptr[addresses + 121 * 4];
}

_declspec(naked) void _mmioFlush()
{
	__asm jmp dword ptr[addresses + 122 * 4];
}

_declspec(naked) void _mmioGetInfo()
{
	__asm jmp dword ptr[addresses + 123 * 4];
}

_declspec(naked) void _mmioInstallIOProcA()
{
	__asm jmp dword ptr[addresses + 124 * 4];
}

_declspec(naked) void _mmioInstallIOProcW()
{
	__asm jmp dword ptr[addresses + 125 * 4];
}

_declspec(naked) void _mmioOpenA()
{
	__asm jmp dword ptr[addresses + 126 * 4];
}

_declspec(naked) void _mmioOpenW()
{
	__asm jmp dword ptr[addresses + 127 * 4];
}

_declspec(naked) void _mmioRead()
{
	__asm jmp dword ptr[addresses + 128 * 4];
}

_declspec(naked) void _mmioRenameA()
{
	__asm jmp dword ptr[addresses + 129 * 4];
}

_declspec(naked) void _mmioRenameW()
{
	__asm jmp dword ptr[addresses + 130 * 4];
}

_declspec(naked) void _mmioSeek()
{
	__asm jmp dword ptr[addresses + 131 * 4];
}

_declspec(naked) void _mmioSendMessage()
{
	__asm jmp dword ptr[addresses + 132 * 4];
}

_declspec(naked) void _mmioSetBuffer()
{
	__asm jmp dword ptr[addresses + 133 * 4];
}

_declspec(naked) void _mmioSetInfo()
{
	__asm jmp dword ptr[addresses + 134 * 4];
}

_declspec(naked) void _mmioStringToFOURCCA()
{
	__asm jmp dword ptr[addresses + 135 * 4];
}

_declspec(naked) void _mmioStringToFOURCCW()
{
	__asm jmp dword ptr[addresses + 136 * 4];
}

_declspec(naked) void _mmioWrite()
{
	__asm jmp dword ptr[addresses + 137 * 4];
}

_declspec(naked) void _mmsystemGetVersion()
{
	__asm jmp dword ptr[addresses + 138 * 4];
}

_declspec(naked) void _mod32Message()
{
	__asm jmp dword ptr[addresses + 139 * 4];
}

_declspec(naked) void _mxd32Message()
{
	__asm jmp dword ptr[addresses + 140 * 4];
}

_declspec(naked) void _sndPlaySoundA()
{
	__asm jmp dword ptr[addresses + 141 * 4];
}

_declspec(naked) void _sndPlaySoundW()
{
	__asm jmp dword ptr[addresses + 142 * 4];
}

_declspec(naked) void _tid32Message()
{
	__asm jmp dword ptr[addresses + 143 * 4];
}

_declspec(naked) void _timeBeginPeriod()
{
	__asm jmp dword ptr[addresses + 144 * 4];
}

_declspec(naked) void _timeEndPeriod()
{
	__asm jmp dword ptr[addresses + 145 * 4];
}

_declspec(naked) void _timeGetDevCaps()
{
	__asm jmp dword ptr[addresses + 146 * 4];
}

_declspec(naked) void _timeGetSystemTime()
{
	__asm jmp dword ptr[addresses + 147 * 4];
}

_declspec(naked) void _timeGetTime()
{
	__asm jmp dword ptr[addresses + 148 * 4];
}

_declspec(naked) void _timeKillEvent()
{
	__asm jmp dword ptr[addresses + 149 * 4];
}

_declspec(naked) void _timeSetEvent()
{
	__asm jmp dword ptr[addresses + 150 * 4];
}

_declspec(naked) void _waveInAddBuffer()
{
	__asm jmp dword ptr[addresses + 151 * 4];
}

_declspec(naked) void _waveInClose()
{
	__asm jmp dword ptr[addresses + 152 * 4];
}

_declspec(naked) void _waveInGetDevCapsA()
{
	__asm jmp dword ptr[addresses + 153 * 4];
}

_declspec(naked) void _waveInGetDevCapsW()
{
	__asm jmp dword ptr[addresses + 154 * 4];
}

_declspec(naked) void _waveInGetErrorTextA()
{
	__asm jmp dword ptr[addresses + 155 * 4];
}

_declspec(naked) void _waveInGetErrorTextW()
{
	__asm jmp dword ptr[addresses + 156 * 4];
}

_declspec(naked) void _waveInGetID()
{
	__asm jmp dword ptr[addresses + 157 * 4];
}

_declspec(naked) void _waveInGetNumDevs()
{
	__asm jmp dword ptr[addresses + 158 * 4];
}

_declspec(naked) void _waveInGetPosition()
{
	__asm jmp dword ptr[addresses + 159 * 4];
}

_declspec(naked) void _waveInMessage()
{
	__asm jmp dword ptr[addresses + 160 * 4];
}

_declspec(naked) void _waveInOpen()
{
	__asm jmp dword ptr[addresses + 161 * 4];
}

_declspec(naked) void _waveInPrepareHeader()
{
	__asm jmp dword ptr[addresses + 162 * 4];
}

_declspec(naked) void _waveInReset()
{
	__asm jmp dword ptr[addresses + 163 * 4];
}

_declspec(naked) void _waveInStart()
{
	__asm jmp dword ptr[addresses + 164 * 4];
}

_declspec(naked) void _waveInStop()
{
	__asm jmp dword ptr[addresses + 165 * 4];
}

_declspec(naked) void _waveInUnprepareHeader()
{
	__asm jmp dword ptr[addresses + 166 * 4];
}

_declspec(naked) void _waveOutBreakLoop()
{
	__asm jmp dword ptr[addresses + 167 * 4];
}

_declspec(naked) void _waveOutClose()
{
	__asm jmp dword ptr[addresses + 168 * 4];
}

_declspec(naked) void _waveOutGetDevCapsA()
{
	__asm jmp dword ptr[addresses + 169 * 4];
}

_declspec(naked) void _waveOutGetDevCapsW()
{
	__asm jmp dword ptr[addresses + 170 * 4];
}

_declspec(naked) void _waveOutGetErrorTextA()
{
	__asm jmp dword ptr[addresses + 171 * 4];
}

_declspec(naked) void _waveOutGetErrorTextW()
{
	__asm jmp dword ptr[addresses + 172 * 4];
}

_declspec(naked) void _waveOutGetID()
{
	__asm jmp dword ptr[addresses + 173 * 4];
}

_declspec(naked) void _waveOutGetNumDevs()
{
	__asm jmp dword ptr[addresses + 174 * 4];
}

_declspec(naked) void _waveOutGetPitch()
{
	__asm jmp dword ptr[addresses + 175 * 4];
}

_declspec(naked) void _waveOutGetPlaybackRate()
{
	__asm jmp dword ptr[addresses + 176 * 4];
}

_declspec(naked) void _waveOutGetPosition()
{
	__asm jmp dword ptr[addresses + 177 * 4];
}

_declspec(naked) void _waveOutGetVolume()
{
	__asm jmp dword ptr[addresses + 178 * 4];
}

_declspec(naked) void _waveOutMessage()
{
	__asm jmp dword ptr[addresses + 179 * 4];
}

_declspec(naked) void _waveOutOpen()
{
	__asm jmp dword ptr[addresses + 180 * 4];
}

_declspec(naked) void _waveOutPause()
{
	__asm jmp dword ptr[addresses + 181 * 4];
}

_declspec(naked) void _waveOutPrepareHeader()
{
	__asm jmp dword ptr[addresses + 182 * 4];
}

_declspec(naked) void _waveOutReset()
{
	__asm jmp dword ptr[addresses + 183 * 4];
}

_declspec(naked) void _waveOutRestart()
{
	__asm jmp dword ptr[addresses + 184 * 4];
}

_declspec(naked) void _waveOutSetPitch()
{
	__asm jmp dword ptr[addresses + 185 * 4];
}

_declspec(naked) void _waveOutSetPlaybackRate()
{
	__asm jmp dword ptr[addresses + 186 * 4];
}

_declspec(naked) void _waveOutSetVolume()
{
	__asm jmp dword ptr[addresses + 187 * 4];
}

_declspec(naked) void _waveOutUnprepareHeader()
{
	__asm jmp dword ptr[addresses + 188 * 4];
}

_declspec(naked) void _waveOutWrite()
{
	__asm jmp dword ptr[addresses + 189 * 4];
}

_declspec(naked) void _wid32Message()
{
	__asm jmp dword ptr[addresses + 190 * 4];
}

_declspec(naked) void _wod32Message()
{
	__asm jmp dword ptr[addresses + 191 * 4];
}