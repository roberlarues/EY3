#include "maze/graphics/mesh.h"

#include <math.h>

#include <vector>

// position (3) + normal (3) + texture coordinates (2)
static const int FLOATS_PER_VERTEX = 8;

Mesh::Mesh() : vao(0), vertexCount(0) {
}

void Mesh::load(const float* vertices, int vertexCount) {
	this->vertexCount = vertexCount;

	glGenVertexArrays(1, &vao);

	GLuint vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, vertexCount * FLOATS_PER_VERTEX * sizeof(float),
	             vertices, GL_STATIC_DRAW);

	glBindVertexArray(vao);
	GLsizei stride = FLOATS_PER_VERTEX * sizeof(float);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (GLvoid*)0);
	glEnableVertexAttribArray(0);   // position

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (GLvoid*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);   // normal

	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, (GLvoid*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);   // texture coordinates

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void Mesh::draw() const {
	if (vertexCount == 0) {
		return;
	}
	glBindVertexArray(vao);
	glDrawArrays(GL_TRIANGLES, 0, vertexCount);
	glBindVertexArray(0);
}

bool Mesh::isLoaded() const {
	return vertexCount > 0;
}

namespace {

	void addVertex(std::vector<float>& out, float px, float py, float pz,
	               float nx, float ny, float nz, float u, float v) {
		out.push_back(px); out.push_back(py); out.push_back(pz);
		out.push_back(nx); out.push_back(ny); out.push_back(nz);
		out.push_back(u); out.push_back(v);
	}

	/** Two triangles from the four corners of a face, in order. */
	void addQuad(std::vector<float>& out,
	             const float a[3], const float b[3], const float c[3], const float d[3],
	             float nx, float ny, float nz) {
		addVertex(out, a[0], a[1], a[2], nx, ny, nz, 0.0f, 1.0f);
		addVertex(out, b[0], b[1], b[2], nx, ny, nz, 1.0f, 1.0f);
		addVertex(out, c[0], c[1], c[2], nx, ny, nz, 1.0f, 0.0f);

		addVertex(out, a[0], a[1], a[2], nx, ny, nz, 0.0f, 1.0f);
		addVertex(out, c[0], c[1], c[2], nx, ny, nz, 1.0f, 0.0f);
		addVertex(out, d[0], d[1], d[2], nx, ny, nz, 0.0f, 0.0f);
	}
}

Mesh Mesh::plane() {
	const float h = 0.5f;
	const float a[3] = {-h, -h, 0.0f}, b[3] = {h, -h, 0.0f};
	const float c[3] = {h, h, 0.0f}, d[3] = {-h, h, 0.0f};

	std::vector<float> vertices;
	addQuad(vertices, a, b, c, d, 0.0f, 0.0f, 1.0f);

	Mesh mesh;
	mesh.load(vertices.data(), (int) vertices.size() / FLOATS_PER_VERTEX);
	return mesh;
}

Mesh Mesh::cube() {
	const float h = 0.5f;
	std::vector<float> vertices;

	// Corners, named by sign: (x, y, z) each -h or +h.
	const float nnn[3] = {-h, -h, -h}, pnn[3] = {h, -h, -h};
	const float ppn[3] = {h, h, -h},   npn[3] = {-h, h, -h};
	const float nnp[3] = {-h, -h, h},  pnp[3] = {h, -h, h};
	const float ppp[3] = {h, h, h},    npp[3] = {-h, h, h};

	addQuad(vertices, nnp, pnp, ppp, npp,  0.0f,  0.0f,  1.0f);   // front
	addQuad(vertices, pnn, nnn, npn, ppn,  0.0f,  0.0f, -1.0f);   // back
	addQuad(vertices, pnp, pnn, ppn, ppp,  1.0f,  0.0f,  0.0f);   // right
	addQuad(vertices, nnn, nnp, npp, npn, -1.0f,  0.0f,  0.0f);   // left
	addQuad(vertices, npp, ppp, ppn, npn,  0.0f,  1.0f,  0.0f);   // top
	addQuad(vertices, nnn, pnn, pnp, nnp,  0.0f, -1.0f,  0.0f);   // bottom

	Mesh mesh;
	mesh.load(vertices.data(), (int) vertices.size() / FLOATS_PER_VERTEX);
	return mesh;
}

Mesh Mesh::sphere(int rings, int sectors) {
	std::vector<float> vertices;
	const float radius = 0.5f;

	for (int ring = 0; ring < rings; ring++) {
		float phi0 = (float) M_PI * ring / rings;
		float phi1 = (float) M_PI * (ring + 1) / rings;

		for (int sector = 0; sector < sectors; sector++) {
			float theta0 = 2.0f * (float) M_PI * sector / sectors;
			float theta1 = 2.0f * (float) M_PI * (sector + 1) / sectors;

			// The four corners of one patch, as unit normals.
			float n[4][3] = {
				{sinf(phi0) * cosf(theta0), cosf(phi0), sinf(phi0) * sinf(theta0)},
				{sinf(phi0) * cosf(theta1), cosf(phi0), sinf(phi0) * sinf(theta1)},
				{sinf(phi1) * cosf(theta1), cosf(phi1), sinf(phi1) * sinf(theta1)},
				{sinf(phi1) * cosf(theta0), cosf(phi1), sinf(phi1) * sinf(theta0)}
			};
			float u[4] = {(float) sector / sectors, (float) (sector + 1) / sectors,
			              (float) (sector + 1) / sectors, (float) sector / sectors};
			float v[4] = {(float) ring / rings, (float) ring / rings,
			              (float) (ring + 1) / rings, (float) (ring + 1) / rings};

			const int triangles[6] = {0, 1, 2, 0, 2, 3};
			for (int i = 0; i < 6; i++) {
				int k = triangles[i];
				addVertex(vertices, n[k][0] * radius, n[k][1] * radius, n[k][2] * radius,
				          n[k][0], n[k][1], n[k][2], u[k], v[k]);
			}
		}
	}

	Mesh mesh;
	mesh.load(vertices.data(), (int) vertices.size() / FLOATS_PER_VERTEX);
	return mesh;
}
