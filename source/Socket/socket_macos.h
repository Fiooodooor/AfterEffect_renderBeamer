#ifndef PLATFORM_SOCKET_H
#define PLATFORM_SOCKET_H

#include "../GF_GlobalTypes.h"

#ifdef AE_OS_MAC
#include "socket_interface.h"

namespace RenderBeamer {

class platform_socket final : public SocketClientInterface
{
public:
    platform_socket(platform_socket &p) = delete;
    platform_socket(platform_socket &&p) = delete;
    platform_socket& operator=(platform_socket &p) = delete;
    platform_socket& operator=(platform_socket &&p) = delete;

    platform_socket();
    ~platform_socket();
    long start_session(long port, const std::string &scene_name) override;
    bool init_interface() override;
    bool create_socket() override;
    long close_socket() override;
    bool connect_socket(unsigned short port) override;
    [[nodiscard]] bool is_connected() const override;
    [[nodiscard]] unsigned long bytes_available() override;
    unsigned long write(const char *data, unsigned long data_length) override;
    unsigned long read(char *data, unsigned long max_length) override;

protected:
    void print_to_debug(const std::string &message, const std::string &caller_name, bool error=true) const override;
    void print_error_string(int error_id, const std::string &caller_name) const override;
};

} // namespace RenderBeamer
#endif

#endif
