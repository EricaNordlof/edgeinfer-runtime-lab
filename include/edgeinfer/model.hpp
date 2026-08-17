#pragma once
#include <cstdint>
#include <string>
#include <vector>
#include "edgeinfer/status.hpp"

namespace edgeinfer {
struct TinyModel {
    std::uint32_t input_size = 0;
    std::uint32_t hidden_size = 0;
    std::uint32_t output_size = 0;
    std::vector<float> w1;
    std::vector<float> b1;
    std::vector<float> w2;
    std::vector<float> b2;
};

class ModelLoader {
public:
    static Status load_mmap(const std::string& path, TinyModel& model);
};
}
