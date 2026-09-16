#ifndef EY3WINDOWDESKTOP_H
#define EY3WINDOWDESKTOP_H

#include "window.h"

struct GLFWwindow;

namespace ey3 {

	/**
	 * IWindow backed by GLFW, requesting an EGL/OpenGL ES 3.0 context so the
	 * shared renderer/shader code (written against GLES3, "#version 300 es")
	 * runs unmodified on desktop (Mesa supports GLES natively via EGL).
	 */
	class WindowDesktop: public IWindow {
		private:
			GLFWwindow* glfwWindow = nullptr;
			Engine* engine = nullptr;
			int32_t width;
			int32_t height;

			static void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
			static void cursorPosCallback(GLFWwindow* window, double x, double y);
			static void framebufferSizeCallback(GLFWwindow* window, int width, int height);

		public:
			WindowDesktop(const char* title, int32_t width, int32_t height);
			~WindowDesktop();

			void setEngine(Engine* engine) override;
			bool createSurface() override;
			void destroySurface() override;
			bool makeCurrent() override;
			void swapBuffers() override;
			void pollEvents() override;
			int32_t getWidth() override;
			int32_t getHeight() override;
	};
}

#endif // EY3WINDOWDESKTOP_H
