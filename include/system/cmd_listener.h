#ifndef EY3CMDLISTENER_H
#define EY3CMDLISTENER_H

#include <cstdint>

namespace ey3 {

	class IWindow;

	/**
	 * Interface that must implement system-comands listeners
	 * They should be added to the CmdHandler for event handling
	 */
	class CmdListener {
		public:
			virtual void handleCmd(int32_t cmd, IWindow* window) = 0;
	};
}

#endif // EY3CMDLISTENER_H
