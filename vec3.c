/*
 * transfor{m,t}_x{1,4,8} routine and testbed
 *
 * Copyright 2024 Martin Krastev <blu.dark@gmail.com>
 * SPDX-License-Identifier: MIT
 */

#include <stdio.h>
#include <arm_neon.h>
#include <assert.h>
#include <string.h>

#if _M_ARM64
	#define VC_EXTRALEAN
	#define WIN32_LEAN_AND_MEAN

	#include <windows.h>
	#include <synchapi.h>

	#define sleep(n) Sleep(1000 * (n))
#else
	#include <unistd.h>
#endif

#if __APPLE__
	#include <pthread.h>
#endif

typedef struct _matx3 {
	float m[3][3];
} matx3;

typedef struct _matx4x3 {
	float m[4][3];
} matx4x3;

#ifndef CACHELINE_SIZE
	#define CACHELINE_SIZE 64
#endif

#if __APPLE__
#define CASE_ENTRY(entry)	\
	case entry: return #entry

static const char *qos_to_str(qos_class_t qos)
{
	switch (qos) {
	CASE_ENTRY(QOS_CLASS_USER_INTERACTIVE);
	CASE_ENTRY(QOS_CLASS_USER_INITIATED);
	CASE_ENTRY(QOS_CLASS_DEFAULT);
	CASE_ENTRY(QOS_CLASS_UTILITY);
	CASE_ENTRY(QOS_CLASS_BACKGROUND);
	CASE_ENTRY(QOS_CLASS_UNSPECIFIED);
	}

	return "alien qos";
}

#endif
#if ETALON

#if __APPLE__
	#define DEMANG(name) asm(#name)
#else
	#define DEMANG(name)
#endif

extern void transform_x4(const matx3* mat, const float* vec, size_t num_vec, float* out) DEMANG(transform_x4);
extern void transform_x8(const matx3* mat, const float* vec, size_t num_vec, float* out) DEMANG(transform_x8);
extern void transfort_x4(const matx4x3* mat, const float* vec, size_t num_vec, float* out) DEMANG(transfort_x4);
extern void transfort_x8(const matx4x3* mat, const float* vec, size_t num_vec, float* out) DEMANG(transfort_x8);

#else
void transform_x1(const matx3* mat, const float* vec, size_t num_vec, float* out)
{
	__builtin_assume(num_vec % 4 == 0);

	const float m00 = mat->m[0][0];
	const float m01 = mat->m[0][1];
	const float m02 = mat->m[0][2];
	const float m10 = mat->m[1][0];
	const float m11 = mat->m[1][1];
	const float m12 = mat->m[1][2];
	const float m20 = mat->m[2][0];
	const float m21 = mat->m[2][1];
	const float m22 = mat->m[2][2];

	for ( ; num_vec; num_vec -= 1, vec += 3, out += 3) {
		const float v0 = vec[0];
		const float v1 = vec[1];
		const float v2 = vec[2];

		out[0] = m00 * v0 + m01 * v1 + m02 * v2;
		out[1] = m10 * v0 + m11 * v1 + m12 * v2;
		out[2] = m20 * v0 + m21 * v1 + m22 * v2;
	}
}

void transfort_x1(const matx4x3* mat, const float* vec, size_t num_vec, float* out)
{
	__builtin_assume(num_vec % 4 == 0);

	const float m00 = mat->m[0][0];
	const float m01 = mat->m[0][1];
	const float m02 = mat->m[0][2];
	const float m10 = mat->m[1][0];
	const float m11 = mat->m[1][1];
	const float m12 = mat->m[1][2];
	const float m20 = mat->m[2][0];
	const float m21 = mat->m[2][1];
	const float m22 = mat->m[2][2];
	const float tr0 = mat->m[3][0];
	const float tr1 = mat->m[3][1];
	const float tr2 = mat->m[3][2];

	for ( ; num_vec; num_vec -= 1, vec += 3, out += 3) {
		const float v0 = vec[0];
		const float v1 = vec[1];
		const float v2 = vec[2];

		out[0] = tr0 + m00 * v0 + m01 * v1 + m02 * v2;
		out[1] = tr1 + m10 * v0 + m11 * v1 + m12 * v2;
		out[2] = tr2 + m20 * v0 + m21 * v1 + m22 * v2;
	}
}

void transform_x4(const matx3* mat, const float* vec, size_t num_vec, float* out)
{
	assert(0 == num_vec % 4);

	const float m00 = mat->m[0][0];
	const float m01 = mat->m[0][1];
	const float m02 = mat->m[0][2];
	const float m10 = mat->m[1][0];
	const float m11 = mat->m[1][1];
	const float m12 = mat->m[1][2];
	const float m20 = mat->m[2][0];
	const float m21 = mat->m[2][1];
	const float m22 = mat->m[2][2];

	for ( ; num_vec; num_vec -= 4, vec += 12, out += 12) {
		const float32x4x3_t t = vld3q_f32(vec);

		const float32x4_t r0 =
			m00 * t.val[0] +
			m01 * t.val[1] +
			m02 * t.val[2];
		const float32x4_t r1 =
			m10 * t.val[0] +
			m11 * t.val[1] +
			m12 * t.val[2];
		const float32x4_t r2 =
			m20 * t.val[0] +
			m21 * t.val[1] +
			m22 * t.val[2];

		const float32x4x3_t r = { .val = { r0, r1, r2 } };
		vst3q_f32(out, r);
	}
}

