#pragma once
#include "../includes.h"

namespace security::opcode::helper {
	struct opcode_info {
		opcode_info(void* a1, const int a2, const std::string& a3) {
			region = a1;
			size = a2;
			name = a3;

			initialize();
		}

		std::string name;
		int size;
		void* region;
		std::vector<int> original;

		void initialize() {
			if (size != 0) {
				for (auto c = 0; c < size; c++) {
					const auto ptr = static_cast<uint8_t*>(region) + c;
					const auto opcode = *ptr;

					original.push_back(opcode);
				}
			} else {
				auto last_result = int();
				auto cnt = 0;
				while (last_result != 0xc3) {
					const auto ptr = static_cast<uint8_t*>(region) + cnt;
					const auto opcode = *ptr;

					original.push_back(opcode);
					cnt++;
					last_result = opcode;
				}

				size = cnt - 1;
				original.pop_back();
			}
		}
	};
}

namespace security::process::helper {
	struct process_info {
		process_info(WCHAR a1[MAX_PATH], const std::uintptr_t a2, TCHAR a3[260], const char a4[MAX_PATH]) {
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
		std::uintptr_t proc_id;
	};

	struct handle_info {
		unsigned long process_id;
		HWND handle;
	};

	inline bool is_main(const HWND handle) {
		return GetWindow(handle, GW_OWNER) == static_cast<HWND>(nullptr) && IsWindowVisible(handle);
	}

	inline int __stdcall enum_window(const HWND handle, const LPARAM param) {
		auto& data = *reinterpret_cast<handle_info*>(param);
		unsigned long process_id;

		GetWindowThreadProcessId(handle, &process_id);
		if (data.process_id != process_id || !is_main(handle)) {
			return true;
		}

		data.handle = handle;

		return false;
	}

	inline HWND find_main(const unsigned long process_id) {
		handle_info parameters{};

		parameters.process_id = process_id;
		EnumWindows(enum_window, reinterpret_cast<LPARAM>(&parameters));

		return parameters.handle;
	}
}