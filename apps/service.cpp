#include <cerrno>
#include <csignal>
#include <cstring>
#include <iostream>
#include <sstream>
#include <string>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include "edgeinfer/runtime.hpp"

namespace {
volatile std::sig_atomic_t g_running = 1;

void handle_signal(int) { g_running = 0; }

bool parse_input(const std::string& text, edgeinfer::Tensor& tensor) {
    std::istringstream stream(text);
    float value = 0.0F;
    tensor.data.clear();
    while (stream >> value) tensor.data.push_back(value);
    return !tensor.data.empty();
}

std::string format_output(const edgeinfer::Tensor& tensor) {
    std::ostringstream stream;
    for (std::size_t i = 0; i < tensor.data.size(); ++i) {
        if (i != 0) stream << " ";
        stream << tensor.data[i];
    }
    stream << "\n";
    return stream.str();
}
}

int main(int argc, char** argv) {
    if (argc < 2 || argc > 3) {
        std::cerr << "Usage: " << argv[0] << " MODEL [SOCKET_PATH]\n";
        return 2;
    }

    const std::string model_path = argv[1];
    const std::string socket_path = argc == 3 ? argv[2] : "/tmp/edgeinfer.sock";

    edgeinfer::Runtime runtime;
    const auto init = runtime.initialize(model_path, "sim-npu", true);
    if (!init.is_ok()) {
        std::cerr << "Initialization failed: " << init.message() << "\n";
        return 1;
    }

    std::signal(SIGINT, handle_signal);
    std::signal(SIGTERM, handle_signal);

    const int server_fd = ::socket(AF_UNIX, SOCK_STREAM, 0);
    if (server_fd < 0) {
        std::cerr << "socket failed: " << std::strerror(errno) << "\n";
        return 1;
    }

    ::unlink(socket_path.c_str());

    struct sockaddr_un address {};
    address.sun_family = AF_UNIX;

    if (socket_path.size() >= sizeof(address.sun_path)) {
        std::cerr << "socket path too long\n";
        ::close(server_fd);
        return 1;
    }

    std::strncpy(address.sun_path, socket_path.c_str(), sizeof(address.sun_path) - 1);

    if (::bind(server_fd, reinterpret_cast<struct sockaddr*>(&address), sizeof(address)) != 0) {
        std::cerr << "bind failed: " << std::strerror(errno) << "\n";
        ::close(server_fd);
        return 1;
    }

    if (::listen(server_fd, 8) != 0) {
        std::cerr << "listen failed: " << std::strerror(errno) << "\n";
        ::close(server_fd);
        ::unlink(socket_path.c_str());
        return 1;
    }

    std::cout << "EdgeInfer service listening on " << socket_path << "\n";

    while (g_running != 0) {
        const int client = ::accept(server_fd, nullptr, nullptr);
        if (client < 0) {
            if (errno == EINTR) continue;
            break;
        }

        char buffer[1024] {};
        const ssize_t count = ::read(client, buffer, sizeof(buffer) - 1);
        std::string response;

        if (count <= 0) {
            response = "ERROR empty request\n";
        } else {
            edgeinfer::Tensor input;
            if (!parse_input(std::string(buffer, static_cast<std::size_t>(count)), input)) {
                response = "ERROR invalid input\n";
            } else {
                edgeinfer::Tensor output;
                const auto status = runtime.infer(input, output);
                response = status.is_ok()
                    ? format_output(output)
                    : "ERROR " + status.message() + "\n";
            }
        }

        const auto ignored = ::write(client, response.data(), response.size());
        (void)ignored;
        ::close(client);
    }

    ::close(server_fd);
    ::unlink(socket_path.c_str());
    return 0;
}
