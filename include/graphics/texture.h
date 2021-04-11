#ifndef EY3TEXTURE_H
#define EY3TEXTURE_H

#define GLEW_STATIC
#if __ANDROID_API__ >= 24
#include <GLES3/gl32.h>
#elif __ANDROID_API__ >= 21
#include <GLES3/gl31.h>
#else
#include <GLES3/gl3.h>
#endif

namespace ey3 {

	/**
	 * Represents a texture for GL objects
	 */
	class Texture {
		private:
			GLuint id;
			GLuint width, height;
			GLuint internalFormat;
			GLuint imageFormat;

			GLuint wrapS;
			GLuint wrapT;
			GLuint filterMin;
			GLuint filterMax;

		public:
			Texture();
			void generate(GLubyte* data, GLuint width, GLuint height);
			void bind() const;
			void update(GLubyte* data);
	};
}

#endif // EY3TEXTURE_H
