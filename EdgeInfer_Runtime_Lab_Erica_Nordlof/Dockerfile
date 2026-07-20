FROM ubuntu:24.04 AS build

RUN apt-get update && \
    apt-get install -y --no-install-recommends build-essential cmake python3 && \
    rm -rf /var/lib/apt/lists/*

WORKDIR /src
COPY . .

RUN python3 tools/generate_model.py && \
    cmake -S . -B build -DCMAKE_BUILD_TYPE=Release && \
    cmake --build build -j2 && \
    ctest --test-dir build --output-on-failure

FROM ubuntu:24.04
WORKDIR /app
COPY --from=build /src/build/edgeinfer_bench /app/
COPY --from=build /src/build/edgeinfer_service /app/
COPY --from=build /src/models /app/models

CMD ["/app/edgeinfer_bench", "/app/models/tinynet.eir", "cpu", "1000"]
