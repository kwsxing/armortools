
#include "cgltf.h"

#include "iron_array.h"
#include "iron_gc.h"
#include "iron_obj.h"

#include <assert.h>
#include <math.h>
#include <memory.h>
#include <stdlib.h>
#include <string.h>

static bool  has_next  = false;
static float scale_pos = 1.0;

typedef uint32_t (*func_value_cast_u32)(const uint8_t *const data);
typedef float (*func_value_cast_float)(const uint8_t *const data);

uint32_t value_cast_i8_to_u32(const uint8_t *const data) {
	int8_t v = *((int8_t *)data);
	return (uint32_t)v;
}

uint32_t value_cast_u8_to_u32(const uint8_t *const data) {
	uint8_t v = *((uint8_t *)data);
	return (uint32_t)v;
}

uint32_t value_cast_i16_to_u32(const uint8_t *const data) {
	int16_t v = *((int16_t *)data);
	return (uint32_t)v;
}

uint32_t value_cast_u16_to_u32(const uint8_t *const data) {
	uint16_t v = *((uint16_t *)data);
	return (uint32_t)v;
}

uint32_t value_cast_i32_to_u32(const uint8_t *const data) {
	int32_t v = *((int32_t *)data);
	return (uint32_t)v;
}

uint32_t value_cast_u32_to_u32(const uint8_t *const data) {
	uint32_t v = *((uint32_t *)data);
	return (uint32_t)v;
}

uint32_t value_cast_float_to_u32(const uint8_t *const data) {
	float v = *((float *)data);
	return (uint32_t)v;
}

float value_cast_i8_to_float(const uint8_t *const data) {
	int8_t v = *((int8_t *)data);
	return (float)v;
}

float value_cast_u8_to_float(const uint8_t *const data) {
	uint8_t v = *((uint8_t *)data);
	return (float)v;
}

float value_cast_i16_to_float(const uint8_t *const data) {
	int16_t v = *((int16_t *)data);
	return (float)v;
}

float value_cast_u16_to_float(const uint8_t *const data) {
	uint16_t v = *((uint16_t *)data);
	return (float)v;
}

float value_cast_i32_to_float(const uint8_t *const data) {
	int32_t v = *((int32_t *)data);
	return (float)v;
}

float value_cast_u32_to_float(const uint8_t *const data) {
	uint32_t v = *((uint32_t *)data);
	return (float)v;
}

float value_cast_float_to_float(const uint8_t *const data) {
	float v = *((float *)data);
	return (float)v;
}

uint32_t *u32_buffer_from_buffer(const uint8_t *const buffer, uint32_t count, uint32_t stride, uint32_t num_component, uint32_t stride_component,
                                 func_value_cast_u32 value_cast) {
	assert(buffer != NULL);
	assert(count != 0);
	assert(num_component != 0);
	assert(stride >= (num_component * stride_component));

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
				if (value_cast) {
					res[i * num_component + j] = value_cast(buffer + i * stride + j * stride_component);
				}
				else {
					res[i * num_component + j] = *(uint32_t *)(buffer + i * stride + j * stride_component);
				}
			}
		}
	}

	return res;
}

float *float_buffer_from_buffer(const uint8_t *const buffer, uint32_t count, uint32_t stride, uint32_t num_component, uint32_t stride_component,
                                func_value_cast_float value_cast) {
	assert(buffer != NULL);
	assert(count != 0);
	assert(num_component != 0);
	assert(stride >= (num_component * stride_component));

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
				if (value_cast) {
					res[i * num_component + j] = value_cast(buffer + i * stride + j * stride_component);
				}
				else {
					res[i * num_component + j] = *(float *)(buffer + i * stride + j * stride_component);
				}
			}
		}
	}

	return res;
}

