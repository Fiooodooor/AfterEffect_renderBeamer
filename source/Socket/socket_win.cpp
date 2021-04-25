
#include "socket_win.h"

#ifdef AE_OS_WIN
#include <WinSock2.h>
#include <WS2tcpip.h>
#include "../GF_AEGP_Dumper.h"

platform_socket::platform_socket() : SocketClientInterface()
{
}

platform_socket::~platform_socket()
{
	WSACleanup();
}

long platform_socket::start_session(long port, const std::string &scene_name)
{
	ERROR_CATCH_START
		ERROR_BOOL_ERR(init_interface())
		ERROR_BOOL_ERR(create_socket())
		ERROR_BOOL_ERR(connect_socket(static_cast<unsigned short>(port)))
		if(_ErrorCode == NoError && is_connected())
		{
			const auto welcome_header = "SETUP=AE\tNAME=" + scene_name + "\n";
			ERROR_BOOL_ERR( write(welcome_header.c_str(), static_cast<unsigned long>(welcome_header.length())) > 0 )
		}
		else
			close_socket();
	ERROR_CATCH_END_NO_INFO_RETURN
}

bool platform_socket::init_interface()
{
	print_to_debug("Initiating interface for socket. ", "platform_socket::init_interface", false);
	WSAData wsa_data = { 0, 0, 0, 0, nullptr, { 0 }, { 0 } };
	if (WSAStartup(MAKEWORD(2, 0), &wsa_data) != 0) {
		print_to_debug("Failed to init the interface!", "platform_socket::init_interface");
		socket_state_ = Error;
		return false;
	}
	socket_descriptor_ = INVALID_SOCKET;
	return true;
}
bool platform_socket::create_socket()
{
	print_to_debug("Creating socket.", "platform_socket::create_socket", false);
	const auto win_socket = ::socket(AF_INET, 0, IPPROTO_TCP);
	
	if (win_socket == INVALID_SOCKET) {
		print_error_string(WSAGetLastError(), "::socket");
		socket_state_ = Error;
		return false;
	}
	
	socket_descriptor_ = static_cast<long long>(win_socket);
	socket_state_ = Unconnected;
	return true;
}
long platform_socket::close_socket()
{
	print_to_debug("Closing socket.", "platform_socket::close_socket", false);
	if(::closesocket(socket_descriptor_) == SOCKET_ERROR)
		print_error_string(WSAGetLastError(), "::closesocket");

	socket_descriptor_ = INVALID_SOCKET;
	socket_state_ = Unconnected;
	return 0;
}
bool platform_socket::connect_socket(const unsigned short port)
{
	print_to_debug("Connect called.", "platform_socket::connect", false);
	DWORD socket_timeout_ms = Socket_Read_Timeout;
	sockaddr_in socket_address_in_v4{};
	memset(&socket_address_in_v4, 0, sizeof(sockaddr_in));
	
	socket_address_in_v4.sin_family = AF_INET;
	socket_address_in_v4.sin_addr.s_addr = inet_addr("127.0.0.1");  // 16777343
	socket_address_in_v4.sin_port = htons(port);
	auto *socket_address_ptr = reinterpret_cast<SOCKADDR*>(&socket_address_in_v4);
	if (socket_address_ptr == nullptr)
		return false;

	const auto connection_result = ::connect(socket_descriptor_, socket_address_ptr, sizeof(socket_address_in_v4));
	if (connection_result == SOCKET_ERROR)
	{
		const auto err = WSAGetLastError();
		print_error_string(err, "::connect");
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
		{
			socket_state_ = Error;
			return false;
		}
	}
	
	const auto set_result = setsockopt(socket_descriptor_, SOL_SOCKET, SO_RCVTIMEO, reinterpret_cast<char*>(&socket_timeout_ms), sizeof(DWORD));
	if (set_result == SOCKET_ERROR) {
		print_error_string(WSAGetLastError(), "setsockopt");
		return false;
	}
	print_to_debug("Set recive timeout success. SO_RCVTIMEO: ON", "setsockopt", false);

	socket_state_ = Connected;
	return true;
}
bool platform_socket::is_connected() const
{
	return socket_state_ == Connected;
}

