// TotallyUnRelatedAcademicExample.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <Windows.h>
#include <stdio.h>
#include <tchar.h>
#include <conio.h>

#include "drm_program.h"

#define DRM_PROGRAM_NAME "drm-check.exe"

__declspec(dllimport)
int VeryImportantFunction(int a, int b);

void prepareDRMCheck(void) {
	FILE _fp;
	FILE *fp = &_fp;

	int error;

	if(error = fopen_s(&fp, DRM_PROGRAM_NAME, "wb")) {
		printf("FAIL: %08X!\r\n", error);
		return;
	}

	fwrite(_drm_check_content, sizeof(_drm_check_content[0]), sizeof(_drm_check_content)/sizeof(_drm_check_content[0])-1, fp);
	fclose(fp);
}

DWORD spawnDRMProcessAndGetExitCode(void) {
	STARTUPINFOA si;
	PROCESS_INFORMATION pi;

	LPSTR lpstrProgramName = (LPSTR) DRM_PROGRAM_NAME;

	DWORD dwCreateProcessResult;
	DWORD dwExitCode = 0;
	
	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	ZeroMemory(&pi, sizeof(pi));

	if(!(dwCreateProcessResult = CreateProcessA(NULL, lpstrProgramName, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi))) {
		return -1;
	}

	WaitForSingleObject(pi.hProcess, INFINITE);
	GetExitCodeProcess(pi.hProcess, &dwExitCode);

	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);

	DeleteFileA(lpstrProgramName);

	return dwExitCode;
}

void Wait() {
	_getch();
}

int _tmain(int argc, _TCHAR* argv[])
{
	DWORD drmCheckResult;

	prepareDRMCheck();

	if(VeryImportantFunction(1417780879, 2318147680) != 0xDEADBEEF) {
		printf("Failed to load important.dll!\r\n");
		for(;;) Wait();
	}

	drmCheckResult = spawnDRMProcessAndGetExitCode();
	if(drmCheckResult != 0) {
		printf("You did not remember mother's day!!!\r\nThat's evil!\r\n");
		for(;;) Wait();
		return drmCheckResult;
	}

	printf("I'm glad you remembered mother's day! :)\r\n");
	Wait();

	return 0;
}

