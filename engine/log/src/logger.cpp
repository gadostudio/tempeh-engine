#include <spdlog/spdlog.h>
#include <spdlog/async_logger.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <memory>
#include <spdlog/async.h>

#include <tempeh/logger.hpp>

namespace Tempeh::Log
{

//#ifdef TEMPEH_EDITOR
	template <typename M>
	void EditorSink<M>::sink_it_(const spdlog::details::log_msg& msg)
	{
		spdlog::memory_buf_t formatted;
		spdlog::sinks::base_sink<M>::formatter_->format(msg, formatted);
        std::string str(formatted.data(), formatted.data() + formatted.size());
        m_log_messages->push_back(str);
	}
//#endif

	void Logger::init(const char* logger_name)
	{
        spdlog::set_error_handler([](const std::string& error_message){
            LOG_ERROR(error_message);
            // TODO
            // Do not break the engine
            assert(false && "Logger error");
        });
		sinks.emplace_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>());
//#ifdef TEMPEH_EDITOR
		sinks.emplace_back(std::make_shared<EditorSink_MT>(&capped_log_messages));
//#endif

		logger = std::make_shared<spdlog::logger>(logger_name, sinks.begin(), sinks.end());
		spdlog::register_logger(logger);
	}

}
