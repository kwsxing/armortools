#include "io_mesh.h"

#if defined(ENABLE_OPENMP)
#include <omp.h>
#endif

#include <assert.h>
#include <memory.h>
#include <stdlib.h>

uint32_t *u32_buffer_from_buffer(const uint8_t *const buffer, uint32_t count, uint32_t stride, uint32_t num_component, uint32_t stride_component,
                                        func_memory_cast_u32 memory_cast) {
	assert(buffer != NULL);
	assert(count != 0);
	assert(num_component != 0);
	assert(stride >= (num_component * stride_component));
	assert(memory_cast != NULL);

	uint32_t *res = malloc(sizeof(uint32_t) * count * num_component);
	if (stride == (sizeof(uint32_t) * count * num_component))
		memcpy(res, buffer, sizeof(uint32_t) * count * num_component);
	else {
#if defined(ENABLE_OPENMP)
#pragma omp parallel
#pragma omp for
		for (uint32_t i = 0; i < count; ++i) {
#else
		for (uint32_t i = 0; i < count; ++i) {
#endif
			for (uint32_t j = 0; j < num_component; ++j) {
				res[i * num_component + j] = memory_cast(buffer + i * stride + j * stride_component);
			}
		}
	}

	return res;
}

float *float_buffer_from_buffer(const uint8_t *const buffer, uint32_t count, uint32_t stride, uint32_t num_component, uint32_t stride_component,
                                       func_memory_cast_float memory_cast) {
	assert(buffer != NULL);
	assert(count != 0);
	assert(num_component != 0);
	assert(stride >= (num_component * stride_component));
	assert(memory_cast != NULL);

	float *res = malloc(sizeof(float) * count * num_component);
	if (stride == (sizeof(float) * count * num_component))
		memcpy(res, buffer, sizeof(float) * count * num_component);
	else {
#if defined(ENABLE_OPENMP)
#pragma omp parallel
#pragma omp for
		for (uint32_t i = 0; i < count; ++i) {
#else
		for (uint32_t i = 0; i < count; ++i) {
#endif
			for (uint32_t j = 0; j < num_component; ++j) {
				res[i * num_component + j] = memory_cast(buffer + i * stride + j * stride_component);
			}
		}
	}

	return res;
}
