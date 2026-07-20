# EdgeInfer Runtime Lab

**Independent embedded / edge deep-learning portfolio project by Erica Nordlöf**

EdgeInfer Runtime Lab is a C++ runtime and Linux service built to demonstrate
engineering concerns found in embedded deep-learning platforms.

## What it demonstrates

- C++17 and a C-facing runtime API
- Linux/POSIX APIs
- model loading with `mmap`
- a real tiny neural-network inference workload
- CPU backend
- simulated accelerator abstraction
- accelerator failure injection and CPU fallback
- latency and memory measurements
- Unix domain socket inference service
- CMake
- automated tests
- Docker
- Jenkins and GitHub Actions

The `sim-npu` backend is **explicitly simulated**. It demonstrates integration,
fallback and reliability patterns without claiming real NPU hardware experience.

## Architecture

```text
Application
    |
    +--> C API
    |
    +--> C++ Runtime
            |
            +--> CPU backend
            |
            +--> simulated NPU backend
                      |
                      +--> failure injection
                      +--> CPU fallback

Model file
    |
    +--> Linux mmap
    |
    +--> Tiny neural network
          4 inputs
             |
          Dense 4x8
             |
            ReLU
             |
          Dense 8x3
             |
           Softmax
```

## Build

```bash
python3 tools/generate_model.py
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j
ctest --test-dir build --output-on-failure
```

## Benchmark

```bash
./build/edgeinfer_bench models/tinynet.eir cpu 10000
```

Simulated accelerator path:

```bash
./build/edgeinfer_bench models/tinynet.eir sim-npu 10000
```

Failure + CPU fallback:

```bash
EDGEINFER_SIM_NPU_FAIL=1 \
./build/edgeinfer_bench models/tinynet.eir sim-npu 100
```

## Linux inference service

```bash
./build/edgeinfer_service models/tinynet.eir /tmp/edgeinfer.sock
```

Example client:

```bash
printf "0.10 0.20 0.30 0.40\n" | nc -U /tmp/edgeinfer.sock
```

The service uses Linux APIs such as:

- `socket`
- `bind`
- `listen`
- `accept`
- `read`
- `write`
- `unlink`

## C API

```c
edgeinfer_handle* runtime =
    edgeinfer_create("models/tinynet.eir", "cpu", 1);

float input[4] = {0.1f, 0.2f, 0.3f, 0.4f};
float output[3];

edgeinfer_infer(runtime, input, 4, output, 3);
edgeinfer_destroy(runtime);
```

## Linux APIs used

- `open`
- `fstat`
- `mmap`
- `munmap`
- `clock_gettime`
- `getrusage`
- Unix domain sockets

## Why this matches an embedded deep-learning role

The project focuses on the same categories of problems:

- runtime APIs
- embedded/Linux integration
- inference execution
- accelerator abstraction
- performance
- memory
- reliability
- debugging
- platform quality

It does not replace missing professional experience or a master's degree, but
it creates a concrete project to discuss in interviews.

## Honest interview description

> I built an independent C++ edge-inference runtime lab to learn the
> engineering concerns around embedded deep-learning platforms. It includes a
> small real neural-network workload, C/C++ APIs, Linux mmap, Unix-domain socket
> service, performance metrics and a simulated accelerator with CPU fallback.
