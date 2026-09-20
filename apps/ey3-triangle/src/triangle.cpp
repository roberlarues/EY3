#include <ey3.h>
#include "triangle.h"

const static GLfloat vertices[] = {
	 0.0f,  0.5f, -1.0f,
	-0.5f, -0.5f, -1.0f,
	 0.5f, -0.5f, -1.0f
};

void Triangle::init(Renderer* renderer, AssetLoader* assetLoader) {
	char vShader[] = "shaders/triangle.vs";
	char fShader[] = "shaders/triangle.frag";

	shader.init(assetLoader, vShader, fShader);

	windowWidth = renderer->getWidth();
	windowHeight = renderer->getHeight();

	glGenVertexArrays(1, &vao);

	GLuint vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindVertexArray(vao);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void Triangle::render() {
	glUseProgram(shader.getProgram());

	GLint colorLocation = glGetUniformLocation(shader.getProgram(), "color");
	glUniform3f(colorLocation, r, g, b);

	glBindVertexArray(vao);
	glDrawArrays(GL_TRIANGLES, 0, 3);
	glBindVertexArray(0);
}

void Triangle::handleInput(const InputEvent& event) {
	if (event.type == InputEventType::POINTER_DOWN) {
		int32_t touchX = (int32_t) event.x;
		int32_t touchY = (int32_t) event.y;
		if (touchX > windowWidth / 2) {
			if (touchY > windowHeight / 2) {
				// Set yellow
				r = 1.0;
				g = 1.0;
				b = 0.0;
			} else {
				// Set green
				r = 0.0;
				g = 1.0;
				b = 0.0;
			}
		} else {
			if (touchY > windowHeight / 2) {
				// Set blue
				r = 0.0;
				g = 0.0;
				b = 1.0;
			} else {
				// Set red
				r = 1.0;
				g = 0.0;
				b = 0.0;
			}
		}
	} else if (event.type == InputEventType::POINTER_UP) {
		r = 0.0;
		g = 0.0;
		b = 0.0;
	}
}
