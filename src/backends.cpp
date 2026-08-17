#include "edgeinfer/backend.hpp"
#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <limits>
#include <memory>
#include <vector>

namespace edgeinfer {
namespace {

class CpuBackend final : public Backend {
public:
    std::string name() const override { return "cpu"; }

    Status infer(const TinyModel& model, const Tensor& input, Tensor& output) override {
        if (input.size() != model.input_size) {
            return {StatusCode::InvalidArgument, "input size does not match model"};
        }

        std::vector<float> hidden(model.hidden_size, 0.0F);

        for (std::size_t h = 0; h < model.hidden_size; ++h) {
            float sum = model.b1[h];
            for (std::size_t i = 0; i < model.input_size; ++i) {
                sum += input.data[i] * model.w1[h * model.input_size + i];
            }
            hidden[h] = std::max(0.0F, sum);
        }

        output.data.assign(model.output_size, 0.0F);
        float max_logit = -std::numeric_limits<float>::infinity();

        for (std::size_t o = 0; o < model.output_size; ++o) {
            float sum = model.b2[o];
            for (std::size_t h = 0; h < model.hidden_size; ++h) {
                sum += hidden[h] * model.w2[o * model.hidden_size + h];
            }
            output.data[o] = sum;
            max_logit = std::max(max_logit, sum);
        }

        float denom = 0.0F;
        for (float& value : output.data) {
            value = std::exp(value - max_logit);
            denom += value;
        }

        if (denom <= 0.0F) {
            return {StatusCode::InferenceError, "softmax denominator invalid"};
        }

        for (float& value : output.data) value /= denom;
        return Status::ok();
    }
};

class SimNpuBackend final : public Backend {
public:
    std::string name() const override { return "sim-npu"; }

    Status infer(const TinyModel& model, const Tensor& input, Tensor& output) override {
        const char* fail = std::getenv("EDGEINFER_SIM_NPU_FAIL");
        if (fail != nullptr && std::string(fail) == "1") {
            return {StatusCode::BackendUnavailable, "simulated accelerator failure"};
        }

        // Explicitly simulated accelerator path:
        // delegates math to CPU to exercise backend integration and fallback
        // without claiming real NPU hardware support.
        auto cpu = make_cpu_backend();
        return cpu->infer(model, input, output);
    }
};

}

std::unique_ptr<Backend> make_cpu_backend() {
    return std::make_unique<CpuBackend>();
}

std::unique_ptr<Backend> make_sim_npu_backend() {
    return std::make_unique<SimNpuBackend>();
}
}
