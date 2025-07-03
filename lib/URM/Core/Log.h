#pragma once

#include <spdlog/spdlog.h>

namespace URM::Core {
	class Logger {
		static bool mLoggerInitialized;

	public:
		static void InitLogger();
		static void DisposeLogger();

		static std::shared_ptr<spdlog::logger> GetFatalLogger();
	};
}
