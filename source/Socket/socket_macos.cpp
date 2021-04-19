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

platform_socket::platform_socket() : SocketClientInterface()
{
}
platform_socket::~platform_socket()
{
}
bool platform_socket::init_interface()
{
	return true;
}
bool platform_socket::create_socket()
{
	if(init_interface() == false) {
		socket_state_ = Error;
		return false;
	}
		
	int protocol = AF_INET;
	int type = SOCK_STREAM;

	int socket = qt_safe_socket(protocol, type, 0);

	if (socket <= 0)
	{
		print_error_string("Socket Create Socket", errno);
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

	socket_descriptor_ = static_cast<long long>(socket);
	socket_state_ = Unconnected;
	return true;
}

void platform_socket::close_socket()
{	
	::closesocket(socket_descriptor_);
	socket_state_ = Unconnected;
}

bool platform_socket::connect(unsigned short port)
{
	sockaddr_in socket_address_in_v4;

	memset(&socket_address_in_v4, 0, sizeof(sockaddr_in));
	socket_address_in_v4.sin_family = AF_INET;
	socket_address_in_v4.sin_port = htons(port);
	socket_address_in_v4.sin_addr.s_addr = htonl(2130706433);	
	auto *socket_address_ptr = reinterpret_cast<struct sockaddr*>( &socket_address_in_v4);

	int connectResult = qt_safe_connect(socket_descriptor_, socket_address_ptr, sizeof(sockaddr_in));
	if (connectResult == -1)
	{
		print_error_string("Socket Connect", errno);
		switch (errno)
		{
			case EISCONN:
				socket_state_ = Connected;
				break;
			case EINPROGRESS:
			case EALREADY:				
				socket_state_ = Connecting;
				break;
			case ECONNREFUSED:
			case EINVAL:
			case ETIMEDOUT:
			case EHOSTUNREACH:
			case ENETUNREACH:
			case EADDRINUSE:
			case EAGAIN:
			case EACCES:
			case EPERM:
			case EAFNOSUPPORT:
			case EBADF:
			case EFAULT:
			case ENOTSOCK:
			default:
				socket_state_ = Unconnected;
				break;
		}

		if (socket_state_ != Connected) {
			socket_state_ = Error;
			return false;
		}
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
	int nbytes = 0;
	unsigned long available = 0;	
	if (::ioctl(socket_descriptor_, FIONREAD, (char *) &nbytes) >= 0)
		available = (qint64) nbytes;

	return available;
}
unsigned long platform_socket::write(const char *data, const unsigned long data_length)
{
	ssize_t data_sent = 0;
	do {
        data_sent = write(socket_descriptor_, data, data_length);
    } while (data_sent == -1 && errno == EINTR)

	if (data_sent < 0)
	{
		data_sent = -1;
		print_error_string("Socket Write", errno);
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
	struct timeval tv = { 2, 0 };
	fd_set fds;
	ssize_t bytes_read = 0;
	FD_ZERO(&fds);
    FD_SET(socket_descriptor_, &fds);
    
    if(::select(0, &fds, nullptr, nullptr, &tv) == 0)
	{
		return 0;
	}
	bzero(data, max_length);

	do {
		bytes_read = read(socket_descriptor_, data, max_length);
	} while (bytes_read == -1 && errno == EINTR)

	if (bytes_read < 0)
	{
		bytes_read = -1;
		print_error_string("Socket Read", errno);
		switch (errno)
		{
			case ECONNRESET:
				bytes_read = 0;
				break;
			case EBADF:
			case EINVAL:
			case EIO:
			case EAGAIN:			// No data was available for reading			
			default:
				break;
		}
	}
	return static_cast<unsigned long>(bytes_read);
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
		case EISCONN: print_to_debug("Socket error: EISCONN"); break;
		case EINPROGRESS: print_to_debug("Socket error: EINPROGRESS"); break;
		case EALREADY: print_to_debug("Socket error: EALREADY"); break;
		case ECONNREFUSED: print_to_debug("Socket error: ECONNREFUSED"); break;
		case EINVAL: print_to_debug("Socket error: EINVAL"); break;
		case ETIMEDOUT: print_to_debug("Socket error: ETIMEDOUT"); break;
		case EHOSTUNREACH: print_to_debug("Socket error: EHOSTUNREACH"); break;
		case ENETUNREACH: print_to_debug("Socket error: ENETUNREACH"); break;
		case EADDRINUSE: print_to_debug("Socket error: EADDRINUSE"); break;
		case EAGAIN: print_to_debug("Socket error: EAGAIN"); break;
		case EACCES: print_to_debug("Socket error: EACCES"); break;
		case EPERM: print_to_debug("Socket error: EPERM"); break;
		case EAFNOSUPPORT: print_to_debug("Socket error: EAFNOSUPPORT"); break;
		case EBADF:	 print_to_debug("Socket error: EBADF"); break;
		case EFAULT: print_to_debug("Socket error: EFAULT"); break;
		case ENOTSOCK: print_to_debug("Socket error: ENOTSOCK"); break;
		case EPROTONOSUPPORT: print_to_debug("Socket error: EPROTONOSUPPORT"); break;
		case ENFILE: print_to_debug("Socket error: ENFILE"); break;
		case EMFILE: print_to_debug("Socket error: EMFILE"); break;
		case ENOBUFS: print_to_debug("Socket error: ENOBUFS"); break;
		case ENOMEM: print_to_debug("Socket error: ENOMEM"); break;
		case EPIPE: print_to_debug("Socket error: EPIPE"); break;
		case ECONNRESET: print_to_debug("Socket error: ECONNRESET"); break;
		case EMSGSIZE: print_to_debug("Socket error: EMSGSIZE"); break;
		case EIO: print_to_debug("Socket error: EIO"); break;
		case EAGAIN: print_to_debug("Socket error: EAGAIN"); break;
		default: print_to_debug("Socket error: UNKNOWN"); break;	  
	}
}

#endif