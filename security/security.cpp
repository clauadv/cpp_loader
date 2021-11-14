#include "../includes.h"

auto opcode_class::setup() -> bool {
	return true;
}

auto opcode_class::add(void* region, int region_size, bool critical, std::string name) -> bool {
	auto gb = [&](int add) -> const uint8_t {
		const auto ptr = (uint8_t*)region + add;
		const auto opcode = ptr;
		const auto opc = *opcode;

		return opc;
	};

	VMProtectBeginUltra("opcode::add");

	unsigned long long res = 0;
	auto first_bt = gb(0x0);
	if (first_bt == 0xe9) {
		auto last_read = 0;
		auto mempad = 0;
		std::stringstream adr_content;
		while (last_read != 0xe9) {
			mempad++;

			last_read = gb(mempad);
			if (last_read != 0xe9) {
				adr_content << std::hex << last_read << "\n";
			}
		}

		auto final_addr = (DWORD64)region + 0x5;
		
		std::stringstream reorg;
		reorg << adr_content.str().at(2);
		reorg << adr_content.str().at(3);
		reorg << adr_content.str().at(0);
		reorg << adr_content.str().at(1);

		auto relacstoi = std::stoul(reorg.str(), nullptr, 16);
		auto funcadr = final_addr + relacstoi;
		res = funcadr;
	}
	else {
		res = (unsigned long long)region;
	}

	this->guard_sections.emplace_back((void*)res, region_size, critical, name);
	//std::cout << stra("hooked 0x") << std::hex << res << std::endl;

	VMProtectEnd();

	return true;
}

auto opcode_class::get_opcode(std::vector<opcode_struct>::value_type& e) -> std::vector<int> {
	auto cur_opcode = std::vector<int>();
	for (auto c = 0; c < e.size; c++) {
		const auto ptr = (uint8_t*)e.region + c;
		const auto opcode = ptr;
		const auto diasm_opcode = *opcode;

		cur_opcode.push_back(diasm_opcode);
	}

	return cur_opcode;
}

auto opcode_class::work() -> void {
	VMProtectBeginUltra("opcode::work");

	if (this->guard_sections.empty()) {
		return;
	}

	for (auto e : this->guard_sections) {
		auto cur = get_opcode(e);
		for (auto f = 0; f < e.size; f++) {
			const auto cur_ = cur.at(f);
			const auto ori_ = e.original.at(f);
			if (cur_ != ori_) {
				const auto ptr = (uint8_t*)e.region + f;
				if (e.critical) {
					//MessageBoxA(nullptr, stra(std::string(e.name).c_str()), "", 0);
					std::exit(-1);
				}

				//MessageBoxA(nullptr, stra(std::string(e.name).c_str()), "", 0);
				WriteProcessMemory(GetCurrentProcess(), (void*)ptr, &ori_, 1, nullptr);
			}
		}
	}

	VMProtectEnd();
}

opcode_class* opcode;

PROCESS_INFORMATION process_class::open(const int process_id) {
	auto info = PROCESS_INFORMATION();
	if (!process_id) {
		return {};
	}

	info.hProcess = OpenProcess(PROCESS_ALL_ACCESS | PROCESS_VM_READ, 0, process_id);
	if (info.hProcess) {
		return info;
	}
}

auto process_class::is_main(const HWND handle) -> bool {
	return GetWindow(handle, GW_OWNER) == HWND(nullptr) && IsWindowVisible(handle);
}

BOOL CALLBACK enum_window(HWND handle, LPARAM param) {
	auto& data = *(handle_info*)param;
	unsigned long process_id;

	GetWindowThreadProcessId(handle, &process_id);
	if (data.pid != process_id || !process->is_main(handle)) {
		return true;
	}

	data.hndl = handle;

	return false;
}

HWND process_class::find_main(unsigned long process_id) {
	handle_info info;

	info.pid = process_id;
	EnumWindows(enum_window, (LPARAM)&info);

	return info.hndl;
}

std::vector<process_info> process_class::get() {
	auto list = std::vector<process_info>();
	const auto handle = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	auto process_entry32 = PROCESSENTRY32W();
	if (handle == INVALID_HANDLE_VALUE) {
		return {};
	}

	process_entry32.dwSize = sizeof(PROCESSENTRY32W);
	if (Process32FirstW(handle, &process_entry32)) {
		TCHAR filename[MAX_PATH];
		auto pe_handle = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, false, process_entry32.th32ProcessID);
		GetModuleFileNameEx(pe_handle, nullptr, filename, MAX_PATH);
		CloseHandle(pe_handle);

		auto wnd = this->find_main(process_entry32.th32ProcessID);
		char wnd_title[MAX_PATH];
		if (wnd) {
			GetWindowTextA(wnd, wnd_title, sizeof(wnd_title));
		}

		list.emplace_back(process_entry32.szExeFile, process_entry32.th32ProcessID, filename, wnd_title);

		while (Process32NextW(handle, &process_entry32)) {
			auto pe_handle = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, false, process_entry32.th32ProcessID);
			GetModuleFileNameEx(pe_handle, nullptr, filename, MAX_PATH);
			CloseHandle(pe_handle);

			wnd = this->find_main(process_entry32.th32ProcessID);
			if (wnd) {
				GetWindowTextA(wnd, wnd_title, sizeof(wnd_title));

				list.emplace_back(process_entry32.szExeFile, process_entry32.th32ProcessID, filename, wnd_title);
			}
		}
	}

	return list;
}

