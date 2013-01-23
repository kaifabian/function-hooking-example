// important.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "important.h"

__declspec(dllexport)
int VeryImportantFunction(int a, int b) {
	return a + b;
}