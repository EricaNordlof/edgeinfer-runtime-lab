#include "edgeinfer/c_api.h"
#include <memory>
#include <string>
#include "edgeinfer/runtime.hpp"

struct edgeinfer_handle {
    edgeinfer::Runtime runtime;
    std::string last_error;
};

edgeinfer_handle* edgeinfer_create(const char* model_path,
                                   const char* backend,
                                   int allow_cpu_fallback) {
    if (model_path == nullptr || backend == nullptr) return nullptr;

    auto handle = std::make_unique<edgeinfer_handle>();
    const auto status = handle->runtime.initialize(
        model_path, backend, allow_cpu_fallback != 0
    );

    if (!status.is_ok()) return nullptr;
    return handle.release();
}

int edgeinfer_infer(edgeinfer_handle* handle,
                    const float* input,
                    size_t input_len,
                    float* output,
                    size_t output_len) {
    if (handle == nullptr || input == nullptr || output == nullptr) return -1;

    const auto& model = handle->runtime.model();
    if (input_len != model.input_size || output_len != model.output_size) {
        handle->last_error = "input/output size mismatch";
        return -2;
    }

    edgeinfer::Tensor in;
    in.data.assign(input, input + input_len);
    edgeinfer::Tensor out;

    const auto status = handle->runtime.infer(in, out);
    if (!status.is_ok()) {
        handle->last_error = status.message();
        return -3;
    }

    for (size_t i = 0; i < output_len; ++i) output[i] = out.data[i];
    handle->last_error.clear();
    return 0;
}

const char* edgeinfer_last_error(edgeinfer_handle* handle) {
    return handle == nullptr ? "invalid handle" : handle->last_error.c_str();
}

double edgeinfer_last_latency_ms(edgeinfer_handle* handle) {
    return handle == nullptr ? -1.0 : handle->runtime.metrics().last_latency_ms;
}

void edgeinfer_destroy(edgeinfer_handle* handle) {
    delete handle;
}
