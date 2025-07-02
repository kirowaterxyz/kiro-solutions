#pragma once


const uint8_t baud_change_command[] = {0xDE, 0xAD, 0x05, 0x00, 0xA5, 0x00, 0x09, 0x3D, 0x00};

inline bool change_baud_rate_kmbox(HANDLE& hSerial, const string& port) {
	// 1. Open port at 115200
	if (!open_port(hSerial, port.c_str(), CBR_115200)) {
		std::cout << "(/) Failed to open port at 115200" << std::endl;
		return false;
	}

	std::this_thread::sleep_for(std::chrono::milliseconds(500)); // Let device settle

	// 2. Send baud change command
	DWORD bytesWritten;
	if (!WriteFile(hSerial, baud_change_command, sizeof(baud_change_command), &bytesWritten, nullptr)) {
		std::cout << "(/) Failed to send baud change command" << std::endl;
		CloseHandle(hSerial);
		return false;
	}

	FlushFileBuffers(hSerial); // Flush

	CloseHandle(hSerial); // Close old handle
	std::this_thread::sleep_for(std::chrono::milliseconds(500));

	// 3. Open port again at 4M baud
	if (!open_port(hSerial, port.c_str(), 4000000)) {
		std::cout << "(/) Failed to reopen port at 4M baud" << std::endl;
		return false;
	}

	// Optional: Send km.version()
	const char* version_cmd = "km.version()\r";
	WriteFile(hSerial, version_cmd, strlen(version_cmd), &bytesWritten, nullptr);
	FlushFileBuffers(hSerial);

	return true;
}

inline bool connect_serial_kmbox() {
	string port = find_port("USB-SERIAL CH340");
	string label = "Arduino/Kmbox";

	if (!port.empty()) {
		if (open_port(hSerial, port.c_str(), CBR_115200)) {
			std::cout << "(+) " << label << " found" << std::endl;
			return true;
		}
		else {
			std::cout << "(/) Couldn't connect to " << label << std::endl;
		}
	}
	else {
		std::cout << "(/) No " << label << " found" << std::endl;

		port = find_port("USB-Enhanced-SERIAL CH343");
		label = "Makcu";

		if (!port.empty()) {
			if (change_baud_rate_kmbox(hSerial, port)) {
				std::cout << "(+) " << label << " connected at 4M" << std::endl;
				return true;
			}
			else {
				std::cout << "(/) Couldn't connect to " << label << std::endl;
			}
		}
		else {
			std::cout << "(/) No " << label << " found" << std::endl;
		}
	}

	return false;
}

inline bool connect_net_kmbox() {
	int result = kmNet_init((char*)settings::kmbox::net::ip.c_str(), (char*)settings::kmbox::net::port.c_str(), (char*)settings::kmbox::net::uuid.c_str());

	if (result == err_creat_socket ||result == err_net_rx_timeout || result == err_net_version) {
		std::cout << "(/) Failed to connect to kmbox net" << std::endl;
		return false;
	}

	std::cout << "(+) Kmbox net connected" << std::endl;
	return true;
}

inline void km_move(int X, int Y) {
	std::string command = "km.move(" + std::to_string(X) + "," + std::to_string(Y) + ")\r\n";
	send_command(hSerial, command.c_str());
}

inline void km_unclick()
{
	std::string unclickcommand = "km.left(0)\r\n";
	send_command(hSerial, unclickcommand.c_str());
}

inline void km_click() {
	std::string command2 = "km.click(0)\r\n";
	std::string command = "km.left(1)\r\n";
	std::string command1 = "km.left(0)\r\n";
	// IM LAZY LMAOOO
	send_command(hSerial, command.c_str());
	send_command(hSerial, command1.c_str());
	send_command(hSerial, command2.c_str());
}
