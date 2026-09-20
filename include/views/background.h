#ifndef EY3BACKGROUND_H
#define EY3BACKGROUND_H

#include "renderizable.h"
#include "shader.h"
#include "texture.h"

namespace ey3 {

	/**
	 * Displays a full screen background with a texture
	 */
	class Background: public Renderizable {
		private:
			Shader shader;
			Texture texture;
			GLuint vao;

			GLuint width, height;
			GLuint format;
			GLubyte* pixels;
		public:
			/** format is GL_RGB or GL_RGBA, matching the pixels given. */
			Background(GLubyte* pixels, GLuint width, GLuint height, GLuint format = GL_RGB);
			void init(Renderer* renderer, AssetLoader* assetLoader);
			void render();
	};

}

#endif // EY3BACKGROUND_H
