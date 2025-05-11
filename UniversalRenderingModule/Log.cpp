#include "pch.h"
#include "Log.h"

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_sinks.h>
#include <spdlog/sinks/callback_sink.h>
#include <spdlog/sinks/msvc_sink.h>

#include <Windows.h>

void InitLogger() {
	auto msvcSink = std::make_shared<spdlog::sinks::msvc_sink_mt>();
	auto stdoutSink = std::make_shared<spdlog::sinks::stdout_sink_mt>();
	auto callbackSink = std::make_shared<spdlog::sinks::callback_sink_mt>(
		[](const spdlog::details::log_msg& msg) {
			std::wstring str(msg.payload.begin(), msg.payload.end());
			auto leftBracket = str.find(L"[");
			auto rightBracket = str.find(L"]");

			std::wstring header = L"Fatal error";
			if (leftBracket != std::wstring::npos && rightBracket != std::wstring::npos) {
				header = str.substr(leftBracket + 1, rightBracket - leftBracket - 1);
				str = str.substr(rightBracket + 2); // +2 to skip the ] and the space
			}

			str += L"\n\nApplication will now close.";
			MessageBox(nullptr, str.c_str(), header.c_str(), MB_OK | MB_ICONERROR);
			exit(1);
		}
	);

	callbackSink->set_level(spdlog::level::critical);

	auto finalLogger = std::make_shared<spdlog::logger>(spdlog::logger("logger", { msvcSink, stdoutSink, callbackSink }));

#if !NDEBUG
	finalLogger->set_level(spdlog::level::trace);
#else
	finalLogger->set_level(spdlog::level::info);
#endif

	spdlog::set_default_logger(finalLogger);
	spdlog::set_pattern("%Y-%m-%d %H:%M:%S.%e %l : %v");
	spdlog::flush_on(spdlog::level::info);
}

void DisposeLogger()
{
}
