#include "background.h"

using namespace ey3;

Background::Background(GLubyte* pixels, GLuint width, GLuint height, GLuint format)
	: width(width), height(height), format(format), pixels(pixels) {
}

void Background::init(Renderer* renderer, AssetLoader* assetLoader) {

	GLfloat vertices[] {
		// Pos              // Tex
		-1.0f, -1.0f, 0.0f, 0.0f, 1.0f, // T1 BL
		 1.0f,  1.0f, 0.0f, 1.0f, 0.0f, // T1 TR
		-1.0f,  1.0f, 0.0f, 0.0f, 0.0f, // T1 TL

		-1.0f, -1.0f, 0.0f, 0.0f, 1.0f, // T2 BL
		 1.0f, -1.0f, 0.0f, 1.0f, 1.0f, // T2 BR
		 1.0f,  1.0f, 0.0f, 1.0f, 0.0f  // T2 TR
	};

	shader.init();

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	texture.generate(pixels, width, height, format);

	glGenVertexArrays(1, &vao);

	GLuint vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindVertexArray(vao);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);   // Position

	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);   // TexCoords

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void Background::render() {
	glUseProgram(shader.getProgram());

	glActiveTexture(GL_TEXTURE0);
	texture.bind();

	glBindVertexArray(vao);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
}