uint32_t *io_gltf_read_u32(const cgltf_accessor *const a) {
	assert(a != NULL);
	if (a == NULL)
		return NULL;

	const cgltf_buffer_view *const v = a->buffer_view;
	assert(v != NULL);
	if (v == NULL)
		return NULL;

	uint32_t num_compoonents = 0;
	switch (a->type) {
	case cgltf_type_scalar:
		num_compoonents = 1;
		break;

	case cgltf_type_vec2:
		num_compoonents = 2;
		break;

	case cgltf_type_vec3:
		num_compoonents = 3;
		break;

	case cgltf_type_vec4:
		num_compoonents = 4;
		break;

	default:
		assert(0);
		break;
	}

	uint32_t            stride_component = 0;
	func_value_cast_u32 func_value_cast  = NULL;
	switch (a->component_type) {
	case cgltf_component_type_r_8:
		stride_component = sizeof(int8_t);
		func_value_cast  = value_cast_i8_to_u32;
		break;

	case cgltf_component_type_r_8u:
		stride_component = sizeof(uint8_t);
		func_value_cast  = value_cast_u8_to_u32;
		break;

	case cgltf_component_type_r_16:
		stride_component = sizeof(int16_t);
		func_value_cast  = value_cast_i16_to_u32;
		break;

	case cgltf_component_type_r_16u:
		stride_component = sizeof(uint16_t);
		func_value_cast  = value_cast_u16_to_u32;
		break;

	case cgltf_component_type_r_32u:
		stride_component = sizeof(uint32_t);
		func_value_cast  = value_cast_u32_to_u32;
		break;

	case cgltf_component_type_r_32f:
		stride_component = sizeof(float);
		func_value_cast  = value_cast_float_to_u32;
		break;

	default:
		assert(0);
		break;
	}

	return u32_buffer_from_buffer((v->buffer->data + v->offset + a->offset), a->count, a->stride, num_compoonents, stride_component, func_value_cast);
}

float *io_gltf_read_float(const cgltf_accessor *const a, uint32_t *out_num_component) {
	assert(a != NULL);
	if (a == NULL)
		return NULL;

	const cgltf_buffer_view *const v = a->buffer_view;
	assert(v != NULL);
	if (v == NULL)
		return NULL;

	uint32_t num_compoonents = 0;
	switch (a->type) {
	case cgltf_type_scalar:
		num_compoonents = 1;
		break;

	case cgltf_type_vec2:
		num_compoonents = 2;
		break;

	case cgltf_type_vec3:
		num_compoonents = 3;
		break;

	case cgltf_type_vec4:
		num_compoonents = 4;
		break;

	default:
		assert(0);
		break;
	}

	uint32_t              stride_component = 0;
	func_value_cast_float func_value_cast  = NULL;
	switch (a->component_type) {
	case cgltf_component_type_r_8:
		stride_component = sizeof(int8_t);
		func_value_cast  = value_cast_i8_to_float;
		break;

	case cgltf_component_type_r_8u:
		stride_component = sizeof(uint8_t);
		func_value_cast  = value_cast_u8_to_float;
		break;

	case cgltf_component_type_r_16:
		stride_component = sizeof(int16_t);
		func_value_cast  = value_cast_i16_to_float;
		break;

	case cgltf_component_type_r_16u:
		stride_component = sizeof(uint16_t);
		func_value_cast  = value_cast_u16_to_float;
		break;

	case cgltf_component_type_r_32u:
		stride_component = sizeof(uint32_t);
		func_value_cast  = value_cast_u32_to_float;
		break;

	case cgltf_component_type_r_32f:
		stride_component = sizeof(float);
		func_value_cast  = value_cast_float_to_float;
		break;

	default:
		assert(0);
		break;
	}

	if (out_num_component != NULL)
		*out_num_component = num_compoonents;
	return float_buffer_from_buffer((v->buffer->data + v->offset + a->offset), a->count, a->stride, num_compoonents, stride_component, func_value_cast);
}

