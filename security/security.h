#pragma once
#include "../includes.h"

struct opcode_struct {
	opcode_struct(void* a1, int a2, bool a3, std::string a4) {
		region = a1;
		size = a2;
		critical = a3;
		name = a4;

		initialize();
	}

	std::string name;
	bool critical;
	int size;
	void* region;
	std::vector<int> original;

	auto initialize() -> void {
		if (size != 0) {
			for (auto c = 0; c < size; c++) {
				const auto ptr = (uint8_t*)region + c;
				const auto opcode = ptr;
				const auto diasm_opcode = *opcode;

				original.push_back(diasm_opcode);
			}
		}
		else {
			auto last_result = int();
			auto cnt = 0;
			while (last_result != 0xc3) {
				const auto ptr = (uint8_t*)region + cnt;
				const auto opcode = ptr;
				const auto opc = *opcode;

				original.push_back(opc);
				cnt++;
				last_result = opc;
			}

			size = cnt - 1;
			original.pop_back();
		}
	}
};

class opcode_class {
public:
	auto setup() -> bool;
	auto add(void* region, int region_size, bool critical, std::string name) -> bool;
	static auto get_opcode(std::vector<opcode_struct>::value_type& e)->std::vector<int>;
	auto work() -> void;

private:
	std::vector<opcode_struct> guard_sections;
};

extern opcode_class* opcode;

struct handle_info {
	unsigned long pid;
	HWND hndl;
};

struct process_info {
	process_info(WCHAR a1[MAX_PATH], const DWORD a2, TCHAR a3[260], const char a4[MAX_PATH]) {
		std::wstring ws(a1);
		const std::string str(ws.begin(), ws.end());
		const std::string str2(a3);

		proc_id = a2;
		exe_name = str;
		full_path = str2;
		title = a4;
	}

	std::string exe_name;
	std::string full_path;
	std::string title;
	DWORD proc_id;
};

class process_class {
public:
	auto is_main(const HWND handle) -> bool;
	auto work() -> void;
	auto stealth_thread(HANDLE h_thread) -> bool;

private:
	PROCESS_INFORMATION open(const int process_id);
	HWND find_main(unsigned long process_id);
	std::vector<process_info> get();
	void scan(std::vector<std::basic_string<char>>& blacklist_procname, std::vector<std::basic_string<char>>& blacklist_title, std::vector<process_info>::value_type obj, uint32_t sum) const;
};

extern process_class* process;