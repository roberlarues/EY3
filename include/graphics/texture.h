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
			bool generated;
			GLuint width, height;
			GLuint internalFormat;
			GLuint imageFormat;

			GLuint wrapS;
			GLuint wrapT;
			GLuint filterMin;
			GLuint filterMax;

		public:
			Texture();

			/**
			 * Uploads an image. format is GL_RGB or GL_RGBA; it is remembered,
			 * so update() keeps using it.
			 */
			void generate(GLubyte* data, GLuint width, GLuint height, GLuint format = GL_RGB);
			void bind() const;
			void update(GLubyte* data);

			/**
			 * Forgets the texture's name without deleting anything, for when
			 * the GL context that held it is gone. Deleting it then would be
			 * worse than leaking: the same number may already belong to a
			 * texture of the new context.
			 */
			void invalidate();
	};
}

#endif // EY3TEXTURE_H
