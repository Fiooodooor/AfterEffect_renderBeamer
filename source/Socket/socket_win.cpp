
#include "socket_win.h"

#ifdef AE_OS_WIN
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <iostream>

platform_socket::platform_socket() : SocketClientInterface()
{
}
platform_socket::~platform_socket()
{
	WSACleanup();
}
bool platform_socket::init_interface()
{
	WSAData wsa_data = { 0, 0, 0, 0, nullptr, { 0 }, { 0 } };
	if (WSAStartup(MAKEWORD(2, 0), &wsa_data) != 0) {
		print_to_debug("Error: WSAStartup failed!");
		return false;
	}
	return true;
}
bool platform_socket::create_socket()
{
	if (init_interface() == false)
		return false;
	
	const auto socket = ::WSASocket(AF_INET, SOCK_STREAM, 0, nullptr, 0, 0);

	if (socket == INVALID_SOCKET) {
		const auto err = WSAGetLastError();
		print_error_string(err);
		return false;
	}
	
	socket_descriptor_ = static_cast<long long>(socket);
	socket_state_ = Unconnected;
	return true;
}
void platform_socket::close_socket()
{
	::closesocket(socket_descriptor_);
	socket_state_ = Unconnected;
}
bool platform_socket::connect(const unsigned short port)
{
	sockaddr_in socket_address_in_v4;
	
	memset(&socket_address_in_v4, 0, sizeof(sockaddr_in));
	socket_address_in_v4.sin_family = AF_INET;
	WSAHtons(socket_descriptor_, port, &(socket_address_in_v4.sin_port));
	WSAHtonl(socket_descriptor_, 2130706433, &(socket_address_in_v4.sin_addr.s_addr));
	auto *socket_address_ptr = reinterpret_cast<struct sockaddr*>( &socket_address_in_v4);

	const auto connection_result = ::WSAConnect(socket_descriptor_, socket_address_ptr, sizeof(sockaddr_in), nullptr, nullptr, nullptr, nullptr);
	if (connection_result == SOCKET_ERROR)
	{
		const auto err = WSAGetLastError();
		print_error_string(err);
		switch (err)
		{
			case WSANOTINITIALISED:
				socket_state_ = Error;
				break;
			case WSAEISCONN:
				socket_state_ = Connected;
				break;
			case WSAEINPROGRESS:
				socket_state_ = Connecting;
				break;
			case WSAEWOULDBLOCK:
			case WSAEADDRINUSE:
			case WSAECONNREFUSED:
			case WSAETIMEDOUT:
			case WSAEACCES:
			case WSAEHOSTUNREACH:
			case WSAENETUNREACH:
			case WSAEINVAL:
			case WSAEALREADY:
			default:
				socket_state_ = Unconnected;
				break;
		}
		if (socket_state_ != Connected)
			return false;
	}
	
	socket_state_ = Connected;
	return true;
}
bool platform_socket::is_connected() const
{
	return socket_state_ == Connected;
}

unsigned long platform_socket::bytes_available() const
{
	unsigned long  bytes_to_read = 0;
	unsigned long dummy = 0;
	DWORD sizeWritten = 0;
	if (::WSAIoctl(socket_descriptor_, FIONREAD, &dummy, sizeof(dummy), &bytes_to_read, sizeof(bytes_to_read), &sizeWritten, nullptr, nullptr) == SOCKET_ERROR) {
		const auto err = WSAGetLastError();
		print_error_string(err);
		return 0;
	}

	return bytes_to_read;
}
unsigned long platform_socket::write(const char *data, const unsigned long data_length)
{
	unsigned long data_sent = 0;
	const unsigned long max_size = 49152;
	auto bytes_to_send = data_length;

	for (;;) {
		WSABUF buffer;
		buffer.buf = const_cast<char*>(data) + data_sent;
		buffer.len = bytes_to_send;
		const DWORD flags = 0;
		DWORD bytes_written = 0;

		const auto socket_return_code = ::WSASend(socket_descriptor_, &buffer, 1, &bytes_written, flags, nullptr, nullptr);
		data_sent += bytes_written;


		if (socket_return_code != SOCKET_ERROR) {
			if (data_sent == data_length)
				break;
			continue;
		}
		const auto err = WSAGetLastError();
		if (err == WSAENOBUFS) { // max 49152 per call to WSASendTo
		}
		else {
			print_error_string(err);
			switch (err) {
				case WSAECONNRESET:
				case WSAECONNABORTED:
					data_sent = -1;					
					close_socket();
					break;
				default:
					break;
			}
			break;
		}
		// for next send:	
		bytes_to_send = max_size <= (data_length - data_sent) ? max_size : (data_length - data_sent);
	}
	return data_sent;
}
unsigned long platform_socket::read(char *data, const unsigned long max_length)
{
	struct timeval tv = { 2, 0 };
	fd_set fds;	
	unsigned long bytes_read = 0;
	WSABUF buffer;
	buffer.buf = data;
	buffer.len = max_length;
	DWORD flags = 0;

	memset(&fds, 0, sizeof(fd_set));
	fds.fd_count = 1;
	fds.fd_array[0] = socket_descriptor_;

	if(select(0, &fds, nullptr, nullptr, &tv) == 0)
	{
		return 0;
	}
	
	if (::WSARecv(socket_descriptor_, &buffer, 1, &bytes_read, &flags, nullptr, nullptr) == SOCKET_ERROR)
	{
		const auto err = WSAGetLastError();
		print_error_string(err);
		switch (err) {
			case WSAETIMEDOUT:
			case WSAEWOULDBLOCK:
			case WSAEBADF:
			case WSAEINVAL:
			case WSAECONNRESET:
			case WSAECONNABORTED:		
			default:
				bytes_read = 0;
				WSASetLastError(0);
				break;
		}
	}

	return bytes_read;
}

