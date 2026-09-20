#ifndef EY3INPUTEVENT_H
#define EY3INPUTEVENT_H

#include <cstdint>

namespace ey3 {

	enum class InputEventType {
		POINTER_DOWN,
		POINTER_MOVE,
		POINTER_UP
	};

	/**
	 * Platform-agnostic pointer event (touch on Android, mouse on desktop).
	 */
	struct InputEvent {
		InputEventType type;
		float x;
		float y;
		int32_t pointerId;
	};
}

#endif // EY3INPUTEVENT_H
