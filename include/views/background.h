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
			GLubyte* pixels;
		public:
			Background(GLubyte* pixels, GLuint width, GLuint height);
			void init(android_app* app);
			void render();
	};

}

#endif // EY3BACKGROUND_H
