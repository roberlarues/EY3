#ifndef EY3INPUTLISTENER_H
#define EY3INPUTLISTENER_H

#include "input_event.h"

namespace ey3 {

	/**
	 * Interface that must implement user-input listeners
	 * They should be added to the InputHandler for event handling
	 */
	class InputListener {
		public:
			virtual void handleInput(const InputEvent& event) = 0;
	};

}

#endif // EY3INPUTLISTENER_H
