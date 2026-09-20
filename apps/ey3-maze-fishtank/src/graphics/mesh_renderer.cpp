#include "maze/graphics/mesh_renderer.h"

#include "maze/graphics/sprite_sheet.h"

// Enough segments that the ball reads as round at the size it is drawn.
static const int SPHERE_RINGS = 16;
static const int SPHERE_SECTORS = 24;

MeshRenderer::MeshRenderer()
	: modelLocation(-1), viewProjectionLocation(-1), spriteFrameLocation(-1),
	  tintLocation(-1), ready(false), contextGeneration(0) {
}

void MeshRenderer::init(Renderer* renderer, AssetLoader* assetLoader) {
	int32_t generation = renderer->getContextGeneration();
	if (ready && generation == contextGeneration) {
		return;   // same context: shader and meshes are still alive
	}
	contextGeneration = generation;
	ready = true;

	char vShader[] = "shaders/mesh.vs";
	char fShader[] = "shaders/mesh.frag";
	shader.init(assetLoader, vShader, fShader);

	modelLocation = glGetUniformLocation(shader.getProgram(), "model");
	viewProjectionLocation = glGetUniformLocation(shader.getProgram(), "viewProjection");
	spriteFrameLocation = glGetUniformLocation(shader.getProgram(), "spriteFrame");
	tintLocation = glGetUniformLocation(shader.getProgram(), "tint");

	// Every shape here is wound counter-clockwise when seen from outside, so
	// the faces pointing away from the player can be thrown away before they
	// are shaded.
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);

	planeMesh = Mesh::plane();
	cubeMesh = Mesh::cube();
	sphereMesh = Mesh::sphere(SPHERE_RINGS, SPHERE_SECTORS);
}

void MeshRenderer::setViewProjection(const Mat4& viewProjection) {
	this->viewProjection = viewProjection;
}

void MeshRenderer::draw(const Mesh& mesh, const Sprite& sprite, const Mat4& model, const Vec3& tint) {
	const SpriteSheet* sheet = sprite.getSheet();
	if (!ready || sheet == nullptr || !mesh.isLoaded()) {
		return;
	}

	glUseProgram(shader.getProgram());
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, model.data());
	glUniformMatrix4fv(viewProjectionLocation, 1, GL_FALSE, viewProjection.data());
	glUniform3f(tintLocation, tint.x, tint.y, tint.z);

	// Which slice of the sheet to sample: where the frame starts and how
	// wide one frame is, in texture coordinates.
	float frameWidth = 1.0f / sheet->getFrameCount();
	glUniform2f(spriteFrameLocation, sprite.getFrame() * frameWidth, frameWidth);

	glActiveTexture(GL_TEXTURE0);
	sheet->bind();

	mesh.draw();
}

void MeshRenderer::drawPlane(const Sprite& sprite, const Mat4& model, const Vec3& tint) {
	draw(planeMesh, sprite, model, tint);
}

void MeshRenderer::drawCube(const Sprite& sprite, const Mat4& model, const Vec3& tint) {
	draw(cubeMesh, sprite, model, tint);
}

void MeshRenderer::drawSphere(const Sprite& sprite, const Mat4& model, const Vec3& tint) {
	draw(sphereMesh, sprite, model, tint);
}
