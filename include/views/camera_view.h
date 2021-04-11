#ifndef EY3CAMERAVIEW_H 
#define EY3CAMERAVIEW_H 

#include <opencv2/opencv.hpp>

#include "renderizable.h"
#include "shader.h"
#include "texture.h"

namespace ey3 {

	/**
	 * Displays a live background, for the image camera.
	 */
	class CameraView: public Renderizable {
		private:
			Shader shader;
			Texture texture;
			GLuint vao;
			bool hasFrame;
			int32_t frameWidth, frameHeight;
			int32_t imageRotation;

		public:
			CameraView(int32_t frameWidth, int32_t frameHeigh);
			void init(android_app* app);
			void render();
			void setFrame(cv::Mat* frame);
	};

}

#endif // EY3CAMERAVIEW_H 
