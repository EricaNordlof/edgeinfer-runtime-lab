# Linux / Embedded Notes

This project uses lower-level Linux/POSIX APIs instead of a web framework.

Demonstrated areas:

- file descriptors
- memory mapping
- process metrics
- monotonic timing
- local IPC
- signal handling
- explicit resource cleanup

A real embedded target would add concerns such as:

- cross-compilation
- watchdogs
- DMA / zero-copy buffers
- thermal limits
- secure boot
- hardware drivers
- ABI compatibility
- OTA updates
