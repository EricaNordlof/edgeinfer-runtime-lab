#pragma once
#include <memory>
#include <string>
#include "edgeinfer/model.hpp"
#include "edgeinfer/status.hpp"
#include "edgeinfer/tensor.hpp"

namespace edgeinfer {
class Backend {
public:
    virtual ~Backend() = default;
    virtual std::string name() const = 0;
    virtual Status infer(const TinyModel& model, const Tensor& input, Tensor& output) = 0;
};

std::unique_ptr<Backend> make_cpu_backend();
std::unique_ptr<Backend> make_sim_npu_backend();
}
