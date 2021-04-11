#include <ey3.h>
#include "triangle.h"

const static GLfloat vertices[] = {
	 0.0f,  0.5f, -1.0f,
	-0.5f, -0.5f, -1.0f,
	 0.5f, -0.5f, -1.0f
};

void Triangle::init(android_app* app) {
	char vShader[] = "shaders/triangle.vs";
	char fShader[] = "shaders/triangle.frag";

	shader.init(app, vShader, fShader);

	Engine* engine = (Engine*) app->userData;
	windowWidth = engine->getRenderer()->getWidth();
	windowHeight = engine->getRenderer()->getHeight();
}

void Triangle::render() {
	glUseProgram(shader.getProgram());

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, vertices);
	GLint colorLocation = glGetUniformLocation(shader.getProgram(), "color");
	glUniform3f(colorLocation, r, g, b);
	glEnableVertexAttribArray(0);

	glDrawArrays(GL_TRIANGLES, 0, 3);
}

void Triangle::handleInput(AInputEvent* event) {
	if (AInputEvent_getType(event) == AINPUT_EVENT_TYPE_MOTION) {
		switch(AInputEvent_getSource(event)){
			case AINPUT_SOURCE_TOUCHSCREEN:
				int action = AKeyEvent_getAction(event) & AMOTION_EVENT_ACTION_MASK;
				switch(action){
					case AMOTION_EVENT_ACTION_DOWN: {
						int32_t touchX = AMotionEvent_getX(event, 0);
						int32_t touchY = AMotionEvent_getY(event, 0);
						if (touchX > windowWidth / 2) {
							if (touchY > windowHeight / 2) {
								// Set yellow 
								r = 1.0;
								g = 1.0;
								b = 0.0;
							} else {
								// Set green
								r = 0.0;
								g = 1.0;
								b = 0.0;
							}
						} else {
							if (touchY > windowHeight / 2) {
								// Set blue 
								r = 0.0;
								g = 0.0;
								b = 1.0;
							} else {
								// Set red
								r = 1.0;
								g = 0.0;
								b = 0.0;
							}
						}
						break;
					}
					case AMOTION_EVENT_ACTION_UP: {
						r = 0.0;
						g = 0.0;
						b = 0.0;
						break;
					}
				}
			break;
		}
	}
}

