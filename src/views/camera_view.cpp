#include "camera_view.h"
#include "engine.h"
#include "log.h"

using namespace cv;
using namespace ey3;

CameraView::CameraView(int32_t frameWidth, int32_t frameHeight): frameWidth(frameWidth), frameHeight(frameHeight), imageRotation(0) {
}

void CameraView::init(android_app* app) {
	Engine* engine = (Engine*) app->userData;

	hasFrame = false;
	int32_t screenWidth = engine->getRenderer()->getWidth();
	int32_t screenHeight = engine->getRenderer()->getHeight();

	float arScreen = screenWidth * 1.0f / screenHeight;
	float arFrame = frameWidth * 1.0f / frameHeight;
	
	GLfloat w, h;
	if (arFrame > arScreen) {
		w = 1.0f;
		h = arScreen * 1.f / arFrame;
	} else {
		h = 1.0f;
		w = arFrame * 1.f / arScreen;
	}

	// Correct the camera rotation
	float u[4];
	float v[4];
	u[(-imageRotation/90 + 4) % 4] = 0.0f;
	v[(-imageRotation/90 + 4) % 4] = 0.0f;
	u[(-imageRotation/90 + 5) % 4] = 0.0f;
	v[(-imageRotation/90 + 5) % 4] = 1.0f;
	u[(-imageRotation/90 + 6) % 4] = 1.0f;
	v[(-imageRotation/90 + 6) % 4] = 1.0f;
	u[(-imageRotation/90 + 7) % 4] = 1.0f;
	v[(-imageRotation/90 + 7) % 4] = 0.0f;

	GLfloat vertices[] {
		// Pos              // Tex
		-w, -h, 0.0f, u[1], v[1], // T1 BL
		 w,  h, 0.0f, u[3], v[3], // T1 TR
		-w,  h, 0.0f, u[0], v[0], // T1 TL

		-w, -h, 0.0f, u[1], v[1], // T2 BL
		 w, -h, 0.0f, u[2], v[2], // T2 BR
		 w,  h, 0.0f, u[3], v[3]  // T2 TR
	};

	//char vShader[] = "shaders/bg.vs";
	//char fShader[] = "shaders/bg.frag";
	//shader.init(app, vShader, fShader);
	shader.init(app);

	glGenVertexArrays(1, &vao);

	GLuint vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindVertexArray(vao);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);   // Position

	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);   // TexCoords

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void CameraView::render() {
	if (hasFrame) {
		glUseProgram(shader.getProgram());

		glActiveTexture(GL_TEXTURE0);
		texture.bind();

		glBindVertexArray(vao);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glBindVertexArray(0);
	}
}

void CameraView::setFrame(Mat* frame) {
	if (!hasFrame) {
		LOGI("Setting up frame %dx%d", frame->cols, frame->rows);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		texture.generate(frame->ptr(), frame->cols, frame->rows);
		hasFrame = true;
	} else {
		texture.update(frame->ptr());
	}
}
