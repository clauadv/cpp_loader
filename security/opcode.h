#pragma once
#include "../includes.h"
#include "helper.h"

namespace security::opcode {
	inline std::vector<helper::opcode_info> guard_sections;

	inline bool add(void* region, std::size_t region_size, const std::string& name) {
		auto gb = [&](const std::size_t add) -> std::uint8_t {
			const auto ptr = static_cast<uint8_t*>(region) + add;
			const auto opcode = *ptr;

			return opcode;
		};

		VMProtectBeginUltra("opcode::add");

		unsigned long long ret = 0;
		const auto first_bt = gb(0x0);
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

			const auto final_addr = reinterpret_cast<std::uintptr_t>(region) + 0x5;

			std::stringstream reorg;
			reorg << adr_content.str().at(2);
			reorg << adr_content.str().at(3);
			reorg << adr_content.str().at(0);
			reorg << adr_content.str().at(1);

			const auto relacstoi = std::stoul(reorg.str(), nullptr, 16);
			const auto addr = final_addr + relacstoi;
			ret = addr;

		} else {
			ret = reinterpret_cast<unsigned long long>(region);
		}

		guard_sections.emplace_back(reinterpret_cast<void*>(ret), region_size, name);
		// std::cout << stra("[*] hooked ") << name << stra(" (0x") << std::hex << ret << stra(")") << std::endl;

		VMProtectEnd();

		return true;
	}

	inline std::vector<int> get_opcode(const std::vector<helper::opcode_info>::value_type& e) {
		auto cur_opcode = std::vector<int>();
		for (auto c = 0; c < e.size; c++) {
			const auto ptr = static_cast<uint8_t*>(e.region) + c;
			const auto opcode = ptr;
			const auto diasm_opcode = *opcode;

			cur_opcode.push_back(diasm_opcode);
		}

		return cur_opcode;
	}

	inline void work() {
		VMProtectBeginUltra("opcode::work");

		if (guard_sections.empty()) {
			return;
		}

		for (auto e : guard_sections) {
			auto cur = get_opcode(e);
			for (auto f = 0; f < e.size; f++) {
				const auto current = cur.at(f);
				const auto original = e.original.at(f);
				if (current != original) {
					// std::cout << stra("security::opcode::work - found ") << stra(e.name.c_str()) << std::endl;
					std::exit(-1);
				}
			}
		}

		VMProtectEnd();
	}
}