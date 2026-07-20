# EdgeInfer Runtime Lab

**Embedded C++ · Linux · Edge Deep Learning**

Självständigt portfolio-labb med en liten C/C++-runtime för edge inference.
Projektet innehåller Linux `mmap`, C- och C++-API, verklig neural
nätverksinferens, backend-abstraktion, simulerad accelerator med CPU-fallback,
latency- och minnesmätning samt en lokal inference-tjänst via Unix domain sockets.

**Teknik:** C++17 · C · Linux/POSIX · CMake · Deep Learning Runtime · mmap · Unix sockets · Docker · Jenkins · GitHub Actions

Acceleratorn är simulerad och används för att demonstrera integration,
felhantering och fallback, inte som påstående om verklig NPU-erfarenhet.