void transfort_x4(const matx4x3* mat, const float* vec, size_t num_vec, float* out)
{
	assert(0 == num_vec % 4);

	const float m00 = mat->m[0][0];
	const float m01 = mat->m[0][1];
	const float m02 = mat->m[0][2];
	const float m10 = mat->m[1][0];
	const float m11 = mat->m[1][1];
	const float m12 = mat->m[1][2];
	const float m20 = mat->m[2][0];
	const float m21 = mat->m[2][1];
	const float m22 = mat->m[2][2];
	const float32x4_t tr0 = vld1q_dup_f32(&mat->m[3][0]);
	const float32x4_t tr1 = vld1q_dup_f32(&mat->m[3][1]);
	const float32x4_t tr2 = vld1q_dup_f32(&mat->m[3][2]);

	for ( ; num_vec; num_vec -= 4, vec += 12, out += 12) {
		const float32x4x3_t t = vld3q_f32(vec);

		const float32x4_t r0 = tr0 +
			m00 * t.val[0] +
			m01 * t.val[1] +
			m02 * t.val[2];
		const float32x4_t r1 = tr1 +
			m10 * t.val[0] +
			m11 * t.val[1] +
			m12 * t.val[2];
		const float32x4_t r2 = tr2 +
			m20 * t.val[0] +
			m21 * t.val[1] +
			m22 * t.val[2];

		const float32x4x3_t r = { .val = { r0, r1, r2 } };
		vst3q_f32(out, r);
	}
}

void transform_x8(const matx3* mat, const float* vec, size_t num_vec, float* out)
{
	assert(0 == num_vec % 8);

	const float m00 = mat->m[0][0];
	const float m01 = mat->m[0][1];
	const float m02 = mat->m[0][2];
	const float m10 = mat->m[1][0];
	const float m11 = mat->m[1][1];
	const float m12 = mat->m[1][2];
	const float m20 = mat->m[2][0];
	const float m21 = mat->m[2][1];
	const float m22 = mat->m[2][2];

	for ( ; num_vec; num_vec -= 8, vec += 24, out += 24) {
		const float32x4x3_t t0 = vld3q_f32(vec +  0);
		const float32x4x3_t t1 = vld3q_f32(vec + 12);

		const float32x4_t r0 =
			m00 * t0.val[0] +
			m01 * t0.val[1] +
			m02 * t0.val[2];
		const float32x4_t r1 =
			m10 * t0.val[0] +
			m11 * t0.val[1] +
			m12 * t0.val[2];
		const float32x4_t r2 =
			m20 * t0.val[0] +
			m21 * t0.val[1] +
			m22 * t0.val[2];

		const float32x4_t r3 =
			m00 * t1.val[0] +
			m01 * t1.val[1] +
			m02 * t1.val[2];
		const float32x4_t r4 =
			m10 * t1.val[0] +
			m11 * t1.val[1] +
			m12 * t1.val[2];
		const float32x4_t r5 =
			m20 * t1.val[0] +
			m21 * t1.val[1] +
			m22 * t1.val[2];

		const float32x4x3_t s0 = { .val = { r0, r1, r2 } };
		const float32x4x3_t s1 = { .val = { r3, r4, r5 } };
		vst3q_f32(out +  0, s0);
		vst3q_f32(out + 12, s1);
	}
}

