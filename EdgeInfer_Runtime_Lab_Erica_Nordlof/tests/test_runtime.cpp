#include <cmath>
#include <cstdlib>
#include <iostream>
#include <string>
#include "edgeinfer/runtime.hpp"

int fail(const std::string& msg) {
    std::cerr << "TEST FAILED: " << msg << "\n";
    return 1;
}

int main(int argc, char** argv) {
    if (argc != 2) return fail("expected model path");

    const std::string model_path = argv[1];

    edgeinfer::Runtime cpu;
    auto status = cpu.initialize(model_path, "cpu", true);
    if (!status.is_ok()) return fail("CPU init failed");

    edgeinfer::Tensor input;
    input.data = {0.1F, 0.2F, 0.3F, 0.4F};
    edgeinfer::Tensor output;

    status = cpu.infer(input, output);
    if (!status.is_ok()) return fail("CPU inference failed");
    if (output.size() != 3) return fail("unexpected output size");

    float sum = 0.0F;
    for (float value : output.data) {
        if (value < 0.0F || value > 1.0F) return fail("softmax outside range");
        sum += value;
    }
    if (std::fabs(sum - 1.0F) > 1e-3F) return fail("softmax does not sum to 1");

    ::setenv("EDGEINFER_SIM_NPU_FAIL", "1", 1);

    edgeinfer::Runtime fallback;
    status = fallback.initialize(model_path, "sim-npu", true);
    if (!status.is_ok()) return fail("sim-npu init failed");

    status = fallback.infer(input, output);
    if (!status.is_ok()) return fail("fallback inference failed");
    if (!fallback.metrics().fallback_used) return fail("fallback flag missing");
    if (fallback.metrics().backend_used != "cpu") return fail("fallback backend should be cpu");

    ::unsetenv("EDGEINFER_SIM_NPU_FAIL");

    edgeinfer::Tensor bad_input;
    bad_input.data = {0.1F, 0.2F};
    status = cpu.infer(bad_input, output);
    if (status.is_ok()) return fail("invalid input should fail");

    std::cout << "All EdgeInfer tests passed.\n";
    return 0;
}
