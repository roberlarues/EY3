#ifndef EY3SHADER_H
#define EY3SHADER_H

#include <string>
#include <vector>

#if __ANDROID_API__ >= 24
#include <GLES3/gl32.h>
#elif __ANDROID_API__ >= 21
#include <GLES3/gl31.h>
#else
#include <GLES3/gl3.h>
#endif

namespace ey3 {

	class AssetLoader;

	/**
	 * Represents a combination of vertex and fragment shader, linked to a program
	 */
	class Shader {
		private:
			static const std::string DEFAULT_VS;
			static const std::string DEFAULT_FS;
			GLuint programObject;
			GLuint loadShader( GLenum type, const char *shaderSrc);
		public:
			Shader();
			virtual ~Shader();
			void init(AssetLoader* assetLoader, const char* vShader, const char* fShader);
			void init();
			GLuint getProgram();
	};
}
#endif // EY3SHADER_H
