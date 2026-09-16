#include "cmd_handler.h"

using namespace ey3;

void CmdHandler::handleCmd(int32_t cmd, IWindow* window) {
	for (int i = 0; i < cmdListeners.size(); i++) {
		cmdListeners[i]->handleCmd(cmd, window);
	}
}

void CmdHandler::addListener(CmdListener* listener) {
	cmdListeners.push_back(listener);
}

void CmdHandler::removeListener(CmdListener* listener) {
	int i = 0;
	while (i < cmdListeners.size()) {
		if (cmdListeners[i] == listener) {
			cmdListeners.erase(cmdListeners.begin() + i);
		} else {
			i++;
		}
	}
}

