# Interview positioning

## 30-second explanation

> I built EdgeInfer Runtime Lab as an independent C++ project to learn the
> engineering concerns behind edge deep-learning platforms. It includes a real
> small neural-network workload, C and C++ runtime APIs, Linux mmap, a local
> inference service, latency and memory metrics, and a simulated accelerator
> interface with CPU fallback.

## Strong points to discuss

- why a stable C API can help integration
- why `mmap` is useful for model files
- how backend abstraction isolates hardware-specific code
- why CPU reference output is useful when validating accelerators
- how fallback should be explicit and observable
- how latency and memory constrain edge inference

## Be honest

Do not claim:

- real NPU integration
- production Axis hardware experience
- 3–5 years embedded C/C++ experience

The simulated backend is a learning and architecture exercise.
