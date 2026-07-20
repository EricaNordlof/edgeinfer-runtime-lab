#include "edgeinfer/model.hpp"
#include <cerrno>
#include <cstring>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

namespace edgeinfer {
namespace {
constexpr char kMagic[4] = {'E','I','R','1'};

template <typename T>
bool read_value(const unsigned char*& cursor, const unsigned char* end, T& out) {
    if (static_cast<std::size_t>(end - cursor) < sizeof(T)) return false;
    std::memcpy(&out, cursor, sizeof(T));
    cursor += sizeof(T);
    return true;
}

bool read_floats(const unsigned char*& cursor,
                 const unsigned char* end,
                 std::vector<float>& out,
                 std::size_t count) {
    const std::size_t bytes = count * sizeof(float);
    if (static_cast<std::size_t>(end - cursor) < bytes) return false;
    out.resize(count);
    std::memcpy(out.data(), cursor, bytes);
    cursor += bytes;
    return true;
}
}

Status ModelLoader::load_mmap(const std::string& path, TinyModel& model) {
    const int fd = ::open(path.c_str(), O_RDONLY);
    if (fd < 0) {
        return {StatusCode::IoError, "open failed: " + std::string(std::strerror(errno))};
    }

    struct stat st {};
    if (::fstat(fd, &st) != 0) {
        const std::string msg = std::strerror(errno);
        ::close(fd);
        return {StatusCode::IoError, "fstat failed: " + msg};
    }

    if (st.st_size < 16) {
        ::close(fd);
        return {StatusCode::InvalidModel, "model file too small"};
    }

    void* mapping = ::mmap(nullptr, static_cast<std::size_t>(st.st_size),
                           PROT_READ, MAP_PRIVATE, fd, 0);
    if (mapping == MAP_FAILED) {
        const std::string msg = std::strerror(errno);
        ::close(fd);
        return {StatusCode::IoError, "mmap failed: " + msg};
    }

    const auto* begin = static_cast<const unsigned char*>(mapping);
    const auto* cursor = begin;
    const auto* end = begin + static_cast<std::size_t>(st.st_size);

    Status result = Status::ok();

    if (std::memcmp(cursor, kMagic, 4) != 0) {
        result = {StatusCode::InvalidModel, "invalid model magic"};
    } else {
        cursor += 4;
        if (!read_value(cursor, end, model.input_size) ||
            !read_value(cursor, end, model.hidden_size) ||
            !read_value(cursor, end, model.output_size)) {
            result = {StatusCode::InvalidModel, "truncated model header"};
        } else if (model.input_size == 0 || model.hidden_size == 0 ||
                   model.output_size == 0 || model.input_size > 4096 ||
                   model.hidden_size > 4096 || model.output_size > 4096) {
            result = {StatusCode::InvalidModel, "invalid model dimensions"};
        } else {
            const std::size_t w1 = static_cast<std::size_t>(model.input_size) * model.hidden_size;
            const std::size_t b1 = model.hidden_size;
            const std::size_t w2 = static_cast<std::size_t>(model.hidden_size) * model.output_size;
            const std::size_t b2 = model.output_size;

            if (!read_floats(cursor, end, model.w1, w1) ||
                !read_floats(cursor, end, model.b1, b1) ||
                !read_floats(cursor, end, model.w2, w2) ||
                !read_floats(cursor, end, model.b2, b2)) {
                result = {StatusCode::InvalidModel, "truncated model weights"};
            }
        }
    }

    ::munmap(mapping, static_cast<std::size_t>(st.st_size));
    ::close(fd);
    return result;
}
}
