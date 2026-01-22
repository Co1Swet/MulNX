#include "DllMain.hpp"

#include "../../../MulNX/MulNX.hpp"

#include <Windows.h>

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
	switch (ul_reason_for_call) {
	case DLL_PROCESS_ATTACH: {
		HANDLE hThread = CreateThread(NULL, 0, Main, NULL, 0, NULL);
		break;
	}
	case DLL_THREAD_ATTACH: {
		break;
	}
	case DLL_THREAD_DETACH: {
		break;
	}
	case DLL_PROCESS_DETACH: {
		break;
	}
	default: {
		break;
	}
	}
	return TRUE;
}

DWORD Main(void*) {
	MulNX::Core::Core& core = MulNX::Core::Core::GetInstance();

	core.Init();

	return 0;
}