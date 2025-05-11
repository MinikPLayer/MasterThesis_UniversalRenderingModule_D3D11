#pragma once

#include <spdlog/spdlog.h>

class Logger {
public:
	static void InitLogger();
	static void DisposeLogger();

	static std::shared_ptr<spdlog::logger> GetFatalLogger();
};

