#ifndef EY3CMDLISTENER_H 
#define EY3CMDLISTENER_H 

#include <android_native_app_glue.h>

namespace ey3 {

	/**
	 * Interface that must implement system-comands listeners
	 * They should be added to the CmdHandler for event handling
	 */
	class CmdListener {
		public:
			virtual void handleCmd(int32_t cmd, android_app* app) = 0;
	};
}

#endif // EY3CMDLISTENER_H 
