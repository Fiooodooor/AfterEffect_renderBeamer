#include "socket_macos.h"

#ifdef AE_OS_MAC
#include <time.h>
#include <errno.h>
#include <fcntl.h>
#include <net/if.h>
#include <arpa/inet.h>
#include <ctype.h>
#include <netinet/tcp.h>
#include <iostream>
#include "../GF_AEGP_Dumper.h"
#include <sys/types.h>     
#include <sys/socket.h>

platform_socket::platform_socket() : SocketClientInterface()
{
}
platform_socket::~platform_socket()
{
	close_socket();
}

long platform_socket::start_session(long port, const std::string &scene_name)
{
	ERROR_CATCH_START
		ERROR_BOOL_ERR(init_interface())
		ERROR_BOOL_ERR(create_socket())
		ERROR_BOOL_ERR(connect_socket(static_cast<unsigned short>(port)))
		if (_ErrorCode == NoError && is_connected())
		{
			const auto welcome_header = "SETUP=AE\tNAME=" + scene_name + "\n";
			ERROR_BOOL_ERR(write(welcome_header.c_str(), static_cast<unsigned long>(welcome_header.length())) > 0)
		}
		else
			close_socket();
	ERROR_CATCH_END_NO_INFO_RETURN
}

bool platform_socket::init_interface()
{
	print_to_debug("Initiating interface for socket. ", "platform_socket::init_interface", false);
	socket_descriptor_ = INVALID_SOCKET;
	return true;
}
bool platform_socket::create_socket()
{	
	print_to_debug("Creating socket.", "platform_socket::create_socket", false);
	auto mac_socket = ::socket(AF_INET, 0, IPPROTO_TCP);

	if (mac_socket <= 0)
	{
		print_error_string(errno, "::socket");
		switch (errno)
		{
			case EPROTONOSUPPORT:
			case EAFNOSUPPORT:
			case EINVAL:
				print_error_string(ProtocolUnsupportedErrorString);
				break;
			case ENFILE:
			case EMFILE:
			case ENOBUFS:
			case ENOMEM:
				print_error_string(ResourceErrorString);
				break;
			case EACCES:
				print_error_string(AccessErrorString);
				break;
			default:
				print_error_string(UnknownSocketErrorString);
				break;
		}
		socket_state_ = Error;
		return false;
	}

	socket_descriptor_ = static_cast<long long>(mac_socket);
	socket_state_ = Unconnected;
	return true;
}

void platform_socket::close_socket()
{
	print_to_debug("Closing socket.", "platform_socket::close_socket", false);
	const std::string quit_msg = "QUIT\n";
	write(quit_msg.c_str(), static_cast<unsigned long>(quit_msg.length()));
	if(socket_descriptor_)
		::closesocket(socket_descriptor_);
	
	socket_descriptor_ = INVALID_SOCKET;
	socket_state_ = Unconnected;
	return 0;
}

