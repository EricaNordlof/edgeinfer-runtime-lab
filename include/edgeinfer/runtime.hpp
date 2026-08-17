#pragma once
#include <memory>
#include <string>
#include "edgeinfer/backend.hpp"

namespace edgeinfer {
struct RuntimeMetrics {
    double last_latency_ms = 0.0;
    long max_rss_kb = 0;
    std::string backend_used;
    bool fallback_used = false;
};

class Runtime {
public:
    Status initialize(const std::string& model_path,
                      const std::string& preferred_backend,
                      bool allow_cpu_fallback);

    Status infer(const Tensor& input, Tensor& output);

    const RuntimeMetrics& metrics() const { return metrics_; }
    const TinyModel& model() const { return model_; }

private:
    TinyModel model_;
    std::unique_ptr<Backend> preferred_backend_;
    std::unique_ptr<Backend> cpu_fallback_;
    bool allow_cpu_fallback_ = true;
    RuntimeMetrics metrics_;
};

double monotonic_time_ms();
long max_rss_kb();
}
