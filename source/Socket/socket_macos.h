#ifndef PLATFORM_SOCKET_H
#define PLATFORM_SOCKET_H

#include "../GF_GlobalTypes.h"

#ifdef AE_OS_MAC
#include "socket_interface.h"

class platform_socket : public SocketClientInterface
{
public:
	platform_socket();
	~platform_socket();
	bool init_interface() override;
	bool create_socket() override;
	void close_socket() override;
	bool connect(unsigned short port) override;
	[[nodiscard]] bool is_connected() const override;
	[[nodiscard]] unsigned long bytes_available() const override;
	unsigned long write(const char *data, unsigned long data_length) override;
	unsigned long read(char *data, unsigned long max_length) override;

protected:
	void print_to_debug(const std::string &message, bool add_end_line=true) const override;
	void print_error_string(const std::string &caller_name, int error_id) const override;
	void print_error_string(int error_id) const override;
};
#endif

#endif