void io_gltf_parse_v2_mesh(raw_mesh_t *raw, cgltf_mesh *mesh, float *to_world, float *scale, bool parse_vcols) {

	cgltf_primitive *prim = NULL;
	uint32_t        *inda = NULL;

	for (int i = 0; i < mesh->primitives_count; ++i) {
		prim = &mesh->primitives[0];

		const cgltf_accessor *const a = prim->indices;
		inda                          = io_gltf_read_u32(a);

		if (inda) {
			break;
		}
	}

	if (!inda) {
		// All of the primitives in the mesh don't have data
		return;
	}

	int index_count = prim->indices->count;

	int      vertex_count          = -1;
	float   *posa32                = NULL;
	uint32_t posa32_num_components = 0;
	float   *nora32                = NULL;
	uint32_t nora32_num_components = 0;
	float   *texa32                = NULL;
	uint32_t texa32_num_components = 0;
	float   *cola32                = NULL;
	uint32_t cola32_num_components = 0;

	for (int i = 0; i < prim->attributes_count; ++i) {
		cgltf_attribute *attrib = &prim->attributes[i];

		if (attrib->type == cgltf_attribute_type_position) {
			vertex_count = attrib->data->count;
			posa32       = io_gltf_read_float(attrib->data, &posa32_num_components);
		}
		else if (attrib->type == cgltf_attribute_type_normal) {
			nora32 = io_gltf_read_float(attrib->data, &nora32_num_components);
		}
		else if (attrib->type == cgltf_attribute_type_texcoord) {
			texa32 = io_gltf_read_float(attrib->data, &texa32_num_components);
		}
		else if (parse_vcols && attrib->type == cgltf_attribute_type_color) {
			cola32 = io_gltf_read_float(attrib->data, &cola32_num_components);
		}
	}

	if (vertex_count == -1) {
		// No vertex data position attributes found in primitive
		return;
	}

	float *m = to_world;
#if defined(ENABLE_OPENMP)
#pragma omp parallel
#pragma omp for
	for (int i = 0; i < vertex_count; ++i) {
#else
	for (int i = 0; i < vertex_count; ++i) {
#endif
		float x           = posa32[i * 3 + 0];
		float y           = posa32[i * 3 + 1];
		float z           = posa32[i * 3 + 2];
		posa32[i * 3 + 0] = m[0] * x + m[4] * y + m[8] * z + m[12];
		posa32[i * 3 + 1] = m[1] * x + m[5] * y + m[9] * z + m[13];
		posa32[i * 3 + 2] = m[2] * x + m[6] * y + m[10] * z + m[14];
	}

	if (nora32 != NULL) {
#if defined(ENABLE_OPENMP)
#pragma omp parallel
#pragma omp for
		for (int i = 0; i < vertex_count; ++i) {
#else
		for (int i = 0; i < vertex_count; ++i) {
#endif
			float x           = nora32[i * 3 + 0] / scale[0];
			float y           = nora32[i * 3 + 1] / scale[1];
			float z           = nora32[i * 3 + 2] / scale[2];
			nora32[i * 3 + 0] = m[0] * x + m[4] * y + m[8] * z;
			nora32[i * 3 + 1] = m[1] * x + m[5] * y + m[9] * z;
			nora32[i * 3 + 2] = m[2] * x + m[6] * y + m[10] * z;
		}
	}

	short *posa = malloc(sizeof(short) * vertex_count * 4);
	{
		// Pack positions to (-1, 1) range
		float _scale_pos = 0.0f;
		for (int i = 0, ic = vertex_count * 3; i < ic; ++i) {
			float f = fabsf(posa32[i]);
			if (_scale_pos < f)
				_scale_pos = f;
		}

		if (_scale_pos > scale_pos)
			scale_pos = _scale_pos;
		const float inv = 32767 / scale_pos;

		// Pack into 16bit
#if defined(ENABLE_OPENMP)
#pragma omp parallel
#pragma omp for
		for (int i = 0; i < vertex_count; ++i) {
#else
		for (int i = 0; i < vertex_count; ++i) {
#endif
			posa[i * 4 + 0] = posa32[i * 3 + 0] * inv;
			posa[i * 4 + 1] = posa32[i * 3 + 1] * inv;
			posa[i * 4 + 2] = posa32[i * 3 + 2] * inv;
		}
	}

	short *nora = malloc(sizeof(short) * vertex_count * 2);
	if (nora32 != NULL) {
#if defined(ENABLE_OPENMP)
#pragma omp parallel
#pragma omp for
		for (int i = 0; i < vertex_count; ++i) {
#else
		for (int i = 0; i < vertex_count; ++i) {
#endif
			nora[i * 2 + 0] = nora32[i * 3 + 0] * 32767;
			nora[i * 2 + 1] = nora32[i * 3 + 1] * 32767;
			posa[i * 4 + 3] = nora32[i * 3 + 2] * 32767;
		}
	}
	else {
		// Calc normals
#if defined(ENABLE_OPENMP)
#pragma omp parallel
#pragma omp for
		for (int i = 0, ic = index_count / 3; i < ic; ++i) {
#else
		for (int i = 0, ic = index_count / 3; i < ic; ++i) {
#endif
			int   i1  = inda[i * 3 + 0];
			int   i2  = inda[i * 3 + 1];
			int   i3  = inda[i * 3 + 2];
			float vax = posa32[i1 * 3 + 0];
			float vay = posa32[i1 * 3 + 1];
			float vaz = posa32[i1 * 3 + 2];
			float vbx = posa32[i2 * 3 + 0];
			float vby = posa32[i2 * 3 + 1];
			float vbz = posa32[i2 * 3 + 2];
			float vcx = posa32[i3 * 3 + 0];
			float vcy = posa32[i3 * 3 + 1];
			float vcz = posa32[i3 * 3 + 2];
			float cbx = vcx - vbx;
			float cby = vcy - vby;
			float cbz = vcz - vbz;
			float abx = vax - vbx;
			float aby = vay - vby;
			float abz = vaz - vbz;
			float x = cbx, y = cby, z = cbz;
			cbx     = y * abz - z * aby;
			cby     = z * abx - x * abz;
			cbz     = x * aby - y * abx;
			float n = sqrt(cbx * cbx + cby * cby + cbz * cbz);
			if (n > 0.0) {
				float inv_n = 1.0 / n;
				cbx *= inv_n;
				cby *= inv_n;
				cbz *= inv_n;
			}
			nora[i1 * 2]     = (int)(cbx * 32767);
			nora[i1 * 2 + 1] = (int)(cby * 32767);
			posa[i1 * 4 + 3] = (int)(cbz * 32767);
			nora[i2 * 2]     = (int)(cbx * 32767);
			nora[i2 * 2 + 1] = (int)(cby * 32767);
			posa[i2 * 4 + 3] = (int)(cbz * 32767);
			nora[i3 * 2]     = (int)(cbx * 32767);
			nora[i3 * 2 + 1] = (int)(cby * 32767);
			posa[i3 * 4 + 3] = (int)(cbz * 32767);
		}
	}

	short *texa = NULL;
	if (texa32 != NULL) {
		texa = malloc(sizeof(short) * vertex_count * 2);
#if defined(ENABLE_OPENMP)
#pragma omp parallel
#pragma omp for
		for (int i = 0; i < vertex_count; ++i) {
#else
		for (int i = 0; i < vertex_count; ++i) {
#endif
			texa[i * 2]     = texa32[i * 2] * 32767;
			texa[i * 2 + 1] = texa32[i * 2 + 1] * 32767;
		}
	}

	short *cola = NULL;
	if (cola32 != NULL) {
		cola = malloc(sizeof(short) * vertex_count * 4);
#if defined(ENABLE_OPENMP)
#pragma omp parallel
#pragma omp for
		for (int i = 0; i < vertex_count; ++i) {
#else
		for (int i = 0; i < vertex_count; ++i) {
#endif
			cola[i * 4 + 0] = cola32[i * cola32_num_components + 0] * 32767;
			cola[i * 4 + 1] = cola32[i * cola32_num_components + 1] * 32767;
			cola[i * 4 + 2] = cola32[i * cola32_num_components + 2] * 32767;
			if (cola32_num_components >= 4) {
				cola[i * 4 + 3] = cola32[i * cola32_num_components + 3] * 32767;
			}
			else {
				cola[i * 4 + 3] = 32767;
			}
		}
	}

	raw->posa         = (i16_array_t *)malloc(sizeof(i16_array_t));
	raw->posa->buffer = posa;
	raw->posa->length = raw->posa->capacity = vertex_count * 4;

	raw->nora         = (i16_array_t *)malloc(sizeof(i16_array_t));
	raw->nora->buffer = nora;
	raw->nora->length = raw->nora->capacity = vertex_count * 2;

	if (texa != NULL) {
		raw->texa         = (i16_array_t *)malloc(sizeof(i16_array_t));
		raw->texa->buffer = texa;
		raw->texa->length = raw->texa->capacity = vertex_count * 2;
	}

	if (cola != NULL) {
		raw->cola         = (i16_array_t *)malloc(sizeof(i16_array_t));
		raw->cola->buffer = cola;
		raw->cola->length = raw->cola->capacity = vertex_count * 4;
	}

	raw->inda         = (u32_array_t *)malloc(sizeof(u32_array_t));
	raw->inda->buffer = inda;
	raw->inda->length = raw->inda->capacity = index_count;

	raw->scale_pos = scale_pos;
	raw->scale_tex = 1.0;

	raw->vertex_count = vertex_count;
	raw->index_count  = index_count;

	if (posa32 != NULL) {
		free(posa32);
		posa32 = NULL;
	}
	if (nora32 != NULL) {
		free(nora32);
		nora32 = NULL;
	}
	if (texa32 != NULL) {
		free(texa32);
		texa32 = NULL;
	}
	if (cola32 != NULL) {
		free(cola32);
		cola32 = NULL;
	}
}

void *io_gltf_parse_v2(char *buf, size_t size, const char *path) {
	raw_mesh_t *raw = NULL;

	cgltf_options gltf_options = {0};
	cgltf_data   *gltf_data    = NULL;

	cgltf_result result = cgltf_parse(&gltf_options, buf, size, &gltf_data);
	if (result == cgltf_result_success) {
		cgltf_load_buffers(&gltf_options, gltf_data, path);

		any_array_t *meshes = any_array_create(gltf_data->nodes_count);
		memset(meshes->buffer, 0, sizeof(raw_mesh_t *) * gltf_data->nodes_count);

		for (uint32_t i = 0; i < gltf_data->nodes_count; ++i) {
			cgltf_node *n = &gltf_data->nodes[i];
			if (n->mesh == NULL)
				continue;

			float m[16];
			cgltf_node_transform_world(n, &m);
			float scale[3];
			scale[0] = 1;
			scale[1] = 1;
			scale[2] = 1;

			if (n->has_scale) {
				scale[0] = n->scale[0];
				scale[1] = n->scale[1];
				scale[2] = n->scale[2];
			}

			raw_mesh_t *part = gc_alloc(sizeof(raw_mesh_t));
			memset(part, 0, sizeof(raw_mesh_t));

			io_gltf_parse_v2_mesh(part, n->mesh, &m, &scale, false);
			if (part->posa != NULL) {
				part->name = malloc(strlen(n->name) + 1);
				strcpy(part->name, n->name);
				meshes->buffer[i] = part;
			}
			else {
				obj_destroy(part);
			}
		}

		raw = obj_merge(meshes, false, false);

		for (uint32_t i = 0; i < meshes->length; ++i) {
			raw_mesh_t *part = (raw_mesh_t *)meshes->buffer[i];
			if (part == NULL)
				continue;

			obj_destroy(part);
		}
		gc_free(meshes);
	}

	cgltf_free(gltf_data);
	gltf_data = NULL;

	return raw;
}