void process_class::scan(std::vector<std::basic_string<char>>& blacklist_procname, std::vector<std::basic_string<char>>& blacklist_title, std::vector<process_info>::value_type obj, uint32_t sum) const {
	for (auto bobj : blacklist_procname) {
		if (strstr(obj.exe_name.c_str(), bobj.c_str())) {
			//MessageBoxA(nullptr, stra(std::string(obj.title).c_str()), 0, 0);
			std::exit(-1);
		}
	}

	for (auto bobj : blacklist_title) {
		if (strstr(obj.title.c_str(), bobj.c_str())) {
			//MessageBoxA(nullptr, stra(std::string(obj.title).c_str()), 0, 0);
			std::exit(-1);
		}
	}
}

auto process_class::work() -> void {
	VMProtectBeginMutation("process::work");

	auto procs = this->get();
	if (procs.empty()) {
		//MessageBoxA(nullptr, stra("List is empty"), "", 0);
		std::exit(-1);
	}

	static auto blacklist_procname = std::vector<std::string>() = {
		VMProtectDecryptStringA("CrySearch"),
		VMProtectDecryptStringA("x64dbg"),
		VMProtectDecryptStringA("pe-sieve"),
		VMProtectDecryptStringA("PowerTool"),
		VMProtectDecryptStringA("windbg"),
		VMProtectDecryptStringA("DebugView"),
		VMProtectDecryptStringA("Process Hacker"),
		VMProtectDecryptStringA("dnSpy"),
		VMProtectDecryptStringA("exeinfope"),
		VMProtectDecryptStringA("HxD"),
		VMProtectDecryptStringA("ida"),
		VMProtectDecryptStringA("joeboxserver"),
		VMProtectDecryptStringA("joeboxcontrol"),
		VMProtectDecryptStringA("wireshark"),
		VMProtectDecryptStringA("avp"),
		VMProtectDecryptStringA("sniff_hit"),
		VMProtectDecryptStringA("sysAnalyzer"),
		VMProtectDecryptStringA("Fiddler"),
		VMProtectDecryptStringA("ollydbg"),
	};

	static auto blacklist_title = std::vector<std::string>() = {
		VMProtectDecryptStringA("Cheat Engine"), 
		VMProtectDecryptStringA("Cheat - Engine"), 
		VMProtectDecryptStringA("CrySearch"), 
		VMProtectDecryptStringA("x64dbg"), 
		VMProtectDecryptStringA("ollydbg"), 
		VMProtectDecryptStringA("PE Tools"), 
		VMProtectDecryptStringA("PowerTool"), 
		VMProtectDecryptStringA("DbgView"), 
		VMProtectDecryptStringA("Dbgview"),
		VMProtectDecryptStringA("\"\DESKTOP"),
		VMProtectDecryptStringA("(local)"),
		VMProtectDecryptStringA("Process Hacker"),
		VMProtectDecryptStringA("dnSpy"),
		VMProtectDecryptStringA("Exeinfo PE"),
		VMProtectDecryptStringA("HxD"),
		VMProtectDecryptStringA("IDA")
	};

	for (auto obj : procs) {
		this->scan(blacklist_procname, blacklist_title, obj, 0);
	}

	VMProtectEnd();
}

auto process_class::stealth_thread(HANDLE h_thread) -> bool {
	typedef NTSTATUS(NTAPI* pNtSetInformationThread)(HANDLE, UINT, PVOID, ULONG);
	NTSTATUS status;

	pNtSetInformationThread NtSIT = (pNtSetInformationThread)GetProcAddress(GetModuleHandle(TEXT(stra("ntdll.dll"))), stra("NtSetInformationThread"));
	if (NtSIT == NULL) {
		return false;
	}

	if (h_thread == NULL) {
		status = NtSIT(GetCurrentThread(), 0x11, 0, 0);
	}
	else {
		status = NtSIT(h_thread, 0x11, 0, 0);
	}

	if (status != 0x00000000) {
		return false;
	}
	else {
		return true;
	}
}

process_class* process;