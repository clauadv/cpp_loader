#include "../includes.h"

void watcher() {
	VMProtectBeginVirtualization("watcher");

	while (true) {
		std::this_thread::sleep_for(std::chrono::milliseconds(250));

		if (VMProtectIsDebuggerPresent(true) || !VMProtectIsProtected() || VMProtectIsVirtualMachinePresent() || IsDebuggerPresent() || !VMProtectIsValidImageCRC()) {
			std::exit(-1);
		}

		security::opcode::work();
		security::process::work();
	}
}

void main() {
	SetConsoleTitle("loader");

	std::cout << "                                          " << std::endl;
	std::cout << "  /$$                           /$$       " << std::endl;
	std::cout << " | $$                          | $$       " << std::endl;
	std::cout << " | $$$$$$$   /$$$$$$   /$$$$$$$| $$   /$$ " << std::endl;
	std::cout << " | $$__  $$ |____  $$ /$$_____/| $$  /$$/ " << std::endl;
	std::cout << " | $$  \ $$  /$$$$$$$| $$      | $$$$$$/  " << std::endl;
	std::cout << " | $$  | $$ /$$__  $$| $$      | $$_  $$  " << std::endl;
	std::cout << " | $$  | $$|  $$$$$$$|  $$$$$$$| $$ \  $$ " << std::endl;
	std::cout << " |__/  |__/ \_______/ \_______/|__/  \__/ " << std::endl;
	std::cout << "                                          " << std::endl;
	std::cout << "                                          " << std::endl;

	const auto main_hook = new safe_string(_("main"));
	const auto debugger_hook = new safe_string(_("IsDebuggerPresent"));
	const auto watcher_hook = new safe_string(_("watcher"));
	const auto load_library_hook = new safe_string(_("LoadLibraryA"));
	const auto virtual_protect_hook = new safe_string(_("VirtualProtectEx"));
	const auto create_thread_hook = new safe_string(_("CreateThread"));

	main_hook->show();
	security::opcode::add(static_cast<void*>(main), 0, main_hook->raw);
	main_hook->hide();

	debugger_hook->show();
	security::opcode::add(static_cast<void*>(IsDebuggerPresent), 0, debugger_hook->raw);
	debugger_hook->hide();

	watcher_hook->show();
	security::opcode::add(static_cast<void*>(watcher), 0, watcher_hook->raw);
	watcher_hook->hide();

	load_library_hook->show();
	security::opcode::add(static_cast<void*>(LoadLibraryA), 0, load_library_hook->raw);
	load_library_hook->hide();

	virtual_protect_hook->show();
	security::opcode::add(static_cast<void*>(VirtualProtectEx), 0, virtual_protect_hook->raw);
	virtual_protect_hook->hide();

	create_thread_hook->show();
	security::opcode::add(static_cast<void*>(CreateThread), 0, create_thread_hook->raw);
	create_thread_hook->hide();

	const auto handle = CreateThread(nullptr, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(watcher), nullptr, 0, nullptr);
	security::process::stealth_thread(handle);

	std::cout << "[*] requesting data" << std::endl;
	std::this_thread::sleep_for(std::chrono::seconds(5));

	const auto cheat = new safe_string(_("http://localhost/data.php"));

	cheat->show();
	const auto web_data = web::get_data(cheat->raw);
	cheat->hide();

	std::cout << "[*] received data (size: " << web_data.size() << ")" << std::endl;
	std::this_thread::sleep_for(std::chrono::seconds(5));

	auto primary = web_data.substr(0, 70);
	auto secondary = web_data.substr(70, 70);
	auto bytes = web_data.substr(140, web_data.size() - 1);

	game::encrypt(primary, 32, bytes);
	game::encrypt(secondary, 32, bytes);

	std::cout << "[*] waiting for " << game::game_process << std::endl;
	while (!game::is_running()) {
		std::this_thread::sleep_for(std::chrono::milliseconds(500));
	}

	std::cout << "[*] injecting dll" << std::endl;
	std::this_thread::sleep_for(std::chrono::seconds(10));

	game::inject(bytes);

	std::cout << "[*] injected" << std::endl;
	std::this_thread::sleep_for(std::chrono::seconds(5));

	primary.clear();
	secondary.clear();
}