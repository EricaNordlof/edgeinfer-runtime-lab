#pragma once
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct edgeinfer_handle edgeinfer_handle;

edgeinfer_handle* edgeinfer_create(
    const char* model_path,
    const char* backend,
    int allow_cpu_fallback
);

int edgeinfer_infer(
    edgeinfer_handle* handle,
    const float* input,
    size_t input_len,
    float* output,
    size_t output_len
);

const char* edgeinfer_last_error(edgeinfer_handle* handle);
double edgeinfer_last_latency_ms(edgeinfer_handle* handle);
void edgeinfer_destroy(edgeinfer_handle* handle);

#ifdef __cplusplus
}
#endif
