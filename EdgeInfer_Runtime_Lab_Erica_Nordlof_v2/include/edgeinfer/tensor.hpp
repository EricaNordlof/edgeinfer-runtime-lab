#pragma once
#include <cstddef>
#include <vector>

namespace edgeinfer {
struct Tensor {
    std::vector<float> data;
    std::size_t size() const { return data.size(); }
};
}
