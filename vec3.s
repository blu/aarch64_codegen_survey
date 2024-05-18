/*
 * transfor{m,t}_x{4,8} etalon routine for vec3.c reference
 *
 * Copyright 2024 Martin Krastev <blu.dark@gmail.com>
 * SPDX-License-Identifier: MIT
 */

	.arch armv8-a

	.global transform_x4
	.global transform_x8
	.global transfort_x4
	.global transfort_x8
	.text

	.align 4

/*
	void transform_x4(const matx3* mat, const float* vec, size_t num_vec, float* out)
*/
transform_x4:
	ldp	q0, q1, [x0]
	ldr	s2, [x0, #32]
/*
	matx3x3 layout in least-to-most-significant reg lanes
	    q0: m00, m01, m02, m10
	    q1: m11, m12, m20, m21
	    s2: m22
*/
	cbz	x2, 2f
	.align 4
1:
	ld3	{v16.4s-v18.4s}, [x1], #48

	fmul	v19.4s, v16.4s, v0.s[0]
	fmul	v20.4s, v16.4s, v0.s[3]
	fmul	v21.4s, v16.4s, v1.s[2]
	fmla	v19.4s, v17.4s, v0.s[1]
	fmla	v20.4s, v17.4s, v1.s[0]
	fmla	v21.4s, v17.4s, v1.s[3]
	fmla	v19.4s, v18.4s, v0.s[2]
	fmla	v20.4s, v18.4s, v1.s[1]
	fmla	v21.4s, v18.4s, v2.s[0]

	st3	{v19.4s-v21.4s}, [x3], #48

	subs	x2, x2, #0x4
	b.ne	1b
2:
	ret

/*
	void transform_x8(const matx3* mat, const float* vec, size_t num_vec, float* out)
*/
transform_x8:
	ldp	q0, q1, [x0]
	ldr	s2, [x0, #32]
/*
	matx3x3 layout in least-to-most-significant reg lanes
	    q0: m00, m01, m02, m10
	    q1: m11, m12, m20, m21
	    s2: m22
*/
	cbz	x2, 2f
	add	x4, x1, #48
	add	x5, x3, #48
	mov	x6, #96
	.align 4
1:
	ld3	{v16.4s-v18.4s}, [x1], x6
	ld3	{v22.4s-v24.4s}, [x4], x6

	fmul	v19.4s, v16.4s, v0.s[0]
	fmul	v20.4s, v16.4s, v0.s[3]
	fmul	v21.4s, v16.4s, v1.s[2]

	fmul	v25.4s, v22.4s, v0.s[0]
	fmul	v26.4s, v22.4s, v0.s[3]
	fmul	v27.4s, v22.4s, v1.s[2]

	fmla	v19.4s, v17.4s, v0.s[1]
	fmla	v20.4s, v17.4s, v1.s[0]
	fmla	v21.4s, v17.4s, v1.s[3]

	fmla	v25.4s, v23.4s, v0.s[1]
	fmla	v26.4s, v23.4s, v1.s[0]
	fmla	v27.4s, v23.4s, v1.s[3]

	fmla	v19.4s, v18.4s, v0.s[2]
	fmla	v20.4s, v18.4s, v1.s[1]
	fmla	v21.4s, v18.4s, v2.s[0]

	fmla	v25.4s, v24.4s, v0.s[2]
	fmla	v26.4s, v24.4s, v1.s[1]
	fmla	v27.4s, v24.4s, v2.s[0]

	st3	{v19.4s-v21.4s}, [x3], x6
	st3	{v25.4s-v27.4s}, [x5], x6

	subs	x2, x2, #0x8
	b.ne	1b
2:
	ret

/*
	void transfort_x4(const matx4x3* mat, const float* vec, size_t num_vec, float* out)
*/
transfort_x4:
	ldp	q0, q1, [x0]
	ldr	q2, [x0, #32]
/*
	matx3x3 layout in least-to-most-significant reg lanes
	    q0: m00, m01, m02, m10
	    q1: m11, m12, m20, m21
	    s2: m22
*/
	cbz	x2, 2f
	dup	v3.4s, v2.s[1]
	dup	v4.4s, v2.s[2]
	dup	v5.4s, v2.s[3]
	.align 4
1:
	ld3	{v16.4s-v18.4s}, [x1], #48

	mov	v19.16b, v3.16b
	mov	v20.16b, v4.16b
	mov	v21.16b, v5.16b

	fmla	v19.4s, v16.4s, v0.s[0]
	fmla	v20.4s, v16.4s, v0.s[3]
	fmla	v21.4s, v16.4s, v1.s[2]
	fmla	v19.4s, v17.4s, v0.s[1]
	fmla	v20.4s, v17.4s, v1.s[0]
	fmla	v21.4s, v17.4s, v1.s[3]
	fmla	v19.4s, v18.4s, v0.s[2]
	fmla	v20.4s, v18.4s, v1.s[1]
	fmla	v21.4s, v18.4s, v2.s[0]

	st3	{v19.4s-v21.4s}, [x3], #48

	subs	x2, x2, #0x4
	b.ne	1b
2:
	ret

/*
	void transfort_x8(const matx4x3* mat, const float* vec, size_t num_vec, float* out)
*/
transfort_x8:
	ldp	q0, q1, [x0]
	ldr	q2, [x0, #32]
/*
	matx3x3 layout in least-to-most-significant reg lanes
	    q0: m00, m01, m02, m10
	    q1: m11, m12, m20, m21
	    s2: m22
*/
	cbz	x2, 2f
	add	x4, x1, #48
	add	x5, x3, #48
	mov	x6, #96
	dup	v3.4s, v2.s[1]
	dup	v4.4s, v2.s[2]
	dup	v5.4s, v2.s[3]
	.align 4
1:
	ld3	{v16.4s-v18.4s}, [x1], x6
	ld3	{v22.4s-v24.4s}, [x4], x6

	mov	v19.16b, v3.16b
	mov	v20.16b, v4.16b
	mov	v21.16b, v5.16b

	mov	v25.16b, v3.16b
	mov	v26.16b, v4.16b
	mov	v27.16b, v5.16b

	fmla	v19.4s, v16.4s, v0.s[0]
	fmla	v20.4s, v16.4s, v0.s[3]
	fmla	v21.4s, v16.4s, v1.s[2]

	fmla	v25.4s, v22.4s, v0.s[0]
	fmla	v26.4s, v22.4s, v0.s[3]
	fmla	v27.4s, v22.4s, v1.s[2]

	fmla	v19.4s, v17.4s, v0.s[1]
	fmla	v20.4s, v17.4s, v1.s[0]
	fmla	v21.4s, v17.4s, v1.s[3]

	fmla	v25.4s, v23.4s, v0.s[1]
	fmla	v26.4s, v23.4s, v1.s[0]
	fmla	v27.4s, v23.4s, v1.s[3]

	fmla	v19.4s, v18.4s, v0.s[2]
	fmla	v20.4s, v18.4s, v1.s[1]
	fmla	v21.4s, v18.4s, v2.s[0]

	fmla	v25.4s, v24.4s, v0.s[2]
	fmla	v26.4s, v24.4s, v1.s[1]
	fmla	v27.4s, v24.4s, v2.s[0]

	st3	{v19.4s-v21.4s}, [x3], x6
	st3	{v25.4s-v27.4s}, [x5], x6

	subs	x2, x2, #0x8
	b.ne	1b
2:
	ret
