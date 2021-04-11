#ifndef TRIANGLE_H
#define TRIANGLE_H

#include <ey3.h>

using namespace ey3;

/**
 * A triangle that changes its color on user input.
 */
class Triangle : public Renderizable, public InputListener {
	private:
		Shader shader;
		float r = 0.0;
		float g = 0.0;
		float b = 0.0;
		int32_t windowWidth = 0.0;
		int32_t windowHeight = 0.0;
	public:
		void init(android_app* app);
		void render();
		void handleInput(AInputEvent* event);
};

#endif // TRIANGLE_H 
