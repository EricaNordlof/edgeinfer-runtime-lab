# Performance

The benchmark measures:

- average inference latency
- P95 inference latency
- maximum resident set size

Latency uses:

```cpp
clock_gettime(CLOCK_MONOTONIC, ...)
```

Memory uses:

```cpp
getrusage(RUSAGE_SELF, ...)
```

Possible future optimizations:

- fixed tensor arenas
- int8 quantization
- SIMD kernels
- thread affinity
- aligned buffers
- zero-copy input
- real accelerator SDK
- ONNX Runtime or TensorFlow Lite backend
