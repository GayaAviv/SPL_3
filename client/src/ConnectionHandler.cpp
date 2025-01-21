#include "../include/ConnectionHandler.h"

using boost::asio::ip::tcp;

using std::cin;
using std::cout;
using std::cerr;
using std::endl;
using std::string;

ConnectionHandler::ConnectionHandler(string host, short port) : host_(host), port_(port), io_service_(),
                                                                socket_(io_service_) , connected(false){}

ConnectionHandler::~ConnectionHandler() {
	close();
}

bool ConnectionHandler::connect() {
	std::cout << "Starting connect to "
	          << host_ << ":" << port_ << std::endl;
	try {
		tcp::endpoint endpoint(boost::asio::ip::address::from_string(host_), port_); // the server endpoint
		boost::system::error_code error;
		socket_.connect(endpoint, error);
		connected = true;
		if (error)
			throw boost::system::system_error(error);
	}
	catch (std::exception &e) {
		std::cerr << "Connection failed (Error: " << e.what() << ')' << std::endl;
		return false;
	}
	return true;
}

bool ConnectionHandler::getBytes(char bytes[], unsigned int bytesToRead) {
	size_t tmp = 0;
	boost::system::error_code error;
	try {
		while (!error && bytesToRead > tmp) {
			tmp += socket_.read_some(boost::asio::buffer(bytes + tmp, bytesToRead - tmp), error);
		}
		if (error)
			throw boost::system::system_error(error);
	} catch (std::exception &e) {
		std::cerr << "recv failed (Error: " << e.what() << ')' << std::endl;
		return false;
	}
	return true;
}

// bool ConnectionHandler::getBytes(char bytes[], unsigned int bytesToRead) {
//     size_t tmp = 0;
//     boost::system::error_code error;
//     boost::asio::steady_timer timer(socket_.get_executor());
//     bool timeout_occurred = false;

//     try {
//         if (!socket_.is_open()) {
//             std::cerr << "Socket is not open." << std::endl;
//             return false;
//         }

//         // הגדר זמן קצוב
//         timer.expires_after(std::chrono::seconds(5));

//         // הגדר פונקציה שתתבצע אם הזמן הקצוב חלף
//         timer.async_wait([&](const boost::system::error_code& ec) {
//             if (!ec) {
//                 timeout_occurred = true;
//                 socket_.cancel(); // מבטל פעולת קריאה/כתיבה פעילה
//             }
//         });

//         // קריאת הנתונים
//         while (!error && bytesToRead > tmp) {
//             tmp += socket_.read_some(boost::asio::buffer(bytes + tmp, bytesToRead - tmp), error);
//             if (timeout_occurred) {
//                 std::cerr << "Timeout occurred while reading from socket." << std::endl;
//                 return false;
//             }
//         }

//         // ביטול הטיימר אם הקריאה הסתיימה בהצלחה
//         timer.cancel();

//         if (error) {
//             if (error == boost::asio::error::eof) {
//                 std::cerr << "Connection closed by the server." << std::endl;
//             } else {
//                 std::cerr << "Boost error: " << error.message() << std::endl;
//             }
//             return false;
//         }
//     } catch (std::exception& e) {
//         std::cerr << "recv failed (Error: " << e.what() << ')' << std::endl;
//         return false;
//     }

//     return true;
// }

bool ConnectionHandler::sendBytes(const char bytes[], int bytesToWrite) {
	int tmp = 0;
	boost::system::error_code error;
	try {
		while (!error && bytesToWrite > tmp) {
			tmp += socket_.write_some(boost::asio::buffer(bytes + tmp, bytesToWrite - tmp), error);
		}
		if (error)
			throw boost::system::system_error(error);
	} catch (std::exception &e) {
		std::cerr << "recv failed (Error: " << e.what() << ')' << std::endl;
		return false;
	}
	return true;
}


bool ConnectionHandler::getLine(std::string &line) {
	return getFrameAscii(line, '\n');
}

bool ConnectionHandler::sendLine(std::string &line) {
	return sendFrameAscii(line, '\n');
}


bool ConnectionHandler::getFrameAscii(std::string &frame, char delimiter) {
	char ch;
	// Stop when we encounter the null character.
	// Notice that the null character is not appended to the frame string.
	try {
		do {
			if (!getBytes(&ch, 1)) {
				return false;
			}
			if (ch != '\0')
				frame.append(1, ch);
		} while (delimiter != ch);
	} catch (std::exception &e) {
		std::cerr << "recv failed2 (Error: " << e.what() << ')' << std::endl;
		return false;
	}
	return true;
}

bool ConnectionHandler::sendFrameAscii(const std::string &frame, char delimiter) {
	bool result = sendBytes(frame.c_str(), frame.length());
	if (!result) return false;
	return sendBytes(&delimiter, 1);
}

// Close down the connection properly.
void ConnectionHandler::close() {
	try {
		socket_.close();
		connected = false;
	} catch (...) {
		std::cout << "closing failed: connection already closed" << std::endl;
	}
}

bool ConnectionHandler::isConnected(){
	return connected;
}
