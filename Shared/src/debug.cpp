#define _CRT_SECURE_NO_WARNINGS
#include "debug.h"

#if _DEBUG

#include <Windows.h>
#include <cstdint>
#include <sstream>

void __consoleAttach() {
	AllocConsole();
	AttachConsole(GetCurrentProcessId());
	freopen("CON", "w", stdout);
	freopen("CON", "w", stderr);
}

void __bindump(const void* ptr, int num_bits, const char* expr)
{
	uint8_t* buf = (uint8_t*)ptr;

	int bitsLeft = num_bits;
	int num_octets = (num_bits + CHAR_BIT - 1) / CHAR_BIT;
	int octets_per_line = 4;

	if (expr)
		fprintf(stderr, "---BINDUMP---\n<%s, %d>:\n0x%p\n", expr, num_bits, ptr);

	for (int i = num_octets - 1; i > 0; i -= octets_per_line) {
		fprintf(stderr, "    +%04x: ", i-3);

		for (int j = 0; j < octets_per_line; j++) {
			if (j) {
				fprintf(stderr, "_");
			}

			int curPos = i - j;
			uint8_t curByte = buf[i - j];

			if (curPos < num_octets) {
				for (int k = 0; k < 8; k++) {
					fprintf(stderr, "%d", (curByte >> (7 - k)) & 1);
					--bitsLeft;
				}
			}
			else {
				fprintf(stderr, "        ");
			}

		}

		fprintf(stderr, "\n");
	}

	fprintf(stderr, "------\n");
}

void __hexdump(const void* ptr, int buflen, const char* expr) {
	unsigned char* buf = (unsigned char*)ptr;
	int i, j;

	if (expr)
		fprintf(stderr, "---HEXDUMP---\n<%s, %d>:\n0x%p\n", expr, buflen, ptr);

	for (i = 0; i < buflen; i += 16) {
		fprintf(stderr, "    +%04x: ", i);
		for (j = 0; j < 16; j++)
			if (i + j < buflen)
				fprintf(stderr, "%02x ", buf[i + j]);
			else
				fprintf(stderr, "   ");
		fprintf(stderr, " ");
		for (j = 0; j < 16; j++)
			if (i + j < buflen)
				fprintf(stderr, "%c", isprint(buf[i + j]) ? buf[i + j] : '.');
		fprintf(stderr, "\n");
	}

	fprintf(stderr, "------\n");
}

bool __debuggerLaunch()
{
	// Get System directory, typically c:\windows\system32
	std::wstring systemDir(MAX_PATH + 1, '\0');
	UINT nChars = GetSystemDirectoryW(&systemDir[0], systemDir.length());
	if (nChars == 0)
		return false; // failed to get system directory

	systemDir.resize(nChars);

	// Get process ID and create the command line
	DWORD pid = GetCurrentProcessId();
	std::wostringstream s;
	s << systemDir << L"\\vsjitdebugger.exe -p " << pid;
	std::wstring cmdLine = s.str();

	// Start debugger process
	STARTUPINFOW si;
	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);

	PROCESS_INFORMATION pi;
	ZeroMemory(&pi, sizeof(pi));

	if (!CreateProcessW(NULL, &cmdLine[0], NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) return false;

	// Close debugger process handles to eliminate resource leak
	CloseHandle(pi.hThread);
	CloseHandle(pi.hProcess);

	// Wait for the debugger to attach
	while (!IsDebuggerPresent()) Sleep(100);

	// Stop execution so the debugger can take over
	DebugBreak();
	return true;
}

#endif