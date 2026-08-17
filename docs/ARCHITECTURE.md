# Architecture

The runtime keeps application code separate from backend-specific execution.

```text
Application
   |
C API / C++ API
   |
Runtime
  / \
CPU  Simulated accelerator
       |
       +-- failure injection
       +-- CPU fallback
```

The model is memory-mapped from disk with `mmap`.

The backend interface is the seam where a real accelerator SDK, ONNX Runtime,
TensorFlow Lite or vendor-specific inference IP could later be integrated.

The simulated backend is deliberately named and documented as simulated.
