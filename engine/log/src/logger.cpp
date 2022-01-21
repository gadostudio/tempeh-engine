#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <memory>

#include <tempeh/logger.hpp>

namespace Tempeh::Log
{

	void Logger::init()
	{
		sinks.emplace_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>());
	}

	inline std::shared_ptr<spdlog::logger>& Logger::get_logger()
	{
		return logger;
	}

}
