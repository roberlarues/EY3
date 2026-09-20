#include "shader.h"
#include "log.h"
#include "asset_loader.h"

using namespace ey3;

Shader::Shader() {
}

Shader::~Shader() {
}

GLuint Shader::loadShader( GLenum type, const char *shaderSrc) {
	GLuint shader;
	GLint compiled;

	shader = glCreateShader(type);
	if ( shader == 0 ) {
		return 0;
	}

	glShaderSource(shader, 1, &shaderSrc, NULL);
	glCompileShader(shader);
	glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);

	if (!compiled) {
		GLchar infoLog[1024];
		glGetShaderInfoLog(shader, 1024, NULL, infoLog);
		LOGE("Could not compile the shader: %s", infoLog);
		glDeleteShader(shader);
		return 0;
	}

	return shader;
}

void Shader::init(AssetLoader* assetLoader, const char* vShader, const char* fShader) {
	const std::string vs = assetLoader->loadStringAsset(vShader);
	const std::string fs = assetLoader->loadStringAsset(fShader);

	GLuint vertexShader = loadShader( GL_VERTEX_SHADER, vs.c_str() );
	GLuint fragmentShader = loadShader( GL_FRAGMENT_SHADER, fs.c_str() );

	programObject = glCreateProgram();

	if ( programObject == 0 ) {
		LOGE("Could not create the shader program");
		return;
	}

	glAttachShader( programObject, vertexShader );
	glAttachShader( programObject, fragmentShader );

	glLinkProgram( programObject );

	GLint linked;
	glGetProgramiv( programObject, GL_LINK_STATUS, &linked );

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

	if ( !linked ) {
		GLchar infoLog[1024];
		glGetProgramInfoLog( programObject, 1024, NULL, infoLog );
		LOGE("Could not link the shaders: %s", infoLog);

		glDeleteProgram ( programObject );
		return ;
	}

	LOGI("Shader loaded OK");
}

void Shader::init() {

	GLuint vertexShader = loadShader( GL_VERTEX_SHADER, DEFAULT_VS.c_str() );
	GLuint fragmentShader = loadShader( GL_FRAGMENT_SHADER, DEFAULT_FS.c_str() );

	programObject = glCreateProgram();

	if ( programObject == 0 ) {
		LOGE("Could not create the shader program");
		return;
	}

	glAttachShader( programObject, vertexShader );
	glAttachShader( programObject, fragmentShader );

	glLinkProgram( programObject );

	GLint linked;
	glGetProgramiv( programObject, GL_LINK_STATUS, &linked );

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

	if ( !linked ) {
		GLchar infoLog[1024];
		glGetProgramInfoLog( programObject, 1024, NULL, infoLog );
		LOGE("Could not link the shaders: %s", infoLog);

		glDeleteProgram ( programObject );
		return ;
	}

	LOGI("Shader loaded OK");
}

GLuint Shader::getProgram() {
	return programObject;
}

// DEFAULT SHADERS
const std::string Shader::DEFAULT_VS = R"(
	#version 300 es
	layout(location = 0) in vec3 position;
	layout(location = 1) in vec2 texCoords;
	out vec2 texCoordsOut;
	void main() {
		texCoordsOut = texCoords;
		gl_Position = vec4(position, 1.0);
	})";

const std::string Shader::DEFAULT_FS = R"(
	#version 300 es
	precision mediump float;
	in vec2 texCoordsOut;
	out vec4 color;
	uniform sampler2D image;
	void main() {
		color = texture(image, texCoordsOut);
	})";
