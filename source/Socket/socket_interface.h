#ifndef SOCKETCLIENTINTERFACE_HPP
#define SOCKETCLIENTINTERFACE_HPP
#include <string>

enum SocketState {
	Error=-1,
	Unconnected,
	Connecting,
	Connected
};

class SocketClientInterface
{
public:
	SocketClientInterface(SocketClientInterface &p) = delete;
	SocketClientInterface(SocketClientInterface &&p) = delete;
	SocketClientInterface& operator=(SocketClientInterface &p) = delete;
	SocketClientInterface& operator=(SocketClientInterface &&p) = delete;

	SocketClientInterface() = default;
	virtual ~SocketClientInterface() = default;
	virtual bool init_interface() = 0;
	virtual bool create_socket() = 0;
	virtual void close_socket() = 0;
	virtual bool connect(unsigned short port) = 0;
	virtual bool is_connected() const = 0;
	[[nodiscard]] virtual unsigned long bytes_available() const = 0;
	virtual unsigned long write(const char *data, unsigned long data_length) = 0;
	virtual unsigned long read(char *data, unsigned long max_length) = 0;
	
protected:
	virtual void print_to_debug(const std::string &message, const std::string &caller_name, bool error) const = 0;
	virtual void print_error_string(int error_id, const std::string &caller_name) const = 0;
	long long socket_descriptor_ = 0;
	int socket_state_ = 0;
};

#endif