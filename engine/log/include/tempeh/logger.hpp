#ifndef _TEMPEH_LOGGER
#define _TEMPEH_LOGGER

#include <vector>
#include <memory>
#include <spdlog/async_logger.h>
#include <spdlog/spdlog.h>

namespace Tempeh
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

#define LOG_TRACE(...)    ::Tempeh::Logger::get_logger()->trace(__VA_ARGS__)
#define LOG_INFO(...)     ::Tempeh::Logger::get_logger()->info(__VA_ARGS__)
#define LOG_WARN(...)     ::Tempeh::Logger::get_logger()->warn(__VA_ARGS__)
#define LOG_ERROR(...)    ::Tempeh::Logger::get_logger()->error(__VA_ARGS__)
#define LOG_CRITICAL(...) ::Tempeh::Logger::get_logger()->critical(__VA_ARGS__)

#endif
