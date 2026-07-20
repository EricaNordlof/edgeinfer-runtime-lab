#include "edgeinfer/runtime.hpp"

namespace edgeinfer {
Status Runtime::initialize(const std::string& model_path,
                           const std::string& preferred_backend,
                           bool allow_cpu_fallback) {
    const Status model_status = ModelLoader::load_mmap(model_path, model_);
    if (!model_status.is_ok()) return model_status;

    allow_cpu_fallback_ = allow_cpu_fallback;

    if (preferred_backend == "cpu") {
        preferred_backend_ = make_cpu_backend();
    } else if (preferred_backend == "sim-npu") {
        preferred_backend_ = make_sim_npu_backend();
    } else {
        return {StatusCode::InvalidArgument, "unknown backend: " + preferred_backend};
    }

    if (preferred_backend != "cpu" && allow_cpu_fallback_) {
        cpu_fallback_ = make_cpu_backend();
    }

    return Status::ok();
}

Status Runtime::infer(const Tensor& input, Tensor& output) {
    if (!preferred_backend_) {
        return {StatusCode::InvalidArgument, "runtime is not initialized"};
    }

    metrics_.fallback_used = false;
    const double start = monotonic_time_ms();

    Status status = preferred_backend_->infer(model_, input, output);
    metrics_.backend_used = preferred_backend_->name();

    if (!status.is_ok() && allow_cpu_fallback_ && cpu_fallback_) {
        status = cpu_fallback_->infer(model_, input, output);
        if (status.is_ok()) {
            metrics_.fallback_used = true;
            metrics_.backend_used = cpu_fallback_->name();
        }
    }

    metrics_.last_latency_ms = monotonic_time_ms() - start;
    metrics_.max_rss_kb = edgeinfer::max_rss_kb();
    return status;
}
}
