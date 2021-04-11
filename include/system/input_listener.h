#ifndef EY3INPUTLISTENER_H 
#define EY3INPUTLISTENER_H

#include <android_native_app_glue.h>

namespace ey3 {

	/**
	 * Interface that must implement user-input listeners
	 * They should be added to the InputHandler for event handling
	 */
	class InputListener {
		public:
			virtual void handleInput(AInputEvent* event) = 0;
	};

}

#endif // EY3INPUTLISTENER_H 
