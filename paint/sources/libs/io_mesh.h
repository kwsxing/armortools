#pragma once

#include <stdbool.h>
#include <stdint.h>

typedef enum {
	IO_MESH_SPLIT_TYPE_OBJECT   = 0,
	IO_MESH_SPLIT_TYPE_GROUP    = 1,
	IO_MESH_SPLIT_TYPE_MATERIAL = 2,
	IO_MESH_SPLIT_TYPE_UDIM     = 3,
} _split_type_t;

typedef struct io_mesh_config {
	_split_type_t split_by;
	bool          parse_transform;
	bool          parse_vcols;
	bool          parse_texs;
	bool          uv_wrap;
	bool          merge_one;
} io_mesh_config_t;

typedef bool (*io_mesh_progress_callback)(uint32_t completed, uint32_t total, char *message, bool finished);

typedef uint32_t (*func_memory_cast_u32)(const uint8_t *const data);
typedef float (*func_memory_cast_float)(const uint8_t *const data);
uint32_t *u32_buffer_from_buffer(const uint8_t *const buffer, uint32_t count, uint32_t stride, uint32_t num_component, uint32_t stride_component,
                                 func_memory_cast_u32 memory_cast);
float    *float_buffer_from_buffer(const uint8_t *const buffer, uint32_t count, uint32_t stride, uint32_t num_component, uint32_t stride_component,
                                   func_memory_cast_float memory_cast);
