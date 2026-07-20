#include <algorithm>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <numeric>
#include <string>
#include <vector>
#include "edgeinfer/runtime.hpp"

int main(int argc, char** argv) {
    if (argc < 2 || argc > 4) {
        std::cerr << "Usage: " << argv[0] << " MODEL [cpu|sim-npu] [iterations]\n";
        return 2;
    }

    const std::string model_path = argv[1];
    const std::string backend = argc >= 3 ? argv[2] : "cpu";
    const int iterations = argc >= 4 ? std::max(1, std::atoi(argv[3])) : 1000;

    edgeinfer::Runtime runtime;
    const auto init = runtime.initialize(model_path, backend, true);

    if (!init.is_ok()) {
        std::cerr << "Initialization failed: " << init.message() << "\n";
        return 1;
    }

    edgeinfer::Tensor input;
    input.data = {0.10F, 0.20F, 0.30F, 0.40F};
    edgeinfer::Tensor output;
    std::vector<double> latencies;
    latencies.reserve(static_cast<std::size_t>(iterations));

    for (int i = 0; i < iterations; ++i) {
        const auto status = runtime.infer(input, output);
        if (!status.is_ok()) {
            std::cerr << "Inference failed: " << status.message() << "\n";
            return 1;
        }
        latencies.push_back(runtime.metrics().last_latency_ms);
    }

    const double total = std::accumulate(latencies.begin(), latencies.end(), 0.0);
    const double avg = total / static_cast<double>(latencies.size());

    auto sorted = latencies;
    std::sort(sorted.begin(), sorted.end());
    const std::size_t p95_index = static_cast<std::size_t>(
        static_cast<double>(sorted.size() - 1) * 0.95
    );

    const auto& metrics = runtime.metrics();

    std::cout << std::fixed << std::setprecision(6);
    std::cout << "Backend requested: " << backend << "\n";
    std::cout << "Backend used: " << metrics.backend_used << "\n";
    std::cout << "Fallback used: " << (metrics.fallback_used ? "yes" : "no") << "\n";
    std::cout << "Iterations: " << iterations << "\n";
    std::cout << "Average latency: " << avg << " ms\n";
    std::cout << "P95 latency: " << sorted[p95_index] << " ms\n";
    std::cout << "Max RSS: " << metrics.max_rss_kb << " KB\n";
    std::cout << "Output:";
    for (float value : output.data) std::cout << " " << value;
    std::cout << "\n";
    return 0;
}
