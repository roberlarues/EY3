#include "input_handler.h"

using namespace ey3;

void InputHandler::handleInput(AInputEvent* event) {
	for (int i = 0; i < inputListeners.size(); i++) {
		inputListeners[i]->handleInput(event);
	}
}

void InputHandler::addListener(InputListener* listener) {
	inputListeners.push_back(listener);
}

void InputHandler::removeListener(InputListener* listener) {
	int i = 0;
	while (i < inputListeners.size()) {
		if (inputListeners[i] == listener) {
			inputListeners.erase(inputListeners.begin() + i);
		} else {
			i++;
		}
	}
}

