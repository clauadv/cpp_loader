#include "../includes.h"

auto watcher() -> void {
	VMProtectBeginVirtualization("watcher");

	while (true) {
		std::this_thread::sleep_for(std::chrono::milliseconds(250));

		if (VMProtectIsDebuggerPresent(true) ||  !VMProtectIsProtected() || VMProtectIsVirtualMachinePresent() || IsDebuggerPresent() || !VMProtectIsValidImageCRC()) {
			std::exit(-1);
		}

		opcode->work();
		process->work();
	}

	VMProtectEnd();
}

auto main() -> int {
	SetConsoleTitle("loader");

	std::cout << "                                          " << std::endl;
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

	game = new game_class();
	process = new process_class();
	opcode = new opcode_class();

	auto main_hook = new safe_string(xor_string("main"));
	auto debugger_hook = new safe_string(xor_string("IsDebuggerPresent"));
	auto watcher_hook = new safe_string(xor_string("watcher"));
	auto load_library_hook = new safe_string(xor_string("LoadLibraryA"));
	auto virtual_protect_hook = new safe_string(xor_string("VirtualProtectEx"));
	auto create_thread_hook = new safe_string(xor_string("CreateThread"));

	opcode->setup();

	main_hook->show();
	opcode->add((void*)main, 0, 1, main_hook->raw);
	main_hook->hide();

	debugger_hook->show();
	opcode->add((void*)IsDebuggerPresent, 0, 1, debugger_hook->raw);
	debugger_hook->hide();

	watcher_hook->show();
	opcode->add((void*)watcher, 0, 1, watcher_hook->raw);
	watcher_hook->hide();

	load_library_hook->show();
	opcode->add((void*)LoadLibraryA, 0, 1, load_library_hook->raw);
	load_library_hook->hide();

	virtual_protect_hook->show();
	opcode->add((void*)VirtualProtectEx, 0, 1, virtual_protect_hook->raw);
	virtual_protect_hook->hide();

	create_thread_hook->show();
	opcode->add((void*)CreateThread, 0, 1, create_thread_hook->raw);
	create_thread_hook->hide();

	auto handle = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)watcher, 0, 0, 0);
	process->stealth_thread(handle);

	print("requesting dll", 0);

	auto cheat = new safe_string(xor_string("http://localhost/data.php"));
	cheat->show();

	unsigned long sz;
	std::string web_data = web->get_data(cheat->raw, sz);
	cheat->hide();

	print(std::string("received dll with size ").append(std::to_string(web_data.size())).c_str(), 5);

	std::string primary, secondary, bytes;
	primary = web_data.substr(0, 70);
	secondary = web_data.substr(70, 70);
	bytes = web_data.substr(140, web_data.size() - 1);

	game->encrypt(primary, 32, bytes);
	game->encrypt(secondary, 32, bytes);

	print("waiting for game", 0);

	while (!game->is_running()) {
		std::this_thread::sleep_for(std::chrono::milliseconds(500));
	}

	print("game found", 10);

	game->inject(bytes);

	print("finished", 5);

	primary = "";
	secondary = "";
}