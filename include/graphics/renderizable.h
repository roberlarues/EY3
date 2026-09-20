#ifndef EY3RENDERIZABLE_H
#define EY3RENDERIZABLE_H

namespace ey3 {

	class Renderer;
	class AssetLoader;

	/**
	 * Interface that must implement renderizable elements.
	 * They should be added to the Renderer for display.
	 */
	class Renderizable {
		public:
			virtual void init(Renderer* renderer, AssetLoader* assetLoader) = 0;
			virtual void render() = 0;
	};
}

#endif // EY3RENDERIZABLE_H
