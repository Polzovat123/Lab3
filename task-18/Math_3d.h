#pragma once
#include <glm/glm.hpp>

namespace VectorsMath {
	const int x = 0, y = 1, z = 2;

	glm::vec3 Cross(glm::vec3& v1, glm::vec3& v2) {
		const float _x = v1[x] * v2[z] - v1[z] * v2[y];
		const float _y = v1[z] * v2[x] - v1[x] * v2[z];
		const float _z = v1[x] * v2[y] - v1[y] * v2[x];

		return glm::vec3{_x, _y, _z};
	}

	void Normalize(glm::vec3& v) {
		const float Length = sqrtf(v[x] * v[x] + v[y] * v[y] + v[z] * v[z]);

		v[x] /= Length;
		v[y] /= Length;
		v[z] /= Length;
	}
};
