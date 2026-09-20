#include "maze/graphics/sprite_renderer.h"

#include "maze/graphics/sprite_sheet.h"

SpriteRenderer::SpriteRenderer()
	: vao(0), screenSizeLocation(-1), spriteRectLocation(-1), spriteFrameLocation(-1),
	  depthLocation(-1), screenWidth(0.0f), screenHeight(0.0f) {
}

void SpriteRenderer::init(AssetLoader* assetLoader, int32_t screenWidth, int32_t screenHeight) {
	this->screenWidth = (float) screenWidth;
	this->screenHeight = (float) screenHeight;

	char vShader[] = "shaders/sprite.vs";
	char fShader[] = "shaders/sprite.frag";
	shader.init(assetLoader, vShader, fShader);

	screenSizeLocation = glGetUniformLocation(shader.getProgram(), "screenSize");
	spriteRectLocation = glGetUniformLocation(shader.getProgram(), "spriteRect");
	spriteFrameLocation = glGetUniformLocation(shader.getProgram(), "spriteFrame");
	depthLocation = glGetUniformLocation(shader.getProgram(), "depth");

	const GLfloat vertices[] = {
		// Position   // TexCoords
		0.0f, 0.0f,   0.0f, 0.0f, // TL
		1.0f, 1.0f,   1.0f, 1.0f, // BR
		0.0f, 1.0f,   0.0f, 1.0f, // BL

		0.0f, 0.0f,   0.0f, 0.0f, // TL
		1.0f, 0.0f,   1.0f, 0.0f, // TR
		1.0f, 1.0f,   1.0f, 1.0f  // BR
	};

	glGenVertexArrays(1, &vao);

	GLuint vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindVertexArray(vao);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);   // Position

	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)(2 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);   // TexCoords

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void SpriteRenderer::draw(const SpriteSheet& sheet, int frame, const Rect& bounds, float depth) {
	glUseProgram(shader.getProgram());

	glUniform2f(screenSizeLocation, screenWidth, screenHeight);
	glUniform4f(spriteRectLocation, bounds.x, bounds.y, bounds.width, bounds.height);
	glUniform1f(depthLocation, depth);

	// Which slice of the strip to sample: where the frame starts, and how
	// wide one frame is, both in texture coordinates.
	float frameWidth = 1.0f / sheet.getFrameCount();
	glUniform2f(spriteFrameLocation, frame * frameWidth, frameWidth);

	glActiveTexture(GL_TEXTURE0);
	sheet.bind();

	glBindVertexArray(vao);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
}
