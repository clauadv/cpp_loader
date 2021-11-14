#pragma once
#include "../includes.h"

typedef NTSTATUS(WINAPI *NtWriteVirtualMemory_fn) (HANDLE, PVOID, PVOID, ULONG, PULONG);
typedef NTSTATUS(WINAPI *NtReadVirtualMemory_fn) (HANDLE, PVOID, PVOID, ULONG, PULONG);
typedef HMODULE(__stdcall* pLoadLibraryA)(LPCSTR);
typedef FARPROC(__stdcall* pGetProcAddress)(HMODULE, LPCSTR);

class game_class {
public:
	auto is_running() -> bool;
	auto inject(std::string bytes) -> void;
	auto encrypt(std::string key, int size_key, std::string& data) -> void;

private:
	DWORD bdo_process_id;
	auto get_process_id(std::string exe)->std::uintptr_t;
	std::string game_process = "csgo.exe";
	std::string game_module = "serverbrowser.dll"; // csgo

	NtWriteVirtualMemory_fn NtWriteVirtualMemory;
	NtReadVirtualMemory_fn NtReadVirtualMemory;

	struct loader_data {
		LPVOID ImageBase;

		PIMAGE_NT_HEADERS NtHeaders;
		PIMAGE_BASE_RELOCATION BaseReloc;
		PIMAGE_IMPORT_DESCRIPTOR ImportDirectory;

		pLoadLibraryA fnLoadLibraryA;
		pGetProcAddress fnGetProcAddress;
	};
};

extern game_class* game;