bool platform_socket::connect_socket(unsigned short port)
{
	print_to_debug("Connect called.", "platform_socket::connect", false);

	struct timeval tv;
	tv.tv_usec = Socket_Read_Timeout;
	
	sockaddr_in socket_address_in_v4;
	memset(&socket_address_in_v4, 0, sizeof(sockaddr_in));

	socket_address_in_v4.sin_family = AF_INET;
	socket_address_in_v4.sin_addr.s_addr = inet_addr("127.0.0.1"); // <==> inet_addr("127.0.0.1");
	socket_address_in_v4.sin_port = htons(port);
	auto *socket_address_ptr = reinterpret_cast<SOCKADDR*>(&socket_address_in_v4);
	if (socket_address_ptr == nullptr)
		return false;
	
	int connectResult;	
	do {
		connectResult = ::connect(socket_descriptor_, socket_address_ptr, sizeof(sockaddr_in))
	} while (connectResult == -1 && errno == EINTR)
	
	if (connectResult == -1)
	{
		print_error_string(errno, "::connect");
		switch (errno)
		{
			case EISCONN:
				socket_state_ = Connected;
				break;
			case EINPROGRESS:
			case EALREADY:				
				socket_state_ = Connecting;
				break;
			default:
				socket_state_ = Unconnected;
				break;
		}

		if (socket_state_ != Connected) {
			socket_state_ = Error;
			return false;
		}
	}
	
	const auto set_result = setsockopt(socket_descriptor_, SOL_SOCKET, SO_RCVTIMEO, (struct timeval *)&tv, sizeof(struct timeval));
	if (set_result == SOCKET_ERROR) {
		print_error_string(errno, "setsockopt");
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
	int nbytes = 0;
	unsigned long available = 0;	
	if (::ioctl(socket_descriptor_, FIONREAD, (char *) &nbytes) >= 0)
		available = (qint64) nbytes;
	else {
		print_error_string(errno, "::ioctl");
		return 0;
	}
	return available;
}

unsigned long platform_socket::write(const char *data, const unsigned long data_length)
{
	print_to_debug("WRITE: Sending data to renderbeamer.", "platform_socket::write", false);
	ssize_t data_sent = 0;
	if (!is_connected())
		return 0;
	
	do {
        data_sent = write(socket_descriptor_, data, data_length);
    } while (data_sent == -1 && errno == EINTR)

	if (data_sent < 0)
	{
		data_sent = 0;
		print_error_string(errno, "Socket Write");
		switch (errno)
		{			
			case EAGAIN:
				data_sent = 0;
				break;
			case EPIPE:
			case ECONNRESET:
				close_socket(); 
			case EMSGSIZE:
			default:
				break;
		}
	}
	return static_cast<unsigned long>(data_sent);
}

unsigned long platform_socket::read(char *data, const unsigned long max_length)
{
	ssize_t bytes_read = 0;
	if (!is_connected())
		return 0;
	
	do {
		bytes_read = read(socket_descriptor_, data, max_length-1);
	} while (bytes_read == -1 && errno == EINTR)

	if (bytes_read < 0)
	{
		bytes_read = 0;
		if(errno != ETIMEDOUT)
			print_error_string(errno, "read");
	}
	else
	{
		print_to_debug("READ: Waiting for data from renderbeamer.", "platform_socket::read", false);
		data[bytes_read] = '\0';
	}
	return static_cast<unsigned long>(bytes_read);
}

vvoid platform_socket::print_to_debug(const std::string &message, const std::string &caller_name, bool error) const
{
	if (error)
		GF_Dumper::rbProj()->loggErr("Socket", caller_name.c_str(), message.c_str());
	else
		GF_Dumper::rbProj()->logg("Socket", caller_name.c_str(), message.c_str());
}

void platform_socket::print_error_string(const int error_id, const std::string &caller_name) const
{
	switch (error_id)
	{
		case EISCONN: print_to_debug("Socket error: EISCONN", caller_name); break;
		case EINPROGRESS: print_to_debug("Socket error: EINPROGRESS", caller_name); break;
		case EALREADY: print_to_debug("Socket error: EALREADY", caller_name); break;
		case ECONNREFUSED: print_to_debug("Socket error: ECONNREFUSED", caller_name); break;
		case EINVAL: print_to_debug("Socket error: EINVAL", caller_name); break;
		case ETIMEDOUT: print_to_debug("Socket error: ETIMEDOUT", caller_name); break;
		case EHOSTUNREACH: print_to_debug("Socket error: EHOSTUNREACH", caller_name); break;
		case ENETUNREACH: print_to_debug("Socket error: ENETUNREACH", caller_name); break;
		case EADDRINUSE: print_to_debug("Socket error: EADDRINUSE", caller_name); break;
		case EAGAIN: print_to_debug("Socket error: EAGAIN", caller_name); break;
		case EACCES: print_to_debug("Socket error: EACCES", caller_name); break;
		case EPERM: print_to_debug("Socket error: EPERM", caller_name); break;
		case EAFNOSUPPORT: print_to_debug("Socket error: EAFNOSUPPORT", caller_name); break;
		case EBADF:	 print_to_debug("Socket error: EBADF", caller_name); break;
		case EFAULT: print_to_debug("Socket error: EFAULT", caller_name); break;
		case ENOTSOCK: print_to_debug("Socket error: ENOTSOCK", caller_name); break;
		case EPROTONOSUPPORT: print_to_debug("Socket error: EPROTONOSUPPORT", caller_name); break;
		case ENFILE: print_to_debug("Socket error: ENFILE", caller_name); break;
		case EMFILE: print_to_debug("Socket error: EMFILE", caller_name); break;
		case ENOBUFS: print_to_debug("Socket error: ENOBUFS", caller_name); break;
		case ENOMEM: print_to_debug("Socket error: ENOMEM", caller_name); break;
		case EPIPE: print_to_debug("Socket error: EPIPE", caller_name); break;
		case ECONNRESET: print_to_debug("Socket error: ECONNRESET", caller_name); break;
		case EMSGSIZE: print_to_debug("Socket error: EMSGSIZE", caller_name); break;
		case EIO: print_to_debug("Socket error: EIO", caller_name); break;
		case EAGAIN: print_to_debug("Socket error: EAGAIN", caller_name); break;
		default: print_to_debug("Socket error: UNKNOWN", caller_name); break;	  
	}
}

#endif