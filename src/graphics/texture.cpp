#include "texture.h"
#include <iostream>
#include "log.h"

using namespace ey3;

Texture::Texture() : generated(false), width(0), height(0), internalFormat(GL_RGB), imageFormat(GL_RGB), wrapS(GL_CLAMP_TO_EDGE), wrapT(GL_CLAMP_TO_EDGE), filterMin(GL_NEAREST), filterMax(GL_NEAREST) { }

void Texture::generate(GLubyte* data, GLuint width, GLuint height, GLuint format) {
	this->width = width;
	this->height = height;
	this->internalFormat = format;
	this->imageFormat = format;

	if (generated) {
		// Replacing an image we still own: give the old name back rather
		// than leak it. Only safe because generated is cleared by
		// invalidate() when the context goes away.
		glDeleteTextures(1, &id);
	}
	glGenTextures(1, &id);
	generated = true;
	glBindTexture(GL_TEXTURE_2D, id);
	glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, imageFormat, GL_UNSIGNED_BYTE, data);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapS);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filterMin);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filterMax);

	glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::bind() const {
	glBindTexture(GL_TEXTURE_2D, id);
}

void Texture::invalidate() {
	generated = false;
}

void Texture::update(GLubyte* data) {
	glBindTexture(GL_TEXTURE_2D, id);
	glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, imageFormat, GL_UNSIGNED_BYTE, data);
	glBindTexture(GL_TEXTURE_2D, 0);
}

