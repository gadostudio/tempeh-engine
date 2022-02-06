#ifndef _TEMPEH_LOGGER_HPP
#define _TEMPEH_LOGGER_HPP

#include <vector>
#include <memory>
#include <spdlog/async_logger.h>
#include <spdlog/spdlog.h>

namespace Tempeh::Log
{
	class Logger
	{
	protected:
		inline static std::vector<spdlog::sink_ptr> sinks;
		inline static std::shared_ptr<spdlog::logger> logger;
	public:
		static void init(const char* logger_name);
		static std::shared_ptr<spdlog::logger>& get_logger() { return logger; }
	};
}

#define LOG_TRACE(...)    ::Tempeh::Log::Logger::get_logger()->trace(__VA_ARGS__)
#define LOG_INFO(...)     ::Tempeh::Log::Logger::get_logger()->info(__VA_ARGS__)
#define LOG_WARN(...)     ::Tempeh::Log::Logger::get_logger()->warn(__VA_ARGS__)
#define LOG_ERROR(...)    ::Tempeh::Log::Logger::get_logger()->error(__VA_ARGS__)
#define LOG_CRITICAL(...) ::Tempeh::Log::Logger::get_logger()->critical(__VA_ARGS__)

#endif _TEMPEH_LOGGER_HPP