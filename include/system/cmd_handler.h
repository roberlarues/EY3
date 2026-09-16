#ifndef EY3CMDHANDLER_H
#define EY3CMDHANDLER_H

#include <vector>
#include "cmd_listener.h"

namespace ey3 {

	/**
	 * Handles system commands (mostly life-cycle events).
	 */
	class CmdHandler: public CmdListener {
		private:
			std::vector<CmdListener*> cmdListeners;
		public:
			void handleCmd(int32_t cmd, IWindow* window);
			void addListener(CmdListener* listener);
			void removeListener(CmdListener* listener);
	};
}

#endif // EY3CMDHANDLER_H
