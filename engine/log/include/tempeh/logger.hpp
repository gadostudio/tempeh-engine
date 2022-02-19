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
	public:
		static void init(const char* logger_name);
		static std::shared_ptr<spdlog::logger>& get_logger() { return logger; }

		template<typename... Args>
		inline static void trace(Args&&... args)
		{
			if (logger) {
				logger->trace(std::forward<Args>(args)...);
			}
		}

		template<typename... Args>
		inline static void info(Args&&... args)
		{
			if (logger) {
				logger->info(std::forward<Args>(args)...);
			}
		}

		template<typename... Args>
		inline static void warn(Args&&... args)
		{
			if (logger) {
				logger->warn(std::forward<Args>(args)...);
			}
		}

		template<typename... Args>
		inline static void error(Args&&... args)
		{
			if (logger) {
				logger->error(std::forward<Args>(args)...);
			}
		}

		template<typename... Args>
		inline static void critical(Args&&... args)
		{
			if (logger) {
				logger->critical(std::forward<Args>(args)...);
			}
		}

	protected:
		inline static std::vector<spdlog::sink_ptr> sinks;
		inline static std::shared_ptr<spdlog::logger> logger;
	};
}

#define LOG_TRACE(...)    ::Tempeh::Log::Logger::trace(__VA_ARGS__)
#define LOG_INFO(...)     ::Tempeh::Log::Logger::info(__VA_ARGS__)
#define LOG_WARN(...)     ::Tempeh::Log::Logger::warn(__VA_ARGS__)
#define LOG_ERROR(...)    ::Tempeh::Log::Logger::error(__VA_ARGS__)
#define LOG_CRITICAL(...) ::Tempeh::Log::Logger::critical(__VA_ARGS__)

#endif