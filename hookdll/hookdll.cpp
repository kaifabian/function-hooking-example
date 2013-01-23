// hookdll.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"

#include <Windows.h>
#include <stdio.h>

int (*origVeryImportantFunction)(int, int) = NULL;

__declspec(dllexport)
int VeryImportantFunction(int a, int b) {
	if(!origVeryImportantFunction) return 0;
	return origVeryImportantFunction(a, b);
}

BOOL WINAPI MyGetExitCodeProcess(HANDLE hProcess, LPDWORD lpExitCode) {
	*lpExitCode = 0;
	return true;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
	if(ul_reason_for_call == DLL_PROCESS_ATTACH)
	{
		printf("Our Mighty Hook DLL(TM) has been loaded!\r\n");

		HMODULE origLibrary = LoadLibraryA("important-orig");

		if(origLibrary) {
			printf("Loaded Library into %p\r\n", origLibrary);
			origVeryImportantFunction = (int (*)(int, int)) GetProcAddress(origLibrary, "?VeryImportantFunction@@YAHHH@Z");
			printf("Original function resides at %p\r\n", origVeryImportantFunction);
		} else {
			printf("Could not load original function");
		}

		/* Now the EEEEEVIL part begins. */

		HMODULE kernel32dll = GetModuleHandleA("kernel32");
		FARPROC getExitCodeProcessFunc = GetProcAddress(kernel32dll, "GetExitCodeProcess");
		char *getExitCodeProcessAsm = ((char *) getExitCodeProcessFunc) - 5;
		char *getExitCodeProcessOrig = ((char *) getExitCodeProcessFunc);

		printf("The CreateProcessA stub from kernel32.dll resides at: %p\r\n", getExitCodeProcessFunc);

		DWORD dwback;

		// We need to make our part of the kernel32.dll writeable //
		VirtualProtect(getExitCodeProcessAsm, 7, PAGE_READWRITE, &dwback);

		// 0xE9 is "long jump", the address is calculated below (the difference between our hook function and the "original" one //
		char longJumpToHookFunction[5] = {0xE9, 0x00, 0x00, 0x00, 0x00};
		*(DWORD *) (longJumpToHookFunction + 1) = (DWORD) (((char *) &MyGetExitCodeProcess) - getExitCodeProcessOrig);

		for(int i = 0; i < 5; i++)
			getExitCodeProcessAsm[i] = longJumpToHookFunction[i];

		// 0xEB is "short jump", 0xF9 is one's complement for -7
		char shortJump7BytesBackwards[2] = {0xEB, 0xF9};
		
		for(int i = 0; i < 2; i++)
			getExitCodeProcessOrig[i] = shortJump7BytesBackwards[i];
		
		// Now we restore the page protection //
		VirtualProtect(getExitCodeProcessAsm, 7, dwback, &dwback);
	}
	return TRUE;
}

