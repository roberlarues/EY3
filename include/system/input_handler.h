#ifndef EY3INPUTHANDLER_H
#define EY3INPUTHANDLER_H

#include <vector>
#include "input_listener.h"

namespace ey3 {

	/**
	 * Handles user input events.
	 */
	class InputHandler: public InputListener {
		private:
			std::vector< InputListener* > inputListeners;
		public:
			void handleInput(const InputEvent& event);
			void addListener(InputListener* listener);
			void removeListener(InputListener* listener);
	};

}

#endif // EY3INPUTHANDLER_H