unsigned long platform_socket::bytes_available() const
{
	print_to_debug("CHECK: Bytes available from renderbeamer.", "platform_socket::bytes_available", false);
	if (!is_connected())
		return 0;
	unsigned long  bytes_to_read = 0;
	unsigned long dummy = 0;
	DWORD sizeWritten = 0;
	if (::WSAIoctl(socket_descriptor_, FIONREAD, &dummy, sizeof(dummy), &bytes_to_read, sizeof(bytes_to_read), &sizeWritten, nullptr, nullptr) == SOCKET_ERROR) {
		print_error_string(WSAGetLastError(), "::WSAIoctl");
		return 0;
	}

	return bytes_to_read;
}
unsigned long platform_socket::write(const char *data, const unsigned long data_length)
{
	print_to_debug("WRITE: Sending data to renderbeamer.", "platform_socket::write", false);
	unsigned long data_sent = 0;
	const unsigned long max_size = 49152;
	auto bytes_to_send = data_length;

	if (!is_connected())
		return 0;

	for (;;) {
		const auto socket_return_code = ::send(socket_descriptor_, data+ data_sent, bytes_to_send, 0);
		
		if (socket_return_code != SOCKET_ERROR) 
		{
			data_sent += socket_return_code;
			if (data_sent == data_length)
				break;
			continue;
		}
		const auto err = WSAGetLastError();
		if (err == WSAENOBUFS) { // max 49152 per call to WSASendTo
		}
		else {
			print_error_string(err, "::send");
			switch (err) {
				case WSAECONNRESET:
				case WSAECONNABORTED:
					data_sent = 0;					
					close_socket();
					break;
				default:
					break;
			}
			break;
		}
		bytes_to_send = max_size <= (data_length - data_sent) ? max_size : (data_length - data_sent);
	}
	return data_sent;
}

unsigned long platform_socket::read(char *data, const unsigned long max_length)
{
	print_to_debug("READ: Waiting for data from renderbeamer.", "platform_socket::read", false);
	unsigned long bytes_read = 0;
	if (!is_connected())
		return 0;
	
	const auto return_val = ::recv(socket_descriptor_, data, static_cast<int>(max_length) - 1, 0);
	if (return_val <= 0)
	{
		const auto err_nr = WSAGetLastError();
		bytes_read = 0;
		WSASetLastError(0);
		if (err_nr != WSAETIMEDOUT) {
			print_error_string(err_nr, "::recv");
		}
	}
	else {		
		bytes_read = return_val;
		data[bytes_read] = '\0';
	}
	return bytes_read;
}

void platform_socket::print_to_debug(const std::string &message, const std::string &caller_name, bool error) const
{
	if(error)
		GF_Dumper::rbProj()->loggErr("Socket", caller_name.c_str(), message.c_str());
	else
		GF_Dumper::rbProj()->logg("Socket", caller_name.c_str(), message.c_str());
}