void transfort_x8(const matx4x3* mat, const float* vec, size_t num_vec, float* out)
{
	assert(0 == num_vec % 8);

	const float m00 = mat->m[0][0];
	const float m01 = mat->m[0][1];
	const float m02 = mat->m[0][2];
	const float m10 = mat->m[1][0];
	const float m11 = mat->m[1][1];
	const float m12 = mat->m[1][2];
	const float m20 = mat->m[2][0];
	const float m21 = mat->m[2][1];
	const float m22 = mat->m[2][2];
	const float32x4_t tr0 = vld1q_dup_f32(&mat->m[3][0]);
	const float32x4_t tr1 = vld1q_dup_f32(&mat->m[3][1]);
	const float32x4_t tr2 = vld1q_dup_f32(&mat->m[3][2]);

	for ( ; num_vec; num_vec -= 8, vec += 24, out += 24) {
		const float32x4x3_t t0 = vld3q_f32(vec +  0);
		const float32x4x3_t t1 = vld3q_f32(vec + 12);

		const float32x4_t r0 = tr0 +
			m00 * t0.val[0] +
			m01 * t0.val[1] +
			m02 * t0.val[2];
		const float32x4_t r1 = tr1 +
			m10 * t0.val[0] +
			m11 * t0.val[1] +
			m12 * t0.val[2];
		const float32x4_t r2 = tr2 +
			m20 * t0.val[0] +
			m21 * t0.val[1] +
			m22 * t0.val[2];

		const float32x4_t r3 = tr0 +
			m00 * t1.val[0] +
			m01 * t1.val[1] +
			m02 * t1.val[2];
		const float32x4_t r4 = tr1 +
			m10 * t1.val[0] +
			m11 * t1.val[1] +
			m12 * t1.val[2];
		const float32x4_t r5 = tr2 +
			m20 * t1.val[0] +
			m21 * t1.val[1] +
			m22 * t1.val[2];

		const float32x4x3_t s0 = { .val = { r0, r1, r2 } };
		const float32x4x3_t s1 = { .val = { r3, r4, r5 } };
		vst3q_f32(out +  0, s0);
		vst3q_f32(out + 12, s1);
	}
}

#endif
uint32_t g_sleep;

#if SMALL_DATA
float arr[] __attribute__ ((aligned(CACHELINE_SIZE))) = { // used also to debug routines, so init to something
	 0,  1,  2,
	 3,  4,  5,
	 6,  7,  8,
	 9, 10, 11,
	12, 13, 14,
	15, 16, 17,
	18, 19, 20,
	21, 22, 23,
	24, 25, 26,
	27, 28, 29,
	30, 31, 32,
	33, 34, 35,
	36, 37, 38,
	39, 40, 41,
	42, 43, 44,
	45, 46, 47,
	48, 49, 50,
	51, 52, 53,
	54, 55, 56,
	57, 58, 59,
	60, 61, 62,
	63, 64, 65,
	66, 67, 68,
	69, 70, 71
};

#else
float arr[1024 * 1024 * 3] __attribute__ ((aligned(CACHELINE_SIZE)));

#endif
float out[sizeof(arr) / sizeof(arr[0])] __attribute__ ((aligned(CACHELINE_SIZE)));

matx3 m __attribute__ ((aligned(CACHELINE_SIZE))) = {
	.m = {
		{ 2, 0, 0 },
		{ 0, 2, 0 },
		{ 0, 0, 2 }
	}
};

size_t arr_count = sizeof(arr) / sizeof(float[3]);

static void *thread_fn(void *arg)
{
#if __APPLE__
	qos_class_t qosClass = qos_class_self();
	fprintf(stdout, "worker qos: %s\n", qos_to_str(qosClass));

#endif
#if SMALL_DATA
	const size_t rep = 5e7;

#else
	const size_t rep = 1e3;

#endif
	if (g_sleep)
		sleep(g_sleep);

	for (size_t i = 0; i < rep; ++i) {
		__asm __volatile ("" ::: "memory");

#if ALT == 1
		transform_x8(&m, arr, arr_count, out);

#elif ALT == 0
		transform_x4(&m, arr, arr_count, out);

#elif ALT == -1
		transform_x1(&m, arr, arr_count, out);

#else
	#error unknown ALT

#endif
	}

#if NDEBUG == 0
	for (size_t i = 0; i < sizeof(out) / sizeof(out[0]); i += 3)
		fprintf(stdout, "%f\t%f\t%f\n",
			out[i + 0],
			out[i + 1],
			out[i + 2]);

#endif
	return NULL;
}

static char arg_sleep[] = "--sleep";

int main(int argc, char** argv)
{
	for (int argi = 1; argi < argc; ++argi) {
		if (0 == strcmp(arg_sleep, argv[argi]) && ++argi < argc && 1 == sscanf(argv[argi], "%u", &g_sleep))
			continue;

		fprintf(stderr, "usage: %s [opt..]\n"
			"\t%s u32\t: sleep u32 seconds before launching the workload\n",
			argv[0], arg_sleep);
		return -1;
	}

#if __APPLE__
	qos_class_t qosClass = qos_class_self();
	fprintf(stdout, "main qos: %s\n", qos_to_str(qosClass));

	pthread_attr_t qosAttribute;
	pthread_t thread;

	pthread_attr_init(&qosAttribute);

#if WORKER_QOS
	if (0 != pthread_attr_set_qos_class_np(&qosAttribute, QOS_CLASS_USER_INTERACTIVE, 0)) {
		fprintf(stderr, "error: pthread_attr_set_qos_class_np\n");
		return -1;
	}

#endif
	if (0 != pthread_create(&thread, &qosAttribute, thread_fn, NULL)) {
		fprintf(stderr, "error: pthread_create failed\n");
		return -1;
	}

	void *retval;

	if (0 != pthread_join(thread, &retval)) {
		fprintf(stderr, "error: pthread_join failed\n");
	}

#else
	thread_fn(NULL);

#endif
	return 0;
}
