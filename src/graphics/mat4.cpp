#include "mat4.h"

#include <math.h>
#include <string.h>

using namespace ey3;

// Column major: m[column * 4 + row], the layout glUniformMatrix4fv expects.
static inline int at(int column, int row) {
	return column * 4 + row;
}

Mat4::Mat4() {
	memset(m, 0, sizeof(m));
	m[at(0, 0)] = m[at(1, 1)] = m[at(2, 2)] = m[at(3, 3)] = 1.0f;
}

Mat4 Mat4::translation(float x, float y, float z) {
	Mat4 result;
	result.m[at(3, 0)] = x;
	result.m[at(3, 1)] = y;
	result.m[at(3, 2)] = z;
	return result;
}

Mat4 Mat4::scaling(float x, float y, float z) {
	Mat4 result;
	result.m[at(0, 0)] = x;
	result.m[at(1, 1)] = y;
	result.m[at(2, 2)] = z;
	return result;
}

Mat4 Mat4::rotation(float angleRadians, float x, float y, float z) {
	float length = sqrtf(x * x + y * y + z * z);
	if (length < 1e-6f) {
		return Mat4();
	}
	x /= length;
	y /= length;
	z /= length;

	float c = cosf(angleRadians);
	float s = sinf(angleRadians);
	float t = 1.0f - c;

	Mat4 result;
	result.m[at(0, 0)] = t * x * x + c;
	result.m[at(0, 1)] = t * x * y + s * z;
	result.m[at(0, 2)] = t * x * z - s * y;

	result.m[at(1, 0)] = t * x * y - s * z;
	result.m[at(1, 1)] = t * y * y + c;
	result.m[at(1, 2)] = t * y * z + s * x;

	result.m[at(2, 0)] = t * x * z + s * y;
	result.m[at(2, 1)] = t * y * z - s * x;
	result.m[at(2, 2)] = t * z * z + c;
	return result;
}

Mat4 Mat4::frustum(float left, float right, float bottom, float top, float near, float far) {
	Mat4 result;
	memset(result.m, 0, sizeof(result.m));

	result.m[at(0, 0)] = 2.0f * near / (right - left);
	result.m[at(1, 1)] = 2.0f * near / (top - bottom);
	result.m[at(2, 0)] = (right + left) / (right - left);
	result.m[at(2, 1)] = (top + bottom) / (top - bottom);
	result.m[at(2, 2)] = -(far + near) / (far - near);
	result.m[at(2, 3)] = -1.0f;
	result.m[at(3, 2)] = -2.0f * far * near / (far - near);
	return result;
}

Mat4 Mat4::operator*(const Mat4& other) const {
	Mat4 result;

	for (int column = 0; column < 4; column++) {
		for (int row = 0; row < 4; row++) {
			float sum = 0.0f;
			for (int i = 0; i < 4; i++) {
				sum += m[at(i, row)] * other.m[at(column, i)];
			}
			result.m[at(column, row)] = sum;
		}
	}
	return result;
}

const float* Mat4::data() const {
	return m;
}
