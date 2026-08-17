# Debugging

A practical embedded inference debugging flow:

1. validate model format and dimensions
2. validate input/output contracts
3. run known vectors on CPU
4. compare accelerator output against CPU reference
5. measure latency and memory
6. inject backend failure
7. verify fallback and error propagation

Useful Linux tools for future extensions:

- gdb
- strace
- perf
- valgrind
- AddressSanitizer
- `/proc`
- core dumps