void platform_socket::print_to_debug(const std::string &message, bool add_end_line) const
{
	std::cout << message << " ";
	if (add_end_line)
		std::cout << std::endl;
}
void platform_socket::print_error_string(const std::string &caller_name, const int error_id) const
{
	print_to_debug(caller_name, false);
	print_error_string(error_id);
}
void platform_socket::print_error_string(const int error_id) const
{
	switch (error_id)
	{
		case WSANOTINITIALISED: print_to_debug("WSA error : WSANOTINITIALISED"); break;
		case WSAEINTR: print_to_debug("WSA error : WSAEINTR"); break;
		case WSAEBADF: print_to_debug("WSA error : WSAEBADF"); break;
		case WSAEACCES: print_to_debug("WSA error : WSAEACCES"); break;
		case WSAEFAULT: print_to_debug("WSA error : WSAEFAULT"); break;
		case WSAEINVAL: print_to_debug("WSA error : WSAEINVAL"); break;
		case WSAEMFILE: print_to_debug("WSA error : WSAEMFILE"); break;
		case WSAEWOULDBLOCK: print_to_debug("WSA error : WSAEWOULDBLOCK"); break;
		case WSAEINPROGRESS: print_to_debug("WSA error : WSAEINPROGRESS"); break;
		case WSAEALREADY: print_to_debug("WSA error : WSAEALREADY"); break;
		case WSAENOTSOCK: print_to_debug("WSA error : WSAENOTSOCK"); break;
		case WSAEDESTADDRREQ: print_to_debug("WSA error : WSAEDESTADDRREQ"); break;
		case WSAEMSGSIZE: print_to_debug("WSA error : WSAEMSGSIZE"); break;
		case WSAEPROTOTYPE: print_to_debug("WSA error : WSAEPROTOTYPE"); break;
		case WSAENOPROTOOPT: print_to_debug("WSA error : WSAENOPROTOOPT"); break;
		case WSAEPROTONOSUPPORT: print_to_debug("WSA error : WSAEPROTONOSUPPORT"); break;
		case WSAESOCKTNOSUPPORT: print_to_debug("WSA error : WSAESOCKTNOSUPPORT"); break;
		case WSAEOPNOTSUPP: print_to_debug("WSA error : WSAEOPNOTSUPP"); break;
		case WSAEPFNOSUPPORT: print_to_debug("WSA error : WSAEPFNOSUPPORT"); break;
		case WSAEAFNOSUPPORT: print_to_debug("WSA error : WSAEAFNOSUPPORT"); break;
		case WSAEADDRINUSE: print_to_debug("WSA error : WSAEADDRINUSE"); break;
		case WSAEADDRNOTAVAIL: print_to_debug("WSA error : WSAEADDRNOTAVAIL"); break;
		case WSAENETDOWN: print_to_debug("WSA error : WSAENETDOWN"); break;
		case WSAENETUNREACH: print_to_debug("WSA error : WSAENETUNREACH"); break;
		case WSAENETRESET: print_to_debug("WSA error : WSAENETRESET"); break;
		case WSAECONNABORTED: print_to_debug("WSA error : WSAECONNABORTED"); break;
		case WSAECONNRESET: print_to_debug("WSA error : WSAECONNRESET"); break;
		case WSAENOBUFS: print_to_debug("WSA error : WSAENOBUFS"); break;
		case WSAEISCONN: print_to_debug("WSA error : WSAEISCONN"); break;
		case WSAENOTCONN: print_to_debug("WSA error : WSAENOTCONN"); break;
		case WSAESHUTDOWN: print_to_debug("WSA error : WSAESHUTDOWN"); break;
		case WSAETOOMANYREFS: print_to_debug("WSA error : WSAETOOMANYREFS"); break;
		case WSAETIMEDOUT: print_to_debug("WSA error : WSAETIMEDOUT"); break;
		case WSAECONNREFUSED: print_to_debug("WSA error : WSAECONNREFUSED"); break;
		case WSAELOOP: print_to_debug("WSA error : WSAELOOP"); break;
		case WSAENAMETOOLONG: print_to_debug("WSA error : WSAENAMETOOLONG"); break;
		case WSAEHOSTDOWN: print_to_debug("WSA error : WSAEHOSTDOWN"); break;
		case WSAEHOSTUNREACH: print_to_debug("WSA error : WSAEHOSTUNREACH"); break;
		case WSAENOTEMPTY: print_to_debug("WSA error : WSAENOTEMPTY"); break;
		case WSAEPROCLIM: print_to_debug("WSA error : WSAEPROCLIM"); break;
		case WSAEUSERS: print_to_debug("WSA error : WSAEUSERS"); break;
		case WSAEDQUOT: print_to_debug("WSA error : WSAEDQUOT"); break;
		case WSAESTALE: print_to_debug("WSA error : WSAESTALE"); break;
		case WSAEREMOTE: print_to_debug("WSA error : WSAEREMOTE"); break;
		case WSAEDISCON: print_to_debug("WSA error : WSAEDISCON"); break;
		default: print_to_debug("WSA error : Unknown"); break;
	}
}



#endif