void platform_socket::print_error_string(const int error_id, const std::string &caller_name) const
{
	switch (error_id)
	{
		case WSANOTINITIALISED: print_to_debug("WSA error : WSANOTINITIALISED", caller_name); break;
		case WSAEINTR: print_to_debug("WSA error : WSAEINTR", caller_name); break;
		case WSAEBADF: print_to_debug("WSA error : WSAEBADF", caller_name); break;
		case WSAEACCES: print_to_debug("WSA error : WSAEACCES", caller_name); break;
		case WSAEFAULT: print_to_debug("WSA error : WSAEFAULT", caller_name); break;
		case WSAEINVAL: print_to_debug("WSA error : WSAEINVAL", caller_name); break;
		case WSAEMFILE: print_to_debug("WSA error : WSAEMFILE", caller_name); break;
		case WSAEWOULDBLOCK: print_to_debug("WSA error : WSAEWOULDBLOCK", caller_name); break;
		case WSAEINPROGRESS: print_to_debug("WSA error : WSAEINPROGRESS", caller_name); break;
		case WSAEALREADY: print_to_debug("WSA error : WSAEALREADY", caller_name); break;
		case WSAENOTSOCK: print_to_debug("WSA error : WSAENOTSOCK", caller_name); break;
		case WSAEDESTADDRREQ: print_to_debug("WSA error : WSAEDESTADDRREQ", caller_name); break;
		case WSAEMSGSIZE: print_to_debug("WSA error : WSAEMSGSIZE", caller_name); break;
		case WSAEPROTOTYPE: print_to_debug("WSA error : WSAEPROTOTYPE", caller_name); break;
		case WSAENOPROTOOPT: print_to_debug("WSA error : WSAENOPROTOOPT", caller_name); break;
		case WSAEPROTONOSUPPORT: print_to_debug("WSA error : WSAEPROTONOSUPPORT", caller_name); break;
		case WSAESOCKTNOSUPPORT: print_to_debug("WSA error : WSAESOCKTNOSUPPORT", caller_name); break;
		case WSAEOPNOTSUPP: print_to_debug("WSA error : WSAEOPNOTSUPP", caller_name); break;
		case WSAEPFNOSUPPORT: print_to_debug("WSA error : WSAEPFNOSUPPORT", caller_name); break;
		case WSAEAFNOSUPPORT: print_to_debug("WSA error : WSAEAFNOSUPPORT", caller_name); break;
		case WSAEADDRINUSE: print_to_debug("WSA error : WSAEADDRINUSE", caller_name); break;
		case WSAEADDRNOTAVAIL: print_to_debug("WSA error : WSAEADDRNOTAVAIL", caller_name); break;
		case WSAENETDOWN: print_to_debug("WSA error : WSAENETDOWN", caller_name); break;
		case WSAENETUNREACH: print_to_debug("WSA error : WSAENETUNREACH", caller_name); break;
		case WSAENETRESET: print_to_debug("WSA error : WSAENETRESET", caller_name); break;
		case WSAECONNABORTED: print_to_debug("WSA error : WSAECONNABORTED", caller_name); break;
		case WSAECONNRESET: print_to_debug("WSA error : WSAECONNRESET", caller_name); break;
		case WSAENOBUFS: print_to_debug("WSA error : WSAENOBUFS", caller_name); break;
		case WSAEISCONN: print_to_debug("WSA error : WSAEISCONN", caller_name); break;
		case WSAENOTCONN: print_to_debug("WSA error : WSAENOTCONN", caller_name); break;
		case WSAESHUTDOWN: print_to_debug("WSA error : WSAESHUTDOWN", caller_name); break;
		case WSAETOOMANYREFS: print_to_debug("WSA error : WSAETOOMANYREFS", caller_name); break;
		case WSAETIMEDOUT: print_to_debug("WSA error : WSAETIMEDOUT", caller_name); break;
		case WSAECONNREFUSED: print_to_debug("WSA error : WSAECONNREFUSED", caller_name); break;
		case WSAELOOP: print_to_debug("WSA error : WSAELOOP", caller_name); break;
		case WSAENAMETOOLONG: print_to_debug("WSA error : WSAENAMETOOLONG", caller_name); break;
		case WSAEHOSTDOWN: print_to_debug("WSA error : WSAEHOSTDOWN", caller_name); break;
		case WSAEHOSTUNREACH: print_to_debug("WSA error : WSAEHOSTUNREACH", caller_name); break;
		case WSAENOTEMPTY: print_to_debug("WSA error : WSAENOTEMPTY", caller_name); break;
		case WSAEPROCLIM: print_to_debug("WSA error : WSAEPROCLIM", caller_name); break;
		case WSAEUSERS: print_to_debug("WSA error : WSAEUSERS", caller_name); break;
		case WSAEDQUOT: print_to_debug("WSA error : WSAEDQUOT", caller_name); break;
		case WSAESTALE: print_to_debug("WSA error : WSAESTALE", caller_name); break;
		case WSAEREMOTE: print_to_debug("WSA error : WSAEREMOTE", caller_name); break;
		case WSAEDISCON: print_to_debug("WSA error : WSAEDISCON", caller_name); break;
		default: print_to_debug("WSA error : Unknown", caller_name); break;
	}
}



#endif