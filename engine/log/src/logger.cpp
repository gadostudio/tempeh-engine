#include <spdlog/spdlog.h>
#include <spdlog/async_logger.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <memory>
#include <spdlog/async.h>

#include <tempeh/logger.hpp>

namespace Tempeh::Log
{

	void Logger::init(const char* logger_name)
	{
		sinks.emplace_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>());
		logger = std::make_shared<spdlog::logger>(logger_name, sinks.begin(), sinks.end());

		spdlog::register_logger(logger);
	}

}
