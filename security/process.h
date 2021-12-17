#pragma once
#include "../includes.h"

namespace security::process {
	inline bool stealth_thread(const HANDLE thread) {
		typedef NTSTATUS(NTAPI* pNtSetInformationThread)(HANDLE, UINT, PVOID, ULONG);
		NTSTATUS status;

		const auto nt_sit = reinterpret_cast<pNtSetInformationThread>(GetProcAddress(GetModuleHandle(TEXT(stra("ntdll.dll"))), stra("NtSetInformationThread")));
		if (nt_sit == nullptr) {
			return false;
		}

		if (thread == nullptr) {
			status = nt_sit(GetCurrentThread(), 0x11, nullptr, 0);
		} else {
			status = nt_sit(thread, 0x11, nullptr, 0);
		}

		if (status != 0x00000000) {
			return false;
		}

		return true;
	}

	inline std::vector<helper::process_info> get() {
		auto list = std::vector<helper::process_info>();
		const auto handle = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
		auto process_entry32 = PROCESSENTRY32W();
		if (handle == INVALID_HANDLE_VALUE) {
			return {};
		}

		process_entry32.dwSize = sizeof(PROCESSENTRY32W);
		if (Process32FirstW(handle, &process_entry32)) {
			TCHAR filename[MAX_PATH];
			const auto pe_handle = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, false, process_entry32.th32ProcessID);
			GetModuleFileNameEx(pe_handle, nullptr, filename, MAX_PATH);
			CloseHandle(pe_handle);

			auto wnd = helper::find_main(process_entry32.th32ProcessID);
			char wnd_title[MAX_PATH];
			if (wnd) {
				GetWindowTextA(wnd, wnd_title, sizeof(wnd_title));
			}

			list.emplace_back(process_entry32.szExeFile, process_entry32.th32ProcessID, filename, wnd_title);

			while (Process32NextW(handle, &process_entry32)) {
				const auto pe_handle = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, false, process_entry32.th32ProcessID);
				GetModuleFileNameEx(pe_handle, nullptr, filename, MAX_PATH);
				CloseHandle(pe_handle);

				wnd = helper::find_main(process_entry32.th32ProcessID);
				if (wnd) {
					GetWindowTextA(wnd, wnd_title, sizeof(wnd_title));

					list.emplace_back(process_entry32.szExeFile, process_entry32.th32ProcessID, filename, wnd_title);
				}
			}
		}

		return list;
	}

	inline void scan(const std::vector<std::basic_string<char>>& blacklist_process, const std::vector<std::basic_string<char>>& blacklist_title, const std::vector<helper::process_info>::value_type& obj) {
		for (const auto& process_name : blacklist_process) {
			if (strstr(obj.exe_name.c_str(), process_name.c_str())) {
				// std::cout << stra("security::process::scan - found ") << stra(obj.title.c_str()) << std::endl;
				std::exit(-1);
			}
		}

		for (const auto& title_name : blacklist_title) {
			if (strstr(obj.title.c_str(), title_name.c_str())) {
				// std::cout << stra("security::process::scan - found ") << stra(obj.title.c_str()) << std::endl;
				std::exit(-1);
			}
		}
	}

	inline void work() {
		VMProtectBeginMutation("process::work");

		const auto process_list = get();
		if (process_list.empty()) {
			return;
		}

		static auto blacklist_process = std::vector<std::string>() = {
			stra("CrySearch"),
			stra("x64dbg"),
			stra("pe-sieve"),
			stra("PowerTool"),
			stra("windbg"),
			stra("DebugView"),
			stra("Process Hacker"),
			stra("dnSpy"),
			stra("exeinfope"),
			stra("HxD"),
			stra("ida"),
			stra("joeboxserver"),
			stra("joeboxcontrol"),
			stra("wireshark"),
			stra("avp"),
			stra("sniff_hit"),
			stra("sysAnalyzer"),
			stra("Fiddler"),
			stra("ollydbg"),
		};

		static auto blacklist_title = std::vector<std::string>() = {
			stra("Cheat Engine"),
			stra("Cheat - Engine"),
			stra("CrySearch"),
			stra("x64dbg"),
			stra("ollydbg"),
			stra("PE Tools"),
			stra("PowerTool"),
			stra("DbgView"),
			stra("Dbgview"),
			stra("\"\DESKTOP"),
			stra("(local)"),
			stra("Process Hacker"),
			stra("dnSpy"),
			stra("Exeinfo PE"),
			stra("HxD"),
			stra("IDA")
		};

		for (const auto& obj : process_list) {
			scan(blacklist_process, blacklist_title, obj);
		}

		VMProtectEnd();
	}
}