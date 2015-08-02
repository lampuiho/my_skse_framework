#include "Utilities.h"
#ifdef _DEBUG
#ifdef _MSC_VER
#include <windows.h>
#include <stdio.h>
#include <fcntl.h>
#include <io.h>
#include <fstream>
void OpenConsole()
{
	int outHandle, errHandle, inHandle;
	FILE *outFile, *errFile, *inFile;
	AllocConsole();
	CONSOLE_SCREEN_BUFFER_INFO coninfo;
	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &coninfo);
	coninfo.dwSize.Y = 9999;
	SetConsoleScreenBufferSize(GetStdHandle(STD_OUTPUT_HANDLE), coninfo.dwSize);

	outHandle = _open_osfhandle((long)GetStdHandle(STD_OUTPUT_HANDLE), _O_TEXT);
	errHandle = _open_osfhandle((long)GetStdHandle(STD_ERROR_HANDLE), _O_TEXT);
	inHandle = _open_osfhandle((long)GetStdHandle(STD_INPUT_HANDLE), _O_TEXT);

	outFile = _fdopen(outHandle, "w");
	errFile = _fdopen(errHandle, "w");
	inFile = _fdopen(inHandle, "r");

	*stdout = *outFile;
	*stderr = *errFile;
	*stdin = *inFile;

	setvbuf(stdout, NULL, _IONBF, 0);
	setvbuf(stderr, NULL, _IONBF, 0);
	setvbuf(stdin, NULL, _IONBF, 0);

	std::ios::sync_with_stdio();

}
#endif        // or std::cerr << (x) << std::flush
#endif

bool SubstringEqual(const char* string, const char* sub_string){
	for (UInt32 i = 0; sub_string[i]; ++i){
		if (string[i] != sub_string[i])
			return false;
	}
	return true;
}
SInt32 Substring(const char* string, const char* sub_string){
	for (UInt32 i = 0; string[i]; ++i){
		if (SubstringEqual(&string[i], sub_string))
			return i;
	}
	return -1;
}
UInt32 log2(UInt32 in){
	UInt32 result = 0;
	while (in >>= 1) ++result;
	return result;
}

std::string format(const char *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);

	const size_t SIZE = 512;
	char buffer[SIZE] = { 0 };
	vsnprintf(buffer, SIZE, fmt, ap);

	va_end(ap);

	return std::string(buffer);
}